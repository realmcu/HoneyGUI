/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <string.h>
#include "guidef.h"
#include "acc_api.h"
#include "gui_server.h"
#include "gui_obj.h"
#include "gui_img.h"
#include "gui_view.h"
#include "gui_view_api.h"

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static void gui_view_move_childlist(gui_obj_t *src, gui_obj_t *tar)
{
    gui_obj_t *child_first = gui_list_entry(src->child_list.next, gui_obj_t, brother_list);
    gui_obj_t *child_last = gui_list_entry(src->child_list.prev, gui_obj_t, brother_list);
    child_first->brother_list.prev = &tar->child_list;
    child_last->brother_list.next = &tar->child_list;
    tar->child_list.next = &child_first->brother_list;
    tar->child_list.prev = &child_last->brother_list;
    src->child_list.next = &src->child_list;
    src->child_list.prev = &src->child_list;
}

static void gui_view_free_snapshot_obj_async(void *msg)
{
    gui_view_t *_this = ((gui_msg_t *)msg)->payload;
    gui_view_t *g_CurrentView = gui_view_get_current();
    if (_this->descriptor->use_snapshot)
    {
        if (_this->bg_img)
        {
            gui_free(_this->bg_img);
            _this->bg_img = NULL;
            // gui_log("%s %s free bg_img\n", __func__, _this->base.name);
        }

        if (_this->obj_temp)
        {
            // gui_log("%s free obj_temp\n", _this->descriptor->name);
            gui_view_move_childlist(_this->obj_temp, GUI_BASE(_this));
            gui_obj_tree_free(_this->obj_temp);
            _this->obj_temp = NULL;
        }
        else if (_this == g_CurrentView)
        {
            g_CurrentView->descriptor->on_switch_in(g_CurrentView); // After-create widget tree
            gui_log("%s after-create\n", g_CurrentView->base.name);
        }
    }
    gui_fb_change();
    gui_view_save_common_data(_this, g_CurrentView);
}

void gui_view_cache_snapshot_data(gui_view_t *_this)
{
    gui_view_t *g_CurrentView = gui_view_get_current();
    if (_this != NULL && _this->descriptor->use_snapshot == true)
    {
        if (!g_SnapShotCacheNeedUpdate)
        {
            g_SnapShotCacheNeedUpdate = true;
        }
        else
        {
            void **cache = _this->descriptor->snapshot_data;
            if (*cache == NULL)
            {
                uint16_t w = (uint16_t)_this->base.w;
                uint16_t h = (uint16_t)_this->base.h;
                uint16_t bit_depth = 16;
                uint32_t size = (uint32_t) w * h * bit_depth / 8;
                *cache = gui_malloc(sizeof(gui_rgb_data_head_t) + size);
                memset(*cache, 0, sizeof(gui_rgb_data_head_t));
            }
            else if (_this != g_CurrentView)
            {
                return;
            }

            uint8_t opacity_rec = _this->opacity;
            _this->opacity = UINT8_MAX;
            bool hidden_rec = gui_obj_is_hidden((gui_obj_t *)_this);
            gui_obj_hidden((gui_obj_t *)_this, false);
            gui_set_bg_color(_this->bg_color);

            gui_img_tree_convert_to_img((gui_obj_t *)_this, NULL, *cache);

            _this->opacity = opacity_rec;
            gui_obj_hidden((gui_obj_t *)_this, hidden_rec);
            gui_set_bg_color(g_CurrentView->bg_color);
            gui_log("%s cache snapshot\n", _this->descriptor->name);
        }
    }

    if (g_SnapShotPreCache == true && _this == g_CurrentView)
    {
        for (uint32_t i = 0; i < _this->on_event_num; i++)
        {
            const gui_view_descriptor_t *nd = _this->on_event[i]->descriptor;
            if (!nd->use_snapshot || nd->snapshot_data == NULL) { continue; }

            gui_view_t *neighbor = *nd->pView;
            if (neighbor == g_CurrentView || *nd->snapshot_data != NULL) { continue; }

            bool temp_created = false;
            if (neighbor == NULL)
            {
                neighbor = gui_view_create(gui_obj_get_fake_root(), nd->name, 0, 0, 0, 0);
                temp_created = true;
            }

            gui_view_cache_snapshot_data(neighbor);

            if (temp_created)
            {
                // Preserve cache buffer before destroying temporary view
                void *saved_cache = *nd->snapshot_data;
                *nd->snapshot_data = NULL;
                gui_obj_tree_free(GUI_BASE(neighbor));
                *nd->pView = NULL;

                // Restore cache buffer
                *nd->snapshot_data = saved_cache;
            }
        }
    }
}

