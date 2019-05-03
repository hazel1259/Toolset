/************************************************************************************************************************

  REGAPI.CPP
  This module implements a functional class of methods for accessin the registry
  
**************************************************************************************************************************/
  

#include "comm_inc.h"
#include "RegApi.h"

BOOL CRegKeyInfo::GetInfo()
{
	if (!m_key)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}
	LONG lret = 
	RegQueryInfoKey(
		m_key,
	  m_ClassName,
	  &m_cClass,
	  NULL,
	  &m_cSubKeys,
	  &m_cMaxSubKeyLen,
	  &m_cMaxClassLen,
	  &m_cValues,
	  &m_cMaxValueNameLen,
	  &m_cMaxValueLen,
	  &m_cbSecurityDescriptor,
		&m_LastWriteTime
	);
	
	if (lret == ERROR_MORE_DATA)
	{
		m_ClassName = MyAlloc(LPSTR, m_cClass + sizeof(TCHAR));
		if (!m_ClassName)
		{
			return FALSE;
		}
		lret = 
		RegQueryInfoKey(
			m_key,
			m_ClassName,
			&m_cClass,
			&m_Reserved,
			&m_cSubKeys,
			&m_cMaxSubKeyLen,
			&m_cMaxClassLen,
			&m_cValues,
			&m_cMaxValueNameLen,
			&m_cMaxValueLen,
			&m_cbSecurityDescriptor,
			&m_LastWriteTime
		);	
	}

	SetLastError(lret);
	return (lret == ERROR_SUCCESS);

}
/************************************************************************************************************************
  Name: CRegApi()
  Type: constructor
  Scope: public
  Base:
  Paramters:
  
  returns
  
  notes
  
**************************************************************************************************************************/
CRegApi::CRegApi(void)
: m_hkey(NULL)
, m_access(KEY_ALL_ACCESS)
{
}

/************************************************************************************************************************
  Name: ~CRegApi()
  Type: destructor
  Scope: public
  Base:
  Paramters:
  
  returns
  
  notes
          if the registry key ois open, it closes it
**************************************************************************************************************************/
CRegApi::~CRegApi(void)
{
  Close();
}

/************************************************************************************************************************
  Name: Open()
  Type: method
  Scope: public
  Base:
  Paramters:
              HKEY hkeyRoot - root key of the path to open
              LPSTR pszPath - path to open as a key
  returns:
  
            TRUE if the key and path were opened
  
  notes
  
        This method first closes any outstanding open keys, and then opens the key + path specified using hkeyRoot as 
        the root, and path as the key
        
**************************************************************************************************************************/
BOOL CRegApi::Open(HKEY hkeyRoot, LPSTR pszPath)
{
  if (NULL != m_hkey)
  {
    Close();
  }
  DWORD dwReturn =  RegOpenKeyEx(hkeyRoot, pszPath, 0, m_access, &m_hkey);
  if (ERROR_SUCCESS != dwReturn)
  {
    SetLastError(dwReturn);
    return FALSE;
  }
	m_name = MyAlloc(LPSTR, slen(pszPath) + 1);
	if (m_name)
	{
		strcpy_s(m_name, slen(pszPath) + 1, pszPath);
	}
  return TRUE;
}

/************************************************************************************************************************
  Name: Open()
  Type: method
  Scope: public
  Base:
  Paramters:
              HKEY hkeyRoot - root key of the path to open
              LPSTR pszPath - path to open as a key
              BOOL fCreate - true indicates the key is created, false means it should not be created  
  returns:
  
            TRUE if the key and path were opened
  
  notes
  
        This overloaded method first closes any outstanding open keys, and then opens the key + path specified. If they
        do not exist, this method will create them
        
**************************************************************************************************************************/
BOOL CRegApi::Open(HKEY hkeyRoot, LPSTR pszPath, BOOL fCreate)
{
  
  if (NULL != m_hkey)
  {
    Close();
  }
  DWORD dwReturn =  RegOpenKeyEx(hkeyRoot, pszPath, 0, KEY_ALL_ACCESS, &m_hkey);
	
  if (ERROR_SUCCESS != dwReturn)
  {
    DWORD dwDisposition;
    
    dwReturn = RegCreateKeyEx(
      hkeyRoot, 
      pszPath, 
      0, 
      NULL, 
      REG_OPTION_NON_VOLATILE, 
      KEY_ALL_ACCESS, 
      NULL, 
      &m_hkey,
      &dwDisposition
      );
     if (ERROR_SUCCESS != dwReturn)
     {
      SetLastError(dwReturn);
      return FALSE;
     }
  }
	m_name = MyAlloc(LPSTR, slen(pszPath) + 1);
	if (m_name)
	{
		strcpy_s(m_name, slen(pszPath) + 1, pszPath);
	}
  return TRUE;
}

