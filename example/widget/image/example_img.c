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
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "gui_server.h"
#include "gui_components_init.h"
#include "../../assets/tiger_blue_compressed.txt"
#include "../../assets/tiger_blue.txt"
#include "jpeg.txt"
#include "green.txt"
#include "gif_demo.txt"
#include "nanovg_generate_pixel.txt"
#include "test_png.txt"
#include "clock3_Vector.txt"


/* gui image widget example start*/
void test_event_cb(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    gui_obj_t *this = (gui_obj_t *)obj;
    gui_log("Event test obj name = %s, e = 0x%x !\n", this->name, e);
}

void test_timer_cb(void *param)
{
    GUI_UNUSED(param);
    gui_log("timer cb test!\n");
}

#include "gifdec.h"
static int app_init(void)
{

    void *addr = (void *)_actiger_blue;
    // void *addr = (void *)_acgreen;
    // void *addr = (void *)_acgif_demo;

    // gui_img_create_from_mem(gui_obj_get_root(),  "img_1_test", _achour, 0, 0, 0, 0);
    gui_img_t *img = gui_img_create_from_mem(gui_obj_get_root(),  "img_1_test", addr, 0, 0, 0, 0);

    gui_img_a8_recolor(img, GUI_COLOR_ARGB8888(255, 0, 0, 0));

    gui_img_a8_fix_bg(img, GUI_COLOR_ARGB8888(255, 0xFF, 0x59, 0x01));

    gui_img_a8_mix_alpha(img, 128);

    gui_img_set_mode(img, IMG_2D_SW_FIX_A8_BGFG);

    // gui_img_set_focus(img, 50, 50);

    gui_img_rotation(img, 0.0f);
    // gui_img_scale(img, 0.5f, 0.5f);

    // gui_img_translate(img, 50, 50);

    gui_obj_add_event_cb(img, (gui_event_cb_t)test_event_cb, GUI_EVENT_TOUCH_CLICKED, NULL);

    gui_obj_add_event_cb(img, (gui_event_cb_t)test_event_cb, GUI_EVENT_KB_SHORT_PRESSED, NULL);

    // gui_obj_focus_set(img);


    // gui_obj_create_timer(&(img->base), 1000, true, test_timer_cb);
    // gui_obj_start_timer(&(img->base));

    return 0;
}

GUI_INIT_APP_EXPORT(app_init);
/* gui image widget example end*/


