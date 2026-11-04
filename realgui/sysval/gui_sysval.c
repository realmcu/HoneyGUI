/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include <string.h>
#include "gui_api_os.h"
#include "gui_message.h"
#include "gui_server.h"
#include "gui_sysval.h"
#include "gui_sysval_adapter.h"

/*
 * Flow of one request:
 *
 *   gui_sysval_get_request()   GUI thread   allocate, call the adapter
 *   gui_sysval_respond()       any thread   store the result, move the
 *                                           request to the done list
 *   sysval_dispatch()          GUI thread   call the UI callback, free
 *
 * Flow of one published change:
 *
 *   gui_sysval_publish()       any thread   copy the value to the notify list
 *   sysval_dispatch()          GUI thread   call matching subscribers
 *
 * Neither response nor publish needs a message slot for the data itself, so an
 * answer or change is never lost. sysval_dispatch() runs from a server hook
 * every frame, and from a wake message when the GUI thread is idle.
 *
 * Every publication carries a serial number. A subscription starts at the
 * serial that was current when it was created and only receives higher ones,
 * so a change published before it subscribed is never replayed, while a change
 * still queued for this frame is. A value equal to the one last delivered for
 * a key is not delivered again, which keeps a writable key from feeding its
 * own change notification back into the widget that wrote it.
 */

struct gui_sysval_request
{
    struct gui_sysval_request *next;
    gui_sysval_request_id_t id;
    const gui_sysval_adapter_t *adapter;
    gui_sysval_type_t type;
    gui_sysval_response_cb_t cb;
    void *user_data;
    gui_sysval_status_t status;
    uint32_t serial_at_creation;
    bool is_get;
    bool has_value;
    gui_sysval_value_t value;
};

struct gui_sysval_subscription
{
    struct gui_sysval_subscription *next;
    const gui_sysval_adapter_t *adapter;
    gui_sysval_change_cb_t cb;
    void *user_data;
    uint32_t serial;
    bool active;
};

typedef struct gui_sysval_notification
{
    struct gui_sysval_notification *next;
    const gui_sysval_adapter_t *adapter;
    gui_sysval_value_t value;
    uint32_t serial;
} gui_sysval_notification_t;

/* Per-adapter bookkeeping. `last_value` and `has_last` belong to the GUI
 * thread. Every field a producer reads, `active_subscribers`, is only touched
 * with the queue lock held. */
typedef struct gui_sysval_adapter_state
{
    struct gui_sysval_adapter_state *next;
    const gui_sysval_adapter_t *adapter;
    gui_sysval_value_t last_value;
    uint32_t last_serial;
    bool has_last;
    int active_subscribers;
} gui_sysval_adapter_state_t;

static gui_sysval_adapter_t *sysval_adapter_head;
static gui_sysval_adapter_state_t *state_head;
static gui_sysval_request_id_t next_request_id = 1;
static struct gui_sysval_subscription *subscription_head;

/* Answered requests waiting for the GUI thread, oldest first. */
static gui_sysval_request_t *done_head;
static gui_sysval_request_t *done_tail;

/* Published values waiting for the GUI thread, coalesced by key. */
static gui_sysval_notification_t *notification_head;
static gui_sysval_notification_t *notification_tail;

/* Count of change callbacks running right now. A nested sysval_dispatch()
 * started from a callback must not clear the flag the outer one still owns,
 * and a subscription destroyed inside a callback stays queued as `active`
 * false until the outer dispatch finishes. */
static int subscription_dispatching;

/* Guards everything below: the done and notification lists, and the
 * publication serial. A one-slot queue holding a single token, or NULL when
 * the port has no message queue, which means a single thread. */
static void *done_lock;

/* Last serial handed out. Guarded by the queue lock. */
static uint32_t publish_serial;

static void sysval_lock(void)
{
    uint8_t token;

    if (done_lock != NULL)
    {
        gui_mq_recv(done_lock, &token, sizeof(token), 0xFFFFFFFF);
    }
}

static void sysval_unlock(void)
{
    uint8_t token = 0;

    if (done_lock != NULL)
    {
        gui_mq_send(done_lock, &token, sizeof(token), 0);
    }
}

