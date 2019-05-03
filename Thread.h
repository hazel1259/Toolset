#pragma once
enum SYNCFLAGS
{
    NOSYNC = 0,
    SYNCTHREADS = 2,
    SYNCPROCESS = 3
};

class CThread
{
public:
	CThread();
	~CThread();
    void            Syncronize(bool global);
    //void            SetSecurity(LPSECURITY_ATTRIBUTES lpSA);
    void            SetSignal(HANDLE eventHandle);
    void            SetPriority(UINT priority);
    //void            SetStackSize(UINT stackSize);
    virtual	DWORD   Start();
	virtual	void    Pause();
	virtual	DWORD   Exit();
    virtual void    Resume();
	virtual	DWORD   Work() = 0;
	DWORD	        GetExitCode();
private:
protected:
    
    DWORD m_timeOut;
    UINT                    m_priority;
    //LPSECURITY_ATTRIBUTES   m_lpSA;
    HANDLE                  m_signal;
    HANDLE                  m_mutex;
    CRITICAL_SECTION        m_cs;
	DWORD                   m_exitCode;
	DWORD                   m_errorCount;
    DWORD                   m_threadId;
    HANDLE                  m_thread;
    UINT                    m_sync;
    UINT                    m_stackSize;
};

