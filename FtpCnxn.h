#pragma once

class CFtpCnxn
{
public:
  CFtpCnxn(void);
  ~CFtpCnxn(void);
////////////////////////////////////////////
//
// Private variables that are used in the object
//
////////////////////////////////////////////
private:
  WORD    m_wsVersion;
  char  * m_dataBuffer;
  BOOL    m_fInited;
  SOCKET  m_control_socket;
  SOCKET  m_listen_socket;
  SOCKET  m_data_socket;
  BOOL    m_connected;
  BOOL    m_LoggedOn;
  int     m_bSendPort;
  LPTSTR  m_portCommand;
  LPTSTR  m_userName;
////////////////////////////////////////////
//
// Private methods
//
////////////////////////////////////////////
private:
  BOOL    Initialize(void);
  void    CloseControlConnection(void);
  void    CloseSocket(SOCKET sd);

////////////////////////////////////////////
//
// Public methods
//
////////////////////////////////////////////
public:
  void    CloseDataConnection( void );
  void    CloseListenSocket(void);
  BOOL    FtpMessage(LPTSTR pszBuffer, int * pbufflen, int ftp_message);
  int     GetServerResponse();
  int     GetDataBuffer(LPTSTR szBuffer, int buffLen);
  int     AcceptConnection();
  BOOL    DoListen(void);
  BOOL    IsConnected() { return m_connected; }
  BOOL    Connect(LPTSTR pszServer);
  BOOL    Disconnect(void);
  void    AbortConnection(void);
  int     GetServerData(LPTSTR ppszData);
  void SetLogOnStatus(bool fLoggedOn, LPTSTR LoggedOnAs);
  BOOL UserStatus(void);
  LPTSTR GetUserName(void);
};
