
BOOL 
__stdcall 
Free(
	PVOID * ppv
	);

BOOL 
__stdcall 
Alloc(
	PVOID * ppv,
	SIZE_T size
	);

void 
DumpArray(
  PUCHAR p, 
  int size, 
  BOOL console
  );
// miscellaneous public functions
void 
DoSocketError(
  int Error
  );

int Round(float f);

BOOL 
NormalizePathA(
    LPSTR pszPath, 
    LPSTR pszNormal, 
    UINT uLenNormal
    );

BOOL 
NormalizePathW(
    LPSTR pszPath, 
    LPSTR pszNormal, 
    UINT uLenNormal
    );

BOOL 
GetCenterWindow(
    HWND hwnd, 
    POINT * pLocation, 
    SIZE * pSize
    );

#ifdef UNICODE
#define NormalizePath NormalizePathW
#else
#define NormalizePath NormalizePathA
#endif

HFONT 
GetFontW(
  LPWSTR pszFont, 
  int height, 
  LPWSTR style
  );

HFONT 
GetFontA(
  LPSTR pszFont, 
  int height, 
  LPSTR style
  );

void 
DumpData(
 unsigned char * pdata, 
 int			size
 );

LPWSTR
StripPathW(
LPWSTR pFile
    );

PSTR 
StripPath(
  LPSTR pFile
  );

__int64 
Factorial(
		  __int64 n
		  );
__int64 
Combination(
			__int64 n, __int64 r
			);

BOOL 
FindInBuffer(
  /* This is a comment */ LPTSTR pszString, 
  PUCHAR pBuffer, 
  int dwSize
  );

char * 
FindChar(
		 char * psz, 
		 char ch, 
		 int len
		 );
int
TraceOut(
  int     LoggingMethod,
  LPTSTR  pszFileName,
  char *  pszData, 
  ...
  );

BOOL 
IsIntegerValue(
  char * val
  );

LPSTR 
GetErrorString(
  UINT dwError
  );
BOOL 
IsInteger(
  char * pszNumber
  );
UINT 
ConDoErrorMessage(
	UINT dwError
	);
UINT 
MBDoErrorMessage(
	UINT dwError
	);
BOOL 
ConvertToUnicode(
  LPCSTR psz, 
  PWSTR pwz
  );
BOOL 
ConvertToAnsi(
  LPWSTR pwz, 
  LPSTR psz
  );

#ifdef USE_WINDOW
#define DoErrorMessage MBDoErrorMessage
#else
#define DoErrorMessage ConDoErrorMessage
#endif

//UINT 
//DoErrorMessage(
//  UINT dwError
//	);

char * 
GetStrChar(
  char * pszString, 
  char ch
  );

char * 
StripChar(
  char *psz, 
  char ch
  );

int 
Divide(
  int iVal1, 
  int iVal2
  );

PUCHAR 
GetLine(
  LPTSTR pszLine, 
  PUCHAR pBuffer, 
  int dwSize
  );

LPTSTR
NewString(
  LPTSTR psz, 
  int len
  );

int 
_hexatoi(
  const char * input
  );

BOOL
//int 
GetConstantValue(
  const char * constant,
  int * pval
  );

BOOL 
IsDigit(
  const char * number
  );

int
DebugOut(
  char *  pszData, 
  ...
  );
void 
TraceErrorMessage(
	DWORD lastError
);

HDC 
CreateMemDC(
	HDC hdc, 
	SIZE * lpsz
	);

BOOL 
GetScreenSize(
	LPSIZE lpsz
	);
#ifdef UNICODE
#define GetFont GetFontA
#else
#define GetFont GetFontW
#endif

BOOL
ConvertUTF8ToUnicode(
PUCHAR pUtf8Stream,
UINT utf8Len,
LPWSTR pUnicode,
UINT uniLen
);

BOOL 
ConvertUnicodeToUTF8(
    LPWSTR pszUnicode, 
    UINT uniLen, 
    PUCHAR pUtf8Stream, 
    UINT streamLen
    );