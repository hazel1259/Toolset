/***********************************************************************************************************
  MODULE: MISC.CPP

  This file contains miscellaneous worker routines we use everywhere

  Author: HAZELL:
  History: 9-22-04 Hazell created it and added functions

*************************************************************************************************************/  
#include "comm_inc.h"
static const char __crlf[] = "\r\n";
static int g_line_len = 0;

BOOL DebugOut(char *, ...);

typedef struct  _LIST
{
    _LIST * pNext;
    _LIST * pPrev;
    void * pData;
}LIST;


BOOL __stdcall Alloc(PVOID * ppv, SIZE_T size)
{
	BOOL br = FALSE;
	if (ppv)
	{
		PVOID pv;
		if (!*ppv)
		{
			pv = MyAlloc(PVOID, size);
			if (pv)
			{
				*ppv = pv;
				br = TRUE;
			}
		}
		else
		{
			__try
			{
				pv = *ppv;
				PVOID ptmp = MyRealloc(PVOID, pv, size);
				if (ptmp)
				{
					*ppv = ptmp;
					br = TRUE;
				}
				else
				{
					ptmp = MyAlloc(PVOID, size);
					if (ptmp)
					{
						memcpy(ptmp, pv, size);
						*ppv = ptmp;
						br = TRUE;
					}
				}
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
				br = FALSE;
#ifdef DEBUG
			DebugOut("An exception %8.08x freeing address %8.08x\r\n", GetExceptionCode(), *ppv);
#endif
			}

		}
	}
	return br;
}

BOOL __stdcall Free(PVOID * ppv)
{
	BOOL br = FALSE;
	if (ppv && * ppv)
	{
		__try
		{
			br = MyFree(*ppv);
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			br = FALSE;
#ifdef DEBUG
			DebugOut("An exception %8.08x freeing address %8.08x\r\n", GetExceptionCode(), *ppv);
#endif
		}
	}
	return br;
}
#ifdef USE_LIST_2_NORMAL

#else
LPSTR FindCharsA(LPSTR pszData, LPSTR pszChars)
{
    LPSTR psz = pszData;
    while(*psz)
    {
        LPSTR psz2 = pszChars;
        while(*psz2)
        {
            if (*psz2 == *psz)
            {
                return psz;
            }
            psz2++;
        }
        psz ++;
    }
    return NULL;
}

BOOL _normalizeA(LPSTR pszNormal, UINT * pOffsetNormal, UINT currLen, LPSTR * ppszCurr)
{
    BOOL    br          =   FALSE;
    UINT    offset      =   *pOffsetNormal;
    LPSTR   tokens      =   "/\\";
    LPSTR   pszCurr     =   *ppszCurr;
    LPSTR   p           =   FindCharsA(pszCurr, tokens);
    if (p)        
    {
        p ++;
        int        len         = (int)(p - pszCurr);
        LPSTR       tmpNorm     = pszNormal + offset;
        HRESULT     hr          = StringCchCopyNA(tmpNorm, len + 1, pszCurr, len);

        if (SUCCEEDED(hr))
        {
            br = TRUE;
            offset += len;
        }
        pszCurr = p;
    }
    // when TRUE, change the offset and pointers
    if (br)
    {
        *ppszCurr = pszCurr;
        *pOffsetNormal = offset;
    }
    return br;
}

BOOL NormalizePathA(LPSTR pszPath, LPSTR pszNormal, UINT uLenNormal)
{
    LPSTR   pszDots     =   "..";
    BOOL br = FALSE;
    LPSTR pszCurr = pszPath;
    UINT offset = 0;
    UINT remaining = uLenNormal;
    while(TRUE)
    {
        if (!_normalizeA(pszNormal, &offset, remaining, &pszCurr))
        {
            break;
        }
        
        // check to see if we need to move back
        if (strncmp(pszCurr, pszDots, 2) == 0)
        {
            pszCurr += 2;
            if (*pszCurr == '/' || *pszCurr == '\\')
            {
                pszCurr++;
            }
            LPSTR ptmp = pszNormal + (offset - 1);
            if (*ptmp == '\\' || *ptmp == '/')
            {
                offset --;
                remaining ++;
                ptmp --;
            }
            while(TRUE)
            {
                if (*ptmp == '\\' || *ptmp == '/' || *ptmp == '\0')
                {
                    break;
                }
                ptmp --;
                offset --;
                remaining ++;
            }
            *(pszNormal + offset) = '\0';
            br = TRUE;
        }
        else
        {
            remaining -= offset;
        }
        if (!*pszCurr)
        {
            br = TRUE;
            break;
        }
        if (offset >= uLenNormal)
        {
            br = TRUE;
            break;
        }
    }
    return br;
}


LPWSTR FindCharsW(LPWSTR pszwData, LPWSTR pszwChars)
{
    LPWSTR pszw = pszwData;
    while(*pszw)
    {
        LPWSTR pszw2 = pszwChars;
        while(*pszw2)
        {
            if (*pszw2 == *pszw)
            {
                return pszw;
            }
            pszw2++;
        }
        pszw ++;
    }
    return NULL;
}

