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
#ifndef __DP_SPORTS_REFS_H__
#define __DP_SPORTS_REFS_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*============================================================================*
 *                        Header Files
*============================================================================*/
#include <time.h>
#include "stdbool.h"
#include "stdint.h"
#include "dp_wearable.h"

/*sports_refs kvdb*/
#define KVDB_SPORT_REFS_NAME "sports_refs"

/*sports_refs key*/
#define KEY_SPORTS_REFS_STEPS_TOTAL "dp_steps_total"
#define KEY_SPORTS_REFS_CALORIES_TOTAL "dp_calories_total"
#define KEY_SPORTS_REFS_DISTANCE_TOTAL "dp_distance_total"
#define KEY_SPORTS_REFS_TARGET_SET "dp_target_set"
#define KEY_SPORTS_REFS_SEDENTARY_REMINDER "dp_sedentary_reminder"
#define KEY_SPORTS_REFS_SLEEP_SET "dp_sleep_set"

/*sports_refs key*/


typedef struct
{
    uint32_t steps_target;
    uint32_t calories_target;
    uint32_t distance_target;
} T_DP_TARGET_SET;

typedef struct
{
    uint8_t sed_switch;
    uint8_t sed_start_hour;
    uint8_t sed_start_min;
    uint8_t sed_stop_hour;
    uint8_t sed_stop_min;
    uint16_t sed_interval;
} T_DP_SEDENTARY_REMINDER;

typedef struct
{
    uint8_t end_min;
    uint8_t end_hour;
    uint8_t start_min;
    uint8_t start_hour;
    uint8_t sleep_set_switch;
} T_DP_SLEEP_SET;

typedef struct
{
    uint32_t dp_steps_total;
    uint32_t dp_calories_total;
    uint32_t dp_distance_total;
    T_DP_TARGET_SET dp_target_set;
    T_DP_SEDENTARY_REMINDER dp_sedentary_reminder;
    T_DP_SLEEP_SET dp_sleep_set;
} T_TUYA_SPORT_REFS;


int dp_sports_refs_flashDB_init(void);
/*sports_refs kvdb*/


/*sport_min_data tsdb*/
#define TSDB_SPORT_MIN_DATA_NAME "sport_min_data"

#define TSDB_SPORT_MIN_DATA_TSL_MAX_LEN 128

typedef struct
{
    uint16_t walk_min;
    uint16_t calorie_min;
    uint16_t mileage_min;
} T_SPORT_MIN_DATA;


int sport_min_data_flashDB_init(void);
/*sport_min_data tsdb*/

/*sleep data tsdb*/
typedef struct
{
    uint8_t sleep_year;
    uint8_t sleep_month;
    uint8_t sleep_day;
    uint8_t sleep_state1;
    uint8_t sleep_hour1;
    uint8_t sleep_min1;
    uint8_t sleep_sec1;
    uint8_t sleep_state2;
    uint8_t sleep_hour2;
    uint8_t sleep_min2;
    uint8_t sleep_sec2;
    uint8_t sleep_state3;
    uint8_t sleep_hour3;
    uint8_t sleep_min3;
    uint8_t sleep_sec3;
} T_DP_SLEEP_REPORT;
/*sleep data tsdb*/


#ifdef __cplusplus
}
#endif

#endif /* __DP_SPORTS_REFS_H__ */
