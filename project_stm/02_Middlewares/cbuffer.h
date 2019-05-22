/*!
 * @file			cbuffer.h	
 * @brief			This Circular Buffer is safe to use in IRQ with single Reader, 
 *						single Writer. No need to disable any IRQ. \n						
 *						See example_cbuffer.c to know how to use the cbuffer
 * 
 * @author  	1.0 - Triet Luu \n
 *						1.1 - Vy Luu 
 * @copyright Fiot Co.,Ltd  
 * @version 	1.1
 * @date    	2016-11-14
 * @example		example_cbuffer.c
 * 						This is an example of how to use the cbuffer api.
 * 						More details about this example.
 */
#ifndef __CBUFFER_H__
#define __CBUFFER_H__

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
typedef uint8_t CBuffer_Data_T;
typedef uint16_t CBuffer_Length_T;

typedef struct
{
	CBuffer_Data_T *Data;
	CBuffer_Length_T Size;
	CBuffer_Length_T Writer;
	CBuffer_Length_T Reader;
	CBuffer_Length_T Overflow;
} CBuffer_T;
/********************************************************************************
 * FUNCTIONS - API
 ********************************************************************************/
/*!
 *******************************************************************************
 * @fn      void CBuffer_Init(CBuffer_T *cb, void *buf, CBuffer_Length_T size)
 *
 * @brief   Initializes the CB structure with the given buffer and size \n
 *					CBuffer capacity = size - 1
 *
 * @param   cb  	- CBuffer Pointer
 * @param   buf 	- CBuffer Buffer
 * @param   size	- Size of CBuffer Buffer
 *
 * @return  None
 ********************************************************************************/
void CBuffer_Init(CBuffer_T *cb, void *buf, CBuffer_Length_T size);
/*!
 *******************************************************************************
 * @fn      void CBuffer_Clear(CBuffer_T *cb)
 *
 * @brief   Reset the CB structure
 *
 * @param   cb  - CBuffer Pointer
 *
 * @return  None
 ********************************************************************************/
void CBuffer_Clear(CBuffer_T *cb);
/*!
 *******************************************************************************
 * @fn      CBuffer_Length_T CBuffer_Read(CBuffer_T *cb, void *buf, CBuffer_Length_T nBytes)
 *
 * @brief   Read upto nBytes from the CB. Actual number of read bytes is returned
 *
 * @param   cb  		- CBuffer Pointer
 * @param   buf			- pointer of read buffer
 * @param   nBytes	- Number of bytes want to read
 *
 * @return  Number of read bytes
 ********************************************************************************/
CBuffer_Length_T CBuffer_Read(CBuffer_T *cb, void *buf, CBuffer_Length_T nBytes);
/*!
 *******************************************************************************
 * @fn      CBuffer_Length_T CBuffer_Write(CBuffer_T *cb, const void *buf, CBuffer_Length_T nBytes)
 *
 * @brief   Write upto nBytes to the CB. Actual number of written byte is returned.
 *
 * @param   cb  		- CBuffer Pointer
 * @param   buf			- pointer of write buffer
 * @param   nBytes	- Number of bytes want to write
 *
 * @return  Number of write bytes
 ********************************************************************************/
CBuffer_Length_T CBuffer_Write(CBuffer_T *cb, const void *buf, CBuffer_Length_T nBytes);
/*!
 *******************************************************************************
 * @fn      CBuffer_Length_T CBuffer_DataCount(CBuffer_T *cb)
 *
 * @brief   Return number of unread bytes in the CB.
 *
 * @param   cb  - CBuffer Pointer
 *
 * @return  unread bytes in the CB.
 ********************************************************************************/
CBuffer_Length_T CBuffer_DataCount(CBuffer_T *cb);
/*!
 *******************************************************************************
 * @fn      CBuffer_Length_T CBuffer_SizeCount(CBuffer_T *cb)
 *
 * @brief   Return number of free bytes in the CB.
 *
 * @param   cb  - CBuffer Pointer
 *
 * @return  free bytes in the CB.
 ********************************************************************************/
CBuffer_Length_T CBuffer_SizeCount(CBuffer_T *cb);

#endif // __CBUFFER_H