static gui_sysval_adapter_state_t *sysval_state_find(const gui_sysval_adapter_t *adapter)
{
    gui_sysval_adapter_state_t *state;

    for (state = state_head; state != NULL; state = state->next)
    {
        if (state->adapter == adapter)
        {
            return state;
        }
    }

    return NULL;
}

static bool sysval_value_equal(const gui_sysval_value_t *a,
                               const gui_sysval_value_t *b)
{
    if (a->type != b->type)
    {
        return false;
    }

    switch (a->type)
    {
    case GUI_SYSVAL_TYPE_BOOL:
        return a->data.b == b->data.b;
    case GUI_SYSVAL_TYPE_INT:
        return a->data.i == b->data.i;
    case GUI_SYSVAL_TYPE_STR:
        return strncmp(a->data.s, b->data.s, GUI_SYSVAL_STR_MAX) == 0;
    }

    return false;
}

static void sysval_prune_subscriptions(void)
{
    struct gui_sysval_subscription **link = &subscription_head;

    while (*link != NULL)
    {
        struct gui_sysval_subscription *subscription = *link;

        if (subscription->active)
        {
            link = &subscription->next;
        }
        else
        {
            *link = subscription->next;
            gui_free(subscription);
        }
    }
}

/* GUI thread, queue lock held: the queued value for this adapter, if any. */
static gui_sysval_notification_t *sysval_notification_find(
    const gui_sysval_adapter_t *adapter)
{
    gui_sysval_notification_t *notification;

    for (notification = notification_head;
         notification != NULL;
         notification = notification->next)
    {
        if (notification->adapter == adapter)
        {
            return notification;
        }
    }

    return NULL;
}

static void sysval_notification_append(gui_sysval_notification_t *notification)
{
    if (notification_tail != NULL)
    {
        notification_tail->next = notification;
    }
    else
    {
        notification_head = notification;
    }
    notification_tail = notification;
}

/* A read that was overtaken by a change already delivered to subscribers
 * carries an older value than the one on screen, so it is reported as stale
 * instead of being handed to the UI. */
static gui_sysval_status_t sysval_request_status(const gui_sysval_request_t *request)
{
    gui_sysval_adapter_state_t *state;

    if (request->status != GUI_SYSVAL_STATUS_OK || !request->is_get ||
        !request->has_value)
    {
        return request->status;
    }

    state = sysval_state_find(request->adapter);
    if (state != NULL && state->has_last &&
        state->last_serial > request->serial_at_creation)
    {
        return GUI_SYSVAL_STATUS_STALE;
    }

    return request->status;
}

/* GUI thread: deliver answered requests and changed values. */
static void sysval_dispatch(void)
{
    gui_sysval_request_t *request;
    gui_sysval_request_t *next;
    gui_sysval_notification_t *notification;
    gui_sysval_notification_t *next_notification;

    sysval_lock();
    request = done_head;
    done_head = NULL;
    done_tail = NULL;
    notification = notification_head;
    notification_head = NULL;
    notification_tail = NULL;
    sysval_unlock();

    for (; request != NULL; request = next)
    {
        gui_sysval_status_t status = sysval_request_status(request);

        next = request->next;
        request->cb(request->id, status,
                    (status == GUI_SYSVAL_STATUS_OK && request->has_value)
                    ? &request->value : NULL,
                    request->user_data);
        gui_free(request);
    }

    if (notification == NULL)
    {
        return;
    }

    subscription_dispatching++;
    for (; notification != NULL; notification = next_notification)
    {
        struct gui_sysval_subscription *subscription;
        gui_sysval_adapter_state_t *state = sysval_state_find(notification->adapter);

        next_notification = notification->next;

        /* Republishing the value that is already on screen is not a change.
         * Delivering it would let a writable key report its own write back to
         * the widget that performed it. */
        if (state != NULL && state->has_last &&
            sysval_value_equal(&state->last_value, &notification->value))
        {
            gui_free(notification);
            continue;
        }

        for (subscription = subscription_head;
             subscription != NULL;
             subscription = subscription->next)
        {
            if (!subscription->active ||
                subscription->adapter != notification->adapter ||
                subscription->serial >= notification->serial)
            {
                continue;
            }

            subscription->serial = notification->serial;
            subscription->cb(notification->adapter->key,
                             &notification->value,
                             subscription->user_data);
        }

        if (state != NULL)
        {
            state->last_value = notification->value;
            state->last_serial = notification->serial;
            state->has_last = true;
        }

        gui_free(notification);
    }

    subscription_dispatching--;
    if (subscription_dispatching == 0)
    {
        sysval_prune_subscriptions();
    }
}

