#include "StdAfx.h"
#include "MsApplication.h"

CMSApplication::CMSApplication()
{
}

CMSApplication::~CMSApplication()
{
	//	for (TWindowClassSet::iterator i=ms_stWCSet.begin(); i!=ms_stWCSet.end(); ++i)
	//		UnregisterClass(*i, ms_hInstance);
}

void CMSApplication::Initialize(HINSTANCE hInstance)
{
	ms_hInstance = hInstance;
}

void CMSApplication::MessageLoop()
{
	while (MessageProcess());
}

bool CMSApplication::IsMessage()
{
#ifdef __ANDROID__
	return false;
#else
	MSG msg;

	if (!PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		return false;

	return true;
#endif
}

bool CMSApplication::MessageProcess()
{
#ifdef __ANDROID__
	return false;
#else
	MSG msg;

	if (!GetMessage(&msg, NULL, 0, 0))
		return false;

	TranslateMessage(&msg);
	DispatchMessage(&msg);
	return true;
#endif
}

LRESULT CMSApplication::WindowProcedure(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
#ifndef __ANDROID__
	switch (uiMsg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	}
#endif

	return CMSWindow::WindowProcedure(hWnd, uiMsg, wParam, lParam);
}
