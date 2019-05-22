/*!
 * @file			cbuffer.h	
 * @author  	1.0 - Triet Luu - Fiot Co.,Ltd
 *						1.1 - Vy Luu - Fiot Co.,Ltd
 * @version 	1.1
 * @date    	2016-11-14
 * @brief			This Circular Buffer is safe to use in IRQ with single Reader, 
 *						single Writer. No need to disable any IRQ. \n						
 *						See example_cbuffer.c to know how to use the cbuffer 
 * @example		example_cbuffer.c
 * 						This is an example of how to use the cbuffer api.
 * 						More details about this example.
 */
/********************************************************************************
 *	INCLUDES
 ********************************************************************************/
#include "cbuffer.h"

/********************************************************************************
 * CONSTANTS
 ********************************************************************************/

/********************************************************************************
 * FUNCTIONS - API
 ********************************************************************************/
/********************************************************************************
 * @fn      CBuffer_Init
 *
 * @brief   Initializes the CB structure with the given buffer and size
 *
 * @param   cb  - CBuffer Pointer
 *					buf - CBuffer Buffer
 *					size- Size of CBuffer Buffer
 *
 * @return  None
 ********************************************************************************/
void CBuffer_Init(CBuffer_T *cb, void *buf, CBuffer_Length_T size)
{
	cb->Data = (CBuffer_Data_T *)buf;
	cb->Size = size;
	cb->Reader = 0;
	cb->Writer = 0;
	cb->Overflow = 0;
}
/********************************************************************************
 * @fn      CBuffer_Clear
 *
 * @brief   Reset the CB structure
 *
 * @param   cb  - CBuffer Pointer
 *
 * @return  None
 ********************************************************************************/
void CBuffer_Clear(CBuffer_T *cb)
{
	cb->Reader = 0;
	cb->Writer = 0;
	cb->Overflow = 0;
}
/********************************************************************************
 * @fn      CBuffer_DataCount
 *
 * @brief   Return number of unread bytes in the CB.
 *
 * @param   cb  - CBuffer Pointer
 *
 * @return  unread bytes in the CB.
 ********************************************************************************/
CBuffer_Length_T CBuffer_DataCount(CBuffer_T *cb)
{
	CBuffer_Length_T tmpReader, tmpWriter;
	tmpReader = cb->Reader;
	tmpWriter = cb->Writer;

	if (tmpReader <= tmpWriter)
		return (tmpWriter - tmpReader);
	else
		return (tmpWriter + cb->Size - tmpReader);
}
/********************************************************************************
 * @fn      CBuffer_SizeCount
 *
 * @brief   Return number of free bytes in the CB.
 *
 * @param   cb  - CBuffer Pointer
 *
 * @return  free bytes in the CB.
 ********************************************************************************/
CBuffer_Length_T CBuffer_SizeCount(CBuffer_T *cb)
{
	return (cb->Size - 1 - CBuffer_DataCount(cb));
}
/********************************************************************************
 * @fn      CBuffer_Read
 *
 * @brief   Read upto <nBytes> from the CB. Actual number of read bytes is returned
 *
 * @param   cb  	- CBuffer Pointer
 *					buf		- pointer of read buffer
 *					nBytes- Number of bytes want to read
 *
 * @return  Number of read bytes
 ********************************************************************************/
CBuffer_Length_T CBuffer_Read(CBuffer_T *cb, void *buf, CBuffer_Length_T nBytes)
{
	CBuffer_Length_T i;

	for (i = 0; i < nBytes; i++)
	{
		// See if any data is available
		if (cb->Reader != cb->Writer)
		{
			// Grab a byte from the internal buffer
			*((CBuffer_Data_T *)buf) = cb->Data[cb->Reader];
			buf = (CBuffer_Data_T *)buf + 1;

			// Check for wrap-around
			if (cb->Reader + 1 == cb->Size)
				cb->Reader = 0;
			else
				cb->Reader = cb->Reader + 1;
		}
		else
		{
			break;
		}
	}

	return i; // Number of bytes read
}
/********************************************************************************
 * @fn      CBuffer_Write
 *
 * @brief   Write upto <nBytes> to the CB. Actual number of written byte is returned.
 *
 * @param   cb  	- CBuffer Pointer
 *					buf		- pointer of write buffer
 *					nBytes- Number of bytes want to write
 *
 * @return  Number of write bytes
 ********************************************************************************/
CBuffer_Length_T CBuffer_Write(CBuffer_T *cb, const void *buf, CBuffer_Length_T nBytes)
{
	CBuffer_Length_T i;

	for (i = 0; i < nBytes; i++)
	{
		// First check to see if there is space in the buffer
		if ((cb->Writer + 1 == cb->Reader) || ((cb->Writer + 1 == cb->Size) && (cb->Reader == 0)))
		{
			cb->Overflow += (nBytes - i);
			break;
		}
		else
		{
			// Write a byte to the internal buffer
			cb->Data[cb->Writer] = *((CBuffer_Data_T *)buf);
			buf = (CBuffer_Data_T *)buf + 1;

			// Check for wrap-around
			if (cb->Writer + 1 == cb->Size)
				cb->Writer = 0;
			else
				cb->Writer = cb->Writer + 1;
		}
	}

	return i; // Number of bytes write
}
/********************************************************************************
 * END OF FILE
 ********************************************************************************/
