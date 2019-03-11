#include <stdio.h>
#include <stdlib.h>
#include "rc_interface.h"

int main(int argc, const char *argv[])
{
  nmc::RCI ui = nmc::RCI();
  ui.initialize();
  ui.start();
  ui.terminate();

  printf("main open!\n");
  system("pause");
  return 0;
}

