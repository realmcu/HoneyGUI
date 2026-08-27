/**
 * @file example_particle.c
 * @brief Particle System Examples for HoneyGUI
 *
 * This file demonstrates the particle system widget with various effects.
 * Each effect is implemented in separate files (effect_*.c/h).
 *
 * The launcher provides a selection menu to browse all effects.
 * For direct API usage, see particle_demo.c/h for all effect_xxx_create() calls.
 *
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "gui_server.h"
#include "gui_components_init.h"
#include "particle_launcher.h"
#ifdef _HONEYGUI_SIMULATOR_
#include <stdlib.h>
#include "gui_api_os.h"
#include "gui_vfs.h"
#endif

/*============================================================================*
 *                    Application Entry Point
 *============================================================================*/

#ifdef _HONEYGUI_SIMULATOR_
unsigned char *resource_root = NULL;

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
#endif

static int particle_demo_init(void)
{
#ifdef _HONEYGUI_SIMULATOR_
    resource_root = load_resource_file(
                        "/pc/example/widget/particle/root_image/root_0x704D1400.bin");
    if (resource_root == NULL)
    {
        return -1;
    }
#endif

    particle_launcher_init();
    gui_set_keep_active_time(0xFFFFFFFF);
    return 0;
}

GUI_INIT_APP_EXPORT(particle_demo_init);
