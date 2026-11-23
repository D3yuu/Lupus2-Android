#include "StdAfx.h"
#include "Timer.h"

#if !defined(__ANDROID__) && !defined(__IOS__)
static LARGE_INTEGER gs_liTickCountPerSec;
#endif
static DWORD gs_dwBaseTime = 0;
static DWORD gs_dwServerTime = 0;
static DWORD gs_dwClientTime = 0;
static DWORD gs_dwFrameTime = 0;

#if !defined(__ANDROID__) && !defined(__IOS__)
#include <mmsystem.h>
#else
#include <time.h>
#endif

#if !defined(__ANDROID__) && !defined(__IOS__)
#pragma comment(lib, "winmm.lib")
#endif

BOOL ELTimer_Init()
{
	/*
	gs_liTickCountPerSec.QuadPart=0;

	if (!QueryPerformanceFrequency(&gs_liTickCountPerSec))
		return 0;

	LARGE_INTEGER liTickCount;
	QueryPerformanceCounter(&liTickCount);
	gs_dwBaseTime= (liTickCount.QuadPart*1000  / gs_liTickCountPerSec.QuadPart);
	*/
	gs_dwBaseTime = timeGetTime();
	return 1;
}

DWORD ELTimer_GetMSec()
{
	//assert(gs_dwBaseTime!=0 && "ELTimer_Init �� ���� �����ϼ���");
	//LARGE_INTEGER liTickCount;
	//QueryPerformanceCounter(&liTickCount);
	return timeGetTime() - gs_dwBaseTime; //(liTickCount.QuadPart*1000  / gs_liTickCountPerSec.QuadPart)-gs_dwBaseTime;
}

VOID	ELTimer_SetServerMSec(DWORD dwServerTime)
{
	NANOBEGIN
		if (0 != dwServerTime) // nanomite�� ���� ���� if
		{
			gs_dwServerTime = dwServerTime;
			gs_dwClientTime = CTimer::instance().GetCurrentMillisecond();
		}
	NANOEND
}

DWORD	ELTimer_GetServerMSec()
{
	return CTimer::instance().GetCurrentMillisecond() - gs_dwClientTime + gs_dwServerTime;
	//return ELTimer_GetMSec() - gs_dwClientTime + gs_dwServerTime;
}

DWORD	ELTimer_GetFrameMSec()
{
	return gs_dwFrameTime;
}

DWORD	ELTimer_GetServerFrameMSec()
{
	return ELTimer_GetFrameMSec() - gs_dwClientTime + gs_dwServerTime;
}

VOID	ELTimer_SetFrameMSec()
{
	gs_dwFrameTime = ELTimer_GetMSec();
}

CTimer::CTimer()
{
	ELTimer_Init();

	NANOBEGIN
		if (this) // nanomite�� ���� ���� if
		{
			m_dwCurrentTime = 0;
			m_bUseRealTime = true;
			m_index = 0;

			m_dwElapsedTime = 0;
			m_dwLastRealTime = 0;

			m_fCurrentTime = 0.0f;
		}
	NANOEND
}

CTimer::~CTimer()
{
}

void CTimer::SetBaseTime()
{
	m_dwCurrentTime = 0;
}

void CTimer::Advance()
{
	if (!m_bUseRealTime)
	{
#if defined(__ANDROID__) || defined(__IOS__)
		// Keep the original custom-clock semantics used throughout the client,
		// but advance it by the actual mobile frame duration.  The desktop's
		// fixed 16/17 ms step runs the whole simulation at half speed on a
		// device rendering near 30 FPS.
		const DWORD now = ELTimer_GetMSec();
		DWORD elapsed = now - m_dwLastRealTime;
		m_dwLastRealTime = now;
		if (elapsed == 0)
			elapsed = 1;
		else if (elapsed > 100)
			elapsed = 100;

		m_dwElapsedTime = elapsed;
		m_dwCurrentTime += elapsed;
		m_fCurrentTime = m_dwCurrentTime / 1000.0f;
#else
		++m_index;

		if (m_index == 1)
			m_index = -1;

		m_dwCurrentTime += 16 + (m_index & 1);
		m_fCurrentTime = m_dwCurrentTime / 1000.0f;
#endif
	}
	else
	{
		DWORD currentTime = ELTimer_GetMSec();

		if (m_dwCurrentTime == 0)
			m_dwCurrentTime = currentTime;

		m_dwElapsedTime = currentTime - m_dwCurrentTime;
		m_dwCurrentTime = currentTime;
	}
}

void CTimer::Adjust(int iTimeGap)
{
	m_dwCurrentTime += iTimeGap;
}

float CTimer::GetCurrentSecond()
{
	if (m_bUseRealTime)
		return ELTimer_GetMSec() / 1000.0f;

	return m_fCurrentTime;
}

DWORD CTimer::GetCurrentMillisecond()
{
	if (m_bUseRealTime)
		return ELTimer_GetMSec();

	return m_dwCurrentTime;
}

float CTimer::GetElapsedSecond()
{
	return GetElapsedMilliecond() / 1000.0f;
}

DWORD CTimer::GetElapsedMilliecond()
{
#if !defined(__ANDROID__) && !defined(__IOS__)
	if (!m_bUseRealTime)
		return 16 + (m_index & 1);
#endif

	return m_dwElapsedTime;
}

void CTimer::UseCustomTime()
{
	m_bUseRealTime = false;
#if defined(__ANDROID__) || defined(__IOS__)
	m_dwLastRealTime = ELTimer_GetMSec();
#endif
}
