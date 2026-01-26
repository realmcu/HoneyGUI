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

#ifndef LX_PLATFORM_LOG_H
#define LX_PLATFORM_LOG_H

#if defined(__cplusplus)
extern "C" {
#endif


/*********************
*      INCLUDES
*********************/
#include <stdint.h>
#include <stdio.h>
#include <gui_api.h>

/*********************
*      DEFINES
*********************/
#define LX_LOG_LEVEL_TRACE 0 /**< A lot of logs to give detailed information*/
#define LX_LOG_LEVEL_INFO  1 /**< Log important events*/
#define LX_LOG_LEVEL_WARN  2 /**< Log if something unwanted happened but didn't caused problem*/
#define LX_LOG_LEVEL_ERROR 3 /**< Only critical issue, when the system may fail*/
#define LX_LOG_LEVEL_USER  4 /**< Custom logs from the user*/
#define LX_LOG_LEVEL_NONE  5 /**< Do not log anything*/
#define _LX_LOG_LEVEL_NUM  6 /**< Number of log levels*/

#define LX_LOG_LEVEL LX_LOG_LEVEL_WARN

#define LX_PRINTF   gui_log

#ifndef LX_LOG_TRACE
#  if LX_LOG_LEVEL <= LX_LOG_LEVEL_TRACE
#    define LX_LOG_TRACE(_format, ...)  do                                                                                          \
    {                                                                                           \
        LX_PRINTF("[%s][%s]<%d> " _format "\n", "LX Trace", __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    }                                                                                           \
    while(0)
#  else
#    define LX_LOG_TRACE(_format, ...) do {}while(0)
#  endif
#endif

#ifndef LX_LOG_INFO
#  if LX_LOG_LEVEL <= LX_LOG_LEVEL_INFO
#    define LX_LOG_INFO(_format, ...)  do                                                                                           \
    {                                                                                           \
        LX_PRINTF("[%s][%s]<%d> " _format "\n", "LX Info", __FUNCTION__, __LINE__, ##__VA_ARGS__);  \
    }                                                                                           \
    while(0)
#  else
#    define LX_LOG_INFO(_format, ...) do {}while(0)
#  endif
#endif

#ifndef LX_LOG_WARN
#  if LX_LOG_LEVEL <= LX_LOG_LEVEL_WARN
#    define LX_LOG_WARN(_format, ...)  do                                                                                           \
    {                                                                                           \
        LX_PRINTF("[%s][%s]<%d> " _format "\n", "LX Warn", __FUNCTION__, __LINE__, ##__VA_ARGS__);  \
    }                                                                                           \
    while(0)
#  else
#    define LX_LOG_WARN(_format, ...) do {}while(0)
#  endif
#endif

#ifndef LX_LOG_ERROR
#  if LX_LOG_LEVEL <= LX_LOG_LEVEL_ERROR
#    define LX_LOG_ERROR(_format, ...)  do                                                                                          \
    {                                                                                           \
        LX_PRINTF("[%s][%s]<%d> " _format "\n", "LX Error", __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    }                                                                                           \
    while(0)
#  else
#    define LX_LOG_ERROR(_format, ...) do {}while(0)
#  endif
#endif

#ifndef LX_LOG_USER
#  if LX_LOG_LEVEL <= LX_LOG_LEVEL_USER
#    define LX_LOG_USER(_format, ...)  do                                                                                           \
    {                                                                                           \
        LX_PRINTF("[%s][%s]<%d> " _format "\n", "LX User", __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    }                                                                                           \
    while(0)
#  else
#    define LX_LOG_USER(_format, ...) do {}while(0)
#  endif
#endif

#define LX_ASSERT(expr)                                        \
    do {                                                       \
        if(!(expr)) {                                          \
            LX_LOG_ERROR("Asserted at expression: %s", #expr); \
            lx_platform_assert(__FILE__, __LINE__);            \
        }                                                      \
    } while(0)

#define LX_ASSERT_MSG(expr, msg)                                         \
    do {                                                                 \
        if(!(expr)) {                                                    \
            LX_LOG_ERROR("Asserted at expression: %s (%s)", #expr, msg); \
            lx_platform_assert(__FILE__, __LINE__);                      \
        }                                                                \
    } while(0)

/**********************
*      TYPEDEFS
**********************/
void lx_platform_trace(const char *buf);
void lx_platform_assert(const char *file, uint32_t line);

/**********************
* GLOBAL PROTOTYPES
**********************/

#if defined(__cplusplus)
}
#endif

#endif // LX_PLATFORM_LOG_H
