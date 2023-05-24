/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#ifndef __TUYA_BLE_SDK_TEST_H__
#define __TUYA_BLE_SDK_TEST_H__


/*********************************************************************
 * INCLUDE
 */
#include "tuya_ble_stdlib.h"
#include "tuya_ble_type.h"

#define TUYA_BLE_SDK_TEST_ENABLE           ( 0 )

/*********************************************************************
 * CONSTANT
 */
#define TY_UARTV_CMD_SHAKE_HAND             0x0000
#define TY_UARTV_CMD_SET_PID                0x0001
#define TY_UARTV_CMD_GET_PID                0x0002
#define TY_UARTV_CMD_SET_AUTH_INFO          0x0003
#define TY_UARTV_CMD_GET_AUTH_INFO          0x0004
#define TY_UARTV_CMD_SET_VERSION            0x0005
#define TY_UARTV_CMD_GET_VERSION            0x0006
#define TY_UARTV_CMD_SET_ADV_INTERVAL       0x0007
#define TY_UARTV_CMD_GET_ADV_INTERVAL       0x0008
#define TY_UARTV_CMD_SET_CONN_INTERVAL      0x0009
#define TY_UARTV_CMD_GET_CONN_INTERVAL      0x000A
#define TY_UARTV_CMD_PASSTHROUGH_REPORT     0x000B
#define TY_UARTV_CMD_TIME_SYNC              0x000C
#define TY_UARTV_CMD_DP_REPORT              0x000D
#define TY_UARTV_CMD_DP_REPORT_PERIOD       0x000E
#define TY_UARTV_CMD_SET_BULKDATA           0x000F
#define TY_UARTV_CMD_SET_CONN_REQUEST       0x0010
#define TY_UARTV_CMD_SET_DEVICE_RESET       0x0011
#define TY_UARTV_CMD_SET_SLEEP_MODE         0x0012

#define TY_UARTV_CMD_GET_DEVICE_STATE       0x0020
#define TY_UARTV_CMD_GET_TIME               0x0021
#define TY_UARTV_CMD_GET_WEATHER            0x0022
#define TY_UARTV_CMD_GET_RAND               0x0023

#define TY_UARTV_CMD_DP_WRITE               0x0040
#define TY_UARTV_CMD_DP_RSP                 0x0041
#define TY_UARTV_CMD_UNBIND_MODE            0x0042
#define TY_UARTV_CMD_PASSTHROUGH_WRITE      0x0043

#define TY_UARTV_CMD_PWM                    0x0060
#define TY_UARTV_CMD_ADC                    0x0061
#define TY_UARTV_CMD_SPI                    0x0062
#define TY_UARTV_CMD_IIC                    0x0063

/*********************************************************************
 * STRUCT
 */
#pragma pack(1)
typedef struct
{
    uint16_t sof;
    uint8_t  version;
    uint8_t  id;
    uint16_t len;
    uint8_t  type;
    uint16_t sub_id;
    uint8_t  value[];
} tuya_ble_sdk_test_cmd_t;
#pragma pack()

/*********************************************************************
 * EXTERNAL VARIABLE
 */

/*********************************************************************
 * EXTERNAL FUNCTION
 */

#ifdef __cplusplus
extern "C"
{
#endif

void tuya_ble_sdk_test_init(void);
uint32_t tuya_ble_sdk_test_send(uint16_t cmd, uint8_t *buf, uint16_t size);
void tuya_ble_sdk_test_wake_up_handler(void);
void tuya_ble_sdk_test_dp_report_handler(void);
void tuya_ble_sdk_test_get_time_rsp(tuya_ble_time_noraml_data_t *data);
void tuya_ble_sdk_test_unbind_mode_rsp(uint8_t mode);

#ifdef __cplusplus
}
#endif

#endif //__TUYA_BLE_SDK_TEST_H__
