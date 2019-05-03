
#include "comm_inc.h"
#include "filestream.h"

/***********************************************************************************************************************

  CFileStream()

  Type:       Constructor
  Base:       CStream
  Scope:      Public
  Parameters: none
  
  returns: None

  This constructor does nothing
*************************************************************************************************************************/
CFileStreamA::CFileStreamA(void)
: m_dwSize(0)
, m_pBuffer(0)
, m_file_handle(INVALID_HANDLE_VALUE)
, m_open_attributes(FILE_ATTRIBUTE_NORMAL)
, m_open_flags(OPEN_EXISTING)
, m_desired_access(GENERIC_READ | GENERIC_WRITE)
, m_security_attributes(NULL)  
, m_file_template(NULL)
, m_share_mode(0)
, m_fError(FALSE)
, m_sizeHigh(0)
, m_hugeFileSize(0)
, m_StreamName(NULL)
{
  m_currPos= 0;
  m_pBuffer= 0;
  m_fError= 0;
  m_stream_name= 0;

  //Init();
//  m_pMem = new CMem();
}

DWORD CFileStreamA::Size()
{
  return m_dwSize;
}

LPTSTR CFileStreamA::Name()
{
  return m_StreamName->GetString();
}
/***********************************************************************************************************************

  ~CFileStream()

  Type:       Destructor
  Base:       CStream
  Scope:      Public
  Parameters: none
  
  returns: None

  This Destructor removes the open file handle and deletes all allocatd objects and memory
*************************************************************************************************************************/
CFileStreamA::~CFileStreamA(void)
{
  Clear();
}

BOOL CFileStreamA::Create(LPSTR pszStream)
{
  // make sure have a valid stream
  //
  if (NULL == pszStream)
  {
    return FALSE;
  }
  // instantiuate the TString object
  m_StreamName = new TStringA(pszStream);
  //
  // If the handle is still valid, close it
  if (INVALID_HANDLE_VALUE != m_file_handle)
  {
    Close();
  }
  //
  // Open a new file
  //
  m_file_handle = 
    CreateFileA(
      pszStream, 
      m_desired_access,
      m_share_mode,
      m_security_attributes,
      CREATE_ALWAYS,
      m_open_attributes,
      m_file_template
      );
  // check for an invalid handle
  //
  if (INVALID_HANDLE_VALUE == m_file_handle)
  {
    return FALSE;
  }
  return TRUE;
}

/***********************************************************************************************************************

  OpenFile()

  Type:       Method
  Base:       this
  Scope:      Public
  Parameters:

  LPTSTR pszStream: pointer to a null terminated string which describes the data to stream
  
  returns: TRUE if we open the stream handle, FALSE otherwise

  This method opens the file using CreateFile() and then determines the size of the file using GetFileSize. Note that
  we get the full 64 bit size.

*************************************************************************************************************************/

BOOL 
CFileStreamA::OpenFile(
  LPSTR pszStream
  )
{
  LARGE_INTEGER li;
  m_dwSize = m_sizeHigh = 0;
  // make sure have a valid stream
  //
  if (NULL == pszStream)
  {
    return FALSE;
  }
  // instantiuate the TString object
  m_StreamName = new TStringA(pszStream);
  //
  // If the handle is still valid, close it
  Clear();
  //
  // Open a new file
  //
  m_file_handle = 
    CreateFileA(
      pszStream, 
      m_desired_access,
      m_share_mode,
      m_security_attributes,
      m_open_flags,
      m_open_attributes,
      m_file_template
      );
  // check for an invalid handle
  //
  if (INVALID_HANDLE_VALUE == m_file_handle)
  {
    return FALSE;
  }
  // Try to obtain hFile's huge size. 
  //
  m_dwSize = GetFileSize (m_file_handle, NULL); //&m_sizeHigh) ; 
  //
  // Check for an error
  //
  if (  m_dwSize        == INVALID_FILE_SIZE &&
        GetLastError() !=   NO_ERROR 
      )
  { 
    m_fError = TRUE;
    // Deal with that failure. 
  }
  else
  {
    m_fError = FALSE;
  }
  li.LowPart = m_dwSize;
  li.HighPart = m_sizeHigh;
  m_hugeFileSize = li.QuadPart;
  return TRUE;
}

