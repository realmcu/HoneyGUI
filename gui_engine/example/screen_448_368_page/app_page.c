#include <gui_tabview.h>
#include "gui_card.h"
#include <gui_obj.h>
#include <gui_win.h>
#include <gui_text.h>
#include <gui_curtain.h>
#include "file_system_image/ui_resource.h"
#include <gui_app.h>
#include "gui_tab.h"
#include "gui_win.h"
#include "gui_server.h"
#include "gui_components_init.h"
#include <stdio.h>

static void app_page_ui_design(gui_app_t *app);


gui_tabview_t *tv;

static gui_app_t app_page =
{
    .screen =
    {
        .name = "app_page",
        .x    = 0,
        .y    = 0,
        .parent = NULL,
    },
    .ui_design = app_page_ui_design,
    .thread_entry = NULL,
    .active_ms = 1000000,
};

void *get_app_page(void)
{
    return &app_page;
}
static void app_app_ui_design(gui_app_t *app);
static gui_app_t app_app =
{
    .screen =
    {
        .name = "app_app",
        .x    = 0,
        .y    = 0,
        .parent = NULL,
    },
    .ui_design = app_app_ui_design,
    .thread_entry = NULL,
    .active_ms = 1000000,
};

void *get_app_app(void)
{
    return &app_app;
}
static void kb_button_cb(void *obj, gui_event_t e)
{
    gui_log("line = %d \n", __LINE__);
}
#include "gui_page.h"
#include "gui_grid.h"
#include "gui_button.h"

