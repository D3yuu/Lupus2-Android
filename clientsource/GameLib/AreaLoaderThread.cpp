// AreaLoaderThread.cpp: implementation of the CAreaLoaderThread class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "../eterLib/ResourceManager.h"

#include "AreaLoaderThread.h"
#include "AreaTerrain.h"
#include "MapOutdoor.h"

#if defined(__ANDROID__) || defined(__IOS__)
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>

struct AndroidAreaThreadHandle { pthread_t thread; };
struct AndroidAreaSemaphoreHandle { sem_t semaphore; };
#ifdef __IOS__
// GLES resource creation must stay on the GL-owning thread on iOS too.
#define __ANDROID__ 1
#endif
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TEMP_CAreaLoaderThread::TEMP_CAreaLoaderThread()
	: m_bShutdowned(false), m_pArg(NULL), m_hThread(NULL), m_uThreadID(0),
	  m_hSemaphore(NULL), m_iRestSemCount(0)
{
}

TEMP_CAreaLoaderThread::~TEMP_CAreaLoaderThread()
{
	Shutdown();
	Destroy();
}

bool TEMP_CAreaLoaderThread::Create(void* arg)
{
	Arg(arg);
#ifdef __ANDROID__
	// CTerrain::CalculateTerrainPatch(), CArea::Load() and several resource
	// loaders create/upload GLES buffers and textures.  An EGL context is only
	// current on GLSurfaceView's render thread; doing that work from this legacy
	// loader thread corrupts GPU state on Android (delayed flicker/frozen map).
	// Android requests are therefore serviced synchronously by Request(), on
	// the render thread which owns the current EGL context.
	m_hThread = NULL;
	return true;
#else
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, EntryPoint, this, 0, &m_uThreadID);

	if (!m_hThread)
		return false;

	SetThreadPriority(m_hThread, THREAD_PRIORITY_NORMAL);
#endif
	return true;
}

UINT TEMP_CAreaLoaderThread::Run(void* arg)
{
	if (!Setup())
		return 0;

	return (Execute(arg));
}

/* Static */
UINT CALLBACK TEMP_CAreaLoaderThread::EntryPoint(void* pThis)
{
	TEMP_CAreaLoaderThread* pThread = (TEMP_CAreaLoaderThread*)pThis;
	return pThread->Run(pThread->Arg());
}

//////////////////////////////////////////////////////////////////////////
void TEMP_CAreaLoaderThread::Destroy()
{
	if (m_hSemaphore)
	{
#ifdef __ANDROID__
		AndroidAreaSemaphoreHandle* semaphoreHandle = static_cast<AndroidAreaSemaphoreHandle*>(m_hSemaphore);
		sem_destroy(&semaphoreHandle->semaphore);
		delete semaphoreHandle;
#else
		CloseHandle(m_hSemaphore);
#endif
		m_hSemaphore = NULL;
	}

	/*
		while(!m_pTerrainRequestDeque.empty())
		{
			CTerrain * pTerrain = m_pTerrainRequestDeque.front();
			delete pTerrain;
			pTerrain = NULL;
			m_pTerrainRequestDeque.pop_front();
		}

		while(!m_pTerrainCompleteDeque.empty())
		{
			CTerrain * pTerrain = m_pTerrainCompleteDeque.front();
			delete pTerrain;
			pTerrain = NULL;
			m_pTerrainCompleteDeque.pop_front();
		}
	*/

	/*stl_wipe(m_pTerrainRequestDeque);
	stl_wipe(m_pTerrainCompleteDeque);
	stl_wipe(m_pAreaRequestDeque);
	stl_wipe(m_pAreaCompleteDeque);*/
}

UINT TEMP_CAreaLoaderThread::Setup()
{
#ifdef __ANDROID__
	AndroidAreaSemaphoreHandle* semaphoreHandle = new AndroidAreaSemaphoreHandle;
	if (sem_init(&semaphoreHandle->semaphore, 0, 0) != 0)
	{
		delete semaphoreHandle;
		return 0;
	}
	m_hSemaphore = static_cast<HANDLE>(semaphoreHandle);
#else
	m_hSemaphore = CreateSemaphore(NULL,		// no security attributes
		0,			// initial count
		65535,		// maximum count
		NULL);		// unnamed semaphore
	if (!m_hSemaphore)
		return 0;
#endif

	return 1;
}

void TEMP_CAreaLoaderThread::Shutdown()
{
	if (!m_hThread)
		return;

#ifndef __ANDROID__
	BOOL bRet;
#endif

	m_bShutdowned = true;

#ifdef __ANDROID__
	if (m_hSemaphore)
		sem_post(&static_cast<AndroidAreaSemaphoreHandle*>(m_hSemaphore)->semaphore);
	AndroidAreaThreadHandle* threadHandle = static_cast<AndroidAreaThreadHandle*>(m_hThread);
	if (!pthread_equal(pthread_self(), threadHandle->thread))
		pthread_join(threadHandle->thread, NULL);
	delete threadHandle;
	m_hThread = NULL;
#else
	do
	{
		bRet = ReleaseSemaphore(m_hSemaphore, 1, NULL);
	}
	while (!bRet);

	WaitForSingleObject(m_hThread, 10000);	// �����尡 ���� �Ǳ⸦ 10�� ��ٸ�
#endif
}

