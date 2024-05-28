#include <gui_win.h>
#include <gui_text.h>
#include <gui_curtain.h>
#include "root_image/ui_resource.h"
#include <gui_img.h>
#include "gui_canvas.h"
#include "app_gui_main.h"
#include "gui_cube.h"
#include "gui_server.h"
#include <stdio.h>
#include "watchface_mgr.h"
#ifndef _WIN32
#include "wdg.h"
#endif

static void win_switch_main_to_app_touch_cb(void *obj, uint16_t event)
{
    static uint32_t first_click = 0;
    static uint32_t second_click = 0;
    static bool is_first_click = 1;
    switch (event)
    {
    case GUI_EVENT_TOUCH_CLICKED:
        if (is_first_click)
        {
            first_click = gui_ms_get();
            is_first_click = 0;
        }
        else
        {
            second_click = gui_ms_get();
            is_first_click = 1;
        }

        if (second_click - first_click < DOUBLE_CLICK_INTERVAL && second_click != 0)
        {
            app_watchface_switch_tabs(WATCHFACE_MAIN);
            is_first_click = 1;
            first_click = 0;
            second_click = 0;
        }
        else if (second_click - first_click >= DOUBLE_CLICK_INTERVAL && second_click != 0)
        {
            is_first_click = 0;
            first_click = second_click;
            second_click = 0;
        }
        break;
    default:
        break;

    }
}

void design_tab_cube(void *parent)
{
    gui_cube_imgfile_t image_file;
    memset(&image_file, 0, sizeof(gui_cube_imgfile_t));
    for (int i = 0; i < 6; i++)
    {
        image_file.src_mode[i] = IMG_SRC_MEMADDR;
    }
    image_file.data_addr.data_addr_up = B1_RLE_BIN;
    image_file.data_addr.data_addr_down = B2_RLE_BIN;
    image_file.data_addr.data_addr_front = B3_RLE_BIN;
    image_file.data_addr.data_addr_left = B4_RLE_BIN;
    image_file.data_addr.data_addr_right = B5_RLE_BIN;
    image_file.data_addr.data_addr_back = B6_RLE_BIN;
    gui_cube_t *ccc = gui_cube_create(parent, "ccc", &image_file, 0, 0);
    gui_cube_set_center(ccc, 227, 227);
    gui_cube_set_size(ccc, 75);
    gui_cube_auto_rotation_by_y(ccc, 100, 5.5f);

    gui_win_t *win_switch_to_main_app = gui_win_create(parent, "win_switch_to_main_app", 0, 0,
                                                       454, 454);
    gui_win_click(win_switch_to_main_app, win_switch_main_to_app_touch_cb, NULL);
    gui_fb_change();
}


