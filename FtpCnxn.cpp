/***********************************************************************************************************************

  MODULE: FtpCnxn.cpp

  Weird name because Microsoft stole this namespace for themselves. We cannot help this, so we tried a anacronym
  for connections as cnxn.

  Author: Hazel Lloyd
  Created: 4-01-05
  History:
  4-01-05 - hazel lloyd started it and added the base class and constructors.
*************************************************************************************************************************/
#include "comm_inc.h"
#include ".\ftpcnxn.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Initializer for the version of winsock, although we don't care, we do keep track of which version of WINSOCK we 
// were able to use. If something does not work from version to version, we go here to fix it.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if 0
WORD  
wsVersions[] = 
  {
    MAKEWORD(2,2),
    MAKEWORD(2,1),
    MAKEWORD(2,0),
    MAKEWORD(1,1),
    MAKEWORD(1,0)
  };
#endif
#define  UC(b)  (((int)b)&0xff)

/***********************************************************************************************************************

  CFtpCnxn()

  Type: Constructor()
  Base: this
  Scope: Public
  Parameters: 
  none

  this constructor initializes winsock and all of our internal handles.

*************************************************************************************************************************/

CFtpCnxn::CFtpCnxn(void) 
:   m_wsVersion(0)
,   m_fInited(FALSE)
,   m_control_socket(INVALID_SOCKET)
,   m_listen_socket(INVALID_SOCKET)
,   m_data_socket(INVALID_SOCKET)
,   m_connected(FALSE)
,   m_bSendPort(1)
,   m_portCommand(NULL)
,   m_LoggedOn(FALSE)
{
  m_fInited = Initialize();
}

/***********************************************************************************************************************

  ~CFtpCnxn()

  Type: Constructor()
  Base: this
  Scope: Public
  Parameters: 
  none

  this constructor initializes winsock and all of our internal handles.

*************************************************************************************************************************/
CFtpCnxn::~CFtpCnxn(void)
{
  if (m_fInited)
  {
    if (m_dataBuffer)
    {
      MyFree(m_dataBuffer);
    }
    
    if (this->m_portCommand)
    {
      MyFree(m_portCommand);
    }
    Disconnect();
    WSACleanup();
  }
}

/***********************************************************************************************************************

  Initialize()

  Type:       Method()
  Base:       this
  Scope:      Public
  
  Parameters: 
  none

  this method initializes winsock for us!

*************************************************************************************************************************/
BOOL CFtpCnxn::Initialize(void)
{
  WSADATA     wsaData;
  m_wsVersion = (WORD)-1;
  static const WORD  
  wsVersions[] = 
  {
    MAKEWORD(2,2),
    MAKEWORD(2,1),
    MAKEWORD(2,0),
    MAKEWORD(1,1),
    MAKEWORD(1,0)
  };
  m_portCommand = MyAlloc(LPTSTR, 0xff);
  if (!m_portCommand)
  {
    goto DONE;
  }
  m_dataBuffer = MyAlloc(char *, MAX_SIZE);
  if (!m_dataBuffer)
  {
    goto DONE;
  }
  //
  // walk the array of known versions and use the latest one we can get!!
  //
  for (int i = 0; i < sizeof(wsVersions)/sizeof(wsVersions[0]); i++)
  {
    if (WSAStartup( wsVersions[i], &wsaData )== 0 ) 
    {
      m_wsVersion  = wsVersions[i];
      break;
    }
  }
DONE:
  return (m_wsVersion != -1);
}
/***********************************************************************************************************************

  Disconnect()

  Type:       Method()
  Base:       this
  Scope:      Public
  
  Parameters: 
  none

  this method disconnects from the server. If it is not connected, it returns immediately 

*************************************************************************************************************************/
BOOL CFtpCnxn::Disconnect(void)
{
  if (m_connected)
  {
    int size = 6;
    m_LoggedOn = FALSE;
    m_userName = NULL;
    if (FtpMessage("quit\r\n", &size, FtpSendControlMessage))
    {
  	  GetServerResponse();
	    CloseControlConnection();
    }
    if (m_control_socket  && m_control_socket != m_data_socket)
    {
      closesocket(m_control_socket);
    }

    if (m_listen_socket  && m_listen_socket  != m_data_socket)
    {
      closesocket(m_listen_socket );
    }
    if (m_data_socket)
    {
      closesocket(m_data_socket);
    }
    m_connected = FALSE;
    return TRUE;
  }
  return FALSE;
}

