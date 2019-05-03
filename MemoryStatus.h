#pragma once

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
