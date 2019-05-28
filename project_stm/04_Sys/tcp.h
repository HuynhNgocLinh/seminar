/*!
 * @file			tcp.h	
 * @brief			
 * 
 * @author  	1.0 - Linh Huynh
 *
 * @copyright
 * @version 	1.0
 * @date    	2019-05-25
 * @example		
 */
#ifndef __TCP_H__
#define __TCP_H__

#ifdef __cplusplus
extern "C" {
#endif
/********************************************************************************
 *	INCLUDES
 ********************************************************************************/
#include "stdint.h"

/********************************************************************************
 * CONSTANTS
 ********************************************************************************/

/********************************************************************************
 * TYPEDEF
 ********************************************************************************/
typedef enum 
{
  TCP_STATUS_OK,
  TCP_STATUS_ERR
} TCP_Status_T;

// typedef enum 
// {
//   TCP_CONNECTION_STATUS_OK,
//   TCP_CONNECTION_STATUS_ERR
// } TCP_Connection_Status_T;

typedef char TCP_IP_Addr_T;
typedef char TCP_Port;
typedef char TCP_Data_T;
typedef uint16_t TCP_Len_T;

typedef struct 
{
  TCP_Status_T Status;
  TCP_IP_Addr_T IP_Addr[15];
  TCP_Port Port[4];
} TCP_Connection_Status_T;
/********************************************************************************
 * FUNCTIONS - API
 ********************************************************************************/
void TCP_Init(void);
/*!
 *******************************************************************************
 * @fn      TCP_Check_Connection_Status(void)
 *
 * @brief   
 *
 * @param   
 *
 * @return  
 ********************************************************************************/
TCP_Status_T TCP_Check_Connection_Status(TCP_Connection_Status_T *Status);

/*!
 *******************************************************************************
 * @fn      TCP_Establish_Connection(TCP_IP_Addr_T Addr, TCP_Port Port)
 *
 * @brief   
 *
 * @param   
 *
 * @return  
 ********************************************************************************/
TCP_Status_T TCP_Establish_Connection(TCP_IP_Addr_T *Addr, TCP_Port *Port);

/*!
 *******************************************************************************
 * @fn      TCP_Send_Data(TCP_Data_T *Send_Data, TCP_Len_T Len)
 *
 * @brief   
 *
 * @param   
 *
 * @return  
 ********************************************************************************/
TCP_Status_T TCP_Send_Data(TCP_Data_T *Send_Data, TCP_Len_T Len);

/*!
 *******************************************************************************
 * @fn      TCP_Receive_Data(TCP_Data_T *Receive_Data)
 *
 * @brief   
 *
 * @param   
 *
 * @return  
 ********************************************************************************/
TCP_Status_T TCP_Receive_Data(TCP_Data_T *Receive_Data);

/*!
 *******************************************************************************
 * @fn      TCP_Receive_Data(TCP_Data_T *Receive_Data)
 *
 * @brief   
 *
 * @param   
 *
 * @return  
 ********************************************************************************/
TCP_Status_T TCP_Receive_Data(TCP_Data_T *Receive_Data);

/*!
 *******************************************************************************
 * @fn      TCP_Close_Connection(void)
 *
 * @brief   
 *
 * @param   
 *
 * @return  
 ********************************************************************************/
TCP_Status_T TCP_Close_Connection(void);

#ifdef __cplusplus
}
#endif
#endif // __CBUFFER_H
