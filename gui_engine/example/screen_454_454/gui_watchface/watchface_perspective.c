#include <gui_obj.h>
#include "root_image/ui_resource.h"
#include "gui_perspective.h"
#include <gui_img.h>
#include "app_gui_main.h"
#include "gui_win.h"
#include "watchface_mgr.h"
#include <stdio.h>

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
            app_watchface_switch_tabs(WATCHFACE_CUBE);
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

void design_tab_perspective(void *parent)
{
    gui_perspective_imgfile_t image_list;
    memset(&image_list, 0, sizeof(gui_perspective_imgfile_t));

    for (int i = 0; i < 6; i++)
    {
        image_list.src_mode[i] = IMG_SRC_MEMADDR;
    }
    image_list.data_addr[0] = A1_BIN;
    image_list.data_addr[1] = A2_BIN;
    image_list.data_addr[2] = A3_BIN;
    image_list.data_addr[3] = A4_BIN;
    image_list.data_addr[4] = A5_BIN;
    image_list.data_addr[5] = A6_BIN;
    gui_perspective_t *ppp = gui_perspective_create(parent, "ppp", &image_list, 0, 0);

    gui_win_t *win_switch_to_main_app = gui_win_create(parent, "win_switch_to_main_app", 0, 0,
                                                       454, 454);
    gui_win_click(win_switch_to_main_app, (gui_event_cb_t)win_switch_main_to_app_touch_cb, NULL);
    gui_fb_change();

}


