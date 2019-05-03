

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
  //DWORD     dwLength;  
  DWORD     MemoryLoad;  
  DWORDLONG TotalPhysicalMemory;  
  DWORDLONG AvailablePhysicalMemory;  
  DWORDLONG TotalPageFile;  
  DWORDLONG AvailPageFile;  
  DWORDLONG TotalVirtualMemory;  
  DWORDLONG AvailableVirtualMemory;  
  DWORDLONG AvailableExtendedVirtualMemory;
private:
  /*
    DWORD dwLength;  DWORD dwMemoryLoad;  DWORDLONG ullTotalPhys;  DWORDLONG ullAvailPhys;  
    DWORDLONG ullTotalPageFile;  DWORDLONG ullAvailPageFile;  DWORDLONG ullTotalVirtual;  DWORDLONG ullAvailVirtual;  
    DWORDLONG ullAvailExtendedVirtual;
  */
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


//LPTSTR arch_names[] =
//{
//#PROCESSOR_ARCHITECTURE_INTEL,
//#PROCESSOR_ARCHITECTURE_MIPS, 
//#PROCESSOR_ARCHITECTURE_UNKNOWN, 
//#PROCESSOR_ARCHITECTURE_SHX, 
//#PROCESSOR_ARCHITECTURE_ARM 
//};
#define arch_name(__N__) (LPCTSTR) #__N__
class CSystemInfo
{
public:
	CSystemInfo() 
  {
    Init();
  }
	~CSystemInfo() {}
  
  LPCTSTR Architecture();
  LPCTSTR ProcessorType();
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

	/*
	typedef struct _SYSTEM_INFO {
  union {
  DWORD dwOemId;
  struct {
    WORD wProcessorArchitecture;
    WORD wReserved;
  };
};
  DWORD dwPageSize;
  LPVOID lpMinimumApplicationAddress;
  LPVOID lpMaximumApplicationAddress;
  DWORD dwActiveProcessorMask;
  DWORD dwNumberOfProcessors;
  DWORD dwProcessorType;
  DWORD dwAllocationGranularity;
  WORD wProcessorLevel;
  WORD wProcessorRevision;
} SYSTEM_INFO, *LPSYSTEM_INFO;
*/
};
