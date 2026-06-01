/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/**
 * @file  example_lite_video.c
 * @brief gui_lite_video unified widget demo.
 *
 * Demonstrates auto-detecting both Microsoft Video 1 (MSV1) and Cinepak AVI
 * files through the same widget API.
 *
 * Interaction:
 *   * Single tap  -- toggle Play / Pause
 *   * Double-click -- switch between the two configured video sources
 *                     (may cross between MSV1 and Cinepak codecs)
 */

#include "guidef.h"
#include "gui_obj.h"
#include "gui_win.h"
#include "gui_server.h"
#include "gui_components_init.h"
#include "gui_obj_event.h"
#include "gui_lite_video.h"

/* -----------------------------------------------------------------------
 * Resource binding
 * In the simulator AVI binaries are linked as object sections by SCons
 * (objcopy -I binary).  On device they sit at fixed Flash addresses.
 * ----------------------------------------------------------------------- */
#ifdef _HONEYGUI_SIMULATOR_
extern const unsigned char _binary_cat_00_avi_start[];  /* Cinepak */
extern const unsigned char _binary_duck_avi_start[];    /* MSV1    */
#else
/* Replace with the actual Flash base addresses for the target board */
#define LV_DEMO_FLASH_ADDR_A  0x240F400UL   /* primary   source (Cinepak) */
#define LV_DEMO_FLASH_ADDR_B  0x24FF400UL   /* alternate source (MSV1)    */
#endif

/* -----------------------------------------------------------------------
 * Runtime state
 * ----------------------------------------------------------------------- */
static int s_src_idx;  /* 0 = primary, 1 = alternate */

/* -----------------------------------------------------------------------
 * Event callbacks
 * ----------------------------------------------------------------------- */
static void lv_click_cb(void *obj, gui_event_t *e)
{
    (void)e;
    gui_lite_video_t *lv = (gui_lite_video_t *)obj;
    GUI_VIDEO_STATE st = gui_lite_video_get_state(lv);

    if (st == GUI_VIDEO_STATE_PLAYING)
    {
        gui_lite_video_set_state(lv, GUI_VIDEO_STATE_PAUSE);
    }
    else
    {
        gui_lite_video_set_state(lv, GUI_VIDEO_STATE_PLAYING);
    }
}

static void lv_double_click_cb(void *obj, gui_event_t *e)
{
    (void)e;
    gui_lite_video_t *lv = (gui_lite_video_t *)obj;

    s_src_idx ^= 1;  /* toggle 0 <-> 1 */

#ifdef _HONEYGUI_SIMULATOR_
    void *next_src = (s_src_idx == 0) ? (void *)_binary_cat_00_avi_start
                     : (void *)_binary_duck_avi_start;
    gui_log("lv demo: switching to source %d\n", s_src_idx);
    gui_lite_video_set_src(lv, next_src, IMG_SRC_MEMADDR);
#else
    void *next_src = (s_src_idx == 0) ? (void *)LV_DEMO_FLASH_ADDR_A
                     : (void *)LV_DEMO_FLASH_ADDR_B;
    gui_log("lv demo: switching to source %d\n", s_src_idx);
    gui_lite_video_set_src(lv, next_src, IMG_SRC_MEMADDR);
#endif
}

/* -----------------------------------------------------------------------
 * Application entry point
 * ----------------------------------------------------------------------- */
static int app_init(void)
{
#ifdef _HONEYGUI_SIMULATOR_
    void *resource = (void *)_binary_cat_00_avi_start;
#else
    void *resource = (void *)LV_DEMO_FLASH_ADDR_A;
#endif

    s_src_idx = 0;

    gui_log("GUI Lite Video Widget Example Start\n");

    /* Width / height will be overwritten with the actual AVI frame dimensions
     * after the header is parsed inside the widget constructor.              */
    gui_lite_video_t *lv = gui_lite_video_create_from_mem(
                               gui_obj_get_root(), "lv_demo",
                               resource,
                               0, 0, 360, 360);
    if (!lv)
    {
        gui_log("ERROR: gui_lite_video_create_from_mem failed\n");
        return -1;
    }

    gui_lite_video_set_frame_rate(lv, 50.0f);
    gui_lite_video_set_repeat_count(lv, GUI_VIDEO_REPEAT_INFINITE);

    /* Single tap toggles Play <-> Pause */
    gui_obj_add_event_cb(lv, lv_click_cb,        GUI_EVENT_TOUCH_CLICKED,        NULL);
    /* Double-click switches to the alternate video source */
    gui_obj_add_event_cb(lv, lv_double_click_cb, GUI_EVENT_TOUCH_DOUBLE_CLICKED, NULL);

    return 0;
}
GUI_INIT_APP_EXPORT(app_init);
