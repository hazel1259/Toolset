#pragma once

#include ".\tstring.h"

#define arch_name(__N__) (LPCTSTR) #__N__
class CSystemInformation
{
public:
	CSystemInformation() 
  {
    Init();
  }
	~CSystemInformation() {}
  
  LPCTSTR   Architecture();
  LPCTSTR   ProcessorType();
  WORD      m_architecture; 
  WORD      m_reserved;
  DWORD     m_pageSize;
  LPVOID    m_minAppAddress;
  LPVOID    m_maxAppAddress;
  DWORD_PTR m_activeProcessorMask;
  DWORD     m_numProcessors;
  DWORD     m_processorType;
  DWORD     m_allocationGranularity; 
  WORD      m_processorLevel; 
  WORD	    m_revision; 

private:
	void Init()
	{
    SYSTEM_INFO info;
    ZeroMemory(&info, sizeof(info));
    GetSystemInfo(&info);
    m_architecture          = info.wProcessorArchitecture;
    m_reserved              = info.wReserved;
    m_pageSize              = info.dwPageSize;
    m_minAppAddress         = info.lpMinimumApplicationAddress;
    m_maxAppAddress         = info.lpMaximumApplicationAddress;
    m_activeProcessorMask   = info.dwActiveProcessorMask;
    m_numProcessors         = info.dwNumberOfProcessors;
    m_processorType         = info.dwProcessorType;
    m_allocationGranularity = info.dwAllocationGranularity;
    m_processorLevel        = info.wProcessorLevel;
  	m_revision              = info.wProcessorRevision;
  }
};

class CMemoryStatus
{
public:
  CMemoryStatus(void)
  {
    Init();
  }
  ~CMemoryStatus(void)
  {
  }
  DWORD     MemoryLoad;  
  DWORDLONG TotalPhysicalMemory;  
  DWORDLONG AvailablePhysicalMemory;  
  DWORDLONG TotalPageFile;  
  DWORDLONG AvailPageFile;  
  DWORDLONG TotalVirtualMemory;  
  DWORDLONG AvailableVirtualMemory;  
  DWORDLONG AvailableExtendedVirtualMemory;
private:
  void Init()
  {
    MEMORYSTATUSEX msex;
    ZeroMemory(&msex, sizeof(msex));
    msex.dwLength = sizeof(msex);
    GlobalMemoryStatusEx(&msex);
    MemoryLoad = msex.dwMemoryLoad ;  
    TotalPhysicalMemory = msex.ullTotalPhys ;  
    AvailablePhysicalMemory = msex.ullAvailPhys ;  
    TotalPageFile = msex.ullTotalPageFile ;  
    AvailPageFile = msex.ullAvailPageFile ;  
    TotalVirtualMemory = msex.ullTotalVirtual ;  
    AvailableVirtualMemory = msex.ullAvailVirtual ;  
    AvailableExtendedVirtualMemory = msex.ullAvailExtendedVirtual ;
  }
};


class CDateTime
{
public:
    CDateTime();
    ~CDateTime();
    LPSTR   GetDateStringA();
    LPSTR   GetTimeStringA();
    LPSTR   GetYearStringA();
    LPWSTR  GetYearStringW();
    LPWSTR  GetDateStringW();
    LPWSTR  GetTimeStringW();
#ifdef UNICODE 
#define GetDateString GetDateStringW
#define GetTimeString GetTimeStringW
#else
#define GetDateString GetDateStringA
#define GetTimeString GetTimeStringA
#endif
    LPSYSTEMTIME GetDateTime() { return &m_sysTime; }
private:
    void Init();
    SYSTEMTIME m_sysTime;
    TStringA * m_dateA;
    TStringA * m_timeA;

    TStringW * m_dateW;
    TStringW * m_timeW;
};