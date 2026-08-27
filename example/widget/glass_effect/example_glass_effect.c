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
#include "gui_server.h"
#include "gui_components_init.h"
#include "gui_video.h"
#include "gui_glass.h"
#include "glass_0.c"
#include "glass_1.c"
#include "glass_1_2.c"

#ifdef _HONEYGUI_SIMULATOR_
#include <stdlib.h>
#include "gui_api_os.h"
#include "gui_vfs.h"

static unsigned char *resource_root;

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
#define BG_ADDR     0x7004D100
#endif

/* gui glass widget example start*/
static int app_init(void)
{
#ifdef _HONEYGUI_SIMULATOR_
    resource_root = load_resource_file(
                        "/pc/example/widget/glass_effect/root_image/bg.bin");
    if (resource_root == NULL)
    {
        return -1;
    }
#else
    unsigned char *resource_root = (unsigned char *)BG_ADDR;
#endif
    gui_video_t *video = gui_video_create_from_mem(gui_obj_get_root(), "background",
                                                   (void *)resource_root,
                                                   0, 0, 410,
                                                   502);
    gui_video_set_state(video, GUI_VIDEO_STATE_PLAYING);
    gui_video_set_repeat_count(video, GUI_VIDEO_REPEAT_INFINITE);
    gui_glass_create_from_mem(gui_obj_get_root(),  "img_2_test", (void *)glass_0, 195, 40, 0, 0);
    gui_glass_create_from_mem(gui_obj_get_root(),  "img_2_test", (void *)_acglass_1, 50, 40, 0,
                              0);
    gui_glass_create_from_mem(gui_obj_get_root(),  "img_2_test", (void *)glass_0, 30, 261, 0, 0);
    gui_glass_create_from_mem(gui_obj_get_root(),  "img_2_test", (void *)_acglass_1_2, 240, 270,
                              0, 0);

    return 0;
}
GUI_INIT_APP_EXPORT(app_init);
/* gui glass widget example end*/
