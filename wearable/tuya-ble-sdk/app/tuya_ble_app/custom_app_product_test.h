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

#ifndef __CUSTOM_APP_PRODUCT_TEST_H__
#define __CUSTOM_APP_PRODUCT_TEST_H__

#include "tuya_ble_type.h"

#define TY_FTM_PROTOCOL_FIRST_HEAD           ( 0x66 )
#define TY_FTM_PROTOCOL_SECOND_HEAD          ( 0xAA )
#define TY_FTM_PROTOCOL_VERSION              ( 0x00 )

#define TY_FTM_UART_BAUD_RATE                ( 9600 )

/* command set */
#define TY_FTM_PCBA_CMD_SET                  ( 0xF0 ) //!< PCBA prod test cmds 
#define TY_FTM_SIMULATION_CMD_SET            ( 0xF1 ) //!< Simulator test cmds 


/* product test sub cmd */
#define TY_FTM_PCBA_CMD_ENTER                ( 0xFF02 )
#define TY_FTM_PCBA_CMD_EXIT                 ( 0xFF03 )
#define TY_FTM_PCBA_CMD_LED                  ( 0x0001 )
#define TY_FTM_PCBA_CMD_RELAY                ( 0x0002 )
#define TY_FTM_PCBA_CMD_KEY                  ( 0x0003 )
#define TY_FTM_PCBA_CMD_BUTTON_SENSOR        ( 0x0004 )
#define TY_FTM_PCBA_CMD_RETAIN               ( 0x0005 )
#define TY_FTM_PCBA_CMD_ANALOG_SENSOR        ( 0x0006 )
#define TY_FTM_PCBA_CMD_MOTOR                ( 0x0007 )
#define TY_FTM_PCBA_CMD_BATTERY_PARAM        ( 0x0008 )
#define TY_FTM_PCBA_CMD_BATTERY_CALIBRATION  ( 0x0009 )
#define TY_FTM_PCBA_CMD_LED_RGBCW            ( 0x000A )
#define TY_FTM_PCBA_CMD_BURNIN               ( 0x000B )
#define TY_FTM_PCBA_CMD_INFRARED_TX          ( 0x000C )
#define TY_FTM_PCBA_CMD_INFRARED_RX_ENTER    ( 0x000D )
#define TY_FTM_PCBA_CMD_INFRARED_RX_EXIT     ( 0x000E )
#define TY_FTM_PCBA_CMD_WRITE_SN             ( 0x000F )
#define TY_FTM_PCBA_CMD_READ_SN              ( 0x0010 )
#define TY_FTM_PCBA_CMD_BLE_RSSI             ( 0x0011 )
#define TY_FTM_PCBA_CMD_WIFI_RSSI            ( 0x0100 )
#define TY_FTM_PCBA_CMD_GSENSOR              ( 0x0012 )
#define TY_FTM_PCBA_CMD_VBAT_ADC             ( 0x0013 )
#define TY_FTM_PCBA_CMD_READ_MAC             ( 0x0014 )
#define TY_FTM_PCBA_CMD_READ_PID             ( 0x0015 )
#define TY_FTM_PCBA_CMD_READ_FW_INFO         ( 0x0016 )
#define TY_FTM_PCBA_CMD_FINGERPRINT          ( 0x0017 )
#define TY_FTM_PCBA_CMD_AUDIO                ( 0x0018 )
#define TY_FTM_PCBA_CMD_SECURITY_CHIP        ( 0x0019 )

typedef enum
{
    PROTOCOL_TYPE_WIFI  = 1,
    PROTOCOL_TYPE_ZIGBEE,
    PROTOCOL_TYPE_BLE,
    PROTOCOL_TYPE_GATAWAY,
    PROTOCOL_TYPE_IPC,
} ENUM_PROTOCOL_TYPE;

typedef enum
{
    FTM_CHANNEL_UART = 0,
    FTM_CHANNEL_BLE,
} ENUM_FTM_CHANNEL;


typedef struct
{
    bool entry;
    bool timeout;
    ENUM_FTM_CHANNEL channel;
    uint16_t last_cmd;

    uint16_t repeat_item;
    uint16_t repeat_cnts;
} entry_ftm_pcba_stu_t;


#ifdef __cplusplus
extern "C" {
#endif

extern tuya_ble_status_t tuya_ble_prod_beacon_scan_start(void);
extern tuya_ble_status_t tuya_ble_prod_beacon_scan_stop(void);
extern tuya_ble_status_t tuya_ble_prod_beacon_get_rssi_avg(int8_t *rssi);
extern tuya_ble_status_t tuya_ble_prod_gpio_test(void);
extern void tuya_ble_custom_app_production_test_process(uint8_t channel, uint8_t *p_in_data,
                                                        uint16_t in_len);

extern void tuya_ble_ftm_init(void);

#ifdef __cplusplus
}
#endif

#endif // __CUSTOM_APP_PRODUCT_TEST_H__
