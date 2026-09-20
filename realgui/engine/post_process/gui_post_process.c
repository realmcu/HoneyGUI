/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include "gui_post_process.h"
#include "gui_api.h"
#include "gui_obj.h"
#include "gui_matrix.h"

typedef struct post_process_item
{
    post_process_event *event;
    struct post_process_item *next;
} T_PROCESS_ITEM;

static T_PROCESS_ITEM *process_list = NULL;
static T_PROCESS_ITEM *list_tail = NULL;

void post_process_add(post_process_event *event)
{
    bool new_list = false;
    if (process_list == NULL)
    {
        process_list = (T_PROCESS_ITEM *)gui_malloc(sizeof(T_PROCESS_ITEM));
        GUI_ASSERT(process_list != NULL);
        list_tail = process_list;
        new_list = true;
    }
    post_process_event *new_event = (post_process_event *)gui_malloc(sizeof(post_process_event));
    GUI_ASSERT(new_event != NULL);
    memcpy(new_event, event, sizeof(post_process_event));

    if (new_list)
    {
        list_tail->event = new_event;
        list_tail->next = NULL;
    }
    else
    {
        T_PROCESS_ITEM *new_item = (T_PROCESS_ITEM *)gui_malloc(sizeof(T_PROCESS_ITEM));
        GUI_ASSERT(new_item != NULL);
        list_tail->next = new_item;
        new_item->event = new_event;
        new_item->next = NULL;
        list_tail = new_item;
    }
}

void post_process_end(void)
{
    T_PROCESS_ITEM *current = process_list;
    while (current != NULL)
    {
        if (current->event != NULL)
        {
            if (current->event->param != NULL)
            {
                if (current->event->type == POST_PROCESS_BLUR)
                {
                    struct acc_engine *acc = gui_get_acc();
                    if ((acc != NULL) && (acc->blur != NULL) && (acc->blur->release != NULL))
                    {
                        post_process_blur_param *param = (post_process_blur_param *)current->event->param;
                        acc->blur->release(&param->cache_mem);
                    }
                }
                gui_free(current->event->param);
            }
            gui_free(current->event);
        }
        T_PROCESS_ITEM *next = current->next;
        gui_free(current);
        current = next;
    }
    process_list = NULL;
    list_tail = NULL;
}

static void post_process_blur_buffer(post_process_blur_param *param)
{
    struct acc_engine *acc = gui_get_acc();
    gui_dispdev_t *dc = gui_get_dc();
    if ((acc == NULL) || (acc->blur == NULL) || (acc->blur->process == NULL)
        || (dc == NULL) || (dc->frame_buf == NULL) || (param->blur_degree == 0U))
    {
        return;
    }

    gui_rect_t screen_rect = {0, 0, dc->screen_width - 1, dc->screen_height - 1};
    gui_rect_t target_rect = {0};
    gui_rect_t valid_rect = {0};
    if (!rect_intersect(&target_rect, &screen_rect, &param->area)
        || !rect_intersect(&valid_rect, &target_rect, &dc->section))
    {
        return;
    }

    uint16_t buffer_stride;
    gui_rect_t buffer_rect;
    if ((dc->type == DC_RAMLESS) || (dc->type == DC_PFB) || (dc->type == DC_PARTIAL_FB))
    {
        buffer_rect = dc->section;
        buffer_stride = dc->section.x2 - dc->section.x1 + 1;
    }
    else
    {
        buffer_rect = (gui_rect_t) {0, 0, dc->fb_width - 1, dc->fb_height - 1};
        buffer_stride = dc->fb_width;
    }

    acc->blur->process(dc->frame_buf, buffer_stride, dc->bit_depth,
                       &buffer_rect, &valid_rect, &target_rect,
                       param->blur_degree, param->cache_mem);
}

void post_process_handle(void)
{
    T_PROCESS_ITEM *current = process_list;
    while (current != NULL)
    {
        switch (current->event->type)
        {
        case POST_PROCESS_BLUR:
            {
                struct acc_engine *acc = gui_get_acc();
                post_process_blur_param *param = (post_process_blur_param *)current->event->param;
                if ((acc != NULL) && (acc->blur != NULL))
                {
                    if (acc->blur->prepare != NULL)
                    {
                        acc->blur->prepare(&param->cache_mem);
                    }
                    post_process_blur_buffer(param);
                }
                break;
            }
        default:
            break;
        }
        current = current->next;
    }
}

void pre_process_handle(post_process_event *event)
{
    switch (event->type)
    {
    case POST_PROCESS_BLUR:
        {
            struct acc_engine *acc = gui_get_acc();
            post_process_blur_param *param = (post_process_blur_param *)event->param;
            if ((acc != NULL) && (acc->blur != NULL))
            {
                post_process_blur_buffer(param);
            }
            break;
        }
    default:
        break;
    }
}
