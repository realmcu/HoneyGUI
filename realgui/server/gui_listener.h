/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __GUI_LISTENER_H__
#define __GUI_LISTENER_H__

#include "guidef.h"
#include "gui_obj.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Message callback function type
 * @param obj The widget object
 * @param topic Message topic string
 * @param data Message data pointer
 * @param len Data length
 */
typedef void (*gui_listener_cb_t)(gui_obj_t *obj, const char *topic, void *data, uint16_t len);

/**
 * @brief Subscribe to a message topic
 * @param obj The widget object
 * @param topic Message topic string (e.g., "sensor/temp")
 * @param callback Callback function to handle the message
 */
void gui_msg_subscribe(gui_obj_t *obj, const char *topic, gui_listener_cb_t callback);

/**
 * @brief Unsubscribe from a message topic
 * @param obj The widget object
 * @param topic Message topic string, set NULL to unsubscribe all topics
 */
void gui_msg_unsubscribe(gui_obj_t *obj, const char *topic);

/**
 * @brief Publish a message to all subscribers
 * @param topic Message topic string
 * @param data Message data pointer
 * @param len Data length
 */
void gui_msg_publish(const char *topic, void *data, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif // __GUI_LISTENER_H__