BOOL _normalizeW(LPWSTR pszwNormal, UINT * pOffsetNormal, UINT currLen, LPWSTR * ppszwCurr)
{
    BOOL    br          =   FALSE;
    UINT    offset      =   *pOffsetNormal;
    LPWSTR   tokens      =   L"/\\";
    LPWSTR   pszwCurr     =   *ppszwCurr;
    LPWSTR   p           =   FindCharsW(pszwCurr, tokens);
    if (p)        
    {
        p ++;
        int        len         = ((int)(p - pszwCurr))/sizeof(WCHAR);
        LPWSTR       tmpNorm     = pszwNormal + offset;
        HRESULT     hr          = StringCchCopyNW(tmpNorm, len + 1, pszwCurr, len);

        if (SUCCEEDED(hr))
        {
            br = TRUE;
            offset += len;
        }
        pszwCurr = p;
    }
    // when TRUE, change the offset and pointers
    if (br)
    {
        *ppszwCurr = pszwCurr;
        *pOffsetNormal = offset;
    }
    return br;
}

BOOL NormalizePathW(LPWSTR pszwPath, LPWSTR pszwNormal, UINT uLenNormal)
{
    LPWSTR   pszwDots     =   L"..";
    BOOL br = FALSE;
    LPWSTR pszwCurr = pszwPath;
    UINT offset = 0;
    UINT remaining = uLenNormal;
    while(TRUE)
    {
        if (!_normalizeW(pszwNormal, &offset, remaining, &pszwCurr))
        {
            break;
        }
        
        // check to see if we need to move back
        if (wcsncmp(pszwCurr, pszwDots, 2) == 0)
        {
            pszwCurr += 2;
            if (*pszwCurr == L'/' || *pszwCurr == L'\\')
            {
                pszwCurr++;
            }
            LPWSTR ptmp = pszwNormal + (offset - 1);
            if (*ptmp == L'\\' || *ptmp == L'/')
            {
                offset --;
                remaining ++;
                ptmp --;
            }
            while(TRUE)
            {
                if (*ptmp == '\\' || *ptmp == '/' || *ptmp == '\0')
                {
                    break;
                }
                ptmp --;
                offset --;
                remaining ++;
            }
            *(pszwNormal + offset) = L'\0';
            br = TRUE;
        }
        else
        {
            remaining -= offset;
        }
        if (!*pszwCurr)
        {
            br = TRUE;
            break;
        }
        if (offset >= uLenNormal)
        {
            br = TRUE;
            break;
        }
    }
    return br;
}
//UINT    ccConverted     = 0;
//LPWSTR  pszw            = pUnicode;
//PUCHAR  pch             = pUtf8Stream;
//UINT    num_bytes       = 1;
//UCHAR    utf8Bom[3]      = {0xef, 0xbb, 0xbf};
//
//if (utf8Len > 3)
//{
//    // check for BOM
//    if (memcmp(pUtf8Stream, utf8Bom, 3) == 0)
//    {
//        // goood, what now?
//        pch += 3;
//    }
//}
//
//while ((DWORD)(pch - pUtf8Stream) <  utf8Len)
//{
//    if ((*pch == 192 || *pch == 193) || (*pch >=245 && *pch <= 255))
//    {
//        return -1; // these are invalid characters;
//    }
//    if(!(*pch & 0x80))
//    {

//        if (pszw)
//        {
//            *pszw = (WCHAR)*pch;
//            pszw ++;
//        }
//        pch ++;
//        ccConverted++;
//    }
//    else
//    {
//    // check to see if this is a leading byte
//        UCHAR leadbyte = (*pch >> 4);
//        WCHAR w = *pch & 0xF;    // mask off the high 4 bits
//        w = w << 6;             // now you are aligned on the next bits

//        switch (leadbyte)
//        {
//            case 0x0C: // 1100
//            {
//                num_bytes =  2;
//            }
//            break;
//            case 0x0E: // 1110
//            {
//                num_bytes = 3;
//            }
//            break;
//            case 0x0F: // 1110
//            default:
//                // num_bytes = 4;
//                // not supported currently
//            return -1; // error or invalid entry
//        }
//        pch ++;
//        num_bytes --;
//        int i = num_bytes;
//        do
//        {
//            w |= *pch & 0x3f;
//            w = (i - 1) > 0 ? w << 6 : w;
//            i--;
//            pch ++;
//            if ((DWORD)(pch - pUtf8Stream) == utf8Len && i > 0)
//            {
//                return -1; // error encountered oin the stream.
//            }
//        } while (i > 0);

//        if (pszw)
//        {
//            *pszw = w;
//            pszw++;
//        }
//        ccConverted++;
//    }
//}

//return ccConverted; // number of chars converted successfully

/**********************************************************************************************

Function: ConvertToUnicode()

This function converts a CHAR to a WCHAR using MultiByteToWideChar()

IN LPSTR psz - pointer to a NULL terminated string of ANSI chars
IN LPWSTR pwz - pointer to a wide char suffiently large to be converted

returns: TRUE if converted. FALSE otherwise
***********************************************************************************************/

BOOL
ConvertToUnicode(
LPCSTR psz,
PWSTR pwz
)
{
    if (!psz || !pwz)
    {
        return FALSE;
    }
    DWORD dwRet =
        MultiByteToWideChar(CP_ACP,
        MB_ERR_INVALID_CHARS,
        psz,
        -1,
        pwz,
        (int)strlen(psz)
        );
    return (dwRet == 0);
}


/**********************************************************************************************

Function: ConvertToAnsi()

This function converts a LPWSTR to a LPSTR using WideCharToMultiByte()

IN LPSTR psz - pointer to a NULL terminated string of ANSI chars
IN LPWSTR pwz - pointer to a wide char suffiently large to be converted

returns: TRUE if converted. FALSE otherwise
***********************************************************************************************/

