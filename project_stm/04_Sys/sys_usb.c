/*!
 *  @file       sys_uart.h
 *  @brief      Tranfer and recieve/verify from/to sys_data_manager\n
 *              Manage the functions of UART: send command, send data 
 * 
 *  @author     Dat Hoang
 *  @copyright  Fiot Co.,Ltd
 *  @version    1.0.1
 *  @date       2018-11-29
 */
 /*!
 *  @file       sys_uart.cpp
 *  @brief      Tranfer and recieve/verify from/to sys_data_manager\n
 *              Manage the functions of UART: send command, send data 
 * 
 *  @author     Dat Hoang
 *  @copyright  Fiot Co.,Ltd
 *  @version    1.0.1
 *  @date       2018-11-23
 */
/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include "cbuffer.h"
#include "sys_usb.h"
#include "fos.h"
#include "sys_data_manager.h"
#include "usbd_cdc_if.h"
/*******************************************************************************
 * MACRO
 ******************************************************************************/


/*******************************************************************************
 * CONFIGURE
 ******************************************************************************/

/*******************************************************************************
 * CONSTANT
 ******************************************************************************/
#define SUSB_PAYLOAD_MAX_SIZE (1024)
#define SUSB_PKT_HEADER       (0x55)
/*******************************************************************************
 * MACRO
 ******************************************************************************/

/*******************************************************************************
 *  TYPEDEFS
 ******************************************************************************/
typedef enum 
{ 
  SUSB_PKT_HEADER_IDX = 0       ,
  SUSB_PKT_TYPE_IDX             ,
  SUSB_PKT_PAYLOAD_SIZE_IDX     ,
  SUSB_PKT_PAYLOAD_DATA_IDX = 4 
} SUSB_Pkt_Idx_T;

typedef enum 
{
  SUSB_STATE_GET_HEADER        ,
  SUSB_STATE_GET_TYPE          ,
  SUSB_STATE_GET_PAYLOAD_SIZE  ,
  SUSB_STATE_GET_PAYLOAD_DATA  ,
  SUSB_STATE_MAX
} SUSB_State_T;

typedef enum 
{
  SUSB_TYPE_TCP_CHECK_CONNECTION  ,
  SUSB_TYPE_TCP_ESTAB_CONNECTION  ,
  SUSB_TYPE_TCP_SEND_DATA         ,
  SUSB_TYPE_TCP_REC_DATA          ,
  SUSB_TYPE_TCP_CLOSE_CONNECTION  ,
  SUSB_TYPE_MAX
} SUSB_Type_T;

typedef struct 
{
  uint8_t        Header;
  SUSB_Type_T    Type;
  uint16_t       Payload_Size;
  uint8_t        Payload_Data[1000];
} SUSB_Packet_T;
/*******************************************************************************
 * VARIABLES - LOCAL
 ******************************************************************************/
// Line
static DM_LINE_Idx_T susbLINE_Evt;
static DM_LINE_Idx_T susbLINE_Cmd;

// FOS
static FOS_Handle_T susbFOS_Cmd_Loop;
static FOS_Handle_T susbFOS_Proc_Rec_Data;

// CBuffer 
static CBuffer_T cBuffer;
static uint8_t cBuffer_Data[1000];

// State
SUSB_State_T susbState = SUSB_STATE_GET_HEADER;

// Packet
SUSB_Packet_T susbPacket;
/*******************************************************************************
 * FUNCTIONS - LOCAL
 ******************************************************************************/
void SUSB_Setup_With_DM(void);
void SUSB_Setup_With_USB(void);
void SUSB_Setup_CBuffer(void);
void SUSB_Proc_Rec_Data(void);
void SUSB_Cmd_Loop(void);
/*******************************************************************************
 * FUNCTIONS - CALLBACK
 ******************************************************************************/