static void sysval_wake(void *msg)
{
    GUI_UNUSED(msg);
    sysval_dispatch();
}

/* GUI thread, before the first adapter runs. */
static void sysval_init(void)
{
    static bool inited;
    uint8_t token = 0;

    if (inited)
    {
        return;
    }
    inited = true;

    gui_register_server_hook(sysval_dispatch);

    if (gui_mq_create(&done_lock, "sysval", sizeof(token), 1) &&
        !gui_mq_send(done_lock, &token, sizeof(token), 0))
    {
        done_lock = NULL;
    }
}

static const gui_sysval_adapter_t *sysval_adapter_find(const char *key)
{
    gui_sysval_adapter_t *adapter;

    for (adapter = sysval_adapter_head;
         adapter != NULL;
         adapter = adapter->next)
    {
        if (strcmp(adapter->key, key) == 0)
        {
            return adapter;
        }
    }

    return NULL;
}

static gui_sysval_request_id_t sysval_request_id_allocate(void)
{
    gui_sysval_request_id_t id = next_request_id++;

    if (next_request_id == GUI_SYSVAL_REQUEST_INVALID)
    {
        next_request_id = 1;
    }

    return id;
}

static bool sysval_type_valid(gui_sysval_type_t type)
{
    return type == GUI_SYSVAL_TYPE_BOOL ||
           type == GUI_SYSVAL_TYPE_INT ||
           type == GUI_SYSVAL_TYPE_STR;
}

static gui_sysval_request_t *sysval_request_create(const gui_sysval_adapter_t *adapter,
                                                   bool is_get,
                                                   gui_sysval_response_cb_t cb,
                                                   void *user_data)
{
    gui_sysval_request_t *request = gui_malloc(sizeof(*request));

    if (request == NULL)
    {
        return NULL;
    }

    memset(request, 0, sizeof(*request));
    request->id = sysval_request_id_allocate();
    request->adapter = adapter;
    request->type = adapter->type;
    request->cb = cb;
    request->user_data = user_data;
    request->is_get = is_get;

    sysval_lock();
    request->serial_at_creation = publish_serial;
    sysval_unlock();

    return request;
}

void gui_sysval_respond(gui_sysval_request_t *request,
                        gui_sysval_status_t status,
                        const gui_sysval_value_t *value)
{
    gui_msg_t msg = {0};
    bool was_empty;

    if (request == NULL)
    {
        return;
    }

    request->status = status;
    request->has_value = false;

    if (value != NULL)
    {
        /* A value of the wrong type is an adapter bug. Report it rather
         * than hand the UI a union it would read through the wrong member. */
        if (value->type != request->type)
        {
            request->status = GUI_SYSVAL_STATUS_ERROR;
        }
        else
        {
            request->has_value = true;
            request->value = *value;
            if (value->type == GUI_SYSVAL_TYPE_STR)
            {
                request->value.data.s[GUI_SYSVAL_STR_MAX - 1] = '\0';
            }
        }
    }

    sysval_lock();
    was_empty = (done_head == NULL);
    request->next = NULL;
    if (done_tail != NULL)
    {
        done_tail->next = request;
    }
    else
    {
        done_head = request;
    }
    done_tail = request;
    sysval_unlock();

    /* Wake a GUI thread that is idle. If the queue is full the GUI thread
     * is busy anyway and the server hook picks the request up. */
    if (was_empty)
    {
        msg.event = GUI_EVENT_USER_DEFINE;
        msg.cb = sysval_wake;
        gui_send_msg_to_server(&msg);
    }
}

gui_sysval_value_t gui_sysval_value_bool(bool b)
{
    gui_sysval_value_t value;

    memset(&value, 0, sizeof(value));
    value.type = GUI_SYSVAL_TYPE_BOOL;
    value.data.b = b;
    return value;
}

