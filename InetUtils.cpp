/********************************************************************************************************************

  MODULE: INETUTILS.CPP

  Helper Module to use when setting up socket and connect, recieve ports.

  Author : Hazel Lloyd
  Created: 4-08-98
  History: 
  4-08-98 Hazel created this module and added the following functions

***********************************************************************************************************************/

#include "comm_inc.h"
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
/************************************************************************************************************************

	GetIpAddress()

	Type:					Member Function
	Scope:				Public
	Base Class:		CMySocket
	Parameters:		

	LPTSTR hostname - pointer to the  name of the machine to obtain the IP address for

	returns: the return code from WSAGetLastError()

	Note:

 We obtain the actual IP address for the machine here.

************************************************************************************************************************/

BOOL 
GetIpAddress(
    char *hostname, 
    struct sockaddr_in  * pDest
  )
{
  HOSTENT *             lpHost  = NULL;
  struct sockaddr_in    Destination;
  int err = 0;
  lpHost = gethostbyname(hostname);
  if (lpHost == NULL)
  {
    printf("Failed to get IP address for %s with error %d\n", hostname,  WSAGetLastError());
  }
  else
  {
    for(int i = 0; lpHost->h_addr_list[i] != NULL ;i++)
    {
      memcpy(&(Destination.sin_addr), lpHost->h_addr_list[i], lpHost->h_length);
    }
    memcpy(pDest, &Destination, sizeof(Destination));
    pDest->sin_family = PF_INET;
    pDest->sin_port = htons(IPPORT_FTP);
 
  }
  err = WSAGetLastError();
  return (err == 0);
} 

SOCKET NewStreamSocket()
{
  return socket(AF_INET, SOCK_STREAM, 0);
}
/************************************************************************************************************************

	SendCommand()

	Type:					Member Function
	Scope:				Public
	Base Class:		
	Parameters:		

  SOCKET sd - source socket to send the command to (like a telnet ASCII command
  LPTSTR pszCommand - pointer to a NULL terminated string containing the command
  LPTSTR pszData - any extra data to send with the command

	returns: TRUE if the command was successful FALSE otherwise

	Note:


************************************************************************************************************************/
#if 0
BOOL 
SendCommand(
  SOCKET sd, 
  LPTSTR pszCommand, 
  LPTSTR pszData
  )
{
  BOOL br = FALSE;
  CHAR Command[0xff];
  int datalen = 0;
  if (pszCommand && pszData)
  {
    PUCHAR pData = (PUCHAR)Command;
    int len = (int)strlen(Command);
    
    MAKECMD(Command, pszCommand, pszData);
    
    while(len > 0)
    {
      datalen = send(sd, (const char *)pData, len , 0);
      if (!SOCKSUCCESS(datalen))
      {
        printf("send failed\n");
        DoSocketError(datalen);
      }
      else
      {
        br = TRUE;
        printf("send %d bytes successful\n", datalen);
      }

      pData += datalen;
      len -= datalen;
      
      if (len > 0)
      {
        printf("sending new block %d bytes\n", len);
      }
    }
  }
  return br;
}
#endif
/************************************************************************************************************************

	GetServerResponse()

	Type:					Member Function
	Scope:				Public
	Base Class:		
	Parameters:		

  SOCKET sd - socket to retrieve the reply from, not the data socket and the host need not send a reply

	returns: TRUE if the command was successful FALSE otherwise

	Note:


************************************************************************************************************************/
BOOL 
GetServerResponse(
  SOCKET sd
  )
{
  CHAR RecvBuff[0xffff];
  BOOL br = FALSE;
  int len = 1;
  
  memset(RecvBuff, 0, sizeof(RecvBuff));
  len = recv(sd, RecvBuff, sizeof(RecvBuff), MSG_PEEK);
  if (len == (int)sizeof(RecvBuff))
  {
    printf("Looks good here\n");
  }
  if (!SOCKSUCCESS(len))
  {
    printf("recv failed\n");
    DoSocketError(len);
  }
  else
  {
    br = TRUE;
    for (int i = 0; i < len; i++)
    {
      if (i >= 0 && i < 3)
      {
        printf("%c", RecvBuff[i]);
        if (i == 2)
          printf(":\n");
      }
      else
      {
        printf("%c", RecvBuff[i]);
      }
    }
  }
  printf("\n");
  return br;
}

/************************************************************************************************************************

	BindToPort()

	Type:					Member Function
	Scope:				Public
	Base Class:		
	Parameters:		

  SOCKET sd - socket to retrieve the reply from, not the data socket and the host need not send a reply
  int port  - socket to bind the port to

	returns: TRUE if the command was successful FALSE otherwise

	Note:


************************************************************************************************************************/
BOOL 
BindToPort(
  SOCKET sd, 
  int port
  )
{
  BOOL br = FALSE;
  sockaddr_in Addr;
  int rc, optval = 0;
  Addr.sin_family       = PF_INET;
  Addr.sin_addr.s_addr  = htonl(INADDR_ANY);
  Addr.sin_port         = port;
  optval = 1;
	rc = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(optval));
  
  if (!SOCKSUCCESS(rc))
  {
    DoSocketError(__ERROR);
    goto DONE;
  }
  rc = bind(sd, (const sockaddr *)&Addr, sizeof(Addr));
  if (!SOCKSUCCESS(rc))
  {
    DoSocketError(__ERROR);
    goto DONE;
  }
  br = TRUE;
