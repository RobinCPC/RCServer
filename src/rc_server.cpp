#include "rc_server.h"
#include "motion.h"

using nmc::Artic;

Artic minibot = Artic(NMC_DEVICE_TYPE_SIMULATOR);

bool WINAPI communicationThread(PVOID PARA)
{
  minibot.enableRobot();
  minibot.show3DView();
  SOCKET sock = (SOCKET)PARA;

  int iResult = 0;
  int oResult = 0;
  std::string msg = "";

  // Keep Receive cmd until client close the connection
  do
  {
    char in_recvbuf[BUFLEN] = "";
    iResult = recv(sock, in_recvbuf, BUFLEN, 0);
    printf("recv result = %d\n", iResult);

    if (iResult > 0)
    {
      printf("Receive cmd from client: %s\n", in_recvbuf);
      msg = in_recvbuf;
      msg = msg.substr(0, msg.find("\r\n"));
      if (msg.compare("movej") == 0)
      {
        std::string tmp = "You ask to Move Joint\n";
        oResult = send(sock, tmp.c_str(), strlen(tmp.c_str()), 0);
      }
      else if (msg.compare("movel") == 0)
      {
        std::string tmp = "You ask to Move Line\n";
        oResult = send(sock, tmp.c_str(), strlen(tmp.c_str()), 0);
      }
      else if (msg.compare("ptp") == 0)
      {
        std::string tmp = "You ask to Move PTP\n";
        oResult = send(sock, tmp.c_str(), strlen(tmp.c_str()), 0);

        Pos_T joint_val = minibot.getJointVal();
        printf("Current Joint Value:\n");
        for (int p : joint_val.pos)
        {
          printf("%d\t", p);
        }
        Pos_T cart_val = minibot.getFlangeVal();
        printf("Current Cartesian Value:\n");
        for (int c : cart_val.pos)
        {
          printf("%d\t", c);
        }
        minibot.moveJoint();
      }
      else if (msg.compare("get joint position") == 0)
      {
        std::string tmp = "You ask to get joint value\n";

        Pos_T joint_val = minibot.getJointVal();
        printf("Current Joint Value:\n");
        tmp = "";
        for (int p : joint_val.pos)
        {
          tmp += std::to_string(p) + " ";
          printf("%d\t", p);
        }
        tmp += "\n";

        oResult = send(sock, tmp.c_str(), strlen(tmp.c_str()), 0);

      }
      else if (msg.compare("get flange frame") == 0)
      {
        std::string tmp = "You ask to get flange cartesian position\n";

        Pos_T cart_val = minibot.getFlangeVal();
        printf("Current Cartesian Value:\n");
        tmp = "";
        for (int p : cart_val.pos)
        {
          tmp += std::to_string(p) + " ";
          printf("%d\t", p);
        }
        tmp += "\n";

        oResult = send(sock, tmp.c_str(), strlen(tmp.c_str()), 0);

      }
      else
      {
        msg = "UnKnown Command\nBack --> " + msg + "\n";
        //msg = "Back --> "  + std::to_string(1.0) + in_recvbuf;
        oResult = send(sock, msg.c_str(), strlen(msg.c_str()), 0);
      }
    }
    else if(iResult == 0)
    {
      printf("Connection closed\n");
    }
    else
    {
      printf("recv failed: %d\n", WSAGetLastError());
    }

  } while (iResult > 0);

  // cleanup
  closesocket(sock);
  WSACleanup();

  minibot.close3DView();
  minibot.disableRobot();

  printf("End Rec Thread\n");
  return false;
}

RCServer::RCServer(void)
{
  listen_socket_ = INVALID_SOCKET;
  accept_socket_ = INVALID_SOCKET;
}

RCServer::~RCServer(void)
{}


bool RCServer::createServer(int iFamily, int iType, int iProtocol, int Port, char *IP)
{
  minibot.initialize();

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

  // close listen socket after accepting.
  iResult = closesocket(listen_socket_);
  if (iResult == SOCKET_ERROR)
  {
    printf("bind failed with error %u\n", WSAGetLastError());
    closesocket(listen_socket_);
    WSACleanup();
    return false;
  }

  // Create thread to handle incoming command from ROS
  HANDLE threadHD1;
  DWORD  dwID1;
  threadHD1=chBEGINTHREADEX(NULL, 0, communicationThread, accept_socket_, 0, &dwID1);

  WaitForSingleObject(threadHD1, INFINITE);

  minibot.shutdown();

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
  char recvbuf[BUFLEN];
  int recvbuflen = BUFLEN;
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
