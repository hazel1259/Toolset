/************************************************************************************************************************

  MODULE: ConsoleStream.cpp

  This module handles directing input and output to the console window.
  
  Author: Hazel LLoyd
  Created: Sometime in the distant past when we first got C+
  History: Created it on that day and did nothing else
  
  Notes: I've always been very fond of C standard IO (stdio). Hope no one minds I use them here
  
  
****************************************************************************************************************************/  
#include "comm_inc.h"
#include "ConsoleStream.h"

#define exit_fail() if (!m_inited) return 0;

/************************************************************************************************************************
  
  Name:       CConsoleStream
  
  Type:       Constructor
  Scope:      Public
  Base:       CStream
  Parameters: none

  This constructor sets the IO stream members to their appropriate runtime variables.
****************************************************************************************************************************/  
CConsoleStreamA::CConsoleStreamA(void)
: m_f_in(NULL)
, m_f_out(NULL)
{
  m_f_in = stdin;
  m_f_out = stdout;
}

/************************************************************************************************************************
  
  Name:       CConsoleStream
  
  Type:       Destructor
  Scope:      Public
  Base:       CStream
  Parameters: none

  This constructor sets the IO stream members to their appropriate runtime variables.
****************************************************************************************************************************/  
CConsoleStreamA::~CConsoleStreamA(void)
{
  m_f_in = (FILE *)-1;
  m_f_out =(FILE *) -1;
}


/************************************************************************************************************************
  
  Name:       Write
  
  Type:       Method
  Scope:      Public
  Base:       
  Parameters: 
  PUCHAR pBuffer - buffer to render to the display
  DWORD dwSize - size
  DWORD Offset - offset, this member is ignored

  returns: TRUE if the data was written, FALSE otherwise. GetLastError() is not set
    
****************************************************************************************************************************/  
BOOL CConsoleStreamA::Write(PUCHAR pBuffer, DWORD dwSize, DWORD Offset) 
{ 
  return Write(pBuffer, dwSize);
}

/************************************************************************************************************************
  
  Name:       Write
  
  Type:       Method
  Scope:      Public
  Base:       
  Parameters: 
  PUCHAR pBuffer - buffer to render to the display
  DWORD dwSize - size
  
  returns: TRUE if the data was written, FALSE otherwise. GetLastError() is not set
****************************************************************************************************************************/  
BOOL CConsoleStreamA::Write(PUCHAR pBuffer, DWORD dwSize)
{ 
  if (-1 != fwrite((void *)pBuffer, sizeof(char), dwSize, m_f_out))
  {
    return FALSE;
  }
  return TRUE; 
}

/************************************************************************************************************************
  
  Name:       Write
  
  Type:       Method
  Scope:      Public
  Base:       
  Parameters: 
  
  LPTSTR pszData -data to writre
  DWORD dwOffset - offset to write to (ignored)
  ...  - variable list of parameters to print out
  
  returns: TRUE if the data was written, FALSE otherwise. GetLastError() is not set

  Note: This method works like printf()
****************************************************************************************************************************/  
BOOL CConsoleStreamA::Write(LPSTR pszData,  ...)
{
  // uninitialized varaible 
  //
  va_list   vaList;             // list of arguments structure
  // initialized variables
  //
  int       cbWritten =   -1;             // bytes cbWritten to the lpBuffer
  char *    lpBuffer  =   NULL;           // lpBuffer to write
  int       increment =   256;            // lpBuffer size adjiustement
  int       size      =   increment;      // size of the lpBuffer
  va_start( vaList, pszData );
  while ( TRUE )
  {
    lpBuffer = MyAlloc(char *, size);
    if (!lpBuffer)
    {
      cbWritten = -1;
      break;
    }
    HRESULT hr = StringCchVPrintfA( lpBuffer, size, pszData, vaList );
    if (SUCCEEDED(hr))
	  {
	    break;
	  }
	  else
	  {
		  if (hr == STRSAFE_E_INSUFFICIENT_BUFFER)
		  {
		    MyFree(lpBuffer);
		    size += increment; //adjust the size of the lpBuffer
		  }
		  else
		  {
			  break;
		  }
  	}
  }
  va_end( vaList );
  // return the bytes cbWritten or failrure
  if (lpBuffer)
  {
    int len = slen(lpBuffer);
    fwrite(lpBuffer, sizeof(char), len, m_f_out);
    HeapFree(GetProcessHeap(), 0, lpBuffer);
    lpBuffer = 0;
  }
  return cbWritten;
  
  return FALSE;
}

