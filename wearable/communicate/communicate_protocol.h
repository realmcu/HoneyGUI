/**
*********************************************************************************************************
*               Copyright(c) 2014, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file         communicate_protocol.c
* @brief       communicate_protocol implementation
* @details   communicate_protocol implementation
* @author
* @date      2014-12-29
* @version   v0.1
* *********************************************************************************************************
*/

#ifndef __COMMUNICATE_PROTOCOL_H__
#define __COMMUNICATE_PROTOCOL_H__


#ifdef __cplusplus
extern "C" {
#endif



#include <stdbool.h>
#include "stdint.h"



#define GLOBAL_RECEIVE_BUFFER_SIZE 244
#define GLOBAL_RESPONSE_BUFFER_SIZE 244

/**************************************************************************
* Note:
* big_endian in communication protocol
* without considering Byte padding
* use constant shift
***************************************************************************/

/******************* Macro defination *************************************/
#define L1_HEADER_MAGIC  (0xAB)     /*header magic number */
#define L1_HEADER_VERSION (0x00)     /*protocol version */
#define L1_HEADER_SIZE   (8)      /*L1 header length*/

/**************************************************************************
* define L1 header byte order
***************************************************************************/
#define L1_HEADER_MAGIC_POS             (0)
#define L1_HEADER_PROTOCOL_VERSION_POS  (1)
#define L1_PAYLOAD_LENGTH_HIGH_BYTE_POS (2)         /* L1 payload lengh high byte */
#define L1_PAYLOAD_LENGTH_LOW_BYTE_POS  (3)
#define L1_HEADER_CRC16_HIGH_BYTE_POS   (4)
#define L1_HEADER_CRC16_LOW_BYTE_POS    (5)
#define L1_HEADER_SEQ_ID_HIGH_BYTE_POS  (6)
#define L1_HEADER_SEQ_ID_LOW_BYTE_POS   (7)


/********************************************************************************
* define version response
*********************************************************************************/
typedef enum
{
    DATA_PACKAGE = 0,
    RESPONSE_PACKAGE =  1,
} L1_PACKAGE_TYPE;

/********************************************************************************
* define ack or nak
*********************************************************************************/
typedef enum
{
    ACK = 0,
    NAK = 1,
} L1_ERROR_FLAG;


#define L2_HEADER_SIZE          (2)      /*L2 header length*/
#define L2_HEADER_VERSION       (0x00)     /*L2 header version*/
#define L2_KEY_SIZE             (1)
#define L2_PAYLOAD_HEADER_SIZE  (3)        /*L2 payload header*/

#define L2_FIRST_VALUE_POS      (L2_HEADER_SIZE + L2_PAYLOAD_HEADER_SIZE)

/******************* Enum & Struct defination ******************************/
/*L1 version defination */
typedef struct
{
    uint8_t version   : 4;
    uint8_t ack_flag  : 1;
    uint8_t err_flag  : 1;
    uint8_t reserve   : 2;
}
L1_version_def_t;

typedef union
{
    L1_version_def_t version_def;
    uint8_t value;
} L1_version_value_t;


typedef enum
{
    NONE    = 0x00,
    IOS     = 0x01,
    ANDROID = 0x02,
} PHONE_OS_VERSION;



typedef enum
{
    KEY_WEATHER_GET    = 0x01,
    KEY_WEATHER_CURRENT = 0x02,
    KEY_WEATHER_FUTURE_HOUR = 0x03,
    KEY_WEATHER_FUTURE_DAY  = 0x04,
} WEATHER_KEY;

typedef enum
{
    WEATHER_SUCCESS    = 0x0,
    WEATHER_CITY_INVALID = 0x01,
    WEATHER_SERVICE_INVALID = 0x02,
} WEATHER_ERROR_CODE;



/* time bit field */
typedef struct
{
    uint32_t seconds    :   6;
    uint32_t minute     :   6;
    uint32_t hours      :   5;
    uint32_t day        :   5;
    uint32_t month      :   4;
    uint32_t year       :   6;
}
time_bit_field_type_t;

typedef union
{
    uint32_t data;
    time_bit_field_type_t time;
} time_union_t;
/* time bit field */

struct protocol_pack
{
    uint8_t l1_magic;
    uint8_t l1_version;
    uint16_t l1_length;
    uint16_t l1_crc;
    uint16_t l1_seq_id;
    uint8_t l2_cmd_id;
    uint8_t l2_version;
    uint8_t l2_key;
    uint16_t l2_lenght;
    uint8_t l2_payload[32];
};


/******************* Function definition **********************************/
void package_prepare_send(struct protocol_pack *package);
void communicate_protocol_init(void);
bool L1_send(uint8_t *buf, uint16_t length);
void resolve_remote_data(uint8_t *data, uint16_t length);


#ifdef __cplusplus
}
#endif

#endif //__COMMUNICATE_PROTOCOL_H__