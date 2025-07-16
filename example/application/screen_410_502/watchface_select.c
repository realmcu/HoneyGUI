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

static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *watchface_butterfly_view = NULL;
const static gui_view_descriptor_t *watchface_ring_view = NULL;
const static gui_view_descriptor_t *watchface_market_view = NULL;
const static gui_view_descriptor_t *watchface_view = NULL;
static void watchface_select_design(gui_view_t *view);

static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = watchface_select_design,
};

static int gui_view_descriptor_register_init(void)
{
    gui_view_descriptor_register(&descriptor);
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_REGISTER(gui_view_descriptor_register_init);

static int gui_view_get_other_view_descriptor_init(void)
{
    /* you can get other view descriptor point here */
    watchface_butterfly_view = gui_view_descriptor_get("watchface_butterfly_view");
    watchface_ring_view = gui_view_descriptor_get("watchface_ring_view");
    watchface_market_view = gui_view_descriptor_get("watchface_market_view");
    watchface_view = gui_view_descriptor_get("watchface_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

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
