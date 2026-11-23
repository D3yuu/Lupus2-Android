#include "StdAfx.h"
#include <semaphore.h>
#include <errno.h>
#include <time.h>
#include "../EterPack/EterPackManager.h"
#include "FileLoaderThread.h"
#include "ResourceManager.h"

CFileLoaderThread::CFileLoaderThread()
    : m_bShutdowned(false), m_pArg(NULL), m_hThread(), m_bThreadCreated(false),
      m_uThreadID(0), m_hSemaphore(), m_bSemaphoreCreated(false), m_iRestSemCount(0)
{
}

CFileLoaderThread::~CFileLoaderThread()
{
    Shutdown();
    Destroy();
}

int CFileLoaderThread::Create(void* arg)
{
    Arg(arg);
    if (!Setup()) return false;
    if (pthread_create(&m_hThread, NULL, [](void* context) -> void* {
        CFileLoaderThread* thread = static_cast<CFileLoaderThread*>(context);
        thread->Run(thread->Arg());
        return NULL;
    }, this) != 0) return false;
    m_bThreadCreated = true;
    return true;
}

UINT CFileLoaderThread::Run(void* arg) { return Execute(arg); }
UINT CALLBACK CFileLoaderThread::EntryPoint(void* context)
{
    CFileLoaderThread* thread = static_cast<CFileLoaderThread*>(context);
    return thread->Run(thread->Arg());
}

void CFileLoaderThread::Destroy()
{
    if (m_bSemaphoreCreated) { sem_destroy(&m_hSemaphore); m_bSemaphoreCreated = false; }
    stl_wipe(m_pRequestDeque);
    stl_wipe(m_pCompleteDeque);
}

UINT CFileLoaderThread::Setup()
{
    if (m_bSemaphoreCreated) return 1;
    if (sem_init(&m_hSemaphore, 0, 0) != 0) return 0;
    m_bSemaphoreCreated = true;
    return 1;
}

void CFileLoaderThread::Shutdown()
{
    if (!m_bThreadCreated) return;
    m_bShutdowned = true;
    sem_post(&m_hSemaphore);
    pthread_join(m_hThread, NULL);
    m_bThreadCreated = false;
}

UINT CFileLoaderThread::Execute(void*)
{
    while (!m_bShutdowned) {
        while (sem_wait(&m_hSemaphore) != 0 && errno == EINTR) {}
        if (!m_bShutdowned) Process();
    }
    return 1;
}

void CFileLoaderThread::Request(std::string& fileName)
{
    TData* data = new TData;
    data->dwSize = 0;
    data->pvBuf = NULL;
    data->stFileName = fileName;
    m_RequestMutex.Lock();
    m_pRequestDeque.push_back(data);
    m_RequestMutex.Unlock();
    sem_post(&m_hSemaphore);
}

bool CFileLoaderThread::Fetch(TData** data)
{
    m_CompleteMutex.Lock();
    if (m_pCompleteDeque.empty()) { m_CompleteMutex.Unlock(); return false; }
    *data = m_pCompleteDeque.front();
    m_pCompleteDeque.pop_front();
    m_CompleteMutex.Unlock();
    return true;
}

void CFileLoaderThread::Process()
{
    m_RequestMutex.Lock();
    if (m_pRequestDeque.empty()) { m_RequestMutex.Unlock(); return; }
    TData* data = m_pRequestDeque.front();
    m_pRequestDeque.pop_front();
    m_RequestMutex.Unlock();
    LPCVOID source;
    if (CEterPackManager::Instance().Get(data->File, data->stFileName.c_str(), &source)) {
        data->dwSize = data->File.Size();
        data->pvBuf = new char[data->dwSize];
        memcpy(data->pvBuf, source, data->dwSize);
    }
    m_CompleteMutex.Lock();
    m_pCompleteDeque.push_back(data);
    m_CompleteMutex.Unlock();
    Sleep(g_iLoadingDelayTime);
}
