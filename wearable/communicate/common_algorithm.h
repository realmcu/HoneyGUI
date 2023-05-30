#ifndef _COMMON_ALGORITHM_H_
#define _COMMON_ALGORITHM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"






typedef enum RtkInType
{
    DATA_IN,    /* input type data*/
    COMPOSITE   /* composite of 3-axis data type*/
} RtkInType_t;




typedef struct              /*RTK Pedometer Input Parameters.*/
{
    RtkInType_t InputType;  /* Default: DATA_IN*/
    int16_t wXAccel;            /* - X axis acceleration, Q4.11*/
    int16_t wYAccel;            /* - Y axis acceleration, Q4.11*/
    int16_t wZAccel;            /* - Z axis acceleration, Q4.11*/
    int16_t wCompositeIn;   /* - sqrt(x^2+y^2+z^2), Q4.11*/
    uint32_t TimeStamp;     /* - reserved*/
} RtkAccelData_t;




/** RTK API results.
    *
    * This enumeration is used to symbolize the possible results of
    * invoking RTK library functions.
    */
typedef enum RTStatus
{
    RT_SUCCESS,
    RT_FAILURE,
    RT_CALLBACK_FAILURE,
    RT_NULL_DATA_PTR,
    RT_CAL_COMPLETE,
    RT_MEM_ALLOCATE_FAILURE,
    RT_ALREADY_OPEN,
    RT_ALREADY_CLOSED,
    RT_UNKNOWN_DATA_TYPE,
    RT_INVALID_CFG,
    RT_NOT_SUPPPORT
} RTStatus_t;

#define SLP_DEBUG_LEN  10
typedef struct
{
    uint16_t reserv0;
    uint8_t w_ptr;
    uint8_t mode[SLP_DEBUG_LEN];
    uint32_t rtc_timer[SLP_DEBUG_LEN];
    uint32_t reserv1;
} RtkSlpDebug;
extern RtkSlpDebug gRTKSlpDbg;

typedef struct              /*RTK Pedometer Input Parameters.*/
{
    //RtkInType_t InputType;  /* Default: DATA_IN*/
    int16_t wXAccel;            /* - X axis acceleration, Q4.11*/
    int16_t wYAccel;            /* - Y axis acceleration, Q4.11*/
    int16_t wZAccel;            /* - Z axis acceleration, Q4.11*/
    //int16_t wCompositeIn;     /* - sqrt(x^2+y^2+z^2), Q4.11*/
    uint32_t TimeStamp;     /* - reserved*/
} RtkAccelSubData_t;
#ifdef __cplusplus
}
#endif


#endif