BOOL
ConvertToAnsi(
LPWSTR pwz,
LPSTR psz
)
{
    DWORD len = 0;
    //BOOL br = FALSE;
    if (!psz || !pwz)
    {
        return FALSE;
    }
    //__try
    //{
    //  sprintf(psz, "%ls", pwz);
    //  br = TRUE;
    //}
    //__except(EXCEPTION_EXECUTE_HANDLER)
    //{
    //  br = FALSE;
    //  *psz = 0;
    //}
    len = WideCharToMultiByte(
        CP_ACP,
        0,
        pwz,
        -1,
        psz,
        (int)wcslen(pwz) + 1,
        NULL,
        NULL
        );

    if (len == 0)
    {
        return FALSE;
    }
    return TRUE;
}

BOOL 
ConvertUnicodeToUTF8(
    LPWSTR pszUnicode, 
    UINT uniLen, 
    PUCHAR pUtf8Stream, 
    UINT streamLen
    )
{
    UINT ret = WideCharToMultiByte(CP_UTF8, 0, pszUnicode, uniLen, (LPSTR)pUtf8Stream, streamLen, NULL, NULL);
    return ret == 0 ? FALSE : TRUE;
}
// converts UTF8 into regular bytes.
BOOL 
ConvertUTF8ToUnicode(
    PUCHAR pUtf8Stream, 
    UINT utf8Len, 
    LPWSTR pUnicode, 
    UINT uniLen
    )
{
    UINT ret = 
        MultiByteToWideChar(
            CP_UTF8, 
            0, 
            (LPSTR)pUtf8Stream, 
            utf8Len, 
            pUnicode, 
            uniLen
            );
    return ret == 0 ? FALSE : TRUE;
}

BOOL 
GetCenterWindow(
    HWND hwnd, 
    POINT * pLocation, 
    SIZE * pSize
    )
{
    BOOL br = FALSE;
    if (hwnd == 0)
    {
        // use the desktop dimensions of the screen
    
        pSize->cx = GetSystemMetrics(SM_CXSCREEN); 
        pSize->cy = GetSystemMetrics(SM_CYSCREEN); 
    
        int width = pSize->cx*3/4;
        int height = pSize->cy*3/4;
        int x= ( pSize->cx - width)/2;
        int y = ( pSize->cy - height)/2;
        pLocation->x = x;
        pLocation->y = y;
        br = TRUE;
    }
    else
    {
        if (IsWindow(hwnd))
        {
            RECT rc;
            if (GetWindowRect(hwnd, &rc))
            {
                pSize->cx = rc.right - rc.left;
                pSize->cy = rc.bottom - rc.top;
                int width = pSize->cx*3/4;
                int height = pSize->cy*3/4;
                int x= ( pSize->cx - width)/2;
                int y = ( pSize->cy - height)/2;
                pLocation->x = x;
                pLocation->y = y;
                br = TRUE;
            }
        }

    }
    return br;
}

#endif

int Round(float f)
{
	float d;
	int x = (int)f;
	d = f - (float)x;
	if (d >= 0.5)
	{
		x++;
	}
	return x;
}
void SetLineLen(int len)
{
  if (len > 0 && len < 64)
  {
    g_line_len = len;
  }
}
unsigned char  hex_table[] = {
'0',
'1',
'2',
'3',
'4',
'5',
'6',
'7',
'8',
'9',
'a',
'b',
'c',
'd',
'e',
'f'
};


LPTSTR
GetPathOfFile(
  LPTSTR pszFileName
  )
{
  LPTSTR      pszPath   = NULL;
  DWORD       dwSize    = 0;
  DWORD       dwLen     = 0;
  //
  // Path to get , check here in case we make utility out of this
  //
  // try the first time
  dwLen = GetFullPathName(pszFileName, dwSize, pszPath, NULL);
  // allocate a buffer for it
  pszPath = MyAlloc(LPTSTR, dwLen);//(LPTSTR)CVSAlloc(dwLen);
  // check to see if we alloc'd OK
  if(!pszPath)
  {
    goto DONE;
  }
  // assign the size
  dwSize = dwLen;
  // assign the length
  dwLen = GetFullPathName(pszFileName, dwSize, pszPath, NULL);
  // heck the return
  if (dwSize < dwLen)
  {
    MyFree(pszPath);
    pszPath = NULL;
    goto DONE;
  }

DONE:
  return pszPath;
}

HFONT 
GetFontW(
  LPWSTR pszFont, 
  int height, 
  LPWSTR style
  )
{
  BOOL bItalic = FALSE;
  int FontWeight = FW_NORMAL;
  if (_wcsicmp(style, L"Bold") == 0)
  {
    FontWeight = FW_BOLD;
  }
  else
  {
    if (_wcsicmp(style, L"Italic") == 0)
    {
      bItalic = TRUE;
      //FontWeight = FW_BOLD;
    }
    else
    {
      if (_wcsicmp(style, L"Bold Italic") == 0)
      {
        bItalic = TRUE;
        FontWeight = FW_BOLD;
        //FontWeight = FW_BOLD;
      }
    }


  }
  HFONT hf = CreateFontW(
    -1*height, //14,
    0,
    0,
    0,
    FontWeight,
    bItalic,
    FALSE,
    FALSE,
    DEFAULT_CHARSET,
    OUT_DEFAULT_PRECIS,
    CLIP_DEFAULT_PRECIS,
    PROOF_QUALITY,
    FF_DONTCARE | DEFAULT_PITCH,
    pszFont
    );
  return hf;
}

