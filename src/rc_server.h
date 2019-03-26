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

#pragma comment (lib, "Ws2_32.lib")

#define BUFLEN 1024
#define PORT 1688

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
  bool createServer(int Port, char *IP);
  bool connectServer(void);
  bool closeServer(void);
  SOCKET getAcceptSocket(void);
  bool isRec(char*buf,int &length);
  bool sendWithoutThread(char *buf,int length);

protected:
  SOCKET listen_socket_;  /*!< socket to listen and bind */
  SOCKET accept_socket_;  /*!< socket to accept connection */
  char string_buf_[BUFLEN];
};

#endif
