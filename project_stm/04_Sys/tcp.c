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
#include "crc8.h"
#include "parson.h"
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
  TCP_PKT_PAYLOAD_SIZE_IDX     ,
  TCP_PKT_PAYLOAD_DATA_IDX = 3 ,
  TCP_PKT_CRC_8_IDX            ,
} TCP_Pkt_Idx_T;

typedef enum 
{
  TCP_TIMER_STATE_DIS,
  TCP_TIMER_STATE_EN
} TCP_Timer_State_T;

typedef enum 
{
  TCP_STATE_GET_HEADER        ,
  TCP_STATE_GET_PAYLOAD_SIZE  ,
  TCP_STATE_GET_PAYLOAD_DATA  ,
  TCP_STATE_GET_CRC           ,
  TCP_STATE_MAX
} TCP_State_T;

typedef enum 
{
  TCP_TYPE_HANDSHAKE             ,
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
  uint16_t       Payload_Size;
  uint8_t        Payload_Data[1000];
  int            CRC_8;                  
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

TCP_Data_CB_T tcpData_CB;
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
      tcpState = TCP_STATE_GET_CRC;

      
    }
    break;
    case TCP_STATE_GET_CRC:
    {
      CBuffer_Read(&cBuffer, &tcpPacket.CRC_8, 1);

			uint8_t tCRC = crc8(tcpPacket.Payload_Data, tcpPacket.Payload_Size);
      if (tCRC == tcpPacket.CRC_8)
      {
        // Update state
        tcpData_CB.State = TCP_DATA_CB_STATE_READY;
        memcpy(tcpData_CB.Data, tcpPacket.Payload_Data, tcpPacket.Payload_Size);
        tcpData_CB.Len = tcpPacket.Payload_Size;

        if (tcpState_Timer != TCP_TIMER_STATE_DIS)
        {
          FIOT_HAL_Timer_Stop(&tcpTimer);
          tcpState_Timer = TCP_TIMER_STATE_DIS;
        }
      }

      tcpState = TCP_STATE_GET_HEADER;
        tcpPacket.Header = 0;
        tcpPacket.Payload_Size = 0;
        memset(tcpPacket.Payload_Data, 0x00, 1000);
        tcpPacket.CRC_8 = 0;
    }
		
		default: break;
  }
}

/********************************************************************************
 * FUNCTIONS - API
 ********************************************************************************/