gui_sysval_value_t gui_sysval_value_int(int32_t i)
{
    gui_sysval_value_t value;

    memset(&value, 0, sizeof(value));
    value.type = GUI_SYSVAL_TYPE_INT;
    value.data.i = i;
    return value;
}

gui_sysval_value_t gui_sysval_value_str(const char *s)
{
    gui_sysval_value_t value;

    memset(&value, 0, sizeof(value));
    value.type = GUI_SYSVAL_TYPE_STR;
    if (s != NULL)
    {
        strncpy(value.data.s, s, GUI_SYSVAL_STR_MAX - 1);
    }
    return value;
}

bool gui_sysval_adapter_register(gui_sysval_adapter_t *adapter)
{
    gui_sysval_adapter_state_t *state;

    if (adapter == NULL || adapter->key == NULL || adapter->key[0] == '\0' ||
        !sysval_type_valid(adapter->type) ||
        (adapter->get_request == NULL && adapter->set_request == NULL) ||
        sysval_adapter_find(adapter->key) != NULL)
    {
        return false;
    }

    state = gui_malloc(sizeof(*state));
    if (state == NULL)
    {
        return false;
    }

    sysval_init();

    memset(state, 0, sizeof(*state));
    state->adapter = adapter;
    state->next = state_head;
    state_head = state;

    adapter->next = sysval_adapter_head;
    sysval_adapter_head = adapter;
    return true;
}

gui_sysval_subscription_t gui_sysval_subscribe(const char *key,
                                               gui_sysval_change_cb_t cb,
                                               void *user_data)
{
    const gui_sysval_adapter_t *adapter;
    gui_sysval_adapter_state_t *state;
    struct gui_sysval_subscription *subscription;

    if (key == NULL || key[0] == '\0' || cb == NULL)
    {
        return NULL;
    }

    adapter = sysval_adapter_find(key);
    if (adapter == NULL)
    {
        return NULL;
    }

    state = sysval_state_find(adapter);
    if (state == NULL)
    {
        return NULL;
    }

    subscription = gui_malloc(sizeof(*subscription));
    if (subscription == NULL)
    {
        return NULL;
    }

    memset(subscription, 0, sizeof(*subscription));
    subscription->adapter = adapter;
    subscription->cb = cb;
    subscription->user_data = user_data;
    subscription->active = true;

    sysval_lock();
    subscription->serial = publish_serial;
    subscription->next = subscription_head;
    subscription_head = subscription;
    state->active_subscribers++;
    sysval_unlock();

    return subscription;
}

void gui_sysval_unsubscribe(gui_sysval_subscription_t subscription)
{
    gui_sysval_adapter_state_t *state;

    if (subscription == NULL || !subscription->active)
    {
        return;
    }

    subscription->active = false;
    state = sysval_state_find(subscription->adapter);
    sysval_lock();
    if (state != NULL)
    {
        state->active_subscribers--;
    }
    sysval_unlock();

    /* The handle itself is only released by prune, so a subscription that is
     * already inactive ignores a second call. */
    if (subscription_dispatching == 0)
    {
        sysval_prune_subscriptions();
    }
}

