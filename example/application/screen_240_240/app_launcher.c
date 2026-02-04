/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include <gui_img.h>
#include <gui_win.h>
#include <gui_text.h>
#include <gui_obj.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include "gui_server.h"
#include <math.h>
#include "font_mem.h"
#include "font_stb.h"
#include "gui_components_init.h"
#include "app_launcher.h"
#ifndef _HONEYGUI_SIMULATOR_
#include "os_mem.h"
#endif // !_HONEYGUI_SIMULATOR_

#if defined _HONEYGUI_SIMULATOR_
unsigned char *resource_root = NULL;
extern const unsigned char _binary_root_0x859000_bin_start[];
extern const unsigned char _binary_root_0x859000_bin_end[];
extern const unsigned char _binary_root_0x859000_bin_size[];
#endif

uint8_t fire_index;
#define FIRE_S 0
#define FIRE_E 19
static void *frames[FIRE_E + 1];


static void frames_init(void)
{
    frames[0] = FIRE_00000_BIN;
    frames[1] = FIRE_00001_BIN;
    frames[2] = FIRE_00002_BIN;
    frames[3] = FIRE_00003_BIN;
    frames[4] = FIRE_00004_BIN;
    frames[5] = FIRE_00005_BIN;
    frames[6] = FIRE_00006_BIN;
    frames[7] = FIRE_00007_BIN;
    frames[8] = FIRE_00008_BIN;
    frames[9] = FIRE_00009_BIN;
    frames[10] = FIRE_00010_BIN;
    frames[11] = FIRE_00011_BIN;
    frames[12] = FIRE_00012_BIN;
    frames[13] = FIRE_00013_BIN;
    frames[14] = FIRE_00014_BIN;
    frames[15] = FIRE_00015_BIN;
    frames[16] = FIRE_00016_BIN;
    frames[17] = FIRE_00017_BIN;
    frames[18] = FIRE_00018_BIN;
    frames[19] = FIRE_00019_BIN;
}

void fire_live(void *param)
{
    (void)param;

    gui_img_t *img = (gui_img_t *)param;
    if (fire_index < FIRE_E)
    {
        fire_index ++;
        gui_img_set_attribute(img, "fire", frames[fire_index], 0, 0);
    }
    else
    {
        fire_index = FIRE_S;
        gui_img_set_attribute(img, "fire", frames[fire_index], 0, 0);
    }
}

static void app_launcher_ui_design(void)
{
    gui_log("app_launcher_ui_design\n");

    frames_init();

    gui_img_t *fire = gui_img_create_from_mem(gui_obj_get_root(), "lake", frames[0], 0, 0, 0, 0);
    gui_img_set_mode(fire, IMG_BYPASS_MODE);

    gui_obj_create_timer(&(fire->base), 50, true, fire_live);
    gui_obj_start_timer(&(fire->base));

    return;
}

static int app_init(void)
{
#ifdef _HONEYGUI_SIMULATOR_
    resource_root = (unsigned char *)_binary_root_0x859000_bin_start;
#endif
    app_launcher_ui_design();
    return 0;
}

GUI_INIT_APP_EXPORT(app_init);
