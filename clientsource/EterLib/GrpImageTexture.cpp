#include "StdAfx.h"
#include "../eterBase/MappedFile.h"
#include "../eterPack/EterPackManager.h"
#include "GrpImageTexture.h"
#include "../eterImageLib/TGAImage.h"

#ifdef __ANDROID__
namespace
{
	bool LoadRawARGBDDS(UINT bufferSize, const void* buffer, UINT& width, UINT& height,
		UINT& bitsPerPixel, UINT& sourcePitch, DWORD& redMask, DWORD& greenMask,
		DWORD& blueMask, DWORD& alphaMask, const BYTE*& pixels)
	{
		if (!buffer || bufferSize < 128)
			return false;

		const BYTE* bytes = static_cast<const BYTE*>(buffer);
		if (memcmp(bytes, "DDS ", 4) != 0)
			return false;

		DWORD headerSize = 0;
		DWORD pixelFormatFlags = 0;
		DWORD fourCC = 0;
		DWORD headerFlags = 0;
		DWORD pitchOrLinearSize = 0;
		memcpy(&headerFlags, bytes + 8, sizeof(headerFlags));
		memcpy(&headerSize, bytes + 4, sizeof(headerSize));
		memcpy(&height, bytes + 12, sizeof(height));
		memcpy(&width, bytes + 16, sizeof(width));
		memcpy(&pitchOrLinearSize, bytes + 20, sizeof(pitchOrLinearSize));
		memcpy(&pixelFormatFlags, bytes + 80, sizeof(pixelFormatFlags));
		memcpy(&fourCC, bytes + 84, sizeof(fourCC));
		memcpy(&bitsPerPixel, bytes + 88, sizeof(bitsPerPixel));
		memcpy(&redMask, bytes + 92, sizeof(redMask));
		memcpy(&greenMask, bytes + 96, sizeof(greenMask));
		memcpy(&blueMask, bytes + 100, sizeof(blueMask));
		memcpy(&alphaMask, bytes + 104, sizeof(alphaMask));

		// The original PC effect set predominantly uses uncompressed A1R5G5B5
		// DDS files. D3DX expands those transparently on PC, while the Android
		// path previously accepted only 32-bit DDS and left the particle quad
		// without its texture. Accept the standard 16/24/32-bit RGB layouts and
		// expand them to A8R8G8B8 below.
		if (headerSize != 124 || width == 0 || height == 0 || fourCC != 0 ||
			(pixelFormatFlags & 0x40) == 0 ||
			(bitsPerPixel != 16 && bitsPerPixel != 24 && bitsPerPixel != 32))
			return false;

		const UINT packedPitch = (width * bitsPerPixel + 7u) / 8u;
		// DDSD_PITCH means this field is a row pitch. With DDSD_LINEARSIZE (as
		// used by the Metin2 A1R5G5B5 effects) it is the whole top-level image.
		sourcePitch = (headerFlags & 0x8u) && pitchOrLinearSize >= packedPitch
			? pitchOrLinearSize
			: packedPitch;
		const unsigned long long pixelBytes =
			static_cast<unsigned long long>(sourcePitch) * static_cast<unsigned long long>(height);
		if (pixelBytes > bufferSize - 128)
			return false;

		pixels = bytes + 128;
		return true;
	}

	BYTE ExtractDDSChannel(DWORD pixel, DWORD mask, BYTE fallback)
	{
		if (!mask)
			return fallback;

		unsigned shift = 0;
		while ((mask & 1u) == 0u)
		{
			mask >>= 1;
			++shift;
		}

		const DWORD value = (pixel >> shift) & mask;
		return static_cast<BYTE>((value * 255u + mask / 2u) / mask);
	}
}
#endif

