#include "StdAfx.h"
#include "GrpText.h"
#include "../eterBase/Stl.h"

#include "Util.h"

#ifdef __ANDROID__
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#include <android/log.h>
#include <stdio.h>
#include <algorithm>

namespace
{
	std::vector<unsigned char> gs_androidFontData;
	stbtt_fontinfo gs_androidFontInfo;
	bool gs_androidFontTried = false;
	bool gs_androidFontReady = false;

	bool EnsureAndroidFont()
	{
		if (gs_androidFontTried)
			return gs_androidFontReady;

		gs_androidFontTried = true;
		const char* candidates[] = {
			"/system/fonts/Roboto-Regular.ttf",
			"/system/fonts/NotoSans-Regular.ttf",
			"/system/fonts/DroidSans.ttf",
		};

		for (size_t i = 0; i < sizeof(candidates) / sizeof(candidates[0]); ++i)
		{
			FILE* file = fopen(candidates[i], "rb");
			if (!file)
				continue;

			fseek(file, 0, SEEK_END);
			const long size = ftell(file);
			fseek(file, 0, SEEK_SET);
			if (size <= 0)
			{
				fclose(file);
				continue;
			}

			gs_androidFontData.resize((size_t)size);
			const bool read = fread(&gs_androidFontData[0], 1, (size_t)size, file) == (size_t)size;
			fclose(file);
			if (!read)
				continue;

			const int offset = stbtt_GetFontOffsetForIndex(&gs_androidFontData[0], 0);
			if (offset >= 0 && stbtt_InitFont(&gs_androidFontInfo, &gs_androidFontData[0], offset))
			{
				gs_androidFontReady = true;
				__android_log_print(ANDROID_LOG_INFO, "Metin2Mobile", "Android font rasterizer ready: %s", candidates[i]);
				break;
			}
		}

		return gs_androidFontReady;
	}
}
#endif

CGraphicFontTexture::CGraphicFontTexture()
{
	Initialize();
}

CGraphicFontTexture::~CGraphicFontTexture()
{
	Destroy();
}

void CGraphicFontTexture::Initialize()
{
	CGraphicTexture::Initialize();
	m_hFontOld = NULL;
	m_hFont = NULL;
	m_isDirty = false;
	m_bItalic = false;
}

bool CGraphicFontTexture::IsEmpty() const
{
	return m_fontMap.size() == 0;
}

void CGraphicFontTexture::Destroy()
{
	HDC hDC = m_dib.GetDCHandle();
	if (hDC)
		SelectObject(hDC, m_hFontOld);

	m_dib.Destroy();

	m_lpd3dTexture = NULL;
	CGraphicTexture::Destroy();
	stl_wipe(m_pFontTextureVector);
	m_charInfoMap.clear();

	if (m_fontMap.size())
	{
		TFontMap::iterator i = m_fontMap.begin();

		while (i != m_fontMap.end())
		{
			DeleteObject((HGDIOBJ)i->second);
			++i;
		}

		m_fontMap.clear();
	}

	Initialize();
}

bool CGraphicFontTexture::CreateDeviceObjects()
{
	return true;
}

void CGraphicFontTexture::DestroyDeviceObjects()
{
}

bool CGraphicFontTexture::Create(const char* c_szFontName, int fontSize, bool bItalic)
{
	Destroy();

	strncpy(m_fontName, c_szFontName, sizeof(m_fontName) - 1);
	m_fontSize = fontSize;
	m_bItalic = bItalic;

	m_x = 0;
	m_y = 0;
	m_step = 0;

	DWORD width = 256, height = 256;
	if (GetMaxTextureWidth() > 512)
		width = 512;
	if (GetMaxTextureHeight() > 512)
		height = 512;

	if (!m_dib.Create(ms_hDC, width, height))
		return false;

	HDC hDC = m_dib.GetDCHandle();

	m_hFont = GetFont(GetDefaultCodePage());

	m_hFontOld = (HFONT)SelectObject(hDC, m_hFont);
	SetTextColor(hDC, RGB(255, 255, 255));
	SetBkColor(hDC, 0);

	if (!AppendTexture())
		return false;

	return true;
}