/************************************************************************************************************************
  Name: Close()
  Type: method
  Scope: public
  Base:
  Paramters:
  
  returns:
  
            TRUE if the key is closed. 
  
  notes
  
        
**************************************************************************************************************************/
BOOL CRegApi::Close()
{
  LONG lret = ERROR_SUCCESS;
  if (NULL != m_hkey)
  {
    lret = RegCloseKey(m_hkey);
    m_hkey = NULL;
    if (lret != ERROR_SUCCESS)
    {
      SetLastError(lret);
      return FALSE;
    }
		if (NULL != m_name)
		{
			MyFree(m_name);
		}
    return TRUE;
  }
  return FALSE;
}

/************************************************************************************************************************
  Name: Close()
  Type: method
  Scope: public
  Base:
  Paramters:
            LPSTR pszValueName - name of the value to retrieve
            PDWORD pdw - pointer to a DWORD to recieve the value
  returns:
  
            TRUE if the data was retrieved
  
  notes
  
        
**************************************************************************************************************************/
BOOL CRegApi::ReadRegDWord(LPSTR pszValueName, PDWORD pdw)
{
  DWORD dwType = REG_DWORD;
  DWORD dwSize = sizeof(DWORD);
  if (NULL == m_hkey)
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }
  LONG lret = RegQueryValueEx(m_hkey, pszValueName, NULL, &dwType, (LPBYTE)pdw, &dwSize);
  if (lret != ERROR_SUCCESS)
  {
    SetLastError(lret);
    return FALSE;
  }
  return TRUE;
}

/************************************************************************************************************************
  Name: ReadRegString()
  Type: method
  Scope: public
  Base:
  Paramters:
              LPSTR pszValueName - name of the value to retrieve
              LPSTR pszValue - pointer to a string to recieve the data
              SIZE_T value_size  - size of the string recieving the data
  returns:
  
            TRUE if the data was retrieved
  
  notes
  
        
**************************************************************************************************************************/
BOOL CRegApi::ReadRegString(LPSTR pszValueName, LPSTR pszValue, SIZE_T value_size)
{
  
  DWORD dwType = REG_SZ;
  DWORD dwSize = (DWORD)value_size; 
  LONG lret = RegQueryValueEx(m_hkey, pszValueName, NULL, &dwType, (LPBYTE)pszValue, &dwSize);
  if (lret != ERROR_SUCCESS)
  {
    SetLastError(lret);
    return FALSE;
  }
  return TRUE;
}


/************************************************************************************************************************
  Name: WriteRegString()
  Type: method
  Scope: public
  Base:
  Paramters:
              LPSTR pszValueName - name of the value to retrieve
              LPSTR pszValue - pointer to a string to recieve the data
              SIZE_T value_size  - size of the string recieving the data
  returns:
  
            TRUE if the data was retrieved
  
  notes
  
        
**************************************************************************************************************************/
BOOL CRegApi::WriteRegString(LPSTR pszValueName, LPSTR pszValue, DWORD value_size)
{
  if (NULL == m_hkey)
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }
  DWORD dwType = REG_SZ;
  LONG lret = ::RegSetValueEx(m_hkey, pszValueName, 0, dwType, (const BYTE *)pszValue, value_size);
  if (ERROR_SUCCESS != lret)
  {
    SetLastError(lret);
    return FALSE;
  }
  return TRUE;
}

