/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/**
 * @file gui_sysval.h
 * @brief System value request and change subscription API.
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
 *
 * A read may come back as GUI_SYSVAL_STATUS_STALE when a change for the same
 * key was published and delivered while the read was in flight: the answer is
 * older than what subscribers already have. The value is still passed so the
 * caller can decide, but a UI that also subscribes should keep the newer one.
 */
typedef enum
{
    GUI_SYSVAL_STATUS_OK = 0,
    GUI_SYSVAL_STATUS_INVALID_VALUE,    /**< Right type, but out of range */
    GUI_SYSVAL_STATUS_BUSY,             /**< Backend cannot take it now */
    GUI_SYSVAL_STATUS_TIMEOUT,          /**< Backend gave up waiting */
    GUI_SYSVAL_STATUS_STALE,            /**< Read overtaken by a published change */
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

/**
 * Subscription handle returned by gui_sysval_subscribe().
 *
 * The handle is invalid once gui_sysval_unsubscribe() returns. Keep it for as
 * long as the subscription is wanted: it is the only way to cancel it.
 */
typedef struct gui_sysval_subscription *gui_sysval_subscription_t;

/**
 * @brief Called when a subscribed system value changes.
 *
 * The callback runs on the GUI thread. @p key identifies the changed value and
 * remains valid for the lifetime of the adapter. @p value is a copied snapshot
 * and is valid only for the duration of the callback. Changes published before
 * the GUI thread dispatches them may be coalesced so that only the latest value
 * for a key is delivered, and a value equal to the last one dispatched for the
 * key is dropped. Order is kept per key only: two keys published in one frame
 * may be delivered in either order.
 *
 * @param key       Changed system value key.
 * @param value     Latest value snapshot.
 * @param user_data Value supplied when subscribing.
 */
typedef void (*gui_sysval_change_cb_t)(const char *key,
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

/**
 * @brief Subscribe to changes of one system value key.
 *
 * Must be called on the GUI thread. The key must already have a registered
 * adapter. This only subscribes to future changes; use gui_sysval_get_request()
 * once after subscribing when the current value is also needed.
 *
 * The subscription stays active until gui_sysval_unsubscribe() is called. A
 * callback that touches @p user_data must be cancelled before that data is
 * released; nothing in HoneyGUI can tell that it is gone.
 *
 * A change published before this call may still be delivered if it is already
 * queued for the GUI thread; each key is coalesced by adapter, not by
 * subscriber. A value equal to the last one dispatched for the key is not
 * delivered again.
 *
 * @param key       System value key.
 * @param cb        Change callback.
 * @param user_data Value passed to @p cb.
 * @return Subscription handle, or NULL when the arguments are invalid, the key
 *         is unknown, or memory is unavailable.
 */
gui_sysval_subscription_t gui_sysval_subscribe(const char *key,
                                               gui_sysval_change_cb_t cb,
                                               void *user_data);

/**
 * @brief Cancel a system value subscription.
 *
 * Must be called on the GUI thread. Passing NULL is allowed. Calling this from
 * a subscription callback is safe; delivery stops as soon as the callback
 * returns and the handle is released when dispatch finishes. Do not reuse the
 * handle afterwards, and do not pass the same handle twice.
 *
 * @param subscription Handle returned by gui_sysval_subscribe().
 */
void gui_sysval_unsubscribe(gui_sysval_subscription_t subscription);


/*============================================================================*
 *                         Data source
 *============================================================================*/

/**
 * @brief Publish a changed system value to GUI subscribers.
 *
 * Call this from any thread after the value a producer owns has changed. The
 * value is copied before this function returns. Multiple changes to the same
 * key may be coalesced before GUI dispatch, in which case subscribers receive
 * only the latest value; order is only kept per key.
 *
 * A value equal to the one currently on screen is not delivered again, so a
 * writable key may publish unconditionally without its own change coming back
 * to the widget that made it. Publishing a key nobody subscribes to does not
 * wake the GUI thread.
 *
 * The key must have a registered adapter and @p value must match its type.
 * Publishing does not update the adapter's backing store: the producer must
 * publish every change it makes, and get_request() must return the same value
 * the last publish carried. A read that an in-flight change overtook is
 * reported to the UI as GUI_SYSVAL_STATUS_STALE rather than silently applied.
 *
 * @param key   System value key.
 * @param value New value snapshot.
 * @return true when the change was queued or superseded a queued change, false
 *         when the arguments are invalid, the key has no adapter, or memory ran
 *         out. A failed publish is not retried; publish again when the next
 *         change happens.
 */
bool gui_sysval_publish(const char *key,
                        const gui_sysval_value_t *value);

#ifdef __cplusplus
}
#endif

#endif
