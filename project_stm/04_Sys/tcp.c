/*!
 * @file			tcp.c
 * @brief			
 * 
 * @author  	1.0 - Linh Huynh
 *
 * @copyright
 * @version 	1.0
 * @date    	2019-05-25
 * @example		
 */
/********************************************************************************
 *	INCLUDES
 ********************************************************************************/
#include "stdint.h"
#include "tcp.h"
#include "fiot_hal_timer.h"
#include "cbuffer.h"
#include "usbd_cdc_if.h"
#include "usb_device.h"

/********************************************************************************
 * CONSTANTS
 ********************************************************************************/
#define TCP_PAYLOAD_MAX_SIZE (1024)
#define TCP_PKT_HEADER       (0x55)

#define TCP_TIMEOUT  (10000000)
/********************************************************************************
 * TYPEDEF
 ********************************************************************************/
typedef enum 
{ 
  TCP_PKT_HEADER_IDX = 0       ,
  TCP_PKT_TYPE_IDX             ,
  TCP_PKT_PAYLOAD_SIZE_IDX     ,
  TCP_PKT_PAYLOAD_DATA_IDX = 4 
} TCP_Pkt_Idx_T;

typedef enum 
{
  TCP_TIMER_STATE_DIS,
  TCP_TIMER_STATE_EN
} TCP_Timer_State_T;

typedef enum 
{
  TCP_STATE_GET_HEADER        ,
  TCP_STATE_GET_TYPE          ,
  TCP_STATE_GET_PAYLOAD_SIZE  ,
  TCP_STATE_GET_PAYLOAD_DATA  ,
  TCP_STATE_MAX
} TCP_State_T;

typedef enum 
{
  TCP_TYPE_TCP_CHECK_CONNECTION  ,
  TCP_TYPE_TCP_ESTAB_CONNECTION  ,
  TCP_TYPE_TCP_SEND_DATA         ,
  TCP_TYPE_TCP_REC_DATA          ,
  TCP_TYPE_TCP_CLOSE_CONNECTION  ,
  TCP_TYPE_MAX
} TCP_Type_T;

typedef struct 
{
  uint8_t        Header;
  TCP_Type_T     Type;
  uint16_t       Payload_Size;
  uint8_t        Payload_Data[1000];
} TCP_Packet_T;

typedef enum
{
  TCP_DATA_CB_STATE_ERR,
  TCP_DATA_CB_STATE_READY
} TCP_Data_CB_State_T;

typedef struct 
{
  TCP_Data_CB_State_T State;
  uint8_t Data[1000];
  uint16_t Len;
} TCP_Data_CB_T;

/********************************************************************************
 * VAR
 ********************************************************************************/
HAL_Timer_T tcpTimer;
TCP_Timer_State_T tcpState_Timer = TCP_TIMER_STATE_DIS;

// CBuffer 
static CBuffer_T cBuffer;
static uint8_t cBuffer_Data[1000];

// State tcp
TCP_State_T tcpState = TCP_STATE_GET_HEADER;
TCP_Packet_T tcpPacket;

TCP_Data_CB_T tcpData_CB[TCP_TYPE_MAX] = {TCP_DATA_CB_STATE_ERR};
/*******************************************************************************
 * FUNCTIONS - LOCAL
 ******************************************************************************/
void TCP_Init_USB(void);
void TCP_Init_Timer(void);
void TCP_Init_CBuffer(void);
void TCP_Timer_Callback(void);
void TCP_Convert_Data(TCP_Type_T TCP_Type, uint8_t *TCP_Data, uint16_t Len, uint8_t *oData);
void TCP_Transfer_Data(uint8_t *tx_Buf, uint16_t tx_Len, uint8_t *rx_Buf, uint16_t rx_Len);
/********************************************************************************
 * FUNCTIONS - CALLBACK
 ********************************************************************************/
void SUSB_Callback_Fxn(uint8_t* Buf, uint32_t Len)
{
  CBuffer_Write(&cBuffer, Buf, Len);
  if (tcpState_Timer != TCP_TIMER_STATE_EN)
  {
    FIOT_HAL_Timer_Start(&tcpTimer);
    tcpState_Timer = TCP_TIMER_STATE_EN;
  }
}