/***********************************************************************************************************************

  Open()

  Type:       Method
  Base:       this
  Scope:      Public
  Parameters:

  LPTSTR pszStream: pointer to a null terminated string which describes the data to stream
  
  returns: TRUE if we open the stream handle, FALSE otherwise

  This method opens the file using CreateFile() and then determines the size of the file using GetFileSize. Note that
  we get the full 64 bit size.

*************************************************************************************************************************/
BOOL 
CFileStreamA::Open(
  LPSTR pszStream
  )
{
  BOOL br = FALSE;
  DWORD cb = 0;
  if (!OpenFile(pszStream))
  {
    //DoErrorMessage(GetLastError());
    goto DONE;
  }
  if (m_pBuffer != NULL)
  {
    MyFree(m_pBuffer);
  }
  m_pBuffer = MyAlloc(PUCHAR, m_dwSize);
  // try to alloc some memeory for the buffre to return
  if (NULL == m_pBuffer)
  {
    Close();
    goto DONE;
  }
  // valid size so consider reading the file
  //
  if (0 == 
          ReadFile(m_file_handle, m_pBuffer, m_dwSize, &cb, NULL)
     )
  {
    MyFree(m_pBuffer);
    goto DONE;
  }
  // if we get this far,it means we had no errors above
  br = TRUE;
DONE:
  return br;
}

/***********************************************************************************************************************

  Close()

  Type:       Method
  Base:       this
  Scope:      Public
  Parameters:

  none  
  
  returns: TRUE if we close the open stream handle, FALSE otherwise

  This method insures that the handle us valid before closing, changes may want to commit dirty cached data.

*************************************************************************************************************************/
BOOL 
CFileStreamA::Close(
  void
  )
{
	if (m_pBuffer)
	{
		MyFree(m_pBuffer);
	}
	// validate the file handle
	if (m_file_handle != INVALID_HANDLE_VALUE)
	{
		// and close the handle
		CloseHandle(m_file_handle);
		m_file_handle = INVALID_HANDLE_VALUE;
	}
	m_currPos = 0;
  return TRUE;
}

/***********************************************************************************************************************

  Write()

  Type:       Method
  Base:       this
  Scope:      Public
  Parameters:

  PUCHAR pBuffer - pointer to a buffer of bytes to write to the file  
  DWORD  dwSize  - size of the buffer, this size is validated
  DWORD  Offset  - Offset in the buffer to start writng, anything at that position is overwritten

  none  
  
  returns: TRUE if we write the data to the open stream handle at the offset, FALSE otherwise

*************************************************************************************************************************/
BOOL 
CFileStreamA::Write(
  PUCHAR pBuffer, 
  DWORD  dwSize, 
  DWORD  Offset
  )
{
  //FILE_ATTRIBUTE_VIRTUAL
  if (m_file_handle == INVALID_HANDLE_VALUE)
  {
    return FALSE;
  }
  // check to see if we were able to write to the file
  if (Offset > m_dwSize)
  {
    return FALSE;
  }
  if (!this->SetPosition(Offset))
  {
    return FALSE;
  }
  return Write(pBuffer, dwSize);
}

/***********************************************************************************************************************

  Close()

  Type:       Method
  Base:       this
  Scope:      Public
  Parameters:

  PUCHAR pBuffer - pointer to a buffer of bytes to write to the file  
  DWORD  dwSize  - size of the buffer, this size is validated

  none  
  
  returns: TRUE if we write the data to the open stream handle at the current offset, FALSE otherwise

*************************************************************************************************************************/
BOOL 
CFileStreamA::Write(
  PUCHAR pBuffer, 
  DWORD dwSize
  )
{
  DWORD cb = 0;
  // if we cannot write to the file then just bail out with no warnings, (keep last error)
  if (FALSE == WriteFile(m_file_handle, pBuffer, dwSize, &cb, NULL))
  {
    return FALSE;
  }
  return TRUE;
}
/***********************************************************************************************************************

  ReadChar()

  Type:       Method
  Base:       this
  Scope:      Public
  Parameters:

  char * pch - pointer to a character to write the data to

  none  
  
  returns: TRUE if we write the data to the specified location

  Note we try/except this code because calling IsBadWritePtr is more costly than just a t/e block

*************************************************************************************************************************/