HFONT CGraphicFontTexture::GetFont(WORD codePage)
{
	HFONT hFont = NULL;
	TFontMap::iterator i = m_fontMap.find(codePage);

	if (i != m_fontMap.end())
	{
		hFont = i->second;
	}
	else
	{
		LOGFONT logFont;

		memset(&logFont, 0, sizeof(LOGFONT));

		logFont.lfHeight = m_fontSize;
		logFont.lfEscapement = 0;
		logFont.lfOrientation = 0;
		logFont.lfWeight = FW_NORMAL;
		logFont.lfItalic = (BYTE)m_bItalic;
		logFont.lfUnderline = FALSE;
		logFont.lfStrikeOut = FALSE;
		logFont.lfCharSet = GetCharsetFromCodePage(codePage);
		logFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
		logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		logFont.lfQuality = ANTIALIASED_QUALITY;
		logFont.lfPitchAndFamily = DEFAULT_PITCH;
		//Tracenf("font: %s", GetFontFaceFromCodePage(codePage));
		strcpy(logFont.lfFaceName, m_fontName); //GetFontFaceFromCodePage(codePage));
		//strcpy(logFont.lfFaceName, GetFontFaceFromCodePage(codePage));

		hFont = CreateFontIndirect(&logFont);

		m_fontMap.insert(TFontMap::value_type(codePage, hFont));
	}

	return hFont;
}

bool CGraphicFontTexture::AppendTexture()
{
	CGraphicImageTexture* pNewTexture = new CGraphicImageTexture;

	if (!pNewTexture->Create(m_dib.GetWidth(), m_dib.GetHeight(), D3DFMT_A4R4G4B4))
	{
		delete pNewTexture;
		return false;
	}

	m_pFontTextureVector.push_back(pNewTexture);
	return true;
}

bool CGraphicFontTexture::UpdateTexture()
{
	if (!m_isDirty)
		return true;

	m_isDirty = false;

	CGraphicImageTexture* pFontTexture = m_pFontTextureVector.back();

	if (!pFontTexture)
		return false;

	#ifdef __ANDROID__
	DWORD* pdwDst;
	#else
	WORD* pwDst;
	#endif
	int pitch;

	#ifdef __ANDROID__
	if (!pFontTexture->Lock(&pitch, (void**)&pdwDst))
	#else
	if (!pFontTexture->Lock(&pitch, (void**)&pwDst))
	#endif
		return false;

	#ifdef __ANDROID__
	pitch /= 4;
	#else
	pitch /= 2;
	#endif

	int width = m_dib.GetWidth();
	int height = m_dib.GetHeight();

	DWORD* pdwSrc = (DWORD*)m_dib.GetPointer();

	#ifdef __ANDROID__
	for (int y = 0; y < height; ++y, pdwDst += pitch, pdwSrc += width)
		for (int x = 0; x < width; ++x)
		{
			const DWORD alpha = pdwSrc[x] & 0xff;
			pdwDst[x] = (alpha << 24) | 0x00ffffff;
		}
	#else
	for (int y = 0; y < height; ++y, pwDst += pitch, pdwSrc += width)
		for (int x = 0; x < width; ++x)
			pwDst[x] = pdwSrc[x];
	#endif

	pFontTexture->Unlock();
	return true;
}

CGraphicFontTexture::TCharacterInfomation* CGraphicFontTexture::GetCharacterInfomation(WORD codePage, wchar_t keyValue)
{
	TCharacterKey code(codePage, keyValue);

	TCharacterInfomationMap::iterator f = m_charInfoMap.find(code);

	if (m_charInfoMap.end() == f)
	{
		return UpdateCharacterInfomation(code);
	}
	else
	{
		return &f->second;
	}
}