bool CGraphicImageTexture::Lock(int* pRetPitch, void** ppRetPixels, int level)
{
	D3DLOCKED_RECT lockedRect;
	if (FAILED(m_lpd3dTexture->LockRect(level, &lockedRect, NULL, 0)))
		return false;

	*pRetPitch = lockedRect.Pitch;
	*ppRetPixels = (void*)lockedRect.pBits;
	return true;
}

void CGraphicImageTexture::Unlock(int level)
{
	assert(m_lpd3dTexture != NULL);
	m_lpd3dTexture->UnlockRect(level);
}

void CGraphicImageTexture::Initialize()
{
	CGraphicTexture::Initialize();

	m_stFileName = "";

	m_d3dFmt = D3DFMT_UNKNOWN;
	m_dwFilter = 0;
}

void CGraphicImageTexture::Destroy()
{
	CGraphicTexture::Destroy();

	Initialize();
}

bool CGraphicImageTexture::CreateDeviceObjects()
{
	assert(ms_lpd3dDevice != NULL);
	assert(m_lpd3dTexture == NULL);

	if (m_stFileName.empty())
	{
		// ��Ʈ �ؽ���
		if (FAILED(ms_lpd3dDevice->CreateTexture(m_width, m_height, 1, 0, m_d3dFmt, D3DPOOL_MANAGED, &m_lpd3dTexture)))
			return false;
	}
	else
	{
		CMappedFile	mappedFile;
		LPCVOID		c_pvMap;

		if (!CEterPackManager::Instance().Get(mappedFile, m_stFileName.c_str(), &c_pvMap))
			return false;

		//@fixme002
		if (!CreateFromMemoryFile(mappedFile.Size(), c_pvMap, m_d3dFmt, m_dwFilter))
		{
			TraceError("CGraphicImageTexture::CreateDeviceObjects: CreateFromMemoryFile: texture not found(%s)", m_stFileName.c_str());
			return false;
		}
		return true;
		// return CreateFromMemoryFile(mappedFile.Size(), c_pvMap, m_d3dFmt, m_dwFilter);
	}

	m_bEmpty = false;
	return true;
}

bool CGraphicImageTexture::Create(UINT width, UINT height, D3DFORMAT d3dFmt, DWORD dwFilter)
{
	assert(ms_lpd3dDevice != NULL);
	Destroy();

	m_width = width;
	m_height = height;
	m_d3dFmt = d3dFmt;
	m_dwFilter = dwFilter;

	return CreateDeviceObjects();
}

void CGraphicImageTexture::CreateFromTexturePointer(const CGraphicTexture* c_pSrcTexture)
{
	if (m_lpd3dTexture)
		m_lpd3dTexture->Release();

	m_width = c_pSrcTexture->GetWidth();
	m_height = c_pSrcTexture->GetHeight();
	m_lpd3dTexture = c_pSrcTexture->GetD3DTexture();

	if (m_lpd3dTexture)
		m_lpd3dTexture->AddRef();

	m_bEmpty = false;
}

