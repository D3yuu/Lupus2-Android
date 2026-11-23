#include "StdAfx.h"

#if defined(__ANDROID__) || defined(__IOS__)
int AndroidGetSurfaceWidth();
int AndroidGetSurfaceHeight();
float AndroidGetInterfaceScale();
#endif
#include "PythonApplication.h"
#include "PythonPlayer.h"
#include "../eterlib/Camera.h"

#ifdef __IOS__
#define LUPUS2_PYAPP_EVENT_ANDROID_COMPAT 1
#define __ANDROID__ 1
#endif

#ifdef __ANDROID__
void CPythonApplication::AndroidTouchCameraDelta(float dx, float dy)
{
	CCamera* camera = CCameraManager::Instance().GetCurrentCamera();
	if (!camera || !m_pyBackground.IsMapReady())
		return;

	// Android supplies relative finger motion.  The desktop Drag() path expects
	// an OS cursor that is warped back to its anchor after every event; without
	// that warp it accumulates velocity and the camera runs away.
	dx = std::max(-32.0f, std::min(32.0f, dx));
	dy = std::max(-32.0f, std::min(32.0f, dy));
	camera->RotateEyeAroundTarget(dy * 0.10f, dx * 0.10f);
}

DWORD CPythonApplication::AndroidTouchActorTap(int x, int y, bool doubleTap)
{
	// Mouse movement updates the Python UI cursor immediately, while the 3D
	// picking ray is normally rebuilt later during RenderGame.  A touch-up can
	// arrive between those stages, so synchronise the ray and actor picker here.
	OnMouseMove(x, y);
	long centerX = 0;
	long centerY = 0;
	m_kWndMgr.GetMousePosition(centerX, centerY);
	const float interfaceScale = AndroidGetInterfaceScale();
	centerX = static_cast<long>(centerX * interfaceScale + 0.5f);
	centerY = static_cast<long>(centerY * interfaceScale + 0.5f);

	// A desktop cursor can target a few pixels precisely; a finger cannot.  Each
	// sample still uses Metin2's original bounding-box/sphere picker.  The ordered
	// samples merely enlarge the touch hit area without changing combat range,
	// target rules or NPC packets.
	static const int offsets[][2] =
	{
		{  0,   0 },
		{  0, -12 }, { -12,   0 }, { 12,   0 }, {  0,  12 },
		{ -12, -12 }, { 12, -12 }, { -12, 12 }, { 12, 12 },
	};

	for (size_t i = 0; i < sizeof(offsets) / sizeof(offsets[0]); ++i)
	{
		m_pyGraphic.SetCursorPosition(centerX + offsets[i][0], centerY + offsets[i][1]);
		m_kChrMgr.AndroidRefreshPick();
		DWORD pickedVID = 0;
		if (m_kChrMgr.OLD_GetPickedInstanceVID(&pickedVID) &&
			CPythonPlayer::Instance().AndroidHandleActorTap(pickedVID, doubleTap))
		{
			m_pyGraphic.SetCursorPosition(centerX, centerY);
			return pickedVID;
		}
	}

	m_pyGraphic.SetCursorPosition(centerX, centerY);
	return 0;
}

void CPythonApplication::AndroidStopMovement()
{
	CPythonPlayer::Instance().NEW_Stop();
}
#endif

void CPythonApplication::OnCameraUpdate()
{
	if (m_pyBackground.IsMapReady())
	{
		CCamera* pkCameraMgr = CCameraManager::Instance().GetCurrentCamera();
		if (pkCameraMgr)
			pkCameraMgr->Update();
	}
}

void CPythonApplication::OnUIUpdate()
{
	UI::CWindowManager& rkUIMgr = UI::CWindowManager::Instance();
	rkUIMgr.Update();
}

void CPythonApplication::OnUIRender()
{
	UI::CWindowManager& rkUIMgr = UI::CWindowManager::Instance();
	rkUIMgr.Render();
}

void CPythonApplication::OnSizeChange(int width, int height)
{
}

