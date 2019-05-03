#pragma once
#include "stream.h"
#include "tstring.h"

#ifdef UNICODE 
#define CFileStream CFileStreamW
#else
#define CFileStream CFileStreamA
#endif

class CFileStreamA : 
  public CStreamA
{
public:
  CFileStreamA(void);
  ~CFileStreamA(void);
  static BOOL  Exists(LPSTR pszStream)
  {
    BOOL br = FALSE;
    HANDLE file_handle = 
    CreateFileA(
      pszStream, 
      GENERIC_READ,
      FILE_SHARE_READ,
      NULL,
      OPEN_EXISTING,
      FILE_ATTRIBUTE_NORMAL,
      NULL
      );
    if (file_handle != INVALID_HANDLE_VALUE)
    {
      br = TRUE; 
      CloseHandle(file_handle);
    }
    else
    {
      //TraceErrorMessage(GetLastError());
    }
    return br;
  }
  BOOL    Create(LPSTR pszStream);
  BOOL    Open(LPSTR pszStream);
  BOOL    Close(void);
  BOOL    Write(PUCHAR pBuffer, DWORD dwSize, DWORD Offset);
  BOOL    Write(PUCHAR pBuffer, DWORD dwSize);
  BOOL    ReadChar(CHAR * pch);
  BOOL    Read(PUCHAR pBuffer, DWORD len);
  BOOL    Read(PUCHAR pBuffer, DWORD len, DWORD position);
  DWORD   SetPosition(DWORD new_position);
  DWORD   Size(); // { return m_dwSize; }
  BOOL    ReadPrev(PCHAR pch);
// properties of the obhect
public:
  void    SetShareMode(DWORD shareMode) 
  { 
    m_share_mode = shareMode; 
  }
  
  void    SetSecurityAttributes(LPSECURITY_ATTRIBUTES securityAttributes) 
  { 
    m_security_attributes = securityAttributes; 
  }
  
  void    SetOpenFlags(DWORD openFlags) 
  { 
    m_open_flags = openFlags; 
  }
  
  void SetDesiredAccess(DWORD desiredAccess) 
  {
    m_desired_access = desiredAccess;
  }

  void SetOpenAttributes(DWORD openAttributes)
  {
    m_open_attributes = openAttributes;
  }

  void SetFileTemplate(HANDLE hTemplate) 
  {
    m_file_template = hTemplate; 
  }

  LPSTR  Name(); // {  }
private:
  HANDLE                  m_file_handle;
  DWORD                   m_dwSize;
  PUCHAR                  m_pBuffer;
  DWORD                   m_open_attributes;
  DWORD                   m_open_flags;
  DWORD                   m_desired_access;
  LPSECURITY_ATTRIBUTES   m_security_attributes;  //CMem *m_pMem;
  HANDLE                  m_file_template;
  DWORD                   m_share_mode;
  BOOL                    m_fError;
  TStringA *              m_StreamName;
  DWORD                   m_sizeHigh;
  LONGLONG                m_hugeFileSize;
private:
  BOOL                    OpenFile(LPSTR pszFile);
public:
  BOOL Commit(void);
  BOOL Grow(DWORD dwBytes);
  void Clear(void);
};

class CFileStreamW : 
  public CStreamW
{
public:
  CFileStreamW(void);
  ~CFileStreamW(void);
  static BOOL  Exists(LPWSTR pszStream)
  {
    BOOL br = FALSE;
    HANDLE file_handle = 
    CreateFileW(
      pszStream, 
      GENERIC_READ,
      FILE_SHARE_READ,
      NULL,
      OPEN_EXISTING,
      FILE_ATTRIBUTE_NORMAL,
      NULL
      );
    if (file_handle != INVALID_HANDLE_VALUE)
    {
      br = TRUE; 
      CloseHandle(file_handle);
    }
    else
    {
      //TraceErrorMessage(GetLastError());
    }
    return br;
  }
  BOOL    Create(LPWSTR pszStream);
  BOOL    Open(LPWSTR pszStream);
  BOOL    Close(void);
  BOOL    Write(PUCHAR pBuffer, DWORD dwSize, DWORD Offset);
  BOOL    Write(PUCHAR pBuffer, DWORD dwSize);
  BOOL    ReadChar(CHAR * pch);
  BOOL    Read(PUCHAR pBuffer, DWORD len);
  BOOL    Read(PUCHAR pBuffer, DWORD len, DWORD position);
  DWORD   SetPosition(DWORD new_position);
  DWORD   Size(); // { return m_dwSize; }
  BOOL    ReadPrev(PCHAR pch);
// properties of the obhect
public:
  void    SetShareMode(DWORD shareMode) 
  { 
    m_share_mode = shareMode; 
  }
  
  void    SetSecurityAttributes(LPSECURITY_ATTRIBUTES securityAttributes) 
  { 
    m_security_attributes = securityAttributes; 
  }
  
  void    SetOpenFlags(DWORD openFlags) 
  { 
    m_open_flags = openFlags; 
  }
  
  void SetDesiredAccess(DWORD desiredAccess) 
  {
    m_desired_access = desiredAccess;
  }

  void SetOpenAttributes(DWORD openAttributes)
  {
    m_open_attributes = openAttributes;
  }

  void SetFileTemplate(HANDLE hTemplate) 
  {
    m_file_template = hTemplate; 
  }

  LPWSTR  Name(); // {  }
private:
  HANDLE                  m_file_handle;
  DWORD                   m_dwSize;
  PUCHAR                  m_pBuffer;
  DWORD                   m_open_attributes;
  DWORD                   m_open_flags;
  DWORD                   m_desired_access;
  LPSECURITY_ATTRIBUTES   m_security_attributes;  //CMem *m_pMem;
  HANDLE                  m_file_template;
  DWORD                   m_share_mode;
  BOOL                    m_fError;
  TStringW *               m_StreamName;
  DWORD                   m_sizeHigh;
  LONGLONG                m_hugeFileSize;
private:
  BOOL                    OpenFile(LPWSTR pszFile);
public:
  BOOL Commit(void);
  BOOL Grow(DWORD dwBytes);
  void Clear(void);
};
