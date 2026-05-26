/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/**
 * @file  example_msv1.c
 * @brief MSV1 Widget Demo -- plays output_msv1_360.avi (360x360, 25 FPS).
 *
 * Interaction:
 *   * Single tap  -- toggle Play / Pause
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
 * In the simulator the AVI binary is linked as an object section by SCons
 * (objcopy -I binary).  On device it sits at a fixed FTL/Flash address.
 * ----------------------------------------------------------------------- */
#ifdef _HONEYGUI_SIMULATOR_
// extern const unsigned char _binary_output_msv1_360_avi_start[];
// extern const unsigned char _binary_output_msv1_360_avi_end[];
extern const unsigned char _binary_output_avi_start[];
extern const unsigned char _binary_output_avi_end[];
#else
/* Replace with the actual FTL/NOR flash base address for the target board */
#define MSV1_DEMO_FLASH_ADDR  0x7004D100UL
#endif

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

/* -----------------------------------------------------------------------
 * Application entry point
 * ----------------------------------------------------------------------- */
static int app_init(void)
{
#ifdef _HONEYGUI_SIMULATOR_
    // void *resource = (void *)_binary_output_msv1_360_avi_start;
    void *resource = (void *)_binary_output_avi_start;
#else
    void *resource = (void *)MSV1_DEMO_FLASH_ADDR;
#endif

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

    gui_msv1_set_frame_rate(msv1, 50.0f);                        /* 25 FPS (AVI native) */
    gui_msv1_set_repeat_count(msv1, GUI_VIDEO_REPEAT_INFINITE);  /* loop forever        */
    /* DIAGNOSTIC: stop after frame 0 to verify decoder + render pipeline */
    // gui_msv1_set_state(msv1, GUI_VIDEO_STATE_STOP);

    /* Single tap toggles Play <-> Pause */
    gui_obj_add_event_cb(msv1,
                         msv1_click_cb,
                         GUI_EVENT_TOUCH_CLICKED,
                         NULL);

    return 0;
}
GUI_INIT_APP_EXPORT(app_init);
