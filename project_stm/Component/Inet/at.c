/*!
 *  @file		    AT.c
 *  @author  	  Linh Huynh
 *	@copyright	
 *  @version 	  1.00.00.20190420
 *  @date    	  April 20 , 2019
 *  @note
 */
/*******************************************************************************
 *  INCLUDES
 ******************************************************************************/
#include "at.h"
#include "cbuffer.h"
#include "usbd_cdc_if.h"
#include "usb_device.h"
/*******************************************************************************
 * MACRO
 ******************************************************************************/
/*******************************************************************************
 * CONST
 ******************************************************************************/
#define TCP_HEADER (0x55)
#define AT_DEFAULT_PKT_LEN (6)
/*******************************************************************************
 * VARIABLE
 ******************************************************************************/
static CBuffer_T cbuffer_handle;
static uint8_t cbuffer_data[1024];
/*******************************************************************************
 * TYPEDEF 
 ******************************************************************************/
/*******************************************************************************
 * ENUM
 ******************************************************************************/
enum 
{
  USB_GET_INFO = 0          ,
  USB_ESTABLISH_CONNECTION  ,
  USB_SEND_DATA             ,
  USB_RECEIVE_DATA          ,
  USB_CLOSE_CONNECTION      , 
  USB_MAX
};

enum 
{
  usb_type_tCP              ,
  USB_TYPE_UDP              ,
  USB_TYPE_MAX
};

enum {
  USB_REP_OK                ,
  USB_REP_ERROR             ,
	USB_REP_CONNECT_OK        ,
  USB_REP_SEND_OK           ,
  USB_REP_MAX
};

enum  
{
  USB_STATUS_IDX  = 0       ,
  USB_IP_ADDR_IDX = 1       ,
  USB_TYPE_IDX    = 16      ,
  USB_PORT_IDX    = 19      
};

typedef enum 
{
  AT_STATE_GET_HEADER,
  AT_STATE_GET_TYPE,
  AT_STATE_GET_CMD,
  AT_STATE_GET_PAYLOAD_SIZE,
  AT_STATE_GET_PAYLOAD_DATA,
  AT_STATE_MAX
} at_state_t;

typedef enum 
{
  AT_PKT_IDX_HEADER = 0,
  AT_PKT_IDX_TYPE   = 1,
  AT_PKT_IDX_CMD    = 2,
  AT_PKT_IDX_LEN    = 3,
  AT_PKT_IDX_DATA   = 5, 
} at_pkt_idx_t;

typedef enum 
{
  AT_TYPE_REQ,
  AT_TYPE_RES,
} at_type_t;

typedef enum 
{
  AT_CMD_TCP_CHECK_CONNECTION_STATUS,
  AT_CMD_TCP_ESTAB_CONNECTION,
  AT_CMD_TCP_SEND_DATA,
  AT_CMD_TCP_REC_DATA,
  AT_CMD_TCP_CLOSE_CONNECTION,
  AT_CMD_MAX
} at_cmd_t;

typedef struct  
{
	uint8_t Header;
	at_type_t Type;
	at_cmd_t Cmd;
	uint8_t Len[2];
	uint8_t Data[1000];
} tcp_packet_t;
/*******************************************************************************
 * VARIABLE
 ******************************************************************************/
at_state_t at_state = AT_STATE_GET_HEADER;
tcp_packet_t at_packet;
/*******************************************************************************
 * FUNCTIONS - Callback
 ******************************************************************************/
void USB_receive_callback_fnx(uint8_t *buf, uint32_t len)
{
  CBuffer_Write(&cbuffer_handle, buf, len);
}
/*******************************************************************************
 * FUNCTIONS - LOCAL
 ******************************************************************************/
at_status_t at_transfer(uint8_t *tx_buf, 
                        uint8_t tx_len, 
                        uint8_t *rx_buf, 
                        uint8_t rx_len,
                        uint16_t timeout);
/*******************************************************************************
 * FUNCTIONS - API
 ******************************************************************************/
at_status_t at_init()
{
  MX_USB_DEVICE_Init();

  CBuffer_Init(&cbuffer_handle, cbuffer_data, sizeof(cbuffer_data));
}

