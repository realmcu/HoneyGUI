/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/**
 * @file gui_sysval.h
 * @brief Generic asynchronous system value request API.
 */

#ifndef __GUI_SYSVAL_H__
#define __GUI_SYSVAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>


/*============================================================================*
 *                         Types
 *============================================================================*/

typedef uint32_t gui_sysval_request_id_t;

#define GUI_SYSVAL_REQUEST_INVALID ((gui_sysval_request_id_t)0)

typedef enum
{
    GUI_SYSVAL_STATUS_OK = 0,
    GUI_SYSVAL_STATUS_NOT_FOUND,
    GUI_SYSVAL_STATUS_READ_ONLY,
    GUI_SYSVAL_STATUS_INVALID_VALUE,
    GUI_SYSVAL_STATUS_BUSY,
    GUI_SYSVAL_STATUS_TIMEOUT,
    GUI_SYSVAL_STATUS_ERROR,
} gui_sysval_status_t;

/**
 * @brief Called when an asynchronous get or set request completes.
 *
 * For a get request, @p value contains the requested value. For a successful
 * set request, it may contain the value that actually took effect. The string
 * is valid only for the duration of the callback.
 *
 * @param request_id Completed request.
 * @param status     Final request status.
 * @param value      Response value, or NULL when no value is available.
 * @param user_data  Value supplied with the request.
 */
typedef void (*gui_sysval_response_cb_t)(gui_sysval_request_id_t request_id,
                                         gui_sysval_status_t status,
                                         const char *value,
                                         void *user_data);


/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * @brief Start an asynchronous read.
 *
 * The key is defined by HoneyGUI or an extension module. The callback is
 * invoked when the platform backend completes the request.
 *
 * @param key       System value key.
 * @param cb        Completion callback.
 * @param user_data Value passed to @p cb.
 * @return Request ID, or GUI_SYSVAL_REQUEST_INVALID if queuing failed.
 */
gui_sysval_request_id_t gui_sysval_get_request(const char *key,
                                               gui_sysval_response_cb_t cb,
                                               void *user_data);

/**
 * @brief Start an asynchronous write.
 *
 * The key and value are valid only for the duration of this call. The platform
 * adapter must copy them before returning if they are needed asynchronously.
 *
 * @param key       System value key.
 * @param value     NUL-terminated value string.
 * @param cb        Completion callback.
 * @param user_data Value passed to @p cb.
 * @return Request ID, or GUI_SYSVAL_REQUEST_INVALID if queuing failed.
 */
gui_sysval_request_id_t gui_sysval_set_request(const char *key,
                                               const char *value,
                                               gui_sysval_response_cb_t cb,
                                               void *user_data);

#ifdef __cplusplus
}
#endif

#endif