/***********************************************************************************************************************

  Disconnect()

  Type:       Method()
  Base:       this
  Scope:      Public
  
  Parameters: 

  LPTSTR  pszFtpData -  pointer to a null terminated string containing the message
  int *   pdatalen   -  pointer to an integer containin the length of pszFtpData. 
                        This variable is updated for some commands
  int     ftp_message - The message id to send or get.

  This method will send the specified message to the appropriate socket and return TRUE or FALSE if the socket 
  message was successful. There are 2 types, send and recv messages. Each has a different meaning, for the data
  message, we set pdatalen to the number of bytes that were pulled off the network connection.


*************************************************************************************************************************/
BOOL CFtpCnxn::FtpMessage(LPTSTR pszFtpData, int * pdatalen, int ftp_message)
{
  BOOL br = FALSE;
  int datalen = -1;
  if (!pdatalen)
  {
    goto DONE;
  }
  datalen = *pdatalen;
  switch(ftp_message)
  {
    case FtpReadControlMessage:
    {
      if( (recv(m_control_socket, pszFtpData, datalen, 0 )) <= 0)
      {
        br = FALSE;
      }
      else
      {
        br = TRUE;
      }
    }
    break;
    
    case FtpSendControlMessage:
    {
      if(send(m_control_socket, pszFtpData, datalen, 0) <= 0)
      {
        br = FALSE;
      }
      else
      {
        return br = TRUE;
      }
    }
    break;
    
    case FtpSendDataMessage:
    {
      if( send(m_data_socket,pszFtpData, datalen, 0) <= 0)
      {
        br = FALSE;
      }
      else
      {
        return br = TRUE;
      }
    }
    break;
    
    case FtpReadDataMessage:
    {
      int ret = 0;
      if( (ret = recv(m_data_socket,pszFtpData, datalen, 0)) <= 0)
      {
        br = FALSE;
      }
      else
      { 
        *pdatalen = ret;
        br = TRUE;
      }
    }
    break;
    
    case FtpReplyMessage:
    {
      if( (recv(m_control_socket, pszFtpData, datalen, 0 )) <= 0)
      {
        br = FALSE;
      }
      else
      {
        br = TRUE;
      }
    }
    break;

    case FtpCheckControlMessage:
    {
      if (recv(m_control_socket, pszFtpData, datalen, MSG_PEEK) <= 0)
      {
        br = FALSE;
      }
      else
      {
        br = TRUE;
      }
    }
    break;
  }
DONE:
  return br;
}


/***********************************************************************************************************************

  GetDataBuffer()

  Type:       Method()
  Base:       this
  Scope:      Public
  
  Parameters: 

  char * pszBuffer
  int BuffSize

*************************************************************************************************************************/
int CFtpCnxn::GetDataBuffer(char * pszBuffer, int BuffSize)
{
  int     i         = 1,
          len       = 1, 
          iBuffLen  = 0;
  char *  szPtr     = pszBuffer, 
          szRetCode[4]  = {0},
          ch        =0;
  
  while( (iBuffLen < BuffSize) && FtpMessage(&ch, &len, FtpCheckControlMessage))
  {
    len = 1;
    if (FtpMessage(&ch, &len, FtpReadControlMessage))
    {
      iBuffLen += len;
      *szPtr = ch;
      szPtr += len;
      if( ch == '\n' )
      {
	      break;    // we have a line: return
      }
    }
  }
 *(szPtr+1) = (char)'\0';
	strncpy_s( szRetCode, 3, pszBuffer, 3);
 //strncpy(szRetCode, pszBuffer, 3);
  szRetCode[3] = '\0';
  return (atoi(szRetCode));
}
/***********************************************************************************************************************

  GetServerResponse()

  Type:       Method()
  Base:       this
  Scope:      Public
  
  Parameters: 

  None

  returns: integer value of the return code, the caller must check for values >= 400 as these indicate a server
  side error that was not recoverable

*************************************************************************************************************************/

