/*!
 * @file        rc_server.h
 * @brief       A socket library for NexGRC to bridge ROS.
 * @author      Chien-Pin Chen
 */
#ifndef RC_SERVER_H_
#define RC_SERVER_H_

#include <stdio.h>
#include <string>
#include <winsock2.h> // for socket
#include <process.h>  // for _beginthreadex

#pragma comment (lib, "Ws2_32.lib")

#define BUFLEN 1024
#define PORT 1688

//define thread
//=================================================================
typedef unsigned(__stdcall *PTHREAD_START) (void *);
#define chBEGINTHREADEX(psa,cbStack,pfnStartAddr, pvParam,fdwCreate, pdwThreadID) \
              ((HANDLE) _beginthreadex( \
              (void*) (psa), \
              (unsigned)(cbStack),\
              (PTHREAD_START) (pfnStartAddr),\
              (void *) (pvParam),\
              (unsigned)(fdwCreate),\
              (unsigned*) (pdwThreadID)))


bool WINAPI communicationThread(PVOID PARA);

/*! \class RCServer
 *  \brief A class to build socket (TCP/IP) to bridge NexGRC with ROS
 *
 *  Detailed description
 */
class RCServer
{
public:
  RCServer(void);
  ~RCServer();

public:
  bool createServer(int iFamily, int iType, int iProtocol, int Port, char *IP);
  bool isRec(char*buf,int &length);
  bool sendWithoutThread(char *buf,int length);
  bool closeServer(void);

protected:
  SOCKET listen_socket_;  /*!< socket to listen and bind */
  SOCKET accept_socket_;  /*!< socket to accept connection */
  char string_buf_[BUFLEN];
};

#endif
