#pragma once
//#include "error.h"

class CFileIo
{
public:
  CFileIo(void);
  ~CFileIo(void);
  HANDLE OpenFile(char * filename);
  BOOL CloseFile(HANDLE * pHandle);
  BOOL MyReadFile(LPTSTR pszFileName , PVOID * ppBuffer, PDWORD pdwSize);
  BOOL MyWriteFile(LPTSTR pszFileName, PUCHAR pBuffer , DWORD dwBufferSize);
  int m_Error;
  BOOL MyFreeFile(PVOID *);
};
