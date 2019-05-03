#include "comm_inc.h"
#include "fileio.h"

CFileIo::CFileIo(void)
{
}

CFileIo::~CFileIo(void)
{
}


HANDLE CFileIo::OpenFile(char * filename)
{
  return CreateFile(
                    filename, 
                    GENERIC_READ | GENERIC_WRITE,
                    0,
                    NULL,
                    OPEN_ALWAYS,
                    FILE_ATTRIBUTE_NORMAL,
                    NULL
                    );
}

BOOL CFileIo::CloseFile(HANDLE * ph)
{
  if (ph)
  {
    if (CloseHandle(*ph))
    {
      *ph = INVALID_HANDLE_VALUE;
      return TRUE;
    }
  }
  return FALSE;
}

BOOL CFileIo::MyReadFile(LPTSTR pszFileName , PVOID * ppBuffer, PDWORD pdwSize)
{
  // open the file
  PUCHAR pBuffer    = NULL;
  HANDLE hf         = INVALID_HANDLE_VALUE;
  DWORD dwSize      = 0;
  DWORD cb          = 0;
  DWORD dwAttribs   = INVALID_FILE_ATTRIBUTES;
  BOOL  br          = FALSE;
  // parameter checking
  //
  if (!ppBuffer || !pdwSize)
  {
    ////printf("%s: Invalid buffers passed to function\n", __FUNCTION__);
    goto DONE;
  }
  // assume failure
  *ppBuffer = NULL;
  *pdwSize = 0;

  // try to get the files attributes
  //
  // open the file - if we fail here then too bad, go away quietly
  hf = OpenFile(pszFileName);

  // if we got no handle, bail out
  if (INVALID_HANDLE_VALUE == hf)
  {
    
    //DoErrorMessage(GetLastError());
    goto DONE;
  }

  // figure out the size of the buffer
  dwSize = GetFileSize(hf, NULL);

  // invalid size then exit
  if (INVALID_FILE_SIZE == dwSize)
  {
   // but close the file first
    
    //CloseFile(&hf);
    goto DONE;
  }

  pBuffer = 
        (LPBYTE)HeapAlloc(GetProcessHeap(), 
                  HEAP_ZERO_MEMORY, 
                  dwSize
                  );
  // try to alloc some memeory for the buffre to return
  if (NULL == pBuffer)
  {
    // no memory, Close file and return FALSE 
    ////printf("%s out of memory\n", __FUNCTION__);
    CloseFile(&hf);
    goto DONE;
  }
  // valid size so consider reading the file
  //
  if (0 == 
          ReadFile(hf, pBuffer, dwSize, &cb, NULL)
     )
  {
    // failed so clean up and leave, no use warning why, we can
    // just do this in the caller
    ////printf("%s- Error %d reading %s\n", __FUNCTION__, GetLastError(), pszFileName);
    //DoErrorMessage(GetLastError());
    HeapFree(GetProcessHeap(), 0, pBuffer);
    pBuffer = NULL;
    goto DONE;
  }
  // if we get this far,it means we had no errors above
  br = TRUE;
  *ppBuffer = pBuffer;
  *pdwSize = dwSize; 
  // we should checkm cb for a valid size, perhaps assert in checked 
  // builds
DONE:
  // got this far., everything must be OK
  if (INVALID_HANDLE_VALUE != hf)
  {
    CloseFile(&hf);
  }
  return br;
}

BOOL 
CFileIo::MyWriteFile(
                     LPTSTR pszFileName, 
                     PUCHAR pBuffer , 
                     DWORD dwBufferSize
                     )
{
  // open the file
  HANDLE  hf            = INVALID_HANDLE_VALUE;
  DWORD   dwSize        = 0;
  DWORD   cb            = 0;
  DWORD   dwAttribs     = 0;
  BOOL    br            = FALSE;
  // parameter checking
  //
  if (!pBuffer || !dwBufferSize)
  {
    OutputDebugString("The buffer or the size provided is not correct\r\n");
//DoErrorMessage(ERROR_INVALID_PARAMETER);
    goto DONE;
  }
  // open the file if possible
  //
  hf = OpenFile(pszFileName);
  // if we got no handle, bail out
  //
  if (INVALID_HANDLE_VALUE == hf)
  {
    ////printf("%s: Error: Unable to open %s\n", __FUNCTION__, pszFileName);
    //DoErrorMessage(GetLastError());
    goto DONE;
  }
  // Check to see if file pointer was set or not
  //
  if ((LONG)INVALID_SET_FILE_POINTER == 
      SetFilePointer( hf, 
                      0, 
                      NULL, 
                      FILE_END)
                    )
  {
    //DoErrorMessage(GetLastError());
    //DoErrorMessage(GetLastError());
    goto DONE;
  }
  // check to see if we were able to write to the file
  if (FALSE == (br = WriteFile(hf, pBuffer, dwBufferSize, &cb, NULL)))
  {
    //DoErrorMessage(GetLastError());
  }
  
  // we should checkm cb for a valid size, perhaps assert in checked 
  // builds
  // got this far., everything must be OK
DONE:
  if (hf != INVALID_HANDLE_VALUE)
  {
    CloseFile(&hf);
  }
  return br;
}

BOOL CFileIo::MyFreeFile(PVOID * pBuff)
{
  
  PVOID p = *pBuff;
  if (p)
  {
    MyFree(p);
    return TRUE;
  }
  return FALSE;
}