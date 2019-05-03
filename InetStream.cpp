//#include "MovieConverter.h"
#include ".\inetstream.h"

CInetStream::CInetStream(void)
{
}

CInetStream::~CInetStream(void)
{
}

BOOL CInetStream::Open(LPTSTR pszStream)
{
  return FALSE;
}

BOOL CInetStream::Close(void)
{
  return FALSE;
}

BOOL CInetStream::Write(PUCHAR pBuffer, DWORD dwSize, DWORD Offset)
{
  return FALSE;
}

BOOL CInetStream::Write(PUCHAR pBuffer, DWORD dwSize) 
{
  return FALSE;
}

BOOL CInetStream::ReadChar(CHAR * pch) 
{
  return FALSE;
}

BOOL CInetStream::Read(PUCHAR pBuffer, DWORD len) 
{
  return FALSE;
}

BOOL CInetStream::Read(PUCHAR pBuffer, DWORD len, DWORD position)
{
  return FALSE;
}

DWORD CInetStream::SetPosition(DWORD new_position) 
{
  return -1;
}
