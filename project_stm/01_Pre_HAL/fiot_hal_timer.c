/*!
 * @file       fiot_hal_timer.h
 * @author  	 Binh Thai
 * @copyright  Fiot Co.,Ltd
 * @version    1.0.1.20180418
 * @date       Apr 18, 2018
 * @brief      FIOT HAL driver of Timer.
 *             See example_hal_timer.c to know how to use HAL Timer APIs
 * @example		 example_hal_timer.c
 * 						 This is an example of how to use HAL Timer APIs.
 * 						 More details about this example.
 */
/*******************************************************************************
 *	INCLUDES
 ******************************************************************************/
#include "fiot_hal_timer.h"

#include "stm32f1xx_hal.h"
/*******************************************************************************
 * TYPEDEF
 ******************************************************************************/
typedef uint32_t HAL_Timer_Freq_T; 
typedef struct {
	HAL_Timer_Freq_T SysFreq;
  HAL_Timer_Period_T Period;
  HAL_Timer_Freq_T Prescaler;
}
HAL_Timming_T;

typedef enum
{
	TIMER_STATE_RUN = 0x00,
	TIMER_STATE_STOP
}
HAL_Timer_State_T;
/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/
#define TIM1_IRQn  							(TIM1_UP_IRQn)
#define TIM8_IRQn	 							(TIM8_UP_IRQn)

#define TIM_DEFAULT_PRESCALER   (1)
#define FREQ_PER_TICK_4000 			(4000)
#define FREQ_PER_TICK_2600 			(2600)
#define PERIOD_15S							(15000)

#define TIM_BASE_INIT {																\
  .CounterMode = TIM_COUNTERMODE_UP, 			  					\
  .ClockDivision = TIM_CLOCKDIVISION_DIV1,  					\
  .RepetitionCounter = 0,									  					\
  .Prescaler = TIM_DEFAULT_PRESCALER,			  					\
	.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE	\
}
/*******************************************************************************
 * MACRO
 ******************************************************************************/
#define __CONVERT_PERIOD_TO_TICK(_TICK_FREQ_,_PERIOD_)	\
																					((_TICK_FREQ_)*(_PERIOD_)/(1000) - 1)

