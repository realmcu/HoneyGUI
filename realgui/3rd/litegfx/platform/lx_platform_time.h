/******************************************************************************
* Copyright (c) 2021 Shanghai QDay Technology Co., Ltd.
* All rights reserved.
*
* This file is part of the LiteGFX 0.0.1 distribution.
*
* This software is licensed under terms that can be found in the LICENSE file in
* the root directory of this software component.
* If no LICENSE file comes with this software, it is provided AS-IS.
*
* Author:LiteGFX Team
* Date:2021.12.05
*******************************************************************************/
#ifndef LX_PLATFORM_TIME_H
#define LX_PLATFORM_TIME_H

#if defined(__cplusplus)
extern "C" {
#endif


/*********************
*      INCLUDES
*********************/
#include <stdint.h>
#include <stdbool.h>


/*********************
*      DEFINES
*********************/

/**********************
*      TYPEDEFS
**********************/
typedef struct
{
    uint16_t year;              // years like 2022,2023...
    uint8_t month;             // months since January - [1, 12]
    uint8_t mday;              // day of the month - [1, 31]
    uint8_t hour;              // hours since midnight - [0, 23]
    uint8_t minute;            // minutes after the hour - [0, 59]
    uint8_t second;            // seconds after the minute - [0, 60] including leap second
    uint16_t millisecond;       // milliseconds after the second - [0, 1000]
    uint8_t weekday;           // days since Sunday - [0, 6]
} lx_platform_time_t;

/**********************
* GLOBAL PROTOTYPES
**********************/
void lx_platform_get_time(lx_platform_time_t *time_p);

#if defined(__cplusplus)
}
#endif

#endif // LX_PLATFORM_TIME_H