/************************************************************************************************************************
  
  Name:       ReadLine
  
  Type:       Method
  Scope:      Public
  Base:       
  Parameters: 
  
  LPTSTR  pszData - data to write to the console
  DWORD   dwSize - siuze of the data
  
  returns: TRUE if the data was written, FALSE otherwise. GetLastError() is not set

  Note: 
  Note that this data works with the console the same way that gets() with the caveat that the data
  is not read anyfurther than dwSize bytes.
  
  dwSize bytes though are not validated!
****************************************************************************************************************************/  
BOOL CConsoleStreamA::ReadLine(
  LPSTR  pszData, 
  DWORD   dwSize
  )
{
  BOOL br = FALSE;
  int curr = 0;
  ZeroMemory(pszData, dwSize);
  if (dwSize == 0 || pszData == NULL)
  {
    return FALSE;
  }
  char ch = 0;
  while(true)
  {
    Read((unsigned char *)&ch, 1);
    if (ch == 0x0A)
    {
      break;
    }
    else
    {
      if (curr < (int)(dwSize - 1))
      {
        pszData[curr] = ch;
      }
      curr ++;
      //fwrite(&ch, sizeof(char), 1, m_f_out);
    }
  }
  
  return br;
}



/************************************************************************************************************************
  
  Name:       ReadLine
  
  Type:       Method
  Scope:      Public
  Base:       
  Parameters: 
  
  PUCHAR pch - character to recieve the character from the console
  
  returns: TRUE if te read was successful
  
  Note: This method works like __getch()
****************************************************************************************************************************/  
BOOL CConsoleStreamA::ReadChar(CHAR * pch)
{ 
  return Read((PUCHAR)pch, 1); 
}

/************************************************************************************************************************
  
  Name:       ReadLine
  
  Type:       Method
  Scope:      Public
  Base:       
  Parameters: 
  
  PUCHAR pBuffer  - buffer of data to store the entered input
  DWORD len       - length of the read buffer
  
  
  returns: TRUE if te read was successful
  
  Note: This method works like gets() with the exception the user should specify the length of the read buffer
****************************************************************************************************************************/  
BOOL CConsoleStreamA::Read(PUCHAR pBuffer, DWORD len)
{ 
  BOOL br = TRUE;
  __try
  {
    fread(pBuffer, sizeof(char), len, m_f_in);
    //return TRUE;
  }
  __except(EXCEPTION_EXECUTE_HANDLER)
  {
    br = FALSE;
  }
  return FALSE;
} 

/************************************************************************************************************************
  
  Name:       ReadLine
  
  Type:       Method
  Scope:      Public
  Base:       
  Parameters: 
  
  PUCHAR pBuffer  - buffer of data to store the entered input
  DWORD len       - length of the read buffer
  
  
  returns: TRUE if te read was successful
  
  Note: This method works like gets() with the exception the user should specify the length of the read buffer
****************************************************************************************************************************/  
BOOL CConsoleStreamA::Read(PUCHAR pBuffer, DWORD len, DWORD position)
{ 
  return Read(pBuffer, len); 
} 

