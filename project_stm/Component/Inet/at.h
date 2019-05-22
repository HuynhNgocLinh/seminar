/*!
 *  @file		    AT.h
 *  @author  	  Linh Huynh
 *	@copyright	
 *  @version 	  1.00.00.20190420
 *  @date    	  April 20 , 2019
 *  @note
 */
#ifndef __AT_H__
#define __AT_H__

#ifdef __cplusplus
extern "C"{
#endif
/*******************************************************************************
 *  INCLUDES
 ******************************************************************************/
#include "usbd_cdc_if.h"
/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/
/*******************************************************************************
 * TYPEDEF
 ******************************************************************************/
/**
 * @brief
 */
typedef enum 
{
  AT_STATUS_OK,
  AT_STATUS_ERR_NULL_PTR,
  AT_STATUS_ERR_OPTION
} at_status_t;

/**
 * @brief
 */
typedef enum 
{
  AT_TCP_CONNECTION_STATUS_CONNECTED,
  AT_TCP_CONNECTION_STATUS_DISCONNECTED
} at_tcp_connection_status_t;

typedef char     at_tcp_addr_t;
typedef char     at_tcp_type_t;
typedef uint16_t at_tcp_port_t;
typedef uint8_t  at_tcp_data_t;
typedef uint8_t  at_tcp_length_t;

/**
 * @brief
 */
typedef struct 
{
  at_tcp_connection_status_t status;
  at_tcp_addr_t              ip_addr[6];
  at_tcp_data_t              port;
} 
at_tcp_info_connection_t;

/**
 * @brief
 */


/**
 * @brief
 */
/*******************************************************************************
 * FUNCTIONS - API
 ******************************************************************************/
at_status_t at_init();

void at_loop();

at_status_t at_tcp_get_info_connection(at_tcp_info_connection_t *info_connection);

at_status_t tcp_establish_connection(at_tcp_addr_t *ip_addr,
                                     at_tcp_port_t     port);

at_status_t tcp_send_data(at_tcp_data_t *data, at_tcp_length_t len);

at_status_t tcp_receive_data(at_tcp_data_t *data, at_tcp_length_t *len);

at_status_t tcp_close_connection();




#ifdef __cplusplus
}
#endif

#endif // __AT_H__