/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/**
 * @file gui_sysval_adapter.h
 * @brief Adapter contract for system value requests.
 *
 * An adapter receives a request and answers it with gui_sysval_respond(),
 * exactly once. It may answer inside get_request (see
 * gui_port_sysval_sync.c) or keep the request pointer and answer later from
 * any thread (see gui_port_sysval_async.c). HoneyGUI always delivers the
 * answer to the UI on the GUI thread.
 */

#ifndef __GUI_SYSVAL_ADAPTER_H__
#define __GUI_SYSVAL_ADAPTER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "gui_sysval.h"

/** A request in flight. Owned by HoneyGUI; valid until it is answered. */
typedef struct gui_sysval_request gui_sysval_request_t;

typedef struct gui_sysval_adapter
{
    const char *key;

    /** Type of every value read from or written to this key. */
    gui_sysval_type_t type;

    /** Start a read. Leave NULL for a write-only key. */
    void (*get_request)(gui_sysval_request_t *request);

    /**
     * @brief Start a write. Leave NULL for a read-only key.
     *
     * HoneyGUI has already checked that @p value has the adapter's type;
     * range checks are the adapter's job. @p value is valid only for the
     * duration of this call.
     */
    void (*set_request)(const gui_sysval_value_t *value,
                        gui_sysval_request_t *request);

    /** Internal, set by gui_sysval_adapter_register(). Leave it out. */
    struct gui_sysval_adapter *next;
} gui_sysval_adapter_t;

/**
 * @brief Register an adapter for one system value key.
 *
 * The adapter and its key must remain valid after registration.
 *
 * @return true when the adapter was registered.
 */
bool gui_sysval_adapter_register(gui_sysval_adapter_t *adapter);

/**
 * @brief Answer a request.
 *
 * Call exactly once per request, from any thread. @p request must not be
 * used afterwards. A successful answer must carry a value of the key's type,
 * otherwise the UI receives GUI_SYSVAL_STATUS_ERROR.
 *
 * @param request Request received by get_request or set_request.
 * @param status  Result.
 * @param value   Value, or NULL when there is none. Copied before return.
 */
void gui_sysval_respond(gui_sysval_request_t *request,
                        gui_sysval_status_t status,
                        const gui_sysval_value_t *value);

#ifdef __cplusplus
}
#endif

#endif