void TCP_Timer_Callback(void)
{
  CBuffer_Length_T tCBuffer_Length = CBuffer_DataCount(&cBuffer);

  if (tCBuffer_Length == 0)
    return;

  switch (tcpState)
  {
    case TCP_STATE_GET_HEADER:
    {
      CBuffer_Read(&cBuffer, &tcpPacket.Header, 1);

      // Check header
      if (tcpPacket.Header == TCP_PKT_HEADER)
      {
        tcpState = TCP_STATE_GET_TYPE;
      }
      else 
      {
        tcpState = TCP_STATE_GET_HEADER;
      }
    }
    break;
    case TCP_STATE_GET_TYPE:
    {
      CBuffer_Read(&cBuffer, &tcpPacket.Type, 1);

      // Check type
      if (tcpPacket.Type < TCP_TYPE_MAX)
      {
        tcpState = TCP_STATE_GET_PAYLOAD_SIZE;
      }
      else 
      {
        tcpState = TCP_STATE_GET_HEADER;
      }
    }
    break;
    case TCP_STATE_GET_PAYLOAD_SIZE:
    {
      uint8_t tData[2];
      CBuffer_Read(&cBuffer, tData, 2);
      tcpPacket.Payload_Size = tData[0];
      tcpPacket.Payload_Size = (tcpPacket.Payload_Size << 8) + tData[1];

      // Check payload size
      if (tcpPacket.Payload_Size <= TCP_PAYLOAD_MAX_SIZE)
      {
        tcpState = TCP_STATE_GET_PAYLOAD_DATA;
      }
      else 
      {
        tcpState = TCP_STATE_GET_PAYLOAD_SIZE;
      }
    }
    break;
    case TCP_STATE_GET_PAYLOAD_DATA:
    {
      //wait for enough data
      if (CBuffer_DataCount(&cBuffer) < tcpPacket.Payload_Size)
        return;

      CBuffer_Read(&cBuffer, &tcpPacket.Payload_Data, tcpPacket.Payload_Size);

      // Update state
      tcpData_CB[tcpPacket.Type].State = TCP_DATA_CB_STATE_READY;
      memcpy(tcpData_CB[tcpPacket.Type].Data, tcpPacket.Payload_Data, tcpPacket.Payload_Size);
      tcpData_CB[tcpPacket.Type].Len = tcpPacket.Payload_Size;

      tcpState = TCP_STATE_GET_HEADER;
      tcpPacket.Header = 0;
      tcpPacket.Type = 0;
      tcpPacket.Payload_Size = 0;
      memset(tcpPacket.Payload_Data, 0x00, 1000);

      if (tcpState_Timer != TCP_TIMER_STATE_DIS)
      {
        FIOT_HAL_Timer_Stop(&tcpTimer);
        tcpState_Timer = TCP_TIMER_STATE_DIS;
      }
    }
    break;
		
		default: break;
  }
}

/********************************************************************************
 * FUNCTIONS - API
 ********************************************************************************/