BOOL 
CFileStreamA::ReadChar(
  CHAR * pch
  )
{
  BOOL br = FALSE;
  if (m_currPos >= m_dwSize)
  {
    return FALSE;
  }
  
  if (!pch)
  {
    goto DONE;
    //return FALSE;
  }

  else
  {
    __try
    {
      *pch = m_pBuffer[m_currPos];
      m_currPos++;
      br = TRUE;
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
      br = FALSE;
    }
  }
DONE:
  return br;
}

/***********************************************************************************************************************

  ReadPrev()

  Type:       Method
  Base:       this
  Scope:      Public
  Parameters:

  char * pch - pointer to a character to write the data to

  none  
  
  returns: TRUE if we write the data to the specified location

  Note we try/except this code because calling IsBadWritePtr is more costly than just a t/e block

*************************************************************************************************************************/

BOOL 
CFileStreamA::ReadPrev(
  PCHAR pch
  )
{
  BOOL br = FALSE;
  if (m_currPos == 0)
  {
    return FALSE;
  }
  
  if (!pch)
  {
    goto DONE;
  }

  else
  {
    __try
    {
      m_currPos--;
      *pch = m_pBuffer[m_currPos];
      br = TRUE;
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
      br = FALSE;
    }
  }
DONE:
  return br;
}

/***********************************************************************************************************************

  ReadChar()

  Type:       Method
  Base:       this
  Scope:      Public
  Parameters:

  char * pch - pointer to a character to write the data to

  none  
  
  returns: TRUE if we write the data to the specified location

  Note we try/except this code because calling IsBadWritePtr is more costly than just a t/e block

*************************************************************************************************************************/
BOOL 
CFileStreamA::Read(
  PUCHAR pBuffer, 
  DWORD len
  )
{
  if (IsBadWritePtr((void *)pBuffer, len))
  {
    return FALSE;
  }
  if (NULL == m_pBuffer)
  {
    return FALSE;
  }
  if (len > m_dwSize)
  {
    return FALSE;
  }
  int remaining = m_dwSize - m_currPos;
  if (remaining <= 0)
  {
    return FALSE;
  }
  //if (m_currPos 
  memcpy(pBuffer, &m_pBuffer[m_currPos], len);
  m_currPos += len;
  return TRUE;
  //return FALSE;
}

/***********************************************************************************************************************

  Read()

  Type:       Method
  Base:       this
  Scope:      Public
  Parameters:

  PUCHAR  pBuffer   - buffer to read into 
  DWORD   len       - length of pBuffer
  DWORD   position  - position in the file stream to start reading

  
  returns: TRUE if we read the buffer at the specified location

  Note that we use IsBadWritePtr on this code which is not thread safe, so you need to protect this code
  if using in MT apps (which is usually the case)
*************************************************************************************************************************/
BOOL 
CFileStreamA::Read(
  PUCHAR  pBuffer, 
  DWORD   len, 
  DWORD   position
  )
{
  if (IsBadWritePtr((void *)pBuffer, len))
  {
    return FALSE;
  }
  if (NULL == m_pBuffer)
  {
    return FALSE;
  }

  if (len > (int)(m_dwSize - position))
  {
    return FALSE;
  }
  memcpy(pBuffer, &m_pBuffer[position], len);
  return TRUE;
}

