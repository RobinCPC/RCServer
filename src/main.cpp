#include <stdio.h>
#include <stdlib.h>
#include "rc_server.h"
#include "motion.h"
#include <process.h>  // for _beginthreadex
//#include <thread>

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

// Coontroller Remote Control Interface
class RCI
{
public:
  RCI();

  ~RCI();

  bool initialize(void);
  bool start(void);

  SOCKET getSocket(void);
  nmc::Artic getRobot(void);

  //bool WINAPI communicationThread(PVOID PARA);

private:
  bool isStart;
  RCServer rosServer;
  nmc::Artic minibot = nmc::Artic(NMC_DEVICE_TYPE_SIMULATOR);
  HANDLE threadHD1;
  DWORD dwID1;
};

RCI::RCI()
{
  isStart = false;
}

RCI::~RCI(){}

bool RCI::initialize(void)
{
  minibot.initialize();
  int ret = rosServer.createServer(AF_INET, SOCK_STREAM, IPPROTO_TCP, PORT, "");
  return ret;
}

bool RCI::start(void)
{
  rosServer.connectServer();
  // Create thread to handle incoming command from ROS
  threadHD1=chBEGINTHREADEX(NULL, 0, communicationThread, this, 0, &dwID1);

  WaitForSingleObject(threadHD1, INFINITE);

  minibot.shutdown();

  return true;
}

SOCKET RCI::getSocket(void)
{
  return rosServer.accept_socket_;
}

nmc::Artic RCI::getRobot(void)
{
  return this->minibot;
}

bool WINAPI communicationThread(PVOID PARA)
{
  RCI* ui = (RCI*)PARA;
  nmc::Artic minibot = ui->getRobot();
  SOCKET sock = ui->getSocket();
  minibot.enableRobot();
  minibot.show3DView();

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
        for (F64_T p : joint_val.pos)
        {
          printf("%4.4f\t", p);
        }
        Pos_T cart_val = minibot.getFlangeVal();
        printf("Current Cartesian Value:\n");
        for (F64_T c : cart_val.pos)
        {
          printf("%4.4f\t", c);
        }
        minibot.moveJoint();
      }
      else if (msg.compare("get joint position") == 0)
      {
        std::string tmp = "You ask to get joint value\n";

        Pos_T joint_val = minibot.getJointVal();
        printf("Current Joint Value:\n");
        tmp = "";
        for (F64_T p : joint_val.pos)
        {
          tmp += std::to_string(p) + " ";
          printf("%4.4f\t", p);
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
        for (F64_T p : cart_val.pos)
        {
          tmp += std::to_string(p) + " ";
          printf("%4.4f\t", p);
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

int main(int argc, const char *argv[])
{
  //RCServer rosServer;
  //rosServer.createServer(AF_INET, SOCK_STREAM, IPPROTO_TCP, PORT, "");

  RCI ui = RCI();
  ui.initialize();
  ui.start();

  printf("main open!\n");
  system("pause");
  return 0;
}

