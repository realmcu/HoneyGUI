#include <gui_tabview.h>
#include "gui_card.h"
#include <gui_obj.h>
#include "root_image/ui_resource.h"
#include <gui_app.h>
#include "gui_tab.h"
#include "gui_perspective.h"
#include "draw_font.h"
#include <gui_img.h>
#include "app_gui_main.h"
#include "app_gui_perspective.h"
#include "app_gui_cube.h"
#include "gui_win.h"
#include "gui_server.h"
#include "gui_components_init.h"
#include <stdio.h>

#define DOUBLE_CLICK_INTERVAL 600

static void design_app_perspective_ui(gui_app_t *app);

//static uint16_t longtouch_count = 0;
static void win_switch_main_to_app_touch_cb(void *obj, gui_event_t event)
{
    static uint32_t first_click = 0;
    static uint32_t second_click = 0;
    static bool is_first_click = 1;
    switch (event)
    {
    // case GUI_EVENT_TOUCH_LONG:
    //     gui_log("win_switch_main_to_app_touch_cb, event = %d longtouch_count = %d\n", event,
    //             longtouch_count);
    //     longtouch_count++;
    //     if (longtouch_count > 30)
    //     {
    //         longtouch_count = 0;
    //         gui_switch_app(get_app_perspective_ui(), get_app_cube_ui());
    //     }
    //     break;
    // case GUI_EVENT_TOUCH_RELEASED:
    //     longtouch_count = 0;
    //     break;

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

        gui_log("333first_click = %d, second_click = %d, is_first_click = %d\n", first_click, second_click,
                is_first_click);
        if (second_click - first_click < DOUBLE_CLICK_INTERVAL && second_click != 0)
        {
            gui_switch_app(get_app_perspective_ui(), get_app_cube_ui());
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
        gui_log("444first_click = %d, second_click = %d, is_first_click = %d\n", first_click, second_click,
                is_first_click);
        break;
    default:
        break;


    }
}

static gui_app_t app_perspective_ui =
{
    .screen =
    {
        .name = "app_perspective",
        .x    = 0,
        .y    = 0,
        .parent = NULL,
    },
    .ui_design = design_app_perspective_ui,
    .thread_entry = NULL,
    .active_ms = 1000000,
};

void *get_app_perspective_ui(void)
{
    return &app_perspective_ui;
}

static void design_app_perspective_ui(gui_app_t *app)
{
    gui_log("design_app_perspective_ui\n");

    gui_font_mem_init(FONT_BIN);

#ifdef _ENABLE_RTK_SOC_WATCH_
#include "mem_config.h"
    //memcpy((void *)SPIC2_ADDR, (void *)0x04400000, 0x100000 * 12);
#endif

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
    gui_perspective_t *ppp = gui_perspective_create(&(app->screen), "ppp", &image_list, 0, 0);

    gui_win_t *win_switch_to_main_app = gui_win_create(&(app->screen), "win_switch_to_main_app", 0, 0,
                                                       454, 454);
    //gui_win_long(win_switch_to_main_app, win_switch_main_to_app_touch_cb, NULL);
    //gui_win_release(win_switch_to_main_app, win_switch_main_to_app_touch_cb, NULL);
    gui_win_click(win_switch_to_main_app, win_switch_main_to_app_touch_cb, NULL);
    gui_fb_change();

}


