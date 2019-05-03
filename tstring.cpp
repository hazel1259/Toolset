#include "comm_inc.h"
//#include "mem.h"
#include ".\tstring.h"
//#include "..\Include\common.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  BUGS: 
//  Replace all str functions with StringCch* fnctions
//  Repace memcpy with their other strincch equivalents
//  Make TString a unicode implementation
//  
//
extern BOOL ConvertToAnsi(__in LPWSTR, __in LPTSTR);
extern BOOL ConvertToUnicode(__in LPCSTR, __in PWSTR);
#define _OFFSET(_P1_, _P2_) (int)(_P1_ - _P2_)

static HANDLE g_mutex = NULL;
#define TSTRING_MUTEX_NAME "$TSTRING_MUTEX$"
BOOL InitLock()
{
    static bool init = false;
    if (init == false && NULL == g_mutex)
    {
        g_mutex = CreateMutexA(NULL, FALSE, TSTRING_MUTEX_NAME);
        init = g_mutex != NULL ? true : false;
        return g_mutex != NULL;
    }
    return g_mutex != NULL;
}

BOOL Lock()
{
    return TRUE;
    BOOL bRet = FALSE;
    if (InitLock())
    {
        DWORD dwWait = WaitForSingleObject(g_mutex, INFINITE);

        switch (dwWait)
        {
            // The thread got ownership of the mutex
            case WAIT_OBJECT_0:
                bRet = TRUE;
            break;
            // The thread got ownership of an abandoned mutex
            // The database is in an indeterminate state
            case WAIT_ABANDONED:
                bRet = FALSE;
            break;
        }
    }
    return bRet;
}

BOOL Unlock()
{
    return TRUE;
    return ReleaseMutex(g_mutex);
}

BOOL ShiftBufferRight(PUCHAR pch, int len, int pos, int bytes)
{
	BOOL br = TRUE;
	if (pos > len)
	{
		br = FALSE;
	}
	else
	{
		__try
		{
			int len2move = len - pos;
			PUCHAR p1 = pch + (len - 1) + bytes;
			PUCHAR  p2 = pch + (len - 1);
			
			for (int i = len2move; i > 0; i --, p1 --, p2 --)
			{
				*p1 = *p2;
				*p2 = '\0';
			}
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			br = FALSE;
		}
	}
	return br;
}

BOOL ShiftBufferLeft(PUCHAR pch, int len, int pos, int bytes)
{
	BOOL br = TRUE;
	if (pos > len)
	{
		br = FALSE;
	}
	else
	{
		int len2move = len - pos;
		PUCHAR  p1 = pch + (len - 1);
		PUCHAR p2 = pch + (len - 1) + bytes;
			
		for (int i = 0; i < len; i ++, p1 ++, p2 ++)
		{
			*p1 = *p2;
			*p2 = '\0';
		}
	}
	return br;
}

HRESULT CopyStringW(LPWSTR src, LPWSTR dest, int len)
{
   return StringCchCopyW(src, len + 1, dest);
}

HRESULT CopyStringA(LPSTR src, LPSTR dest, int len)
{
   return StringCchCopyA(src, len + 1, dest);
}

BOOL AllocStringA(LPSTR * ppsz, int len)
{
    if (!Lock())
    {
        return FALSE;
    }
    LPSTR pszRet = NULL;
	int s = (len + 1)*sizeof(CHAR);
    
	if (*ppsz)
    {
		int test = HeapSize(GetProcessHeap(), 0, *ppsz);
		if (len <= test)
		{
            pszRet = *ppsz;
            ZeroMemory(pszRet, len);
			goto DONE;
		}
        LPSTR psz = *ppsz;
        pszRet = MyRealloc(LPSTR, psz, s);
        if (!pszRet)
        {
            // occasionally in fragmented memory situations, we cannot reallocate 
            // but allocating works OK
            pszRet = MyAlloc(LPSTR, s);
            if (pszRet)
            {
                HRESULT hr = StringCchCopyA(pszRet, s, psz);
                if (SUCCEEDED(hr))
                {
                    *ppsz = pszRet;
                    MyFree(psz);
                }
                else
                {
                    MyFree(pszRet);
                }
            }
        }
        else
        {
            *ppsz = pszRet;
        }
    }
    else
    {
        pszRet = MyAlloc(LPSTR, s);
        if (pszRet)
        {
            *ppsz = pszRet;
        }
    } 
DONE:
    Unlock();
    return (pszRet != NULL);
}

BOOL AllocStringW(LPWSTR * ppsz, int len)
{
    if (!Lock())
    {
        return FALSE;
    }
    int s = (len + 1)*sizeof(WCHAR);
    LPWSTR pszRet = NULL;
    if (*ppsz)
    {
		int test = HeapSize(GetProcessHeap(), 0, *ppsz);
		if (len <= (int)(test/sizeof(WCHAR)))
		{
            pszRet = *ppsz;
            ZeroMemory(pszRet, len);
            goto DONE;
		}
        LPWSTR psz = *ppsz;
        pszRet = MyRealloc(LPWSTR, psz, s);
        if (!pszRet)
        {
            // occasionally in fragmented memory situations, we cannot reallocate 
            // but allocating works OK
            pszRet = MyAlloc(LPWSTR, s);
            if (pszRet)
            {
                HRESULT hr = CopyStringW(pszRet, psz, s);
                //HRESULT hr = StringCchCopyW(pszRet, s, psz);
                if (SUCCEEDED(hr))
                {
                    *ppsz = pszRet;
                    MyFree(psz);
                }
                else
                {
                    MyFree(pszRet);
                }
            }
        }
        else
        {
            *ppsz = pszRet;
        }
    }
    else
    {
        pszRet = MyAlloc(LPWSTR, s);
        if (pszRet)
        {
            *ppsz = pszRet;
        }
    } 
DONE:
    Unlock();
    return (pszRet != NULL);
}

BOOL CompareStringA(LPSTR pszSrc, LPSTR pszFind, BOOL ignoreCase)
{
    if (ignoreCase)
    {
        if (_stricmp(pszSrc, pszFind) == 0)
        {
            return TRUE;
        }
    }
    else
    {
        if (strcmp(pszSrc, pszFind) == 0)
        {
            return TRUE;
        }
    }
    return FALSE;
}

BOOL CompareStringW(LPWSTR pszSrc, LPWSTR pszFind, BOOL ignoreCase)
{
    if (ignoreCase)
    {
        if (_wcsicmp(pszSrc, pszFind) == 0)
        {
            return TRUE;
        }
    }
    else
    {
        if (wcscmp(pszSrc, pszFind) == 0)
        {
            return TRUE;
        }
    }
    return FALSE;
}
LPSTR FindStringA(LPSTR pszSrc, LPSTR pszFind, BOOL ignoreCase)
{
    LPTSTR        pszret          = NULL;
    BOOL          bFound          = FALSE;
    COMPARESTRING compstr         = NULL;
   
    // check for valid object and input
    //
    if (!pszSrc || !pszFind)
    {
        return NULL;
    }
    // intialize some locals
    LPTSTR  pszCurr     = pszSrc;
    int     findLen     = slen(pszFind);
    int     currLen     = slen(pszCurr);
    // ignore case
    compstr = (ignoreCase == TRUE) ? (COMPARESTRING)_strnicmp : (COMPARESTRING)strncmp; 
    // locate and return
    while(*pszCurr)
    {
        // if the current length is less than the findLen we are done
        if (currLen < findLen)
        {
            return NULL;
        }
        if (compstr(pszCurr, pszFind, findLen) == 0)
        {
            return pszCurr;
        }
        pszCurr ++;
        currLen --;
    }

    if (*pszCurr)
    {
        pszret = pszCurr;
    }
    return pszret;
}

LPWSTR FindStringW(LPWSTR pszSrc, LPWSTR pszFind, BOOL ignoreCase)
{
    LPWSTR          pszret          = NULL;
    BOOL            bFound          = FALSE;
    COMPARESTRINGW  compstr         = NULL;
   
    // check for valid object and input
    //
    if (!pszSrc || !pszFind)
    {
        return NULL;
    }
    // intialize some locals
    LPWSTR  pszCurr     = pszSrc;
    int     findLen     = wlen(pszFind);
    int     currLen     = wlen(pszCurr);
    // ignore case
    compstr = (ignoreCase == TRUE) ? (COMPARESTRINGW) _wcsnicmp : (COMPARESTRINGW) wcsncmp; 
    // locate and return
    while(*pszCurr)
    {
        // if the current length is less than the findLen we are done
        if (currLen < findLen)
        {
            return NULL;
        }
        if (compstr(pszCurr, pszFind, findLen) == 0)
        {
            return pszCurr;
        }
        pszCurr ++;
        currLen --;
    }

    if (*pszCurr)
    {
        pszret = pszCurr;
    }
    return pszret;
}

