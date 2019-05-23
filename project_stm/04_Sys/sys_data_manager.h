/*!
 *  @file       sys_data_manager.h
 *  @brief      Database of task layers to store
 *              the requests/responses/events of tasks.
 * 
 *  @author     Vy Luu
 *  @copyright  Fiot Co.,Ltd
 *  @version    1.1.1
 *  @date       Aug 08, 2018
 */
#ifndef __SYS_DATA_MANAGER_H__
#define __SYS_DATA_MANAGER_H__

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include <stdint.h>
/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/
#define __DM_MAX_LINE_BUFF (8000) //2000 Bytes 
#define __DM_MAX_LINE (40)

#define __DM_MAX_VAR_BUFF (600) //200 Bytes
#define __DM_MAX_VAR (40)
/*******************************************************************************
 *  TYPEDEFS
 ******************************************************************************/
typedef enum { 
	DM_LINE_EVT = 0x00,
	DM_LINE_CMD,
	DM_LINE_MAX
} DM_Line_T;
typedef enum {
	DM_VAR_EVT = 0x00,
	DM_VAR_CMD,
	DM_VAR_MAX
} DM_Var_T;
/*******************************************************************************
 *  SYSTEM EVENT TYPEDEFS
 ******************************************************************************/
/**
 * @brief Type of Data Length
 */
typedef uint16_t DM_Len_T;
/**
 * @brief Type of Max data in a LINE 
 */
typedef uint16_t DM_LINE_MaxSize_T;
/**
 * @brief Type of Data Size in LINE
 */
typedef uint16_t DM_LINE_DataSize_T;
/**
 * @brief Type of LINE Idx
 */
typedef int8_t DM_LINE_Idx_T;
/**
 * @brief Type of Data Size in VAR
 */
typedef uint16_t DM_VAR_DataSize_T;
/**
 * @brief Type of VAR Idx
 */
typedef int8_t DM_VAR_Idx_T;
/*******************************************************************************
 * LINE FUNCTIONS - API
 ******************************************************************************/
/*!
 *******************************************************************************
 * @fn      DM_LINE_Idx_T DM_LINE_Init(DM_LINE_MaxSize_T iMaxSize,
 *                                     DM_LINE_DataSize_T iDataSize)
 *
 * @brief   Register a LINE on sys data manager 
 *
 * @param   iMaxSize: Max data of a LINE
 * @param   iDataSize: Data Size of Data in LINE
 * 
 * @return  Idx of LINE. 
 * 
 * @note    If Full line or error, return 0x00
 ******************************************************************************/
DM_LINE_Idx_T DM_LINE_Init(DM_LINE_MaxSize_T iMaxSize,
                           DM_LINE_DataSize_T iDataSize);
/*!
 *******************************************************************************
 * @fn      void DM_LINE_Read(DM_LINE_Idx_T iIdx, void *oData)
 *
 * @brief   Wait and Read a Data in LINE
 *
 * @param   iIdx - Idx of LINE. 
 * @param   oData  - Return data buffer
 *
 * @return  None
 ******************************************************************************/
void DM_LINE_Read(DM_LINE_Idx_T iIdx, void *oData);
/*!
 *******************************************************************************
 * @fn      void DM_LINE_Write(DM_LINE_Idx_T iIdx, void *iData)
 *
 * @brief   Write a Data to LINE
 *
 * @param   iIdx - Idx of LINE. 
 * @param   iData  - Written data buffer
 *
 * @return  None
 ******************************************************************************/
void DM_LINE_Write(DM_LINE_Idx_T iIdx, void *iData);
/*!
 *******************************************************************************
 * @fn      void DM_LINE_Clear(DM_LINE_Idx_T iIdx)
 *
 * @brief   Clear a LINE
 *
 * @param   iIdx - Idx of LINE. 
 *
 * @return  None
 ******************************************************************************/
void DM_LINE_Clear(DM_LINE_Idx_T iIdx);
/*!
 *******************************************************************************
 * @fn      DM_Len_T DM_LINE_DataCount(DM_LINE_Idx_T iIdx)
 *
 * @brief   Number of data in LINE
 *
 * @param   iIdx - Idx of LINE. 
 *
 * @return  Number of data in LINE
 ******************************************************************************/
DM_Len_T DM_LINE_DataCount(DM_LINE_Idx_T iIdx);
/*!
 *******************************************************************************
 * @fn      DM_Len_T DM_LINE_SizeCount(DM_LINE_Idx_T iIdx)
 *
 * @brief   Number of free data in LINE
 *
 * @param   iIdx - Idx of LINE. 
 *
 * @return  Number of free data in LINE
 ******************************************************************************/
DM_Len_T DM_LINE_SizeCount(DM_LINE_Idx_T iIdx);
/*******************************************************************************
 * VAR FUNCTIONS - API
 ******************************************************************************/
/*!
 *******************************************************************************
 * @fn      DM_VAR_Idx_T DM_VAR_Init(DM_VAR_DataSize_T iDataSize)
 *
 * @brief   Register a VAR on sys data manager. VAR is a global variable  
 *          that all tasks or apps can read anytime
 *
 * @param   iDataSize: Data Size of Data in VAR
 * 
 * @return  Idx of VAR. 
 * 
 * @note    If Full VAR or error, return 0x00
 ******************************************************************************/
DM_VAR_Idx_T DM_VAR_Init(DM_VAR_DataSize_T iDataSize);
/*!
 *******************************************************************************
 * @fn      void DM_VAR_Read(DM_VAR_Idx_T iIdx,void *oData)
 *
 * @brief   Read the value of VAR
 *
 * @param   iIdx - Index of VAR
 * @param   oData - Return data.
 *
 * @return  None
 ******************************************************************************/
void DM_VAR_Read(DM_VAR_Idx_T iIdx, void *oData);
/*!
 *******************************************************************************
 * @fn      void DM_VAR_Write(DM_VAR_Idx_T iIdx,void *iData)
 *
 * @brief   Write the Data at iIdx in Buffer
 *
 * @param   iIdx - Index of VAR
 * @param   iData - Written data.
 *
 * @return  None
 ******************************************************************************/
void DM_VAR_Write(DM_VAR_Idx_T iIdx, void *iData);
/*!
 *******************************************************************************
 * @fn      void DM_VAR_Clear(DM_VAR_Idx_T iIdx)
 *
 * @brief   Clear the Data at iIdx in Buffer
 *
 * @param   iIdx - Index of VAR
 *
 * @return  None
 ******************************************************************************/
void DM_VAR_Clear(DM_VAR_Idx_T iIdx);
#endif /* __SYS_DATA_MANAGER_H__ */
