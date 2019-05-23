/*!
 * @file       fiot_hal_timer.h
 * @author  	 Binh Thai
 * @copyright  Fiot Co.,Ltd
 * @version    1.0.2.20180418
 * @date       Apr 18, 2018
 * @brief      FIOT HAL driver of Timer.
 *             See example_hal_timer.c to know how to use HAL Timer APIs
 * @note 			    - Define in fiot_hal_define.h to choose the MCU
 *                  + Define __FIOT_STM32F407_ for STM32F407
 *                  + Define __FIOT_STM32F411_ for STM32F411
 *                  + Define __FIOT_STM32F103_ for STM32F103
 *                  + Define __FIOT_SAM3X8E_ for SAM3X8E
 *                  + Define __FIOT_ATMEGA328P_ for ATMEGA328P
 *                  + Define __FIOT_CC2650_ for CC2650
 *                  + Define __FIOT_NRF52832_ for NRF52832
 * @example		 example_hal_timer.c
 * 						 This is an example of how to use HAL Timer APIs.
 * 						 More details about this example.
 */
#ifndef __HAL_TIMER_H__
#define __HAL_TIMER_H__

#ifdef __cplusplus
extern "C" {
#endif
/*******************************************************************************
 *	INCLUDES
 ******************************************************************************/
#include "fiot_hal_define.h"
/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/
#ifdef __FIOT_STM32F407_
//!Priority of TIMER_1
#define IRQ_PRIORITY_TIM1 		(2)
//!Priority of TIMER_2
#define IRQ_PRIORITY_TIM2 		(2)
//!Priority of TIMER_3
#define IRQ_PRIORITY_TIM3 		(2)
//!Priority of TIMER_4
#define IRQ_PRIORITY_TIM4 		(2)
//!Priority of TIMER_5
#define IRQ_PRIORITY_TIM5 		(2)
//!Priority of TIMER_6
#define IRQ_PRIORITY_TIM6 		(2)
//!Priority of TIMER_7
#define IRQ_PRIORITY_TIM7 		(2)
//!Priority of TIMER_8
#define IRQ_PRIORITY_TIM8 		(2)
//!Priority of TIMER_9
#define IRQ_PRIORITY_TIM9 		(2)
//!Priority of TIMER_10
#define IRQ_PRIORITY_TIM10 		(2)
//!Priority of TIMER_11
#define IRQ_PRIORITY_TIM11		(2)
//!Priority of TIMER_12
#define IRQ_PRIORITY_TIM12		(2)
//!Priority of TIMER_13
#define IRQ_PRIORITY_TIM13		(2)
//!Priority of TIMER_14
#define IRQ_PRIORITY_TIM14		(2)
#endif // __FIOT_STM32F407_
/******************************************************************************/
#ifdef __FIOT_STM32F103_
//!Priority of TIMER_1
#define IRQ_PRIORITY_TIM1 		(4)
//!Priority of TIMER_2
#define IRQ_PRIORITY_TIM2 		(2)
//!Priority of TIMER_3
#define IRQ_PRIORITY_TIM3 		(2)
//!Priority of TIMER_4
#define IRQ_PRIORITY_TIM4 		(2)
//!Priority of TIMER_5
#define IRQ_PRIORITY_TIM5 		(2)
//!Priority of TIMER_6
#define IRQ_PRIORITY_TIM6 		(2)
//!Priority of TIMER_7
#define IRQ_PRIORITY_TIM7 		(2)
//!Priority of TIMER_8
#define IRQ_PRIORITY_TIM8 		(2)
#endif // __FIOT_STM32F103_
/*******************************************************************************
 * TYPEDEF
 ******************************************************************************/
//!Status of Timer
typedef enum
{
	TIMER_STATUS_OK,            /**<Success*/
  TIMER_STATUS_ERROR_OPTION,  /**<Error input options are not valid*/
  TIMER_STATUS_ERROR_NULL_PTR /**<Error input null pointer*/
}
HAL_Timer_Status_T;
//!Module of Timer
typedef enum
{
#ifdef __FIOT_CC2650_
	TIMER_0A,
	TIMER_0B,
	TIMER_1A,
	TIMER_1B,
	TIMER_2A,
	TIMER_2B,
	TIMER_3A,
	TIMER_3B,
#endif // __FIOT_CC2650_

#ifdef __FIOT_NRF52832_
  TIMER_0,
  TIMER_1,
  TIMER_2,
  TIMER_3,
  TIMER_4,
#endif // __FIOT_NRF52832_

#ifdef __FIOT_STM32F407_
	TIMER_1 = 0,
	TIMER_2,
	TIMER_3,
	TIMER_4,
	TIMER_5,
	TIMER_6,
	TIMER_7,
	TIMER_8,
	TIMER_9,
	TIMER_10,
	TIMER_11,
	TIMER_12,
	TIMER_13,
	TIMER_14,
#endif // __FIOT_STM32F407_

#ifdef __FIOT_STM32F103_
	TIMER_1 = 0,
	TIMER_2,
	TIMER_3,
	TIMER_4,
	TIMER_5,
	TIMER_6,
	TIMER_7,
	TIMER_8,
#endif //__FIOT_STM32F103_

#ifdef __FIOT_STM32F411_
  //! NOT supported yet
#endif // __FIOT_STM32F411_

#ifdef __FIOT_SAM3X8E_
  //! NOT supported yet
#endif // __FIOT_SAM3X8E_

#ifdef __FIOT_ATMEGA328P_
	//! NOT supported yet
#endif // __FIOT_ATMEGA328P_

#ifdef __FIOT_ATMEGA328PB_
	//! NOT supported yet
#endif // __FIOT_ATMEGA328PB_

	TIMER_TOTAL
}
HAL_Timer_Module_T;
//! Period of Timer.
typedef uint32_t	HAL_Timer_Period_T;
//! Frequency of system.
typedef uint32_t	HAL_Timer_Freq_T;
//! Callback of Timer
typedef void (*HAL_Timer_Callback_T)(void);
//! Timer settings
typedef struct
{
  HAL_Timer_Module_T	Module;
  HAL_Timer_Freq_T		SysFreq;
  HAL_Timer_Period_T	Period;
}
HAL_Timer_Setting_T;
//!Struct of timer to manage information of timer
typedef struct
{
	HAL_Timer_Setting_T   Settings;
	HAL_Timer_Callback_T  CallbackFxn;
}
HAL_Timer_T;
/*******************************************************************************
 * FUNCTIONS - API
 ******************************************************************************/
/*!
 *******************************************************************************
 * @fn      HAL_Timer_Status_T FIOT_HAL_Timer_Init(HAL_Timer_T *iTimer,
 *                                      		 HAL_Timer_Setting_T iSettings,
 *																			     HAL_Timer_Callback_T iCallbackFxn);
 *
 * @brief   Init timer and set period of timer.
 *
 * @param   iTimer - Pointer to manage information of timer
 * @param   iSettings -	 Timer settings
 * @param   iCallbackFxn - Callback function for timer interrupt
 *
 * @return  TIMER_STATUS_OK
 * @return	TIMER_STATUS_ERROR_OPTION
 ******************************************************************************/
HAL_Timer_Status_T FIOT_HAL_Timer_Init(HAL_Timer_T *iTimer,
                                       HAL_Timer_Setting_T iSettings,
																			 HAL_Timer_Callback_T iCallbackFxn);
/*!
 *******************************************************************************
 * @fn      FIOT_HAL_Timer_DeInit(HAL_Timer_T *iTimer)
 *
 * @brief   De-Init timer module
 *
 * @param   iTimer - Pointer to manage information of timer
 *
 * @return  TIMER_STATUS_OK
 * @return	TIMER_STATUS_ERROR_OPTION
 ******************************************************************************/
HAL_Timer_Status_T FIOT_HAL_Timer_DeInit(HAL_Timer_T *iTimer);
/*!
 *******************************************************************************
 * @fn      FIOT_HAL_Timer_Start(HAL_Timer_T *iTimer)
 *
 * @brief   Start Timer
 *
 * @param   iTimer - Pointer to manage information of timer
 *
 * @return  TIMER_STATUS_OK
 *					TIMER_STATUS_ERROR_OPTION
 ******************************************************************************/
HAL_Timer_Status_T FIOT_HAL_Timer_Start(HAL_Timer_T *iTimer);
/*!
 *******************************************************************************
 * @fn      FIOT_HAL_Timer_Stop(HAL_Timer_T *iTimer)
 *
 * @brief   Stop Timer
 *
 * @param   iTimer - Pointer to manage information of timer
 *
 * @return  TIMER_STATUS_OK
 *					TIMER_STATUS_ERROR_OPTION
 ******************************************************************************/
HAL_Timer_Status_T FIOT_HAL_Timer_Stop(HAL_Timer_T *iTimer);
/*!
 *******************************************************************************
 * @fn      FIOT_HAL_Timer_Clear(HAL_Timer_T *iTimer)
 *
 * @brief   Clear timer Counter
 *
 * @param   iTimer - Pointer to manage information of timer
 *
 * @return  TIMER_STATUS_OK
 *					TIMER_STATUS_ERROR_OPTION
 ******************************************************************************/
HAL_Timer_Status_T FIOT_HAL_Timer_Clear(HAL_Timer_T *iTimer);
/*!
 *******************************************************************************
 * @fn      FIOT_HAL_Timer_SetPeriod(HAL_Timer_T *iTimer, 
 * 																	 HAL_Timer_Period_T iTimer_Period);
 *
 * @brief   Set timer period
 *
 * @param   iTimer - Pointer to manage information of timer
 * @param		iTimer_Period - Timer period in miliseconds
 *
 * @return  TIMER_STATUS_OK
 *					TIMER_STATUS_ERROR_OPTION
 ******************************************************************************/
HAL_Timer_Status_T FIOT_HAL_Timer_SetPeriod(HAL_Timer_T *iTimer, 
																						HAL_Timer_Period_T iPeriod);
/******************************************************************************/
#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* __HAL_TIMER_H__ */