void SUSB_Receive_CallbackFxn(uint8_t* Buf, uint32_t Len)
{
  CBuffer_Write(&cBuffer, buf, len);
}
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
void SUSB_Init(void)
{
  SUSB_Setup_With_DM();

  SUSB_Setup_With_USB();

  SUSB_Setup_CBuffer();

  // Create task
  FOS_CreateTask(&susbFOS_Cmd_Loop, FOS_TYPE_SYS, SUSB_Cmd_Loop, NULL, FOS_ENABLE);
  FOS_CreateTask(&susbFOS_Proc_Rec_Data, FOS_TYPE_SYS, SUSB_Proc_Rec_Data, NULL, FOS_ENABLE);
}

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
DM_LINE_Idx_T SUSB_SubscribeLINE(SUSB_Line_T iLine)
{
  switch (iLine)
  {
    case SUSB_LINE_CMD:
      return susbLINE_Cmd;
    case SUSB_LINE_EVT:
      return susbLINE_Evt;
    default: 
      return -1;
  } 
}

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
void SUSB_Cmd_Loop(void)
{
  if (DM_LINE_DataCount(susbLINE_Cmd) > 0)
  {
    SUSB_Cmd_T tCmd;
    DM_LINE_Read(susbLINE_Cmd, &tCmd);
    
    uint8_t tPacket[tCmd.Data.Len + 4];

    // Add header
    tPacket[SUSB_PKT_HEADER_IDX]           = SUSB_PKT_HEADER;
    // Add type
    tPacket[SUSB_PKT_TYPE_IDX]             = (SUSB_Type_T)tCmd.Type;
    // Add Payload Size
    tPacket[SUSB_PKT_PAYLOAD_SIZE_IDX]     = (tCmd.Data.Len >> 8) & 0xFF;
    tPacket[SUSB_PKT_PAYLOAD_SIZE_IDX + 1] = (tCmd.Data.Len) & 0xFF;
    // Add Payload Data
    memcpy((char *)&tPacket[SUSB_PKT_PAYLOAD_DATA_IDX],
           (char *)tCmd.Data.Data,
           tCmd.Data.Len);

    // Send data
    CDC_Transmit_FS(tPacket, tCmd.Data.Len + 4);
  }
}

/*******************************************************************************
 * FUNCTIONS - LOCAL
 ******************************************************************************/
void SUSB_Setup_With_DM(void)
{
  // Init Line Evt
  susbLINE_Evt = DM_LINE_Init(__SUSB_EVT_LINE_MAX_SIZE, __SUSB_EVT_LINE_DATA_SIZE);
  // Init Line Cmd
  susbLINE_Cmd = DM_LINE_Init(__SUSB_CMD_LINE_MAX_SIZE, __SUSB_CMD_LINE_DATA_SIZE);
}

void SUSB_Setup_With_USB(void)
{
  MX_USB_DEVICE_Init();
}

void SUSB_Setup_CBuffer(void)
{
  CBuffer_Init(&cBuffer, cBuffer_Data, sizeof(cBuffer_Data));
}

void SUSB_Proc_Rec_Data(void)
{
  CBuffer_Length_T tCBuffer_Length = CBuffer_DataCount(&cBuffer);

  if (tCBuffer_Length == 0)
    return;

  switch (susbState)
  {
    case SUSB_STATE_GET_HEADER:
    {
      CBuffer_Read(&cBuffer, &susbPacket.Header, 1);

      // Check header
      if (susbPacket.Header == SUSB_PKT_HEADER)
      {
        susbState = SUSB_STATE_GET_TYPE;
      }
      else 
      {
        susbState = SUSB_STATE_GET_HEADER;
      }
    }
    break;
    case SUSB_STATE_GET_TYPE:
    {
      CBuffer_Read(&cBuffer, &susbPacket.Type, 1);

      // Check type
      if (susbPacket.Type < SUSB_TYPE_MAX)
      {
        susbState = SUSB_STATE_GET_PAYLOAD_SIZE
      }
      else 
      {
        susbState = SUSB_STATE_GET_HEADER;
      }
    }
    break;
    case SUSB_STATE_GET_PAYLOAD_SIZE:
    {
      uint8_t tData[2];
      CBuffer_Read(&cBuffer, tData, 2);
      susbPacket.Payload_Size = tData[0];
      susbPacket.Payload_Size = (susbPacket.Payload_Size << 8) + tData[1]

      // Check payload size
      if (susbPacket.Len <= SUSB_PAYLOAD_MAX_SIZE)
      {
        susbState = SUSB_STATE_GET_PAYLOAD_DATA;
      }
      else 
      {
        susbState = SUSB_STATE_GET_HEADER;
      }
    }
    break;
    case SUSB_STATE_GET_PAYLOAD_DATA:
    {
      //wait for enough data
      if (CBuffer_DataCount(&cBuffer) < susbPacket.Payload_Size)
        return;

      CBuffer_Read(&cBuffer, &susbPacket.Payload_Data, susbPacket.Payload_Size);

      // Update Evt to sys_data_manager
      SUSB_Evt_Type_T tEvt;
      tEvt.Type = (SUSB_Evt_Type_T)susbPacket.Type;
      memcpy(tEvt.Data.Data, susbPacket.Payload_Data, susbPacket.Payload_Size);

      DM_LINE_Write(susbLINE_Evt, &tEvt);

    }
    break;
  }
}

/*******************************************************************************
 * END
 ******************************************************************************/
