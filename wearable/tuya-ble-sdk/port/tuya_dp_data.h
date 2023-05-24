/*
 * File      : gui_text.h
 */
#ifndef __TUYA_DP_DATA_H__
#define __TUYA_DP_DATA_H__

#include "stdint.h"
#include <stdbool.h>
#include "tuya_ble_type.h"
#include "tuya_ble_mutli_tsf_protocol.h"
#include "dp_sports_refs.h"
#include "trace.h"
#include "tuya_ble_api.h"
#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
    uint32_t dp_calories_total;

} DP_CALORIES_TOTAL;

typedef struct
{
    uint32_t dp_distance_total;

} DP_DISTANCE_TOTAL;



typedef struct
{
    uint16_t dp_walk_per_min;

} DP_WALK_MIN;

typedef struct
{
    uint16_t dp_cal_per_min;

} DP_CALORIE_MIN;

typedef struct
{
    uint16_t dp_mile_per_min;

} DP_MILEAGE_MIN;

typedef struct
{
    uint8_t  dp_exercise_type;
    uint8_t  dp_exercise_start_year;
    uint8_t  dp_exercise_start_month;
    uint8_t  dp_exercise_start_day;
    uint8_t  dp_exercise_start_hour;
    uint8_t  dp_exercise_start_min;
    uint8_t  dp_exercise_start_sec;
    uint32_t dp_exercise_time_length;
    uint32_t dp_exercise_distance;
    uint32_t dp_exercise_calories;
    uint32_t dp_exercise_walks;
    uint8_t  dp_exercise_max_heartrate;
    uint8_t  dp_exercise_ave_heartrate;
    uint8_t  dp_exercise_min_heartrate;

} DP_EXERCISE_RECORD_RAW;


typedef struct
{
    uint8_t  dp_mes_notify_call;
    uint8_t  dp_mes_mes_qq;
    uint8_t  dp_mes_wechat_facebook;
    uint8_t  dp_mes_messenger_twitter;
    uint8_t  dp_mes_whatsapp_ins;
    uint8_t  dp_mes_linkedin_viber;
    uint8_t  dp_mes_line_skype;
    uint8_t  dp_mes_outlook_watchappb;
    uint8_t  dp_mes_email_calendar;
    uint8_t  dp_mes_kakao_vkontakte;
    uint8_t  dp_mes_tumblr_snapchat;
    uint8_t  dp_mes_telegram_youtube;
    uint8_t  dp_mes_pinterest_tiktok;
    uint8_t  dp_mes_Gmail_null;

} DP_NOTIFICATION_PUSH;


typedef struct
{
    uint8_t  dp_pressure_min;

} DP_PRESSURE_MIN;

typedef struct
{
    uint8_t  dp_stress_set_switch;

    uint8_t  dp_stress_set_start_hour;
    uint8_t  dp_stress_set_start_min;
    uint8_t  dp_stress_set_stop_hour;
    uint8_t  dp_stress_set_stop_min;
    uint8_t  dp_stress_set_interval;
    uint8_t  dp_stress_set_hremaminder_switch;
    uint8_t  dp_stress_set_remaminder_value;

} DP_STRESS_SET;



typedef struct
{
    uint8_t  dp_recog_set_switch;

    uint8_t  dp_recog_run_switch;
//defination not defined
} DP_RECOGNITION_SET;



typedef struct
{
    uint32_t  dp_func_step_offer : 1;
    uint32_t  dp_func_step_choose : 1;
    uint32_t  dp_func_cal_offer : 1;
    uint32_t  dp_func_cal_choose : 1;
    uint32_t  dp_func_mile_offer : 1;
    uint32_t  dp_func_mile_choose : 1;
    uint32_t  dp_func_sleep_offer : 1;
    uint32_t  dp_func_sleep_choose : 1;
    uint32_t  dp_func_sport_offer : 1;
    uint32_t  dp_func_sport_choose : 1;
    uint32_t  dp_func_heartrate_offer : 1;
    uint32_t  dp_func_heartrate_choose : 1;
    uint32_t  dp_func_blood_oxygen_offer : 1;
    uint32_t  dp_func_blood_oxygen_choose : 1;
    uint32_t  dp_func_blood_pressure_offer : 1;
    uint32_t  dp_func_blood_pressure_choose : 1;
    uint32_t  dp_func_temp_offer : 1;
    uint32_t  dp_func_temp_choose : 1;
    uint32_t  dp_func_pressure_offer : 1;
    uint32_t  dp_func_pressure_choose : 1;

} DP_FUNCTION_SET;



typedef struct
{
    uint32_t  dp_exercise_type;

} DP_EXERCISE_TYPE;

typedef struct
{
    uint32_t  dp_duration;

} DP_DURATION;

