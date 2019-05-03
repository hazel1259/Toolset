#pragma once
#ifdef UNICODE
#define CStream CStreamW
#else
#define CStream CStreamA
#endif

class CStreamA
{
public:
  CStreamA(void);
  ~CStreamA(void);
  virtual   BOOL  Open(LPSTR pszStream)                                         = 0;
  virtual   BOOL  Close(void)                                                   = 0;
  virtual   BOOL  Write(PUCHAR pBuffer, DWORD dwSize, DWORD Offset)             = 0;
  virtual   BOOL  Write(PUCHAR pBuffer, DWORD dwSize)                           = 0;
  virtual   BOOL  ReadChar(CHAR * pch)                                          = 0;
  virtual   BOOL  Read(PUCHAR pBuffer, DWORD len)                            = 0;
  virtual   BOOL  Read(PUCHAR pBuffer, DWORD len, DWORD position)  = 0;
  virtual   DWORD SetPosition(DWORD new_position)                               = 0;
  virtual   LPSTR Name() { return m_stream_name; }
protected:
  void Init()
  {
    m_currPos = 0;
    m_pBuffer = NULL;
    m_fError = FALSE;
    m_stream_name = NULL;
  }
  DWORD     m_currPos;
  PUCHAR    m_pBuffer;
  BOOL      m_fError;
  LPSTR    m_stream_name;
private:
  
};


class CStreamW
{
public:
  CStreamW(void);
  ~CStreamW(void);
  virtual   BOOL  Open(LPWSTR pszStream)                                         = 0;
  virtual   BOOL  Close(void)                                                   = 0;
  virtual   BOOL  Write(PUCHAR pBuffer, DWORD dwSize, DWORD Offset)             = 0;
  virtual   BOOL  Write(PUCHAR pBuffer, DWORD dwSize)                           = 0;
  virtual   BOOL  ReadChar(CHAR * pch)                                          = 0;
  virtual   BOOL  Read(PUCHAR pBuffer, DWORD len)                            = 0;
  virtual   BOOL  Read(PUCHAR pBuffer, DWORD len, DWORD position)  = 0;
  virtual   DWORD SetPosition(DWORD new_position)                               = 0;
  virtual   LPWSTR Name() { return m_stream_name; }
protected:
  void Init()
  {
    m_currPos = 0;
    m_pBuffer = NULL;
    m_fError = FALSE;
    m_stream_name = NULL;
  }
  DWORD     m_currPos;
  PUCHAR    m_pBuffer;
  BOOL      m_fError;
  LPWSTR    m_stream_name;
private:
  
};