UINT TEMP_CAreaLoaderThread::Execute(void* pvArg)
{
	bool bProcessTerrain = true;
	while (!m_bShutdowned)
	{
		DWORD dwWaitResult;

#ifdef __ANDROID__
		AndroidAreaSemaphoreHandle* semaphoreHandle = static_cast<AndroidAreaSemaphoreHandle*>(m_hSemaphore);
		int waitResult;
		do { waitResult = sem_wait(&semaphoreHandle->semaphore); } while (waitResult != 0 && errno == EINTR);
		dwWaitResult = (waitResult == 0) ? 0u : 1u;
#else
		dwWaitResult = WaitForSingleObject(m_hSemaphore, INFINITE);
#endif

		if (m_bShutdowned)
			break;

		switch (dwWaitResult)
		{
		case 0:
			if (bProcessTerrain)
				ProcessTerrain();
			else
				ProcessArea();
			break;
#ifndef __ANDROID__
		case WAIT_TIMEOUT:
			TraceError("TEMP_CAreaLoaderThread::Execute: Timeout occured while time-out interval is INIFITE");
			break;
#endif
		}
	}

#ifndef __ANDROID__
	Destroy();
#endif
	return 1;
}

void TEMP_CAreaLoaderThread::Request(CTerrain* pTerrain)	// called in main thread
{
	m_TerrainRequestMutex.Lock();
	m_pTerrainRequestDeque.push_back(pTerrain);
	m_TerrainRequestMutex.Unlock();

#ifdef __ANDROID__
	ProcessTerrain();
	return;
#else
	++m_iRestSemCount;

	if (!ReleaseSemaphore(m_hSemaphore, m_iRestSemCount, NULL))
		TraceError("TEMP_CAreaLoaderThread::Request: ReleaseSemaphore error");

	--m_iRestSemCount;
#endif
}

bool TEMP_CAreaLoaderThread::Fetch(CTerrain** ppTerrain)	// called in main thread
{
	m_TerrainCompleteMutex.Lock();

	if (m_pTerrainCompleteDeque.empty())
	{
		m_TerrainCompleteMutex.Unlock();
		return false;
	}

	*ppTerrain = m_pTerrainCompleteDeque.front();
	m_pTerrainCompleteDeque.pop_front();

	m_TerrainCompleteMutex.Unlock();
	return true;
}

void TEMP_CAreaLoaderThread::Request(CArea* pArea)	// called in main thread
{
	m_AreaRequestMutex.Lock();
	m_pAreaRequestDeque.push_back(pArea);
	m_AreaRequestMutex.Unlock();

#ifdef __ANDROID__
	ProcessArea();
	return;
#else
	++m_iRestSemCount;

	if (!ReleaseSemaphore(m_hSemaphore, m_iRestSemCount, NULL))
		TraceError("TEMP_CAreaLoaderThread::Request: ReleaseSemaphore error");

	--m_iRestSemCount;
#endif
}

bool TEMP_CAreaLoaderThread::Fetch(CArea** ppArea)	// called in main thread
{
	m_AreaCompleteMutex.Lock();

	if (m_pAreaCompleteDeque.empty())
	{
		m_AreaCompleteMutex.Unlock();
		return false;
	}

	*ppArea = m_pAreaCompleteDeque.front();
	m_pAreaCompleteDeque.pop_front();

	m_AreaCompleteMutex.Unlock();
	return true;
}

void TEMP_CAreaLoaderThread::ProcessArea()	// called in loader thread
{
	m_AreaRequestMutex.Lock();

	if (m_pAreaRequestDeque.empty())
	{
		m_AreaRequestMutex.Unlock();
		return;
	}

	CArea* pArea = m_pAreaRequestDeque.front();
	m_pAreaRequestDeque.pop_front();

	Tracef("TEMP_CAreaLoaderThread::ProcessArea() RequestDeque Size : %d\n", m_pAreaRequestDeque.size());
	m_AreaRequestMutex.Unlock();

	DWORD dwStartTime = ELTimer_GetMSec();

	// Area Load
	WORD wAreaCoordX, wAreaCoordY;
	pArea->GetCoordinate(&wAreaCoordX, &wAreaCoordY);
	DWORD dwID = (DWORD)(wAreaCoordX) * 1000L + (DWORD)(wAreaCoordY);

	const std::string& c_rStrMapName = pArea->GetOwner()->GetName();

	char szAreaPathName[64 + 1];
	_snprintf(szAreaPathName, sizeof(szAreaPathName), "%s\\%06u\\", c_rStrMapName.c_str(), dwID);

	pArea->Load(szAreaPathName);

	Tracef("TEMP_CAreaLoaderThread::ProcessArea LoadArea : %d ms elapsed\n", ELTimer_GetMSec() - dwStartTime);

	m_AreaCompleteMutex.Lock();
	m_pAreaCompleteDeque.push_back(pArea);
	m_AreaCompleteMutex.Unlock();

	Sleep(g_iLoadingDelayTime);
}

