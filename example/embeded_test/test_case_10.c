/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include "guidef.h"
#include "gui_obj.h"
#include "gui_video.h"
#include "test_cases.h"

#ifdef _HONEYGUI_SIMULATOR_
#include <stdlib.h>
#include "gui_vfs.h"

static unsigned char *test_case_10_video_data;

static unsigned char *test_case_10_load_video(void)
{
    gui_vfs_stat_t stat;
    gui_vfs_file_t *file;
    unsigned char *data;
    size_t total = 0;

    if (gui_vfs_stat("/pc/example/application/screen_410_502/root_image/root/video/earth_420_336_416.mjpg",
                     &stat) != 0 ||
        stat.type != GUI_VFS_TYPE_FILE ||
        stat.size == 0)
    {
        return NULL;
    }

    file = gui_vfs_open("/pc/example/application/screen_410_502/root_image/root/video/earth_420_336_416.mjpg",
                        GUI_VFS_READ);
    if (file == NULL)
    {
        return NULL;
    }

    data = (unsigned char *)malloc(stat.size);
    if (data == NULL)
    {
        gui_vfs_close(file);
        return NULL;
    }

    while (total < stat.size)
    {
        int read_size = gui_vfs_read(file, data + total, stat.size - total);

        if (read_size <= 0)
        {
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
#include "flash_map.h"

static const uint8_t *test_case_10_find_video_data(void)
{
    const uint8_t *data = (const uint8_t *)USER_DATA1_ADDR;
    uint32_t offset;

    for (offset = 0; offset < 0x1000U - 1U; offset++)
    {
        if (data[offset] == 0xFFU && data[offset + 1U] == 0xD8U)
        {
            gui_log("test_case_10 video offset: 0x%x\n", (unsigned int)offset);
            return data + offset;
        }
    }

    gui_log("test_case_10 video JPEG header not found\n");
    return NULL;
}
#endif

#define TEST_CASE_10_EXPECTED_RENDER_TIME_MS 20U

uint32_t test_case_10(void)
{
    gui_video_t *video;
    void *video_data;

    gui_log("test_case_10 start!\n");

    gui_obj_child_free(gui_obj_get_root());

#ifdef _HONEYGUI_SIMULATOR_
    if (test_case_10_video_data == NULL)
    {
        test_case_10_video_data = test_case_10_load_video();
    }
    if (test_case_10_video_data == NULL)
    {
        gui_log("test_case_10 failed to load video resource\n");
        return TEST_CASE_10_EXPECTED_RENDER_TIME_MS;
    }
    video_data = test_case_10_video_data;
#else
    video_data = (void *)test_case_10_find_video_data();
    if (video_data == NULL)
    {
        return TEST_CASE_10_EXPECTED_RENDER_TIME_MS;
    }
#endif

    video = gui_video_create_from_mem(gui_obj_get_root(), "earth",
                                      video_data,
                                      0, 0, 410, 502);
    gui_video_set_frame_rate(video, 30.0f);
    gui_video_set_repeat_count(video, GUI_VIDEO_REPEAT_INFINITE);
    gui_video_set_state(video, GUI_VIDEO_STATE_PLAYING);

    gui_log("test_case_10 end!\n");

    return TEST_CASE_10_EXPECTED_RENDER_TIME_MS;
}
