/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __DASHBOARD_ADAPT_H__
#define __DASHBOARD_ADAPT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/*============================================================================*
 *                           Types
 *============================================================================*/
typedef enum
{
    DASHBOARD_OTA_EVENT_START,
    DASHBOARD_OTA_EVENT_DATA_CONTINUE,
    DASHBOARD_OTA_EVENT_FINISH,
    DASHBOARD_OTA_EVENT_ERROR,
} dashboard_ota_event_t;
typedef struct gui_dashboard
{
    /* BT & WIFI status */
    uint32_t bt_status         : 1; // bluetooth status, 1: on, 0: off
    uint32_t wifi_status       : 1; // wifi status, 1: on, 0: off

    /* LED status */
    uint32_t led0_status       : 1; // 1: on, 0: off
    uint32_t led1_status       : 1;
    uint32_t led2_status       : 1;
    uint32_t led3_status       : 1;
    uint32_t led4_status       : 1;
    uint32_t led5_status       : 1;
    uint32_t led_turn_l_status : 1;
    uint32_t led_turn_r_status : 1;

    /* Map status */
    uint32_t map_data_update   : 1;
    uint32_t map_data_index    : 1;
    void *map_data[2];     // map data pointer

    /* OTA status */
    uint32_t ota_in_progress   : 1; // OTA in progress flag

    /* Message notification status */
    uint32_t message_notify    : 1; // Message notification flag

    /* Navigation status */
    uint32_t navi_active       : 1; // Navigation active flag (NAVI_START=1, NAVI_IDLE/NAVI_PAUSE=0)

    uint16_t speed_val; // km/h
    uint16_t power_val; // kW
    uint16_t odo_val;   // km
    uint16_t soc_val;   // %
    uint16_t temp_val;  // °C

    char location[16];

    /* OTA data */
    uint32_t ota_total_num;
    uint32_t ota_current_num;

    /* Message notification data */
    char message_text[128];
    uint16_t message_length;
    uint8_t message_type;

    /* Phone call status */
    uint32_t phone_active      : 1; // Phone call active flag
    uint32_t phone_calling     : 1; // In call flag

    char phone_number[16];           // Phone number
    uint8_t phone_number_len;        // Phone number length

} gui_dashboard_t;


/*============================================================================*
 *                         Functions
 *============================================================================*/
void gui_dashboard_info_register(gui_dashboard_t *info);

gui_dashboard_t *get_dashboard_info(void);

void gui_dashboard_ota_set_info(uint32_t total_num, uint32_t current_num,
                                dashboard_ota_event_t ota_event);

float gui_dashboard_ota_get_percentage(void);

#ifdef __cplusplus
}
#endif
#endif


