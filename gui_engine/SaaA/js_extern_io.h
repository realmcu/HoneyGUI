/**
 * @file js_user.h
 * @author howie_wang (howie_wang@realtek.com.cn)
 * @brief
 * @version 0.1
 * @date 2023-11-22
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef __JS_EXTERN_IO_H__
#define __JS_EXTERN_IO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "js_user.h"



typedef enum
{
// js extern io event define
    EXTERN_EVENT_NULL,
    EXTERN_EVENT_SMARTHOME = 1,

// js smarthome event define
    SH_EVENT_WIFI_SWITCH   = 1,

// js xxx event define


} JS_EXTERN_EVENT_ENUM;


// size and mem alignment refer to gui_msg_t
#ifdef  __CC_ARM
#pragma anon_unions
#endif
typedef struct
{
    /* the event type */
    // uint16_t type;
    /* user field of event */
    union
    {
        uint32_t  param;
        uint8_t extern_event_type;
    };
    uint8_t data_rsv[8];   // reserve
} gui_msg_js_t;



void gui_extern_event_js_handler(gui_msg_t *msg);

#ifdef __cplusplus
}
#endif

#endif
