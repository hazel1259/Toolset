#pragma once


//#include "tcollection.h"
#ifndef UNICODE
#define TString TStringA
#else
#define TString TStringW
#endif
class TStringA
{
public:
    TStringA(void);
    TStringA(LPSTR);
    ~TStringA(void);
    operator  
    LPSTR() 
    { 
        return m_pstr; 
    }
    BOOL          operator==( LPSTR);
    BOOL          operator==( TStringA);
    TStringA      operator=(TStringA);
    TStringA      operator=(LPSTR);
    TStringA      operator+=(LPSTR);
    TStringA      operator+=(TStringA);
    TStringA      operator+=(CHAR);

protected:
    LPSTR m_pstr;

private:
    int           m_len;
    int           m_size;
public:
	void		  Add(CHAR ch);
	void		  Add(LPSTR pszText);
	CHAR		  Get(int pos);
	LPWSTR        Convert();
    void          Clear();
    TStringA *    Clone();
    BOOL          Clone(TStringA * ts);
    BOOL          Contains(LPSTR psz);
    int           GetLength(void);
    BOOL          Concat(__in LPSTR pszSrc);
    BOOL          Init(__in LPSTR);
    BOOL          Init(__in LPWSTR);
    BOOL          Init(__in LPSTR pszSrc, int len);
    LPSTR         Search(__in LPSTR psz);
    LPSTR         GetString() { return m_pstr; }
    CHAR          GetChar(int pos);
    BOOL          IsEmpty();
    BOOL          InitEmptyString(int size);
    BOOL          InitFrom(HINSTANCE hinst, int idsString);
    BOOL          IsEqual(__in LPSTR psz);
    BOOL          IsEqual(__in LPSTR psz, BOOL bIgnoreCase);
	BOOL		  IsLowerCase();
	BOOL		  IsUpperCase();
    TStringA*     Split(char ch);
    TStringA*     Substring(int pos);
    TStringA*     Substring(int pos, int len);
    BOOL          Compare(__in LPSTR psz);
    BOOL          Compare(__in LPSTR psz, BOOL ignoreCase);
    BOOL          Compare(__in LPSTR psz, BOOL ignoreCase, DWORD dwLen);
    BOOL          TrimLeft();
    BOOL          TrimRight();
    void          ToUpper();
    void          ToLower();
    LPSTR         Find(__in LPSTR psz);
    LPSTR         Find(__in LPSTR, BOOL ignoreCase);
    LPSTR         Find(CHAR ch);
    BOOL          Replace(__in LPSTR psz, __in LPSTR pszReplace);
    BOOL          Replace(__in LPSTR psz, __in LPSTR pszReplace, BOOL ignoreCase);
    BOOL          ReplaceChar(CHAR tch, CHAR tchNew);
    BOOL          ReplaceChar(CHAR tch, CHAR tchNew, BOOL IgnoreCase);
    BOOL          RemoveChar(CHAR tch);
    BOOL          RemoveChar(CHAR tch, BOOL IgnoreCase);
    BOOL          Remove(int pos);
    BOOL          Remove(int pos, int len);
    BOOL          Remove();
    BOOL          Insert(CHAR ch, int pos);
    BOOL          Insert(LPSTR psz, int pos);
    BOOL          Remove(__in LPSTR pszRemove);
    BOOL          Remove(__in LPSTR pszRemove, BOOL ignoreCase);
    BOOL          Reverse();
private:  
    COMPARESTRING GetCompareFunction(BOOL ignoreCase);
};

class TStringW
{
public:
    TStringW(void);
    TStringW(LPWSTR);
    ~TStringW(void);
    // cast operator
    operator      LPWSTR()
    { 
        return m_pstr; 
    }

    BOOL          operator==( LPWSTR);
    BOOL          operator==( TStringW);
    TStringW      operator=(__in LPWSTR);
    TStringW      operator=(TStringW);
    TStringW      operator=(LPSTR);
    TStringW      operator+=(__in LPWSTR);
    TStringW      operator+=(TStringW);
    TStringW      operator+=(WCHAR);

    protected:
    LPWSTR        m_pstr;

    private:
    int           m_len;
    int           m_size;
    public:
    void          Clear();
    TStringW *    Clone();
	void          Add(WCHAR wch);
	void          Add(LPWSTR pszText);
	LPSTR	      Convert();
	WCHAR         Get(int pos);
    BOOL          Clone(TStringW * ts);
    BOOL          Contains(LPWSTR psz);
    int           GetLength(void);
    BOOL          Concat(__in LPWSTR pszSrc);
    BOOL          IsEqual(__in LPWSTR psz);
    BOOL          IsEqual(__in LPWSTR psz, BOOL bIgnoreCase);
    BOOL          Init(__in LPWSTR);
    BOOL          Init(LPSTR);
    BOOL          Init(__in LPWSTR pszSrc, int len);
	BOOL		  IsLowerCase();
	BOOL		  IsUpperCase();
    BOOL          IsEmpty();
    LPWSTR        Search(__in LPWSTR psz);
    LPWSTR        GetString() { return m_pstr; }
    WCHAR         GetChar(int pos);
    BOOL          InitEmptyString(int size);
    BOOL          InitFrom(HINSTANCE hinst, int idsString);
    TStringW*     Split(WCHAR ch);
    TStringW*     Substring(int pos);
    TStringW*     Substring(int pos, int len);
    BOOL          Compare(__in LPWSTR psz);
    BOOL          Compare(__in LPWSTR psz, BOOL ignoreCase);
    BOOL          Compare(__in LPWSTR psz, BOOL ignoreCase, DWORD dwLen);
    BOOL          TrimLeft();
    BOOL          TrimRight();
    void          ToUpper();
    void          ToLower();
    LPWSTR        Find(__in LPWSTR psz);
    LPWSTR        Find(__in LPWSTR, BOOL ignoreCase);
    LPWSTR        Find(WCHAR tch);
    BOOL          Replace(__in LPWSTR psz, LPWSTR pszReplace);
    BOOL          Replace(__in LPWSTR psz, LPWSTR pszReplace, BOOL ignoreCase);
    BOOL          ReplaceChar(WCHAR tch, WCHAR tchNew);
    BOOL          ReplaceChar(WCHAR tch, WCHAR tchNew, BOOL IgnoreCase);
    BOOL          RemoveChar(WCHAR tch);
    BOOL          RemoveChar(WCHAR tch, BOOL IgnoreCase);
    BOOL          Remove(int pos);
    BOOL          Remove(int pos, int len);
    BOOL          Remove();
    BOOL          Insert(WCHAR ch, int pos);
    BOOL          Insert(LPWSTR psz, int pos);
    BOOL          Remove(__in LPWSTR pszRemove);
    BOOL          Remove(__in LPWSTR pszRemove, BOOL ignoreCase);
    BOOL          Reverse();
private:  
  COMPARESTRINGW    
                GetCompareFunction(BOOL ignoreCase);
};



