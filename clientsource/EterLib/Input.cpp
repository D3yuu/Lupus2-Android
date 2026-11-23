#include "StdAfx.h"
#include "Input.h"

LPDIRECTINPUT8			CInputDevice::ms_lpDI = NULL;
LPDIRECTINPUTDEVICE8	CInputKeyboard::ms_lpKeyboard = NULL;
bool					CInputKeyboard::ms_bPressedKey[256];
char					CInputKeyboard::ms_diks[256];

CInputDevice::CInputDevice()
{
}

CInputDevice::~CInputDevice()
{
#if !defined(__ANDROID__) && !defined(__IOS__)
	SAFE_RELEASE(ms_lpDI);
#else
	ms_lpDI = NULL;
#endif
}

HRESULT CInputDevice::CreateDevice(HWND /*hWnd*/)
{
#if defined(__ANDROID__) || defined(__IOS__)
	// Touch/key events are injected by the native mobile host.
	return S_OK;
#else
	if (ms_lpDI)
	{
		ms_lpDI->AddRef();
		return S_OK;
	}

	HRESULT hr;

	// Create a DInput object
	if (FAILED(hr = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION,
		IID_IDirectInput8, (VOID**)&ms_lpDI, NULL)))
		return hr;

	return S_OK;
#endif
}

CInputKeyboard::CInputKeyboard()
{
	ResetKeyboard();
}

CInputKeyboard::~CInputKeyboard()
{
#if !defined(__ANDROID__) && !defined(__IOS__)
	if (ms_lpKeyboard)
		ms_lpKeyboard->Unacquire();

	SAFE_RELEASE(ms_lpKeyboard);
#else
	ms_lpKeyboard = NULL;
#endif
}

void CInputKeyboard::ResetKeyboard()
{
	memset(ms_diks, 0, sizeof(ms_diks));
	memset(ms_bPressedKey, 0, sizeof(ms_bPressedKey));
}

bool CInputKeyboard::InitializeKeyboard(HWND hWnd)
{
#if defined(__ANDROID__) || defined(__IOS__)
	ResetKeyboard();
	return true;
#else
	NANOBEGIN

		if (ms_lpKeyboard)
			return true;

	if (FAILED(CreateDevice(hWnd)))
		return false;

	HRESULT hr;

	// Obtain an interface to the system keyboard device.
	if (FAILED(hr = ms_lpDI->CreateDevice(GUID_SysKeyboard, &ms_lpKeyboard, NULL)))
		return false;

	if (FAILED(hr = ms_lpKeyboard->SetDataFormat(&c_dfDIKeyboard)))
		return false;

	// Alt + F4�� ���� ���� ���� - [levites]
	//	DWORD dwCoopFlags = DISCL_FOREGROUND | DISCL_EXCLUSIVE;
	//	DWORD dwCoopFlags = DISCL_NONEXCLUSIVE | DISCL_BACKGROUND;
	DWORD dwCoopFlags = DISCL_FOREGROUND | DISCL_NONEXCLUSIVE;

	if (FAILED(hr = ms_lpKeyboard->SetCooperativeLevel(hWnd, dwCoopFlags)))
		return false;

	ms_lpKeyboard->Acquire();

	NANOEND
		return true;
#endif
}

void CInputKeyboard::UpdateKeyboard()
{
#if defined(__ANDROID__) || defined(__IOS__)
	return;
#else
	if (!ms_lpKeyboard)
		return;

	HRESULT hr;

	hr = ms_lpKeyboard->GetDeviceState(sizeof(ms_diks), &ms_diks);

	if (FAILED(hr))
	{
		hr = ms_lpKeyboard->Acquire();

		// ���� ���ø����̼��� ��Ȱ��ȭ �Ǿ� �־� �Է��� ���� �� ����.
		//if (hr == DIERR_OTHERAPPHASPRIO || hr == DIERR_NOTACQUIRED);
		return;
	}

	for (int i = 0; i < 256; ++i)
	{
		if (ms_diks[i] & 0x80)
		{
			if (!IsPressed(i))
				KeyDown(i);
		}
		else if (IsPressed(i))
			KeyUp(i);
	}
#endif
}

void CInputKeyboard::KeyDown(int iIndex)
{
	ms_bPressedKey[iIndex] = true;
	OnKeyDown(iIndex);
}

void CInputKeyboard::KeyUp(int iIndex)
{
	ms_bPressedKey[iIndex] = false;
	OnKeyUp(iIndex);
}

bool CInputKeyboard::IsPressed(int iIndex)
{
	return ms_bPressedKey[iIndex];
}