/***********************************************************************************************************************

  SetPosition()

  Type:       Method
  Base:       this
  Scope:      Public
  Parameters:

  DWORD position - position to set the current file pointer to.

  returns: TRUE if set the file pointer, false otherwise.

*************************************************************************************************************************/
DWORD 
CFileStreamA::SetPosition(
  DWORD new_position
  )
{
  m_currPos =  SetFilePointer( m_file_handle, 0, NULL, new_position);
  
  if (m_currPos == INVALID_SET_FILE_POINTER)
  {
    return FALSE;
  }
  return TRUE;
}

/***********************************************************************************************************************

  Commit()

  Type:       Method
  Base:       this
  Scope:      Public
  Parameters:


  returns: FALSE always

  This is a stub for file commits which we'll need when we move to a cache model. This functions purpose is to 
  write the data to the storage device.

*************************************************************************************************************************/
BOOL 
CFileStreamA::Commit(
  void
  )
{
  return 0;
}


/***********************************************************************************************************************

  Grow()

  Type:       Method
  Base:       this
  Scope:      Public
  Parameters:
  DWORD dwBytesToGrow - number of bytes to grow.

  returns: FALSE always

  This is a stub for file extends the file bu dwBytes

*************************************************************************************************************************/
BOOL CFileStreamA::Grow(DWORD dwBytesToGrow)
{
  //DWORD dwOldEnd = GetFilePoonter
  if (!SetEndOfFile(m_file_handle))
  {
    return FALSE;
  }
  m_currPos = m_dwSize;

  DWORD pos = 
    SetFilePointer (m_file_handle, dwBytesToGrow, NULL, FILE_END) ; 
  
  if (INVALID_SET_FILE_POINTER == pos)
  {
    return FALSE;
  }
  
  PUCHAR pData = MyAlloc(PUCHAR, dwBytesToGrow);
  if (!Write(pData, dwBytesToGrow))
  {
    return FALSE;
  }
  return TRUE;
}

void CFileStreamA::Clear(void)
{
  //if (m_pMem)
  //{
  //  delete m_pMem;
  //}
  if (m_pBuffer)
  {
    MyFree(m_pBuffer);
  }
  Close();

}

/***********************************************************************************************************************

  CFileStream()

  Type:       Constructor
  Base:       CStream
  Scope:      Public
  Parameters: none
  
  returns: None

  This constructor does nothing
*************************************************************************************************************************/
CFileStreamW::CFileStreamW(void)
: m_dwSize(0)
, m_pBuffer(0)
, m_file_handle(INVALID_HANDLE_VALUE)
, m_open_attributes(FILE_ATTRIBUTE_NORMAL)
, m_open_flags(OPEN_EXISTING)
, m_desired_access(GENERIC_READ | GENERIC_WRITE)
, m_security_attributes(NULL)  
, m_file_template(NULL)
, m_share_mode(0)
, m_fError(FALSE)
, m_sizeHigh(0)
, m_hugeFileSize(0)
, m_StreamName(NULL)
{
  m_currPos= 0;
  m_pBuffer= 0;
  m_fError= 0;
  m_stream_name= 0;

  //Init();
//  m_pMem = new CMem();
}

DWORD CFileStreamW::Size()
{
  return m_dwSize;
}

LPWSTR CFileStreamW::Name()
{
  return m_StreamName->GetString();
}
/***********************************************************************************************************************

  ~CFileStream()

  Type:       Destructor
  Base:       CStream
  Scope:      Public
  Parameters: none
  
  returns: None

  This Destructor removes the open file handle and deletes all allocatd objects and memory
*************************************************************************************************************************/
CFileStreamW::~CFileStreamW(void)
{
  Clear();
}

BOOL CFileStreamW::Create(LPWSTR pszStream)
{
  // make sure have a valid stream
  //
  if (NULL == pszStream)
  {
    return FALSE;
  }
  // instantiuate the TStringW object
  m_StreamName = new TStringW(pszStream);
  //
  // If the handle is still valid, close it
  if (INVALID_HANDLE_VALUE != m_file_handle)
  {
    Close();
  }
  //
  // Open a new file
  //
  m_file_handle = 
    CreateFileW(
      pszStream, 
      m_desired_access,
      m_share_mode,
      m_security_attributes,
      CREATE_ALWAYS,
      m_open_attributes,
      m_file_template
      );
  // check for an invalid handle
  //
  if (INVALID_HANDLE_VALUE == m_file_handle)
  {
    return FALSE;
  }
  return TRUE;
}