bool CGraphicImageTexture::CreateDDSTexture(CDXTCImage& image, const BYTE* /*c_pbBuf*/)
{
	int mipmapCount = image.m_dwMipMapCount == 0 ? 1 : image.m_dwMipMapCount;

	D3DFORMAT format;
	LPDIRECT3DTEXTURE8 lpd3dTexture;
	D3DPOOL pool = ms_bSupportDXT ? D3DPOOL_MANAGED : D3DPOOL_SCRATCH;;

	if (image.m_CompFormat == PF_DXT5)
		format = D3DFMT_DXT5;
	else if (image.m_CompFormat == PF_DXT3)
		format = D3DFMT_DXT3;
	else
		format = D3DFMT_DXT1;

	UINT uTexBias = 0;
	if (IsLowTextureMemory())
		uTexBias = 1;

	UINT uMinMipMapIndex = 0;
	if (uTexBias > 0)
	{
		if (mipmapCount > uTexBias)
		{
			uMinMipMapIndex = uTexBias;
			image.m_nWidth >>= uTexBias;
			image.m_nHeight >>= uTexBias;
			mipmapCount -= uTexBias;
		}
	}

	if (FAILED(D3DXCreateTexture(ms_lpd3dDevice, image.m_nWidth, image.m_nHeight,
		mipmapCount, 0, format, pool, &lpd3dTexture)))
	{
		TraceError("CreateDDSTexture: Cannot creatre texture");
		return false;
	}

	for (DWORD i = 0; i < mipmapCount; ++i)
	{
		D3DLOCKED_RECT lockedRect;

		if (FAILED(lpd3dTexture->LockRect(i, &lockedRect, NULL, 0)))
		{
			TraceError("CreateDDSTexture: Cannot lock texture");
		}
		else
		{
			image.Copy(i + uMinMipMapIndex, (BYTE*)lockedRect.pBits, lockedRect.Pitch);
			lpd3dTexture->UnlockRect(i);
		}
	}

	if (ms_bSupportDXT)
	{
		m_lpd3dTexture = lpd3dTexture;
	}
	else
	{
		if (image.m_CompFormat == PF_DXT3 || image.m_CompFormat == PF_DXT5)
			format = D3DFMT_A4R4G4B4;
		else
			format = D3DFMT_A1R5G5B5;

		UINT imgWidth = image.m_nWidth;
		UINT imgHeight = image.m_nHeight;

		extern bool GRAPHICS_CAPS_HALF_SIZE_IMAGE;

		if (GRAPHICS_CAPS_HALF_SIZE_IMAGE && uTexBias > 0 && mipmapCount == 0)
		{
			imgWidth >>= uTexBias;
			imgHeight >>= uTexBias;
		}

		if (FAILED(D3DXCreateTexture(ms_lpd3dDevice, imgWidth, imgHeight,
			mipmapCount, 0, format, D3DPOOL_MANAGED, &m_lpd3dTexture)))
		{
			TraceError("CreateDDSTexture: Cannot creatre texture");
			return false;
		}

		IDirect3DTexture8* pkTexSrc = lpd3dTexture;
		IDirect3DTexture8* pkTexDst = m_lpd3dTexture;

		for (int i = 0; i < mipmapCount; ++i) {
			IDirect3DSurface8* ppsSrc = NULL;
			IDirect3DSurface8* ppsDst = NULL;

			if (SUCCEEDED(pkTexSrc->GetSurfaceLevel(i, &ppsSrc)))
			{
				if (SUCCEEDED(pkTexDst->GetSurfaceLevel(i, &ppsDst)))
				{
					D3DXLoadSurfaceFromSurface(ppsDst, NULL, NULL, ppsSrc, NULL, NULL, D3DX_FILTER_NONE, 0);
					ppsDst->Release();
				}
				ppsSrc->Release();
			}
		}

		lpd3dTexture->Release();
	}

	m_width = image.m_nWidth;
	m_height = image.m_nHeight;
	m_bEmpty = false;

	return true;
}

