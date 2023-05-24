/**
 * \file custom_tuya_ble_config.h
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


#ifndef CUSTOM_TUYA_BLE_CONFIG_H__
#define CUSTOM_TUYA_BLE_CONFIG_H__

#include "tuya_ble_type.h"
#include "tuya_ble_app_demo.h"


#define TUYA_BLE_APP_VERSION_STRING         TY_APP_VER_STR


#define TUYA_BLE_APP_BUILD_FIRMNAME_STRING  APP_BUILD_FIRMNAME


#define TUYA_BLE_PORT_PLATFORM_HEADER_FILE  "tuya_ble_port_rtl8762.h"


#define CUSTOMIZED_TUYA_BLE_APP_PRODUCT_TEST_HEADER_FILE "custom_app_product_test.h"


#define CUSTOMIZED_TUYA_BLE_APP_UART_COMMON_HEADER_FILE "custom_app_uart_common_handler.h"


/*
 * If using an OS, be sure to call the tuya api functions and SDK-related queues within the same task
 */

#define TUYA_BLE_USE_OS 1

/*
 * If using an OS, tuya ble sdk  will create a task autonomously to process ble event.
 */

#if TUYA_BLE_USE_OS

#define TUYA_BLE_SELF_BUILT_TASK  0

#if TUYA_BLE_SELF_BUILT_TASK

#define TUYA_BLE_TASK_PRIORITY  1

#define TUYA_BLE_TASK_STACK_SIZE  256 * 10   //!<  Task stack size ，application do not change this default value

#endif

#endif


/*
 * TUYA_BLE_DEVICE_COMMUNICATION_ABILITY_BLE                   ble normal
 * TUYA_BLE_DEVICE_COMMUNICATION_ABILITY_REGISTER_FROM_BLE     device register from ble
 * TUYA_BLE_DEVICE_COMMUNICATION_ABILITY_MESH                  ble mesh
 * TUYA_BLE_DEVICE_COMMUNICATION_ABILITY_WIFI_24G              wifi_2.4g
 * TUYA_BLE_DEVICE_COMMUNICATION_ABILITY_WIFI_5G               wifi_5g
 * TUYA_BLE_DEVICE_COMMUNICATION_ABILITY_ZIGBEE                zigbee
 * TUYA_BLE_DEVICE_COMMUNICATION_ABILITY_NB                    nb-iot
 * @note:
 * for example (TUYA_BLE_DEVICE_COMMUNICATION_ABILITY_BLE|TUYA_BLE_DEVICE_COMMUNICATION_ABILITY_MESH|TUYA_BLE_DEVICE_COMMUNICATION_ABILITY_WIFI_24G)
 */

#define TUYA_BLE_DEVICE_COMMUNICATION_ABILITY  (TUYA_BLE_DEVICE_COMMUNICATION_ABILITY_BLE|TUYA_BLE_DEVICE_COMMUNICATION_ABILITY_REGISTER_FROM_BLE)


/*
 * Whether it is a shared device.
 */
#define TUYA_BLE_DEVICE_SHARED  0

/*
 * If it is 1, then sdk need to perform the unbind operation,otherwise sdk do not need.
 */
#define TUYA_BLE_DEVICE_UNBIND_MODE  1

/*
 * If TUYA_BLE_WIFI_DEVICE_REGISTER_MODE is 1,Mobile app must first sends instructions to query network status after sending the pairing request .
 */
#define TUYA_BLE_WIFI_DEVICE_REGISTER_MODE  0

/*
 * if 1 ,tuya ble sdk authorization self-management
 */
#define  TUYA_BLE_DEVICE_AUTH_SELF_MANAGEMENT   1

/*
 * TUYA_BLE_SECURE_CONNECTION_WITH_AUTH_KEY     encrypt with auth key
 * TUYA_BLE_SECURE_CONNECTION_WITH_ECC          encrypt with ECDH
 * TUYA_BLE_SECURE_CONNECTION_WTIH_PASSTHROUGH  no encrypt
 * TUYA_BLE_SECURE_CONNECTION_WITH_AUTH_KEY_ADVANCED_ENCRYPTION  advanced encrypt(security chip) with auth key
 * @note : only choose one
*/
#define  TUYA_BLE_SECURE_CONNECTION_TYPE  TUYA_BLE_SECURE_CONNECTION_WITH_AUTH_KEY


#if (TUYA_BLE_SECURE_CONNECTION_TYPE == TUYA_BLE_SECURE_CONNECTION_WITH_AUTH_KEY_ADVANCED_ENCRYPTION)
#define TUYA_BLE_ADVANCED_ENCRYPTION_AUTH_ON_CONNECT    1

/*
 * if 1 ,tuya ble sdk include cjson components [used for production test parse comm config json data]
 */
#define TUYA_BLE_INCLUDE_CJSON_COMPONENTS               1
#endif


/*
 * if 1 ,ble sdk will update mac address with with the address of the authorization information.
 */
#define  TUYA_BLE_DEVICE_MAC_UPDATE             1
/*
 * if 1 ,after update mac will reset.
 */
#define  TUYA_BLE_DEVICE_MAC_UPDATE_RESET       0


#define  TUYA_BLE_USE_PLATFORM_MEMORY_HEAP      0


/*
 * gatt mtu max sizes
 */
#define TUYA_BLE_DATA_MTU_MAX       (247-3)


/*
 * if defined ,enable log output
 */
#define TUYA_BLE_LOG_ENABLE         1

#define TUYA_BLE_LOG_COLORS_ENABLE  1

#define TUYA_BLE_LOG_LEVEL  TUYA_BLE_LOG_LEVEL_DEBUG

/*
 * if defined ,enable app log output
 */
#define TUYA_APP_LOG_ENABLE         1

#define TUYA_APP_LOG_COLORS_ENABLE  1

#define TUYA_APP_LOG_LEVEL  TUYA_APP_LOG_LEVEL_DEBUG


/*
 * If it is 1, then sdk need to request beacon key from app when binding .
 */
#define TUYA_BLE_BEACON_KEY_ENABLE  0


/*
 * If it is 1, the sdk support weather feature
 */
#define TUYA_BLE_FEATURE_WEATHER_ENABLE       1


/*
 * If it is 1, the sdk support common iot data channel
 */
#define TUYA_BLE_FEATURE_IOT_CHANNEL_ENABLE     1

#if ( TUYA_BLE_FEATURE_IOT_CHANNEL_ENABLE != 0 )
/*
 * If it is 1, the sdk support scene list data request and control
 */
#define TUYA_BLE_FEATURE_SCENE_ENABLE           1


#endif


/*
 * Whether to automatically perform a time request after the connection is established.
 * - 0  Not request.
 * - 1  Request cloud time.
 * - 2  Request phone local time.
 */
#define TUYA_BLE_AUTO_REQUEST_TIME_CONFIGURE  1



//nv
/* The minimum size of flash erasure. May be a flash sector size. */
#define TUYA_NV_ERASE_MIN_SIZE         (0x100)

/* the flash write granularity, unit: byte*/
#define TUYA_NV_WRITE_GRAN             (4)

/* start address */
#define TUYA_NV_START_ADDR              0

/* area size. */
#define TUYA_NV_AREA_SIZE              (4*TUYA_NV_ERASE_MIN_SIZE)

#endif

