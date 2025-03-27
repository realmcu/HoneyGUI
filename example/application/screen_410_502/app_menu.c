#include <gui_tabview.h>
#include "gui_card.h"
#include <gui_obj.h>
#include <gui_win.h>
#include <gui_text.h>
#include <gui_curtain.h>
#include "root_image_hongkong/ui_resource.h"
#include <gui_app.h>
#include "gui_tab.h"
#include "app_hongkong.h"
#include "gui_win.h"
#include "gui_server.h"
#include "gui_components_init.h"
#include <stdio.h>
#include "gui_menu_cellular.h"
#include "math.h"
#include "gui_return.h"
#include "guidef.h"
#include "gui_soccer.h"
#include "gui_page.h"

#define SCREEN_WIDTH 410
#define SCREEN_HEIGHT 502
#define MENU_GAP 125
#define APP_NUM 10

#define CURRENT_VIEW_NAME "menu_view"

static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *watchface_view = NULL;
const static gui_view_descriptor_t *activity_view = NULL;
const static gui_view_descriptor_t *box2d_ring_view = NULL;
const static gui_view_descriptor_t *fruit_ninja_view = NULL;
const static gui_view_descriptor_t *heartrate_view = NULL;
const static gui_view_descriptor_t *music_view = NULL;
void app_menu_design(gui_view_t *view);