/************************************************************************************************************************
  Name: RegWriteDword()
  Type: method
  Scope: public
  Base:
  Paramters:
              LPSTR pszValueName - name of the value to retrieve
              LPSTR pszValue - pointer to a string to recieve the data
  returns:
  
            TRUE if the data was retrieved
  
  notes
  
        
**************************************************************************************************************************/
BOOL CRegApi::WriteRegDword(LPSTR pszValueName , DWORD dwValue)
{
  if (NULL == m_hkey)
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }
  
  DWORD dwType = REG_DWORD;
  LONG lret = ::RegSetValueEx(m_hkey, pszValueName, 0, dwType, (const BYTE *)&dwValue, sizeof(DWORD));
  
  if (ERROR_SUCCESS != lret)
  {
    SetLastError(lret);
    return FALSE;
  }
  return TRUE;
}


/************************************************************************************************************************
  Name: DeleteRegValue()
  Type: method
  Scope: public
  Base:
  Paramters:
              LPSTR pszValueName - name of the value to delete
  returns:
  
            TRUE if the value was deleted from the opened key
  
  notes
  
        
**************************************************************************************************************************/
BOOL CRegApi::DeleteRegValue(LPSTR pszValueName)
{
  if (NULL == m_hkey)
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }
  
  LONG lret = ::RegDeleteValue(m_hkey, pszValueName);
  
  if (ERROR_SUCCESS != lret)
  {
    SetLastError(lret);
    return FALSE;
  }
  return TRUE;
}

/************************************************************************************************************************
  Name: DeleteRegKey()
  Type: method
  Scope: public
  Base:
  Paramters:
              LPSTR pszValueName - name of the value to delete
  returns:
  
            TRUE if the value was deleted from the opened key
  
  notes
  
        
**************************************************************************************************************************/
BOOL CRegApi::DeleteRegKey(void)
{
  return this->DeleteRegKey(NULL);
}

/************************************************************************************************************************
  Name: DeleteRegKey()
  Type: method
  Scope: public
  Base:
  Paramters:
              LPSTR pszValueName - name of the value to delete
  returns:
  
            TRUE if the value was deleted from the opened key
  
  notes
  
        
**************************************************************************************************************************/
BOOL CRegApi::DeleteRegKey(LPSTR pszSubKey)
{
  if (NULL == m_hkey)
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return FALSE;
  }
  
  LONG lret = ::RegDeleteKey(m_hkey, pszSubKey);
  
  if (ERROR_SUCCESS != lret)
  {
    SetLastError(lret);
    return FALSE;
  }
  return TRUE;
}
#if 0
const CRegApi * CRegApi::GetFirstChild()
{
	m_reg_info.GetInfo(m_hkey);
	LPSTR pszName = MyAlloc(LPSTR, m_reg_info.m_cMaxSubKeyLen  + sizeof(TCHAR));
	if (!pszName)
	{
		return NULL;
	}
	m_currIndex = 0;
	LONG lret = RegEnumKey(m_hkey, m_currIndex, pszName, m_reg_info.m_cMaxSubKeyLen);
	if (lret == ERROR_SUCCESS)
	{
		return NULL;
	}
	CRegApi * reg = new CRegApi();
	reg->Open(this->m_hkey, pszName);
	MyFree(pszName);
	return (const CRegApi *)reg;
}

const CRegApi * CRegApi::GetNextChild()
{
	m_reg_info.GetInfo(m_hkey);
	LPSTR pszName = MyAlloc(LPSTR, m_reg_info.m_cMaxSubKeyLen  + sizeof(TCHAR));
	if (!pszName)
	{
		return NULL;
	}
	m_currIndex ++;
	LONG lret = RegEnumKey(m_hkey, m_currIndex, pszName, m_reg_info.m_cMaxSubKeyLen);
	if (lret == ERROR_SUCCESS)
	{
		return NULL;
	}
	CRegApi * reg = new CRegApi();
	reg->Open(this->m_hkey, pszName);
	MyFree(pszName);
	return (const CRegApi *)reg;
}
#endif
const CRegKeyInfo & CRegApi::GetKeyInfo()
{
	CRegKeyInfo & ret = m_reg_info;
	m_reg_info.GetInfo(m_hkey);
	return ret;
}

BOOL CRegValue::GetValue(LPSTR pszValue)
{
	SetLastError(0);
	LONG lret = RegQueryValueEx(m_owner, pszValue, NULL, &m_valueType, (LPBYTE)m_value, &m_valueLen);
	if (ERROR_SUCCESS != lret)
	{
		SetLastError(lret);
		return FALSE;
	}
	return TRUE;
}