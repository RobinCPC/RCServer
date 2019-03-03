#include <stdio.h>
#include <stdlib.h>
#include "rc_server.h"
//#include <thread>

int main(int argc, const char *argv[])
{
  RCServer rosServer;
  rosServer.createServer(AF_INET, SOCK_STREAM, IPPROTO_TCP, PORT, "");

  printf("main open!\n");
  system("pause");
  return 0;
}

