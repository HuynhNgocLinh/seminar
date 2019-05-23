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
#ifndef __FOS_H__
#define __FOS_H__

#ifdef __cplusplus
extern "C" {
#endif
/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include "stdint.h"
#include "string.h"

#include "fiot_hal_timer.h"

/*******************************************************************************
 * DRIVER'S CONFIGS
 ******************************************************************************/
/**
 * @brief Config the timer of fos
 */
#define FOS_TICK_TIMER (TIMER_3)
/**
 * @brief Tick period of Timer in fos
 */
#define FOS_TICK_PERIOD (10) //10ms
/**
 * @brief Max task that is supported from fos
 */
#define FOS_MAX_TASK_SYS (50)
/**
 * @brief Max task that is supported from fos
 */
#define FOS_MAX_TASK_EVT (50)
/**
 * @brief Max Timer task that is supported from fos
 */
#define FOS_MAX_TASK_TIMER (10)
/*******************************************************************************
 * MACROS
 ******************************************************************************/

/*******************************************************************************
 *  CONSTANTS
 ******************************************************************************/

/*******************************************************************************
 *  TYPEDEFS
 ******************************************************************************/
typedef enum {
	FOS_OK,
	FOS_ERROR
} FOS_Status_T;

typedef enum {
	FOS_EVT_DISABLE,
	FOS_EVT_ENABLE
} FOS_Evt_T;

typedef enum {
	FOS_DISABLE,
	FOS_ENABLE
} FOS_State_T;

typedef enum {
	FOS_TYPE_SYS,
	FOS_TYPE_EVENT,
  FOS_TYPE_TIMER
} FOS_Type_T;

typedef uint16_t FOS_Idx_T;
typedef uint32_t FOS_Time_T;

typedef void (*FOS_Fxn_Sys_T)(void);

typedef void (*FOS_Fxn_Evt_T)(void);

typedef struct
{
	FOS_Idx_T Idx;
	FOS_Type_T Type;
	FOS_State_T State;
} FOS_Handle_T;
/*******************************************************************************
 * FUNCTIONS - API
 ******************************************************************************/
/*!
 *******************************************************************************
 * @fn      void FOS_Init(void)
 *
 * @brief   Init fos
 *
 * @param   none
 *
 * @return  none
 ******************************************************************************/
void FOS_Init(void);
/*!
 *******************************************************************************
 * @fn      void FOS_Start(void)
 *
 * @brief   Start Timer for check Evt. This function must be called before
 * 					FOS_Run function
 *
 * @param   none
 *
 * @return  none
 ******************************************************************************/
void FOS_Start(void);
/*!
 *******************************************************************************
 * @fn      void FOS_Run(void)
 *
 * @brief   Check and Run all evt and sys task
 *
 * @param   none
 *
 * @return  none
 ******************************************************************************/
void FOS_Run(void);
/*!
 *******************************************************************************
 * @fn      FOS_Status_T FOS_CreateTask(FOS_Handle_T *iFos,
 * 																		 FOS_Type_T iType,
 * 																		 FOS_Fxn_Evt_T iFxn,
 * 																		 FOS_Time_T iPeriod,
 * 																		 FOS_State_T iState);
 *
 * @brief   Generate a evt/sys task.
 *
 * @param   iFos - handle for fos Task
 * @param   iType - Type of Function.	FOS_TYPE_SYS/FOS_TYPE_EVENT
 * @param   iFxn_Evt - Function of task. 
 * 					if iType is FOS_TYPE_SYS, This function is called with background
 * 					if iType is FOS_TYPE_EVENT, This function is called with period
 * @param   iPeriod - if Tyoe is FOS_TYPE_EVENT, This is period of task. Unit is ms
 * @param   iState - set state of Task. FOS_ENABLE/FOS_DISABLE
 *
 * @return  FOS_OK
 * @return  FOS_ERROR
 ******************************************************************************/
FOS_Status_T FOS_CreateTask(FOS_Handle_T *iFos,
														FOS_Type_T iType,
														FOS_Fxn_Evt_T iFxn,
														FOS_Time_T iPeriod,
														FOS_State_T iState);
/*!
 *******************************************************************************
 * @fn      FOS_Status_T FOS_SetState(FOS_Handle_T *iFos,FOS_State_T iState)
 *
 * @brief   Set State of evt/sys task.
 *
 * @param   iFos - handle for fos Task
 * @param   iState - state of task. FOS_ENABLE/FOS_DISABLE 
 * 					to enable or disable a task
 *
 * @return  FOS_OK
 * @return  FOS_ERROR
 ******************************************************************************/
FOS_Status_T FOS_SetState(FOS_Handle_T *iFos, FOS_State_T iState);
/*!
 *******************************************************************************
 * @fn      FOS_Status_T FOS_SetTick(FOS_Handle_T *iFos, FOS_Time_T iTick)
 *
 * @brief   Set tick for a evt task. This function dont support for sys task
 *
 * @param   iFos - handle for fos Task
 * @param   iTick - tick time of evt task. Unit is ms
 *
 * @return  FOS_OK
 * @return  FOS_ERROR
 ******************************************************************************/
FOS_Status_T FOS_SetTick(FOS_Handle_T *iFos, FOS_Time_T iTick);
/*!
 *******************************************************************************
 * @fn      FOS_Status_T FOS_SetPeriod(FOS_Handle_T *iFos, FOS_Time_T iPeriod)
 *
 * @brief   Set period for a evt task. This function dont support for sys task
 *
 * @param   iFos - handle for fos Task
 * @param   iPeriod - period time. Unit is ms
 *
 * @return  FOS_OK
 * @return  FOS_ERROR
 ******************************************************************************/
FOS_Status_T FOS_SetPeriod(FOS_Handle_T *iFos, FOS_Time_T iPeriod);
/*!
 *******************************************************************************
 * @fn      FOS_Status_T FOS_SetDelay(FOS_Handle_T *iFos, FOS_Time_T iDelay)
 *
 * @brief   Set delay for a sys task. This function dont support for evt task
 *
 * @param   iFos - handle for fos Task
 * @param   iDelay - delay time. Unit is ms
 *
 * @return  FOS_OK
 * @return  FOS_ERROR
 ******************************************************************************/
FOS_Status_T FOS_SetDelay(FOS_Handle_T *iFos, FOS_Time_T iDelay);
/*!
 *******************************************************************************
 * @fn      FOS_Time_T FOS_Timer_GetTick(FOS_Handle_T *iFos)
 *
 * @brief   Get Tick of Timer
 *
 * @param   iFos - handle for fos Task
 *
 * @return  Number of Tick
 ******************************************************************************/
FOS_Time_T FOS_Timer_GetTick(FOS_Handle_T *iFos);
/******************************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* __FOS_H__ */
