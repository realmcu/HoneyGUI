/**
 * \file tuya_ble_port_rtl8762.h
 *
 * \brief
 */
/*
 *  Copyright (C) 2014-2019, Tuya Inc., All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of tuya ble sdk
 */


#ifndef TUYA_BLE_PORT_RTL8762_H__
#define TUYA_BLE_PORT_RTL8762_H__


#include "tuya_ble_stdlib.h"
#include "tuya_ble_type.h"
#include "tuya_ble_config.h"

#if TUYA_BLE_LOG_ENABLE

#include "trace.h"

uint8_t get_args(const char *format);
void log_hexdump(const char *name, uint8_t width, uint8_t *buf, uint16_t size);

//#define  TUYA_BLE_PRINTF(fmt,...) \
//        { \
//            uint8_t param_num = get_args(fmt); \
//            DBG_BUFFER_INTERNAL(TYPE_BEE2, SUBTYPE_FORMAT, MODULE_APP, LEVEL_INFO, fmt, param_num, ##__VA_ARGS__); \
//        }


#if 0
#ifdef __RTTHREAD__
#define  TUYA_BLE_PRINTF(fmt,...) \
    { \
        rt_kprintf(fmt, ##__VA_ARGS__); \
        rt_kprintf("\n"); \
    }
#else
#if (WS_USING_LETTER_SHELL == 1)
#include "shell.h"
#include "shell_port.h"

#define  TUYA_BLE_PRINTF(fmt,...) \
    do{ \
        WS_PRINTF(fmt, ##__VA_ARGS__); \
        WS_PRINTF("\r\n"); \
    }while(0)
#else
#define  TUYA_BLE_PRINTF(fmt,...)  DBG_DIRECT(fmt, ##__VA_ARGS__)
#endif
#endif
#endif

#define  TUYA_BLE_PRINTF(fmt,...)  DBG_DIRECT(fmt, ##__VA_ARGS__)

#define TUYA_BLE_HEXDUMP(...)    log_hexdump("", 8, __VA_ARGS__)

#else

#define TUYA_BLE_PRINTF(...)
#define TUYA_BLE_HEXDUMP(...)

#endif


#include "os_sync.h"

#define tuya_ble_device_enter_critical() \
    {                                \
        uint32_t int_mask;           \
        int_mask = os_lock();

#define tuya_ble_device_exit_critical()  \
    os_unlock(int_mask); \
    }

#endif