HFONT 
GetFontA(
  LPSTR pszFont, 
  int height, 
  LPSTR style
  )
{
  BOOL bItalic = FALSE;
  int FontWeight = FW_NORMAL;
  if (_stricmp(style, "Bold") == 0)
  {
    FontWeight = FW_BOLD;
  }
  else
  {
    if (_stricmp(style, "Italic") == 0)
    {
      bItalic = TRUE;
      //FontWeight = FW_BOLD;
    }
    else
    {
      if (_stricmp(style, "Bold Italic") == 0)
      {
        bItalic = TRUE;
        FontWeight = FW_BOLD;
        //FontWeight = FW_BOLD;
      }
    }


  }
  HFONT hf = CreateFont(
    -1*height, //14,
    0,
    0,
    0,
    FontWeight,
    bItalic,
    FALSE,
    FALSE,
    DEFAULT_CHARSET,
    OUT_DEFAULT_PRECIS,
    CLIP_DEFAULT_PRECIS,
    PROOF_QUALITY,
    FF_DONTCARE | DEFAULT_PITCH,
    pszFont
    );
  return hf;
}


/***********************************************************************************************************
  Function: DoSocketError()

  int error - error recieved on the socket request, This text was taken verbatim from MSDN

  returns: no return

  This is a message error handler informing us of the type of error.

*************************************************************************************************************/  
void 
DoSocketError(
  int Error
  )
{
  switch(Error)
  {
    case WSANOTINITIALISED: 
      printf(" A successful case WSAStartup call must occur before using this function. \n"); 
    break;
    case WSAENETDOWN: 
      printf(" The network subsystem has failed. \n"); 
    break;
    case WSAEACCES: 
      printf(" Attempt to connect datagram socket to broadcast address failed because setsockopt option SO_BROADCAST is not enabled. \n"); 
    break;
    case WSAEADDRINUSE: 
      printf(" A process on the computer is already bound to the same fully-qualified address and the socket has not been marked to allow address reuse with SO_REUSEADDR.\nFor example, the IP address and port are bound in the af_inet case). \nSee the SO_REUSEADDR socket option under setsockopt. \n");
    break;
    case WSAEADDRNOTAVAIL : 
      printf("The specified address is not a valid address for this computer. \n"); 
    break;
    case WSAEFAULT : 
      printf("The name or namelen parameter is not a valid part of the user address space,\nthe namelen parameter is too small,\nthe name parameter contains an incorrect address format for the associated address family,\nor the first two bytes of the memory block specified by name does not match the address family associated with the socket descriptor s. \n"); 
    break;
    case WSAEINPROGRESS : 
      printf("A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function. \n"); 
    break;
    case WSAEINVAL : 
      printf("The socket is already bound to an address. \n"); 
    break;
    case WSAENOBUFS : 
      printf("Not enough buffers available, too many connections. \n"); 
    break;
    case WSAENOTSOCK : 
      printf("The descriptor is not a socket. \n"); 
    break;
    
    case WSAEOPNOTSUPP :
      printf("The referenced socket is not of a type that supports the listen operation. \n");
    break;
    
    case 11001:
      printf("Authoritative answer host not found.\n");
    break;
    
    case WSAEMFILE:
      printf("No more socket descriptors are available. \n");
    break;
    
    case WSAEISCONN :
      printf("The socket is already connected. \n");
    break;
    
    case WSATRY_AGAIN: 
      printf("WSATRY_AGAIN Nonauthoritative host not found, or server failure. \n");
    break;
    case WSANO_RECOVERY :
      printf("A nonrecoverable error occurred. \n");
    break;
    case WSANO_DATA :
      printf("Valid name, no data record of requested type. \n");
    break;
    case WSAEINTR :
      printf("A blocking Windows Socket 1.1 call was canceled through WSACancelBlockingCall. \n");
    break;

    default:
      printf("Unknown error %d\n", Error);
    break;
  }
}
void DumpByte(BYTE b, BOOL console)
{
  int f = 0x80;
  for (int i = 0; i < 8; i ++)
  {
    if (i == 4)
      printf(" ");
    if (f & b)
    {
      if (console)
      {
        printf("1");
      }
      else
      {
        OutputDebugString("1");
      }
    }
    else
    {
      if (console)
      {
        printf("0");
      }
      else
      {
        OutputDebugString("0");
      }
    }
    f = f >> 1;
  }
}

void DumpArray(PUCHAR p, int size, BOOL console)
{
  int i= 0;
  while(i < size)
  {
    DumpByte(p[i], console);
    if (console)
    {
      printf(" ");
      if ((i + 1) % 4 == 0)
      {
        printf("\r\n");
      }
    }
    else
    {
      OutputDebugString(" ");
      if ((i + 1) % 4 == 0)
      {
        OutputDebugString("\r\n");
      }
    }
    i++;
  }
  OutputDebugString("\r\n");
}

void DumpLine(unsigned char * pdata, int size)
{
  for (int i = 0; i < size; i ++)
  {
    printf("%2.02x ", pdata[i]);
  }
  
  if (size < 8)
  {
    int buffer = 8 - size;
    for(int i = 0; i < buffer; i ++)
    {
      printf("   ");
    }
  }
  printf("    ");  
  for (int i = 0; i < size; i ++)
  {
		if(pdata[i] < 32 || pdata[i] > 126)
    {
      printf(".");
    }
    else
    {
      printf("%c", pdata[i]);
    }
  }
  printf("\n");
}

