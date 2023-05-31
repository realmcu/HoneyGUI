#ifndef _WATCH_CLOCK_H_
#define _WATCH_CLOCK_H_


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "time.h"


#define ONE_HOUR_SECONDS 3600


typedef enum
{
    MOn  = 0,
    Tues  = 1,
    Wed  = 2,
    Thur = 3,
    Fri  = 4,
    Sat  = 5,
    Sun  = 6
} DAY_OF_WEEK;

// To be used with
typedef struct
{
    uint16_t year;    // 1970+
    uint8_t month;    // 1-12
    uint8_t day;      // 1-31
    uint8_t wday;     // 0-6 DAY_OF_WEEK
    uint8_t seconds;  // 0-59
    uint8_t minutes;  // 0-59
    uint8_t hour;     // 0-23
} T_UTC_TIME;

typedef struct
{
    uint8_t time_zone;
    uint32_t unix_timestamp;
    uint32_t pre_rtc_tick_count;
    T_UTC_TIME local_time;
    struct tm tm;
} T_WATCH_CLOCK;


struct tm convert_to_utc(time_t timestamp);
time_t convert_to_unix_timestamp(struct tm *time);
time_t get_unix_timestamp(void);

void watch_clock_init(time_t timestamp);
void watch_clock_set(time_t timestamp);
struct tm watch_clock_get(void);
void watch_clock_update(void *p_value);

#ifdef __cplusplus
}
#endif


#endif //_WATCH_CLOCK_H_

