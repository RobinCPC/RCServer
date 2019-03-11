/*!
 * @file        rc_interface.h
 * @brief       A interface for NexGRC to bridge ROS.
 * @author      Chien-Pin Chen
 */
#ifndef RC_INTERFACE_H_
#define RC_INTERFACE_H_

#include "rc_server.h"
#include "motion.h"
#include <process.h>  // for _beginthreadex
#include <vector>
//#include <thread>

using std::vector;
using std::string;

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

namespace nmc
{
// Coontroller Remote Control Interface
class RCI
{
public:
  RCI();

  ~RCI();

  bool initialize(void);
  bool start(void);
  bool terminate(void);

  SOCKET getSocket(void);
  Artic getRobot(void);

  string& trim(string &str, const string& end_word = "\t\n\v\f\r ");
  vector<string> split(string cmd, string delimiter);

private:
  bool isStart;
  RCServer rosServer;
  Artic minibot = Artic(NMC_DEVICE_TYPE_SIMULATOR);
  HANDLE threadHD1;
  DWORD dwID1;
};

}

#endif