#define __TIM_BASE_MSP_INIT(_TIM_)  									 \
do { 																									 \
	__HAL_RCC_##_TIM_##_CLK_ENABLE();                    \
	HAL_NVIC_SetPriority( _TIM_##_IRQn ,         				 \
												IRQ_PRIORITY_##_TIM_,  				 \
												IRQ_PRIORITY_##_TIM_);		 		 \
	HAL_NVIC_EnableIRQ(_TIM_##_IRQn);										 \
} while(0)

#define __TIM_BASE_MSP_DEINIT(_TIM_)  \
do {																  \
	__HAL_RCC_##_TIM_##_CLK_DISABLE();  \
	HAL_NVIC_DisableIRQ(_TIM_##_IRQn);  \
} while(0)

#define __TIM_PROCESS_INT(_TIM_)  																		\
do {																																	\
	if (__HAL_TIM_GET_FLAG(&TIM_Handle[_TIM_], TIM_FLAG_UPDATE) == SET) \
	{                                             									  	\
		__HAL_TIM_CLEAR_FLAG(&TIM_Handle[_TIM_], TIM_FLAG_UPDATE); 				\
		if (Timer_CallbackTable[_TIM_] != NULL)     											\
		{                                             										\
			Timer_CallbackTable[_TIM_]();             											\
		}                                             										\
	}                                               										\
} while(0)                                                        
/*******************************************************************************
 * VARIABLES
 ******************************************************************************/
static TIM_HandleTypeDef TIM_Handle[TIMER_TOTAL] = \
{
	{
		.Instance = TIM1,
		.Init = TIM_BASE_INIT
	},
	{
		.Instance = TIM2,
		.Init = TIM_BASE_INIT
	},
	{
		.Instance = TIM3,
		.Init = TIM_BASE_INIT
	},
	{
		.Instance = TIM4,
		.Init = TIM_BASE_INIT
	},
	{
		.Instance = TIM5,
		.Init = TIM_BASE_INIT
	},
	{
		.Instance = TIM6,
		.Init = TIM_BASE_INIT
	},
	{
		.Instance = TIM7,
		.Init = TIM_BASE_INIT
	},
	{
		.Instance = TIM8,
		.Init = TIM_BASE_INIT
	}
};

static HAL_Timer_State_T TIM_ModuleState[TIMER_TOTAL] = \
{
	TIMER_STATE_STOP, TIMER_STATE_STOP, TIMER_STATE_STOP, TIMER_STATE_STOP,
	TIMER_STATE_STOP, TIMER_STATE_STOP, TIMER_STATE_STOP, TIMER_STATE_STOP,
};

static HAL_Timer_Callback_T Timer_CallbackTable[TIMER_TOTAL];
/*******************************************************************************
 * IRQ - HANDLERS
 ******************************************************************************/
/**
* @brief This function handles TIM global interrupt.
*/
void TIM1_UP_IRQHandler(void)
{
	if (Timer_CallbackTable[TIMER_1] != NULL)
	{                                    
		__TIM_PROCESS_INT(TIMER_1);        
	}    
}

/**
* @brief This function handles TIM2 global interrupt.
*/
void TIM2_IRQHandler(void)
{
	if (Timer_CallbackTable[TIMER_2] != NULL)
	{ 
		__TIM_PROCESS_INT(TIMER_2);  
	}   
}

/**
* @brief This function handles TIM3 global interrupt.
*/
void TIM3_IRQHandler(void)
{
	if (Timer_CallbackTable[TIMER_3] != NULL)
	{                                    
		__TIM_PROCESS_INT(TIMER_3);        
	}   
}

/**
* @brief This function handles TIM4 global interrupt.
*/
void TIM4_IRQHandler(void)
{
	if (Timer_CallbackTable[TIMER_4] != NULL)
	{                                    
		__TIM_PROCESS_INT(TIMER_4);       
	}   
}

/**
* @brief This function handles TIM5 global interrupt.
*/
void TIM5_IRQHandler(void)
{
	if (Timer_CallbackTable[TIMER_5] != NULL)
	{                                    
		__TIM_PROCESS_INT(TIMER_5);     
	}   
}

/**
* @brief This function handles TIM6 global interrupt.
*/
void TIM6_IRQHandler(void)
{
	if (Timer_CallbackTable[TIMER_6] != NULL)
	{                                    
		__TIM_PROCESS_INT(TIMER_6);     
	}   
}

/**
* @brief This function handles TIM7 global interrupt.
*/
void TIM7_IRQHandler(void)
{
	if (Timer_CallbackTable[TIMER_7] != NULL)
	{                                    
		__TIM_PROCESS_INT(TIMER_7);       
	}   
}

/**
* @brief This function handles TIM8 update interrupt.
*/
void TIM8_UP_IRQHandler(void)
{
	if (Timer_CallbackTable[TIMER_8] != NULL)
	{                                    
		__TIM_PROCESS_INT(TIMER_8);      
	}   
}
/*******************************************************************************
 * TIM1_TRG_COM_TIM11_IRQn
 ******************************************************************************/
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htimerBase)
{
	if (htimerBase->Instance == TIM1)   { __TIM_BASE_MSP_INIT(TIM1);  }
	if (htimerBase->Instance == TIM2)   { __TIM_BASE_MSP_INIT(TIM2);  }
	if (htimerBase->Instance == TIM3)   { __TIM_BASE_MSP_INIT(TIM3);  }
	if (htimerBase->Instance == TIM4)   { __TIM_BASE_MSP_INIT(TIM4);  }
	if (htimerBase->Instance == TIM5)   { __TIM_BASE_MSP_INIT(TIM5);  }
	if (htimerBase->Instance == TIM6)   { __TIM_BASE_MSP_INIT(TIM6);  }
	if (htimerBase->Instance == TIM7)   { __TIM_BASE_MSP_INIT(TIM7);  }
	if (htimerBase->Instance == TIM8)   { __TIM_BASE_MSP_INIT(TIM8);  }
}
/******************************************************************************/
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *htimerBase)
{
	if (htimerBase->Instance == TIM1)   { __TIM_BASE_MSP_DEINIT(TIM1);  }
	if (htimerBase->Instance == TIM2)   { __TIM_BASE_MSP_DEINIT(TIM2);  }
	if (htimerBase->Instance == TIM3)   { __TIM_BASE_MSP_DEINIT(TIM3);  } 
	if (htimerBase->Instance == TIM4)   { __TIM_BASE_MSP_DEINIT(TIM4);  }
	if (htimerBase->Instance == TIM5)   { __TIM_BASE_MSP_DEINIT(TIM5);  }
	if (htimerBase->Instance == TIM6)   { __TIM_BASE_MSP_DEINIT(TIM6);  }
	if (htimerBase->Instance == TIM7)   { __TIM_BASE_MSP_DEINIT(TIM7);  }
	if (htimerBase->Instance == TIM8)   { __TIM_BASE_MSP_DEINIT(TIM8);  }
}
/*******************************************************************************
 * FUNCTIONS - LOCAL
 ******************************************************************************/
static HAL_Timer_Freq_T Timer_GetClkSrc(HAL_Timer_T *iTimer);
static HAL_Timer_Status_T TIM_ConvertTimming(HAL_Timer_T *iTimer, 
																						 TIM_HandleTypeDef *iTimer_Handle);
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
 * @return	TIMER_STATUS_ERROR_NULL_PTR
 ******************************************************************************/
HAL_Timer_Status_T FIOT_HAL_Timer_Init(HAL_Timer_T *iTimer,
                                       HAL_Timer_Setting_T iSettings,
																			 HAL_Timer_Callback_T iCallbackFxn)
{
	// Verify variable
  __ASSERT((iTimer != NULL), TIMER_STATUS_ERROR_NULL_PTR);
	__ASSERT((iCallbackFxn != NULL), TIMER_STATUS_ERROR_NULL_PTR);
	
	__ASSERT((iSettings.Period > 0), TIMER_STATUS_ERROR_OPTION);
	__ASSERT((iSettings.Module <= TIMER_TOTAL), TIMER_STATUS_ERROR_OPTION);
	
  iTimer->Settings = iSettings;
  iTimer->CallbackFxn = iCallbackFxn;

  Timer_CallbackTable[iTimer->Settings.Module] = iTimer->CallbackFxn;

  TIM_ClockConfigTypeDef sClockSourceConfig = \
	{
		.ClockSource = TIM_CLOCKSOURCE_INTERNAL,
	};

	TIM_MasterConfigTypeDef sMasterConfig = \
	{
		.MasterOutputTrigger = TIM_TRGO_RESET,
		.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE
	};
	
	TIM_ConvertTimming(iTimer, &TIM_Handle[iTimer->Settings.Module]);
  HAL_StatusTypeDef errStatus;
	errStatus = HAL_TIM_Base_Init(&TIM_Handle[iTimer->Settings.Module]);
	__ASSERT((errStatus == HAL_OK), TIMER_STATUS_ERROR_OPTION);

  errStatus = HAL_TIM_ConfigClockSource(&TIM_Handle[iTimer->Settings.Module], 
                                        &sClockSourceConfig);
	__ASSERT((errStatus == HAL_OK),TIMER_STATUS_ERROR_OPTION);

  errStatus = 
		HAL_TIMEx_MasterConfigSynchronization(&TIM_Handle[iTimer->Settings.Module], 
                                          &sMasterConfig);
	__ASSERT((errStatus == HAL_OK),TIMER_STATUS_ERROR_OPTION);

  return TIMER_STATUS_OK;
}                                       
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
 * @return	TIMER_STATUS_ERROR_NULL_PTR
 ******************************************************************************/
HAL_Timer_Status_T FIOT_HAL_Timer_DeInit(HAL_Timer_T *iTimer)
{
	// Check: Have Module been initialized yet? 
	__ASSERT((TIM_Handle[iTimer->Settings.Module].State != HAL_TIM_STATE_RESET), 
						TIMER_STATUS_ERROR_OPTION);
	
	__ASSERT((iTimer != NULL), TIMER_STATUS_ERROR_NULL_PTR);

	__ASSERT((HAL_TIM_Base_DeInit(&TIM_Handle[iTimer->Settings.Module]) == HAL_OK),
           TIMER_STATUS_ERROR_OPTION);

		return TIMER_STATUS_OK;
}
/*!
 *******************************************************************************
 * @fn      FIOT_HAL_Timer_Start(HAL_Timer_T *iTimer)
 *
 * @brief   Start Timer
 *
 * @param   iTimer - Pointer to manage information of timer
 *
 * @return  TIMER_STATUS_OK
 * @return  TIMER_STATUS_ERROR_OPTION
 * @return  TIMER_STATUS_ERROR_NULL_PTR
 ******************************************************************************/
HAL_Timer_Status_T FIOT_HAL_Timer_Start(HAL_Timer_T *iTimer)
{
  __ASSERT((iTimer != NULL), TIMER_STATUS_ERROR_NULL_PTR); 
		// Check: Have Module been initialized yet? 
	__ASSERT((TIM_Handle[iTimer->Settings.Module].State != HAL_TIM_STATE_RESET), 
						TIMER_STATUS_ERROR_OPTION);
	// Check Timer's state
	__ASSERT((TIM_ModuleState[iTimer->Settings.Module] == TIMER_STATE_STOP), 
						TIMER_STATUS_ERROR_OPTION);
	// Set Timer state run.
	TIM_ModuleState[iTimer->Settings.Module] = TIMER_STATE_RUN;

	__ASSERT((HAL_TIM_Base_Start_IT(&TIM_Handle[iTimer->Settings.Module]) == HAL_OK), 
            TIMER_STATUS_ERROR_OPTION);

	return TIMER_STATUS_OK;
}
/*!
 *******************************************************************************
 * @fn      FIOT_HAL_Timer_Stop(HAL_Timer_T *iTimer)
 *
 * @brief   Stop Timer
 *
 * @param   iTimer - Pointer to manage information of timer
 *
 * @return  TIMER_STATUS_OK
 * @return  TIMER_STATUS_ERROR_OPTION
 * @return  TIMER_STATUS_ERROR_NULL_PTR
 ******************************************************************************/
HAL_Timer_Status_T FIOT_HAL_Timer_Stop(HAL_Timer_T *iTimer)
{
  __ASSERT((iTimer != NULL), TIMER_STATUS_ERROR_NULL_PTR);
		// Check: Have Module been initialized yet? 
	__ASSERT((TIM_Handle[iTimer->Settings.Module].State != HAL_TIM_STATE_RESET), 
						TIMER_STATUS_ERROR_OPTION);  
	// Check Timer's state
	__ASSERT((TIM_ModuleState[iTimer->Settings.Module] == TIMER_STATE_RUN), 
						TIMER_STATUS_ERROR_OPTION);
	// Set Timer state run.
	TIM_ModuleState[iTimer->Settings.Module] = TIMER_STATE_STOP;			

	__ASSERT((HAL_TIM_Base_Stop_IT(&TIM_Handle[iTimer->Settings.Module]) == HAL_OK), 
            TIMER_STATUS_ERROR_OPTION);

  return TIMER_STATUS_OK;
}
/*!
 *******************************************************************************
 * @fn      FIOT_HAL_Timer_Clear(HAL_Timer_T *iTimer)
 *
 * @brief   Clear Counter in Timer
 *
 * @param   iTimer - Pointer to manage information of timer
 *
 * @return  TIMER_STATUS_OK
 * @return	TIMER_STATUS_ERROR_OPTION
 * @return	TIMER_STATUS_ERROR_NULL_PTR
 ******************************************************************************/
HAL_Timer_Status_T FIOT_HAL_Timer_Clear(HAL_Timer_T *iTimer)
{
  __ASSERT((iTimer != NULL), TIMER_STATUS_ERROR_NULL_PTR); 
	// Check: Have Module been initialized yet? 
	__ASSERT((TIM_Handle[iTimer->Settings.Module].State != HAL_TIM_STATE_RESET), 
						TIMER_STATUS_ERROR_OPTION);
	__HAL_TIM_SET_COUNTER(&TIM_Handle[iTimer->Settings.Module], 0);

	return TIMER_STATUS_OK;
}
/*!
 *******************************************************************************
 * @fn      HAL_Timer_Status_T TIM_ConvertTimming(HAL_Timer_T *iTimer, 
 *																								HAL_Timming_T *oTim)
 *
* @brief   Convert timming: prescaler, period, timer clock freq
 *
 * @param   iTimer - Pointer to manage information of timer
 * @param   oTim	- Pointer of timming
 *
 * @return  TIMER_STATUS_OK
 ******************************************************************************/
static HAL_Timer_Status_T TIM_ConvertTimming(HAL_Timer_T *iTimer, 
																						 TIM_HandleTypeDef *iTimer_Handle)
{
	__ASSERT((iTimer != NULL), TIMER_STATUS_ERROR_NULL_PTR); 
	//Get system timer clock
  HAL_Timer_Freq_T sysFreq = Timer_GetClkSrc(iTimer);
	HAL_Timer_Period_T tmpPeriod = iTimer->Settings.Period;
	// change frequence per tick
	if(tmpPeriod > PERIOD_15S)
	{
		iTimer_Handle->Init.Prescaler = sysFreq/(FREQ_PER_TICK_2600 + 1);
		iTimer_Handle->Init.Period = __CONVERT_PERIOD_TO_TICK(FREQ_PER_TICK_2600, 
																													tmpPeriod);
	}
	else 
	{
		iTimer_Handle->Init.Prescaler = sysFreq/(FREQ_PER_TICK_4000 + 1);
		iTimer_Handle->Init.Period = __CONVERT_PERIOD_TO_TICK(FREQ_PER_TICK_4000, 
																													tmpPeriod);
	}
	return TIMER_STATUS_OK;
}

static HAL_Timer_Freq_T Timer_GetClkSrc(HAL_Timer_T *iTimer)
{
	switch (iTimer->Settings.Module)
	{
		case TIMER_1: case TIMER_8: 							return HAL_RCC_GetPCLK2Freq();

		case TIMER_2: case TIMER_3: case TIMER_4: 
		case TIMER_5: case TIMER_6: case TIMER_7: return 2*HAL_RCC_GetPCLK1Freq();

		default: return 0;
	}
}
/*!
 *******************************************************************************
 * END
 ******************************************************************************/
