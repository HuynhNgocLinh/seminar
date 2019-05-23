/*!
 *  @file       fos.h
 *  @brief      driver to implement of Fusion OS
 *
 *  @author  	  Vy Luu
 *	@copyright  Fiot Co.,Ltd
 *  @version    1.0
 *  @date       2018-01-25
 *  @example		
 */
/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include "fos.h"
/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/

/*******************************************************************************
 *  TYPEDEFS
 ******************************************************************************/
typedef struct
{
  FOS_State_T State[FOS_MAX_TASK_SYS];
  FOS_Fxn_Sys_T Fxn[FOS_MAX_TASK_SYS];
  FOS_Time_T Delay[FOS_MAX_TASK_SYS];
  FOS_Idx_T Idx;
} FOS_Task_Sys_T;

typedef struct
{
  FOS_State_T State[FOS_MAX_TASK_EVT];
  FOS_Fxn_Evt_T Fxn[FOS_MAX_TASK_EVT];
  FOS_Time_T Period[FOS_MAX_TASK_EVT];
  FOS_Time_T Tick[FOS_MAX_TASK_EVT];
  FOS_Evt_T Evt[FOS_MAX_TASK_EVT];
  FOS_Idx_T Idx;
} FOS_Task_Evt_T;

typedef struct
{
  FOS_State_T State[FOS_MAX_TASK_TIMER];
  FOS_Time_T Tick[FOS_MAX_TASK_TIMER];
  FOS_Idx_T Idx;
} FOS_Task_Timer_T;


/*******************************************************************************
 * VARIABLES - LOCAL
 ******************************************************************************/
//Timer
HAL_Timer_T mFOS_Tick;
//Task
FOS_Task_Sys_T mFOS_Task_Sys = {
    .Idx = 0,
};
FOS_Task_Evt_T mFOS_Task_Evt = {
    .Idx = 0,
};
FOS_Task_Timer_T mFOS_Task_Timer = {
    .Idx = 0,
};
/*******************************************************************************
 * FUNCTIONS - LOCAL
 ******************************************************************************/
void FOS_Tick_Init(void);
/*******************************************************************************
 * CALLBACK - TIMER 0
 ******************************************************************************/
void FOS_Tick_Cb(void);

void FOS_Tick_Cb(void)
{
  for (uint8_t i = 0; i < mFOS_Task_Evt.Idx; i++)
  {
    if (mFOS_Task_Evt.State[i] == FOS_ENABLE)
    {
      if (mFOS_Task_Evt.Tick[i] >= mFOS_Task_Evt.Period[i])
      {
        mFOS_Task_Evt.Evt[i] = FOS_EVT_ENABLE;
        mFOS_Task_Evt.Tick[i] = 0;
      }
      else
      {
        mFOS_Task_Evt.Tick[i] += FOS_TICK_PERIOD;
      }
    }
    else
    {
      mFOS_Task_Evt.Evt[i] = FOS_EVT_DISABLE;
    }
  }
  for (uint8_t i = 0; i < mFOS_Task_Sys.Idx; i++)
  {
    if (mFOS_Task_Sys.Delay[i] > 0)
    {
      if (mFOS_Task_Sys.Delay[i] >= FOS_TICK_PERIOD)
      {
        mFOS_Task_Sys.Delay[i] -= FOS_TICK_PERIOD;
      }
      else
      {
        mFOS_Task_Sys.Delay[i] = 0;
      }
    }
  }
  for (uint8_t i = 0; i < mFOS_Task_Timer.Idx; i++)
  {
    if (mFOS_Task_Timer.State[i] == FOS_ENABLE)
    {
      mFOS_Task_Timer.Tick[i]+= FOS_TICK_PERIOD;
    }
  }  
}
/*******************************************************************************
 * FUNCTIONS - API
 ******************************************************************************/
