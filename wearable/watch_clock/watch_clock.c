/*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     wristband_clock.c
* @brief
* @details
* @author
* @date     2017-10-31
* @version  v0.1
*********************************************************************************************************
*/

#include "trace.h"
#include "string.h"
#include "watch_clock.h"
#include "drv_rtc.h"
#if (WS_USING_LETTER_SHELL == 1)
#include "shell.h"
#include "shell_port.h"
#endif
#ifdef TASK_HANG_TEST
uint32_t auto_test_count = 0;
#endif




static T_WATCH_CLOCK watch_clock;

time_t get_unix_timestamp(void)
{
    uint32_t cur_rtc_tick_count = drv_rtc_count();
    uint32_t diff_second = 0;
    if (cur_rtc_tick_count > watch_clock.pre_rtc_tick_count)
    {
        diff_second = (cur_rtc_tick_count - watch_clock.pre_rtc_tick_count) / drv_rtc_clock_src_freq();
    }
    else
    {
        diff_second = (cur_rtc_tick_count + 0xffffffff - watch_clock.pre_rtc_tick_count) /
                      drv_rtc_clock_src_freq();
    }
    return (watch_clock.unix_timestamp + diff_second);
}

time_t convert_to_unix_timestamp(struct tm *time)
{
    /* converts the local time in time to calendar time. */
    return mktime(time);
}

struct tm convert_to_utc(time_t now)
{
    struct tm *p_tm;
    struct tm tm_new;

    /* converts calendar time time into local time. */
    p_tm = localtime(&now);
    /* copy the statically located variable */
    memcpy(&tm_new, p_tm, sizeof(struct tm));

    return tm_new;
}

void watch_clock_set(time_t time_stamp)
{
    struct tm tm_new = convert_to_utc(time_stamp);
    memcpy(&watch_clock.tm, &tm_new, sizeof(struct tm));

    watch_clock.local_time.year = tm_new.tm_year + 1900;
    watch_clock.local_time.month = tm_new.tm_mon + 1;
    watch_clock.local_time.day = tm_new.tm_mday;
    watch_clock.local_time.wday = tm_new.tm_wday;
    watch_clock.local_time.hour = tm_new.tm_hour;
    watch_clock.local_time.minutes = tm_new.tm_min;
    watch_clock.local_time.seconds = tm_new.tm_sec;

    watch_clock.unix_timestamp = time_stamp;
    watch_clock.pre_rtc_tick_count = drv_rtc_count();
}

struct tm watch_clock_get(void)
{
    return watch_clock.tm;
}

void watch_clock_update(void *p_value)
{
    watch_clock.unix_timestamp = watch_clock.unix_timestamp + (60 - watch_clock.unix_timestamp % 60);
    watch_clock.pre_rtc_tick_count = drv_rtc_count();

    drv_rtc_set_comp(false, watch_clock.unix_timestamp);

    /* get UTCTime time */
    watch_clock_set(watch_clock.unix_timestamp);

    APP_PRINT_INFO6("year:%d,month:%d,day:%d,hour:%d,minute:%d,second:%d\r\n",
                    watch_clock.local_time.year,
                    watch_clock.local_time.month, watch_clock.local_time.day,
                    watch_clock.local_time.hour, watch_clock.local_time.minutes,
                    watch_clock.local_time.seconds);

    APP_PRINT_INFO1("local time %s", TRACE_STRING(asctime(localtime(&watch_clock.unix_timestamp))));
}


/**
  * @brief   watch clock init
  * @param   init offset secod, sometimes is zero
  * @return  void
  */
void watch_clock_init(time_t time_stamp)
{
    drv_rtc_minute_attach_irq(watch_clock_update, NULL);

    watch_clock.time_zone = 8;

    watch_clock.pre_rtc_tick_count = 0;

    watch_clock_set(time_stamp);

    APP_PRINT_INFO1("watch clock_start second_diff_value: %d", (60 - watch_clock.unix_timestamp % 60));

    drv_rtc_set_comp(true, watch_clock.unix_timestamp);
}

#if (WS_USING_LETTER_SHELL == 1)
void watch_clock_test(time_t time_stamp)
{
    time_t time_new = time_stamp + ONE_HOUR_SECONDS * watch_clock.time_zone;
    watch_clock_init(time_new);
    LOG_I("set watch date and clock: %s\r\n", asctime(localtime(&time_new)));
}
SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN,
    watch_clock_test, watch_clock_test, test);
#endif