DONE:
  return br;
}

/************************************************************************************************************************

	SendCommandAndWait()

	Type:					Member Function
	Scope:				Public
	Base Class:		
	Parameters:		

  SOCKET sd - socket to retrieve the reply from, not the data socket and the host need not send a reply
  LPTSTR pszCommand = command to send to the port
  LPTSTR pszData - data to send with the command

	returns: TRUE if the command was successful FALSE otherwise

	Note:


  This function wraps SendCommand and GetServerResponse into a single function, you should use this only if you know
  you'll get a reply, otherwise you'll hang the port waiting on a response that never comes

************************************************************************************************************************/
#if 0
BOOL
SendCommandAndWait(
    SOCKET sd, 
    LPTSTR pszCommand, 
    LPTSTR pszData
  )
{
  BOOL br = FALSE;

  if (!SendCommand(sd, pszCommand, pszData))
  {
    printf("No message sent\n");
    goto DONE;
  }
  if (!GetServerResponse(sd))
  {
    printf("No reply recieved\n");
    goto DONE;
  }
  br = TRUE;
DONE:
 
  return br;
}
#endif
//rc = gethostname(thisname, sizeof(thisname));
  /*
    //struct      sockaddr_in Dest;

  if (SOCKSUCCESS(rc))
  {
    if (0 != GetIpAddress(thisname, &Dest))
    {
      printf("Failed to retrieve IP address for %s\n", thisname);
      //goto DONE;
    }
    else
    {
      CHAR IpAddress[(int)sizeof(Dest) + 5];
      ZeroMemory(IpAddress, (DWORD)sizeof(Dest));
      sprintf(IpAddress, "%d,%d,%d,%d", 
        Dest.sin_addr.S_un.S_un_b.s_b1, 
        Dest.sin_addr.S_un.S_un_b.s_b2,
        Dest.sin_addr.S_un.S_un_b.s_b3,
        Dest.sin_addr.S_un.S_un_b.s_b4
        );
      printf("%s\n", IpAddress);
      //printf("IP address for %s is: %s\n", thisname, inet_ntoa(Dest.sin_addr));
      
    }
    printf("Client Name:     %s\n", thisname);
    printf("Maximum sockets: %d\n", wsaData.iMaxSockets);
    printf("Maximum UDP dg:  %d\n", wsaData.iMaxUdpDg);
    //printf("Vendor:          %s\n", wsaData.lpVendorInfo);
    printf("Description:     %s\n", wsaData.szDescription);
    printf("Status:          %s\n", wsaData.szSystemStatus);
    printf("Version:         %d\n", wsaData.wVersion);
    printf("HiVersion:       %d\n", wsaData.wHighVersion);
  }
  else
  {
    printf("Don't know my name\n");
    DoSocketError((int)WSAGetLastError());
  }
  */

/************************************************************************************************************************

	GetAddressString()

	Type:					Member Function
	Scope:				Public
	Base Class:		
	Parameters:		

  LPTSTR pszName  Computer name you want the address for 
  LPTSTR pszIpAddress - ipaddress to fill

	returns: TRUE if the command was successful FALSE otherwise

	Note:



************************************************************************************************************************/
#if 0
BOOL 
GetAddressString(
  LPTSTR pszName, 
  LPTSTR pszIpAddress
  )
{
  BOOL br = FALSE;
  struct      sockaddr_in Dest;
  if (0 != GetIpAddress(pszName, &Dest))
  {
    printf("Failed to retrieve IP address for %s\n", pszName);
  }
  else
  {
    br = TRUE;
    sprintf(pszIpAddress, "%d,%d,%d,%d", 
      Dest.sin_addr.S_un.S_un_b.s_b1, 
      Dest.sin_addr.S_un.S_un_b.s_b2,
      Dest.sin_addr.S_un.S_un_b.s_b3,
      Dest.sin_addr.S_un.S_un_b.s_b4
      );
  }
  return br;
}
#endif
/************************************************************************************************************************

	InitializeWinsock()

	Type:					Member Function
	Scope:				Public
	Base Class:		
	Parameters:		
  none

	returns: -1 on error, the high byte us the vbersio we have

	Note:



************************************************************************************************************************/
#if 0
WORD 
InitializeWinsock()
{
  WORD                    wret              = (WORD)-1;
  WSADATA     wsaData;
  //
  // walk the array of known versions and use the latest one we can get!!
  //
  for (int i = 0; i < sizeof(wsVersions)/sizeof(wsVersions[0]); i++)
  {
    if (WSAStartup( wsVersions[i], &wsaData )== 0 ) 
    {
      wret  = wsVersions[i];
      break;
    }
  }
  //return the version supported
  return wret;
}
#endif
