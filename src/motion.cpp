#include "motion.h"

namespace nmc
{

Artic::Artic(I32_T type) : devType(type)
{
}

Artic::~Artic()
{
}

bool Artic::initialize(void)
{
  //=================================================
  //              Step 1 : Device open up
  //=================================================
  printf("Start to openup device...\n");
  ret = NMC_DeviceOpenUp(devType, devIndex, &retDevID);
  if (ret != 0)
  {
    printf("ERROR! NMC_DeviceOpenUp: (%d)%s.\n", ret, NMC_GetErrorDescription(ret, nullptr, 0));
    shutdown();
  }
  else
    printf("\nDevice open up succeed, device ID: %d.\n", retDevID);

  ret = NMC_DeviceResetStateAll(retDevID);
  if (ret != NMC_AXIS_STATE_DISABLE)
  {
    printf("ERROR! NMC_DeviceResetStateAll, device ID: %d. (err code: %d)\n", retDevID, ret);
    shutdown();
  }
  else
    printf("Device ID %d: Device Reset State All success.\n", retDevID);

  //get device state
  ret = NMC_DeviceGetState(retDevID, &retDevState);
  if (retDevState != NMC_DEVICE_STATE_OPERATION)
  {
    printf("ERROR! Device open up failed, device ID: %d.\n", retDevID);
    shutdown();
  }
  else
    printf("Device ID %d: state is OPERATION.\n", retDevID);

  //=================================================
  //              Step 2 : Get device infomation
  //=================================================
  //Get amount of single axis
  ret = NMC_DeviceGetAxisCount(retDevID, &retSingleAxisCount);
  if (ret != 0)
  {
    printf("ERROR! NMC_DeviceGetAxisCount: (%d)%s.\n", ret, NMC_GetErrorDescription(ret, nullptr, 0));
    shutdown();
  }
  else
    printf("\nGet single axis count succeed, device has %d single axis.\n", retSingleAxisCount);

  //Get amount of GROUP
  ret = NMC_DeviceGetGroupCount(retDevID, &retGroupCount);
  if (ret != 0)
  {
    printf("ERROR! NMC_DeviceGetGroupCount: (%d)%s.\n", ret, NMC_GetErrorDescription(ret, nullptr, 0));
    shutdown();
  }
  else
    printf("Get group count succeed, device has %d group.\n", retGroupCount);

  if (retGroupCount != 1)
  {
    printf("This service only support ONE group ");
    shutdown();
  }
  else
  {
    group_index = 0;
  }


  //Get amount of AXIS of each GROUP
  ret = NMC_DeviceGetGroupAxisCount(retDevID, group_index, &retGroupAxisCount);
  if (ret != 0)
  {
    printf("ERROR! NMC_DeviceGetGroupAxisCount: (%d)%s.\n", ret, NMC_GetErrorDescription(ret, nullptr, 0));
    shutdown();
  }
  else
    printf("Get group axis count succeed, group index %d has %d axis.\n", group_index, retGroupAxisCount);

  if (retGroupAxisCount != 6)
  {
    printf("Right now, This service only support 6-axis (articulated) industrial robot");
    shutdown();
  }
  printf("\nReady to reset all drives in device...\n");

  //=================================================
  //       Step 4 : Clean alarm of drives of each group
  //=================================================
  ret = NMC_GroupResetDriveAlmAll(retDevID, group_index);
  if (ret != 0)
  {
    printf("ERROR! NMC_GroupResetDriveAlmAll: (%d)%s.\n", ret, NMC_GetErrorDescription(ret, nullptr, 0));
    shutdown();
  }

  //sleep
  Sleep(5000);

  //check state
  ret = NMC_GroupGetState(retDevID, group_index, &retState);
  if (ret != 0)
  {
    printf("ERROR! NMC_GroupGetState(group index %d): (%d)%s.\n", group_index, ret, NMC_GetErrorDescription(ret, nullptr, 0));
    shutdown();
  }

  if (retState != NMC_GROUP_STATE_DISABLE)
  {
    printf("ERROR! Group reset failed.(group index %d)\n", group_index);
    shutdown();
  }
  else
    printf("Group reset succeed.(group index %d)\n", group_index);

  return true;
}

bool Artic::enableRobot(void)
{
  //=================================================
  //       Step 5 : Enable group
  //=================================================
  ret = NMC_DeviceEnableAll(retDevID);
  if (ret != 0)
  {
    printf("ERROR! NMC_DeviceEnableAll: (%d)%s.\n", ret, NMC_GetErrorDescription(ret, nullptr, 0));
    shutdown();
  }
  else
    printf("\nReady to enable group...\n");

  //ret = _WaitGroupStandStill(retDevID, i);
  //if (ret != 0)
  //{
  //  goto _ERR_SHUTDOWN;
  //}

  //sleep
  Sleep(sleepTime);

  //check group state
  ret = NMC_GroupGetState(retDevID, group_index, &retState);
  if (ret != 0)
  {
    printf("ERROR! NMC_GroupGetState(group index %d): (%d)%s.\n", group_index, ret, NMC_GetErrorDescription(ret, nullptr, 0));
    shutdown();
  }

  if (retState != NMC_GROUP_STATE_STAND_STILL)
  {
    printf("ERROR! Group enable failed.(group index %d) (err code : %d) \n", group_index, retState);
    shutdown();
  }
  else
    printf("Group enable succeed.(group index %d)\n", group_index);

  //sleep
  Sleep(sleepTime);

  return true;
}

bool Artic::disableRobot(void)
{
  ret = NMC_DeviceDisableAll(retDevID);
  if (ret != 0)
  {
    printf("ERROR! NMC_DeviceDisableAll: (%d)%s.\n", ret, NMC_GetErrorDescription(ret, nullptr, 0));
    shutdown();
  }
  else
    printf("\nReady to disable group...\n");

  //sleep
  Sleep(sleepTime);

  //check group state
  ret = NMC_GroupGetState(retDevID, 0, &retState);
  if (ret != 0)
  {
    printf("ERROR! NMC_GroupGetState(group index %d): (%d)%s.\n", group_index, ret, NMC_GetErrorDescription(ret, nullptr, 0));
    shutdown();
  }

  if (retState != NMC_GROUP_STATE_DISABLE)
  {
    printf("ERROR! Group disable failed.(group index %d)\n", group_index);
    shutdown();
  }
  else
    printf("Group disable succeed.(group index %d)\n", group_index);
  return true;
}

bool Artic::shutdown(void)
{
  //=================================================
  //              Step 10 : Shutdown device
  //=================================================
  ret = NMC_DeviceDisableAll(retDevID);
  if (ret != 0)
    printf("ERROR! NMC_DeviceDisableAll: (%d) %s.\n", ret, NMC_GetErrorDescription(ret, nullptr, 0));
  else
    printf("\nDevice disable all succeed.\n");

  ret = NMC_DeviceShutdown(retDevID);
  if (ret != 0)
    printf("ERROR! NMC_DeviceShutdown: (%d) %s.\n", ret, NMC_GetErrorDescription(ret, nullptr, 0));
  else
    printf("\nDevice shutdown succeed.\n");

  return true;
}

bool Artic::moveJoint(std::vector<double>& vals)
{
    Pos_T   cmdPosition      = {0};
    I32_T   groupAxesIdxMask = 0;
    groupAxesIdxMask += NMC_GROUP_AXIS_MASK_X | NMC_GROUP_AXIS_MASK_Y |
                        NMC_GROUP_AXIS_MASK_Z | NMC_GROUP_AXIS_MASK_A |
                        NMC_GROUP_AXIS_MASK_B | NMC_GROUP_AXIS_MASK_C;

    for (size_t i = 0; i < vals.size(); ++i)
    {
      cmdPosition.pos[i] = vals[i];
    }
    ret = NMC_GroupPtpAcsAll( retDevID , group_index , groupAxesIdxMask , &cmdPosition );
    if( ret != 0 )
    {
        printf( "ERROR! NMC_GroupPtpAcsAll: (%d) %s.\n", ret, NMC_GetErrorDescription( ret, nullptr, 0 ) );
        //shutdown();
    }
    else
        printf( "\n NMC_GroupPtpAcsAll Success! Wait Command done.\n" );

    return true;
}


Pos_T Artic::getJointVal(void)
{
  ret = NMC_GroupGetActualPosAcs(retDevID, group_index, &acs_val);

  if (ret != ERR_NEXMOTION_SUCCESS)
  {
    printf("get actual acs pose failed. error code: %d\n %s.\n", ret, NMC_GetErrorDescription(ret, nullptr, 0));
  }

  return this->acs_val;
}

Pos_T Artic::getFlangeVal(void)
{
  ret = NMC_GroupGetActualPosPcs(retDevID, group_index, &pcs_val);

  if (ret != ERR_NEXMOTION_SUCCESS)
  {
    printf("get actual pcs pose failed. error code: %d\n %s.\n", ret, NMC_GetErrorDescription(ret, nullptr, 0));
  }

  return this->pcs_val;
}

void Artic::setDevType(I32_T type)
{
  this->devType = type;
  return;
}

bool Artic::checkState(void)
{
  ret = NMC_GroupGetState(retDevID, group_index, &group_state);
  if (ret != ERR_NEXMOTION_SUCCESS)
  {
    printf("get group state failed. error code: %d\n %s.\n", ret, NMC_GetErrorDescription(ret, nullptr, 0));
    return false;
  }
  if (group_state == NMC_GROUP_STATE_STAND_STILL)
    return true;
  else
    return false;
}

bool Artic::haltMotion(void)
{
  ret = NMC_GroupHalt(this->retDevID, this->group_index);
  if (ret == ERR_NEXMOTION_SUCCESS)
    return true;
  else
    return false;
}

void Artic::show3DView(void)
{
  ret = NMC_Group3DShow(retDevID, group_index);
  return;
}

void Artic::close3DView(void)
{
  ret = NMC_Group3DHide(retDevID, group_index);
  return;
}

char* Artic::displayErrorMessage(RTN_ERR err_code)
{
  const U32_T err_des_size= 512;
  char err_des[err_des_size] = "";
  NMC_GetErrorDescription(err_code, err_des, err_des_size);
  return err_des;
}

}