bool gui_sysval_publish(const char *key,
                        const gui_sysval_value_t *value)
{
    const gui_sysval_adapter_t *adapter;
    gui_sysval_adapter_state_t *state;
    gui_sysval_notification_t *notification;
    gui_sysval_notification_t *queued;
    gui_msg_t msg = {0};
    bool was_empty;
    bool listening;
    bool wake;
    uint32_t serial;

    if (key == NULL || key[0] == '\0' || value == NULL)
    {
        return false;
    }

    adapter = sysval_adapter_find(key);
    if (adapter == NULL || value->type != adapter->type)
    {
        return false;
    }

    state = sysval_state_find(adapter);
    if (state == NULL)
    {
        return false;
    }

    /* Nobody is listening, so this change has no future subscriber to reach:
     * a subscription only starts at the values published after it. Skipping
     * the queue here also keeps an unlistened key from taking a slot and from
     * being remembered as "already delivered" once someone does subscribe. */
    sysval_lock();
    listening = (state->active_subscribers > 0);
    sysval_unlock();
    if (!listening)
    {
        return true;
    }

    /* A value already queued for this key is replaced, so a burst of changes
     * reaches the GUI thread as one delivery of the latest value. */
    sysval_lock();
    queued = sysval_notification_find(adapter);
    if (queued != NULL)
    {
        queued->value = *value;
        queued->serial = ++publish_serial;
        if (queued->value.type == GUI_SYSVAL_TYPE_STR)
        {
            queued->value.data.s[GUI_SYSVAL_STR_MAX - 1] = '\0';
        }
        sysval_unlock();
        return true;
    }

    serial = ++publish_serial;
    was_empty = (notification_head == NULL);
    sysval_unlock();

    /* Allocated outside the lock: gui_malloc() takes the heap lock, which must
     * not nest inside the queue lock. */
    notification = gui_malloc(sizeof(*notification));
    if (notification == NULL)
    {
        /* Nothing to retry with. The producer sees the failure and can publish
         * again once memory is available. */
        return false;
    }

    memset(notification, 0, sizeof(*notification));
    notification->adapter = adapter;
    notification->value = *value;
    notification->serial = serial;
    if (notification->value.type == GUI_SYSVAL_TYPE_STR)
    {
        notification->value.data.s[GUI_SYSVAL_STR_MAX - 1] = '\0';
    }

    sysval_lock();
    /* Another producer may have queued this key while the lock was released.
     * Fold into that entry rather than queueing twice. */
    queued = sysval_notification_find(adapter);
    if (queued != NULL)
    {
        queued->value = notification->value;
        queued->serial = serial;
        sysval_unlock();
        gui_free(notification);
        return true;
    }

    sysval_notification_append(notification);
    wake = was_empty && (state->active_subscribers > 0);
    sysval_unlock();

    /* If the queue is full, the GUI thread is busy and its server hook will
     * pick the notification up on the next frame. A key nobody listens to does
     * not wake the GUI thread at all. */
    if (wake)
    {
        msg.event = GUI_EVENT_USER_DEFINE;
        msg.cb = sysval_wake;
        gui_send_msg_to_server(&msg);
    }

    return true;
}

gui_sysval_request_id_t gui_sysval_get_request(const char *key,
                                               gui_sysval_response_cb_t cb,
                                               void *user_data)
{
    const gui_sysval_adapter_t *adapter;
    gui_sysval_request_t *request;
    gui_sysval_request_id_t request_id;

    if (key == NULL || key[0] == '\0' || cb == NULL)
    {
        return GUI_SYSVAL_REQUEST_INVALID;
    }

    adapter = sysval_adapter_find(key);
    if (adapter == NULL || adapter->get_request == NULL)
    {
        return GUI_SYSVAL_REQUEST_INVALID;
    }

    request = sysval_request_create(adapter, true, cb, user_data);
    if (request == NULL)
    {
        return GUI_SYSVAL_REQUEST_INVALID;
    }

    /* The adapter may answer from another thread straight away, after
     * which the request belongs to the dispatcher: read the ID first. */
    request_id = request->id;
    adapter->get_request(request);
    return request_id;
}

gui_sysval_request_id_t gui_sysval_set_request(const char *key,
                                               const gui_sysval_value_t *value,
                                               gui_sysval_response_cb_t cb,
                                               void *user_data)
{
    const gui_sysval_adapter_t *adapter;
    gui_sysval_request_t *request;
    gui_sysval_request_id_t request_id;

    if (key == NULL || key[0] == '\0' || value == NULL ||
        cb == NULL)
    {
        return GUI_SYSVAL_REQUEST_INVALID;
    }

    adapter = sysval_adapter_find(key);
    if (adapter == NULL || adapter->set_request == NULL ||
        value->type != adapter->type)
    {
        return GUI_SYSVAL_REQUEST_INVALID;
    }

    request = sysval_request_create(adapter, false, cb, user_data);
    if (request == NULL)
    {
        return GUI_SYSVAL_REQUEST_INVALID;
    }

    request_id = request->id;
    adapter->set_request(value, request);
    return request_id;
}
