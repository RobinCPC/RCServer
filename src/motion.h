/*!
 * @file        motion.h
 * @brief       A simplified motion library for NexGRC articulated (6-axis)
 *              robot type to bridge ROS.
 * @author      Chien-Pin Chen
 */
#ifndef MOTION_H_
#define MOTION_H_

#include "NexMotion.h"
#include "NexMotionError.h"
#include <Windows.h>
//#include <thread>
#include <stdio.h>
#include <stdlib.h>

namespace nmc
{
class Artic
{
public:
  Artic( I32_T type=NMC_DEVICE_TYPE_SIMULATOR);

  ~Artic();

  bool initialize(void);

  bool enableRobot(void);

  bool disableRobot(void);

  bool shutdown(void);

  bool moveJoint(void);

  Pos_T getJointVal(void);

  Pos_T getFlangeVal(void);

  void show3DView(void);

  void close3DView(void);

  char* displayErrorMessage(RTN_ERR err_code);

private:
  RTN_ERR ret              = 0;
  I32_T devIndex           = 0;
  I32_T retDevID           = 0;
  I32_T retSingleAxisCount = 0;
  I32_T retGroupCount      = 0;
  I32_T retGroupAxisCount  = 0;
  I32_T retState           = 0;
  I32_T retDevState        = 0;

  I32_T group_index        = 0;
  I32_T group_state        = 0;
  I32_T group_status       = 0;
  I32_T vel_ratio          = 0;
  I32_T devType;
  Pos_T acs_val;
  Pos_T pcs_val;

  const int numAxis = 6;
  const int sleepTime = 500;

};  // class artic

}   // namespace nmc

#endif
