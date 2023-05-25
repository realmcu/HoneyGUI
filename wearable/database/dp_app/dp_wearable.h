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
#ifndef DP_WEARABLE_H
#define DP_WEARABLE_H

#ifdef __cplusplus
extern "C"
{
#endif

/*============================================================================*
 *                        Header Files
*============================================================================*/
#include "stdbool.h"
#include "stdint.h"
#include "string.h"
#include "dp_sports_refs.h"

typedef struct
{
    uint8_t *name;
    uint8_t  data_type;
    uint8_t  data_child_type;
    uint16_t data_length;
    void *data_buf;

} data_info_def;

typedef enum
{
    HRS_DATA,
    GSR_DATA,
    CONFIG_DATA,
    WEATHER_DATA,
    CLOCK_DATA,
    PANEL_INFO_DATA,
    BLE_INFO_DATA,
    NOTIFY_DATA,
} dp_data_type;

typedef enum
{
    HRS_HEARTRATE_MIN,
    HRS_BLOODOXGEN_MIN,
    HRS_BLOODE_LOW_HIGH_MIN,
    HRS_BLOOD_PER_HIGH_MIN,
} hrs_data_type;




typedef enum
{
    GSR_ALL_TARGET,
    GSR_STEPS_TARGET,
    GSR_CAL_TARGET,
    GSR_DISC_TARGET,
    GSR_STEPS_TOTAL,
    GSR_SEDENTARY_TIME,
    GSR_CAL_TOTAL,
    GSR_DISC_TOTAL,
    GSR_EXERCISE_CONTROL,
    GSR_SLEEP_SET,
    GSR_MIN_STEP,
    GSR_MIN_CAL,
    GSR_MIN_MILE,
} gsr_data_type;

typedef enum
{
    CFG_INFO_SET,
    CFG_NOTIFY_SET,
    CFG_CLOCK_SET,
    CFG_DRKING_SET,
    CFG_TIME_MODE_SET,
    CFG_LANGUAGE_SET,

} config_data_type;

typedef enum
{
    MES_PUSH,
    MES_WECHAT,
    MES_QQ,
    MES_CALL,
    MES_OUTLOOK,
    MES_SMES,

} message_type;


void gui_port_get_local_time(time_t *now);
void gui_port_data_init(void);
uint8_t db_data_get_mes_num(uint8_t type);


void dp_database_init(void);

#ifdef __cplusplus
}
#endif

#endif /* DP_WEARABLE_H */
