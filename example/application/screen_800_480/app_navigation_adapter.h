/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Map Adapter Header
 *============================================================================*/
#ifndef __APP_MAP_ADAPTER_H__
#define __APP_MAP_ADAPTER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
/*============================================================================*
 *                          Navigation Constants
 *============================================================================*/

/* Test navigation type definitions (for simulator testing) */
#define SIM_TURN_LEFT         2
#define SIM_TURN_RIGHT        3
#define SIM_LEFT_FRONT        4
#define SIM_RIGHT_FRONT       5
#define SIM_LEFT_BACK         6
#define SIM_RIGHT_BACK        7
#define SIM_LEFT_TURN_AROUND  8
#define SIM_GO_STRAIGHT       9

/*============================================================================*
 *                           Function Declarations
 *============================================================================*/

/**
 * @brief Update map display buffers
 * @param buffer Pointer to map buffer data
 * @param length Length of buffer data
 */
void map_update_buffers_display(void *buffer, uint32_t length);

/**
 * @brief Stop normal navigation display
 */
void map_display_normal_navi_stop(void);

/**
 * @brief Stop buffer navigation display
 */
void map_display_buffer_stop(void);

/**
 * @brief Send ITP (Image Transfer Protocol) ACK
 */
void app_dashboard_data_send_itp_ack(void);

/**
 * @brief Update navigation status from BLE data
 * @param pValue Pointer to navigation data
 * @param length Length of data
 *
 * Navigation icon mapping:
 * - NAVI_C1_BIN: Straight
 * - NAVI_C2_BIN: Left turn
 * - NAVI_C3_BIN: Right turn
 * - NAVI_C4_BIN: U-turn
 */
void app_dashboard_data_update_navi_status(const uint8_t *pValue, uint16_t length);

/**
 * @brief Get navigation distance message (e.g., "1.5" or "500")
 * @param msg Buffer to store message
 * @param len Pointer to store message length
 */
void app_navigation_get_distance_msg(uint8_t **msg, uint8_t *len);

/**
 * @brief Get navigation unit
 * @param unit Buffer to store unit
 * @param len Pointer to store unit length
 */
void app_navigation_get_unit(uint8_t **unit, uint8_t *len);

/**
 * @brief Get navigation road name
 * @param road Buffer to store road name
 * @param len Pointer to store road name length
 */
void app_navigation_get_road_name(uint8_t **road, uint8_t *len);

/**
 * @brief Get navigation icon type
 * @return Icon type: 1=straight, 2=left, 3=right, 4=u-turn
 */
uint8_t app_navigation_get_icon_type(void);

#ifdef __cplusplus
}
#endif

#endif /* __APP_MAP_ADAPTER_H__ */
