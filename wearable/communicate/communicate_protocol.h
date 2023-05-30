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


typedef enum
{
    L1SEND_SPORT_AND_SLEEP_DATA                = 1,
    L1SEND_BOND_FAIL_EVENT                     = 2,
    L1SEND_BOND_SUCCESS_EVENT                  = 3,
    L1SEND_LOGIN_FAIL_EVENT                    = 4,
    L1SEND_LOGIN_SUCCESS_EVENT                 = 5,
    L1SEND_RETURN_ALARM_EVENT                  = 6,
    L1SEND_RETURN_PHONE_CONTROL_CMD_EVENT      = 7,
    L1SEND_RETURN_LIFT_SWITCH_EVENT            = 8,
    L1SEND_RETURN_INCOMMING_MESSAGE_SETTINGS   = 9,
    L1SEND_SIT_SETTING_RETURN                  = 10,
    L1SEND_HISTORY_DATA_SYNC_START             = 11,
    L1SEND_HISTORY_DATA_SYNC_END               = 12,
    L1SEND_SPORT_DATA                          = 13,
    L1SEND_SLEEP_DATA                          = 14,
    L1SEND_HEART_DATA                          = 15,
    L1SEND_BLOODPRESSURE_DATA                  = 16,
    L1SEND_EXERCISE_DATA                       = 17,
    L1SEND_RETURN_CANCEL_HEART_SAMPLE          = 18,
    L1SEND_RETURN_HEART_SETTING                = 19,
    L1SEND_RETURN_CALL_REJECT_COMMAND          = 20,
    L1SEND_RETURN_FIND_MOBILE_COMMAND          = 21,
    L1SEND_RETURN_FUNCTIONS_EVENT              = 22,
    L1SEND_RETURN_CANCEL_BP_SAMPLE             = 23,
    L1SEND_RETURN_HOUR_FORMAT_SETTING          = 24,
    L1SEND_RETURN_DISTANCE_UNIT_SETTING        = 25,
    L1SEND_RETURN_DNDM_SETTING                 = 26,
    L1SEND_RETURN_OLED_DISPLAY_TIME            = 27,
    L1SEND_RETURN_LANGUAGE                     = 28,
    L1SEND_RETURN_TWIST_SWITCH_EVENT           = 29,
    L1SEND_RETURN_SPORT_OFFSETDATA             = 30,
    L1SEND_RETURN_CHARGE_STATUS                = 31,
    L1SEND_RETURN_WEATHER_DATA_GET             = 32,
    L1SEND_RETURN_BBPRO_MAC_ADDR               = 33,
    L1SEND_RETURN_DEVICE_INFO                  = 34,
    L1SEND_RETURN_DAIL_CHANGE                  = 35,
    L1SEND_RETURN_BBPRO_STATE                  = 36,
    L1SEND_RETURN_EXERCISEMODE_EVENT           = 37,
    L1SEND_RETURN_ANCS_INCOMMING_CALL          = 38,
    L1SEND_RETURN_HR_SAMPLE_EVENT              = 39,
    L1SEND_RETURN_BACKLIGHT_EVENT              = 40,
    L1SEND_RETURN_CHARGE_EVENT                 = 41,
    L1SEND_RETURN_PATCH_EVENT                  = 42,
    L1SEND_RETURN_GSENSOR_ID_EVENT             = 43,
    L1SEND_RETURN_HR_ID_EVENT                  = 44,
    L1SEND_RETURN_HALL_EVENT                   = 45,
    L1SEND_RETURN_LOCK                         = 46,
    L1SEND_RETURN_FACTORY_END_EVENT            = 47,
    L1SEND_RETURN_COUNTER_EVENT                = 48,
    L1SEND_RETURN_HIDDEN_FUNC                  = 49,
    L1SEND_RETURN_BBPRO_CONNECTED_STATE        = 50,
    L1SEND_RETURN_BBPRO_CONN_INFO              = 51,
    L1SEND_RETURN_REALTIME_HEART_DATA          = 52,
    L1SEND_RETURN_USER_ID_EVENT                = 53,
    L1SEND_RETURN_DAILY_DATA_CALIBRATION       = 54,
    L1SEND_AUTO_TEST,
    L1SEND_INVALID,
} L1SEND_TYPE_WRISTBAND;

/******************* Function definition **********************************/
void communicate_protocol_init(void);
bool L1_send(uint8_t *buf, uint16_t length);
void L1_receive_data(uint8_t *data, uint16_t length);
void L1_send_event(L1SEND_TYPE_WRISTBAND event, void *res);

#ifdef __cplusplus
}
#endif

#endif //__COMMUNICATE_PROTOCOL_H__
