/*!
 * @file       fiot_hal_timer.h
 * @author  	 01.00.01 				 - Khang Vo
 * 				  	 01.00.02 				 - Khang Vo
 * 						 01.00.03.20180803 - Bita
 * @copyright  Fiot Co.,Ltd
 * @version    01.00.03.20180803
 * @date       Aug 03, 2018
 * @brief      FIOT HAL driver of Timer.
 *             See example_hal_timer.c to know how to use HAL Timer APIs
 * @note 				- Define in fiot_hal_define.h to choose the MCU
 *                + Define __FIOT_STM32F407_    -> STM32F407
 *                + Define __FIOT_STM32F411_    -> STM32F411
 *                + Define __FIOT_STM32F103_    -> STM32F103
 *                + Define __FIOT_STM32L151_    -> STM32L151
 *                + Define __FIOT_SAM3X8E_      -> SAM3X8E
 *                + Define __FIOT_ATMEGA328PB_  -> ATMEGA328PB
 *                + Define __FIOT_ATMEGA328P_   -> ATMEGA328P
 *                + Define __FIOT_CC2650_       -> CC2650
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
 * CONFIG
 ******************************************************************************/
#if defined(__FIOT_STM32F407_)

	#define IRQ_PRIORITY_TIM1 		(2)
	#define IRQ_PRIORITY_TIM2 		(2)
	#define IRQ_PRIORITY_TIM3 		(2)
	#define IRQ_PRIORITY_TIM4 		(2)
	#define IRQ_PRIORITY_TIM5 		(2)
	#define IRQ_PRIORITY_TIM6 		(2)
	#define IRQ_PRIORITY_TIM7 		(2)
	#define IRQ_PRIORITY_TIM8 		(2)
	#define IRQ_PRIORITY_TIM9 		(2)
	#define IRQ_PRIORITY_TIM10 		(2)
	#define IRQ_PRIORITY_TIM11		(2)
	#define IRQ_PRIORITY_TIM12		(2)
	#define IRQ_PRIORITY_TIM13		(2)
	#define IRQ_PRIORITY_TIM14		(2)

#elif defined(__FIOT_STM32F411_)

	#define IRQ_PRIORITY_TIM1 		(2)
	#define IRQ_PRIORITY_TIM2 		(2)
	#define IRQ_PRIORITY_TIM3 		(2)
	#define IRQ_PRIORITY_TIM4 		(2)
	#define IRQ_PRIORITY_TIM5 		(2)
	#define IRQ_PRIORITY_TIM9 		(2)
	#define IRQ_PRIORITY_TIM10 		(2)
	#define IRQ_PRIORITY_TIM11		(2)

#elif defined(__FIOT_STM32F103_)

	#define IRQ_PRIORITY_TIM1 		(2)
	#define IRQ_PRIORITY_TIM2 		(2)
	#define IRQ_PRIORITY_TIM3 		(2)
	#define IRQ_PRIORITY_TIM4 		(2)
	#define IRQ_PRIORITY_TIM5 		(2)
	#define IRQ_PRIORITY_TIM6 		(2)
	#define IRQ_PRIORITY_TIM7 		(2)
	#define IRQ_PRIORITY_TIM8 		(2)

#elif defined(__FIOT_STM32L151_)

	#define IRQ_PRIORITY_TIM2 		(2)
	#define IRQ_PRIORITY_TIM3 		(2)
	#define IRQ_PRIORITY_TIM4 		(2)
	#define IRQ_PRIORITY_TIM5 		(2)
	#define IRQ_PRIORITY_TIM6 		(2)
	#define IRQ_PRIORITY_TIM7 		(2)
	#define IRQ_PRIORITY_TIM9 		(2)
	#define IRQ_PRIORITY_TIM10 		(2)
	#define IRQ_PRIORITY_TIM11		(2)

#elif defined(__FIOT_ATMEGA328P_)

#elif defined(__FIOT_ATMEGA328PB_)

#elif defined(__FIOT_CC2650_)

