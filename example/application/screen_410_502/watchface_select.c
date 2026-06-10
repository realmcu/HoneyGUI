/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#if 0

#include "guidef.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "app_main_watch.h"
#include "gui_cube.h"

#define CURRENT_VIEW_NAME "watchface_select_view"

static void watchface_select_design(gui_view_t *view);

GUI_VIEW_INSTANCE(CURRENT_VIEW_NAME, false, watchface_select_design, NULL);


// extern void WATCHFACE_CUBE_ui_design(gui_view_t *view);
// extern void _APP_WATCHFACE_PRISM3D_ui_design(gui_view_t *view);
// extern void WATCHFACE_MARKET_list(gui_view_t *view);

static void watchface_select_design(gui_view_t *view)
{
    extern  uint8_t menu_style;
    switch (menu_style)
    {
    case 0:
        _APP_WATCHFACE_PRISM3D_ui_design(view);
        break;
        // case 1:
        //     WATCHFACE_MARKET_list(view);
        //     break;
        // case 2:
        //     WATCHFACE_CUBE_ui_design(view);
        //     break;
        // default:
        //     WATCHFACE_MARKET_list(view);
        //     break;
    }
}
#endif
