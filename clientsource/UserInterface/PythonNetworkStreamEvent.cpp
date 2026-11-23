#include "StdAfx.h"
#include "PythonNetworkStream.h"
#ifdef __ANDROID__
#include <android/log.h>
#endif

void CPythonNetworkStream::OnRemoteDisconnect()
{
#ifdef __ANDROID__
	__android_log_print(ANDROID_LOG_ERROR, "Lupus2Net", "ON_REMOTE_DISCONNECT phase=%s online=%d", m_strPhase.c_str(), IsOnline() ? 1 : 0);
#endif
	PyCallClassMemberFunc(m_poHandler, "SetLoginPhase", Py_BuildValue("()"));
}

void CPythonNetworkStream::OnDisconnect()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Main Game
void CPythonNetworkStream::OnScriptEventStart(int iSkin, int iIndex)
{
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OpenQuestWindow", Py_BuildValue("(ii)", iSkin, iIndex));
}
