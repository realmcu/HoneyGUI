/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/**
 * @file  example_msv1.c
 * @brief MSV1 Widget Demo -- plays cat_03.avi / cat_00.avi.
 *
 * Interaction:
 *   * Single tap  -- toggle Play / Pause
 *   * Long press  -- switch to the alternate video source
 */

#include "guidef.h"
#include "gui_obj.h"
#include "gui_win.h"
#include "gui_server.h"
#include "gui_components_init.h"
#include "gui_obj_event.h"
#include "gui_msv1.h"

/* -----------------------------------------------------------------------
 * Resource binding
 * In the simulator both AVI binaries are linked as object sections by SCons
 * (objcopy -I binary).  On device they sit at fixed Flash addresses.
 * ----------------------------------------------------------------------- */
#ifdef _HONEYGUI_SIMULATOR_
extern const unsigned char _binary_cat_03_avi_start[];
extern const unsigned char _binary_cat_00_avi_start[];
#else
/* Replace with the actual Flash base addresses for the target board */
#define MSV1_DEMO_FLASH_ADDR_A  0x240F400UL   /* primary source   (cat_03) */
#define MSV1_DEMO_FLASH_ADDR_B  0x24FF400UL   /* alternate source (cat_00) */
#endif

/* -----------------------------------------------------------------------
 * Runtime state
 * ----------------------------------------------------------------------- */
static int s_src_idx;  /* 0 = cat_03 (primary), 1 = cat_00 (alternate) */

/* -----------------------------------------------------------------------
 * Event callbacks
 * ----------------------------------------------------------------------- */
static void msv1_click_cb(void *obj, gui_event_t *e)
{
    (void)e;
    gui_msv1_t *msv1 = (gui_msv1_t *)obj;
    GUI_VIDEO_STATE st = gui_msv1_get_state(msv1);

    if (st == GUI_VIDEO_STATE_PLAYING)
    {
        gui_msv1_set_state(msv1, GUI_VIDEO_STATE_PAUSE);
    }
    else
    {
        gui_msv1_set_state(msv1, GUI_VIDEO_STATE_PLAYING);
    }
}

static void msv1_double_click_cb(void *obj, gui_event_t *e)
{
    (void)e;
    gui_msv1_t *msv1 = (gui_msv1_t *)obj;

    s_src_idx ^= 1;  /* toggle 0 <-> 1 */

#ifdef _HONEYGUI_SIMULATOR_
    void *next_src = (s_src_idx == 0) ? (void *)_binary_cat_03_avi_start
                     : (void *)_binary_cat_00_avi_start;
    gui_log("msv1 demo: switching to source %d\n", s_src_idx);
    gui_msv1_set_src(msv1, next_src, IMG_SRC_MEMADDR);
#else
    void *next_src = (s_src_idx == 0) ? (void *)MSV1_DEMO_FLASH_ADDR_A
                     : (void *)MSV1_DEMO_FLASH_ADDR_B;
    gui_log("msv1 demo: switching to source %d\n", s_src_idx);
    gui_msv1_set_src(msv1, next_src, IMG_SRC_MEMADDR);
#endif
}

/* -----------------------------------------------------------------------
 * Application entry point
 * ----------------------------------------------------------------------- */
static int app_init(void)
{
#ifdef _HONEYGUI_SIMULATOR_
    void *resource = (void *)_binary_cat_03_avi_start;
#else
    void *resource = (void *)MSV1_DEMO_FLASH_ADDR_A;
#endif

    s_src_idx = 0;

    gui_log("GUI MSV1 Widget Example Start\n");

    /* Width / height will be overwritten with the actual AVI frame dimensions
     * after the header is parsed inside the widget constructor.              */
    gui_msv1_t *msv1 = gui_msv1_create_from_mem(
                           gui_obj_get_root(), "msv1_demo",
                           resource,
                           0, 0, 360, 360);
    if (!msv1)
    {
        gui_log("ERROR: gui_msv1_create_from_mem failed\n");
        return -1;
    }

    gui_msv1_set_frame_rate(msv1, 50.0f);
    gui_msv1_set_repeat_count(msv1, GUI_VIDEO_REPEAT_INFINITE);

    /* Single tap toggles Play <-> Pause */
    gui_obj_add_event_cb(msv1, msv1_click_cb,      GUI_EVENT_TOUCH_CLICKED,        NULL);
    /* Double-click switches to the alternate video source */
    gui_obj_add_event_cb(msv1, msv1_double_click_cb, GUI_EVENT_TOUCH_DOUBLE_CLICKED, NULL);

    return 0;
}
GUI_INIT_APP_EXPORT(app_init);