/***********************************************************************************************************************

  OpenFile()

  Type:       Method
  Base:       this
  Scope:      Public
  Parameters:

  LPWSTR pszStream: pointer to a null terminated string which describes the data to stream
  
  returns: TRUE if we open the stream handle, FALSE otherwise

  This method opens the file using CreateFileW) and then determines the size of the file using GetFileSize. Note that
  we get the full 64 bit size.

*************************************************************************************************************************/

BOOL 
CFileStreamW::OpenFile(
  LPWSTR pszStream
  )
{
  LARGE_INTEGER li;
  m_dwSize = m_sizeHigh = 0;
  // make sure have a valid stream
  //
  if (NULL == pszStream)
  {
    return FALSE;
  }
  // instantiuate the TStringW object
  m_StreamName = new TStringW(pszStream);
  //
  // If the handle is still valid, close it
  Clear();
  //
  // Open a new file
  //
  m_file_handle = 
    CreateFileW(
      pszStream, 
      m_desired_access,
      m_share_mode,
      m_security_attributes,
      m_open_flags,
      m_open_attributes,
      m_file_template
      );
  // check for an invalid handle
  //
  if (INVALID_HANDLE_VALUE == m_file_handle)
  {
    return FALSE;
  }
  // Try to obtain hFile's huge size. 
  //
  m_dwSize = GetFileSize (m_file_handle, NULL); //&m_sizeHigh) ; 
  //
  // Check for an error
  //
  if (  m_dwSize        == INVALID_FILE_SIZE &&
        GetLastError() !=   NO_ERROR 
      )
  { 
    m_fError = TRUE;
    // Deal with that failure. 
  }
  else
  {
    m_fError = FALSE;
  }
  li.LowPart = m_dwSize;
  li.HighPart = m_sizeHigh;
  m_hugeFileSize = li.QuadPart;
  return TRUE;
}

/***********************************************************************************************************************

  Open()

  Type:       Method
  Base:       this
  Scope:      Public
  Parameters:

  LPWSTR pszStream: pointer to a null terminated string which describes the data to stream
  
  returns: TRUE if we open the stream handle, FALSE otherwise

  This method opens the file using CreateFileW) and then determines the size of the file using GetFileSize. Note that
  we get the full 64 bit size.

*************************************************************************************************************************/
BOOL 
CFileStreamW::Open(
  LPWSTR pszStream
  )
{
  BOOL br = FALSE;
  DWORD cb = 0;
  if (!OpenFile(pszStream))
  {
    ///DoErrorMessage(GetLastError());
    goto DONE;
  }
  if (m_pBuffer != NULL)
  {
    MyFree(m_pBuffer);
  }
  m_pBuffer = MyAlloc(PUCHAR, m_dwSize);
  // try to alloc some memeory for the buffre to return
  if (NULL == m_pBuffer)
  {
    Close();
    goto DONE;
  }
  // valid size so consider reading the file
  //
  if (0 == 
          ReadFile(m_file_handle, m_pBuffer, m_dwSize, &cb, NULL)
     )
  {
    MyFree(m_pBuffer);
    goto DONE;
  }
  // if we get this far,it means we had no errors above
  br = TRUE;
DONE:
  return br;
}

/***********************************************************************************************************************

  Close()

  Type:       Method
  Base:       this
  Scope:      Public
  Parameters:

  none  
  
  returns: TRUE if we close the open stream handle, FALSE otherwise

  This method insures that the handle us valid before closing, changes may want to commit dirty cached data.

*************************************************************************************************************************/
BOOL 
CFileStreamW::Close(
  void
  )
{
  // validate the file handle
  if (m_file_handle != INVALID_HANDLE_VALUE)
  {
    // and close the handle
    CloseHandle(m_file_handle);
    m_file_handle = INVALID_HANDLE_VALUE;
  
    return TRUE;
  }
  if (m_pBuffer)
  {
    MyFree(m_pBuffer);
  }
  m_currPos = 0;

  return FALSE;
}

