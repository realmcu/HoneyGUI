/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include <string.h>
#include "guidef.h"
#include "gui_img.h"
#include "gui_fb.h"
#include "gui_server.h"
#include "gui_api_os.h"
#include "gui_vfs.h"
#include "gui_components_init.h"

#include "map_main.h"
#include "platform/map_platform.h"
#include "core/nav_api.h"
#include "render/render_api.h"
#include "state/map_state.h"



#define MAP_WIDTH 480
#define MAP_HEIGHT 480

map_t *g_map = NULL;
renderer_t *g_renderer = NULL;
uint8_t map_img_data[MAP_WIDTH * MAP_HEIGHT * 2 + 8];


static int map_app_init(void)
{
    extern const map_platform_ops_t *map_platform_honeygui_get(void);

    map_platform_register(map_platform_honeygui_get());

    gui_vfs_mount_posix("/map", ".");

    g_map = map_load("/map/example/map/sip.trmap");

    // g_map = map_load_from_memory(_acsip, sizeof(_acsip), false);

    if (!g_map)
    {
        gui_log("ERROR: Failed to load map from memory\n");
        return -1;
    }


    gui_vfs_file_t *f = gui_vfs_open("/map/example/map/NotoSansSC-Medium_subset.ttf", GUI_VFS_READ);
    if (!f)
    {
        gui_log("ERROR: Failed to open font file\n");
        return -1;
    }
    gui_vfs_seek(f, 0, GUI_VFS_SEEK_END);
    int font_size = gui_vfs_tell(f);
    gui_vfs_seek(f, 0, GUI_VFS_SEEK_SET);
    char *font_data = (char *)gui_malloc(font_size);

    gui_vfs_read(f, font_data, font_size);


    if (!render_set_font_from_memory(font_data, font_size))
    {
        gui_log("WARNING: Failed to load font\n");
    }

    g_renderer = renderer_create(MAP_WIDTH, MAP_HEIGHT, TRVG_PIXEL_RGB565, map_img_data + 8);

    if (!g_renderer)
    {
        gui_log("ERROR: Failed to create renderer\n");
        return -1;
    }

    gui_log("Map loaded successfully\n");

    if (!map_state_init())
    {
        gui_log("ERROR: Failed to init map state machine\n");
        return -1;
    }

    if (!gui_register_server_hook(map_state_tick))
    {
        gui_log("ERROR: Failed to register map state tick hook\n");
        map_state_deinit();
        return -1;
    }

    gui_log("Map state tick hook registered\n");


    map_img_data[0] = 0;
    map_img_data[1] = 0;
    map_img_data[2] = MAP_WIDTH & 0xFF;
    map_img_data[3] = (MAP_WIDTH >> 8) & 0xFF;
    map_img_data[4] = MAP_HEIGHT & 0xFF;
    map_img_data[5] = (MAP_HEIGHT >> 8) & 0xFF;
    map_img_data[6] = 0;
    map_img_data[7] = 0;

    gui_img_t *map_img = gui_img_create_from_mem(
                             gui_obj_get_root(),
                             "map",
                             (void *)map_img_data,
                             0, 0, 0, 0);

    if (!map_img)
    {
        gui_log("ERROR: Failed to create map widget\n");
        return -1;
    }

    gui_log("Map application initialized!\n");
    return 0;
}


GUI_INIT_APP_EXPORT(map_app_init);

