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
/********************************************************************************
 *	INCLUDES
 ********************************************************************************/
#include "dbuffer.h"
#include "string.h"
/********************************************************************************
 * CONSTANTS
 ********************************************************************************/

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
DBuffer_Status_T DBuffer_Init(DBuffer_T *iDb, DBuffer_Data_T *iBuf, DBuffer_DataSize_T iDataSize, DBuffer_Length_T iSize)
{
	if(iBuf==NULL || iDb==NULL)
		return DBUFFER_ERROR;
	
	iDb->Data = iBuf;
	iDb->DataSize = iDataSize;
	iDb->Size = iSize;
	
	return DBUFFER_OK;
}
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
DBuffer_Status_T DBuffer_Clear(DBuffer_T *iDb, DBuffer_Idx_T iIdx)
{
	if(iIdx>=iDb->Size || iDb==NULL)
		return DBUFFER_ERROR;
	memset(&iDb->Data[iIdx*iDb->DataSize],0x00,iDb->DataSize);
	return DBUFFER_OK;
}
/*!
 *******************************************************************************
 * @fn      void DBuffer_Write(DBuffer_T *iDb,DBuffer_Idx_T iIdx, void *iBuf)
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
DBuffer_Status_T DBuffer_Write(DBuffer_T *iDb, DBuffer_Idx_T iIdx, void *iBuf)
{
	if(iIdx>=iDb->Size || iDb==NULL)
		return DBUFFER_ERROR;

	memcpy(&iDb->Data[iIdx*iDb->DataSize],iBuf,iDb->DataSize);
	
	return DBUFFER_OK;
}
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
DBuffer_Status_T DBuffer_Read(DBuffer_T *iDb, DBuffer_Idx_T iIdx, void *oBuf)
{
	if(iIdx>=iDb->Size || iDb==NULL)
		return DBUFFER_ERROR;
	
	memcpy(oBuf,&iDb->Data[iIdx*iDb->DataSize],iDb->DataSize);
	
	return DBUFFER_OK;	
}
/********************************************************************************
 * END OF FILE
 ********************************************************************************/
