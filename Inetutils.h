//
// Gets the IP address of the specified host computer
//
BOOL 
GetIpAddress(
    char *hostname, 
    struct sockaddr_in  * pDest
  );

//
// Sends a command to host socket as specified by pszCommand and pszData
//
BOOL 
SendCommand(
  SOCKET sd, 
  LPTSTR pszCommand, 
  LPTSTR pszData
  );

//
// Retrieves a reply from the socket
//
BOOL 
GetServerResponse(
  SOCKET sd
  );

//
// Binds the socket to the specified port
//
BOOL 
BindToPort(
  SOCKET sd, 
  int port
  );

//
// Sends a command on the specified socket and waits on a response
//
BOOL
SendCommandAndWait(
    SOCKET sd, 
    LPTSTR pszCommand, 
    LPTSTR pszData
  );

//
// Initializes the winsock driver returning the version initialized to!
//
WORD 
InitializeWinsock(
  void
  );

//
// Get the IP address of the specified name
BOOL 
GetAddressString(
  LPTSTR pszName, 
  LPTSTR pszIpAddress
  );

SOCKET NewStreamSocket();
