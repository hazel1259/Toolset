#include "comm_inc.h"
#include "Thread.h"


DWORD WINAPI ThreadProc(PVOID pv)
{
    CThread * thread = (CThread *)pv;
    DWORD exitCode = thread->Work();
    ExitThread(exitCode);
    return exitCode;
}
/*
UINT                    m_priority;
LPSECURITY_ATTRIBUTES   m_lpSA;
HANDLE                  m_signal;
HANDLE                  m_mutex;
CRITICAL_SECTION        m_cs;
DWORD                   m_exitCode;
DWORD                   m_errorCount;
DWORD                   m_threadId;
HANDLE                  m_thread;
UINT                    m_sync;
UINT                    m_stackSize;
*/
CThread::CThread()
: m_exitCode(0)
, m_errorCount(0)
, m_thread(NULL)
, m_signal(NULL)
, m_mutex(NULL)
, m_sync(0)
, m_stackSize(0)
//, m_lpSA(NULL)
, m_threadId(-1)
, m_priority(NORMAL_PRIORITY_CLASS)
{
}


CThread::~CThread()
{
}

DWORD CThread::Start()
{
    DWORD dwRet = 0; 
    m_thread = CreateThread(NULL, m_stackSize, (LPTHREAD_START_ROUTINE)ThreadProc, (PVOID)this, CREATE_SUSPENDED, &m_threadId);
    return dwRet;
}

void CThread::Pause()
{
    if (m_thread)
    {
        SuspendThread(m_thread);
    }
}

void CThread::Resume()
{
    if (m_thread)
    {
        ResumeThread(m_thread);
    }
}

DWORD CThread::Exit()
{
    DWORD dwRet = 0;
    DWORD dwWait = WaitForSingleObject(m_thread, m_timeOut);
    if (dwWait != WAIT_ABANDONED)
    {
        CloseHandle(m_thread);
    }
    m_thread = NULL;
    if (m_signal)
    {
        SetEvent(m_signal);
    }
    return m_exitCode;
}

void CThread::SetSignal(HANDLE eventHandle)
{
    m_signal = eventHandle;
}

void CThread::Syncronize(bool bGlobal)
{
    if (bGlobal)
    {
        m_sync = SYNCFLAGS::SYNCPROCESS;
    }
    else
    {
        m_sync = SYNCFLAGS::SYNCTHREADS;
    }
}
DWORD CThread::GetExitCode()
{
    return m_exitCode;
}