/******************************************************************************/
void at_loop()
{
  if (CBuffer_DataCount(&cbuffer_handle) != 0)
  {
    switch (at_state)
    {
      case AT_STATE_GET_HEADER:
      {

      }
      break;
      case AT_STATE_GET_TYPE:
      {

      }
      break;
      case AT_STATE_GET_CMD:
      {

      }
      break;
      case AT_STATE_GET_PAYLOAD_SIZE:
      {

      }
      break;
      case AT_STATE_GET_PAYLOAD_DATA:
      {
        switch (at_packet.Cmd)
        {
          case AT_CMD_TCP_CHECK_CONNECTION_STATUS:
          case AT_CMD_TCP_ESTAB_CONNECTION:
          case AT_CMD_TCP_SEND_DATA:
          case AT_CMD_TCP_REC_DATA:
          case AT_CMD_TCP_CLOSE_CONNECTION:
          default: break;
        }
      }
      break;
      default: break;
    }
  }
}

/******************************************************************************/

at_status_t at_tcp_get_info_connection(at_tcp_info_connection_t *info_connection)
{
  uint8_t t_packet[AT_DEFAULT_PKT_LEN];
  uint8_t rx_buf[100];

  at_status_t m_status;

  uint8_t tData[] = NULL;

  t_packet[AT_PKT_IDX_HEADER]  = TCP_HEADER;
  t_packet[AT_PKT_IDX_TYPE]    = AT_TYPE_REQ;
  t_packet[AT_PKT_IDX_CMD]     = AT_CMD_TCP_CHECK_CONNECTION_STATUS;
  t_packet[AT_PKT_IDX_LEN]     = 0x00;
  t_packet[AT_PKT_IDX_LEN + 1] = 0x00;
  t_packet[AT_PKT_IDX_DATA]    = 0x00;

  // send packet
  at_transfer(t_packet, sizeof(t_packet), rx_buf, sizeof(rx_buf), 1000);

  // Check response
  //TODO: Need review
  info_connection->status     = rx_buf[0];
  info_connection->ip_addr[0] = rx_buf[1];
  info_connection->port       = rx_buf[7];
}

/******************************************************************************/
usb_tatus_t usb_establish_tcp_connection(usb_ip_addr_t *ip_addr,
                                            usb_port_t     port)
{
  char tx_buf[USB_TX_LEN];
  char rx_buf[USB_RX_LEN];
  char space_buf[] = ",";

  uint8_t tx_len = strlen(USB_at_command[USB_ESTABLISH_CONNECTION]);
  uint8_t rx_len = strlen(USB_rep[USB_REP_OK]);


  // Copy command
  memcpy(tx_buf, USB_at_command[USB_ESTABLISH_CONNECTION], tx_len);
  // Copy type
  tx_len = strlen(USB_type[usb_type_tCP]);
  memcpy(&tx_buf[strlen(tx_buf)], USB_type[usb_type_tCP], tx_len);
  // Copy string ","
  memcpy(&tx_buf[strlen(tx_buf)], space_buf, strlen(space_buf));
  // Copy ip address
  memcpy(&tx_buf[strlen(tx_buf)], ip_addr, strlen(ip_addr));
  // Copy string ","
  memcpy(&tx_buf[strlen(tx_buf)], space_buf, strlen(space_buf));
  // Copy port
  sprintf(&tx_buf[strlen(tx_buf)], "%d", port);

  // Send command 
  USB_transfer((uint8_t *)tx_buf, strlen(tx_buf), NULL, rx_len, 1000);
  
  // Check response
  if (strstr(rx_buf, USB_rep[USB_REP_OK]) != NULL && 
      strstr(rx_buf, USB_rep[USB_REP_CONNECT_OK]) != NULL)
    return USB_STATUS_OK;
  else 
    return USB_STATUS_ERR_OPTION;


}

/******************************************************************************/
usb_tatus_t USB_establish_udp_connection(usb_ip_addr_t *ip_addr,
                                            usb_port_t     port)
{
  char tx_buf[USB_TX_LEN];
  char rx_buf[USB_RX_LEN];
  char space_buf[] = ",";

  uint8_t tx_len = strlen(USB_at_command[USB_ESTABLISH_CONNECTION]);
  uint8_t rx_len = strlen(USB_rep[USB_REP_OK]);

  // Copy command
  memcpy(tx_buf, USB_at_command[USB_ESTABLISH_CONNECTION], tx_len);
  // Copy type
  tx_len = strlen(USB_type[USB_TYPE_UDP]);
  memcpy(&tx_buf[strlen(tx_buf)], USB_type[USB_TYPE_UDP], tx_len);
  // Copy string ","
  memcpy(&tx_buf[strlen(tx_buf)], space_buf, strlen(space_buf));
  // Copy ip address
  memcpy(&tx_buf[strlen(tx_buf)], ip_addr, strlen(ip_addr));
  // Copy string ","
  memcpy(&tx_buf[strlen(tx_buf)], space_buf, strlen(space_buf));
  // Copy port
  sprintf(&tx_buf[strlen(tx_buf)], "%d", port);

  // Send command 
  USB_transfer((uint8_t *)tx_buf, strlen(tx_buf), NULL, rx_len, 1000);
  
  // Check response
  if (strstr(rx_buf, USB_rep[USB_REP_OK]) != NULL && 
      strstr(rx_buf, USB_rep[USB_REP_CONNECT_OK]) != NULL)
    return USB_STATUS_OK;
  else 
    return USB_STATUS_ERR_OPTION;
}                                            

