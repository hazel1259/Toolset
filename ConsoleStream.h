#pragma once
#include "stream.h"
static const LPSTR  __name__A = "CONSOLE";
static const LPWSTR __name__W = L"CONSOLE";

#ifdef UNICODE
#define CConsoleStream CConsoleStreamW
#define __NAME__ __name__W
#else
#define CConsoleStream CConsoleStreamA
#define __NAME__ __name__A
#endif

class CConsoleStreamA :
  public CStreamA
{
public:
  CConsoleStreamA(void);
  ~CConsoleStreamA(void);
  BOOL  Open(LPSTR pszStream){ return TRUE; } 
  BOOL  Close(void) { return TRUE;}
  BOOL  Write(PUCHAR pBuffer, DWORD dwSize, DWORD Offset) ;
  BOOL  Write(PUCHAR pBuffer, DWORD dwSize);
  BOOL  Write(LPSTR psz, ...);
  BOOL  ReadChar(CHAR * pch);
  BOOL  Read(PUCHAR pBuffer, DWORD len); 
  BOOL  Read(PUCHAR pBuffer, DWORD len, DWORD position); 
  BOOL  ReadLine(LPSTR pszRetData, DWORD size);
  DWORD SetPosition(DWORD new_position);
  LPSTR Name() { return (LPSTR)__name__A; }
protected:
  DWORD     m_currPos;
  PUCHAR    m_pBuffer;
  BOOL      m_fError;
  FILE *    m_f_in;
  FILE *    m_f_out;
private:
  BOOL      m_inited;
};

class CConsoleStreamW :
  public CStreamW
{
public:
  CConsoleStreamW(void);
  ~CConsoleStreamW(void);
  BOOL  Open(LPWSTR pszStream){ return TRUE; } 
  BOOL  Close(void) { return TRUE;}
  BOOL  Write(PUCHAR pBuffer, DWORD dwSize, DWORD Offset) ;
  BOOL  Write(PUCHAR pBuffer, DWORD dwSize);
  BOOL  Write(LPWSTR pszw, ...);
  BOOL  ReadChar(LPWSTR pch);
  BOOL  Read(PUCHAR pBuffer, DWORD len); 
  BOOL  Read(PUCHAR pBuffer, DWORD len, DWORD position); 
  BOOL  ReadLine(LPWSTR pszRetData, DWORD size);
  DWORD SetPosition(DWORD new_position);
  LPWSTR Name() { return (LPWSTR)__name__W; }
protected:
  DWORD     m_currPos;
  PUCHAR    m_pBuffer;
  BOOL      m_fError;
  FILE *    m_f_in;
  FILE *    m_f_out;
private:
  BOOL      m_inited;
};
