/*!
 *  @file       sys_data_manager.h
 *  @brief      Database of task layers to store
 *              the requests/responses/events of tasks.
 * 
 *  @author     Vy Luu
 *  @copyright  Fiot Co.,Ltd
 *  @version    1.1.1.20181018
 *  @date       Oct 18, 2018
 */
/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include "sys_data_manager.h"
#include "cbuffer.h"
#include "dbuffer.h"
#include "string.h"
/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/

/*******************************************************************************
 *  TYPEDEFS
 ******************************************************************************/
/*******************************************************************************
 * VARIABLES - LOCAL
 ******************************************************************************/
static CBuffer_Data_T dmLINE_Buffer[__DM_MAX_LINE_BUFF];
static CBuffer_T dmLINE_CBuffer[__DM_MAX_LINE];

DM_LINE_Idx_T dmLINE_Idx = -1;
uint32_t dmLINE_Position = 0x00;
DM_LINE_MaxSize_T dmLINE_MaxSize[__DM_MAX_LINE] = {0};
DM_LINE_DataSize_T dmLINE_DataSize[__DM_MAX_LINE]= {0};

DBuffer_T dmVAR_DBuffer[__DM_MAX_VAR];
DBuffer_Data_T dmVAR_DBuffer_Data[__DM_MAX_VAR_BUFF] = {0};

DM_VAR_Idx_T dmVAR_Idx = -1;
uint32_t dmVAR_Position = 0x00;
DM_VAR_DataSize_T dmVAR_DataSize[__DM_MAX_VAR];
/*******************************************************************************
 * FUNCTIONS - LOCAL
 ******************************************************************************/

/*******************************************************************************
 * LINE FUNCTIONS - API
 ******************************************************************************/
DM_LINE_Idx_T DM_LINE_Init(uint16_t iMaxSize, uint16_t iDataSize)
{
  if (dmLINE_Idx < __DM_MAX_LINE)
  {
    dmLINE_Idx++;
    
    dmLINE_MaxSize[dmLINE_Idx] = iMaxSize;
    dmLINE_DataSize[dmLINE_Idx] = iDataSize;

    uint32_t totalSize = dmLINE_MaxSize[dmLINE_Idx] * dmLINE_DataSize[dmLINE_Idx];
		
    CBuffer_Init(&dmLINE_CBuffer[dmLINE_Idx],
                 &dmLINE_Buffer[dmLINE_Position],
                 totalSize);

    dmLINE_Position += totalSize;
    
    return (dmLINE_Idx);

  }

  return -1;
}

void DM_LINE_Read(DM_LINE_Idx_T iIdx, void *oData)
{
  if (iIdx <= dmLINE_Idx && iIdx > -1)
  {
    CBuffer_Read(&dmLINE_CBuffer[iIdx], oData, dmLINE_DataSize[iIdx]);
  }
}

void DM_LINE_Write(DM_LINE_Idx_T iIdx, void *iData)
{
  if (iIdx <= dmLINE_Idx && iIdx > -1)
  {
    if (DM_LINE_SizeCount(iIdx) == 0)
    {
			DM_LINE_Clear(iIdx);
    }
    CBuffer_Write(&dmLINE_CBuffer[iIdx], iData, dmLINE_DataSize[iIdx]);
  }
}

void DM_LINE_Clear(DM_LINE_Idx_T iIdx)
{
  if (iIdx <= dmLINE_Idx && iIdx > -1)
  {
    CBuffer_Clear(&dmLINE_CBuffer[iIdx]);
  }
}

DM_Len_T DM_LINE_DataCount(DM_LINE_Idx_T iIdx)
{
  if (iIdx <= dmLINE_Idx && iIdx > -1)
  {
    return CBuffer_DataCount(&dmLINE_CBuffer[iIdx]);
  }
  return 0;
}

DM_Len_T DM_LINE_SizeCount(DM_LINE_Idx_T iIdx)
{
  if (iIdx <= dmLINE_Idx && iIdx > -1)
  {
    return CBuffer_SizeCount(&dmLINE_CBuffer[iIdx]);
  }
  return 0;
}
/*******************************************************************************
 * VAR FUNCTIONS - API
 ******************************************************************************/
DM_VAR_Idx_T DM_VAR_Init(DM_VAR_DataSize_T iDataSize)
{
  if (dmVAR_Idx < __DM_MAX_VAR)
  {
    //Insert VAR
    dmVAR_Idx++;
    //Store DataSize of VAR
    dmVAR_DataSize[dmVAR_Idx] = iDataSize;
    //Set DBuffer
    DBuffer_Init(&dmVAR_DBuffer[dmVAR_Idx],
                 &dmVAR_DBuffer_Data[dmVAR_Position],
                 dmVAR_DataSize[dmVAR_Idx],
                 1);   

    dmVAR_Position += dmVAR_DataSize[dmVAR_Idx];
    return (dmVAR_Idx);
  }

  return -1;
}

void DM_VAR_Read(DM_VAR_Idx_T iIdx, void *oData)
{
  if (iIdx <= dmVAR_Idx && iIdx > -1)
  {
    //Read data from Dbuffer
    DBuffer_Read(&dmVAR_DBuffer[iIdx], 0x00, oData);
  }
}

void DM_VAR_Write(DM_VAR_Idx_T iIdx, void *iData)
{
  if (iIdx <= dmVAR_Idx && iIdx > -1)
  {
    //Write data from Dbuffer
    DBuffer_Write(&dmVAR_DBuffer[iIdx], 0x00, iData);
  }
}

void DM_VAR_Clear(DM_VAR_Idx_T iIdx)
{
  if (iIdx <= dmVAR_Idx && iIdx > -1)
  {
    //Read data from Dbuffer
    DBuffer_Clear(&dmVAR_DBuffer[iIdx], 0x00);
  }
}
/*******************************************************************************
 * END
 ******************************************************************************/
