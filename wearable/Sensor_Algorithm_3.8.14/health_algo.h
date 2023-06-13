
#ifndef _HEALTH_ALGO_H_
#define _HEALTH_ALGO_H_

#ifdef __cplusplus
extern "C" {
#endif

// Headers
#include <stdint.h>
#include "rtk_gsa.h"


// Macros
#define DEFAULT_STEP_TASK   10000
#define GSENSOR_XPOS        GS_XPOS_DOWN
#define GSENSOR_ZPOS        GS_ZPOS_UP

typedef uint32_t DateType;



/* time bit field */
typedef struct
{
    uint16_t day        : 5;
    uint16_t month      : 4;
    uint16_t year       : 6;
    uint16_t reserved   : 1;
}
Date_bit_field_type_t;

typedef union
{
    uint16_t data;
    Date_bit_field_type_t date;
} Date_union_t;

typedef struct SportsHead
{
    Date_union_t Date;
    uint8_t reserved;
    uint8_t length;
}
SportsHead_t;

typedef struct SleepHead
{
    Date_union_t Date;
    uint16_t length;
}
SleepHead_t;

typedef struct
{
    uint64_t Distance    : 16;
    uint64_t Calory      : 19;
    uint64_t active_time : 4;
    uint64_t steps       : 12;
    uint64_t mode        : 2;
    uint64_t offset      : 11;
}
SportsData_bit_field_type_t;

typedef union SportsData
{
    uint64_t data;
    SportsData_bit_field_type_t bits;
} SportsData_U;

typedef struct
{
    uint32_t mode          : 4;
    uint32_t sleeping_flag : 1;
    uint32_t reserved      : 11;
    uint32_t timeStamp     : 16;
}
SleepData_bit_field_type_t;

typedef union SleepDataU
{
    uint32_t data;
    SleepData_bit_field_type_t bits;
} SleepData_U;

typedef enum
{
    TSLEEP = 0x01,
    TDEEP_SLEEP = 0x02,
    TGET_UP = 0x03,
    TNOT_WEAR = 0x04,
} T_SLEEP_STATUS;


typedef struct algorithm_event_sleep
{
    uint8_t mode;
    uint32_t starting_time_stamp;
} algorithm_event_sleep_t;


typedef struct HeartHead
{
    Date_union_t Date;
    uint16_t length;
}
HeartHead_t;


typedef struct
{
uint32_t heart_rate   :
    8;
uint32_t heart_second   :
    8;
uint32_t timeStamp  :
    16;
}
HeartData_bit_field_type_t;

typedef union HeartDataU
{
    uint32_t data;
    HeartData_bit_field_type_t bits;
} HeartData_U;


int health_algo_init(void);

#ifdef __cplusplus
}
#endif

#endif