void CPythonApplication::OnMouseMiddleButtonDown(int x, int y)
{
	CCameraManager& rkCmrMgr = CCameraManager::Instance();
	CCamera* pkCmrCur = rkCmrMgr.GetCurrentCamera();
	if (pkCmrCur)
		pkCmrCur->BeginDrag(x, y);

	if (!m_pyBackground.IsMapReady())
		return;

	SetCursorNum(CAMERA_ROTATE);
	if (CURSOR_MODE_HARDWARE == GetCursorMode())
		SetCursorVisible(FALSE, true);
}

void CPythonApplication::OnMouseMiddleButtonUp(int x, int y)
{
	CCameraManager& rkCmrMgr = CCameraManager::Instance();
	CCamera* pkCmrCur = rkCmrMgr.GetCurrentCamera();
	if (pkCmrCur)
		pkCmrCur->EndDrag();

	if (!m_pyBackground.IsMapReady())
		return;

	SetCursorNum(NORMAL);
	if (CURSOR_MODE_HARDWARE == GetCursorMode())
		SetCursorVisible(TRUE);
}

void CPythonApplication::OnMouseWheel(int nLen)
{
	CCameraManager& rkCmrMgr = CCameraManager::Instance();
	CCamera* pkCmrCur = rkCmrMgr.GetCurrentCamera();
	if (pkCmrCur)
		pkCmrCur->Wheel(nLen);
}

void CPythonApplication::OnMouseMove(int x, int y)
{
	CCameraManager& rkCmrMgr = CCameraManager::Instance();
	CCamera* pkCmrCur = rkCmrMgr.GetCurrentCamera();

	POINT Point;
	if (pkCmrCur)
	{
		if (CPythonBackground::Instance().IsMapReady() && pkCmrCur->Drag(x, y, &Point))
		{
			x = Point.x;
			y = Point.y;
#ifndef __ANDROID__
			ClientToScreen(m_hWnd, &Point);

			// 2004.07.26.myevan.��ö��HackShield�� �浹
			SetCursorPos(Point.x, Point.y);
#endif
		}
	}

	UI::CWindowManager& rkWndMgr = UI::CWindowManager::Instance();

#ifdef __ANDROID__
	// Touch coordinates arrive in physical GLSurfaceView pixels. The desktop
	// configuration resolution is unrelated and would offset every hit-test.
	rkWndMgr.SetResolution(::AndroidGetSurfaceWidth(), ::AndroidGetSurfaceHeight());
#else
	RECT rcWnd;
	GetClientRect(&rcWnd);
	rkWndMgr.SetResolution(rcWnd.right - rcWnd.left, rcWnd.bottom - rcWnd.top);
#endif

	rkWndMgr.RunMouseMove(x, y);
}

void CPythonApplication::OnMouseLeftButtonDown(int x, int y)
{
	UI::CWindowManager& rkWndMgr = UI::CWindowManager::Instance();
	rkWndMgr.RunMouseLeftButtonDown(x, y);
}

void CPythonApplication::OnMouseLeftButtonUp(int x, int y)
{
	UI::CWindowManager& rkWndMgr = UI::CWindowManager::Instance();
	rkWndMgr.RunMouseLeftButtonUp(x, y);
}

void CPythonApplication::OnMouseLeftButtonDoubleClick(int x, int y)
{
	UI::CWindowManager& rkWndMgr = UI::CWindowManager::Instance();
	rkWndMgr.RunMouseLeftButtonDown(x, y);
	rkWndMgr.RunMouseLeftButtonDoubleClick(x, y);
}

void CPythonApplication::OnMouseRightButtonDown(int x, int y)
{
	UI::CWindowManager& rkWndMgr = UI::CWindowManager::Instance();
	rkWndMgr.RunMouseRightButtonDown(x, y);
}

void CPythonApplication::OnMouseRightButtonUp(int x, int y)
{
	UI::CWindowManager& rkWndMgr = UI::CWindowManager::Instance();
	rkWndMgr.RunMouseRightButtonUp(x, y);
}