static void callback(gui_button_t *button)
{

    static bool first_flag;
    static float current_scale;
    if (!first_flag)
    {
        first_flag = true;
        current_scale = button->img->scale_x;
    }
    /**
     * @brief Shrunk current scale to 0.7 in animation duration
     *  y = (0.7-scale)x+scale;
     */
    float scale = (0.7 - current_scale) * button->animate->progress_percent + current_scale;
    {
        gui_img_scale(button->img, scale, scale);
        gui_img_rotation(button->img, 0, gui_img_get_width(button->img) / 2,
                         gui_img_get_height(button->img) / 2);
        gui_img_translate(button->img, gui_img_get_width(button->img) / 2,
                          gui_img_get_height(button->img) / 2);
    }
    if (button->animate->progress_percent == 1)
    {
        first_flag = false;
    }

}
static void event_cb(gui_button_t *button)
{
    gui_button_set_animate(button, 1000, 0, callback, button);
}
static void callback_release(gui_button_t *button)
{
    static bool first_flag = false;
    static float current_scale;
    if (!first_flag)
    {
        first_flag = true;
        current_scale = button->img->scale_x;
    }
    /**
     * @brief Shrunk current scale to 1 in animation duration
     *  y = (1-scale)x+scale;
     */
    float scale = (1 - current_scale) * button->animate->progress_percent + current_scale;
    {
        gui_img_scale(button->img, scale, scale);
        gui_img_rotation(button->img, 0, gui_img_get_width(button->img) / 2,
                         gui_img_get_height(button->img) / 2);
        gui_img_translate(button->img, gui_img_get_width(button->img) / 2,
                          gui_img_get_height(button->img) / 2);
    }
    if (button->animate->progress_percent == 1)
    {
        first_flag = false;
    }
}
static void event_cb_release(gui_button_t *button)
{
    gui_button_set_animate(button, 1000, 0, callback_release, button);
}
static void event_cb_click(gui_button_t *button)
{
    gui_app_startup(get_app_app());
    gui_app_layer_top();
}
static void deal_img_in_root(gui_obj_t *object)
{
    gui_list_t *node = NULL;
    gui_list_for_each(node, &object->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        obj->ay = obj->y + obj->parent->ay;
        if (obj->type == IMAGE_FROM_MEM)
        {

            if (obj->ay < gui_get_screen_height() &&
                obj->ay + gui_img_get_height((void *)obj) > gui_get_screen_height())
            {
                //gui_log("obj->ay :%d", obj->ay );
                float scale = ((float)(gui_get_screen_height() - obj->ay)) / ((float)gui_img_get_height((
                                                                                  void *)obj));
                gui_img_scale((void *)obj, scale, scale);
                gui_img_rotation((void *)obj, 0, gui_img_get_width((void *)obj) / 2, 0);
                gui_img_translate((void *)obj, gui_img_get_width((void *)obj) / 2, 0);
            }
            else
            {
                float scale = 1;
                gui_img_scale((void *)obj, scale, scale);
                gui_img_rotation((void *)obj, 0, gui_img_get_width((void *)obj) / 2, 0);
                gui_img_translate((void *)obj, gui_img_get_width((void *)obj) / 2, 0);
            }


        }


        deal_img_in_root(obj);
    }
}
static void page_callback(gui_page_t *page)
{
    deal_img_in_root(page); GET_BASE(page)->opacity_value = 200;
    //gui_log("-------\n");
}
const static void *array[] =
{
    APPS_BIN,
    BATTERY_BIN,
    BLUETOOTH_BIN,
    DISPLAY_BIN,
    MYDEVICE_BIN,
    RTOUCH_BIN,
    SECURITY_BIN,
    SOUND_BIN,
    WALLET_BIN,
    WLAN_BIN,
};
static gui_button_t *button_array[sizeof(array) / sizeof(array[0])];
static void app_page_ui_design(gui_app_t *app)
{
    gui_log("app_page_ui_design\n");
    gui_img_create_from_mem(&(app->screen), 0, BACK_BIN, 0, 0, 0, 0);
    gui_page_t *page = gui_page_create(&(app->screen), 0, 4, 0, 0, 0);
    gui_page_rebound(page, true);
    int array_size = sizeof(array) / sizeof(array[0]);
    gui_grid_t *grid = gui_grid_create(page, 0, 0, array_size, 1, 0, 104);
    for (size_t i = 0; i < array_size; i++)
    {

        gui_button_t *button = gui_button_create(grid, 0, 0, 368, 100, array[i], array[i], 0, 0, 0);
        gui_obj_add_event_cb(button, event_cb, GUI_EVENT_TOUCH_PRESSED, button);
        gui_obj_add_event_cb(button, event_cb_release, GUI_EVENT_TOUCH_RELEASED, button);
        //gui_img_set_opacity(button->img, 200);
        gui_img_set_mode(button->img, IMG_SRC_OVER_MODE);
        button_array[i] = button;
    }
    gui_obj_add_event_cb(button_array[0], event_cb_click, GUI_EVENT_TOUCH_CLICKED, button_array[0]);
    gui_page_set_animate(page, 1000, -1, page_callback, page);
}
#include "gui_win.h"
static void app_app_animate_exit(gui_win_t *win)
{
    touch_info_t *tp = tp_get_info();
    static bool end;
    if ((tp->deltaX > gui_get_screen_width() / 3 ||
         GET_BASE(win)->x > gui_get_screen_width() / 3 * 2) && (tp->type == TOUCH_HOLD_X) && tp->deltaX > 0)
    {
        GET_BASE(win)->x = gui_get_screen_width();
        end = true;

    }
    static bool press;
    if (tp->pressed)
    {
        press = true;
    }
    if (tp->released && end)
    {
        GET_BASE(win)->x = gui_get_screen_width();
        gui_app_shutdown(get_app_app());
        end = false;
    }
    if (tp->type == TOUCH_HOLD_X && tp->deltaX > 0)
    {
        if (press)
        {
            gui_app_startup(get_app_page());
            gui_app_layer_buttom();
            press = false;
        }

        GET_BASE(win)->x = tp->deltaX;
    }


}
static void app_app_animate(gui_win_t *win)
{
    GET_BASE(win)->x = win->animate->progress_percent * (-((float)gui_get_screen_width())) +
                       (float)gui_get_screen_width();
    gui_log("x:%d\n", GET_BASE(win)->x);
    if (win->animate->progress_percent == 1)
    {
        gui_app_shutdown(get_app_page());
        gui_win_set_animate(win, 200, -1, app_app_animate_exit, win);
    }

}
static void app_app_ui_design(gui_app_t *app)
{
    gui_log("app_app_ui_design\n");
    gui_win_t *win = gui_win_create(&(app->screen), 0, gui_get_screen_width(), 0, 0, 0);
    gui_img_create_from_mem(win, 0, BACK_BIN, 0, 0, 0, 0);
    {
        gui_img_t *img = gui_img_create_from_mem(win, 0, REALGUI_BIN, 3, 3, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
    }
    {
        gui_img_t *img = gui_img_create_from_mem(win, 0, DEVICE_NAME_BIN, 185, 3, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
    }
    {
        gui_img_t *img = gui_img_create_from_mem(win, 0, STORAGE_BIN, 185, 187, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
    }



    gui_win_set_animate(win, 200, 0, app_app_animate, win);

}

uint8_t resource_root[1024 * 1024 * 20];
static int app_init(void)
{
#if defined _WIN32
    int fd;
    fd = open("./gui_engine/example/screen_448_368_page/file_system_image/root(0x4400000).bin", 0);
    if (fd > 0)
    {
        printf("open root(0x4400000).bin Successful!\n");
        read(fd, resource_root, 1024 * 1024 * 20);
    }
    else
    {
        printf("open root(0x4400000).bin Fail!\n");
        printf("open root(0x4400000).bin Fail!\n");
        printf("open root(0x4400000).bin Fail!\n");
        return 0;
    }
#endif
    gui_server_init();
    gui_app_startup(get_app_page());
    return 0;
}

GUI_INIT_APP_EXPORT(app_init);