/***********************************************************************************************************************

  Write()

  Type:       Method
  Base:       this
  Scope:      Public
  Parameters:

  PUCHAR pBuffer - pointer to a buffer of bytes to write to the file  
  DWORD  dwSize  - size of the buffer, this size is validated
  DWORD  Offset  - Offset in the buffer to start writng, anything at that position is overwritten

  none  
  
  returns: TRUE if we write the data to the open stream handle at the offset, FALSE otherwise

*************************************************************************************************************************/
BOOL 
CFileStreamW::Write(
  PUCHAR pBuffer, 
  DWORD  dwSize, 
  DWORD  Offset
  )
{
  //FILE_ATTRIBUTE_VIRTUAL
  if (m_file_handle == INVALID_HANDLE_VALUE)
  {
    return FALSE;
  }
  // check to see if we were able to write to the file
  if (Offset > m_dwSize)
  {
    return FALSE;
  }
  if (!this->SetPosition(Offset))
  {
    return FALSE;
  }
  return Write(pBuffer, dwSize);
}

/***********************************************************************************************************************

  Close()

  Type:       Method
  Base:       this
  Scope:      Public
  Parameters:

  PUCHAR pBuffer - pointer to a buffer of bytes to write to the file  
  DWORD  dwSize  - size of the buffer, this size is validated

  none  
  
  returns: TRUE if we write the data to the open stream handle at the current offset, FALSE otherwise

*************************************************************************************************************************/
BOOL 
CFileStreamW::Write(
  PUCHAR pBuffer, 
  DWORD dwSize
  )
{
  DWORD cb = 0;
  // if we cannot write to the file then just bail out with no warnings, (keep last error)
  if (FALSE == WriteFile(m_file_handle, pBuffer, dwSize, &cb, NULL))
  {
    return FALSE;
  }
  return TRUE;
}
/***********************************************************************************************************************

  ReadChar()

  Type:       Method
  Base:       this
  Scope:      Public
  Parameters:

  char * pch - pointer to a character to write the data to

  none  
  
  returns: TRUE if we write the data to the specified location

  Note we try/except this code because calling IsBadWritePtr is more costly than just a t/e block

*************************************************************************************************************************/

BOOL 
CFileStreamW::ReadChar(
  CHAR * pch
  )
{
  BOOL br = FALSE;
  if (m_currPos >= m_dwSize)
  {
    return FALSE;
  }
  
  if (!pch)
  {
    goto DONE;
    //return FALSE;
  }

  else
  {
    __try
    {
      *pch = m_pBuffer[m_currPos];
      m_currPos++;
      br = TRUE;
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
      br = FALSE;
    }
  }
DONE:
  return br;
}

/***********************************************************************************************************************

  ReadPrev()

  Type:       Method
  Base:       this
  Scope:      Public
  Parameters:

  char * pch - pointer to a character to write the data to

  none  
  
  returns: TRUE if we write the data to the specified location

  Note we try/except this code because calling IsBadWritePtr is more costly than just a t/e block

*************************************************************************************************************************/

BOOL 
CFileStreamW::ReadPrev(
  PCHAR pch
  )
{
  BOOL br = FALSE;
  if (m_currPos == 0)
  {
    return FALSE;
  }
  
  if (!pch)
  {
    goto DONE;
  }

  else
  {
    __try
    {
      m_currPos--;
      *pch = m_pBuffer[m_currPos];
      br = TRUE;
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
      br = FALSE;
    }
  }
DONE:
  return br;
}