static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .design_cb = app_menu_design,
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
    watchface_view = gui_view_descriptor_get("watchface_view");
    activity_view = gui_view_descriptor_get("activity_view");
    box2d_ring_view = gui_view_descriptor_get("box2d_ring_view");
    fruit_ninja_view = gui_view_descriptor_get("fruit_ninja_view");
    heartrate_view = gui_view_descriptor_get("heartrate_view");
    music_view = gui_view_descriptor_get("music_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

uint8_t menu_style = 0;
static gui_win_t *win_array[APP_NUM];
static int16_t page_menu_y_his = 0;
bool sidebar_flag = 0;

extern void sidebar_app_array_fill(void *img_addr, gui_event_cb_t callback_function);

void switch_APP_BOX2D_RING(void *obj, gui_event_t e, void *param)
{
    gui_view_switch_direct(current_view, box2d_ring_view, VIEW_ANIMATION_8, VIEW_ANIMATION_5);
}

void switch_APP_FRUIT_NINJA(void *obj, gui_event_t e, void *param)
{
    gui_view_switch_direct(current_view, fruit_ninja_view, VIEW_ANIMATION_6, VIEW_ANIMATION_2);
}

void switch_APP_MUSIC(void *obj, gui_event_t e, void *param)
{
    gui_view_switch_direct(current_view, music_view, VIEW_ANIMATION_7, VIEW_ANIMATION_3);
}

void switch_APP_HEART_RATE(void *obj, gui_event_t e, void *param)
{
    gui_view_switch_direct(current_view, heartrate_view, VIEW_ANIMATION_7, VIEW_ANIMATION_1);
}

void switch_APP_ACTIVITY(void *obj, gui_event_t e, void *param)
{

    gui_view_switch_direct(current_view, activity_view, VIEW_ANIMATION_7, VIEW_ANIMATION_4);
}

/*Define gui_app_return_array*/
const uint32_t *gui_app_return_array[] =
{
    PATH04_BIN,
    PATH05_BIN,
    PATH07_BIN,
    PATH08_BIN,
    PATH09_BIN,
    PATH11_BIN,
    PATH12_BIN,
    PATH14_BIN,
    PATH15_BIN,
    PATH16_BIN,
    PATH18_BIN,
    PATH19_BIN,
    PATH20_BIN,
    PATH22_BIN,
    PATH23_BIN,
    PATH24_BIN,
    PATH25_BIN,
};

static void app_back2watchface_cb(void)
{
    gui_view_switch_direct(current_view, watchface_view, VIEW_ANIMATION_8, VIEW_ANIMATION_5);
}

static void app_soccer_cb(void *obj, gui_event_t e, void *param)
{
    gui_soccer_t *soccer = (gui_soccer_t *)obj;
    switch (soccer->press_face % 5)
    {
    case 0:
        gui_view_switch_direct(current_view, heartrate_view, VIEW_ANIMATION_7, VIEW_ANIMATION_1);
        break;
    case 1:
        gui_view_switch_direct(current_view, music_view, VIEW_ANIMATION_7, VIEW_ANIMATION_3);
        break;
    case 2:
        gui_view_switch_direct(current_view, fruit_ninja_view, VIEW_ANIMATION_6, VIEW_ANIMATION_2);
        break;
    case 3:
        gui_view_switch_direct(current_view, box2d_ring_view, VIEW_ANIMATION_8, VIEW_ANIMATION_5);
        break;
    case 4:
        gui_view_switch_direct(current_view, activity_view, VIEW_ANIMATION_7, VIEW_ANIMATION_4);
        break;
    default:
        break;
    }
}

void page_callback(gui_page_t *page)
{
    int32_t page_y = GUI_BASE(page)->y;
    uint8_t offX_range = 20;
    for (uint8_t i = 0; i < APP_NUM; i++)
    {
        gui_obj_t *obj = GUI_BASE(win_array[i]);
        int32_t h = SCREEN_HEIGHT / 2 - (obj->y + page_y + 50);
        obj->x = (uint8_t)(offX_range * abs(h) / (SCREEN_HEIGHT / 2));
        // obj->x = (int16_t)(SCREEN_HEIGHT - (uint16_t)sqrt((SCREEN_HEIGHT + h) * (SCREEN_HEIGHT - h)));
    }
    page_menu_y_his = page_y;
}


void app_menu_design(gui_view_t *view)
{
    clear_mem();

    gui_win_t *win = gui_win_create(view, "win_app_menu", 0, 0, SCREEN_WIDTH,
                                    SCREEN_HEIGHT);

    extern bool return_to_watchface_flag;
    return_to_watchface_flag = false;
    if (menu_style == 0)
    {
        uint32_t *array[] =
        {
            UI_CLOCK_HEARTRATE_ICON_BIN,
            UI_CLOCK_MUSIC_ICON_BIN,
            UI_CLOCK_FRUIT_NINJA_ICON_BIN,
            UI_CLOCK_BOX2D_RING_ICON_BIN,
            UI_CLOCK_ACTIVITY_ICON_BIN,
            UI_CLOCK_HEARTRATE_ICON_BIN,
            UI_CLOCK_MUSIC_ICON_BIN,
            UI_CLOCK_FRUIT_NINJA_ICON_BIN,
            UI_CLOCK_BOX2D_RING_ICON_BIN,
            UI_CLOCK_ACTIVITY_ICON_BIN,
            UI_CLOCK_HEARTRATE_ICON_BIN,
            UI_CLOCK_MUSIC_ICON_BIN,
            UI_CLOCK_FRUIT_NINJA_ICON_BIN,
            UI_CLOCK_BOX2D_RING_ICON_BIN,
            UI_CLOCK_ACTIVITY_ICON_BIN,
            UI_CLOCK_HEARTRATE_ICON_BIN,
            UI_CLOCK_MUSIC_ICON_BIN,
            UI_CLOCK_FRUIT_NINJA_ICON_BIN,
            UI_CLOCK_BOX2D_RING_ICON_BIN,
            UI_CLOCK_ACTIVITY_ICON_BIN,
            UI_CLOCK_HEARTRATE_ICON_BIN,
            UI_CLOCK_MUSIC_ICON_BIN,
            UI_CLOCK_FRUIT_NINJA_ICON_BIN,
            UI_CLOCK_BOX2D_RING_ICON_BIN,
            UI_CLOCK_ACTIVITY_ICON_BIN,
            UI_CLOCK_HEARTRATE_ICON_BIN,
            UI_CLOCK_MUSIC_ICON_BIN,
            UI_CLOCK_FRUIT_NINJA_ICON_BIN,
            UI_CLOCK_BOX2D_RING_ICON_BIN,
            UI_CLOCK_ACTIVITY_ICON_BIN,
            UI_CLOCK_HEARTRATE_ICON_BIN,
            UI_CLOCK_MUSIC_ICON_BIN,
            UI_CLOCK_FRUIT_NINJA_ICON_BIN,
            UI_CLOCK_BOX2D_RING_ICON_BIN,
            UI_CLOCK_ACTIVITY_ICON_BIN,
            UI_CLOCK_HEARTRATE_ICON_BIN,
            UI_CLOCK_MUSIC_ICON_BIN,
            UI_CLOCK_FRUIT_NINJA_ICON_BIN,
            UI_CLOCK_BOX2D_RING_ICON_BIN,
            UI_CLOCK_ACTIVITY_ICON_BIN,
            UI_CLOCK_HEARTRATE_ICON_BIN,
            UI_CLOCK_MUSIC_ICON_BIN,
            UI_CLOCK_FRUIT_NINJA_ICON_BIN,
            UI_CLOCK_BOX2D_RING_ICON_BIN,
            UI_CLOCK_ACTIVITY_ICON_BIN,
        };
        gui_menu_cellular_t *menu = gui_menu_cellular_create(win, 100, array,
                                                             sizeof(array) / sizeof(uint32_t *));
        // gui_menu_cellular_offset((void *)menu, -200, -200);
        {
            struct gui_menu_cellular_gesture_parameter gesture_parameter_array[] =
            {
                {switch_APP_HEART_RATE, (void *)1},
                {switch_APP_MUSIC, (void *)1}, {switch_APP_FRUIT_NINJA, (void *)1}, {switch_APP_BOX2D_RING, (void *)1}, {switch_APP_ACTIVITY, (void *)1},
                {switch_APP_HEART_RATE, (void *)1},
                {switch_APP_MUSIC, (void *)1}, {switch_APP_FRUIT_NINJA, (void *)1}, {switch_APP_BOX2D_RING, (void *)1}, {switch_APP_ACTIVITY, (void *)1},
                {switch_APP_HEART_RATE, (void *)1},
                {switch_APP_MUSIC, (void *)1}, {switch_APP_FRUIT_NINJA, (void *)1}, {switch_APP_BOX2D_RING, (void *)1}, {switch_APP_ACTIVITY, (void *)1},
                {switch_APP_HEART_RATE, (void *)1},
                {switch_APP_MUSIC, (void *)1}, {switch_APP_FRUIT_NINJA, (void *)1}, {switch_APP_BOX2D_RING, (void *)1}, {switch_APP_ACTIVITY, (void *)1},
                {switch_APP_HEART_RATE, (void *)1},
                {switch_APP_MUSIC, (void *)1}, {switch_APP_FRUIT_NINJA, (void *)1}, {switch_APP_BOX2D_RING, (void *)1}, {switch_APP_ACTIVITY, (void *)1},
                {switch_APP_HEART_RATE, (void *)1},
                {switch_APP_MUSIC, (void *)1}, {switch_APP_FRUIT_NINJA, (void *)1}, {switch_APP_BOX2D_RING, (void *)1}, {switch_APP_ACTIVITY, (void *)1},
                {switch_APP_HEART_RATE, (void *)1},
                {switch_APP_MUSIC, (void *)1}, {switch_APP_FRUIT_NINJA, (void *)1}, {switch_APP_BOX2D_RING, (void *)1}, {switch_APP_ACTIVITY, (void *)1},
                {switch_APP_HEART_RATE, (void *)1},
                {switch_APP_MUSIC, (void *)1}, {switch_APP_FRUIT_NINJA, (void *)1}, {switch_APP_BOX2D_RING, (void *)1}, {switch_APP_ACTIVITY, (void *)1},
                {switch_APP_HEART_RATE, (void *)1},
                {switch_APP_MUSIC, (void *)1}, {switch_APP_FRUIT_NINJA, (void *)1}, {switch_APP_BOX2D_RING, (void *)1}, {switch_APP_ACTIVITY, (void *)1},
            };
            gui_menu_cellular_on_click(menu, gesture_parameter_array,
                                       sizeof(gesture_parameter_array) / sizeof(gesture_parameter_array[0]));

        }
    }
    else if (menu_style == 1)
    {
        uint32_t *array[] =
        {
            SOCCER_HR_BIN,
            SOCCER_MUSIC_BIN,
            SOCCER_FRUIT_NINJA_BIN,
            SOCCER_BOX2DRING_BIN,
            SOCCER_ACTIVITY_BIN,
            SOCCER_HR_BIN,
            SOCCER_MUSIC_BIN,
            SOCCER_FRUIT_NINJA_BIN,
            SOCCER_BOX2DRING_BIN,
            SOCCER_ACTIVITY_BIN,
            SOCCER_HR_BIN,
            SOCCER_MUSIC_BIN,
            SOCCER_FRUIT_NINJA_BIN,
            SOCCER_BOX2DRING_BIN,
            SOCCER_ACTIVITY_BIN,
            SOCCER_HR_BIN,
            SOCCER_MUSIC_BIN,
            SOCCER_FRUIT_NINJA_BIN,
            SOCCER_BOX2DRING_BIN,
            SOCCER_ACTIVITY_BIN,
        };
        gui_soccer_t *menu = gui_soccer_create(win, "soccer", array, 0, 0);
        gui_soccer_set_center(menu, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
        gui_soccer_on_click(menu, app_soccer_cb, NULL);
    }
    else if (menu_style == 2)
    {
        gui_page_t *menu = gui_page_create(win, "menu_page", 0, 0, 0, 0);
        gui_page_set_animate(menu, 1000, -1, page_callback, menu);
        GUI_BASE(menu)->y = page_menu_y_his;
        uint32_t *array[] =
        {
            UI_CLOCK_HEARTRATE_ICON_BIN,
            UI_CLOCK_MUSIC_ICON_BIN,
            UI_CLOCK_FRUIT_NINJA_ICON_BIN,
            UI_CLOCK_BOX2D_RING_ICON_BIN,
            UI_CLOCK_ACTIVITY_ICON_BIN,
            UI_CLOCK_HEARTRATE_ICON_BIN,
            UI_CLOCK_MUSIC_ICON_BIN,
            UI_CLOCK_FRUIT_NINJA_ICON_BIN,
            UI_CLOCK_BOX2D_RING_ICON_BIN,
            UI_CLOCK_ACTIVITY_ICON_BIN,
        };
        char *text_array[] =
        {
            "Heart Rate",
            "Music",
            "Fruit Ninja",
            "Box2d Ring",
            "Activity",
            "Heart Rate",
            "Music",
            "Fruit Ninja",
            "Box2d Ring",
            "Activity",
        };
        int array_size = sizeof(array) / sizeof(array[0]);
        for (uint8_t i = 0; i < array_size; i++)
        {
            gui_win_t *win = gui_win_create(menu, "win", 0, i * MENU_GAP, SCREEN_WIDTH, 100);
            {
                gui_img_t *img = gui_img_create_from_mem(win, 0, array[i], 0, 0, 0, 0);
                gui_img_set_mode(img, IMG_SRC_OVER_MODE);
            }
            win_array[i] = win;
            char *text = text_array[i];
            int font_size = 32;
            gui_text_t *t = gui_text_create(win, "txt", 120, 27, gui_get_screen_width(), font_size);
            gui_text_set(t, text, GUI_FONT_SRC_BMP, gui_rgb(200, 200, 200), strlen(text), font_size);
            gui_text_type_set(t, SOURCEHANSANSSC_SIZE32_BITS1_FONT_BIN, FONT_SRC_MEMADDR);
        }
        gui_win_click(win_array[0], (gui_event_cb_t)switch_APP_HEART_RATE,     win_array[0]);
        gui_win_click(win_array[1], (gui_event_cb_t)switch_APP_MUSIC,          win_array[1]);
        gui_win_click(win_array[2], (gui_event_cb_t)switch_APP_FRUIT_NINJA,    win_array[2]);
        gui_win_click(win_array[3], (gui_event_cb_t)switch_APP_BOX2D_RING,     win_array[3]);
        gui_win_click(win_array[4], (gui_event_cb_t)switch_APP_ACTIVITY,       win_array[4]);
        gui_win_click(win_array[5], (gui_event_cb_t)switch_APP_HEART_RATE,     win_array[5]);
        gui_win_click(win_array[6], (gui_event_cb_t)switch_APP_MUSIC,          win_array[6]);
        gui_win_click(win_array[7], (gui_event_cb_t)switch_APP_FRUIT_NINJA,    win_array[7]);
        gui_win_click(win_array[8], (gui_event_cb_t)switch_APP_BOX2D_RING,     win_array[8]);
        gui_win_click(win_array[9], (gui_event_cb_t)switch_APP_ACTIVITY,       win_array[9]);
    }
    gui_return_create(win, gui_app_return_array,
                      sizeof(gui_app_return_array) / sizeof(uint32_t *), app_back2watchface_cb, NULL);
}