void DumpData(unsigned char * pdata, int size)
{
  BOOL bDone = FALSE;
  int i = 0;
  int remaining = size;
  int len = 16;
  while(!bDone)
  {
    if (remaining < 16)
    {
      bDone = TRUE;
      len = remaining;
    }
    else
    {
      len = 16;
    }
    DumpLine(&pdata[i], len);
    remaining -= 16;
    i += len;
  }
}
/************************************************************************************************

  Function: LogData()

  This function logs data to the log file. The log file is created globally and is dependent upon
  the type of test executed. 

  IN LPTSTR pszArgs           - pointer to string containing data and args
  IN LPTSTR pszFileName       - name of the log file to use which may have no meaning if we are logging to a server 
  IN ULONG LogType            - Logging method to use (This will be defined somewhere else)

  returns: The count of characters actually added to the log, -1 on error

**************************************************************************************************/
int
Message(
  char *  pszData, 
  ...
  )
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
  // if we cannot take the critical section object, then we are doomed.
  //
  //if (!TakeLock())
  //{
   // goto DONE;
  //}
  // wrap it in a try/except
  //
  // initialize the vaList 
  va_start( vaList, pszData );
  // if cbWritten is -1 return
  while ( TRUE )
  {
    lpBuffer = MyAlloc(char *, size);
    if (!lpBuffer)
    {
      cbWritten = -1;
      break;
    }
    // if cbWritten returns here as -1 means there is not any lpBuffer
    HRESULT hr = StringCchVPrintf( lpBuffer, size, pszData, vaList );
    if (!SUCCEEDED(hr))
	{
		if (hr == STRSAFE_E_INSUFFICIENT_BUFFER)
		{
		  HeapFree(GetProcessHeap(), 0, lpBuffer);
		  lpBuffer = NULL;
		  size += increment; //adjust the size of the lpBuffer
		}
		else
		{
			break;
		}
	}
  }
  // terminate string formatting
  va_end( vaList );
  // if lpBuffer is valid, then free it
  //ReleaseLock();
//DONE:
  // return the bytes cbWritten or failrure
  if (lpBuffer)
  {
    MessageBox(NULL, lpBuffer, "Information", MB_OK);
    //OutputDebugString(lpBuffer);
    HeapFree(GetProcessHeap(), 0, lpBuffer);
    lpBuffer = 0;
    //MyFree((PVOID *)&lpBuffer);
  }
  return cbWritten;
}
/*
char 1 = 49
char 2 = 50
char 3 = 51
char 4 = 52
char 5 = 53
char 6 = 54
char 7 = 55
char 8 = 56
char 9 = 57
char a = 97
char b = 98
char c = 99
char d = 100
char e = 101
char f = 102
*/

char * afromhex(unsigned char val)
{
  static char ret[3];
  ret[2] = '\0';
  unsigned char lo_nibble = 0;
  unsigned char hi_nibble = 0;
  lo_nibble = val >> 4;
  hi_nibble = val & 0xf;
  ret[0] = hex_table[lo_nibble];
  ret[1] = hex_table[hi_nibble];
  return ret;
}


BOOL 
PrintHexLine(
    unsigned char * pdata, 
    int             size, 
    int             line_len, 
    PRINTPROC       print
  )
{
  // on the heap
  static int    in_len        = 0;
  static char*  print_data    = NULL; //
  // local
  int           bytes         = 0;
  int           total_len     = 0;
  // worker
  char*         p; //             = print_data;
  char *        fill_data     = "   ";
  char *        line_fill     = "    ";
  char *        hex_str       = NULL;
  
  if (in_len < line_len)
  {
    in_len = line_len;
    if (print_data)
    {
      MyFree(print_data);
    }
    total_len = line_len*2 + 8 + line_len*2 + 8 + 1;
    print_data = MyAlloc(char *, total_len);
    if (!print_data)
    {
      print("#ERROR#", slen("#ERROR#"));
      return TRUE;
    }
  }
  else
  {
    ZeroMemory(print_data, total_len);
  }
  
  p = print_data;  
  
  for (int i = 0; i < line_len; i ++)
  {
    hex_str = afromhex(pdata[i]);
    memcpy(p, hex_str, 2);
    p+= 2;
    *p = ' ';
    p ++;
    bytes+=3;
  }
  
  if (size < line_len)
  {
    int buffer = line_len - size;
    for(int i = 0; i < buffer; i ++)
    {
      memcpy(p, fill_data, 3);
      bytes+= 3;
      p+= 3;
    }
  }
  
  memcpy(p, line_fill, 4);
  bytes+= 4;
  p += 4;
  for (int i = 0; i < line_len; i ++)
  {
		if(pdata[i] < 32 || pdata[i] > 126)
    {
      *p = '.';
    }
    else
    {
      *p = pdata[i];
    }
    p++;
    bytes++;
  }
  
  memcpy(p, __crlf, 2);
  bytes += 2;
  p += 2;
  *p = '\0';
  return print(print_data, slen(print_data));
 
}

void PrintHexData(unsigned char * pdata, int size, int line_len, PRINTPROC printer)
{
  BOOL bDone    = FALSE;
  int i         = 0;
  int remaining = size;
  int len       = line_len;
  unsigned char * p = pdata;
  
  while(!bDone)
  {
    if (remaining < line_len)
    {
      bDone = TRUE;
      len = remaining;
    }
    else
    {
      len = line_len;
    }
    if (!bDone)
    {
      bDone = PrintHexLine(p, remaining, len, printer);
    }
    remaining -= len;
    p += len;
    i += len;
  }
}

#ifndef GetWindowInstance
HINSTANCE GetWindowInstance(HWND hwnd)
{
  return (HINSTANCE)(LONG)GetWindowLongPtrA(hwnd, GWLP_HINSTANCE);
}
#endif
/**************************************************************************************************************

  Function: Factorial()

  This function performs the factorial of a number

  in __int64 n - number to take factorial of

  returns: __int64 containing the factorial
**************************************************************************************************************/