usb_tatus_t USB_send_data(usb_data_t *data, usb_length_t len)
{
  char tx_buf[USB_TX_LEN];
  char rx_buf[USB_RX_LEN];
  char enter_buf[] = "\n>\n";
  
  uint8_t tx_len = strlen(USB_at_command[USB_SEND_DATA]);
  uint8_t rx_len = strlen(USB_rep[USB_REP_SEND_OK]);

  // Copy command
  memcpy(tx_buf, USB_at_command[USB_SEND_DATA], tx_len);  
  // Copy enter char
  memcpy(&tx_buf[strlen(tx_buf)], enter_buf, strlen(enter_buf));
  // Copy data
  memcpy(&tx_buf[strlen(tx_buf)], data, len);

  // Send command 
  USB_transfer((uint8_t *)tx_buf, strlen(tx_buf), NULL, rx_len, 1000);

  if (strstr(rx_buf, USB_rep[USB_REP_SEND_OK]) != NULL)
    return USB_STATUS_OK;
  else 
    return USB_STATUS_ERR_OPTION;
}

usb_tatus_t USB_receive_data(usb_data_t *data)
{
  char tx_buf[USB_TX_LEN];
  char rx_buf[1024];

  uint8_t tx_len = strlen(USB_at_command[USB_RECEIVE_DATA]);
	uint8_t rx_len = strlen(USB_rep[USB_REP_SEND_OK]);

  
  // Copy command
  memcpy(tx_buf, USB_at_command[USB_RECEIVE_DATA], tx_len);

  // Send command 
  USB_transfer((uint8_t *)tx_buf, strlen(tx_buf), NULL, rx_len, 1000);

  if (strstr(rx_buf, "<\n") != NULL)
  {
    *data = rx_buf[1];
  }
  else 
  {
    return USB_STATUS_ERR_OPTION;
  }

  return USB_STATUS_OK;
}

usb_tatus_t USB_close_connection()
{
  char tx_buf[USB_TX_LEN];
  char rx_buf[USB_RX_LEN];

  uint8_t tx_len = strlen(USB_at_command[USB_CLOSE_CONNECTION]);
  uint8_t rx_len = strlen(USB_rep[USB_REP_OK]);


  // Copy command
  memcpy(tx_buf, USB_at_command[USB_CLOSE_CONNECTION], tx_len);

  // Send command 
  USB_transfer((uint8_t *)tx_buf, strlen(tx_buf), NULL, rx_len, 1000);
 
  if (strstr(rx_buf, USB_rep[USB_REP_OK]) != NULL)
    return USB_STATUS_OK;
  else 
    return USB_STATUS_ERR_OPTION;
}

at_status_t at_transfer(uint8_t *tx_buf, 
                        uint8_t tx_len, 
                        uint8_t *rx_buf, 
                        uint8_t rx_len,
                        uint16_t timeout);
{
  if (tx_buf)
  {
    CDC_Transmit_FS(tx_buf, tx_len);
  }
  else 
  {
    return USB_STATUS_ERR_NULL_PTR;
  }

  if (rx_buf)
  {
    uint16_t t_len;
    for (uint32_t i = 0; i < timeout; i++)
    {
      t_len = CBuffer_DataCount(&cbuffer_handle);
      if (t_len >= rx_len)
      {
        t_len = rx_len;
        break;
      }
    }
    CBuffer_Read(&cbuffer_handle, rx_buf, t_len);
  }
  
  // Clear buffer 
  CBuffer_Clear(&cbuffer_handle);
  memset(&cbuffer_data, 0x00, sizeof(cbuffer_data));

  return USB_STATUS_OK;
}