void TEMP_CAreaLoaderThread::ProcessTerrain()	// called in loader thread
{
	m_TerrainRequestMutex.Lock();

	if (m_pTerrainRequestDeque.empty())
	{
		m_TerrainRequestMutex.Unlock();
		return;
	}

	CTerrain* pTerrain = m_pTerrainRequestDeque.front();
	m_pTerrainRequestDeque.pop_front();

	Tracef("TEMP_CAreaLoaderThread::ProcessTerrain() RequestDeque Size : %d\n", m_pTerrainRequestDeque.size());
	m_TerrainRequestMutex.Unlock();

	DWORD dwStartTime = ELTimer_GetMSec();

	// Terrain Load
	WORD wCoordX, wCoordY;
	pTerrain->GetCoordinate(&wCoordX, &wCoordY);
	DWORD dwID = (DWORD)(wCoordX) * 1000L + (DWORD)(wCoordY);

	const std::string& c_rStrMapName = pTerrain->GetOwner()->GetName();
	char filename[256];
	sprintf(filename, "%s\\%06u\\AreaProperty.txt", c_rStrMapName.c_str(), dwID);

	CTokenVectorMap stTokenVectorMap;

	if (!LoadMultipleTextData(filename, stTokenVectorMap))
		return;

	Sleep(g_iLoadingDelayTime);

	if (stTokenVectorMap.end() == stTokenVectorMap.find("scripttype"))
		return;

	if (stTokenVectorMap.end() == stTokenVectorMap.find("areaname"))
		return;

	const std::string& c_rstrType = stTokenVectorMap["scripttype"][0];
	const std::string& c_rstrAreaName = stTokenVectorMap["areaname"][0];

	if (c_rstrType != "AreaProperty")
		return;

	char szRawHeightFieldname[64 + 1];
	char szWaterMapName[64 + 1];
	char szAttrMapName[64 + 1];
	char szShadowTexName[64 + 1];
	char szShadowMapName[64 + 1];
	char szMiniMapTexName[64 + 1];
	char szSplatName[64 + 1];

	_snprintf(szRawHeightFieldname, sizeof(szRawHeightFieldname), "%s\\%06u\\height.raw", c_rStrMapName.c_str(), dwID);
	_snprintf(szSplatName, sizeof(szSplatName), "%s\\%06u\\tile.raw", c_rStrMapName.c_str(), dwID);
	_snprintf(szAttrMapName, sizeof(szAttrMapName), "%s\\%06u\\attr.atr", c_rStrMapName.c_str(), dwID);
	_snprintf(szWaterMapName, sizeof(szWaterMapName), "%s\\%06u\\water.wtr", c_rStrMapName.c_str(), dwID);
	_snprintf(szShadowTexName, sizeof(szShadowTexName), "%s\\%06u\\shadowmap.dds", c_rStrMapName.c_str(), dwID);
	_snprintf(szShadowMapName, sizeof(szShadowMapName), "%s\\%06u\\shadowmap.raw", c_rStrMapName.c_str(), dwID);
	_snprintf(szMiniMapTexName, sizeof(szMiniMapTexName), "%s\\%06u\\minimap.dds", c_rStrMapName.c_str(), dwID);

	pTerrain->CopySettingFromGlobalSetting();

	pTerrain->LoadWaterMap(szWaterMapName);
	Sleep(g_iLoadingDelayTime);
	pTerrain->LoadHeightMap(szRawHeightFieldname);
	Sleep(g_iLoadingDelayTime);
	pTerrain->LoadAttrMap(szAttrMapName);
	Sleep(g_iLoadingDelayTime);
	pTerrain->RAW_LoadTileMap(szSplatName, true);
	Sleep(g_iLoadingDelayTime);
	pTerrain->LoadShadowTexture(szShadowTexName);
	Sleep(g_iLoadingDelayTime);
	pTerrain->LoadShadowMap(szShadowMapName);
	Sleep(g_iLoadingDelayTime);
	pTerrain->LoadMiniMapTexture(szMiniMapTexName);
	Sleep(g_iLoadingDelayTime);
	pTerrain->SetName(c_rstrAreaName.c_str());
	Sleep(g_iLoadingDelayTime);
	pTerrain->CalculateTerrainPatch();
	Sleep(g_iLoadingDelayTime);

	pTerrain->SetReady();

	Tracef("TEMP_CAreaLoaderThread::ProcessTerrain LoadTerrain : %d ms elapsed\n", ELTimer_GetMSec() - dwStartTime);

	m_TerrainCompleteMutex.Lock();
	m_pTerrainCompleteDeque.push_back(pTerrain);
	m_TerrainCompleteMutex.Unlock();

	Sleep(g_iLoadingDelayTime);
}
