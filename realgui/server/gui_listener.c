/**
 * @file gui_listener.c
 * @author howie_wang (howie_wang@realtek.com.cn)
 * @brief Message listener implementation
 * @version 0.1
 * @date 2025-12-25
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "gui_listener.h"
#include "gui_api.h"
#include "gui_message.h"
#include <string.h>

#define MAX_SUBSCRIPTIONS 32

typedef struct
{
    gui_obj_t *obj;
    char topic[64];
    gui_listener_cb_t callback;
    bool active;
} subscription_t;

static subscription_t subscriptions[MAX_SUBSCRIPTIONS];
static bool listener_initialized = false;

static void listener_init(void)
{
    if (listener_initialized)
    {
        return;
    }

    memset(subscriptions, 0, sizeof(subscriptions));
    listener_initialized = true;
}

void gui_msg_subscribe(gui_obj_t *obj, const char *topic, gui_listener_cb_t callback)
{
    if (!listener_initialized)
    {
        listener_init();
    }

    if (!obj || !topic || !callback)
    {
        return;
    }

    // Find empty slot
    for (int i = 0; i < MAX_SUBSCRIPTIONS; i++)
    {
        if (!subscriptions[i].active)
        {
            subscriptions[i].obj = obj;
            strncpy(subscriptions[i].topic, topic, sizeof(subscriptions[i].topic) - 1);
            subscriptions[i].topic[sizeof(subscriptions[i].topic) - 1] = '\0';
            subscriptions[i].callback = callback;
            subscriptions[i].active = true;
            return;
        }
    }

    // No free slot
    GUI_ASSERT(NULL);
}

void gui_msg_unsubscribe(gui_obj_t *obj, const char *topic)
{
    if (!obj || !topic)
    {
        return;
    }

    for (int i = 0; i < MAX_SUBSCRIPTIONS; i++)
    {
        if (subscriptions[i].active &&
            subscriptions[i].obj == obj &&
            strcmp(subscriptions[i].topic, topic) == 0)
        {
            subscriptions[i].active = false;
            return;
        }
    }
}

typedef struct
{
    gui_listener_cb_t callback;
    gui_obj_t *obj;
    char topic[64];
    void *data;
    uint16_t len;
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

    for (int i = 0; i < MAX_SUBSCRIPTIONS; i++)
    {
        if (subscriptions[i].active &&
            strcmp(subscriptions[i].topic, topic) == 0)
        {
            msg_task_t *task = gui_malloc(sizeof(msg_task_t));
            if (task)
            {
                task->callback = subscriptions[i].callback;
                task->obj = subscriptions[i].obj;
                strncpy(task->topic, topic, sizeof(task->topic) - 1);
                task->topic[sizeof(task->topic) - 1] = '\0';
                task->data = data;
                task->len = len;

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