TCP_Status_T TCP_Init(void)
{
  // tcpLINE_USB_Cmd = SUSB_SubscribeLINE(SUSB_LINE_CMD);

  // tcpLINE_USB_Evt = SUSB_SubscribeLINE(SUSB_LINE_EVT);
  TCP_Init_USB();

  TCP_Init_CBuffer();

  TCP_Init_Timer();

  for (uint32_t i = 0; i < 0xFFFFFFFF; i++)
  {
    if (tcpData_CB.State != TCP_DATA_CB_STATE_ERR)
    {
      // Change state callback packet
      tcpData_CB.State = TCP_DATA_CB_STATE_ERR;

      // Create json value
      JSON_Value *smRoot_Value;
      const char *tCommand = NULL;
      // Parse to string
      smRoot_Value = json_parse_string((char *)tcpData_CB.Data);
      tCommand = json_object_get_string(json_object(smRoot_Value), "Data");
      if(strcmp(tCommand, "USB_Handshake") == 0)
        return TCP_STATUS_OK;
      else
        return TCP_STATUS_ERR;
    }
  }

  return TCP_STATUS_ERR_TIMEOUT;
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
  // Create json value
  JSON_Value* root_value = json_value_init_object();
  JSON_Object* root_object = json_value_get_object(root_value);

  json_object_set_string(root_object, "Cmd", "TCP_check_status_connection");
  // Parse to string
  char* tJson = NULL;
  tJson = json_serialize_to_string_pretty(root_value);

  uint16_t json_Len = strlen(tJson);
  uint8_t tPacket[json_Len + 4];

  // Add header
  tPacket[TCP_PKT_HEADER_IDX]           = TCP_PKT_HEADER;
  // Add Payload Size
  tPacket[TCP_PKT_PAYLOAD_SIZE_IDX]     = (json_Len >> 8) & 0xFF;
  tPacket[TCP_PKT_PAYLOAD_SIZE_IDX + 1] = (json_Len) & 0xFF;
  // Add Payload Data
  memcpy(&tPacket[TCP_PKT_PAYLOAD_DATA_IDX], tJson, json_Len);
  // Calculate and add CRC of data
  tPacket[json_Len + 3] = crc8(&tPacket[TCP_PKT_PAYLOAD_DATA_IDX], json_Len);

  // Send data
  CDC_Transmit_FS(tPacket, json_Len + 4);

  for (uint32_t i = 0; i < TCP_TIMEOUT; i++)
  {
    if (tcpData_CB.State != TCP_DATA_CB_STATE_ERR)
    {
      // Change state callback packet
      tcpData_CB.State = TCP_DATA_CB_STATE_ERR;

      // Create json value
      JSON_Value *smRoot_Value;
      const char *tCommand = NULL;
      // Parse to string
      smRoot_Value = json_parse_string((char *)tcpData_CB.Data);
      tCommand = json_object_get_string(json_object(smRoot_Value), "Data");
      if (strcmp(tCommand, "TCP_check_status_connection") == 0)
      {
        // get status
				const char *tStatus = NULL;
        const char *tIP_Address = NULL;
        uint16_t tPort;
        tStatus = json_object_get_string(json_object(smRoot_Value), "Status");
        tIP_Address = json_object_dotget_string(json_object(smRoot_Value), "Value.IP_address");
        tPort = (uint16_t)json_object_dotget_number(json_object(smRoot_Value), "Value.Port");

        // Copy data to response
        Status_Data->Status = ((strcmp(tStatus, "OK") ==0) ? TCP_STATUS_OK : TCP_STATUS_ERR);
        Status_Data->IP_Addr = (char *)tIP_Address;
        Status_Data->Port = tPort;

        return TCP_STATUS_OK;
      }
      else 
      {
        return TCP_STATUS_ERR;
      }
    }
  }

  return TCP_STATUS_ERR_TIMEOUT;
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
TCP_Status_T TCP_Establish_Connection(TCP_IP_Addr_T *Addr, TCP_Port Port)
{
  // Create json value
  JSON_Value* root_value = json_value_init_object();
  JSON_Object* root_object = json_value_get_object(root_value);

  json_object_set_string(root_object, "Cmd", "TCP_establish_connection");
  json_object_dotset_string(root_object, "Value.IP_address", Addr);
  json_object_dotset_number(root_object, "Value.Port", (double)Port);
  // Parse to string
  char* tJson = NULL;
  tJson = json_serialize_to_string_pretty(root_value);

  uint16_t json_Len = strlen(tJson);
  uint8_t tPacket[json_Len + 4];

  // Add header
  tPacket[TCP_PKT_HEADER_IDX]           = TCP_PKT_HEADER;
  // Add Payload Size
  tPacket[TCP_PKT_PAYLOAD_SIZE_IDX]     = (json_Len >> 8) & 0xFF;
  tPacket[TCP_PKT_PAYLOAD_SIZE_IDX + 1] = (json_Len) & 0xFF;
  // Add Payload Data
  memcpy(&tPacket[TCP_PKT_PAYLOAD_DATA_IDX], tJson, json_Len);
  // Calculate and add CRC of data
	uint8_t crc = crc8(&tPacket[TCP_PKT_PAYLOAD_DATA_IDX], json_Len);
  tPacket[json_Len + 3] = crc8(&tPacket[TCP_PKT_PAYLOAD_DATA_IDX], json_Len);

  // Send data
  CDC_Transmit_FS(tPacket, json_Len + 4);

  for (uint32_t i = 0; i < TCP_TIMEOUT; i++)
  {
    if (tcpData_CB.State != TCP_DATA_CB_STATE_ERR)
    {
      // Change state callback packet
      tcpData_CB.State = TCP_DATA_CB_STATE_ERR;
      
      // Create json value
      JSON_Value *smRoot_Value;
      const char *tCommand = NULL;
      // Parse to string
      smRoot_Value = json_parse_string((char *)tcpData_CB.Data);
      tCommand = json_object_get_string(json_object(smRoot_Value), "Data");
      if (strcmp(tCommand, "TCP_establish_connection") == 0)
      {
        // get status
				const char *tStatus = NULL;
        tStatus = json_object_get_string(json_object(smRoot_Value), "Status");

        if (strcmp(tStatus, "OK") == 0)
        {
          return TCP_STATUS_OK;
        }
        else 
        {
          return TCP_STATUS_ERR;
        }
      }
      else 
      {
        return TCP_STATUS_ERR;
      }
    }
  }

  return TCP_STATUS_ERR_TIMEOUT;
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
  // Create json value
  JSON_Value* root_value = json_value_init_object();
  JSON_Object* root_object = json_value_get_object(root_value);

  json_object_set_string(root_object, "Cmd", "TCP_send_data");
  json_object_dotset_string(root_object, "Value.Data", Send_Data);
  json_object_dotset_number(root_object, "Value.Len", (double)Len);
  // Parse to string
  char* tJson = NULL;
  tJson = json_serialize_to_string_pretty(root_value);

  uint16_t json_Len = strlen(tJson);
  uint8_t tPacket[json_Len + 4];

  // Add header
  tPacket[TCP_PKT_HEADER_IDX]           = TCP_PKT_HEADER;
  // Add Payload Size
  tPacket[TCP_PKT_PAYLOAD_SIZE_IDX]     = (json_Len >> 8) & 0xFF;
  tPacket[TCP_PKT_PAYLOAD_SIZE_IDX + 1] = (json_Len) & 0xFF;
  // Add Payload Data
  memcpy(&tPacket[TCP_PKT_PAYLOAD_DATA_IDX], tJson, json_Len);
  // Calculate and add CRC of data
  tPacket[json_Len + 3] = crc8(&tPacket[TCP_PKT_PAYLOAD_DATA_IDX], json_Len);

  // Send data
  CDC_Transmit_FS(tPacket, json_Len + 4);

  for (uint32_t i = 0; i < TCP_TIMEOUT; i++)
  {
    if (tcpData_CB.State != TCP_DATA_CB_STATE_ERR)
    {
      // Change state callback packet
      tcpData_CB.State = TCP_DATA_CB_STATE_ERR;
      
      // Create json value
      JSON_Value *smRoot_Value;
      const char *tCommand = NULL;
      // Parse to string
      smRoot_Value = json_parse_string((char *)tcpData_CB.Data);
      tCommand = json_object_get_string(json_object(smRoot_Value), "Data");
      if (strcmp(tCommand, "TCP_send_data") == 0)
      {
        // get status
				const char *tStatus = NULL;
        tStatus = json_object_get_string(json_object(smRoot_Value), "Status");

        if (strcmp(tStatus, "OK") == 0)
        {
          return TCP_STATUS_OK;
        }
        else 
        {
          return TCP_STATUS_ERR;
        }
      }
      else 
      {
        return TCP_STATUS_ERR;
      }
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
  // Create json value
  JSON_Value* root_value = json_value_init_object();
  JSON_Object* root_object = json_value_get_object(root_value);

  json_object_set_string(root_object, "Cmd", "TCP_receive_data");
  // Parse to string
  char* tJson = NULL;
  tJson = json_serialize_to_string_pretty(root_value);

  uint16_t json_Len = strlen(tJson);
  uint8_t tPacket[json_Len + 4];

  // Add header
  tPacket[TCP_PKT_HEADER_IDX]           = TCP_PKT_HEADER;
  // Add Payload Size
  tPacket[TCP_PKT_PAYLOAD_SIZE_IDX]     = (json_Len >> 8) & 0xFF;
  tPacket[TCP_PKT_PAYLOAD_SIZE_IDX + 1] = (json_Len) & 0xFF;
  // Add Payload Data
  memcpy(&tPacket[TCP_PKT_PAYLOAD_DATA_IDX], tJson, json_Len);
  // Calculate and add CRC of data
  tPacket[json_Len + 3] = crc8(&tPacket[TCP_PKT_PAYLOAD_DATA_IDX], json_Len);

  // Send data
  CDC_Transmit_FS(tPacket, json_Len + 4);

  for (uint32_t i = 0; i < TCP_TIMEOUT; i++)
  {
    if (tcpData_CB.State != TCP_DATA_CB_STATE_ERR)
    {
      // Change state callback packet
      tcpData_CB.State = TCP_DATA_CB_STATE_ERR;

      // Create json value
      JSON_Value *smRoot_Value;
      const char *tCommand = NULL;
      // Parse to string
      smRoot_Value = json_parse_string((char *)tcpData_CB.Data);
      tCommand = json_object_get_string(json_object(smRoot_Value), "Data");
      if (strcmp(tCommand, "TCP_receive_data") == 0)
      {
        // get status
				const char *tStatus = NULL;
        const char *tValueData = NULL;
        uint16_t tLen;
        tStatus = json_object_get_string(json_object(smRoot_Value), "Status");
        tValueData = json_object_dotget_string(json_object(smRoot_Value), "Value.Data");
        tLen = (uint16_t)json_object_dotget_number(json_object(smRoot_Value), "Value.Len");

        // Copy data to response
        memcpy(Receive_Data, tValueData, tLen);

        if (strcmp(tStatus, "OK") == 0)
          return TCP_STATUS_OK;
        else 
          return TCP_STATUS_ERR;
      }
      else 
      {
        return TCP_STATUS_ERR;
      }
    }
  }

  return TCP_STATUS_ERR_TIMEOUT;
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
  // Create json value
  JSON_Value* root_value = json_value_init_object();
  JSON_Object* root_object = json_value_get_object(root_value);

  json_object_set_string(root_object, "Cmd", "TCP_close_connection");
  // Parse to string
  char* tJson = NULL;
  tJson = json_serialize_to_string_pretty(root_value);

  uint16_t json_Len = strlen(tJson);
  uint8_t tPacket[json_Len + 4];

  // Add header
  tPacket[TCP_PKT_HEADER_IDX]           = TCP_PKT_HEADER;
  // Add Payload Size
  tPacket[TCP_PKT_PAYLOAD_SIZE_IDX]     = (json_Len >> 8) & 0xFF;
  tPacket[TCP_PKT_PAYLOAD_SIZE_IDX + 1] = (json_Len) & 0xFF;
  // Add Payload Data
  memcpy(&tPacket[TCP_PKT_PAYLOAD_DATA_IDX], tJson, json_Len);
  // Calculate and add CRC of data
  tPacket[json_Len + 3] = crc8(&tPacket[TCP_PKT_PAYLOAD_DATA_IDX], json_Len);

  // Send data
  CDC_Transmit_FS(tPacket, json_Len + 4);

  for (uint32_t i = 0; i < TCP_TIMEOUT; i++)
  {
    if (tcpData_CB.State != TCP_DATA_CB_STATE_ERR)
    {
      // Change state callback packet
      tcpData_CB.State = TCP_DATA_CB_STATE_ERR;
      
      // Create json value
      JSON_Value *smRoot_Value;
      const char *tCommand = NULL;
      // Parse to string
      smRoot_Value = json_parse_string((char *)tcpData_CB.Data);
      tCommand = json_object_get_string(json_object(smRoot_Value), "Data");
      if (strcmp(tCommand, "TCP_close_connection") == 0)
      {
        // get status
				const char *tStatus = NULL;
        tStatus = json_object_get_string(json_object(smRoot_Value), "Status");

        if (strcmp(tStatus, "OK") == 0)
        {
          return TCP_STATUS_OK;
        }
        else 
        {
          return TCP_STATUS_ERR;
        }
      }
      else 
      {
        return TCP_STATUS_ERR;
      }
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
