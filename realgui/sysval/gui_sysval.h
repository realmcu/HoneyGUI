/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/**
 * @file gui_sysval.h
 * @brief System value request API.
 */

#ifndef __GUI_SYSVAL_H__
#define __GUI_SYSVAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "gui_sysval_keys.h"


/*============================================================================*
 *                         Types
 *============================================================================*/

/** Longest string value, including the terminating NUL. */
#define GUI_SYSVAL_STR_MAX 32

typedef enum
{
    GUI_SYSVAL_TYPE_BOOL = 0,
    GUI_SYSVAL_TYPE_INT,
    GUI_SYSVAL_TYPE_STR,
} gui_sysval_type_t;

/**
 * @brief A typed system value.
 *
 * Every key has one fixed type, documented in gui_sysval_keys.h. The value is
 * small and fixed-size, so it can be copied by assignment.
 */
typedef struct
{
    gui_sysval_type_t type;
    union
    {
        bool b;
        int32_t i;
        char s[GUI_SYSVAL_STR_MAX];
    } data;
} gui_sysval_value_t;

typedef uint32_t gui_sysval_request_id_t;

#define GUI_SYSVAL_REQUEST_INVALID ((gui_sysval_request_id_t)0)

/**
 * @brief Result of a request that was started.
 *
 * Only a started request reports a status. A request that cannot start, for
 * example an unknown key, a read-only key or a value of the wrong type, is
 * rejected with GUI_SYSVAL_REQUEST_INVALID instead and never calls back.
 */
typedef enum
{
    GUI_SYSVAL_STATUS_OK = 0,
    GUI_SYSVAL_STATUS_INVALID_VALUE,    /**< Right type, but out of range */
    GUI_SYSVAL_STATUS_BUSY,             /**< Backend cannot take it now */
    GUI_SYSVAL_STATUS_TIMEOUT,          /**< Backend gave up waiting */
    GUI_SYSVAL_STATUS_ERROR,            /**< Any other failure */
} gui_sysval_status_t;

/**
 * @brief Called when an asynchronous get or set request completes.
 *
 * For a get request, @p value contains the requested value. For a successful
 * set request, it may contain the value that actually took effect. When
 * @p status is GUI_SYSVAL_STATUS_OK and @p value is not NULL, its type is the
 * type of the key. The value is valid only for the duration of the callback.
 *
 * @param request_id Completed request.
 * @param status     Final request status.
 * @param value      Response value, or NULL when no value is available.
 * @param user_data  Value supplied with the request.
 */
typedef void (*gui_sysval_response_cb_t)(gui_sysval_request_id_t request_id,
                                         gui_sysval_status_t status,
                                         const gui_sysval_value_t *value,
                                         void *user_data);


/*============================================================================*
 *                         Functions
 *============================================================================*/

/** @brief Make a boolean value. */
gui_sysval_value_t gui_sysval_value_bool(bool b);

/** @brief Make an integer value. */
gui_sysval_value_t gui_sysval_value_int(int32_t i);

/**
 * @brief Make a string value.
 *
 * @p s is truncated to GUI_SYSVAL_STR_MAX - 1 characters. NULL is treated as
 * an empty string.
 */
gui_sysval_value_t gui_sysval_value_str(const char *s);

/**
 * @brief Start an asynchronous read.
 *
 * The key is defined by HoneyGUI or an extension module. If this returns a
 * valid ID, @p cb is called exactly once, on the GUI thread, after this
 * function has returned -- even when the backend has the value at hand. If
 * it returns GUI_SYSVAL_REQUEST_INVALID, @p cb is never called.
 *
 * @param key       System value key.
 * @param cb        Completion callback.
 * @param user_data Value passed to @p cb.
 * @return Request ID, or GUI_SYSVAL_REQUEST_INVALID if the arguments are
 *         invalid, the key is unknown or not readable, or out of memory.
 */
gui_sysval_request_id_t gui_sysval_get_request(const char *key,
                                               gui_sysval_response_cb_t cb,
                                               void *user_data);

/**
 * @brief Start an asynchronous write.
 *
 * Same callback rules as gui_sysval_get_request(). The key and value are only
 * read during this call. A value of the right type but out of range is
 * reported through @p cb as GUI_SYSVAL_STATUS_INVALID_VALUE.
 *
 * @param key       System value key.
 * @param value     New value. Its type must match the type of the key.
 * @param cb        Completion callback.
 * @param user_data Value passed to @p cb.
 * @return Request ID, or GUI_SYSVAL_REQUEST_INVALID if the arguments are
 *         invalid, the key is unknown or read-only, the value type does not
 *         match the key, or out of memory.
 */
gui_sysval_request_id_t gui_sysval_set_request(const char *key,
                                               const gui_sysval_value_t *value,
                                               gui_sysval_response_cb_t cb,
                                               void *user_data);

#ifdef __cplusplus
}
#endif

#endif