__int64 Factorial(__int64 n)
{
  __int64 ret = (__int64)1;
  for (__int64 i = 1; i <= n; i ++)
  {
    ret = i*ret;
  }
  return ret;
}

/**************************************************************************************************************

  Function: Combination()

  This function performs the factorial of a number

  in __int64 n - number to take combinations of
  in __int64 r - number of times to take a combination of n at
  returns: __int64 containing the combinations. This is done by the formula n!/r!(n-r)!
**************************************************************************************************************/
__int64 Combination(__int64 n, __int64 r)
{
  __int64 ret = 0;
  if (n > r)
  {
    ret = (Factorial(n)/(Factorial(r)*Factorial(n - r)));
  }
  return ret;
}
/********************************************************************************************************

  Function: _hexatoi()
  
  Converts a string that is hex based to an integer
  
  IN const char * input - pointer to the hex data string to convert to an interger
  
  The formula is as follows

  String s of the form 0 - 9 and A - F, of n letters, such as aeff will be represented decimally by

  returns - the integer value of the hex string or -1 on error
  
*********************************************************************************************************/
int 
_hexatoi(
  const char * input
  )
{
  int i; 
  int retval  = 0; 
  int radix   = 1;
  int t       = 0;
  // compute the effective length of the string so we know where to start. We start at the 0th
  // position which is actually the far right.
  i = (int)strlen(input) -1;
  // while we are on the actual hex positions in the string, we compute each decimal value
  while (i >= 0)
  {
    // if between 0 and 9 then we beed to subtract the '0' value to get the decimal of this 
    // position
    if (input[i] >= '0' && input[i] <= '9')
    {
      t = (input[i] - '0');
    }         
    else
    {
      // otherwise, we need to subtract the 'a' and add 10 to it so that it fits in the 16 size
      //
      if ((input[i] | 0x20) >= 'a' && (input[i] | 0x20) <= 'f')
      {
        t = ((input[i] - 'a') + 10);
      }
      // invalid character so, return value should be -1 according to doc..
      else
      {
        retval = -1;
        break;
      }
    }
    // incremment the return value by the current value by t times the current radix position
    // the radix position will be 16 exp position in the array
    // so position 1 = 1, position 2 = 16, 3 = 256, 4 = 4096, ..etc
    retval += (t * radix);
    radix *= 0x10;
    // go to the next value
    //
    i--;
  }
  //return what we get
  return retval;
} 


/*************************************************************************************************************

  Function: Divide()

  This function divides to values and rounds the value up or down

  in Val1 - numerator
  in denominator

  returns: The result of the division
**************************************************************************************************************/

int 
Divide(
  int iVal1, 
  int iVal2
  )
{
  float     remainder   = 0;
  float     fresult     = 0;
  int       result      = 0;
  float     f1          = (float)iVal1;
  float     f2          = (float)iVal2;
  
  
  result    = iVal1/iVal2;
  fresult   = f1/f2;
  remainder = fresult - (float)result;
  
  if (remainder >= 0.5)
  {
    result++;
  }
  
  return result;
}


BOOL 
IsIntegerValue(
  char * val
  )
{
  BOOL br = TRUE;
  char * psz = strchr(val, '.');
  if (!psz)
  {
    //TraceOut(USE_CONSOLE|USE_DEBUGGER, NULL,"%s does not appear to be a real number\n", val);
    goto DONE;
  }
  // skip the dot
  psz++;
  // verify it is an integer (it has only 0's). Anything else is real
  do
  {
    if (*psz != '0')
    {
      br = FALSE;
      goto DONE;
    }
    psz++;
  }while(*psz);
  br = TRUE;
DONE:
  return br;
}

BOOL 
IsInteger(
  char * pszNumber
  )
{
  char * p = pszNumber;
  if (!p)
  {
    return FALSE;
  }
  if (!*p)
  {
    return FALSE;
  }
  while(*p)
  {
    if (*p >= '0' && *p <= '9')
    {
      p++;
      continue;
    }
    else
    {
      return FALSE;
    }
  }
  return TRUE;
}

char * 
StripChar(
  char *psz, 
  char ch
  )
{
  char * psztmp = psz;
  int size = (int)MyHeapSize(psz);
  char * psznew = MyAlloc(LPTSTR, size);
  int i = 0;
  //TraceOut(USE_CONSOLE|USE_DEBUGGER, NULL,"Size of pszNew is %d\n", size);
  //TraceOut(USE_CONSOLE|USE_DEBUGGER, NULL,"Strlen of psz is %d\n", strlen(psz));
  //TraceOut(USE_CONSOLE|USE_DEBUGGER, NULL,"psz is %s\n", psz);
  if (!psznew)
  {
    return FALSE;
  }
  do
  {
    if (*psztmp != ch)
    {
      psznew[i] = *psztmp;
      i++;
    }
    psztmp++;
  }while(*psztmp);

  //psznew[i] = '\0';
  //strcpy(psz, psznew);
  StringCchCopy(psz, size, psznew);
  MyFree(psznew);
  return psz;
}

char * 
GetStrChar(
  char * pszString, 
  char ch
  )
{
  int len = 1;
  char * pszret = 0;
  char * psz = pszString;
  do
  {
    len ++;
    psz++;
  }while(*psz != ch);
  pszret = MyAlloc(char *, len + 1);
  if (pszret)
  {
    strncpy_s(pszret, len, pszString, len);
    pszString[len] = '\0';
  }
  return pszret;
}
/*
BOOL IsOperator(char ch)
{
  BOOL br = FALSE;
  //char * psz = Operators;
  for (int i = 0; i < strlen(Operators); i++)
  {
    if (ch == Operators[i])
    {
      br = TRUE;
      break;
    }
  }
  return br;
}
*/