CGraphicFontTexture::TCharacterInfomation* CGraphicFontTexture::UpdateCharacterInfomation(TCharacterKey code)
{
	#ifdef __ANDROID__
	if (!EnsureAndroidFont())
		return NULL;

	wchar_t keyValue = code.second;
	if (keyValue == 0x08)
		keyValue = L' ';

	// The game keeps the PC UI in a 640px-high logical coordinate system, while
	// the Android surface is normally 1080px high.  Rasterizing at the logical
	// size and then enlarging the atlas made every label soft and pixelated.
	// Build the glyph atlas at native surface density, but keep the original PC
	// measurements below so text layout and window sizes do not change.
	const float surfaceScale = std::max(1.0f,
		(float)AndroidGetSurfaceHeight() / (float)std::max(1, AndroidGetVirtualHeight()));
	const float logicalScale = stbtt_ScaleForPixelHeight(&gs_androidFontInfo, (float)m_fontSize);
	const float rasterScale = stbtt_ScaleForPixelHeight(&gs_androidFontInfo,
		(float)m_fontSize * surfaceScale);
	int ascent = 0, descent = 0, lineGap = 0;
	stbtt_GetFontVMetrics(&gs_androidFontInfo, &ascent, &descent, &lineGap);
	const int rasterBaseline = (int)ceilf(ascent * rasterScale);
	const int rasterLineHeight = std::max(1,
		(int)ceilf((ascent - descent + lineGap) * rasterScale));
	const int logicalLineHeight = std::max(1,
		(int)ceilf((ascent - descent + lineGap) * logicalScale));
	int advanceWidth = 0;
	int leftBearing = 0;
	stbtt_GetCodepointHMetrics(&gs_androidFontInfo, (int)keyValue, &advanceWidth, &leftBearing);

	int x0 = 0, y0 = 0, x1 = 0, y1 = 0;
	stbtt_GetCodepointBitmapBox(&gs_androidFontInfo, (int)keyValue,
		rasterScale, rasterScale, &x0, &y0, &x1, &y1);
	const int glyphWidth = x1 - x0;
	const int glyphHeight = y1 - y0;
	const int logicalAdvance = std::max(1, (int)ceilf(advanceWidth * logicalScale));
	const int rasterAdvance = std::max(1, (int)ceilf(advanceWidth * rasterScale));
	const int leftPadding = 1 + std::max(0, -x0);
	const int cellWidth = std::max(1, std::max(rasterAdvance + 2,
		leftPadding + glyphWidth + std::max(0, x0) + 1));
	const int cellHeight = rasterLineHeight + 2;
	const int logicalCellWidth = std::max(1, (int)ceilf((float)cellWidth / surfaceScale));
	const int logicalCellHeight = std::max(logicalLineHeight + 2,
		(int)ceilf((float)cellHeight / surfaceScale));

	const int width = m_dib.GetWidth();
	const int height = m_dib.GetHeight();
	if (m_x + cellWidth >= width - 1)
	{
		m_y += m_step + 1;
		m_step = 0;
		m_x = 0;
	}

	if (m_y + cellHeight >= height - 1)
	{
		if (!UpdateTexture() || !AppendTexture())
			return NULL;
		m_x = 0;
		m_y = 0;
		m_step = 0;
		memset(m_dib.GetPointer(), 0, width * height * sizeof(DWORD));
	}

	if (glyphWidth > 0 && glyphHeight > 0)
	{
		std::vector<unsigned char> bitmap((size_t)glyphWidth * glyphHeight);
		stbtt_MakeCodepointBitmap(&gs_androidFontInfo, &bitmap[0], glyphWidth, glyphHeight,
			glyphWidth, rasterScale, rasterScale, (int)keyValue);

		// STB's y0 is relative to the baseline.  Align every glyph to one
		// common baseline; aligning bitmap tops made letters jump up/down.
		const int drawX = m_x + leftPadding + x0;
		const int drawY = m_y + 1 + rasterBaseline + y0;
		DWORD* pixels = (DWORD*)m_dib.GetPointer();
		for (int y = 0; y < glyphHeight && drawY + y < height; ++y)
		{
			for (int x = 0; x < glyphWidth && drawX + x < width; ++x)
			{
				DWORD& destination = pixels[(drawY + y) * width + drawX + x];
				const DWORD coverage = bitmap[(size_t)y * glyphWidth + x];
				if (coverage > (destination & 0xff))
					destination = coverage;
			}
		}
	}

	const float rhwidth = 1.0f / (float)width;
	const float rhheight = 1.0f / (float)height;
	TCharacterInfomation& info = m_charInfoMap[code];
	info.index = (short)m_pFontTextureVector.size() - 1;
	info.width = (short)logicalCellWidth;
	info.height = (short)logicalCellHeight;
	info.left = (float)m_x * rhwidth;
	info.top = (float)m_y * rhheight;
	info.right = (float)(m_x + cellWidth) * rhwidth;
	info.bottom = (float)(m_y + cellHeight) * rhheight;
	info.advance = (float)logicalAdvance;

	m_x += cellWidth;
	if (m_step < cellHeight)
		m_step = cellHeight;
	m_isDirty = true;
	return &info;
	#else
	HDC hDC = m_dib.GetDCHandle();
	SelectObject(hDC, GetFont(code.first));

	wchar_t keyValue = code.second;

	if (keyValue == 0x08)
		keyValue = L' '; // ���� �������� �ٲ۴� (�ƶ� ��½� �� ���: NAME:\tTEXT -> TEXT\t:NAME �� ��ȯ�� )

	ABCFLOAT	stABC;
	SIZE		size;

	if (!GetTextExtentPoint32W(hDC, &keyValue, 1, &size) || !GetCharABCWidthsFloatW(hDC, keyValue, keyValue, &stABC))
		return NULL;

	size.cx = stABC.abcfB;
	if (stABC.abcfA > 0.0f)
		size.cx += ceilf(stABC.abcfA);
	if (stABC.abcfC > 0.0f)
		size.cx += ceilf(stABC.abcfC);
	size.cx++;

	LONG lAdvance = ceilf(stABC.abcfA + stABC.abcfB + stABC.abcfC);

	int width = m_dib.GetWidth();
	int height = m_dib.GetHeight();

	if (m_x + size.cx >= (width - 1))
	{
		m_y += (m_step + 1);
		m_step = 0;
		m_x = 0;

		if (m_y + size.cy >= (height - 1))
		{
			if (!UpdateTexture())
			{
				return NULL;
			}

			if (!AppendTexture())
				return NULL;

			m_y = 0;
		}
	}

	TextOutW(hDC, m_x, m_y, &keyValue, 1);

	int nChrX;
	int nChrY;
	int nChrWidth = size.cx;
	int nChrHeight = size.cy;
	int nDIBWidth = m_dib.GetWidth();

	DWORD* pdwDIBData = (DWORD*)m_dib.GetPointer();
	DWORD* pdwDIBBase = pdwDIBData + nDIBWidth * m_y + m_x;
	DWORD* pdwDIBRow;

	pdwDIBRow = pdwDIBBase;
	for (nChrY = 0; nChrY < nChrHeight; ++nChrY, pdwDIBRow += nDIBWidth)
	{
		for (nChrX = 0; nChrX < nChrWidth; ++nChrX)
		{
			pdwDIBRow[nChrX] = (pdwDIBRow[nChrX] & 0xff) ? 0xffff : 0;
		}
	}

	float rhwidth = 1.0f / float(width);
	float rhheight = 1.0f / float(height);

	TCharacterInfomation& rNewCharInfo = m_charInfoMap[code];

	rNewCharInfo.index = (short)m_pFontTextureVector.size() - 1;
	rNewCharInfo.width = size.cx;
	rNewCharInfo.height = size.cy;
	rNewCharInfo.left = float(m_x) * rhwidth;
	rNewCharInfo.top = float(m_y) * rhheight;
	rNewCharInfo.right = float(m_x + size.cx) * rhwidth;
	rNewCharInfo.bottom = float(m_y + size.cy) * rhheight;
	rNewCharInfo.advance = (float)lAdvance;

	m_x += size.cx;

	if (m_step < size.cy)
		m_step = size.cy;

	m_isDirty = true;

	return &rNewCharInfo;
	#endif
}

bool CGraphicFontTexture::CheckTextureIndex(DWORD dwTexture)
{
	if (dwTexture >= m_pFontTextureVector.size())
		return false;

	return true;
}

void CGraphicFontTexture::SelectTexture(DWORD dwTexture)
{
	assert(CheckTextureIndex(dwTexture));
	m_lpd3dTexture = m_pFontTextureVector[dwTexture]->GetD3DTexture();
}
