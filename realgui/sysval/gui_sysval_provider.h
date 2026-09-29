/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/**
 * @file gui_sysval_provider.h
 * @brief Provider contract for system value requests.
 */

#ifndef __GUI_SYSVAL_PROVIDER_H__
#define __GUI_SYSVAL_PROVIDER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "gui_sysval.h"

/**
 * @brief Complete a request accepted by the platform adapter.
 *
 * HoneyGUI copies the value before this callback returns. A successful
 * response must carry a value of the handler's type, otherwise it is reported
 * to the caller as GUI_SYSVAL_STATUS_ERROR.
 */
typedef void (*gui_sysval_complete_cb_t)(void *request_context,
                                         gui_sysval_status_t status,
                                         const gui_sysval_value_t *value);

typedef struct gui_sysval_handler
{
    const char *key;

    /** Type of every value read from or written to this key. */
    gui_sysval_type_t type;

    /**
     * @brief Start reading a system value.
     *
     * Invoke @p complete exactly once after accepting the request.
     */
    bool (*get_request)(gui_sysval_complete_cb_t complete,
                        void *request_context);

    /**
     * @brief Start writing a system value.
     *
     * HoneyGUI has already checked that @p value has the handler's type;
     * range checks remain the handler's job. The value is valid only for the
     * duration of this call. Invoke @p complete exactly once after accepting
     * the request.
     */
    bool (*set_request)(const gui_sysval_value_t *value,
                        gui_sysval_complete_cb_t complete,
                        void *request_context);

    struct gui_sysval_handler *next;
} gui_sysval_handler_t;

/**
 * @brief Register a handler for one system value key.
 *
 * The handler and its key must remain valid after registration.
 *
 * @return true when the handler was registered.
 */
bool gui_sysval_handler_register(gui_sysval_handler_t *handler);

#ifdef __cplusplus
}
#endif

#endif