char * NewString(char * psz, int len)
{
  char * psznew = MyAlloc(char *, len + 1);
  if (psznew)
  {
    strncpy_s(psznew, len, psz, len);
    psznew[len] = '\0';
  }
  return psznew;
}

LPSTR 
GetErrorString(
  UINT dwError
  )
{
	LPSTR			pszTemplate     = "An error %d occurred while processing a system call.\r\n\r\n%s";
	LPSTR			pszUnk			= "An unknown error %d occurred while processing a system call.\r\n";
	LPVOID			lpMsgBuf		= NULL; 
	LPSTR			pszMessage      = NULL;
	int			    len;
	
	//Format the last error into out message buffer
	//
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		dwError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
		);
	if (lpMsgBuf)
	{
		len = slen((LPTSTR)lpMsgBuf) + slen(pszTemplate) + sizeof(TCHAR) + sizeof(TCHAR)*11;
		pszMessage = MyAlloc(LPTSTR, len);
		if (pszMessage)
		{
            if (!SUCCEEDED(StringCchPrintfA(pszMessage, len, pszTemplate, dwError, lpMsgBuf)))
            {
                MyFree(pszMessage);
            }
		}
		LocalFree( lpMsgBuf );
	}
	else
	{
		len = slen(lpMsgBuf) + slen(pszUnk) + sizeof(TCHAR) + sizeof(TCHAR) * 11;
		pszMessage = MyAlloc(LPSTR, len);
        if (pszMessage)
        {
            if (!SUCCEEDED(StringCchPrintfA(pszMessage, len, pszUnk, dwError)))
            {
                MyFree(pszMessage);
            }
	    }
    }
  return (LPTSTR)pszMessage;
}

void TraceErrorMessage(DWORD lastError)
{
    LPTSTR pszError = GetErrorString(lastError);
    if (pszError)
    {
        DebugOut("%s\r\n", pszError);
        MyFree(pszError);
    }
    else
    {
        DebugOut("An error %d occurred\r\n", lastError);
    }
}
/*****************************************************************************************

  Function: StripPath()

  IN LPSTR pFile - pointer to NULL terminated name of file

  this function strips the full path and returns the path of the binary

  returs the pointer to the stripped down version of the string
*******************************************************************************************/
PSTR 
StripPath(
  LPSTR pFile
  )
{
  PSTR psz = strrchr( pFile, '\\' );
  // if valid pointer, then we are OK to advance the pointer
  if( psz )
  {
    psz++;  // advance to the character following the backslash
  }
  // otherwise, jsut return the pointer please
 	else
  {
    psz = (PSTR)pFile;
  }
  return psz; // return the string for the caller
}

/*****************************************************************************************

Function: StripPath()

IN LPSTR pFile - pointer to NULL terminated name of file

this function strips the full path and returns the path of the binary

returs the pointer to the stripped down version of the string
*******************************************************************************************/
LPWSTR
StripPathW(
LPWSTR pFile
)
{
    LPWSTR psz = wcsrchr(pFile, L'\\');
    // if valid pointer, then we are OK to advance the pointer
    if (psz)
    {
        psz++;  // advance to the character following the backslash
    }
    // otherwise, jsut return the pointer please
    else
    {
        psz = (LPWSTR)pFile;
    }
    return psz; // return the string for the caller
}

UINT
ConDoErrorMessage(
	UINT dwError
	)
{
	
	LPTSTR pszMessage = GetErrorString(dwError);
	if (pszMessage)
	{
		puts(pszMessage);
		MyFree(pszMessage);
	}
	return dwError;
}

UINT
MBDoErrorMessage(UINT dwError)
{
	LPTSTR			pszMessage = GetErrorString(dwError);
	if (pszMessage)
	{
		MessageBox(NULL, pszMessage, "Error", MB_OK | MB_ICONERROR);
		//puts(pszMessage);
		MyFree(pszMessage);
	}
	return dwError;
}

char * FindChar(char * psz, char ch, int len)
{
  char * p = psz;
  //int i = 0;
  do
  {
    //TraceOut(USE_CONSOLE|USE_DEBUGGER, NULL,"p is %s and curr len is %d\n", p, p - psz);
    if (*p == ch)
    {
      goto DONE;
    }
    p++;
    //i++;
  }while((int)(p - psz) < len);
  p = NULL;
DONE:  
  return p;
}

/************************************************************************************************

  Function: TraceOut()

  This function logs data to the log file. The log file is created globally and is dependent upon
  the type of test executed. 

  IN LPTSTR pszArgs           - pointer to string containing data and args
  IN LPTSTR pszFileName       - name of the log file to use which may have no meaning if we are logging to a server 
  IN ULONG LogType            - Logging method to use (This will be defined somewhere else)

  returns: The count of characters actually added to the log, -1 on error

**************************************************************************************************/

