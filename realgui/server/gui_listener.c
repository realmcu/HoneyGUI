/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include "gui_listener.h"
#include "gui_api.h"
#include "gui_message.h"
#include <string.h>

typedef struct subscription
{
    struct subscription *next;
    gui_obj_t *obj;
    gui_listener_cb_t callback;
    char topic[];
} subscription_t;

static subscription_t *sub_head = NULL;

void gui_msg_subscribe(gui_obj_t *obj, const char *topic, gui_listener_cb_t callback)
{
    if (!obj || !topic || !callback)
    {
        return;
    }

    size_t topic_len = strlen(topic) + 1;
    subscription_t *sub = gui_malloc(sizeof(subscription_t) + topic_len);
    if (!sub)
    {
        return;
    }

    obj->has_subscribe = true;
    sub->obj = obj;
    sub->callback = callback;
    memcpy(sub->topic, topic, topic_len);
    sub->next = sub_head;
    sub_head = sub;
}

void gui_msg_unsubscribe(gui_obj_t *obj, const char *topic)
{
    if (!obj)
    {
        return;
    }

    subscription_t **pp = &sub_head;
    if (topic != NULL)
    {
        while (*pp)
        {
            subscription_t *cur = *pp;
            if (cur->obj == obj && strcmp(cur->topic, topic) == 0)
            {
                *pp = cur->next;
                gui_free(cur);
                return;
            }
            pp = &cur->next;
        }
    }
    else
    {
        while (*pp)
        {
            subscription_t *cur = *pp;
            if (cur->obj == obj)
            {
                *pp = cur->next;
                gui_free(cur);
            }
            pp = &cur->next;
        }
        obj->has_subscribe = false;
    }
}

typedef struct
{
    gui_listener_cb_t callback;
    gui_obj_t *obj;
    void *data;
    uint16_t len;
    char topic[];
} msg_task_t;

static void msg_task_exec(void *p)
{
    msg_task_t *task = (msg_task_t *)p;
    if (task && task->callback)
    {
        task->callback(task->obj, task->topic, task->data, task->len);
    }
    gui_free(task);
}

void gui_msg_publish(const char *topic, void *data, uint16_t len)
{
    if (!topic)
    {
        return;
    }

    for (subscription_t *sub = sub_head; sub; sub = sub->next)
    {
        if (strcmp(sub->topic, topic) == 0)
        {
            size_t topic_len = strlen(topic) + 1;
            msg_task_t *task = gui_malloc(sizeof(msg_task_t) + topic_len);
            if (task)
            {
                task->callback = sub->callback;
                task->obj = sub->obj;
                task->data = data;
                task->len = len;
                memcpy(task->topic, topic, topic_len);

                gui_msg_t msg =
                {
                    .event = GUI_EVENT_SUBSCRIBE_TOPIC,
                    .cb = msg_task_exec,
                    .payload = task
                };
                gui_send_msg_to_server(&msg);
            }
        }
    }
}