/***********************************************************************************************************************

  ReadChar()

  Type:       Method
  Base:       this
  Scope:      Public
  Parameters:

  char * pch - pointer to a character to write the data to

  none  
  
  returns: TRUE if we write the data to the specified location

  Note we try/except this code because calling IsBadWritePtr is more costly than just a t/e block

*************************************************************************************************************************/
BOOL 
CFileStreamW::Read(
  PUCHAR pBuffer, 
  DWORD len
  )
{
  if (IsBadWritePtr((void *)pBuffer, len))
  {
    return FALSE;
  }
  if (NULL == m_pBuffer)
  {
    return FALSE;
  }
  if (len > m_dwSize)
  {
    return FALSE;
  }
  int remaining = m_dwSize - m_currPos;
  if (remaining <= 0)
  {
    return FALSE;
  }
  //if (m_currPos 
  memcpy(pBuffer, &m_pBuffer[m_currPos], len);
  m_currPos += len;
  return TRUE;
  //return FALSE;
}

/***********************************************************************************************************************

  Read()

  Type:       Method
  Base:       this
  Scope:      Public
  Parameters:

  PUCHAR  pBuffer   - buffer to read into 
  DWORD   len       - length of pBuffer
  DWORD   position  - position in the file stream to start reading

  
  returns: TRUE if we read the buffer at the specified location

  Note that we use IsBadWritePtr on this code which is not thread safe, so you need to protect this code
  if using in MT apps (which is usually the case)
*************************************************************************************************************************/
BOOL 
CFileStreamW::Read(
  PUCHAR  pBuffer, 
  DWORD   len, 
  DWORD   position
  )
{
  if (IsBadWritePtr((void *)pBuffer, len))
  {
    return FALSE;
  }
  if (NULL == m_pBuffer)
  {
    return FALSE;
  }

  if (len > (int)(m_dwSize - position))
  {
    return FALSE;
  }
  memcpy(pBuffer, &m_pBuffer[position], len);
  return TRUE;
}

/***********************************************************************************************************************

  SetPosition()

  Type:       Method
  Base:       this
  Scope:      Public
  Parameters:

  DWORD position - position to set the current file pointer to.

  returns: TRUE if set the file pointer, false otherwise.

*************************************************************************************************************************/
DWORD 
CFileStreamW::SetPosition(
  DWORD new_position
  )
{
  m_currPos =  SetFilePointer( m_file_handle, 0, NULL, new_position);
  
  if (m_currPos == INVALID_SET_FILE_POINTER)
  {
    return FALSE;
  }
  return TRUE;
}

/***********************************************************************************************************************

  Commit()

  Type:       Method
  Base:       this
  Scope:      Public
  Parameters:


  returns: FALSE always

  This is a stub for file commits which we'll need when we move to a cache model. This functions purpose is to 
  write the data to the storage device.

*************************************************************************************************************************/
BOOL 
CFileStreamW::Commit(
  void
  )
{
  return 0;
}


/***********************************************************************************************************************

  Grow()

  Type:       Method
  Base:       this
  Scope:      Public
  Parameters:
  DWORD dwBytesToGrow - number of bytes to grow.

  returns: FALSE always

  This is a stub for file extends the file bu dwBytes

*************************************************************************************************************************/
BOOL CFileStreamW::Grow(DWORD dwBytesToGrow)
{
  //DWORD dwOldEnd = GetFilePoonter
  if (!SetEndOfFile(m_file_handle))
  {
    return FALSE;
  }
  m_currPos = m_dwSize;

  DWORD pos = 
    SetFilePointer (m_file_handle, dwBytesToGrow, NULL, FILE_END) ; 
  
  if (INVALID_SET_FILE_POINTER == pos)
  {
    return FALSE;
  }
  
  PUCHAR pData = MyAlloc(PUCHAR, dwBytesToGrow);
  if (!Write(pData, dwBytesToGrow))
  {
    return FALSE;
  }
  return TRUE;
}

void CFileStreamW::Clear(void)
{
  //if (m_pMem)
  //{
  //  delete m_pMem;
  //}
  if (m_pBuffer)
  {
    MyFree(m_pBuffer);
  }
  Close();

}

