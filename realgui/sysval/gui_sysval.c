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
 * gui_sysval_respond() neither allocates nor needs a message slot, so an
 * answer is never lost. sysval_dispatch() runs from a server hook every
 * frame, and from a wake message when the GUI thread is idle.
 */

struct gui_sysval_request
{
    struct gui_sysval_request *next;
    gui_sysval_request_id_t id;
    gui_sysval_type_t type;
    gui_sysval_response_cb_t cb;
    void *user_data;
    gui_sysval_status_t status;
    bool has_value;
    gui_sysval_value_t value;
};

static gui_sysval_adapter_t *sysval_adapter_head;
static gui_sysval_request_id_t next_request_id = 1;

/* Answered requests waiting for the GUI thread, oldest first. */
static gui_sysval_request_t *done_head;
static gui_sysval_request_t *done_tail;

/* Guards the done list: a one-slot queue holding a single token. NULL when
 * the port has no message queue, which means a single thread. */
static void *done_lock;

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

/* GUI thread: deliver every answered request. */
static void sysval_dispatch(void)
{
    gui_sysval_request_t *request;
    gui_sysval_request_t *next;

    sysval_lock();
    request = done_head;
    done_head = NULL;
    done_tail = NULL;
    sysval_unlock();

    for (; request != NULL; request = next)
    {
        next = request->next;
        request->cb(request->id, request->status,
                    request->has_value ? &request->value : NULL,
                    request->user_data);
        gui_free(request);
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
    request->type = adapter->type;
    request->cb = cb;
    request->user_data = user_data;
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
    if (adapter == NULL || adapter->key == NULL || adapter->key[0] == '\0' ||
        !sysval_type_valid(adapter->type) ||
        (adapter->get_request == NULL && adapter->set_request == NULL) ||
        sysval_adapter_find(adapter->key) != NULL)
    {
        return false;
    }

    sysval_init();

    adapter->next = sysval_adapter_head;
    sysval_adapter_head = adapter;
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

    request = sysval_request_create(adapter, cb, user_data);
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

    request = sysval_request_create(adapter, cb, user_data);
    if (request == NULL)
    {
        return GUI_SYSVAL_REQUEST_INVALID;
    }

    request_id = request->id;
    adapter->set_request(value, request);
    return request_id;
}