TStringA::TStringA(void)
: m_len(0)
, m_size(0)
, m_pstr(NULL)
{
}

TStringA::TStringA(__in LPTSTR psz)
: m_len(0)
, m_pstr(NULL)
{
  Init(psz);
}
TStringA::~TStringA(void)
{
  if (m_pstr)
  {
    MyFree(m_pstr);
  }
}

COMPARESTRING TStringA::GetCompareFunction(BOOL ignoreCase)
{
  if (ignoreCase)
  {
    return _strnicmp;
  }
  else
  {
    return strncmp;
  }
}
void TStringA::Clear()
{
  if (m_pstr)
  {
    MyFree(m_pstr);
    m_len = 0;
  }
}
#if 0
LPSTR TStringA::Clone()
{
  LPSTR pret = NULL;
  if (!m_pstr)
  {
    return NULL;
  }
  int len = wlen(m_pstr)*sizeof(CHAR);
  pret = MyAlloc(__in LPSTR, len + 1);
  if (pret)
  {
    if (FAILED(StringCchCopyA(pret, len + 1, m_pstr)))
    {
      MyFree(pret);
    }
  }
  return pret;
}
#endif

BOOL TStringA::TrimRight()
{
  if (IsEmpty())
  {
    return FALSE;
  }
  LPTSTR psz = m_pstr;
  psz += (slen(m_pstr) - 1);
  while(*psz == ' ')
  {
    psz --;
  }
  // nothing to do
  if (m_pstr == psz)
  {
    return TRUE;
  }
  *(psz + 1) = '\0';
  m_len = slen(m_pstr);
  return TRUE;
}

BOOL TStringA::TrimLeft()
{
  if (!m_pstr)
  {
    return FALSE;
  }
  LPTSTR psz = m_pstr;
  //psz += slen(m_pstr);
  while(*psz == ' ')
  {
    psz ++;
  }
  // nothing to do
  if (m_pstr == psz)
  {
    return TRUE;
  }
  int len = slen(psz);
  LPTSTR psztmp = MyAlloc(__in LPTSTR, len + 1);
  if (!psztmp)
  {
    return FALSE;
  }
  HRESULT hr = StringCchCopy(psztmp, len + 1, psz);
  if (FAILED(hr))
  {
    MyFree(psztmp);
    return FALSE;
  }
  if (!Init(psztmp))
  {
    return FALSE;
  }
  MyFree(psztmp);
  return TRUE;
}

void TStringA::ToUpper()
{
	if (m_pstr)
	{
		LPSTR psz = m_pstr;
		while(*psz)
		{
			*psz = toupper(*psz);
			psz ++;
		}
	}
}

void TStringA::ToLower()
{
  if (m_pstr)
  {
	LPSTR psz = m_pstr;
	while(*psz)
	{
		*psz = tolower(*psz); ////*psz &= ~(0x20);
		psz ++;
	}
  }
}

BOOL TStringA::IsLowerCase()
{
	if (IsEmpty())
	{
		return FALSE;
	}
	LPTSTR psz = m_pstr;
	while(*psz)
	{
        if (isalpha(*psz) && !islower(*psz))
		{
			return FALSE;
		}
		psz ++;
	}
	return TRUE;
}
// we will need this eventually
//_locale_t _get_current_locale(void);
BOOL TStringA::IsUpperCase()
{
	if (IsEmpty())
	{
		return FALSE;
	}
	LPSTR psz = m_pstr;
	while(*psz)
	{
		CHAR tch = (CHAR)*psz;
		//tch &= 0xFF;
        if (isalpha(*psz) && !isupper(tch))
		{
			return FALSE;
		}
		psz ++;
	}
	return TRUE;
}

LPTSTR TStringA::Find(__in LPTSTR psz)
{
  return Find(psz, FALSE);
}

LPTSTR TStringA::Find(__in LPTSTR psz, BOOL ignoreCase)
{
  //LPTSTR        pszret          = NULL;
  BOOL          bFound          = FALSE;
  COMPARESTRING compstr         = NULL;

  // check for valid object and input
  //
  if (!m_pstr || !psz)
  {
    return NULL;
  }
  // intialize some locals
  LPTSTR  pszCurr   = m_pstr;
  int     len       = slen(psz);
  int     currLen   = slen(pszCurr);
  // ignore case
  compstr = GetCompareFunction(ignoreCase);
  // locate and return
  while(*pszCurr)
  {
    if (currLen < len)
    {
      break;
    }
    if (compstr(pszCurr, psz, len) == 0)
    {
      return pszCurr;
    }
    pszCurr ++;
    currLen --;
  }

  return NULL;
}

LPSTR TStringA::Find(CHAR tch)
{
    if (m_pstr)
    {
        CHAR sz[2];
        sz[0] = tch;
        sz[1] = '\0';
        return Find(sz);
    }
    return NULL;
}


void TStringA::Add(CHAR ch)
{
	*this += ch;
}

void TStringA::Add(LPSTR pszText)
{
	Concat(pszText);
}

CHAR TStringA::Get(int pos)
{
	return GetChar(pos);
}

LPWSTR TStringA::Convert()
{
	LPWSTR pszRet = NULL;
	if (AllocStringW(&pszRet, m_len + 1))
	{
		if (!ConvertToUnicode(m_pstr, pszRet))
		{
			MyFree(pszRet);
		}
	}
	return pszRet;
}

BOOL TStringA::Replace(__in LPTSTR psz, LPTSTR pszReplace)
{
  return Replace(psz, pszReplace, FALSE);
}

#ifdef POOR_PERF_REPLACE
BOOL TStringA::Replace(__in LPTSTR psz2Rep, LPTSTR pszReplace, BOOL ignoreCase)
{
    BOOL    br          =   FALSE;
    LPSTR   pszFind     =   NULL;
    DWORD   count       =   0;
    int     repLen      =   slen(pszReplace);
    int     len2rep     =   slen(psz2Rep);
    LPSTR   pszCurr     =   m_pstr;
    // check for NULL pointers
    //
    if (psz2Rep == NULL || pszReplace == NULL)
    {
        goto DONE;
    }
    // check for NULL strings
    if (*psz2Rep == '\0' || *pszReplace == '\0')
    {
        goto DONE;
    }
    // if the replace string is the same as the string to replace, we're done
    if (CompareStringA(psz2Rep, pszReplace, ignoreCase))
    {
        br = TRUE;
        goto DONE;
    }
    
    // count the number of occurences of psz2Rep
    // perf hit, but what do we do for reallocation which could be worse
    while(TRUE)
    {
        pszFind = FindStringA(pszCurr, psz2Rep, ignoreCase);
        if(!pszFind)
        {
            break;
        }
        pszCurr = pszFind + len2rep;
        count ++;
    }
    // check to see if we have a string to replace;
    if (count == 0)
    {
        goto DONE;
    }

    int totalLen = slen(m_pstr);
    // this will tell us if we have to reallocate the memory
    if (len2rep < repLen)
    {
        totalLen -= len2rep*count;
        if (totalLen < 0)
        {
            // possibly throw since we should not hit this
            goto DONE;
        }
    }
    br = TRUE;
    
    int     len;
    LPSTR   pszNew      = NULL;
    LPSTR   pNew        = NULL;
    int     remaining   = 0;
    LPSTR pOld          = m_pstr;
    
    
    totalLen += repLen*count;

    pszNew = MyAlloc(LPSTR, totalLen + sizeof(CHAR));
    
    pNew = pszNew;
    // walk the string and replace each occurrence of string
    while(TRUE)
    {
        pszFind = FindStringA(pOld, psz2Rep, ignoreCase);
        if (!pszFind)
        {
            if (remaining > 0)
            {
                HRESULT hr = StringCchCatA(pszNew, totalLen + sizeof(CHAR), pszCurr);
                if (!SUCCEEDED(hr))
                {
                    MyFree(pszNew);
                    br = FALSE;
                    goto DONE;
                }
            }
            break;
        }
        len = (pszFind - pOld);
        if (len > 0)
        {
            memcpy(pNew, pOld, len);
            pNew += len;
        }
        memcpy(pNew, pszReplace, repLen);
        pNew    += repLen;
        pOld    = pszFind + len2rep;
        remaining = slen(pOld);
    }
    if (pszNew)
    {
        MyFree(m_pstr);
        m_pstr = pszNew;
        m_len = slen(pszNew);
    }
DONE:
    return br;
}
#else

