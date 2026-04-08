/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include "guidef.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "gui_obj_focus.h"
#include "stdio.h"
#include "stdlib.h"
#include "gui_components_init.h"
#include "../../assets/tiger_blue.txt"



/* gui image widget example start*/
void img_kb_cb(void *obj, gui_event_t *e)
{
    gui_obj_t *this = (gui_obj_t *)obj;
    const char *dev_name = e->indev_name ? (const char *)e->indev_name : "unknown";
    gui_log("Event test obj name = %s, e = 0x%x, indev = %s !\n", this->name, e->code, dev_name);
}

void img_tp_clicked_cb(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    gui_log("Single clicked!\n");
}

void img_tp_double_clicked_cb(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    gui_log("Double clicked!\n");
}

void img_tp_triple_clicked_cb(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    gui_log("Triple clicked!\n");
}

void img_tp_long_pressed_cb(void *obj, gui_event_t *e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    gui_log("Touch long!\n");
}

void img_timer_cb(void *param)
{
    static float angle = 0;
    gui_img_t *img = (gui_img_t *)param;

    gui_img_rotation(img, angle++);

    gui_log("Handler[img] timer cb!\n");
}


static int app_init(void)
{
    void *addr = (void *)_actiger_blue;
    gui_dispdev_t *dc = gui_get_dc();

    gui_dirty_border_enable(true);

    // gui_img_t *img = gui_img_create_from_fs(gui_obj_get_root(),  "img_1_test", "/pc/example/application/screen_410_502/root_image/root/UI/cellular_menu_card.bin", 0, 0, 0, 0);
    gui_img_t *img = gui_img_create_from_mem(gui_obj_get_root(),  "img_test", addr, 0, 0, 0, 0);

    gui_img_set_focus(img, gui_img_get_width(img) / 2, gui_img_get_height(img) / 2);
    gui_img_rotation(img, 0.0f);
    gui_img_translate(img, dc->screen_width / 2, dc->screen_height / 2);

    gui_obj_add_event_cb(img, (gui_event_cb_t)img_tp_clicked_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_obj_add_event_cb(img, (gui_event_cb_t)img_tp_double_clicked_cb, GUI_EVENT_TOUCH_DOUBLE_CLICKED,
                         NULL);
    gui_obj_add_event_cb(img, (gui_event_cb_t)img_tp_triple_clicked_cb, GUI_EVENT_TOUCH_TRIPLE_CLICKED,
                         NULL);
    gui_obj_add_event_cb(img, (gui_event_cb_t)img_tp_long_pressed_cb, GUI_EVENT_TOUCH_LONG, NULL);

    gui_obj_add_event_cb(img, (gui_event_cb_t)img_kb_cb, GUI_EVENT_KB_SHORT_PRESSED, NULL);
    gui_obj_focus_set(img);


    gui_obj_create_timer(&(img->base), 100, true, img_timer_cb);
    gui_obj_start_timer(&(img->base));


    return 0;
}

GUI_INIT_APP_EXPORT(app_init);
/* gui image widget example end*/