#ifdef ENABLE_MOUSEWHEEL_EVENT
bool CPythonApplication::OnMouseWheelScroll(long x, long y, short wDelta)
{
	UI::CWindowManager& rkWndMgr = UI::CWindowManager::Instance();
	return rkWndMgr.RunMouseWheelScroll(x, y, wDelta);
}
#endif

void CPythonApplication::OnKeyDown(int iIndex)
{
	UI::CWindowManager& rkWndMgr = UI::CWindowManager::Instance();

	if (DIK_ESCAPE == iIndex)
	{
		rkWndMgr.RunPressEscapeKey();
	}

	rkWndMgr.RunKeyDown(iIndex);
}

void CPythonApplication::OnKeyUp(int iIndex)
{
	UI::CWindowManager& rkWndMgr = UI::CWindowManager::Instance();
	rkWndMgr.RunKeyUp(iIndex);
}

void CPythonApplication::RunIMEUpdate()
{
	UI::CWindowManager& rkWndMgr = UI::CWindowManager::Instance();
	rkWndMgr.RunIMEUpdate();
}
void CPythonApplication::RunIMETabEvent()
{
	UI::CWindowManager& rkWndMgr = UI::CWindowManager::Instance();
	rkWndMgr.RunIMETabEvent();
}
void CPythonApplication::RunIMEReturnEvent()
{
	UI::CWindowManager& rkWndMgr = UI::CWindowManager::Instance();
	rkWndMgr.RunIMEReturnEvent();
}
void CPythonApplication::OnIMEKeyDown(int iIndex)
{
	UI::CWindowManager& rkWndMgr = UI::CWindowManager::Instance();
	rkWndMgr.RunIMEKeyDown(iIndex);
}
/////////////////////////////

void CPythonApplication::RunIMEChangeCodePage()
{
	UI::CWindowManager& rkWndMgr = UI::CWindowManager::Instance();
	rkWndMgr.RunChangeCodePage();
}
void CPythonApplication::RunIMEOpenCandidateListEvent()
{
	UI::CWindowManager& rkWndMgr = UI::CWindowManager::Instance();
	rkWndMgr.RunOpenCandidate();
}
void CPythonApplication::RunIMECloseCandidateListEvent()
{
	UI::CWindowManager& rkWndMgr = UI::CWindowManager::Instance();
	rkWndMgr.RunCloseCandidate();
}
void CPythonApplication::RunIMEOpenReadingWndEvent()
{
	UI::CWindowManager& rkWndMgr = UI::CWindowManager::Instance();
	rkWndMgr.RunOpenReading();
}
void CPythonApplication::RunIMECloseReadingWndEvent()
{
	UI::CWindowManager& rkWndMgr = UI::CWindowManager::Instance();
	rkWndMgr.RunCloseReading();
}

/////////////////////////////
void CPythonApplication::RunPressExitKey()
{
	UI::CWindowManager& rkWndMgr = UI::CWindowManager::Instance();
	rkWndMgr.RunPressExitKey();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CPythonApplication::OnMouseUpdate()
{
#ifdef _DEBUG
	if (!m_poMouseHandler)
	{
		//assert(!" CPythonApplication::OnMouseUpdate - Mouse handler has not set!");
		return;
	}
#endif _DEBUG

	UI::CWindowManager& rkWndMgr = UI::CWindowManager::Instance();
	long lx, ly;
	rkWndMgr.GetMousePosition(lx, ly);
	PyCallClassMemberFunc(m_poMouseHandler, "Update", Py_BuildValue("(ii)", lx, ly));
}

void CPythonApplication::OnMouseRender()
{
#ifdef _DEBUG
	if (!m_poMouseHandler)
	{
		//assert(!" CPythonApplication::OnMouseRender - Mouse handler has not set!");
		return;
	}
#endif _DEBUG

	PyCallClassMemberFunc(m_poMouseHandler, "Render", Py_BuildValue("()"));
}

#ifdef LUPUS2_PYAPP_EVENT_ANDROID_COMPAT
#undef __ANDROID__
#undef LUPUS2_PYAPP_EVENT_ANDROID_COMPAT
#endif
