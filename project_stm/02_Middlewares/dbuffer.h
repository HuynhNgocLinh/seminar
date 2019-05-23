/*!
 * @file			dbuffer.h	
 * @brief			
 * 
 * @author  	1.0 - Vy Luu
 *
 * @copyright Fiot Co.,Ltd  
 * @version 	1.0
 * @date    	2018-02-07
 * @example		
 */
#ifndef __DBUFFER_H__
#define __DBUFFER_H__

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
/**
 * @brief Type of Status
 */
typedef enum {
	DBUFFER_OK,
	DBUFFER_ERROR
} DBuffer_Status_T;
/**
 * @brief Type of Data
 */
typedef uint8_t DBuffer_Data_T;
/**
 * @brief Type of Data Size
 */
typedef uint8_t DBuffer_DataSize_T;
/**
 * @brief Type of Idx
 */
typedef uint16_t DBuffer_Idx_T;
/**
 * @brief Type of Size
 */
typedef uint16_t DBuffer_Length_T;
/**
 * @brief Type of DBuffer Handler
 */
typedef struct
{
	DBuffer_Data_T *Data;				 /**<Pointer of DBuffer Buffer*/
	DBuffer_DataSize_T DataSize; /**<Size of a data compare with DBuffer_Data_T*/
	DBuffer_Length_T Size;			 /**<Size of data*/
} DBuffer_T;
/********************************************************************************
 * FUNCTIONS - API
 ********************************************************************************/
/*!
 *******************************************************************************
 * @fn      void DBuffer_Init(DBuffer_T *iDb, DBuffer_Data_T *iBuf, DBuffer_DataSize_T iSize, DBuffer_Length_T iLen)
 *
 * @brief   Initializes the Data Buffer structure with the given buffer and size \n
 *					DBuffer Buffer capacity = iSize*iDataSize
 *
 * @param   iDb  	- DBuffer Pointer
 * @param   iBuf 	- DBuffer Buffer
 * @param   iDataSize	- Size of a data compare with DBuffer_Data_T
 * @param   iSize	- Size of Data
 *
 * @return  DBUFFER_OK
 * @return  DBUFFER_ERROR
 ********************************************************************************/
DBuffer_Status_T DBuffer_Init(DBuffer_T *iDb, DBuffer_Data_T *iBuf, DBuffer_DataSize_T iDataSize, DBuffer_Length_T iSize);
/*!
 *******************************************************************************
 * @fn      void DBuffer_Clear(DBuffer_T *iDb,DBuffer_Idx_T idx)
 *
 * @brief   Clear data at idx in DBuffer
 *
 * @param   iDb  - DBuffer Pointer
 * @param   iIdx - idx  
 *
 * @return  DBUFFER_OK
 * @return  DBUFFER_ERROR
 ********************************************************************************/
DBuffer_Status_T DBuffer_Clear(DBuffer_T *iDb, DBuffer_Idx_T iIdx);
/*!
 *******************************************************************************
 * @fn      void DBuffer_Read(DBuffer_T *iDb,DBuffer_Idx_T iIdx, void *iBuf)
 *
 * @brief   Read data at idx in DBuffer
 *
 * @param   iDb  		- DBuffer Pointer
 * @param   iIdx 		- idx
 * @param   iBuf	  - pointer of read buffer
 *
 * @return  DBUFFER_OK
 * @return  DBUFFER_ERROR
 ********************************************************************************/
DBuffer_Status_T DBuffer_Read(DBuffer_T *iDb, DBuffer_Idx_T iIdx, void *iBuf);
/*!
 *******************************************************************************
 * @fn      DBuffer_Length_T DBuffer_Write(DBuffer_T *iDb,DBuffer_Idx_T iIdx, void *oBuf)
 *
 * @brief   Write data at idx in DBuffer
 *
 * @param   iDb  		- DBuffer Pointer
 * @param   iIdx 		- idx
 * @param   oBuf	  - pointer of write buffer
 *
 * @return  DBUFFER_OK
 * @return  DBUFFER_ERROR
 ********************************************************************************/
DBuffer_Status_T DBuffer_Write(DBuffer_T *iDb, DBuffer_Idx_T iIdx, void *oBuf);
#ifdef __cplusplus
}
#endif
#endif // __CBUFFER_H