/************************************************************************************************************************
  
  Name:       ReadLine
  
  Type:       Method
  Scope:      Public
  Base:       
  Parameters: 
  
  PUCHAR pBuffer  - buffer of data to store the entered input
  DWORD len       - length of the read buffer
  
  
  returns: TRUE if te read was successful
  
  Note: This method works like gets() with the exception the user should specify the length of the read buffer
****************************************************************************************************************************/  
DWORD CConsoleStreamA::SetPosition(DWORD new_position)
{ 
  return -1; 
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                  U n i c o d e    s t a r t s     h e r e
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/************************************************************************************************************************
  
  Name:       CConsoleStream
  
  Type:       Constructor
  Scope:      Public
  Base:       CStream
  Parameters: none

  This constructor sets the IO stream members to their appropriate runtime variables.
****************************************************************************************************************************/  
CConsoleStreamW::CConsoleStreamW(void)
: m_f_in(NULL)
, m_f_out(NULL)
{
  m_f_in = stdin;
  m_f_out = stdout;
}

/************************************************************************************************************************
  
  Name:       CConsoleStream
  
  Type:       Destructor
  Scope:      Public
  Base:       CStream
  Parameters: none

  This constructor sets the IO stream members to their appropriate runtime variables.
****************************************************************************************************************************/  
CConsoleStreamW::~CConsoleStreamW(void)
{
  m_f_in = (FILE *)-1;
  m_f_out =(FILE *) -1;
}


/************************************************************************************************************************
  
  Name:       Write
  
  Type:       Method
  Scope:      Public
  Base:       
  Parameters: 
  PUCHAR pBuffer - buffer to render to the display
  DWORD dwSize - size
  DWORD Offset - offset, this member is ignored

  returns: TRUE if the data was written, FALSE otherwise. GetLastError() is not set
    
****************************************************************************************************************************/  
BOOL CConsoleStreamW::Write(PUCHAR pBuffer, DWORD dwSize, DWORD Offset) 
{ 
  return Write(pBuffer, dwSize);
}

/************************************************************************************************************************
  
  Name:       Write
  
  Type:       Method
  Scope:      Public
  Base:       
  Parameters: 
  PUCHAR pBuffer - buffer to render to the display
  DWORD dwSize - size
  
  returns: TRUE if the data was written, FALSE otherwise. GetLastError() is not set
****************************************************************************************************************************/  
BOOL CConsoleStreamW::Write(PUCHAR pBuffer, DWORD dwSize)
{ 
  if (-1 != fwrite((void *)pBuffer, sizeof(char), dwSize, m_f_out))
  {
    return FALSE;
  }
  return TRUE; 
}

/************************************************************************************************************************
  
  Name:       Write
  
  Type:       Method
  Scope:      Public
  Base:       
  Parameters: 
  
  LPTSTR pszData -data to writre
  DWORD dwOffset - offset to write to (ignored)
  ...  - variable list of parameters to print out
  
  returns: TRUE if the data was written, FALSE otherwise. GetLastError() is not set

  Note: This method works like printf()
****************************************************************************************************************************/  
BOOL CConsoleStreamW::Write(LPWSTR pszData,  ...)
{
  // uninitialized varaible 
  //
  va_list   vaList;             // list of arguments structure
  // initialized variables
  //
  int       cbWritten =   -1;             // bytes cbWritten to the lpBuffer
  LPWSTR    lpBuffer  =   NULL;           // lpBuffer to write
  int       increment =   256;            // lpBuffer size adjiustement
  int       size      =   increment;      // size of the lpBuffer
  va_start( vaList, pszData );
  while ( TRUE )
  {
    lpBuffer = MyAlloc(LPWSTR, size*sizeof(WCHAR));
    if (!lpBuffer)
    {
      cbWritten = -1;
      break;
    }
    HRESULT hr = StringCchVPrintfW( lpBuffer, size, pszData, vaList );
    if (SUCCEEDED(hr))
	  {
	    break;
	  }
	  else
	  {
		  if (hr == STRSAFE_E_INSUFFICIENT_BUFFER)
		  {
		    MyFree(lpBuffer);
		    size += increment; //adjust the size of the lpBuffer
		  }
		  else
		  {
			  break;
		  }
  	}
  }
  va_end( vaList );
  // return the bytes cbWritten or failrure
  if (lpBuffer)
  {
    int len = wlen(lpBuffer);
    fwrite(lpBuffer, sizeof(char), len, m_f_out);
    HeapFree(GetProcessHeap(), 0, lpBuffer);
    lpBuffer = 0;
  }
  return cbWritten;
  
  return FALSE;
}

/************************************************************************************************************************
  
  Name:       ReadLine
  
  Type:       Method
  Scope:      Public
  Base:       
  Parameters: 
  
  LPTSTR  pszData - data to write to the console
  DWORD   dwSize - siuze of the data
  
  returns: TRUE if the data was written, FALSE otherwise. GetLastError() is not set

  Note: 
  Note that this data works with the console the same way that gets() with the caveat that the data
  is not read anyfurther than dwSize bytes.
  
  dwSize bytes though are not validated!
****************************************************************************************************************************/  
BOOL CConsoleStreamW::ReadLine(
  LPWSTR  pszData, 
  DWORD   dwSize
  )
{
  BOOL br = FALSE;
  int curr = 0;
  ZeroMemory(pszData, dwSize);
  if (dwSize == 0 || pszData == NULL)
  {
    return FALSE;
  }
  char ch = 0;
  while(true)
  {
    Read((unsigned char *)&ch, 1);
    if (ch == 0x0A)
    {
      break;
    }
    else
    {
      if (curr < (int)(dwSize - 1))
      {
        pszData[curr] = ch;
      }
      curr ++;
      //fwrite(&ch, sizeof(char), 1, m_f_out);
    }
  }
  
  return br;
}



/************************************************************************************************************************
  
  Name:       ReadLine
  
  Type:       Method
  Scope:      Public
  Base:       
  Parameters: 
  
  PUCHAR pch - character to recieve the character from the console
  
  returns: TRUE if te read was successful
  
  Note: This method works like __getch()
****************************************************************************************************************************/  
BOOL CConsoleStreamW::ReadChar(LPWSTR pch)
{ 
  return Read((PUCHAR)pch, 1); 
}

/************************************************************************************************************************
  
  Name:       ReadLine
  
  Type:       Method
  Scope:      Public
  Base:       
  Parameters: 
  
  PUCHAR pBuffer  - buffer of data to store the entered input
  DWORD len       - length of the read buffer
  
  
  returns: TRUE if te read was successful
  
  Note: This method works like gets() with the exception the user should specify the length of the read buffer
****************************************************************************************************************************/  
BOOL CConsoleStreamW::Read(PUCHAR pBuffer, DWORD len)
{ 
  BOOL br = TRUE;
  __try
  {
    fread(pBuffer, sizeof(char), len, m_f_in);
    //return TRUE;
  }
  __except(EXCEPTION_EXECUTE_HANDLER)
  {
    br = FALSE;
  }
  return FALSE;
} 

/************************************************************************************************************************
  
  Name:       ReadLine
  
  Type:       Method
  Scope:      Public
  Base:       
  Parameters: 
  
  PUCHAR pBuffer  - buffer of data to store the entered input
  DWORD len       - length of the read buffer
  
  
  returns: TRUE if te read was successful
  
  Note: This method works like gets() with the exception the user should specify the length of the read buffer
****************************************************************************************************************************/  
BOOL CConsoleStreamW::Read(PUCHAR pBuffer, DWORD len, DWORD position)
{ 
  return Read(pBuffer, len); 
} 

/************************************************************************************************************************
  
  Name:       ReadLine
  
  Type:       Method
  Scope:      Public
  Base:       
  Parameters: 
  
  PUCHAR pBuffer  - buffer of data to store the entered input
  DWORD len       - length of the read buffer
  
  
  returns: TRUE if te read was successful
  
  Note: This method works like gets() with the exception the user should specify the length of the read buffer
****************************************************************************************************************************/  
DWORD CConsoleStreamW::SetPosition(DWORD new_position)
{ 
  return -1; 
}