#elif defined(__FIOT_NRF52832__)

#elif defined(__FIOT_SAM3X8E_)

#endif
/*******************************************************************************
 * TYPEDEF
 ******************************************************************************/
/**
 * @brief Option of TIMER's status
 */
typedef enum {
	TIMER_STATUS_OK,           
  TIMER_STATUS_ERROR_OPTION, 
  TIMER_STATUS_ERROR_NULL_PTR
} HAL_Timer_Status_T;
/**
 * @brief Option of TIMER module
 */
typedef enum
{
#if defined(__FIOT_STM32F407_)
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
#elif defined(__FIOT_STM32F411_)
	TIMER_1 = 0,
	TIMER_2,
	TIMER_3,
	TIMER_4,
	TIMER_5,
	TIMER_9,
	TIMER_10,
	TIMER_11,
#elif defined(__FIOT_STM32F103_)
	TIMER_1 = 0,
	TIMER_2,
	TIMER_3,
	TIMER_4,
	TIMER_5,
	TIMER_6,
	TIMER_7,
	TIMER_8,	
#elif defined(__FIOT_STM32L151_)
	TIMER_2 = 0,
	TIMER_3,
	TIMER_4,
	TIMER_5,
	TIMER_6,
	TIMER_7,
	TIMER_9,
	TIMER_10,
	TIMER_11,
#elif defined(__FIOT_CC2650_)
	TIMER_0A,
	TIMER_0B,
	TIMER_1A,
	TIMER_1B,
	TIMER_2A,
	TIMER_2B,
	TIMER_3A,
	TIMER_3B,
#elif defined(__FIOT_NRF52832_)
  TIMER_0,
  TIMER_1,
  TIMER_2,
  TIMER_3,
  TIMER_4,
#elif defined(__FIOT_SAM3X8E_)
#elif defined(__FIOT_ATMEGA328P_)
#elif defined(__FIOT_ATMEGA328PB_)
#endif // defined(__FIOT_CC2650_)

	TIMER_TOTAL
} HAL_Timer_Module_T;
/**
 * @brief Type of TIMER's period
 */
typedef uint32_t	HAL_Timer_Period_T;
/**
 * @brief Type of system TIMER frequence
 */
typedef uint32_t	HAL_Timer_Freq_T;
/**
 * @brief Type of  TIMER's callback function
 */
typedef void (*HAL_Timer_Callback_T)(void);
/**
 * @brief Type of  TIMER's settings
 */
typedef struct {
  HAL_Timer_Module_T	Module;
  HAL_Timer_Freq_T		SysFreq;
  HAL_Timer_Period_T	Period;
} HAL_Timer_Setting_T;
/**
 * @brief Type of  TIMER's settings
 */
typedef struct {
	HAL_Timer_Setting_T   Settings;
	HAL_Timer_Callback_T  CallbackFxn;
} HAL_Timer_T;
/*******************************************************************************
 * FUNCTIONS - API
 ******************************************************************************/
/*!
 *******************************************************************************
 * @fn      HAL_Timer_Status_T FIOT_HAL_Timer_Init(HAL_Timer_T *iTimer,
 *                                      					 HAL_Timer_Setting_T iSettings,
 *																			           HAL_Timer_Callback_T iCallbackFxn);
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
 * @fn      FIOT_HAL_Timer_SetPeriod(HAL_Timer_T *iTimer, HAL_Timer_Period_T iTimer_Period);
 *
 * @brief   Set timer period
 *
 * @param   iTimer - Pointer to manage information of timer
 * @param		iTimer_Period - Timer period in miliseconds
 *
 * @return  TIMER_STATUS_OK
 *					TIMER_STATUS_ERROR_OPTION
 ******************************************************************************/
HAL_Timer_Status_T FIOT_HAL_Timer_SetPeriod(HAL_Timer_T *iTimer, HAL_Timer_Period_T iPeriod);
/******************************************************************************/
#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* __HAL_TIMER_H__ */
