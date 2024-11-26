#include "guidef.h"
#include "gui_img.h"
#include "gui_tabview.h"
#include "gui_tab.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include <gui_app.h>
#include "gui_server.h"
#include "gui_components_init.h"
#include "gui_cube.h"
#include "gui_win.h"
#include "gui_canvas_arc.h"
#include "gui_canvas_round_rect.h"
#include "tiger_blue_compressed.txt"
#include "butterfly/ButterFly_Antenna.c"
#include "butterfly/ButterFly_BodyHalf.c"
#include "butterfly/ButterFly_dot0.c"
#include "butterfly/ButterFly_dot1.c"
#include "butterfly/ButterFly_SwingDown.c"
#include "butterfly/ButterFly_SwingUp.c"
#include "butterfly/Common_Fixed.c"
#include "butterfly/Common_Font32.c"
#include "butterfly/Common_Font96.c"
#include "butterfly/Common_Unfixed.c"
#include "acc_sw_rle.h"
#include "gui_widget_litegfx.h"

// Callback function for butterfly widget
static uintptr_t watchface_butterfly_user_cb(uint16_t cmd, uintptr_t param, uintptr_t user_data)
{
    if (cmd == LX_CMD_ID_INIT)
    {
        gui_obj_t *obj = (gui_obj_t *)user_data;

        // Set images for the butterfly widget
        gui_widget_litegfx_set_image(obj, 1, (void *)_acButterFly_Antenna);
        gui_widget_litegfx_set_image(obj, 2, (void *)_acButterFly_SwingUp);
        gui_widget_litegfx_set_image(obj, 3, (void *)_acButterFly_SwingDown);
        gui_widget_litegfx_set_image(obj, 4, (void *)_acButterFly_dot0);
        gui_widget_litegfx_set_image(obj, 5, (void *)_acButterFly_dot1);
        gui_widget_litegfx_set_image(obj, 6, (void *)_acCommon_Font32);
        gui_widget_litegfx_set_image(obj, 7, (void *)_acCommon_Font96);
        gui_widget_litegfx_set_image(obj, 8, (void *)_acCommon_Unfixed);
        gui_widget_litegfx_set_image(obj, 9, (void *)_acCommon_Fixed);

        // Set parameters for the butterfly widget
        gui_widget_litegfx_set_param(obj, LX_CMD_ID_COLOR,
                                     0x82bbfc); // Set particle color for the butterfly
        gui_widget_litegfx_set_param(obj, LX_CMD_ID_SHOW_TIME,
                                     (uint32_t)false); // Time display switch, default is on
        gui_widget_litegfx_set_param(obj, LX_CMD_ID_TIME_FORMAT,
                                     1); // Time format switch, 0: 24-hour format, 1: 12-hour format, default is 0
        gui_widget_litegfx_set_param(obj, LX_CMD_ID_FIXED_BUTTON,
                                     (uint32_t)false); // Fixed button switch for the watch face, default is off
    }
    return 0;
}

// UI design function
static void app_ui_design(gui_app_t *app)
{
    gui_widget_litegfx_t *widget = gui_widget_litegfx_create(&(app->screen), "lx vglite butterfly",
                                                             LX_VGWIDGET_INS_BUTTERFLY, watchface_butterfly_user_cb, 0, 0, 454, 454);
}

// Define GUI application
static gui_app_t rtk_gui_demo =
{
    .screen =
    {
        .name = "LiteGFX",
        .x    = 0,
        .y    = 0,
    },
    .ui_design = app_ui_design,
    .active_ms = 1000 * 60 * 60,
};

// Application initialization function
static int app_init(void)
{
    gui_server_init();
    gui_app_startup(&rtk_gui_demo);
    return 0;
}

GUI_INIT_APP_EXPORT(app_init);
