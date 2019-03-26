#include "rc_interface.h"

bool WINAPI communicationThread(PVOID PARA)
{
  nmc::RCI* ui = (nmc::RCI*)PARA;
  nmc::Artic minibot = ui->getRobot();
  SOCKET sock = ui->getSocket();

  minibot.enableRobot();
  if(ui->show3D)
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

      if (cmd.compare("ptp") == 0)
      {
        std::string tmp = "You ask to Move PTP\n";
        if (params.size() < 6)
        {
          tmp += "But input parameters need 6 value\n";
          oResult = send(sock, tmp.c_str(), strlen(tmp.c_str()), 0);
          continue;
        }
        oResult = send(sock, tmp.c_str(), strlen(tmp.c_str()), 0);

        vector<double> jnt_val;
        printf("\nTarget Joint Value:\n");
        for (auto p : params)
        {
          jnt_val.emplace_back(std::stod(p));
          printf("%4.4f\t", jnt_val.back());
        }

        minibot.moveJoint(jnt_val);
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
      else if (cmd.compare("get state") == 0)
      {
        std::string tmp = "Still moving\n";
        if (minibot.checkState())
          tmp = "Motion Done\n";
        oResult = send(sock, tmp.c_str(), strlen(tmp.c_str()), 0);
      }
      else if (cmd.compare("halt") == 0)
      {
        printf("You ask to halt the minibot\n");
        bool result = minibot.haltMotion();

        std::string tmp = "Halt command send\n";
        if (!result)
          tmp = "Halt command failed\n";

        oResult = send(sock, tmp.c_str(), strlen(tmp.c_str()), 0);
      }
      else if (cmd.compare("movej") == 0)
      {
        std::string tmp = "You ask to Move Joint\n";
        oResult = send(sock, tmp.c_str(), strlen(tmp.c_str()), 0);
      }
      else if (cmd.compare("movel") == 0)
      {
        std::string tmp = "You ask to Move Line\n";
        oResult = send(sock, tmp.c_str(), strlen(tmp.c_str()), 0);
      }
      else
      {
        cmd = "UnKnown Command\nBack --> " + cmd + "\n";
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

  if(ui->show3D)
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

bool RCI::loadParameter(void)
{
  json config;
  std::ifstream ifstr ("C:\\NEXCOBOT\\config.json", std::ifstream::in);
  if (!ifstr.good())
  {
    std::cout << "Can't open the config file!\n";
    return -1;
  }

  ifstr >> config;
  std::cout << config.dump() << std::endl;
  this->port = config["port"].get<int>();
  this->mode = config["mode"].get<std::string>();
  this->show3D = config["show3D"].get<bool>();
  // check to find cwd
  char buffer[256];
  DWORD val = GetCurrentDirectoryA(sizeof(buffer), buffer);
  if (val)
  {
    std::cout << buffer << std::endl;
  }

  return 0;
}

bool RCI::initialize(void)
{
  if (this->mode.compare("Operation") == 0)
  { 
    minibot.setDevType(NMC_DEVICE_TYPE_ETHERCAT);
  }
  else if (this->mode.compare("Simulation") == 0)
  {
    minibot.setDevType(NMC_DEVICE_TYPE_SIMULATOR);
  }
  minibot.initialize();
  int ret = rosServer.createServer(this->port, "");
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