BOOL TStringA::Replace(__in LPSTR psz2Rep, __in LPSTR pszReplace, BOOL ignoreCase)
{
    BOOL    br          =   FALSE;
    int     repLen      =   slen(pszReplace);
    int     len2rep     =   slen(psz2Rep);
    LPSTR   pszCurr     =   m_pstr;
    // check for NULL pointers
    //
    if (psz2Rep == NULL || pszReplace == NULL)
    {
        goto DONE;
    }
    // check for NULL strings
    if (*psz2Rep == '\0' || *pszReplace == '\0')
    {
        goto DONE;
    }
    // if the replace string is the same as the string to replace, we're done
    if (CompareStringA(psz2Rep, pszReplace, ignoreCase))
    {
        br = TRUE;
        goto DONE;
    }
    
    br = TRUE;
    // if the string is less than the one we are replacing, we can just use 
    // the existing buffer
    if (repLen <= len2rep)
    {
        // in this case, we need not make the string bigger, just replace
        // the necessary strings
        //
        LPSTR p1    = m_pstr;
        LPSTR p     = p1;
        LPSTR pNext;
        int dcb = len2rep - repLen;
        do
        {
            p1 = FindStringA(pszCurr, psz2Rep, ignoreCase);
            if (p1)
            {
                memcpy(p1, pszReplace, repLen);
                p1 += repLen;
                pszCurr = p1;
                // copy string bytes forward
                if (repLen < len2rep)
                {
                    p = p1;
                    pNext = p1 + dcb;
                    while(*pNext)
                    {
                        *p = *pNext;
                        p ++;
                        pNext ++;
                    }
                    // go ahead and 0 out the remaining bytes
                    while(*p) 
                    {
                        *p = '\0';
                         p++;
                    }

                }
            }
        }while(p1);
    }
    else
    {
        // otherwise, the buffer size will need to be adjusted by at least the 
        // difference in characters
        LPSTR   pszFind;
        LPSTR   pszNew; 
        LPSTR   pNew        = NULL;
        int     remaining;
        int     ccFind      = 1;
        int     totalLen    = m_len + (repLen - len2rep);
        int     len, newRemaining, offset;
        //totalLen += repLen;
        remaining       = m_len;
        newRemaining    = totalLen;
        // in this case, the string will need to be made larger for the new string
        //
        pszNew = MyAlloc(LPSTR, (totalLen + 1) * sizeof(CHAR));
        pNew = pszNew;
        // walk the string and replace each occurrence of string
        while(TRUE)
        {
            // find the string!
            pszFind = FindStringA(pszCurr, psz2Rep, ignoreCase);
            if (!pszFind && pszNew)
            {
                if (remaining > 0)
                {
                    if (newRemaining < remaining)
                    {
                        totalLen += (remaining - newRemaining);
                        if (!AllocStringA(&pszNew, totalLen))
                        {
                            if (pszNew)
                            {
                                MyFree(pszNew);
                                br = FALSE;
                                goto DONE;
                            }
                        }
                    }
                    HRESULT hr = StringCchCatA(pszNew, totalLen + sizeof(CHAR), pszCurr);
                    if (!SUCCEEDED(hr))
                    {
                        DebugOut("%s: Failed to copy buffer\r\n", __FUNCTION__);
                        MyFree(pszNew);
                        br = FALSE;
                        goto DONE;
                    }
                }
                if (*pszNew)
                {
                    m_pstr = pszNew;
                }
                break;
            }
            // calculate the first length
            len = _OFFSET(pszFind, pszCurr); 
            if (newRemaining < len)
            {
                totalLen += (len - newRemaining) + repLen;
                if (!AllocStringA(&pszNew, totalLen))
                {
                    br = FALSE;
                    goto DONE;
                }
                pNew = pszNew + offset;
            }
            // if we have some bytes here copy them now
            memcpy(pNew, pszCurr, len);
            pNew += len;
            offset = _OFFSET(pNew, pszNew);
            newRemaining = totalLen - offset;
            if (newRemaining < repLen)
            {
                totalLen += (repLen - newRemaining) + (repLen - len2rep);
                if (!AllocStringA(&pszNew, totalLen))
                {
                    br = FALSE;
                    goto DONE;
                }
                pNew = pszNew + offset;
            }
            // if we get this far, we can copy the memory over to the location
            memcpy(pNew, pszReplace, repLen);
            pNew += repLen;
            offset = _OFFSET(pNew, pszNew);
            newRemaining = totalLen - offset;
            pszCurr    = pszFind + len2rep;
            remaining = m_len - _OFFSET(pszCurr, m_pstr);
            
        }
    }
    
    m_len = slen(m_pstr);
DONE:
    return br;
}
#endif
BOOL TStringA::Remove(__in LPTSTR pszRemove)
{
  return Remove(pszRemove, FALSE);
}

BOOL TStringA::Remove(__in LPTSTR pszRemove, BOOL ignoreCase)
{
  BOOL          bFound          = FALSE;
  COMPARESTRING compstr         = NULL;

  // check for valid object and input
  //
  if (!m_pstr || !pszRemove)
  {
    return FALSE;
  }
  // intialize some locals
  TString   ts;
  LPTSTR    psz;
  LPTSTR    pszCurr     =   m_pstr;
  int       len         =   slen(pszRemove);
  int       currLen     =   slen(pszCurr);
  int       i           = 0;
  // ignore case
  if (!Find(pszRemove, ignoreCase))
  {
    return FALSE;
  }
  // setup the comparison functions by looking at the ignore flag
  compstr = GetCompareFunction(ignoreCase);
  // walk the string looking for candidates to replace
  //
  if (!ts.InitEmptyString(m_len + 1))
  {
    return FALSE;
  }
  psz = ts.GetString();
  while(*pszCurr)
  {
    // if we reach the end of the string and we care, append the remainder of the string
    //
    if (currLen < len)
    {
      if (bFound)
      {
        psz[i] = '\0';
      }
      break;
    }
    // if we find the string, then remove it without prejudice
    if (compstr(pszCurr, pszRemove, len) == 0)
    {
      bFound = TRUE;
      i += len;
      pszCurr += len;
      currLen -= len;
      continue;
    }
    psz[i] = *pszCurr;
    i++;
    pszCurr ++;
    currLen --;
  }
  // now that we have the string in place, intialize this object with the new string
  return Init(ts.GetString());
}

BOOL TStringA::RemoveChar(CHAR tch)
{
  return RemoveChar(tch, FALSE);
}

BOOL TStringA::RemoveChar(CHAR tch, BOOL ignoreCase)
{
  if (!m_pstr)
  {
    return FALSE;
  }
  TString ts;
  ts.InitEmptyString(m_len);
  LPTSTR pszNew = ts.GetString();
  int i = 0;
  LPTSTR pszCurr = m_pstr;
  while(*pszCurr)
  {
    if (ignoreCase)
    {
        CHAR chOld = ::toupper(tch);
        CHAR chCurr = ::toupper(*pszCurr);
        if (chOld != chCurr)
        {
            pszNew[i] = chCurr;
            i ++;
        }
    }
    else
    {
        if (tch != *pszCurr)
        {
            pszNew[i] = tch;
            i ++;
        }
    }
    pszCurr ++;
  }
  return Init(pszNew);
}

BOOL TStringA::ReplaceChar(CHAR tch, CHAR tchNew)
{
  return ReplaceChar(tch, tchNew, FALSE);
}

BOOL TStringA::ReplaceChar(CHAR tch, CHAR tchNew, BOOL ignoreCase)
{
  if (!m_pstr)
  {
    return FALSE;
  }
  TString ts;
  ts.InitEmptyString(m_len);
  LPTSTR pszNew = ts.GetString();
  int i = 0;
  LPTSTR pszCurr = m_pstr;
  while(*pszCurr)
  {
    if (ignoreCase)
    {
        TCHAR chOld = ::toupper(tch);
        TCHAR chCurr = ::toupper(*pszCurr);
        if (chCurr == chOld)
        {
            pszNew[i] = tchNew;
        }
        else
        {
            pszNew[i] = *pszCurr;
        }
    }
    else
    {
      if (tch == *pszCurr)
      {
        pszNew[i] = tchNew;
      }
      else
      {
        pszNew[i] = *pszCurr;
      }
    }
    i ++;
    pszCurr ++;
  }
  return Init(pszNew);
}
int TStringA::GetLength(void)
{
  return m_len;
}

CHAR TStringA::GetChar(int pos)
{
  if (!m_pstr)
  {
    return -1;
  }
  if (pos >= m_len || pos < 0)
  {
    return -1;
  }
  return m_pstr[pos];
}

BOOL TStringA::Concat(__in LPTSTR psz)
{
    if (!psz)
    {
        return FALSE;
    }
    if (!m_pstr)
    {
        return Init(psz);
    }

    int len = slen(psz);
    m_len += len;
	if (m_len >= m_size - 1)
	{
		int grow_size = 0;
		if (m_size + 10 < m_len)
		{
			grow_size = 10 + len;
		}
		else
		{
			grow_size = 10;
		}
		m_size += grow_size;
		if (!AllocStringA(&m_pstr, m_size))		
		{
			m_size -= grow_size;
			return FALSE;
		}
		else
		{
            HRESULT hr = StringCchCatA(m_pstr, m_len + 1, psz);
			if (SUCCEEDED(hr))
            {
                return TRUE;
            }
		}
	}
	else
	{
        if (SUCCEEDED(StringCchCatA(m_pstr, m_len + 1, psz)))
        {
            return TRUE;
        }
	}
 
    m_len -= len;
    return FALSE;
}