void TCP_Init(void)
{
  // tcpLINE_USB_Cmd = SUSB_SubscribeLINE(SUSB_LINE_CMD);

  // tcpLINE_USB_Evt = SUSB_SubscribeLINE(SUSB_LINE_EVT);
  TCP_Init_USB();

  TCP_Init_CBuffer();

  TCP_Init_Timer();
}
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
TCP_Status_T TCP_Check_Connection_Status(TCP_Connection_Status_T *Status_Data)
{
  uint8_t tPacket[5];

  uint8_t tLen = 1;
  uint8_t rx_Buf[5];

  // Add header
  tPacket[TCP_PKT_HEADER_IDX]           = TCP_PKT_HEADER;
  // Add type
  tPacket[TCP_PKT_TYPE_IDX]             = TCP_TYPE_TCP_CHECK_CONNECTION;
  // Add Payload Size
  tPacket[TCP_PKT_PAYLOAD_SIZE_IDX]     = (tLen >> 8) & 0xFF;
  tPacket[TCP_PKT_PAYLOAD_SIZE_IDX + 1] = (tLen) & 0xFF;
  // Add Payload Data
  memset(&tPacket[TCP_PKT_PAYLOAD_DATA_IDX], (char)0x00, 1);

  // Send data
  CDC_Transmit_FS(tPacket, tLen + 4);

  for (uint32_t i = 0; i < TCP_TIMEOUT; i++)
  {
    if (tcpData_CB[TCP_TYPE_TCP_CHECK_CONNECTION].State != TCP_DATA_CB_STATE_ERR)
    {
      Status_Data->Status = (TCP_Status_T)((uint8_t)tcpData_CB[TCP_TYPE_TCP_CHECK_CONNECTION].Data[0]);
      memcpy(Status_Data->IP_Addr, &tcpData_CB[TCP_TYPE_TCP_CHECK_CONNECTION].Data[1], 15);
      memcpy(Status_Data->Port, &tcpData_CB[TCP_TYPE_TCP_CHECK_CONNECTION].Data[16], 4);

      tcpData_CB[TCP_TYPE_TCP_CHECK_CONNECTION].State = TCP_DATA_CB_STATE_ERR;
      return TCP_STATUS_OK;
    }
  }

  return TCP_STATUS_ERR;
}

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
TCP_Status_T TCP_Establish_Connection(TCP_IP_Addr_T *Addr, TCP_Port *Port)
{
  uint8_t tPacket[23];

  uint8_t tLen = 19;
  uint8_t rx_Buf[5];

  // Add header
  tPacket[TCP_PKT_HEADER_IDX]           = TCP_PKT_HEADER;
  // Add type
  tPacket[TCP_PKT_TYPE_IDX]             = TCP_TYPE_TCP_ESTAB_CONNECTION;
  // Add Payload Size
  tPacket[TCP_PKT_PAYLOAD_SIZE_IDX]     = (tLen >> 8) & 0xFF;
  tPacket[TCP_PKT_PAYLOAD_SIZE_IDX + 1] = (tLen) & 0xFF;
  // Add Payload Data
  memcpy((char *)&tPacket[TCP_PKT_PAYLOAD_DATA_IDX], (char *)Addr, 15);

  memcpy((char*)&tPacket[TCP_PKT_PAYLOAD_DATA_IDX + 15], Port, 4);

  // Send data
  CDC_Transmit_FS(tPacket, tLen + 4);

  for (uint32_t i = 0; i < TCP_TIMEOUT; i++)
  {
    if (tcpData_CB[TCP_TYPE_TCP_ESTAB_CONNECTION].State != TCP_DATA_CB_STATE_ERR)
    {
      tcpData_CB[TCP_TYPE_TCP_ESTAB_CONNECTION].State = TCP_DATA_CB_STATE_ERR;
      return TCP_STATUS_OK;
    }
  }

  return TCP_STATUS_ERR;
}

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
TCP_Status_T TCP_Send_Data(TCP_Data_T *Send_Data, TCP_Len_T Len)
{
  uint8_t tPacket[Len + 4];

  uint8_t tLen = Len;
  uint8_t rx_Buf[5];

  // Add header
  tPacket[TCP_PKT_HEADER_IDX]           = TCP_PKT_HEADER;
  // Add type
  tPacket[TCP_PKT_TYPE_IDX]             = TCP_TYPE_TCP_SEND_DATA;
  // Add Payload Size
  tPacket[TCP_PKT_PAYLOAD_SIZE_IDX]     = (tLen >> 8) & 0xFF;
  tPacket[TCP_PKT_PAYLOAD_SIZE_IDX + 1] = (tLen) & 0xFF;
  // Add Payload Data
  memcpy((char *)&tPacket[TCP_PKT_PAYLOAD_DATA_IDX], (char *)Send_Data, Len);

  // Send data
  CDC_Transmit_FS(tPacket, tLen + 4);

  for (uint32_t i = 0; i < TCP_TIMEOUT; i++)
  {
    if (tcpData_CB[TCP_TYPE_TCP_SEND_DATA].State != TCP_DATA_CB_STATE_ERR)
    {
      tcpData_CB[TCP_TYPE_TCP_SEND_DATA].State = TCP_DATA_CB_STATE_ERR;
      return TCP_STATUS_OK;
    }
  }

  return TCP_STATUS_ERR;
}

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
TCP_Status_T TCP_Receive_Data(TCP_Data_T *Receive_Data)
{
  uint8_t tPacket[100];
  uint8_t tLen = 1;

  uint8_t rx_Buf[5];

  // Add header
  tPacket[TCP_PKT_HEADER_IDX]           = TCP_PKT_HEADER;
  // Add type
  tPacket[TCP_PKT_TYPE_IDX]             = TCP_TYPE_TCP_REC_DATA;
  // Add Payload Size
  tPacket[TCP_PKT_PAYLOAD_SIZE_IDX]     = (tLen >> 8) & 0xFF;
  tPacket[TCP_PKT_PAYLOAD_SIZE_IDX + 1] = (tLen) & 0xFF;
  // Add Payload Data
  memset(&tPacket[TCP_PKT_PAYLOAD_DATA_IDX], (char)0x00, tLen);

  // Send data
  CDC_Transmit_FS(tPacket, tLen + 4);

  for (uint32_t i = 0; i < TCP_TIMEOUT; i++)
  {
    if (tcpData_CB[TCP_TYPE_TCP_REC_DATA].State != TCP_DATA_CB_STATE_ERR)
    {
			
      tcpData_CB[TCP_TYPE_TCP_REC_DATA].State = TCP_DATA_CB_STATE_ERR;
      memcpy(Receive_Data, 
             &tcpData_CB[TCP_TYPE_TCP_REC_DATA].Data, 
             tcpData_CB[TCP_TYPE_TCP_REC_DATA].Len);
//			for (uint8_t i = 0; i < tcpData_CB[TCP_TYPE_TCP_REC_DATA].Len; i++)
//			{
//				Receive_Data[i] = tcpData_CB[TCP_TYPE_TCP_REC_DATA].Data[i];
//			}
			
      return TCP_STATUS_OK;
    }
  }

  return TCP_STATUS_ERR;
}

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
TCP_Status_T TCP_Close_Connection(void)
{
  uint8_t tPacket[100];
  uint8_t tLen = 1;

  uint8_t rx_Buf[5];

  // Add header
  tPacket[TCP_PKT_HEADER_IDX]           = TCP_PKT_HEADER;
  // Add type
  tPacket[TCP_PKT_TYPE_IDX]             = TCP_TYPE_TCP_CLOSE_CONNECTION;
  // Add Payload Size
  tPacket[TCP_PKT_PAYLOAD_SIZE_IDX]     = (tLen >> 8) & 0xFF;
  tPacket[TCP_PKT_PAYLOAD_SIZE_IDX + 1] = (tLen) & 0xFF;
  // Add Payload Data
  memset(&tPacket[TCP_PKT_PAYLOAD_DATA_IDX], (char)0x00, tLen);

  // Send data
  CDC_Transmit_FS(tPacket, tLen + 4);

  for (uint32_t i = 0; i < TCP_TIMEOUT; i++)
  {
    if (tcpData_CB[TCP_TYPE_TCP_CLOSE_CONNECTION].State != TCP_DATA_CB_STATE_ERR)
    {
      tcpData_CB[TCP_TYPE_TCP_CLOSE_CONNECTION].State = TCP_DATA_CB_STATE_ERR;
      return TCP_STATUS_OK;
    }
  }

  return TCP_STATUS_ERR;
}

/*******************************************************************************
 * FUNCTIONS - LOCAL
 ******************************************************************************/
void TCP_Init_USB()
{
  MX_USB_DEVICE_Init();

  CDC_Set_Callback_Fxn(SUSB_Callback_Fxn);
}

void TCP_Init_CBuffer()
{
  CBuffer_Init(&cBuffer, cBuffer_Data, sizeof(cBuffer_Data));
}

void TCP_Init_Timer()
{
  HAL_Timer_Setting_T tTimer_Settings;
  //Init the Timer 1
  tTimer_Settings.Module = TIMER_1;
  //Interrupt enable every 2 MS
  tTimer_Settings.Period = 1;
  //Init Timer
  FIOT_HAL_Timer_Init(&tcpTimer, tTimer_Settings, TCP_Timer_Callback);
}