typedef struct
{
    uint8_t  dp_detection_start;

} DP_DETECTION_START_SELECT;

typedef struct
{
    void  *dp_function_select;

} DP_FUNCTION_SELECT;

typedef struct
{
    uint8_t  dp_distance_unit;
    uint8_t  dp_calories_unit;
    uint8_t  dp_temperature_unit;
    uint8_t  dp_length_unit;
    uint8_t  dp_week_start_time;
    uint8_t  dp_riding_distance;

} DP_PREFERENCES_SET;





typedef struct
{
    void  *dp_custom_sport_set;

} DP_CUSTOM_SPORT_SET;



typedef struct
{
    uint8_t  dp_contact_operate;

} DP_CONTACT_OPERATE;



typedef struct
{
    uint8_t  dp_voice_vol;

} DP_VOICE_VOL;

typedef struct
{
    bool  dp_voice_mic;

} DP_VOICE_MIC;

typedef struct
{
    bool  dp_voice_play;

} DP_VOICE_PLAY;

typedef struct
{
    bool  dp_voice_bt_play;

} DP_VOICE_BT_PLAY;

typedef struct
{
    void  *dp_voice_alarm_clock;

} DP_VOICE_ALARM_CLOCK;

typedef struct
{
    void  *dp_voice_ctrl_group;

} DP_VOICE_CTRL_GROUP;
/**********************
 *      TYPEDEFS
 **********************/
typedef enum
{
    DP_ID_TIME_MODE = 20,
    DP_ID_PHOTO_MODE = 26,
    DP_ID_LANGUAGE = 27,
    DP_ID_BRIGHT_SET = 30,
    DP_ID_EXERICE_CONTROL = 45,
    DP_ID_EXERICE_TYPE = 46,
    DP_ID_DETECTION_START_SELECT = 48,
    DP_ID_EXERICE_START_SELECT = 49,
    DP_ID_FUNCTION_SELECT = 50,
    DP_ID_SCREEN_TIME_SET = 55,
    DP_ID_CONTACT_OPERATE = 56,
    DP_ID_BLUETOOTH_STATE = 57,

} DP_ENUM_ID;

typedef enum
{
    DP_ID_STEPS_TOTAL = 1,//ro
    DP_ID_CALORIES_TOTAL,//ro
    DP_ID_DISTANCE_TOTAL,//ro
    DP_ID_BATTERY_PERCENTAGE,//ro
    DP_ID_SEDENTARY_REMAINDER = 6,
    DP_ID_WALK_MIN = 10,//ro
    DP_ID_CALORIE_MIN,//ro
    DP_ID_MILEEAGE_MIN,
    DP_ID_HEART_RATE_MIN,//ro
    DP_ID_BLOOD_OXYGEN_MIN,
    DP_ID_BLOOD_PRE_HIGH_MIN,//RO
    DP_ID_BODY_TEMP_MIN,
    DP_ID_HEART_RATE = 32,//RO
    DP_ID_BLOOD_OXYGEN,
    DP_ID_PRESSURE_MIN,
    DP_ID_SCREEN_BRIGHT_VALUE = 39,

} DP_VALUE_ID;

typedef enum
{
    DP_ID_TARGET_SET = 5,

    DP_ID_SLEEP_SET = 7,
    DP_ID_CLOCK_SET,
    DP_ID_INFO_SET,
    DP_ID_SLEEP_REPORT = 18,
    DP_ID_EXERCISE_RECORD_RAW,
    DP_ID_HEART_RATE_SET = 21,
    DP_ID_BLOOD_OXYGEN_SET,
    DP_ID_BLOOD_PRE_SET,
    DP_ID_BODYTEMP_SET,
    DP_ID_NOTIFICATION_PUSH,
    DP_ID_STRESS_SET = 35,
    DP_ID_DRINKING_SET,
    DP_ID_HANDWASH_SET,
    DP_ID_RECOGNITION_SET,
    DP_ID_NIGHT_BRIGHT_SET = 40,
    DP_ID_DIAL_SYNC,
    DP_ID_DIAL_OPERATION,
    DP_ID_FUNCTION_SET,
    DP_ID_EXERCISE_REALTIME,
    DP_ID_PERFERENCE_SET = 51,
    DP_ID_MENSTRUAL_CYCLE_SET,
    DP_ID_CUSTOM_SCREEN_SET,
    DP_ID_CUSTOM_SPORT_SET,
    DP_ID_BLUETOOTH_NAME = 59,

} DP_RAW_ID;

void dp_data_classify(tuya_ble_dp_data_received_data_t *receive_data);
void notify_push_parse(tuya_ble_passthrough_data_t receive_data);
void All_dp_data_report(void);
void All_dp_data_init(void);
void dp_data_package(data_info_def *data_buf_in);
#ifdef __cplusplus
}
#endif

#endif