int CFtpCnxn::GetServerResponse()
{
  int    FtpReturnCode= 0;
  
  // if we were not successful, return
  //
  if (!m_dataBuffer)
  {
    FtpReturnCode = 400;
    goto DONE;
  }
  while(TRUE ) 
  {
    ZeroMemory(m_dataBuffer, MAX_SIZE);
    FtpReturnCode = GetDataBuffer(m_dataBuffer, MAX_SIZE);
    if (!*m_dataBuffer)
    {
      break;
    }
    printf(m_dataBuffer);
    if( m_dataBuffer[3] != '-' && FtpReturnCode > 0 )
    {
      break;
    }
  }
DONE:
  return FtpReturnCode;
}

/***********************************************************************************************************************

  AcceptConnection()

  Type:       Method()
  Base:       this
  Scope:      Public
  
  Parameters: 

  None

  returns: integer value of the connection socket

  This method accepts the socket connection and returns it to the caller

*************************************************************************************************************************/
int CFtpCnxn::AcceptConnection()

{
  struct sockaddr_in  cli_addr;
  int                 clilen = sizeof(cli_addr);
  int                 sockfd;

  sockfd = (int)accept(m_listen_socket, (struct sockaddr *) &cli_addr, &clilen);
  if (sockfd < 0) 
  {
    perror("accept");
	  return INVALID_SOCKET;
  }
  m_data_socket = sockfd;
  closesocket(m_listen_socket);
  return sockfd;

}

/***********************************************************************************************************************

  AcceptConnection()

  Type:       Method()
  Base:       this
  Scope:      Public
  
  Parameters: 

  char * pszServer - name of the server to connect to, ie, ftp.servername.com

  returns: TRUE if we connected and recieved a response, false otherwise

  This method tries to connect to the server and if that fails, responds to the user

*************************************************************************************************************************/
BOOL CFtpCnxn::Connect(char * pszServer)
{
  int                         s = INVALID_SOCKET, 
                              rc;
  struct        sockaddr_in   server;
  if (m_connected)
  {
    printf("We are already connected, please disconnect first.\n");
    goto DONE;
  }
  // clean the memory
  //
  ZeroMemory(&server, sizeof(server));
  if (!GetIpAddress(pszServer, &server))
  {
    goto DONE;
  }
  // re-initialize just in case
  m_connected = FALSE;
  // get a socket
  if( (s = (int)NewStreamSocket()) < 1) 
  {
    DoSocketError(WSAGetLastError());
	  printf("socket: error code: %d\r\n", WSAGetLastError());
    goto DONE;
  }
  rc = connect(s,  (struct sockaddr *)&server, sizeof(server)); 
  if ( rc  == SOCKET_ERROR) 
  {
    DoSocketError(WSAGetLastError());
    closesocket(s);
    s = INVALID_SOCKET;
    goto DONE;
  }
  // set the socket options to what we need
  setsockopt(s,SOL_SOCKET,SO_LINGER,0,0);
  setsockopt(s,SOL_SOCKET,SO_REUSEADDR,0,0);
  setsockopt(s,SOL_SOCKET,SO_KEEPALIVE,0,0);
  m_data_socket     = s;
  m_control_socket  = s;
  m_connected       = TRUE;
DONE:
  return m_connected;
}
/***********************************************************************************************************************

  DoListen()

  Type:       Method()
  Base:       this
  Scope:      Public
  
  Parameters: 

  char * pszServer - name of the server to connect to, ie, ftp.servername.com

  returns: TRUE if we connected and recieved a response, false otherwise

  This method tries to connect to the server and if that fails, responds to the user

*************************************************************************************************************************/