bool CGraphicImageTexture::CreateFromMemoryFile(UINT bufSize, const void* c_pvBuf, D3DFORMAT d3dFmt, DWORD dwFilter)
{
	assert(ms_lpd3dDevice != NULL);
	assert(m_lpd3dTexture == NULL);

#ifdef __ANDROID__
	UINT rawWidth = 0;
	UINT rawHeight = 0;
	UINT rawBitsPerPixel = 0;
	UINT rawSourcePitch = 0;
	DWORD redMask = 0;
	DWORD greenMask = 0;
	DWORD blueMask = 0;
	DWORD alphaMask = 0;
	const BYTE* rawPixels = NULL;
	if (LoadRawARGBDDS(bufSize, c_pvBuf, rawWidth, rawHeight, rawBitsPerPixel, rawSourcePitch,
		redMask, greenMask, blueMask, alphaMask, rawPixels))
	{
		m_width = rawWidth;
		m_height = rawHeight;
		if (FAILED(ms_lpd3dDevice->CreateTexture(m_width, m_height, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_lpd3dTexture)))
			return false;

		D3DLOCKED_RECT lockedRect;
		if (FAILED(m_lpd3dTexture->LockRect(0, &lockedRect, NULL, 0)))
		{
			m_lpd3dTexture->Release();
			m_lpd3dTexture = NULL;
			return false;
		}

		for (UINT y = 0; y < m_height; ++y)
		{
			const BYTE* source = rawPixels + y * rawSourcePitch;
			DWORD* destination = reinterpret_cast<DWORD*>(static_cast<BYTE*>(lockedRect.pBits) + y * lockedRect.Pitch);
			for (UINT x = 0; x < m_width; ++x)
			{
				DWORD pixel = 0;
				if (rawBitsPerPixel == 16)
				{
					WORD value;
					memcpy(&value, source + x * 2, sizeof(value));
					pixel = value;
				}
				else if (rawBitsPerPixel == 24)
				{
					const BYTE* value = source + x * 3;
					pixel = DWORD(value[0]) | (DWORD(value[1]) << 8) | (DWORD(value[2]) << 16);
				}
				else
				{
					memcpy(&pixel, source + x * 4, sizeof(pixel));
				}
				const DWORD red = ExtractDDSChannel(pixel, redMask, 0);
				const DWORD green = ExtractDDSChannel(pixel, greenMask, 0);
				const DWORD blue = ExtractDDSChannel(pixel, blueMask, 0);
				const DWORD alpha = ExtractDDSChannel(pixel, alphaMask, 255);
				destination[x] = (alpha << 24) | (red << 16) | (green << 8) | blue;
			}
		}
		m_lpd3dTexture->UnlockRect(0);
		m_bEmpty = false;
		return true;
	}

	CDXTCImage image;
	if (image.LoadHeaderFromMemory((const BYTE*)c_pvBuf))
	{
		if (image.LoadFromMemory((const BYTE*)c_pvBuf))
		{
			m_width = image.m_nWidth;
			m_height = image.m_nHeight;

			if (FAILED(ms_lpd3dDevice->CreateTexture(m_width, m_height, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_lpd3dTexture)))
				return false;

			D3DLOCKED_RECT lockedRect;
			if (SUCCEEDED(m_lpd3dTexture->LockRect(0, &lockedRect, NULL, 0)))
			{
				image.Decompress(0, (DWORD*)lockedRect.pBits);
				m_lpd3dTexture->UnlockRect(0);
			}
			m_bEmpty = false;
			return true;
		}
	}

	CTGAImage tga;
	if (tga.LoadFromMemory(bufSize, (const BYTE*)c_pvBuf))
	{
		m_width = tga.GetWidth();
		m_height = tga.GetHeight();

		if (SUCCEEDED(ms_lpd3dDevice->CreateTexture(m_width, m_height, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_lpd3dTexture)))
		{
			D3DLOCKED_RECT lockedRect;
			if (SUCCEEDED(m_lpd3dTexture->LockRect(0, &lockedRect, NULL, 0)))
			{
				// Match the PC D3DX loader, which is called with 0xffff00ff as
				// its color key for every non-DDS texture.  A large part of the
				// original Metin2 effect set stores transparency as solid magenta
				// in 24-bit TGA/PNG files.  Copying those pixels unchanged on GLES
				// exposes the particle billboards as opaque pink rectangles.
				const DWORD* source = tga.GetBasePointer();
				for (UINT y = 0; y < m_height; ++y)
				{
					DWORD* destination = reinterpret_cast<DWORD*>(
						static_cast<BYTE*>(lockedRect.pBits) + y * lockedRect.Pitch);
					for (UINT x = 0; x < m_width; ++x)
					{
						const DWORD pixel = source[y * m_width + x];
						destination[x] = (pixel & 0x00ffffffu) == 0x00ff00ffu ? 0x00000000u : pixel;
					}
				}
				m_lpd3dTexture->UnlockRect(0);
			}
			m_bEmpty = false;
			return true;
		}
	}
	
	TraceError("CGraphicImageTexture::CreateFromMemoryFile: Android texture load failed (%s, %u bytes)", m_stFileName.c_str(), bufSize);
	return false;
#else

	static CDXTCImage image;

	if (image.LoadHeaderFromMemory((const BYTE*)c_pvBuf))	// DDS�ΰ� Ȯ��
	{
		return (CreateDDSTexture(image, (const BYTE*)c_pvBuf));
	}
	else
	{
		D3DXIMAGE_INFO imageInfo;
		if (FAILED(D3DXCreateTextureFromFileInMemoryEx(
			ms_lpd3dDevice,
			c_pvBuf,
			bufSize,
			D3DX_DEFAULT,
			D3DX_DEFAULT,
			D3DX_DEFAULT,
			0,
			d3dFmt,
			D3DPOOL_MANAGED,
			dwFilter,
			dwFilter,
			0xffff00ff,
			&imageInfo,
			NULL,
			&m_lpd3dTexture)))
		{
			TraceError("CreateFromMemoryFile: Cannot create texture");
			return false;
		}

		m_width = imageInfo.Width;
		m_height = imageInfo.Height;

		D3DFORMAT format = imageInfo.Format;
		switch (imageInfo.Format) {
		case D3DFMT_A8R8G8B8:
			format = D3DFMT_A4R4G4B4;
			break;

		case D3DFMT_X8R8G8B8:
		case D3DFMT_R8G8B8:
			format = D3DFMT_A1R5G5B5;
			break;
		}

		UINT uTexBias = 0;

		extern bool GRAPHICS_CAPS_HALF_SIZE_IMAGE;
		if (GRAPHICS_CAPS_HALF_SIZE_IMAGE)
			uTexBias = 1;

		if (IsLowTextureMemory())
			if (uTexBias || format != imageInfo.Format)
			{
				IDirect3DTexture8* pkTexSrc = m_lpd3dTexture;
				IDirect3DTexture8* pkTexDst;

				if (SUCCEEDED(D3DXCreateTexture(
					ms_lpd3dDevice,
					imageInfo.Width >> uTexBias,
					imageInfo.Height >> uTexBias,
					imageInfo.MipLevels,
					0,
					format,
					D3DPOOL_MANAGED,
					&pkTexDst)))
				{
					m_lpd3dTexture = pkTexDst;

					for (int i = 0; i < imageInfo.MipLevels; ++i) {
						IDirect3DSurface8* ppsSrc = NULL;
						IDirect3DSurface8* ppsDst = NULL;

						if (SUCCEEDED(pkTexSrc->GetSurfaceLevel(i, &ppsSrc)))
						{
							if (SUCCEEDED(pkTexDst->GetSurfaceLevel(i, &ppsDst)))
							{
								D3DXLoadSurfaceFromSurface(ppsDst, NULL, NULL, ppsSrc, NULL, NULL, D3DX_FILTER_LINEAR, 0);
								ppsDst->Release();
							}
							ppsSrc->Release();
						}
					}

					pkTexSrc->Release();
				}
			}
	}

	m_bEmpty = false;
	return true;
#endif
}

void CGraphicImageTexture::SetFileName(const char* c_szFileName)
{
	m_stFileName = c_szFileName;
}

bool CGraphicImageTexture::CreateFromDiskFile(const char* c_szFileName, D3DFORMAT d3dFmt, DWORD dwFilter)
{
	Destroy();

	SetFileName(c_szFileName);

	m_d3dFmt = d3dFmt;
	m_dwFilter = dwFilter;
	return CreateDeviceObjects();
}

CGraphicImageTexture::CGraphicImageTexture()
{
	Initialize();
}

CGraphicImageTexture::~CGraphicImageTexture()
{
	Destroy();
}
