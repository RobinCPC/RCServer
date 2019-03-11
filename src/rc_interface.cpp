#include "rc_interface.h"

bool WINAPI communicationThread(PVOID PARA)
{
  nmc::RCI* ui = (nmc::RCI*)PARA;
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
      ui->trim(msg);
      //msg = msg.substr(0, msg.find("\r\n"));
      vector<string> tokens = ui->split(msg, ":");
      string cmd;
      vector<string> params{};
      if (tokens.size() == 1)
      {
        cmd = ui->trim(tokens[0]);
      }
      else if (tokens.size() == 2)
      {
        cmd = ui->trim(tokens[0]);
        ui->trim(tokens[1]);
        params = ui->split(tokens[1]," ");
      }

      if (cmd.compare("movej") == 0)
      {
        std::string tmp = "You ask to Move Joint\n";
        oResult = send(sock, tmp.c_str(), strlen(tmp.c_str()), 0);
      }
      else if (cmd.compare("movel") == 0)
      {
        std::string tmp = "You ask to Move Line\n";
        oResult = send(sock, tmp.c_str(), strlen(tmp.c_str()), 0);
      }
      else if (cmd.compare("ptp") == 0)
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
      else if (cmd.compare("get joint position") == 0)
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
      else if (cmd.compare("get flange frame") == 0)
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
        cmd = "UnKnown Command\nBack --> " + cmd + "\n";
        //msg = "Back --> "  + std::to_string(1.0) + in_recvbuf;
        oResult = send(sock, cmd.c_str(), strlen(cmd.c_str()), 0);
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
  return true;
}

namespace nmc
{

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
  // Create thread to handle incoming command from ROS (through TCPIP)
  threadHD1=chBEGINTHREADEX(nullptr, 0, communicationThread, this, 0, &dwID1);

  WaitForSingleObject(threadHD1, INFINITE);

  return true;
}

bool RCI::terminate(void)
{
  minibot.shutdown();
  rosServer.closeServer();
  return true;
}

SOCKET RCI::getSocket(void)
{
  return this->rosServer.getAcceptSocket();
}

nmc::Artic RCI::getRobot(void)
{
  return this->minibot;
}

string& RCI::trim(string& str, const string& end_word)
{
  // left trim
  str.erase(0, str.find_first_not_of(end_word));
  // right trim
  str.erase(str.find_last_not_of(end_word) + 1);
  return str;
}

vector<string> RCI::split(string cmd, string delimiter)
{
  vector<string> list;
  size_t start = 0;
  size_t end = 0;
  string token;
  while ((end = cmd.find(delimiter, start)) != string::npos)
  {
    size_t len = end - start;
    token = cmd.substr(start, len);
    list.emplace_back(token);
    start += len + delimiter.length();
  }
  // catch last one
  token = cmd.substr(start, end - start);
  list.emplace_back(token);
  return list;
}


}