//Define assignment operator.
TStringA TStringA::operator=( __in LPTSTR psz)
{
  if (Init(psz))
  {
    return m_pstr;
  }
  return NULL;
}

TStringA TStringA::operator=( TString ts)
{
  if (Init(ts.GetString()))
  {
    return m_pstr;
  }
  return NULL;
}

TStringA TStringA::operator+=( __in LPTSTR psz)
{
  if (!this->m_pstr)
  {
    if (Init(psz))
    {
      return m_pstr;
    }
  }
  else
  {
    this->Concat(psz);
    return m_pstr;
  }
  return NULL;
}

TStringA TStringA::operator+=( TStringA ts)
{
  if (!this->m_pstr)
  {
    if (Init(ts.GetString()))
    {
      return m_pstr;
    }
  }
  else
  {
    this->Concat(ts.GetString());
    return m_pstr;
  }
  return NULL;
}

TStringA TStringA::operator+=( __in CHAR ch)
{
    CHAR sz[2];
    sz[0] = ch;
    sz[1] = '\0';
    if (!m_pstr)
    {
        Init(sz);
    }
    else
    {
        Concat(sz);
    }
    return m_pstr;
}



BOOL TStringA::operator ==(__in LPTSTR psz)
{
  return IsEqual(psz);
}

BOOL TStringA::IsEqual(__in LPTSTR psz)
{
    if (!m_pstr || !psz)
    {
        return FALSE;
    }  
    if (strcmp(m_pstr, psz) == 0)
    {
        return TRUE;
    }
    return FALSE;
}

BOOL TStringA::IsEqual(__in LPSTR psz, BOOL bIgnoreCase)
{
    if (!m_pstr || !psz)
    {
        return FALSE;
    }  
    if (bIgnoreCase)
    {
        if (_stricmp(m_pstr, psz) == 0)
        {
            return TRUE;
        }
    }
    else
    {
        return IsEqual(psz);
    }
    return FALSE;
}

BOOL TStringA::IsEmpty()
{
    return (m_pstr == NULL || *m_pstr == '\0');
}

BOOL TStringA::Init(__in LPTSTR psz)
{
	if (!psz)
	{
		return FALSE;
	}
	m_len = (int)strlen(psz);
	m_size = m_len + 10;
	if (m_pstr)
	{
		MyFree(m_pstr);
	}
	if (!AllocStringA(&m_pstr, m_size))
	{
		return FALSE;
	}
	else
	{
		if (SUCCEEDED(StringCchCopyA(m_pstr, m_len + 1, psz)))
		{
			return TRUE;
		}
		else
		{
			MyFree(m_pstr);
			m_len = m_size = 0;
		}
	}
	return FALSE;
}

// only init a sub string of len bytes
BOOL TStringA::Init(__in LPTSTR pszSrc, int len)
{
	if (len == 0 || pszSrc == NULL)
	{
		return FALSE;
	}
	if (m_pstr)
	{
		MyFree(m_pstr);
	}
	m_len = len + 1;
	m_size = len + 1 + 10;
	  
	if (AllocStringA(&m_pstr, m_size))
	{
		if (SUCCEEDED(StringCchCopyA(m_pstr, m_len + 1, pszSrc)))
		{
			m_pstr[m_len] = '\0';
			return TRUE;
		}
		else
		{
			MyFree(m_pstr);
			m_len = m_size = 0;
		}
	}
	return FALSE;
}

BOOL TStringA::Init(LPWSTR pszw)
{
  if (!pszw)
  {
    return FALSE;
  }
  m_len = (int)wcslen(pszw);
  m_size = m_len + 10;
  if (m_pstr)
  {
    MyFree(m_pstr);
  }
  //m_pstr = MyAlloc(__in LPTSTR, (m_size + 1)*sizeof(WCHAR));
  
  if (AllocStringA(&m_pstr, m_size))
  {
    if (ConvertToAnsi(pszw, m_pstr))
    {
      return TRUE;
    }
    else
    {
      MyFree(m_pstr);
      m_len = m_size = 0;
    }
  }
  return FALSE;
}

LPTSTR TStringA::Search(__in LPTSTR psz)
{
  return strstr(psz, m_pstr);
}

BOOL TStringA::InitEmptyString(int size)
{
	if (m_pstr)
	{
		MyFree(m_pstr);
	}
	m_len = 0;
	m_size = size + 10;
	if (!AllocStringA(&m_pstr, m_size))
	{
		m_size = m_len = 0;
		return FALSE;
	}
	return TRUE;
}

