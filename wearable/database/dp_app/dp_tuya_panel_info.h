/**
*********************************************************************************************************
*               Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      datebse.h
* @brief
* @details
* @author
* @date
* @version
* *********************************************************************************************************
*/

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __DP_TUYA_PANEL_INFO_H__
#define __DP_TUYA_PANEL_INFO_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*============================================================================*
 *                        Header Files
*============================================================================*/
#include "stdbool.h"
#include "stdint.h"
#include "dp_sports_refs.h"


#define KVDB_TUYA_PANEL_INFO_NAME "tuya_panel_info"

/*tuya_cfg key*/
#define KEY_TUYA_PANEL_INFO_BRIGHT_SCREEN    "dp_bright_screen"
#define KEY_TUYA_PANEL_INFO_SCREEN_BRIGHT_VALUE    "dp_screen_bright_value"
#define KEY_TUYA_PANEL_INFO_BRIGHT_SET    "dp_night_bright_set"
#define KEY_TUYA_PANEL_INFO_DIAL_SYNC    "dp_dial_sync"
#define KEY_TUYA_PANEL_INFO_DIAL_OPERATION    "dp_dial_operation"
#define KEY_TUYA_PANEL_INFO_CUSTOM_SCREEN_SET    "dp_custom_screen_set"
#define KEY_TUYA_PANEL_INFO_SCREEN_TIME_SET    "dp_screen_time_set"
#define KEY_TUYA_PANEL_INFO_PHOTO_MODE    "dp_photo_mode"
#define KEY_TUYA_PANEL_INFO_LANGUAGE    "dp_language"
/*tuya_cfg key*/


typedef struct
{
    uint8_t  night_bright_switch;
    uint8_t  night_bright_start_hour;
    uint8_t  night_bright_start_min;
    uint8_t  night_bright_stop_hour;
    uint8_t  night_bright_stop_min;
    uint8_t  night_bright_light_value;

} T_DP_NIGHT_BRIGHT_SET;

typedef struct
{
    uint8_t  dial_sync_handle;
    uint8_t  dial_sync_dial1;
    uint8_t  dial_sync_dial2;
    uint8_t  dial_sync_dial3;
//defination not defined
} T_DP_DIAL_SYNC;

typedef struct
{
    uint8_t  dial_oper_handle;
    uint16_t  dial_oper_id;

} T_DP_DIAL_OPERATION;

typedef struct
{
    bool dp_bright_screen;
    uint8_t dp_screen_bright_value;
    T_DP_NIGHT_BRIGHT_SET dp_night_bright_set;
    T_DP_DIAL_SYNC dp_dial_sync;
    T_DP_DIAL_OPERATION dp_dial_operation;
    void *dp_custom_screen_set;
    uint8_t  dp_screen_time_set;
    uint8_t  dp_photo_mode;
    uint8_t  dp_language;
} T_TUYA_PANEL_INFO;

int dp_tuya_panelinfo_flashDB_init(void);


#ifdef __cplusplus
}
#endif

#endif /* __DP_TUYA_PANEL_INFO_H__ */
