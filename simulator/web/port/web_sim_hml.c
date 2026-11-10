/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/**
 * @file web_sim_hml.c
 * @brief Run designer HML on the prebuilt browser engine
 *
 * HoneyDesigner previews a project without compiling it: the page writes the
 * converted assets, with the HML files in ui/, into MEMFS and calls
 * web_sim_load_hml() before main(). The HML runtime then builds the views
 * from the XML the way the designer's exported code does. C code of the
 * project (callbacks, custom timers) is not part of such a run.
 *
 * Built only with CONFIG_REALTEK_BUILD_XML_LOADER (see port/CMakeLists.txt).
 */

#include <stdio.h>
#include <string.h>
#include "guidef.h"
#include "gui_api.h"
#include "gui_components_init.h"
#include "gui_obj.h"
#include "gui_server.h"
#include "gui_vfs.h"
#include "gui_view.h"
#include "hml_utils.h"
#include "hml_view_instance.h"
#include "web_sim.h"
#include <emscripten.h>

static char hml_dir[256];
static char hml_start_view[128];
/* Set from JavaScript, consumed by the GUI server between frames. */
static bool hml_reload_pending;

static bool copy_arg(char *dst, size_t size, const char *src)
{
    size_t len = src ? strlen(src) : 0;
    if (len >= size)
    {
        return false;
    }
    memcpy(dst, src ? src : "", len + 1);
    return true;
}

EMSCRIPTEN_KEEPALIVE void web_sim_load_hml(const char *dir, const char *start_view)
{
    if (!dir || !dir[0] || !copy_arg(hml_dir, sizeof(hml_dir), dir) ||
        !copy_arg(hml_start_view, sizeof(hml_start_view), start_view))
    {
        hml_dir[0] = '\0';
        /* Called before main(): the GUI OS API, and gui_log() with it, is
         * registered by gui_server_init() inside main(). */
        fprintf(stderr, "web_sim_load_hml: dir must be 1-255 characters and "
                "start_view at most 127\n");
    }
}

EMSCRIPTEN_KEEPALIVE void web_sim_reload_hml(void)
{
    hml_reload_pending = true;
}

EMSCRIPTEN_KEEPALIVE const char *web_sim_current_view(void)
{
    gui_view_t *view = gui_view_get_current();
    return (view && view->descriptor) ? view->descriptor->name : "";
}

/* Runs on the GUI server every frame. hml_reload() parses the files whose
 * content changed and rebuilds the current view from them. */
static void hml_reload_hook(void)
{
    if (hml_reload_pending)
    {
        hml_reload_pending = false;
        hml_reload(false);
    }
}

/* Replace the entry view created by hml_scan_views() with the start view,
 * the same way hml_reload() rebuilds the current view. */
static void hml_show_start_view(void)
{
    if (!hml_start_view[0] || !gui_view_descriptor_get(hml_start_view))
    {
        return;
    }
    gui_view_t *view = gui_view_get_current();
    if (view && strcmp(view->descriptor->name, hml_start_view) == 0)
    {
        return;
    }
    if (view)
    {
        gui_obj_tree_free(view);
    }
    gui_view_create(gui_obj_get_root(), hml_start_view, 0, 0, 0, 0);
}

static int web_sim_hml_init(void)
{
    if (!hml_dir[0])
    {
        /* A compiled project runs instead. */
        return 0;
    }

    /* Same mount as the designer's generated entry file in the simulator. */
    gui_vfs_mount_posix("/", hml_dir);
    hml_set_asset_root("/");
    if (hml_scan_views("/ui") <= 0)
    {
        gui_log("web_sim: no HML views found in %s/ui\n", hml_dir);
        return -1;
    }
    hml_show_start_view();
    gui_register_server_hook(hml_reload_hook);
    return 0;
}
GUI_INIT_APP_EXPORT(web_sim_hml_init);
