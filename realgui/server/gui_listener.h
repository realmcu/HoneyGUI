/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/**
 * @file gui_listener.h
 * @brief Legacy topic-based listener API.
 *
 * @deprecated This API is scheduled for removal in a future release. Existing
 * users may continue to use it temporarily, but new code should not depend on
 * it.
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
 *
 * @deprecated Used only by the legacy listener API, which is scheduled for
 * removal in a future release.
 */
typedef void (*gui_listener_cb_t)(gui_obj_t *obj, const char *topic, void *data, uint16_t len);

/**
 * @brief Subscribe to a message topic
 * @param obj The widget object
 * @param topic Message topic string (e.g., "sensor/temp")
 * @param callback Callback function to handle the message
 *
 * @deprecated Scheduled for removal in a future release. Do not use this API
 * in new code.
 */
void gui_msg_subscribe(gui_obj_t *obj, const char *topic, gui_listener_cb_t callback);

/**
 * @brief Unsubscribe from a message topic
 * @param obj The widget object
 * @param topic Message topic string, set NULL to unsubscribe all topics
 *
 * @deprecated Scheduled for removal in a future release. Do not use this API
 * in new code.
 */
void gui_msg_unsubscribe(gui_obj_t *obj, const char *topic);

/**
 * @brief Publish a message to all subscribers, data deep copied inside
 * @param topic Message topic string
 * @param data Message data pointer
 * @param len Data length
 *
 * @deprecated Scheduled for removal in a future release. Do not use this API
 * in new code.
 */
void gui_msg_publish(const char *topic, void *data, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif // __GUI_LISTENER_H__
