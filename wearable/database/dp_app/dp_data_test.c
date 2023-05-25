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


/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <stdlib.h>
#include "dp_wearable.h"
#include "dp_data_test.h"
#include "tuya_dp_data.h"


#ifndef __RTTHREAD__
void kvdb_get_test(const char *kvdb_name, const char *kvdb_key)
{
    uint8_t device_name_legacy_default[40];
    rtk_kvdb_get(kvdb_name, kvdb_key, device_name_legacy_default, sizeof(device_name_legacy_default));
    LOG_I("kvdb_get_test device_name_legacy_default : %s \r\n", device_name_legacy_default);
}
SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN,
    kvdb_get_test, kvdb_get_test, test);

void kvdb_set_test(const char *kvdb_name, const char *kvdb_key)
{
    uint8_t device_name_legacy_default[40];
    rtk_kvdb_set(kvdb_name, kvdb_key, device_name_legacy_default, sizeof(device_name_legacy_default));
    LOG_I("kvdb_set_test device_name_legacy_default : %s \r\n", device_name_legacy_default);
}
SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN,
    kvdb_set_test, kvdb_set_test, test);

void kvdb_dp_target_get_test(const char *kvdb_name, const char *kvdb_key)
{
    T_DP_TARGET_SET target_set = {0};
    rtk_kvdb_get(kvdb_name, kvdb_key, &target_set, sizeof(target_set));
    LOG_I("kvdb_dp_target_get_test steps_target %d calories_target %d distance_target %d\r\n",
          target_set.steps_target, target_set.calories_target, target_set.distance_target);
}
SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN,
    kvdb_dp_target_get_test, kvdb_dp_target_get_test, test);

void kvdb_dp_target_set_test(const char *kvdb_name, const char *kvdb_key, uint32_t steps_target,
                             uint32_t calories_target, uint32_t distance_target)
{
    T_DP_TARGET_SET target_set = {0};
    target_set.steps_target = steps_target;
    target_set.calories_target = calories_target;
    target_set.distance_target = distance_target;
    rtk_kvdb_set(kvdb_name, kvdb_key, &target_set, sizeof(target_set));
    LOG_I("kvdb_dp_target_set_test\r\n");
}
SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN,
    kvdb_dp_target_set_test, kvdb_dp_target_set_test, test);

void rtk_kvdb_delete_test(const char *kvdb_name)
{
    rtk_kvdb_delete(kvdb_name);
}
SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN,
    rtk_kvdb_delete_test, rtk_kvdb_delete_test, test);

void rtk_kvdb_reset_default_test(const char *kvdb_name)
{
    rtk_kvdb_reset_default(kvdb_name);
}
SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN,
    rtk_kvdb_reset_default_test, rtk_kvdb_reset_default_test, test);

void rtk_kvdb_deinit_test(const char *kvdb_name)
{
    rtk_kvdb_deinit(kvdb_name);
}
SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN,
    rtk_kvdb_deinit_test, rtk_kvdb_deinit_test, test);

void tsdb_add_test(uint16_t walk_min, uint16_t calorie_min, uint16_t mileage_min)
{
    T_SPORT_MIN_DATA sport_min_data = {0};
    sport_min_data.walk_min = walk_min;
    sport_min_data.calorie_min = calorie_min;
    sport_min_data.mileage_min = mileage_min;

    rtk_tsdb_tsl_append(TSDB_SPORT_MIN_DATA_NAME, &sport_min_data, sizeof(sport_min_data));
}
SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN,
    tsdb_add_test, tsdb_add_test, test);

void tsdb_iter_test(const char *tsdb_name)
{
    rtk_tsdb_iter(tsdb_name);
}
SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN,
    tsdb_iter_test, tsdb_iter_test, test);

void tsdb_iter_one_by_time_test(const char *tsdb_name, uint16_t year, uint8_t mon, uint8_t mday,
                                uint8_t hour, uint8_t min, uint8_t sec)
{
    struct tm tm = {0};
    tm.tm_year = year - 1900;
    tm.tm_mon = mon - 1;
    tm.tm_mday = mday;
    tm.tm_hour = hour;
    tm.tm_min = min;
    tm.tm_sec = sec;

    T_SPORT_MIN_DATA sport_min_data;
    LOG_I("tm year %d mon %d mday %d hour %d min %d sec %d\r\n", tm.tm_year, tm.tm_mon, tm.tm_mday,
          tm.tm_hour, tm.tm_min, tm.tm_sec);
    rtk_tsdb_iter_one_by_time(tsdb_name, tm, &sport_min_data);
    LOG_I("tsdb_iter_one_by_time_test : walk_min %d calorie_min %d mileage_min %d\r\n",
          sport_min_data.walk_min, sport_min_data.calorie_min, sport_min_data.mileage_min);
}
SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN,
    tsdb_iter_one_by_time_test, tsdb_iter_one_by_time_test, test);

void rtk_tsdb_delete_test(const char *tsdb_name)
{
    rtk_tsdb_delete(tsdb_name);
}
SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN,
    rtk_tsdb_delete_test, rtk_tsdb_delete_test, test);

void rtk_tsdb_clean_test(const char *tsdb_name)
{
    rtk_tsdb_clean(tsdb_name);
}
SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN,
    rtk_tsdb_clean_test, rtk_tsdb_clean_test, test);

void rtk_tsdb_deinit_test(const char *tsdb_name)
{
    rtk_tsdb_deinit(tsdb_name);
}
SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN,
    rtk_tsdb_deinit_test, rtk_tsdb_deinit_test, test);

void rtk_tsdb_tsl_query_count_test(const char *tsdb_name, uint16_t year, uint8_t mon, uint8_t mday,
                                   uint8_t hour, uint8_t min, uint8_t sec)
{
    struct tm tm = {0};
    tm.tm_year = year - 1900;
    tm.tm_mon = min - 1;
    tm.tm_mday = mday;
    tm.tm_hour = hour;
    tm.tm_min = min;
    tm.tm_sec = sec;

    rtk_tsdb_tsl_query_count(tsdb_name, tm, tm);
}
SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN,
    rtk_tsdb_tsl_query_count_test, rtk_tsdb_tsl_query_count_test, test);
#endif

