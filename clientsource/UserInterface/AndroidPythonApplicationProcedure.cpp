#include "StdAfx.h"
#include "PythonApplication.h"

static int gs_nMouseCaptureRef = 0;

void CPythonApplication::SafeSetCapture()
{
	++gs_nMouseCaptureRef;
}

void CPythonApplication::SafeReleaseCapture()
{
	if (gs_nMouseCaptureRef > 0)
		--gs_nMouseCaptureRef;
}

void CPythonApplication::__SetFullScreenWindow(HWND, DWORD, DWORD, DWORD)
{
}

void CPythonApplication::__MinimizeFullScreenWindow(HWND, DWORD, DWORD)
{
}

LRESULT CPythonApplication::WindowProcedure(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	return CMSApplication::WindowProcedure(hWnd, uiMsg, wParam, lParam);
}
