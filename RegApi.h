#pragma once
#include "tarray.h"


class CRegKeyInfo
{
public:
	CRegKeyInfo()
	{
		Init();
	}
	CRegKeyInfo(HKEY hKey)
	{
		m_key = hKey;
		Init();
	}
	
	~CRegKeyInfo()
	{
		if (m_ClassName) 
		{
			HeapFree(GetProcessHeap(),0, m_ClassName);
			m_ClassName = NULL;
		}
	}
	void Init(HKEY hkey)
	{
		m_key = hkey;
		Init();
	}
	void Init()
	{
		  m_ClassName = 0;
		  m_cClass = 0;
		  m_Reserved = 0;
		  m_cSubKeys = 0;
		  m_cMaxSubKeyLen = 0;
		  m_cMaxClassLen = 0;
		  m_cValues = 0;
		  m_cMaxValueNameLen = 0;
		  m_cMaxValueLen = 0;
		  m_cbSecurityDescriptor = 0;
			ZeroMemory(&m_LastWriteTime, sizeof(m_LastWriteTime));
	}
	BOOL GetInfo(HKEY hkey)
	{
		m_key = hkey;
		return GetInfo();
	}
	BOOL GetInfo();
  LPSTR m_ClassName;
  DWORD m_cClass;
  DWORD m_Reserved;
  DWORD m_cSubKeys;
  DWORD m_cMaxSubKeyLen;
  DWORD m_cMaxClassLen;
  DWORD m_cValues;
  DWORD m_cMaxValueNameLen;
  DWORD m_cMaxValueLen;
  DWORD m_cbSecurityDescriptor;
  FILETIME m_LastWriteTime;
private:
	HKEY m_key;
};

class CRegValue
{
public:
	CRegValue() 
	{
		Init();
	}
	
	
	CRegValue(HKEY hkeyOwner) 
	{ 
		Init();
		m_owner = hkeyOwner; 
	}
	
	void Init()
	{
		m_valueName		= NULL;
		m_value				= NULL;
		m_valueType		= 0;
		m_valueLen		= 0;
	}
	LPSTR	m_valueName;
	PVOID		m_value;
	DWORD		m_valueType;
	DWORD		m_valueLen;
	BOOL		GetValue(LPSTR pszValue);
private:
	HKEY		m_owner;
};

class CRegApi
{
public:
  CRegApi(void);
public:
  ~CRegApi(void);
  operator HKEY () { return m_hkey; }
	operator LPSTR() { return m_name; }
  BOOL				Open(HKEY, LPSTR);
  BOOL				Open(HKEY, LPSTR, BOOL);
  BOOL				Close();
  BOOL				ReadRegString(LPSTR pszValueName, LPSTR pszString, SIZE_T str_size);
  BOOL				ReadRegDWord(LPSTR pszValueName, PDWORD pdw);
  BOOL				WriteRegString(LPSTR pszValueName, LPSTR pszValue, DWORD value_size);
  BOOL				WriteRegDword(LPSTR pszValueName , DWORD dwValue);
  BOOL				DeleteRegValue(LPSTR pszValueName);
  BOOL				DeleteRegKey(void);
  BOOL				DeleteRegKey(LPSTR pszSubKey);
  HKEY				GetOpenKey() { return m_hkey; }
	void				SetAccess(DWORD access) { m_access = access; }
	LPSTR			GetName() { return m_name; }
	/* Accessor functions for children */
	const	CRegKeyInfo & 
							GetKeyInfo();
	const	CRegApi * 
							GetParent();
	//const	CRegApi * 
	//						GetFirstChild();
	//const	CRegApi * 
	//						GetNextChild();
	const	CRegValue *
							GetFirstValue();
	const	CRegValue *
							GetNextValue();
	
	BOOL				HasChildren();
private:
	HKEY				m_hkey;
  DWORD				m_access;
	CRegKeyInfo m_reg_info;
	TArray<CRegValue *, 10> 
							m_values;
	DWORD				m_currIndex;
	LPSTR			m_name;
};
