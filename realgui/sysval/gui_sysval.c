/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include <string.h>
#include "gui_api_os.h"
#include "gui_message.h"
#include "gui_sysval.h"
#include "gui_sysval_provider.h"

typedef struct
{
    gui_sysval_request_id_t id;
    gui_sysval_type_t type;
    gui_sysval_response_cb_t callback;
    void *user_data;
} gui_sysval_request_t;

typedef struct
{
    gui_sysval_request_t *request;
    gui_sysval_status_t status;
    bool has_value;
    gui_sysval_value_t value;
} gui_sysval_response_t;

static gui_sysval_handler_t *sysval_handler_head;
static gui_sysval_request_id_t next_request_id = 1;

static const gui_sysval_handler_t *sysval_handler_find(const char *key)
{
    gui_sysval_handler_t *handler;

    for (handler = sysval_handler_head;
         handler != NULL;
         handler = handler->next)
    {
        if (strcmp(handler->key, key) == 0)
        {
            return handler;
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

static gui_sysval_request_t *sysval_request_create(gui_sysval_type_t type,
                                                   gui_sysval_response_cb_t callback,
                                                   void *user_data)
{
    gui_sysval_request_t *request = gui_malloc(sizeof(*request));

    if (request == NULL)
    {
        return NULL;
    }

    request->id = sysval_request_id_allocate();
    request->type = type;
    request->callback = callback;
    request->user_data = user_data;

    return request;
}

static void sysval_response_free(gui_sysval_response_t *response)
{
    gui_free(response->request);
    gui_free(response);
}

static void sysval_response_dispatch(void *message)
{
    gui_msg_t *msg = (gui_msg_t *)message;
    gui_sysval_response_t *response = (gui_sysval_response_t *)msg->payload;
    gui_sysval_request_t *request = response->request;

    request->callback(request->id, response->status,
                      response->has_value ? &response->value : NULL,
                      request->user_data);
    sysval_response_free(response);
}

static void sysval_request_complete(void *request_context,
                                    gui_sysval_status_t status,
                                    const gui_sysval_value_t *value)
{
    gui_sysval_request_t *request = (gui_sysval_request_t *)request_context;
    gui_sysval_response_t *response;
    gui_msg_t msg;

    if (request == NULL)
    {
        return;
    }

    response = gui_malloc(sizeof(*response));
    if (response == NULL)
    {
        gui_free(request);
        return;
    }

    response->request = request;
    response->status = status;
    response->has_value = false;

    if (value != NULL)
    {
        /* A value of the wrong type is a provider bug. Report it rather
         * than hand the UI a union it would read through the wrong member. */
        if (value->type != request->type)
        {
            response->status = GUI_SYSVAL_STATUS_ERROR;
        }
        else
        {
            response->has_value = true;
            response->value = *value;
            if (value->type == GUI_SYSVAL_TYPE_STR)
            {
                response->value.data.s[GUI_SYSVAL_STR_MAX - 1] = '\0';
            }
        }
    }

    msg.event = GUI_EVENT_USER_DEFINE;
    msg.sub_event = 0;
    msg.cb = sysval_response_dispatch;
    msg.payload = response;

    if (!gui_send_msg_to_server(&msg))
    {
        sysval_response_free(response);
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

bool gui_sysval_handler_register(gui_sysval_handler_t *handler)
{
    if (handler == NULL || handler->key == NULL || handler->key[0] == '\0' ||
        !sysval_type_valid(handler->type) ||
        (handler->get_request == NULL && handler->set_request == NULL) ||
        sysval_handler_find(handler->key) != NULL)
    {
        return false;
    }

    handler->next = sysval_handler_head;
    sysval_handler_head = handler;
    return true;
}

gui_sysval_request_id_t gui_sysval_get_request(const char *key,
                                               gui_sysval_response_cb_t cb,
                                               void *user_data)
{
    const gui_sysval_handler_t *handler;
    gui_sysval_request_t *request;
    gui_sysval_request_id_t request_id;

    if (key == NULL || key[0] == '\0' || cb == NULL)
    {
        return GUI_SYSVAL_REQUEST_INVALID;
    }

    handler = sysval_handler_find(key);
    if (handler == NULL || handler->get_request == NULL)
    {
        return GUI_SYSVAL_REQUEST_INVALID;
    }

    request = sysval_request_create(handler->type, cb, user_data);
    if (request == NULL)
    {
        return GUI_SYSVAL_REQUEST_INVALID;
    }
    request_id = request->id;

    if (!handler->get_request(sysval_request_complete, request))
    {
        gui_free(request);
        return GUI_SYSVAL_REQUEST_INVALID;
    }

    return request_id;
}

gui_sysval_request_id_t gui_sysval_set_request(const char *key,
                                               const gui_sysval_value_t *value,
                                               gui_sysval_response_cb_t cb,
                                               void *user_data)
{
    const gui_sysval_handler_t *handler;
    gui_sysval_request_t *request;
    gui_sysval_request_id_t request_id;

    if (key == NULL || key[0] == '\0' || value == NULL ||
        cb == NULL)
    {
        return GUI_SYSVAL_REQUEST_INVALID;
    }

    handler = sysval_handler_find(key);
    if (handler == NULL || handler->set_request == NULL ||
        value->type != handler->type)
    {
        return GUI_SYSVAL_REQUEST_INVALID;
    }

    request = sysval_request_create(handler->type, cb, user_data);
    if (request == NULL)
    {
        return GUI_SYSVAL_REQUEST_INVALID;
    }
    request_id = request->id;

    if (!handler->set_request(value, sysval_request_complete, request))
    {
        gui_free(request);
        return GUI_SYSVAL_REQUEST_INVALID;
    }

    return request_id;
}
