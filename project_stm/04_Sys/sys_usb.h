/*!
 *  @file       sys_usb.h
 *  @brief     
 *  @author     Dat Hoang
 *  @copyright  Fiot Co.,Ltd
 *  @version    1.0.1
 *  @date       Sep 17, 2018
 */
#ifndef __SYS_USB_H__
#define __SYS_USB_H__

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include "sys_data_manager.h"
#include <stdint.h>
/*******************************************************************************
 * SETTINGS
 ******************************************************************************/
#define __SUSB_EVT_LINE_TYPE SUSB_Evt_T
#define __SUSB_EVT_LINE_DATA_SIZE sizeof(__SUSB_EVT_LINE_TYPE)
#define __SUSB_EVT_LINE_MAX_SIZE (10)

#define __SUSB_CMD_LINE_TYPE SUSB_Cmd_T
#define __SUSB_CMD_LINE_DATA_SIZE sizeof(__SUSB_CMD_LINE_TYPE)
#define __SUSB_CMD_LINE_MAX_SIZE (10)

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/
/*******************************************************************************
 *  TYPEDEFS
 ******************************************************************************/
/**
 * @brief Led Line 
 */
typedef enum
{
  SUSB_LINE_CMD,
  SUSB_LINE_EVT,
  SUSB_LINE_MAX
} SUSB_Line_T;

/*******************************************************************************
 *  USB 
 ******************************************************************************/
typedef struct 
{
  uint16_t Len;
  uint8_t  Data[1000];
} SUSB_Data_T;
/*******************************************************************************
 *  EVT TYPEDEFS
 ******************************************************************************/
/**
 * @brief Type of LED event
 */
typedef enum
{
  SUSB_EVT_TYPE_TCP_CHECK_CONNECTION_COMPLETED  ,                 
  SUSB_EVT_TYPE_TCP_ESTAB_CONNECTION_COMPLETED  ,                 
  SUSB_EVT_TYPE_TCP_SEND_DATA_COMPLETED         ,                 
  SUSB_EVT_TYPE_TCP_REC_DATA_COMPLETED          ,                 
  SUSB_EVT_TYPE_TCP_CLOSE_CONNECTION_COMPLETED  ,                 
  SUSB_EVT_TYPE_MAX
} SUSB_Evt_Type_T;

/**
 * @brief  
 */
typedef SUSB_Data_T SUSB_Evt_Data_T;
/**
 * @brief  
 */
typedef struct
{
  SUSB_Evt_Type_T Type; /*!<Type of Evt */
  SUSB_Evt_Data_T Data; /*!<Type of Info depend on Type*/
} SUSB_Evt_T;
/*******************************************************************************
 *  CMD TYPEDEFS
 ******************************************************************************/
/**
 * @brief 
 */
typedef enum
{
  SUSB_CMD_TYPE_TCP_CHECK_CONNECTION  ,                 
  SUSB_CMD_TYPE_TCP_ESTAB_CONNECTION  ,                 
  SUSB_CMD_TYPE_TCP_SEND_DATA         ,                 
  SUSB_CMD_TYPE_TCP_REC_DATA          ,                 
  SUSB_CMD_TYPE_TCP_CLOSE_CONNECTION  ,                 
  SUSB_CMD_TYPE_MAX  
} SUSB_Cmd_Type_T;

/**
 * @brief  
 */
typedef SUSB_Data_T SUSB_Cmd_Data_T;

/**
 * @brief Information of Cmd
 */
typedef struct
{
  SUSB_Cmd_Type_T Type; /*!<Type of Led */
  SUSB_Cmd_Data_T Data; /*!<Type of Data depend on Type*/
} SUSB_Cmd_T;
/*******************************************************************************
 * FUNCTIONS - API
 ******************************************************************************/
/*!
 *******************************************************************************
 * @fn      void SUSB_Init(void)
 *
 * @brief  
 *          
 * @param   None
 *
 * @return  None
 ******************************************************************************/
void SUSB_Init(void);
/*!
 *******************************************************************************
 * @fn      DM_LINE_Idx_T SUSB_SubscribeLINE(SUSB_Line_T iLine)
 *
 * @brief   Subscribe a LINE on sys_led
 *          
 * @param   iLine - Type of LINE need to subscribe
 *
 * @return  Return LINE Index
 ******************************************************************************/
DM_LINE_Idx_T SUSB_SubscribeLINE(SUSB_Line_T iLine);

/*!
 *******************************************************************************
 * @fn      void SUSB_Cmd_Loop(void)
 *
 * @brief   Process of Cmd. This function need to call to check cmd
 *          
 * @param   None
 *
 * @return  None
 ******************************************************************************/
void SUSB_Cmd_Loop(void);
/******************************************************************************/

#endif /* __SYS_USB_H__ */
