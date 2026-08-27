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
#ifdef _HONEYGUI_SIMULATOR_
#include <stdlib.h>
#include "gui_api_os.h"
#include "gui_vfs.h"
#endif

/* -----------------------------------------------------------------------
 * Resource binding
 * In the simulator AVI binaries are loaded into system RAM through VFS.
 * On device they sit at fixed Flash addresses.
 * ----------------------------------------------------------------------- */
#ifdef _HONEYGUI_SIMULATOR_
static unsigned char *s_video_resources[2];

static unsigned char *load_resource_file(const char *path)
{
    gui_vfs_stat_t stat;
    if (gui_vfs_stat(path, &stat) != 0 ||
        stat.type != GUI_VFS_TYPE_FILE ||
        stat.size == 0)
    {
        gui_log("Failed to get simulator resource: %s\n", path);
        return NULL;
    }

    gui_vfs_file_t *file = gui_vfs_open(path, GUI_VFS_READ);
    if (file == NULL)
    {
        gui_log("Failed to open simulator resource: %s\n", path);
        return NULL;
    }

    unsigned char *data = (unsigned char *)malloc(stat.size);
    if (data == NULL)
    {
        gui_log("Failed to allocate simulator resource: %s\n", path);
        gui_vfs_close(file);
        return NULL;
    }

    size_t total = 0;
    while (total < stat.size)
    {
        int read_size = gui_vfs_read(file, data + total, stat.size - total);
        if (read_size <= 0)
        {
            gui_log("Failed to read simulator resource: %s\n", path);
            free(data);
            gui_vfs_close(file);
            return NULL;
        }
        total += (size_t)read_size;
    }

    gui_vfs_close(file);
    return data;
}
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
    void *next_src = s_video_resources[s_src_idx];
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
    s_video_resources[0] = load_resource_file(
                               "/pc/example/widget/lite_video/root_image/cat_00.avi");
    if (s_video_resources[0] == NULL)
    {
        return -1;
    }

    s_video_resources[1] = load_resource_file(
                               "/pc/example/widget/lite_video/root_image/duck.avi");
    if (s_video_resources[1] == NULL)
    {
        free(s_video_resources[0]);
        s_video_resources[0] = NULL;
        return -1;
    }

    void *resource = s_video_resources[0];
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
#ifdef _HONEYGUI_SIMULATOR_
        free(s_video_resources[0]);
        free(s_video_resources[1]);
        s_video_resources[0] = NULL;
        s_video_resources[1] = NULL;
#endif
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