BOOL TStringA::InitFrom(HINSTANCE hinst, int idsString)
{
	if (m_pstr)
	{
		MyFree(m_pstr);
	}
	m_size = 0x400;
	if (AllocStringA(&m_pstr, m_size))
	{
		m_len = LoadString(hinst, idsString, m_pstr, 0x400);
		if ( m_len == 0)
		{
		MyFree(m_pstr);
		m_size = m_len = 0;
		return FALSE;
		}
		else
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL TStringA::Compare(__in LPTSTR psz)
{
  return Compare(psz, FALSE);
}

BOOL TStringA::Compare(__in LPTSTR psz, BOOL ignoreCase)
{
  if (psz)
  {
    DWORD dwLen = slen(psz);
    return Compare(psz, ignoreCase, dwLen);
  }
  return FALSE;
}

BOOL TStringA::Compare(__in LPTSTR psz, BOOL ignoreCase, DWORD dwLen)
{
  if (m_pstr && psz)
  {
    COMPARESTRING compstr = GetCompareFunction(ignoreCase);
    return compstr(m_pstr, psz, dwLen) == 0;
  }
  return FALSE;
}

TStringA * TStringA::Split(char ch)
{
  if (!m_pstr)
  {
    return NULL;
  }
  else
  {
    if (!strchr(m_pstr, ch))
    {
      return new TString(m_pstr);
    }
    else
    {
      LPTSTR psz = m_pstr;
      int count = 0;
      TString * ts = new TString();
      if (ts->InitEmptyString(m_len + 1))
      {
        LPTSTR psz2 = ts->GetString();
        while(*psz)
        {
          if (*psz == ch)
          {
            *psz2 = '\0';
          }
          else
          {
            *psz2 = *psz;
          }
          psz ++;
          psz2 ++;
        }
        *psz2 = '\0';
        return ts;
      }
    }
  }
  return NULL;
}


TStringA * TStringA::Substring(int pos)
{
    if (m_pstr == NULL || pos > m_len)
    {
        return NULL;
    }
    return new TStringA(m_pstr + pos);
}

TStringA * TStringA::Substring(int pos, int len)
{
    if (m_pstr == NULL || pos > m_len)
    {
        return NULL;
    }
    TStringA * ret = new TStringA();    
    ret->InitEmptyString(len + 1);
    LPSTR psz = ret->GetString();
    for (int i = pos, j = 0; i < len; i++, j++)
    {
        psz[j] = m_pstr[i];
    }
    psz[len] = '\0';
    return ret;
}

BOOL TStringA::Remove(int pos)
{
    BOOL br = TRUE;
    if (!m_pstr)
    {
        br = FALSE;
    }
    else
    {
        //int len = strlen(m_pstr);
        if (m_len == 0 || pos > m_len)
        {
            br = FALSE;
        }
        else
        {
            for(int i = pos; i < m_len; i ++)
            {
                m_pstr[i] = m_pstr[i + 1];
            } 
            m_pstr[m_len - 1] = '\0';
            m_len --;
        }
    }
    return TRUE;
}

BOOL TStringA::Remove(int pos, int len)
{
    BOOL br = TRUE;
    if (!m_pstr || m_len < pos + len || m_pstr[0] == '\0')
    {
        br = FALSE;
    }
    else
    {
		if (pos > m_len || pos + len > m_len)
		{
			br =FALSE;
		}
		else
		{
			if (len == 0)
			{
				br = Remove(pos);
			}
			else
			{        
				char * p1 = m_pstr + pos;
				char * p2 = m_pstr + pos + len;
				int ch2move = m_len - pos;
				for (int i = 0; i < ch2move; i ++, p1++, p2 ++)
				{
					*p1 = *p2;
				}
				p1 ++;
				//p1 = m_pstr + m_len;
				while(*p1)
				{
					*p1 = '\0';
				}
				m_len -= len;

			}
		}
    }
    return br;
}

BOOL TStringA::Remove()
{
    if (!m_pstr)
    {
        return FALSE;
    }
    //int len = strlen(m_pstr);
    if (m_len == 0)
    {
        return FALSE;
    }
    else
    {
        m_pstr[m_len - 1] = '\0';
        m_len --;
    }
    return TRUE;    
}

BOOL TStringA::Insert(CHAR ch, int pos)
{
	char tmp[2];
	tmp[0] = ch;
	tmp[1] = '\0';
	return Insert(tmp, pos);
}

BOOL TStringA::Insert(LPSTR psz, int pos)
{
    BOOL br = TRUE;
	if (!*psz)
	{
		return br; // nothing to do
	}
    if (pos == m_len)
	{
		return Concat(psz);
	}
	if(psz && m_pstr)
    {
		LPSTR p = psz;
		int ccString   = slen(psz);
		if (ccString    + m_len >= m_size)
		{
			m_size += ccString    + 10;
			//m_pstr = MyRealloc(LPSTR, m_pstr, m_size);
			if (!AllocStringA(&m_pstr, m_size))
			{
				br = FALSE;
			}
		}
		if (br)
		{
			int len2move = m_len - pos;
			LPSTR p1 = m_pstr + (m_len - 1) + ccString;
			LPTSTR p2 = m_pstr + (m_len - 1);
			
			if (ShiftBufferRight((PUCHAR)m_pstr, m_len*sizeof(CHAR), pos, ccString*sizeof(CHAR)))
			{
				p2 = m_pstr + pos;
			
				for (int i = 0; i < ccString; i ++)
				{
					p2[i] = psz[i];
				}
				m_len = m_len + ccString;
			}
		}
    }
    return br;
}

BOOL TStringA::Reverse()
{
	if (!m_pstr)
	{
		return FALSE;
	}
	int len = slen(m_pstr);
	if (len == 1)
	{
		return TRUE;
	}
	LPTSTR  pszEnd   = m_pstr + len;
	LPTSTR  pszStart = m_pstr;
	if (!*pszEnd)
	{
		pszEnd --;
	}
	CHAR    tmp;
	while(TRUE)
	{
		tmp  = *pszStart;
		*pszStart = *pszEnd;
		*pszEnd = tmp;
		pszStart ++;
		pszEnd --;
		if (pszStart >= pszEnd)
		{
			break;
		}
	}
	return TRUE;
}

TStringA * TStringA::Clone()
{
    if (!m_pstr)
    {
        return NULL;
    }
    return new TStringA(m_pstr);
}

BOOL TStringA::Clone(TString * ts)
{
    if (!ts)
    {
        return FALSE;
    }
    else
    {
        ts->Clear();
        ts->Init(m_pstr);
        return TRUE;
    }
    return FALSE;
}

BOOL TStringA::Contains(LPSTR psz)
{
    return Find(psz) != NULL;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Unicode version starts here
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
TStringW::TStringW(void)
: m_len(0)
, m_size(0)
, m_pstr(NULL)
{
}

TStringW::TStringW(__in LPWSTR pszw)
: m_len(0)
, m_pstr(NULL)
, m_size(0)
{
  Init(pszw);
}

TStringW::~TStringW(void)
{
  if (m_pstr)
  {
    MyFree(m_pstr);
  }
}

TStringW * TStringW::Clone()
{
    if (!m_pstr)
    {
        return NULL;
    }
    return new TStringW(m_pstr);
}

BOOL TStringW::Clone(TStringW * ts)
{
    if (!ts)
    {
        return FALSE;
    }
    else
    {
        ts->Clear();
        ts->Init(m_pstr);
        return TRUE;
    }
    return FALSE;
}

BOOL TStringW::Contains(LPWSTR psz)
{
    return (Find(psz) != NULL);
}
COMPARESTRINGW TStringW::GetCompareFunction(BOOL ignoreCase)
{
  if (ignoreCase)
  {
    return _wcsnicmp;
  }
  else
  {
    return wcsncmp;
  }
  return NULL;
}

void TStringW::Clear()
{
  if (m_pstr)
  {
    MyFree(m_pstr);
    m_len = 0;
  }
}

BOOL TStringW::TrimRight()
{
    if (IsEmpty())
    {
        return FALSE;
    }
    LPWSTR psz = m_pstr;
    psz += (wlen(m_pstr) - 1);
    while (*psz == ' ')
    {
        psz--;
    }
    // nothing to do
    if (m_pstr == psz)
    {
        return TRUE;
    }
    *(psz + 1) = '\0';
    m_len = wlen(m_pstr);
    return TRUE;
}

BOOL TStringW::TrimLeft()
{
  if (!m_pstr)
  {
    return FALSE;
  }
  LPWSTR psz = m_pstr;

  if (*psz != L' ')
  {
    return TRUE; // nothing to do
  }
  while(*psz == ' ')
  {
    psz ++;
  }
  
  int len = wlen(psz);
  LPWSTR psztmp = NULL;
  if (!Alloc((PVOID *)&psztmp, (len + 1) * sizeof(WCHAR)))
  {
    return FALSE;
  }
  
  HRESULT hr = CopyStringW(psztmp, psz, len); //StringCchCopyW(psztmp, len + 1, psz);
  if (FAILED(hr))
  {
    MyFree(psztmp);
    return FALSE;
  }
  if (!Init(psztmp))
  {
    return FALSE;
  }
  MyFree(psztmp);
  return TRUE;
}

void TStringW::ToUpper()
{
  if (m_pstr)
  {
    LPWSTR psztmp = m_pstr;
    while (*psztmp)
    {
        *psztmp = towupper(*psztmp);//*m_pstr | 0x20;
        psztmp++;
    }
  }
}

void TStringW::ToLower()
{
//  WCHAR wch;
  if (m_pstr)
  {
    LPWSTR psztmp = m_pstr;
    while (*psztmp)
    {
        *psztmp = towlower(*psztmp);//*m_pstr | 0x20;
        psztmp++;
    }
  }
 
}

BOOL TStringW::IsUpperCase()
{
	if (IsEmpty())
	{
		return FALSE;
	}
	LPWSTR psz = m_pstr;
	while(*psz)
	{
        if (iswalpha(*psz) && !iswupper(*psz))
		{
			return FALSE;
		}
        psz ++;
	}
	return TRUE;
}


BOOL TStringW::IsLowerCase()
{
	if (IsEmpty())
	{
		return FALSE;
	}
	LPWSTR psz = m_pstr;
	while(*psz)
	{
        if (iswalpha(*psz) && !iswlower(*psz))
		{
			return FALSE;
		}
        psz++;
	}
	return TRUE;
}

LPWSTR TStringW::Find(__in LPWSTR psz)
{
  return Find(psz, FALSE);
}

LPWSTR TStringW::Find(__in LPWSTR psz, BOOL ignoreCase)
{
  BOOL          bFound          = FALSE;
  COMPARESTRINGW compstr         = NULL;

  // check for valid object and input
  //
  if (!m_pstr || !psz)
  {
    return NULL;
  }
  // intialize some locals
  LPWSTR  pszCurr   = m_pstr;
  int     len       = wlen(psz);
  int     currLen   = wlen(pszCurr);
  // ignore case
  compstr = GetCompareFunction(ignoreCase);
  while(*pszCurr)
  {
    if (currLen < len)
    {
      break;
    }
    if (compstr(pszCurr, psz, len) == 0)
    {
      return pszCurr;
    }
    pszCurr ++;
    currLen --;
  }
  
  return NULL;
}

LPWSTR TStringW::Find(WCHAR tch)
{
    if (m_pstr)
    {
        WCHAR wz[2];
        wz[0] = tch;
        wz[1] = '\0';
        return Find(wz);
    }
    return NULL;
}

void TStringW::Add(WCHAR wch)
{
	*this += wch;
}

void TStringW::Add(LPWSTR pszText)
{
	Concat(pszText);
}

WCHAR TStringW::Get(int pos)
{
	return GetChar(pos);
}

// converts the tstring into a ansi string and returns it to the caller
LPSTR TStringW::Convert()
{
	LPSTR pszRet = NULL;
	if (AllocStringA(&pszRet, m_len + 1))
	{
		if (!ConvertToAnsi(m_pstr, pszRet))
		{
			MyFree(pszRet);
		}
	}
	return pszRet;
}
BOOL TStringW::Replace(__in LPWSTR psz, LPWSTR pszReplace)
{
  return Replace(psz, pszReplace, FALSE);
}

#ifdef POOR_PERFORMANCE
BOOL TStringW::Replace(__in LPWSTR psz2Rep, LPWSTR pszReplace, BOOL ignoreCase)
{
    BOOL    br          =   FALSE;
    LPWSTR   pszFind    =   NULL;
    DWORD   count       =   0;
    int     repLen      =   slen(pszReplace);
    int     len2rep     =   slen(psz2Rep);
    LPWSTR  pszCurr     =   m_pstr;
    // check for NULL pointers
    //
    if (psz2Rep == NULL || pszReplace == NULL)
    {
        goto DONE;
    }
    // check for NULL strings
    if (*psz2Rep == '\0' || *pszReplace == '\0')
    {
        goto DONE;
    }
    // if the replace string is the same as the string to replace, we're done
    if (CompareStringW(psz2Rep, pszReplace, ignoreCase))
    {
        br = TRUE;
        goto DONE;
    }
    
    // count the number of occurences of psz2Rep
    // perf hit, but what do we do for reallocation which could be worse
    while(TRUE)
    {
        pszFind = FindStringW(pszCurr, psz2Rep, ignoreCase);
        if(!pszFind)
        {
            break;
        }
        pszCurr = pszFind + len2rep;
        count ++;
    }
    // check to see if we have a string to replace;
    if (count == 0)
    {
        goto DONE;
    }

    int totalLen = slen(m_pstr);
    // this will tell us if we have to reallocate the memory
    if (len2rep < repLen)
    {
        totalLen -= len2rep*count;
        if (totalLen < 0)
        {
            // possibly throw since we should not hit this
            goto DONE;
        }
    }
    br = TRUE;
    
    int     len;
    LPWSTR   pszNew      = NULL;
    LPWSTR   pNew        = NULL;
    int     remaining   = 0;
    LPWSTR pOld          = m_pstr;
    
    
    totalLen += repLen*count;

    pszNew = MyAlloc(LPWSTR, totalLen + sizeof(WCHAR));
    
    pNew = pszNew;
    // walk the string and replace each occurrence of string
    while(TRUE)
    {
        pszFind = FindStringW(pOld, psz2Rep, ignoreCase);
        if (!pszFind)
        {
            if (remaining > 0)
            {
                HRESULT hr = StringCchCatW(pszNew, totalLen + sizeof(WCHAR), pszCurr);
                if (!SUCCEEDED(hr))
                {
                    MyFree(pszNew);
                    br = FALSE;
                    goto DONE;
                }
            }
            break;
        }
        len = (pszFind - pOld);
        if (len > 0)
        {
            memcpy(pNew, pOld, len);
            pNew += len;
        }
        memcpy(pNew, pszReplace, repLen);
        pNew    += repLen;
        pOld    = pszFind + len2rep;
        remaining = slen(pOld);
    }
    if (pszNew)
    {
        MyFree(m_pstr);
        m_pstr = pszNew;
        m_len = slen(pszNew);
    }
DONE:
    return br;
}
if (repLen <= len2rep)
{
    // in this case, we need not make the string bigger, just replace
    // the necessary strings
    //
    LPWSTR p1 = m_pstr;
    LPWSTR p = p1;
    LPWSTR pNext;
    int dcb = len2rep - repLen;
    do
    {
        p1 = FindStringW(pszCurr, psz2Rep, ignoreCase);
        if (p1)
        {
            memcpy(p1, pszReplace, repLen);
            p1 += repLen;
            pszCurr = p1;
            // copy string bytes forward
            if (repLen < len2rep)
            {
                p = p1;
                pNext = p1 + dcb;
                while (*pNext)
                {
                    *p = *pNext;
                    p++;
                    pNext++;
                }
                // go ahead and 0 out the remaining bytes
                while (*p)
                {
                    *p = '\0';
                    p++;
                }

            }
        }
    } while (p1);
}
else
{
    // otherwise, the buffer size will need to be adjusted by at least the 
    // difference in characters
    LPWSTR   pszFind;
    LPWSTR   pszNew;
    LPWSTR   pNew;//        = NULL;
    int     remaining;
    int     ccFind = 1;
    int     totalLen = m_len + (repLen - len2rep);
    int     len, newRemaining, offset;
    //totalLen += repLen;
    remaining = m_len;
    newRemaining = totalLen;
    // in this case, the string will need to be made larger for the new string
    //
    //pszNew = MyAlloc(LPWSTR, (totalLen + 1) * sizeof(WCHAR));
    pszNew = NULL;
    if (!AllocStringW(&pszNew, totalLen))
    {
        return FALSE;
    }
    pNew = pszNew;
    // walk the string and replace each occurrence of string
    while (pszNew)
    {
        // find the string!
        pszFind = FindStringW(pszCurr, psz2Rep, ignoreCase);
        if (!pszFind && pszNew)
        {
            if (remaining > 0)
            {
                if (newRemaining < remaining)
                {
                    totalLen += (remaining - newRemaining);
                    if (!AllocStringW(&pszNew, totalLen))
                    {
                        if (pszNew)
                        {
                            MyFree(pszNew);
                            br = FALSE;
                            goto DONE;
                        }
                    }
                }
                HRESULT hr = StringCchCatW(pszNew, totalLen + sizeof(WCHAR), pszCurr);
                if (!SUCCEEDED(hr))
                {
                    MyFree(pszNew);
                    br = FALSE;
                    goto DONE;
                }
            }
            if (*pszNew)
            {
                m_pstr = pszNew;
            }
            break;
        }
        // calculate the first length
        len = _OFFSET(pszFind, pszCurr);
        if (newRemaining < len)
        {
            totalLen += (len - newRemaining) + repLen;
            if (!AllocStringW(&pszNew, totalLen))
            {
                br = FALSE;
                goto DONE;
            }
            pNew = pszNew + offset;
        }
        // if we have some bytes here copy them now
        memcpy(pNew, pszCurr, len);
        pNew += len;
        offset = _OFFSET(pNew, pszNew);
        newRemaining = totalLen - offset;
        if (newRemaining < repLen)
        {
            totalLen += (repLen - newRemaining) + (repLen - len2rep);
            if (!AllocStringW(&pszNew, totalLen))
            {
                br = FALSE;
                goto DONE;
            }
            pNew = pszNew + offset;
        }
        // if we get this far, we can copy the memory over to the location
        memcpy(pNew, pszReplace, repLen);
        pNew += repLen;
        offset = _OFFSET(pNew, pszNew);
        newRemaining = totalLen - offset;
        pszCurr = pszFind + len2rep;
        remaining = m_len - _OFFSET(pszCurr, m_pstr);

    }
}
#else
BOOL TStringW::Replace(__in LPWSTR psz2Rep, __in LPWSTR pszReplace, BOOL ignoreCase)
{
    BOOL    br          =   FALSE;
    int     repLen      =   wlen(pszReplace);
    int     len2rep     =   wlen(psz2Rep);
    LPWSTR   pszCurr     =   m_pstr;
    // check for NULL pointers
    //
    if (psz2Rep == NULL || pszReplace == NULL)
    {
        goto DONE;
    }
    // check for NULL strings
    if (*psz2Rep == '\0' || *pszReplace == '\0')
    {
        goto DONE;
    }
    // if the replace string is the same as the string to replace, we're done
    if (CompareStringW(psz2Rep, pszReplace, ignoreCase))
    {
        br = TRUE;
        goto DONE;
    }
    
    br = TRUE;
    // if the string is less than the one we are replacing, we can just use 
    // the existing buffer
    
    int count = 0;
    LPWSTR pszFind = m_pstr;
    //LPWSTR pszwLast = pszFind;
    // count the occurrencese so we know if we need to allocate memory
    while ((pszFind = FindStringW(pszFind, psz2Rep, ignoreCase)) != NULL)
    {
        pszFind += len2rep;
        count ++;
    }
    if (count == 0)
    {
        goto DONE; // nothing to do
    }
    len2rep *= count;
    repLen *= count;
    int tmp_size = m_size; // m_size is in count, not bytes
    tmp_size -= len2rep;
    tmp_size += repLen;
    LPWSTR pszwNew = NULL;
    // allocate it if you need it.
    if (tmp_size >= m_size)
    {
        int tmp = tmp_size + 10;
        if (!AllocStringW(&pszwNew, tmp))
        {
            br = FALSE;
            goto DONE;
        }
        else
        {
            m_size = tmp;
            HRESULT hr = CopyStringW(pszwNew, m_pstr, tmp);
            //HRESULT hr = StringCchCopyW(pszwNew, tmp, m_pstr);
            if (!SUCCEEDED(hr))
            {
                MyFree(pszwNew);
                goto DONE;
            }
            m_pstr = pszwNew;
        }
    }
    // recalibrate the lengths
    repLen /=count;
    len2rep /= count;
    LPWSTR psztmp = m_pstr; // + lastOffset;
    int len = wlen(psztmp);
    if (repLen >= len2rep)
    {
        int remaining = repLen - len2rep;
        for (int c = 0; c < count; c++)
        {
            pszFind = FindStringW(psztmp, psz2Rep, ignoreCase);
            if (pszFind)
            {
                len = wlen(pszFind);
                LPWSTR pszwCurr = pszFind + remaining + wlen(pszFind);
                LPWSTR pszw = pszFind + wlen(pszFind);
                for (int i = 0; i < len; i++)
                {
                    *pszwCurr = *pszw;
                    pszwCurr --;
                    pszw --;
                }
                psztmp = pszFind;
                LPWSTR pszRep = pszReplace;
                for (int i = 0; i < repLen; i ++)
                {
                    *psztmp = *pszRep;
                    psztmp ++;
                    pszRep++;
                }
            }
        }
    }
    else
    {
        if (repLen == len2rep)
        {
            for (int c = 0; c < count; c++)
            {
                pszFind = FindStringW(psztmp, psz2Rep, ignoreCase);
                if (pszFind)
                {
                    psztmp = pszFind;
                    LPWSTR pszRep = pszReplace;
                    for (int i = 0; i < repLen; i++)
                    {
                        *psztmp = *pszRep;
                        psztmp++;
                        pszRep++;
                    }
                }
            }
        }
        else
        {
            int remaining = len2rep - repLen;
            for (int c = 0; c < count; c++)
            {
                pszFind = FindStringW(psztmp, psz2Rep, ignoreCase);
                if (pszFind)
                {
                    psztmp = pszFind;
                    LPWSTR pszRep = pszReplace;
                    for (int i = 0; i < repLen; i++)
                    {
                        *psztmp = *pszRep;
                        psztmp++;
                        pszRep++;
                    }

                    len = wlen(psztmp);
                    LPWSTR pszwCurr = psztmp + remaining;
                    LPWSTR pszw = psztmp;
                    for (int i = 0; i < len; i++)
                    {
                        *pszw= *pszwCurr;
                        pszwCurr++;
                        pszw++;
                    }
                }
            }
        }
    }
    m_len = wlen(m_pstr);
DONE:
    return br;
}
#endif
BOOL TStringW::Remove(__in LPWSTR pszRemove)
{
  return Remove(pszRemove, FALSE);
}

BOOL TStringW::Remove(__in LPWSTR pszRemove, BOOL ignoreCase)
{
  BOOL          bFound          = FALSE;
  COMPARESTRINGW compstr         = NULL;

  // check for valid object and input
  //
  if (!m_pstr || !pszRemove)
  {
    return NULL;
  }
  // intialize some locals
  TStringW   ts;
  LPWSTR    psz;
  LPWSTR    pszCurr     =   m_pstr;
  int       len         =   wlen(pszRemove);
  int       currLen     =   wlen(pszCurr);
  int       i           = 0;
  // ignore case
  if (!Find(pszRemove, ignoreCase))
  {
    return FALSE;
  }
  // setup the comparison functions by looking at the ignore flag
  compstr = GetCompareFunction(ignoreCase);
  // walk the string looking for candidates to replace
  //
  if (!ts.InitEmptyString(m_len + 1))
  {
    return FALSE;
  }
  psz = ts.GetString();
  while(*pszCurr)
  {
    // if we reach the end of the string and we care, append the remainder of the string
    //
    if (currLen < len)
    {
      if (bFound)
      {
        psz[i] = '\0';
      }
      break;
    }
    // if we find the string, then remove it without prejudice
    if (compstr(pszCurr, pszRemove, len) == 0)
    {
      bFound = TRUE;
      i += len;
      pszCurr += len;
      currLen -= len;
      continue;
    }
    psz[i] = *pszCurr;
    i++;
    pszCurr ++;
    currLen --;
  }
  // now that we have the string in place, intialize this object with the new string
  return Init(ts.GetString());
}

BOOL TStringW::RemoveChar(WCHAR tch)
{
  return RemoveChar(tch, FALSE);
}

BOOL TStringW::RemoveChar(WCHAR tch, BOOL ignoreCase)
{
  if (!m_pstr)
  {
    return FALSE;
  }
  TStringW ts;
  ts.InitEmptyString(m_len);
  LPWSTR pszNew = ts.GetString();
  int i = 0;
  LPWSTR pszCurr = m_pstr;
  while(*pszCurr)
  {
    if (ignoreCase)
    {
      WCHAR w1 = tch,w2 = *pszCurr;
      if (towlower(w1) != towlower(w2))
      {
        pszNew[i] = tch;
      }
    }
    else
    {
      if (tch != *pszCurr)
      {
        pszNew[i] = tch;
      }
    }
    i ++;
    pszCurr ++;
  }
  return Init(pszNew);
}

BOOL TStringW::ReplaceChar(WCHAR tch, WCHAR tchNew)
{
  return ReplaceChar(tch, tchNew, FALSE);
}

BOOL TStringW::ReplaceChar(WCHAR tch, WCHAR tchNew, BOOL ignoreCase)
{
  if (!m_pstr)
  {
    return FALSE;
  }
  TStringW ts;
  ts.InitEmptyString(m_len);
  LPWSTR pszNew = ts.GetString();
  int i = 0;
  LPWSTR pszCurr = m_pstr;
  while(*pszCurr)
  {
    if (ignoreCase)
    {
      WCHAR w1 = towlower(tch),w2 = towlower(*pszCurr);
      if (w1 == w2)
      {
        pszNew[i] = tch;
      }
      else
      {
        pszNew[i] = *pszCurr;
      }
    }
    else
    {
      if (tch == *pszCurr)
      {
        pszNew[i] = tch;
      }
      else
      {
        pszNew[i] = *pszCurr;
      }
    }
    i ++;
    pszCurr ++;
  }
  return Init(pszNew);
}

BOOL TStringW::Remove(int pos)
{
    BOOL br = TRUE;
    if (!m_pstr)
    {
        br = FALSE;
    }
    else
    {
        int len = wcslen(m_pstr);
        if (len == 0 || pos >= len)
        {
            br = FALSE;
        }
        else
        {
            for(int i = pos; i < len; i ++)
            {
                m_pstr[i] = m_pstr[i + 1];
            } 
            m_pstr[len - 1] = '\0';
            m_len --;
        }
    }
    return TRUE;
}

BOOL TStringW::Remove(int pos, int len)
{
    BOOL br = TRUE;
    if (!m_pstr || m_len < (pos + len))
    {
        br = FALSE;
    }
    else
    {
        if (len == 0)
        {
            br = Remove(pos);
        }
        else
        {
            int start = pos + len;
            for (int i = start, j = pos; i < m_len; i ++, j ++)
            {
                m_pstr[j] = m_pstr[i];
            }
            
            for (int k = start; k < m_len; k ++)
            {
                m_pstr[k] = '\0';
            }
            m_len -= len;
            m_pstr[m_len] = '\0';
        }
    }
    return br;
}

BOOL TStringW::Remove()
{
    if (!m_pstr)
    {
        return FALSE;
    }
    if (m_len == 0)
    {
        return FALSE;
    }
    else
    {
        m_pstr[m_len - 1] = '\0';
        m_len --;
    }
    return TRUE;    
}

BOOL TStringW::Insert(WCHAR ch, int pos)
{
	WCHAR szw[2];
	szw[0] = ch;
	szw[1] = '\0';
	return Insert(szw, pos);
}

BOOL TStringW::Insert(LPWSTR psz, int pos)
{
    BOOL br = TRUE;
    if (pos == m_len)
	{
		return Concat(psz);
	}
	if(psz && m_pstr)
    {
		//LPWSTR p = psz;
		int ccString   = wlen(psz);
		if (ccString    + m_len >= m_size)
		{
			m_size += ccString    + 10;
			//m_pstr = MyRealloc(LPSTR, m_pstr, m_size);
			if (!AllocStringW(&m_pstr, m_size))
			{
				br = FALSE;
			}
		}
		if (br)
		{
			int len2move = m_len - pos;
			LPWSTR p1 = m_pstr + (m_len - 1) + ccString;
			LPWSTR p2 = m_pstr + (m_len - 1);
			
			for (int i = len2move; i > 0; i --, p1 --, p2 --)
			{
				*p1 = *p2;
				*p2 = '\0';
			}
			p2 = m_pstr + pos;
			
			for (int i = 0; i < ccString; i ++)
			{
				p2[i] = psz[i];
			}
			m_len = m_len + ccString;
		}
    }
    return br;
}


int TStringW::GetLength(void)
{
  return m_len;
}

WCHAR TStringW::GetChar(int pos)
{
    if (!m_pstr)
    {
        return -1;
    }
    if (pos >= m_len || pos < 0)
    {
        return -1;
    }
        return m_pstr[pos];
}

BOOL TStringW::Concat(__in LPWSTR psz)
{
    LPWSTR tmp = NULL;
    if (!psz)
    {
        return FALSE;
    }
    if (!m_pstr)
    {
        return Init(psz);
    }
    int len = wlen(psz);
    if (m_len + len >= (int)(m_size/sizeof(WCHAR)))
    {
        m_size += 10 + len*sizeof(WCHAR);
        if (!AllocStringW(&tmp, m_size))
        {
            return FALSE;
        }
        if (SUCCEEDED(CopyStringW(tmp, m_pstr, m_len)))
        {
        //if (!SUCCEEDED(StringCchCopyW(tmp, m_len + 1, m_pstr)))
        //{
            return FALSE;
        }
        m_pstr = tmp;
    }
    
    m_len += (len);

    if (SUCCEEDED(StringCchCatW(m_pstr, m_size, psz)))
    {
        return TRUE;
    }
      
    return FALSE;
}

//Define assignment operator.
TStringW TStringW::operator=( LPWSTR psz)
{
  if (Init(psz))
  {
    return m_pstr;
  }
  return NULL;
}

TStringW TStringW::operator=( TStringW ts)
{
  if (Init(ts.GetString()))
  {
    return m_pstr;
  }
  return NULL;
}

TStringW TStringW::operator=( LPSTR psz)
{
  if (Init(psz))
  {
    return m_pstr;
  }
  return NULL;
}

TStringW TStringW::operator+=( LPWSTR psz)
{
  if (!this->m_pstr)
  {
    if (Init(psz))
    {
      return m_pstr;
    }
  }
  else
  {
    this->Concat(psz);
    return m_pstr;
  }
  return NULL;
}

TStringW TStringW::operator+=( TStringW ts)
{
  if (!this->m_pstr)
  {
    if (Init(ts.GetString()))
    {
      return m_pstr;
    }
  }
  else
  {
    this->Concat(ts.GetString());
    return m_pstr;
  }
  return NULL;
}

TStringW TStringW::operator+=( __in WCHAR wch)
{
    WCHAR wsz[2];
    wsz[0] = wch;
    wsz[1] = L'\0';
    Concat(wsz);
    return m_pstr;
}

BOOL TStringW::operator ==(__in LPWSTR psz)
{
  return IsEqual(psz);
}

BOOL TStringW::operator ==(TStringW ts)
{
	return IsEqual(ts.GetString());
}
BOOL TStringW::IsEqual(__in LPWSTR psz)
{
    return IsEqual(psz, FALSE);
}

BOOL TStringW::IsEqual(__in LPWSTR psz, BOOL bIgnoreCase)
{
    if (!m_pstr || !psz)
    {
        return FALSE;
    }  
    return CompareStringW(psz, m_pstr, bIgnoreCase);
}

BOOL TStringW::IsEmpty()
{
    return (m_pstr == NULL || *m_pstr == L'\0');
}

BOOL TStringW::Init(__in LPWSTR psz)
{
  if (!psz)
  {
    return FALSE;
  }
  m_len = (int)wlen(psz);
  m_size = m_len + 10;
  if (m_pstr)
  {
    MyFree(m_pstr);
  }
  
  if (AllocStringW(&m_pstr, m_size))
  {
    if (SUCCEEDED(CopyStringW(m_pstr,psz, m_len)))
    {
    //if (SUCCEEDED(StringCchCopyW(m_pstr, m_len + 1, psz)))
    //{
      return TRUE;
    }
    else
    {
      MyFree(m_pstr);
      m_len = 0;
    }
  }
  else
  {
	  m_size = m_len = 0;
  }
  return FALSE;
}

// only init a sub string of len bytes
BOOL TStringW::Init(__in LPWSTR pszSrc, int len)
{
  if (len == 0 || pszSrc == NULL)
  {
    return FALSE;
  }
  if (m_pstr)
  {
    MyFree(m_pstr);
  }
  int m_len = len + 1;
  m_size = m_len + 20;
  //m_pstr = MyAlloc(__in LPWSTR, (m_len + 1)*sizeof(WCHAR));
  
  if (AllocStringW(&m_pstr, m_size))
  {
    if (SUCCEEDED(CopyStringW(m_pstr, pszSrc, m_len)))
    {
    //if (SUCCEEDED(StringCchCopyW(m_pstr, m_len + 1, pszSrc)))
    //{
		m_pstr[m_len] = L'\0';
		return TRUE;
    }
    else
    {
		MyFree(m_pstr);
		m_size = m_len = 0;
    }
  }
  else
  {
		m_size = m_len = 0;
  }
  return FALSE;
}

BOOL TStringW::Init(__in LPSTR psz)
{
  if (!psz)
  {
    return FALSE;
  }
  m_len = (int)slen(psz);
  m_size = m_len + 20;
  if (m_pstr)
  {
    MyFree(m_pstr);
  }
  
  if (AllocStringW(&m_pstr, m_size))
  {
    if (ConvertToUnicode(psz, m_pstr))
    {
      return TRUE;
    }
    else
    {
      MyFree(m_pstr);
      m_len = m_size = 0;
    }
  }
  else
  {
      m_len = m_size = 0;
  }
  return FALSE;
}

LPWSTR TStringW::Search(__in LPWSTR psz)
{
  return wcsstr(psz, m_pstr);
}

BOOL TStringW::InitEmptyString(int size)
{
  if (m_pstr)
  {
    MyFree(m_pstr);
  }
  m_len = size; 
  m_size = m_len + 10;
  //m_pstr = MyAlloc(__in LPWSTR, (m_len + 1)*sizeof(WCHAR));
  if (AllocStringW(&m_pstr, m_size))
  {
    return TRUE;
  }
  return FALSE;
}

BOOL TStringW::InitFrom(HINSTANCE hinst, int idsString)
{
  if (m_pstr)
  {
    MyFree(m_pstr);
  }
  //m_pstr = MyAlloc(__in LPWSTR, 0x400*sizeof(WCHAR));
  //if (!m_pstr)
  //{
  //  return FALSE;
  //}
  m_size = 0x400;
  if (AllocStringW(&m_pstr, m_size))
  {
	  m_len = LoadStringW(hinst, idsString, m_pstr, 0x400);
	  if ( m_len == 0)
	  {
		  m_size = 0;
		MyFree(m_pstr);
		return FALSE;
	  }
  }
  else
  {
		  m_size = m_len = 0;
		  return FALSE;
  }
  return TRUE;
}

BOOL TStringW::Compare(__in LPWSTR psz)
{
  return Compare(psz, FALSE);
}

BOOL TStringW::Compare(__in LPWSTR psz, BOOL ignoreCase)
{
  if (psz)
  {
    DWORD dwLen = wlen(psz);
    return Compare(psz, ignoreCase, dwLen);
  }
  return FALSE;
}

BOOL TStringW::Compare(__in LPWSTR psz, BOOL ignoreCase, DWORD dwLen)
{
  if (m_pstr && psz)
  {
    COMPARESTRINGW compstr =   GetCompareFunction(ignoreCase);
    return compstr(m_pstr, psz, dwLen) == 0;
  }
  return FALSE;
}

TStringW * TStringW::Split(WCHAR ch)
{
  if (!m_pstr)
  {
    return NULL;
  }
  else
  {
    if (!wcschr(m_pstr, ch))
    {
      return new TStringW(m_pstr);
    }
    else
    {
      LPWSTR psz = m_pstr;
      int count = 0;
      TStringW * ts = new TStringW();
      if (ts->InitEmptyString(m_len + 1))
      {
        LPWSTR psz2 = ts->GetString();
        while(*psz)
        {
          if (*psz == ch)
          {
            *psz2 = '\0';
          }
          else
          {
            *psz2 = *psz;
          }
          psz ++;
          psz2 ++;
        }
        *psz2 = '\0';
        return ts;
      }
    }
  }
  return NULL;
}

TStringW * TStringW::Substring(int pos)
{
    if (m_pstr == NULL || pos > m_len)
    {
        return NULL;
    }
    return new TStringW(m_pstr + pos);
}

TStringW * TStringW::Substring(int pos, int len)
{
    if (m_pstr == NULL || pos > m_len)
    {
        return NULL;
    }
    TStringW * ret = new TStringW();
    ret->InitEmptyString(len + 1);
    LPWSTR psz = ret->GetString();
    for (int i = pos, j = 0; i < len; i ++, j++)
    {
       psz[j] = m_pstr[i];
    }
    psz[len] = '\0';
    return ret;
}

BOOL TStringW::Reverse()
{
  if (!m_pstr)
  {
    return FALSE;
  }
  //int len = (int)wcslen(m_pstr);
  //if (len <= 1)
  //{
  //  return TRUE;
  //}
  //WCHAR tmp;
  //LPWSTR pszStart = m_pstr;
  //LPWSTR pszEnd = m_pstr + len;
  //while(TRUE)
  //{
  //  tmp = *pszStart;
  //  *pszStart = *pszEnd;
  //  *pszEnd = tmp;
  //  pszStart ++;
  //  pszEnd --;
  //  if (pszStart >= pszEnd)
  //  {
  //      break;
  //  }
  //}
  //return TRUE;
  m_pstr = _wcsrev(m_pstr);
  return TRUE;
}

