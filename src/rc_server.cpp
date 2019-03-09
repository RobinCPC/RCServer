#include "rc_server.h"

RCServer::RCServer(void)
{
  listen_socket_ = INVALID_SOCKET;
  accept_socket_ = INVALID_SOCKET;
}

RCServer::~RCServer(void)
{}


bool RCServer::createServer(int iFamily, int iType, int iProtocol, int Port, char *IP)
{
  WORD wVersionRequested;
  WSADATA wsaData;
  int err;

  // Request Winsock version 2.2
  //==========================================================
  wVersionRequested = MAKEWORD(2, 2);
  if ((err = WSAStartup(wVersionRequested, &wsaData)) != 0)
  {  ///// WSAStartup => Winsock Initialize
    printf("Server: WSAStartup() failed with error %d\n", err);
    return false;
  }
  else
    printf("Server: WSAStartup() is OK.\n");

  /* Confirm that the WinSock DLL supports 2.2.        */
  /* Note that if the DLL supports versions greater    */
  /* than 2.2 in addition to 2.2, it will still return */
  /* 2.2 in wVersion since that is the version we      */
  /* requested.                                        */

  if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
  {
    /* Tell the user that we could not find a usable */
    /* WinSock DLL.                                  */
    printf("Could not find a usable version of Winsock.dll\n");
    WSACleanup(); ///// WSAStartup => Winsock Close
    return false;
  }
  else
    printf("The Winsock 2.2 dll was found okay\n");

  // Create a listening socket for connect to server
  //int iFamily = AF_INET;
  //int iType = SOCK_STREAM;
  //int iProtocol = IPPROTO_IP;
  int iResult = 0;
  printf("Creating listen socket...");
  listen_socket_ = socket(iFamily, iType, iProtocol);

  if (listen_socket_ == INVALID_SOCKET)
  {
    printf("socket function failed with error = %d\n", WSAGetLastError());
    WSACleanup();
    return false;
  }
  else
  {
    printf("socket create success\n");
  }

  // The socket addres to be passed to bind
  sockaddr_in service;

  //The sockaddr_in structure specifies the address family,
  //IP address, and port for the socket that is beinf bind.
  service.sin_family = iFamily;
  service.sin_addr.s_addr = INADDR_ANY;
  service.sin_port = htons(Port);

  // Bind the listen socket
  iResult = bind(listen_socket_, (SOCKADDR *)&service, sizeof(service));
  if (iResult == SOCKET_ERROR)
  {
    printf("bind failed with error %u\n", WSAGetLastError());
    closesocket(listen_socket_);
    WSACleanup();
    return false;
  }
  else
  {
    printf("bind return success\n");
  }

  // Listen to socket
  if (listen(listen_socket_, SOMAXCONN) == SOCKET_ERROR)
    printf("listen function failed with error: %d\n", WSAGetLastError());
  else
    printf("Listening on socket...\n");

  return true;
}

bool RCServer::connectServer(void)
{
  // Accept (and wait for) the connection
  accept_socket_ = accept(listen_socket_, NULL, NULL);
  if (accept_socket_ == INVALID_SOCKET)
  {
    printf("accept failed with error %ld\n", WSAGetLastError());
    closesocket(listen_socket_);
    WSACleanup();
    return false;
  }
  else
  {
    printf("Client connected.\n");
  }

  return true;
}

bool RCServer::isRec(char*buf,int &length)
{
  return false;
}

bool RCServer::sendWithoutThread(char *buf,int length)
{
  return false;
}

bool RCServer::closeServer(void)
{
  int iResult;

  // shutdown the connection since no more data will be sent
  iResult = shutdown(listen_socket_, SD_SEND);
  if (iResult == SOCKET_ERROR)
  {
    printf("shutdown failed: %d\n", WSAGetLastError());
    closesocket(listen_socket_);
    WSACleanup();
    return 1;
  }

  // cleanup
  closesocket(listen_socket_);
  WSACleanup();

  return true;
}