int
TraceOut(
  int     LoggingMethod,
  LPTSTR  pszFileName,
  char *  pszData, 
  ...
  )
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
  // if we cannot take the critical section object, then we are doomed.
  //
  //if (!TakeLock())
  //{
   // goto DONE;
  //}
  // wrap it in a try/except
  //
  // initialize the vaList 
  va_start( vaList, pszData );
  // if cbWritten is -1 return
  while ( cbWritten == -1 )
  {
    lpBuffer = MyAlloc(char *, size);
    if (!lpBuffer)
    {
      cbWritten = -1;
      break;
    }
    cbWritten = _vsnprintf_s(lpBuffer, size, size, pszData, vaList);
    //_vsnprintf_s(lpBUffer, size, size,
    // if cbWritten returns here as -1 means there is not any lpBuffer
    //cbWritten = _vsnprintf( lpBuffer, size, pszData, vaList );
    // if everything did not fit in the buffer, then re-alloc the memory and try again
    // this has an unusual side effect for bad buffers in that it may never complete
    if ( cbWritten < 0)
    {
      HeapFree(GetProcessHeap(), 0, lpBuffer);
      lpBuffer = NULL;
      size += increment; //adjust the size of the lpBuffer
    }
  }
  // terminate string formatting
  va_end( vaList );
  // if lpBuffer is valid, then free it
  //ReleaseLock();
//DONE:
  // return the bytes cbWritten or failrure
  if (lpBuffer)
  {
    OutputDebugString(lpBuffer);
    HeapFree(GetProcessHeap(), 0, lpBuffer);
    lpBuffer = 0;
    //MyFree((PVOID *)&lpBuffer);
  }
  return cbWritten;
}

BOOL 
FindInBuffer(
  LPTSTR pszString, 
  PUCHAR pBuffer, 
  int dwSize
  )
{
	int curr, i = 0;
	CHAR tmp[500];
	char buffer[500];
	BOOL br = FALSE;
	PUCHAR pch = (PUCHAR)pBuffer;
	memset(tmp, 0, sizeof(tmp));
	StringCchCopy(tmp, 500, pszString);
  //strcpy(tmp, pszString);
	i = 0;
  curr = 0;
	do
	{
		if (*pch == '\r' || *pch == '\n')
		{
			pch++;
			if (*pch == '\n')
			{
				pch ++;
			}
			buffer[i] = '\0';
			LPTSTR psz = strrchr(pszString, '\\');
			if (psz)
			{
				psz++;
				LPTSTR psz2 = strchr(psz, '.');
				if (psz2)
					*psz2 = '\0';
				if (strstr(buffer, psz))
				{
					//TraceOut(USE_CONSOLE|USE_DEBUGGER, NULL,"%s %s\n", tmp, buffer);
					break;
				}
			}
			memset(buffer, 0, sizeof(buffer));
			i = 0;
		}
		buffer[i] = *pch;
		if ((DWORD)(pch - pBuffer) == dwSize)
		{
			break;
		}
		pch++;
		i++;
	}while(TRUE);
  return br;
}

#ifdef GUI_APP  
#endif
/************************************************************************************************

  Function: DebugOut()

  This function logs data to the log file. The log file is created globally and is dependent upon
  the type of test executed. 

  IN LPTSTR pszArgs           - pointer to string containing data and args
  IN LPTSTR pszFileName       - name of the log file to use which may have no meaning if we are logging to a server 
  IN ULONG LogType            - Logging method to use (This will be defined somewhere else)

  returns: The count of characters actually added to the log, -1 on error

**************************************************************************************************/

int
DebugOut(
  char *  pszData, 
  ...
  )
{
  // uninitialized varaible 
  //
  va_list   vaList;             // list of arguments structure
  // initialized variables
  //
  int       cbWritten =   -1;             // bytes cbWritten to the lpBuffer
  char *    lpBuffer  =   NULL;           // lpBuffer to write
  int       increment =   512;            // lpBuffer size adjiustement
  int       size      =   increment;      // size of the lpBuffer
  // initialize the vaList 
  va_start( vaList, pszData );
  // if cbWritten is -1 return
  while ( cbWritten == -1 )
  {
    lpBuffer = MyAlloc(char *, size);
    if (!lpBuffer)
    {
      cbWritten = -1;
      break;
    }
    // if cbWritten returns here as -1 means there is not any lpBuffer
    cbWritten = StringCchVPrintfA( lpBuffer, size, pszData, vaList );
    // if everything did not fit in the buffer, then re-alloc the memory and try again
    // this has an unusual side effect for bad buffers in that it may never complete
    if ( !SUCCEEDED(cbWritten) )
    {
      HeapFree(GetProcessHeap(), 0, lpBuffer);
      lpBuffer = NULL;
      size += increment; //adjust the size of the lpBuffer
    }
  }
  // terminate string formatting
  va_end( vaList );
  if (lpBuffer)
  {
    OutputDebugString(lpBuffer);
    HeapFree(GetProcessHeap(), 0, lpBuffer);
    lpBuffer = 0;
  }
  return cbWritten;
}

HDC 
CreateMemDC(
	HDC hdc, 
	SIZE * lpsz
	)
{
	HDC hdcMem = CreateCompatibleDC(hdc);
	if (hdcMem)
	{
		HBITMAP hbm = CreateCompatibleBitmap(hdc, lpsz->cx, lpsz->cy);
		if (hbm)
		{
			RECT rc = {0};
			rc.right = lpsz->cx;
			rc.bottom = lpsz->cy;
			HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbm);
			if (hbmOld)
			{
				DeleteObject(hbmOld);
			}
			if (!FillRect(hdcMem, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH)))
			{
				// error happened
			}
		}
		else
		{
			DeleteDC(hdcMem);
			hdcMem = NULL;
		}
	}
	return hdcMem;
}

BOOL 
GetScreenSize(
	LPSIZE lpsz
	)
{
	if (lpsz)
	{
		lpsz->cx = GetSystemMetrics (SM_CXSCREEN);
		lpsz->cy = GetSystemMetrics(SM_CYSCREEN);
		return TRUE;
	}
	return FALSE;
}