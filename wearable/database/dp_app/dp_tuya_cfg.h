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
#ifndef __DP_TUYS_CFG_H__
#define __DP_TUYS_CFG_H__

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


#define KVDB_TUYA_CFG_NAME "tuya_cfg"

/*tuya_cfg key*/
#define KEY_TUYA_CFG_INFO_SET    "dp_info_set"
#define KEY_TUYA_CFG_BATTERY_PERCENTAGE    "dp_battery_percentage"
#define KEY_TUYA_CFG_CLOCK_SET    "dp_clock_set"
#define KEY_TUYA_CFG_DRINKING_SET    "dp_drinking_set"
#define KEY_TUYA_CFG_HANDWASH_SET    "dp_handwash_set"
#define KEY_TUYA_CFG_TIME_MODE    "dp_time_mode"
#define KEY_TUYA_CFG_ALARM_SWITCH    "dp_alarm_switch"
#define KEY_TUYA_CFG_MENSTRUAL_CYCLE_SET    "dp_menstrual_cycle_set"
#define KEY_TUYA_CFG_NOTIFY_SET    "dp_notify_set"
/*tuya_cfg key*/


typedef struct
{
    uint8_t  weight;   //unit cm
    uint8_t  height;   //unit kg
    uint8_t  age;
    uint8_t  gender;   // 0-female, 1-male
} T_DP_INFO_SET;

typedef struct
{
    uint8_t clock1_hour;
    uint8_t clock1_min;
    uint8_t clock1_repeat;
    uint8_t clock1_mode;
    uint8_t clock1_enable;

    uint8_t clock2_hour;
    uint8_t clock2_min;
    uint8_t clock2_repeat;
    uint8_t clock2_mode;
    uint8_t clock2_enable;

    uint8_t clock3_hour;
    uint8_t clock3_min;
    uint8_t clock3_repeat;
    uint8_t clock3_mode;
    uint8_t clock3_enable;

    uint8_t clock4_hour;
    uint8_t clock4_min;
    uint8_t clock4_repeat;
    uint8_t clock4_mode;
    uint8_t clock4_enable;

    uint8_t clock5_hour;
    uint8_t clock5_min;
    uint8_t clock5_repeat;
    uint8_t clock5_mode;
    uint8_t clock5_enable;

} T_DP_CLOCK_SET;


typedef struct
{
    uint8_t  drinking_set_switch;

    uint8_t  drinking_set_start_hour;
    uint8_t  drinking_set_start_min;
    uint8_t  drinking_set_stop_hour;
    uint8_t  drinking_set_stop_min;
    uint16_t drinking_set_interval;

} T_DP_DRINKING_SET;

typedef struct
{
    uint8_t  handwash_set_switch;

    uint8_t  handwash_set_start_hour;
    uint8_t  handwash_set_start_min;
    uint8_t  handwash_set_stop_hour;
    uint8_t  handwash_set_stop_min;
    uint16_t handwash_set_interval;

} T_DP_HANDWASH_SET;

typedef struct
{
    uint8_t  menstrual_cycle_switch;
    uint8_t  menstrual_cycle_remainder;
    uint8_t  menstrual_cycle_ouvlation_remainder;
    uint8_t  menstrual_cycle_remtime_hour;
    uint8_t  menstrual_cycle_remtime_min;
    uint16_t  menstrual_cycle_lasttime_y;
    uint8_t  menstrual_cycle_lasttime_m;
    uint8_t  menstrual_cycle_lasttime_d;
    uint8_t  menstrual_cycle_time_duration;
    uint8_t  menstrual_cycle_time;

} T_DP_MENSTRUAL_CYCLE_SET;

typedef union
{
    uint8_t data[16];
    struct
    {
        uint8_t  notify_switch     : 4;
        uint8_t  in_call           : 4;
        uint8_t  message           : 4;
        uint8_t  qq                : 4;
        uint8_t  wechat            : 4;
        uint8_t  facebook          : 4;
        uint8_t  messenger         : 4;
        uint8_t  twitter           : 4;
        uint8_t  whatsapp          : 4;
        uint8_t  instagram         : 4;
        uint8_t  likedin           : 4;
        uint8_t  viber             : 4;
        uint8_t  line              : 4;
        uint8_t  skype             : 4;
        uint8_t  outlook           : 4;
        uint8_t  watchapp_Bus      : 4;
        uint8_t  Email             : 4;
        uint8_t  Calendar          : 4;
        uint8_t  Kakaotalk         : 4;
        uint8_t  VKontakte         : 4;
        uint8_t  Tumblr            : 4;
        uint8_t  Snapchat          : 4;
        uint8_t  Telegram          : 4;
        uint8_t  Youtube           : 4;
        uint8_t  Pinterest         : 4;
        uint8_t  Tiktok            : 4;
        uint8_t  Gmail             : 4;
        uint8_t  def_1             : 4;
    } notify_type;

} T_DP_NOTIFY_SET;


typedef struct
{
    T_DP_INFO_SET dp_info_set;
    uint8_t dp_battery_percentage;
    T_DP_CLOCK_SET dp_clock_set;
    T_DP_DRINKING_SET dp_drinking_set;
    T_DP_HANDWASH_SET dp_handwash_set;
    uint8_t dp_time_mode;
    bool dp_alarm_switch;
    T_DP_MENSTRUAL_CYCLE_SET dp_menstrual_cycle_set;
    T_DP_NOTIFY_SET dp_notify_set;
} T_TUYA_CONFIG;


int dp_tuya_cfg_flashDB_init(void);


#ifdef __cplusplus
}
#endif

#endif /* __DP_TUYS_CFG_H__ */