/* Ensure the view has a snapshot buffer and wrap it in a draw_img_t (bg_img) so
   it can be blitted as a static background during a transition. */
void gui_view_create_snapshot(gui_view_t *_this)
{
    if (_this != NULL && _this->descriptor->use_snapshot == true)
    {
        if (*_this->descriptor->snapshot_data == NULL)
        {
            g_SnapShotCacheNeedUpdate = true;
            gui_view_cache_snapshot_data(_this);
        }

        if (_this->bg_img == NULL)
        {
            gui_view_show_snapshot(_this, true);

            gui_dispdev_t *dc = gui_get_dc();
            _this->bg_img = gui_malloc(sizeof(draw_img_t));
            GUI_ASSERT(_this->bg_img != NULL);
            memset(_this->bg_img, 0x00, sizeof(draw_img_t));
            _this->bg_img->img_w = dc->screen_width;
            _this->bg_img->img_h = dc->screen_height;
            _this->bg_img->data = (void *)*_this->descriptor->snapshot_data;
            _this->bg_img->opacity_value = _this->opacity;
        }
        BLEND_MODE_TYPE blend_mode = IMG_BYPASS_MODE;
        // if (_this->opacity != UINT8_MAX || _this->current_transition_style >= SWITCH_IN_FROM_LEFT_USE_CUBE)
        // {
        //     blend_mode = IMG_FILTER_BLACK;
        // }
        _this->bg_img->blend_mode = blend_mode;

    }
}

/* Free snapshot buffers that belonged to 'old_view' (and its neighbours) but are
   not shared with 'new_view', reclaiming memory after a switch completes. With
   pre-caching off, only 'old_view's own snapshot is released. */
void gui_view_save_common_data(gui_view_t *old_view, gui_view_t *new_view)
{
    GUI_UNUSED(old_view);
    GUI_UNUSED(new_view);

    if (old_view == NULL || new_view == NULL || old_view == new_view) { return; }
    if (g_SnapShotPreCache == true)
    {
        // Process all neighbors of old_view
        for (uint32_t i = 0; i < old_view->on_event_num; i++)
        {
            const gui_view_descriptor_t *nd = old_view->on_event[i]->descriptor;
            if (!nd->use_snapshot || nd->snapshot_data == NULL) { continue; }
            if (*nd->snapshot_data == NULL || nd == new_view->descriptor) { continue; }

            // Check if new_view also has this neighbor
            bool shared = false;
            for (uint32_t j = 0; j < new_view->on_event_num; j++)
            {
                if (new_view->on_event[j]->descriptor == nd)
                {
                    shared = true;
                    break;
                }
            }

            if (!shared)
            {
                gui_free(*nd->snapshot_data);
                *nd->snapshot_data = NULL;
                // gui_log("%s free snapshot data\n", nd->name);
            }
        }

        // Process old_view's snapshot data
        if (old_view->descriptor->use_snapshot && *old_view->descriptor->snapshot_data != NULL)
        {
            bool shared = false;
            for (uint32_t j = 0; j < new_view->on_event_num; j++)
            {
                if (new_view->on_event[j]->descriptor == old_view->descriptor)
                {
                    shared = true;
                    break;
                }
            }
            if (!shared)
            {
                gui_free(*old_view->descriptor->snapshot_data);
                *old_view->descriptor->snapshot_data = NULL;
                // gui_log("%s free snapshot data\n", old_view->descriptor->name);
            }
        }
    }
    else
    {
        if (old_view->descriptor->use_snapshot && *old_view->descriptor->snapshot_data != NULL)
        {
            gui_free(*old_view->descriptor->snapshot_data);
            *old_view->descriptor->snapshot_data = NULL;
            // gui_log("%s free snapshot data\n", old_view->descriptor->name);
        }
    }
}


/*============================================================================*
 *                           Public Functions
 *============================================================================*/
void gui_view_show_snapshot(gui_view_t *_this, bool show)
{
    gui_obj_t *obj = GUI_BASE(_this);
    if (show)
    {
        if (obj->child_list.next != &obj->child_list)
        {
            _this->obj_temp = gui_obj_create(gui_obj_get_fake_root(), "obj_temp", 0, 0, 0, 0);
            gui_view_move_childlist(obj, _this->obj_temp);
        }
    }
    else
    {
        gui_msg_t msg =
        {
            .event = GUI_EVENT_USER_DEFINE,
            .cb = gui_view_free_snapshot_obj_async,
            .payload = (void *)_this,
        };
        gui_send_msg_to_server(&msg);
    }
}

void gui_view_update_snapshot(void *msg)
{
    gui_view_t *_this = (gui_view_t *)(((gui_msg_t *)msg)->payload);
    gui_view_cache_snapshot_data(_this);
}
