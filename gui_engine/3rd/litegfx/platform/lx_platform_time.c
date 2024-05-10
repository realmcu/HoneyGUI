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

/*********************
 *      INCLUDES
 *********************/
#include "lx_platform_time.h"
#include <time.h>

//#define CONFIG_SIMULATOR 0

#if defined(_WIN32)
#include <Windows.h>
#else
#include "module_global_data.h"
#endif

/*********************
*      DEFINES
*********************/


/**********************
*      TYPEDEFS
**********************/


/**********************
*  STATIC PROTOTYPES
**********************/


/**********************
 *  STATIC VARIABLES
 **********************/


/**********************
*  GLOBAL VARIABLES
**********************/


/**********************
*      MACROS
**********************/


/**********************
*   GLOBAL FUNCTIONS
**********************/
void lx_platform_get_time(lx_platform_time_t *time_p)
{
#if defined(_WIN32)
    SYSTEMTIME lt;

    GetLocalTime(&lt);

    time_p->year = lt.wYear + 1900;
    time_p->month = lt.wMonth + 1;
    time_p->mday = lt.wDay;
    time_p->weekday = lt.wDayOfWeek;
    time_p->hour = lt.wHour;
    time_p->minute = lt.wMinute;
    time_p->second = lt.wSecond;
    time_p->millisecond = lt.wMilliseconds;
#else
    time_p->year = RtkWristbandSys.Global_Time.year;
    time_p->month = RtkWristbandSys.Global_Time.month + 1;
    time_p->mday = RtkWristbandSys.Global_Time.day + 1;
    time_p->weekday = (uint8_t)get_day_of_week(RtkWristbandSys.SecondCountRTC);
    time_p->hour = RtkWristbandSys.Global_Time.hour;
    time_p->minute = RtkWristbandSys.Global_Time.minutes;
    time_p->second = get_system_clock_second();
    time_p->millisecond = 0;
#endif /* CONFIG_RTC_ACTS */
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