void FOS_Init(void)
{
  FOS_Tick_Init();
}
void FOS_Start(void)
{
  FIOT_HAL_Timer_Start(&mFOS_Tick);
}
void FOS_Run(void)
{
  //Run Update Function
  for (FOS_Idx_T i = 0; i < mFOS_Task_Sys.Idx; i++)
  {
    if (mFOS_Task_Sys.State[i] == FOS_ENABLE &&
        mFOS_Task_Sys.Delay[i] == 0)
    {
      if (mFOS_Task_Sys.Fxn[i] != 0)
      {
        mFOS_Task_Sys.Fxn[i]();
      }
    }
  }
  //Check Event
  for (FOS_Idx_T i = 0; i < mFOS_Task_Evt.Idx; i++)
  {
    if (mFOS_Task_Evt.State[i] == FOS_ENABLE)
    {
      if (mFOS_Task_Evt.Evt[i] == FOS_EVT_ENABLE)
      {
        //Clear Event
        mFOS_Task_Evt.Evt[i] = FOS_EVT_DISABLE;
        //Run Event Function
        if (mFOS_Task_Evt.Fxn[i] != 0)
        {
          mFOS_Task_Evt.Fxn[i]();
        }
      }
    }
  }
}
FOS_Status_T FOS_CreateTask(FOS_Handle_T *iFos,
                            FOS_Type_T iType,
                            FOS_Fxn_Evt_T iFxn,
                            FOS_Time_T iPeriod,
                            FOS_State_T iState)
{

  if (iType == FOS_TYPE_EVENT && iPeriod < FOS_TICK_PERIOD)
  {
    return (FOS_ERROR);
  }
  if (iType == FOS_TYPE_SYS && mFOS_Task_Sys.Idx >= FOS_MAX_TASK_SYS)
  {
    return (FOS_ERROR);
  }
  if (iType == FOS_TYPE_EVENT && mFOS_Task_Evt.Idx >= FOS_MAX_TASK_EVT)
  {
    return (FOS_ERROR);
  }
  //Update Type of Task
  iFos->Type = iType;
  iFos->State = iState;
  //Update Information
  switch (iType)
  {
  case FOS_TYPE_SYS:
  {
    //Update Idx for Handler
    iFos->Idx = mFOS_Task_Sys.Idx;
    //Update Type of Task
    iFos->Type = FOS_TYPE_SYS;
    //Store iFxn_Update
    mFOS_Task_Sys.Fxn[mFOS_Task_Sys.Idx] = iFxn;
    //Set state
    mFOS_Task_Sys.State[mFOS_Task_Sys.Idx] = iState;
    //Clear Delay
    mFOS_Task_Sys.Delay[mFOS_Task_Sys.Idx] = 0;
    //Insert Idx
    mFOS_Task_Sys.Idx++;
    break;
  }
  case FOS_TYPE_EVENT:
  {
    //Update Idx for Handler
    iFos->Idx = mFOS_Task_Evt.Idx;
    //Store iFxn_Evt
    mFOS_Task_Evt.Fxn[mFOS_Task_Evt.Idx] = iFxn;
    //Store Period
    mFOS_Task_Evt.Period[mFOS_Task_Evt.Idx] = iPeriod;
    //Clear Tick
    mFOS_Task_Evt.Tick[mFOS_Task_Evt.Idx] = 0;
    //Default Task is Disable
    mFOS_Task_Evt.State[mFOS_Task_Evt.Idx] = iState;
    //Insert Idx
    mFOS_Task_Evt.Idx++;
    break;
  }
  case FOS_TYPE_TIMER:
  {
    //Update Idx for Handler
    iFos->Idx = mFOS_Task_Timer.Idx;
    //Clear Tick
    mFOS_Task_Timer.Tick[mFOS_Task_Timer.Idx] = 0;
    //Default Task is Disable
    mFOS_Task_Timer.State[mFOS_Task_Timer.Idx] = iState;
    //Insert Idx
    mFOS_Task_Timer.Idx++;    
    break;
  }
  default:
  {
    break;
  }
  }
  return (FOS_OK);
}
FOS_Status_T FOS_SetState(FOS_Handle_T *iFos, FOS_State_T iState)
{
  iFos->State = iState;
  if (iFos->Type == FOS_TYPE_SYS)
  {
    mFOS_Task_Sys.State[iFos->Idx] = iState;
    mFOS_Task_Sys.Delay[iFos->Idx] = 0;
  }
  else
  if (iFos->Type == FOS_TYPE_EVENT)  
  {
    mFOS_Task_Evt.State[iFos->Idx] = iState;
    mFOS_Task_Evt.Tick[iFos->Idx] = 0;
  }
  else
  if (iFos->Type == FOS_TYPE_TIMER)  
  {
    mFOS_Task_Timer.State[iFos->Idx] = iState;
    mFOS_Task_Timer.Tick[iFos->Idx] = 0;
  }
  return (FOS_OK);
}
FOS_Status_T FOS_SetTick(FOS_Handle_T *iFos, FOS_Time_T iTick)
{
  if (iFos->Type == FOS_TYPE_SYS)
  {
    return (FOS_ERROR);
  }
  mFOS_Task_Evt.Tick[iFos->Idx] = iTick;
  mFOS_Task_Evt.Evt[iFos->Idx] = FOS_EVT_DISABLE;
  return (FOS_OK);
}
FOS_Status_T FOS_SetPeriod(FOS_Handle_T *iFos, FOS_Time_T iPeriod)
{
  if (iFos->Type == FOS_TYPE_SYS)
  {
    return (FOS_ERROR);
  }
  mFOS_Task_Evt.Tick[iFos->Idx] = 0;
  mFOS_Task_Evt.Evt[iFos->Idx] = FOS_EVT_DISABLE;
  mFOS_Task_Evt.Period[iFos->Idx] = iPeriod;
  return (FOS_OK);
}
FOS_Status_T FOS_SetDelay(FOS_Handle_T *iFos, FOS_Time_T iDelay)
{
  if (iFos->Type == FOS_TYPE_EVENT)
  {
    return (FOS_ERROR);
  }
  mFOS_Task_Sys.Delay[iFos->Idx] = iDelay;
  return (FOS_OK);
}
FOS_Time_T FOS_Timer_GetTick(FOS_Handle_T *iFos)
{
  return mFOS_Task_Timer.Tick[iFos->Idx];
}
/*******************************************************************************
 * FUNCTIONS - LOCAL
 ******************************************************************************/
void FOS_Tick_Init(void)
{
  HAL_Timer_Setting_T FOS_Tick_Settings;
  //Init the Timer 1 to detect mode hold every 3s
  FOS_Tick_Settings.Module = FOS_TICK_TIMER;
  //Interrupt enable every 100 MS
  FOS_Tick_Settings.Period = FOS_TICK_PERIOD;
  //Init Timer
  FIOT_HAL_Timer_Init(&mFOS_Tick, FOS_Tick_Settings, FOS_Tick_Cb);
  //Init Tick Count and Event
  for (uint8_t i = 0; i < FOS_MAX_TASK_EVT; i++)
  {
    mFOS_Task_Evt.Tick[i] = 0;
    mFOS_Task_Evt.Period[i] = 0;
    mFOS_Task_Evt.Evt[i] = FOS_EVT_DISABLE;
  }
}
/*******************************************************************************
 * END
 ******************************************************************************/