BOOL CFtpCnxn::DoListen(void)
{
  BOOL br = FALSE;
  SOCKET  sockfd; 
  int     flag  = 1,
          len;

  struct sockaddr_in  serv_addr, 
                      TempAddr;
  char  * port,
        * ipaddr;

  // release the previous socket
  //
  if (m_listen_socket != -1)
  {
    CloseSocket(m_listen_socket);
  }
  // get a new listen socket
  if ((sockfd = NewStreamSocket()) < 0) 
  {
    DoSocketError(WSAGetLastError());
    goto DONE;
  }

  if( m_bSendPort ) 
  {
    ZeroMemory(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family      = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port        = htons(0); /* let system choose */
  }

  else 
  {
    if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR, (char *)&flag,sizeof(flag)) < 0) 
    {
      DoSocketError(WSAGetLastError());
      goto DONE;
    }
  }
  // bind the socket to the address
  //
  if (bind(sockfd,(struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
  {
    DoSocketError(WSAGetLastError());
    goto DONE;
  }
  // set up
  len = sizeof(serv_addr);

  // get the socket name from the caller
  //
  if(getsockname(sockfd,(struct sockaddr *)&serv_addr, &len) < 0) 
  {
    DoSocketError(WSAGetLastError());
    goto DONE;
  }
  // get the name from the control socket
  //
  len = sizeof(TempAddr);
  if(getsockname(m_control_socket, (struct sockaddr *)&TempAddr, &len)  < 0 ) 
  {
    DoSocketError(WSAGetLastError());
    goto DONE;
  }

  ipaddr = (char *)&TempAddr.sin_addr;
  port  = (char *)&serv_addr.sin_port;
  if (!m_portCommand)
  {
    if (!(m_portCommand = MyAlloc(LPTSTR, 32)))
    {
      DoErrorMessage(GetLastError());
      goto DONE;
    }
  }
  // now copy the send command to the buffer to be sent
  //
  StringCchPrintf(m_portCommand,32, 
    "PORT %d,%d,%d,%d,%d,%d\r\n",
    UC(ipaddr[0]), 
    UC(ipaddr[1]), 
    UC(ipaddr[2]), 
    UC(ipaddr[3]), 
    UC(port[0]), 
    UC(port[1])
    );
  // let the server connect and talk to us
  //
  if( listen(sockfd, 1) < 0) 
  {
    DoSocketError(WSAGetLastError());
    goto DONE;
  }
  // now send the PORT command with our address and port to connect to
  //
  int size = (int)strlen(m_portCommand);
  if (FtpMessage(m_portCommand, &size, FtpSendControlMessage))
  {
    GetServerResponse();
    m_listen_socket = sockfd;
    // all is well in the library, return true
    br = TRUE;
  }
DONE:
  if (!br)
  {
    closesocket(sockfd);
  }
  return br;
}

/***********************************************************************************************************************

  CloseDataConnection()

  Type:       Method()
  Base:       this
  Scope:      Public
  
  Parameters: 


  This method closes the data connection to the server

*************************************************************************************************************************/

void CFtpCnxn::CloseDataConnection( void )
{
  CloseSocket(m_data_socket);
}



/***********************************************************************************************************************

  CloseControlConnection()

  Type:       Method()
  Base:       this
  Scope:      Public
  
  Parameters: 


  This method closes the control connection to the server

*************************************************************************************************************************/
void CFtpCnxn::CloseControlConnection( void )
{
  m_connected = FALSE;
  CloseSocket(m_control_socket);
}

/***********************************************************************************************************************

  CloseListenSocket()

  Type:       Method()
  Base:       this
  Scope:      Public
  
  Parameters: 


  This method closes the listen connection to the server

*************************************************************************************************************************/
void CFtpCnxn::CloseListenSocket( void )
{
  CloseSocket(m_listen_socket);
}

/***********************************************************************************************************************

  AbortConnection()

  Type:       Method()
  Base:       this
  Scope:      Public
  
  Parameters: 


  This method aborts a data response by sending the server a ABOR command

*************************************************************************************************************************/
void CFtpCnxn::AbortConnection(void)
{
  char *abortstr = "ABOR\r\n";
  int len = (int)strlen(abortstr);
  printf("\r\nAbort: Waiting for server to finish.");
  FtpMessage(abortstr, &len, FtpSendControlMessage);
}

/***********************************************************************************************************************

 AbortConnection()

  Type:       Method()
  Base:       this
  Scope:      Public
  
  Parameters: 

  LPTSTR ppszData - data to fill with response 

  This method is not yet completed

*************************************************************************************************************************/
/*
int CFtpCnxn::AbortConnection(LPTSTR ppszData)
{
  int               done = 0, 
                    iRetCode = 0;
  char              szBuffer[1024];
  ZeroMemory(szBuffer, sizeof(szBuffer)/sizeof(szBuffer[0]));
  while(TRUE ) 
  {
    iRetCode = GetDataBuffer(szBuffer, sizeof(szBuffer)/sizeof(szBuffer[0]));
    printf(szBuffer);
    if( szBuffer[3] != '-' && iRetCode > 0 )
    {
      break;
    }
    ZeroMemory(szBuffer, sizeof(szBuffer)/sizeof(szBuffer[0]));
  }
  return iRetCode;
}
*/
/***********************************************************************************************************************

 CloseSocket()

  Type:       Method()
  Base:       this
  Scope:      Public
  
  Parameters: 

  SOCKET sd - socket to close

  This method closes the socket and sets its valie to INVALID_SOCKET

*************************************************************************************************************************/
void CFtpCnxn::CloseSocket(SOCKET sd)
{
  closesocket(sd);
  sd = INVALID_SOCKET;
}

/***********************************************************************************************************************

 SetLogOnStatus()

  Type:       Method()
  Base:       this
  Scope:      Public
  
  Parameters: 

  BOOL    fLoggedOn   - true if the log on was successful
  LPTSTR  LoggedOnAs  - name to use on success
  
  This method sets the name of the user and the log on status flag for future reference by users.

*************************************************************************************************************************/
void CFtpCnxn::SetLogOnStatus(bool fLoggedOn, LPTSTR LoggedOnAs)
{
  m_LoggedOn = fLoggedOn;
  if (fLoggedOn)
  {
    int len = slen(LoggedOnAs) + 1;
    m_userName = MyAlloc(LPTSTR, len);
    if (!m_userName)
    {
      printf("Non fatal Internal failure allocating memory for user name.\n");
      DoErrorMessage(GetLastError());
    }
    else
    {
      if (FAILED(StringCchCopy(m_userName, len, LoggedOnAs)))
      {
        printf("Non fatal Internal failure setting user name.\n");
      }
    }
  }
}

/***********************************************************************************************************************

  UserStatus()

  Type:       Method()
  Base:       this
  Scope:      Public
  
  Parameters: 

  
  returns the current log on status of this connection

*************************************************************************************************************************/
BOOL CFtpCnxn::UserStatus(void)
{
  return m_LoggedOn;
}

/***********************************************************************************************************************

  UserStatus()

  Type:       Method()
  Base:       this
  Scope:      Public
  
  Parameters: 

  
  returns the current user name of this connection

*************************************************************************************************************************/
LPTSTR CFtpCnxn::GetUserName(void)
{
  if (m_userName)
  {
    return m_userName;
  }
  return "Not logged on";
}
