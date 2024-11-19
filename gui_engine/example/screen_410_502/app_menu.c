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

#define SCREEN_WIDTH 410
#define SCREEN_HEIGHT 502

#define APP_MUSIC
#define APP_FRUIT_NINJA
#define APP_BOX2D_RING

#define GUI_APP_DEFINE_NAME(APP_NAME) \
    static void _##APP_NAME##_ui_design(gui_app_t*); \
    static gui_app_t _app_##APP_NAME = \
                                       { \
                                         .screen = \
                                                   { \
                                                     .name = #APP_NAME, /**< The screen name is set to the application name. */ \
                                                     .magic = GUI_MAGIC_NUMBER, /**< check number. */ \
                                                   }, \
                                         .ui_design = _##APP_NAME##_ui_design, /**< The UI design function is assigned with the modified name. */ \
                                         .active_ms = 1000000, /**< The active duration is set to 1,000,000 milliseconds. */ \
                                       }; \
    gui_app_t *_get_app_##APP_NAME##_handle(void) \
    { \
        return &_app_##APP_NAME; \
    }

GUI_APP_DEFINE_NAME(APP_BOX2D_RING)
GUI_APP_DEFINE_NAME(APP_FRUIT_NINJA)
GUI_APP_DEFINE_NAME(APP_MUSIC)

extern void sidebar_app_array_fill(void *img_addr, gui_event_cb_t callback_function);
extern gui_app_t *_get_app_APP_HEART_RATE_handle(void);
void switch_APP_HEART_RATE(void *obj, gui_event_t e, void *param)
{
    gui_switch_app(gui_current_app(), _get_app_APP_HEART_RATE_handle());
}

void switch_BOX2D_RING(void *obj, gui_event_t e, void *param)
{
    gui_switch_app(gui_current_app(), _get_app_APP_BOX2D_RING_handle());
}

void switch_APP_FRUIT_NINJA(void *obj, gui_event_t e, void *param)
{
    gui_switch_app(gui_current_app(), _get_app_APP_FRUIT_NINJA_handle());
}

void switch_APP_MUSIC(void *obj, gui_event_t e, void *param)
{
    gui_switch_app(gui_current_app(), _get_app_APP_MUSIC_handle());
}

// static void switch_APP_CALCULATOR()
// {
//     gui_switch_app(gui_current_app(), _get_app_APP_CALCULATOR_handle());
// }

extern const uint32_t *gui_app_return_array[17];

static void app_menu_design(gui_app_t *app);
static gui_app_t app_menu =
{
    .screen =
    {
        .name = "app_menu",
        .x    = 0,
        .y    = 0,
        .magic = GUI_MAGIC_NUMBER,
        .parent = NULL,
    },
    .ui_design = app_menu_design,
    .thread_entry = NULL,
    .active_ms = 1000000,
};

void *get_app_menu()
{
    return &app_menu;
}

static void menu_win_ani_cb(void *args, gui_win_t *win)
{
    float pro = gui_win_get_animation_progress_percent(win);
    gui_win_set_scale_rate(win, sinf(pro * (M_PI / 2 - 0.2f) + 0.2f),
                           sinf(pro * (M_PI / 2 - 0.2f) + 0.2f));
    gui_win_set_opacity(win, (pro) * UINT8_MAX);
    if (gui_win_is_animation_end_frame(win))
    {
        gui_win_set_scale_rate(win, 0, 0);//reset scale
    }
}

static void app_menu_cb(void *obj, gui_event_t e, void *param)
{
    gui_log("%d,%p\n", GUI_TYPE(gui_obj_t, obj)->type, param);
}

static void app_back2watchface_cb(void)
{
    gui_switch_app(gui_current_app(), get_app_hongkong());
}

static void app_back2prescreen_cb(void)
{
    extern bool sidebar_flag;
    if (sidebar_flag)
    {
        gui_switch_app(gui_current_app(), get_app_hongkong());
    }
    else
    {
        gui_switch_app(gui_current_app(), get_app_menu());
    }
}

static void app_BOX2D_RING_return_cb(void)
{
    extern bool sidebar_flag;
    if (sidebar_flag)
    {
        gui_switch_app(gui_current_app(), get_app_hongkong());
    }
    else
    {
        gui_switch_app(gui_current_app(), get_app_menu());
    }
}

static void app_menu_design(gui_app_t *app)
{
    /**
     * @link https://docs.realmcu.com/Honeygui/latest/widgets/gui_menu_cellular.html#example
    */
    gui_win_t *win = gui_win_create(GUI_APP_ROOT_SCREEN, "win_app_menu", 0, 0, SCREEN_WIDTH,
                                    SCREEN_HEIGHT);
    gui_win_set_animate(win, 1000, 0, (gui_animate_callback_t)menu_win_ani_cb,
                        0);//aniamtion start to play at app startup
    /* app swap animation configration of the next app*/
    uint32_t *array[] =
    {
        UI_CLOCK_HEARTRATE_ICON_BIN,
        UI_CLOCK_MUSIC_ICON_BIN,
        UI_CLOCK_FRUIT_NINJA_ICON_BIN,
        UI_CLOCK_BOX2D_RING_ICON_BIN,
        UI_CLOCK_COMPASS_ICON_BIN,
        UI_CLOCK_HEARTRATE_ICON_BIN,
        UI_CLOCK_MUSIC_ICON_BIN,
        UI_CLOCK_FRUIT_NINJA_ICON_BIN,
        UI_CLOCK_BOX2D_RING_ICON_BIN,
        UI_CLOCK_COMPASS_ICON_BIN,
        UI_CLOCK_HEARTRATE_ICON_BIN,
        UI_CLOCK_MUSIC_ICON_BIN,
        UI_CLOCK_FRUIT_NINJA_ICON_BIN,
        UI_CLOCK_BOX2D_RING_ICON_BIN,
        UI_CLOCK_COMPASS_ICON_BIN,
        UI_CLOCK_HEARTRATE_ICON_BIN,
        UI_CLOCK_MUSIC_ICON_BIN,
        UI_CLOCK_FRUIT_NINJA_ICON_BIN,
        UI_CLOCK_BOX2D_RING_ICON_BIN,
        UI_CLOCK_COMPASS_ICON_BIN,
        UI_CLOCK_HEARTRATE_ICON_BIN,
        UI_CLOCK_MUSIC_ICON_BIN,
        UI_CLOCK_FRUIT_NINJA_ICON_BIN,
        UI_CLOCK_BOX2D_RING_ICON_BIN,
        UI_CLOCK_COMPASS_ICON_BIN,
        UI_CLOCK_HEARTRATE_ICON_BIN,
        UI_CLOCK_MUSIC_ICON_BIN,
        UI_CLOCK_FRUIT_NINJA_ICON_BIN,
        UI_CLOCK_BOX2D_RING_ICON_BIN,
        UI_CLOCK_COMPASS_ICON_BIN,
        UI_CLOCK_HEARTRATE_ICON_BIN,
        UI_CLOCK_MUSIC_ICON_BIN,
        UI_CLOCK_FRUIT_NINJA_ICON_BIN,
        UI_CLOCK_BOX2D_RING_ICON_BIN,
        UI_CLOCK_COMPASS_ICON_BIN,
        UI_CLOCK_HEARTRATE_ICON_BIN,
        UI_CLOCK_MUSIC_ICON_BIN,
        UI_CLOCK_FRUIT_NINJA_ICON_BIN,
        UI_CLOCK_BOX2D_RING_ICON_BIN,
        UI_CLOCK_COMPASS_ICON_BIN,
        UI_CLOCK_HEARTRATE_ICON_BIN,
        UI_CLOCK_MUSIC_ICON_BIN,
        UI_CLOCK_FRUIT_NINJA_ICON_BIN,
        UI_CLOCK_BOX2D_RING_ICON_BIN,
        UI_CLOCK_COMPASS_ICON_BIN,
        UI_CLOCK_HEARTRATE_ICON_BIN,
    };

    gui_menu_cellular_t *cell = gui_menu_cellular_create(win, 100, array,
                                                         sizeof(array) / sizeof(uint32_t *));
    gui_menu_cellular_offset((void *)cell, -200, -416);
    {
        extern bool return_to_watchface_flag;
        return_to_watchface_flag = false;
        struct gui_menu_cellular_gesture_parameter gesture_parameter_array[] =
        {
            {switch_APP_HEART_RATE, (void *)1},
            {switch_APP_MUSIC, (void *)1}, {switch_APP_FRUIT_NINJA, (void *)1}, {switch_BOX2D_RING, (void *)1}, {app_menu_cb, (void *)1},
            {switch_APP_HEART_RATE, (void *)1},
            {switch_APP_MUSIC, (void *)1}, {switch_APP_FRUIT_NINJA, (void *)1}, {switch_BOX2D_RING, (void *)1}, {app_menu_cb, (void *)1},
            {switch_APP_HEART_RATE, (void *)1},
            {switch_APP_MUSIC, (void *)1}, {switch_APP_FRUIT_NINJA, (void *)1}, {switch_BOX2D_RING, (void *)1}, {app_menu_cb, (void *)1},
            {switch_APP_HEART_RATE, (void *)1},
            {switch_APP_MUSIC, (void *)1}, {switch_APP_FRUIT_NINJA, (void *)1}, {switch_BOX2D_RING, (void *)1}, {app_menu_cb, (void *)1},
            {switch_APP_HEART_RATE, (void *)1},
            {switch_APP_MUSIC, (void *)1}, {switch_APP_FRUIT_NINJA, (void *)1}, {switch_BOX2D_RING, (void *)1}, {app_menu_cb, (void *)1},
            {switch_APP_HEART_RATE, (void *)1},
            {switch_APP_MUSIC, (void *)1}, {switch_APP_FRUIT_NINJA, (void *)1}, {switch_BOX2D_RING, (void *)1}, {app_menu_cb, (void *)1},
            {switch_APP_HEART_RATE, (void *)1},
            {switch_APP_MUSIC, (void *)1}, {switch_APP_FRUIT_NINJA, (void *)1}, {switch_BOX2D_RING, (void *)1}, {app_menu_cb, (void *)1},
            {switch_APP_HEART_RATE, (void *)1},
            {switch_APP_MUSIC, (void *)1}, {switch_APP_FRUIT_NINJA, (void *)1}, {switch_BOX2D_RING, (void *)1}, {app_menu_cb, (void *)1},
            {switch_APP_HEART_RATE, (void *)1},
            {switch_APP_MUSIC, (void *)1}, {switch_APP_FRUIT_NINJA, (void *)1}, {switch_BOX2D_RING, (void *)1}, {app_menu_cb, (void *)1},
            {switch_APP_HEART_RATE, (void *)1},
        };
        gui_menu_cellular_on_click(cell, gesture_parameter_array,
                                   sizeof(gesture_parameter_array) / sizeof(gesture_parameter_array[0]));

    }
    //status_bar(win, (void *)cell);
    gui_return_create(win, gui_app_return_array,
                      sizeof(gui_app_return_array) / sizeof(uint32_t *), app_back2watchface_cb, (void *)cell);
}

GUI_APP_ENTRY(APP_BOX2D_RING)
{
    sidebar_app_array_fill(UI_CLOCK_BOX2D_RING_ICON_BIN, switch_BOX2D_RING);
    extern void app_box2d_ring_ui_design(gui_obj_t *obj);
    app_box2d_ring_ui_design(GUI_APP_ROOT_SCREEN);
    gui_return_create(GUI_APP_ROOT_SCREEN, gui_app_return_array,
                      sizeof(gui_app_return_array) / sizeof(uint32_t *), app_BOX2D_RING_return_cb, (void *)0);
}

GUI_APP_ENTRY(APP_MUSIC)
{
    sidebar_app_array_fill(UI_CLOCK_MUSIC_ICON_BIN, switch_APP_MUSIC);
    extern void app_music_ui_design(gui_obj_t *obj);
    app_music_ui_design(GUI_APP_ROOT_SCREEN);
    gui_return_create(GUI_APP_ROOT_SCREEN, gui_app_return_array,
                      sizeof(gui_app_return_array) / sizeof(uint32_t *), app_back2prescreen_cb, (void *)0);
    // gui_fps_create(GUI_APP_ROOT_SCREEN);
}

GUI_APP_ENTRY(APP_FRUIT_NINJA)
{
    sidebar_app_array_fill(UI_CLOCK_FRUIT_NINJA_ICON_BIN, switch_APP_FRUIT_NINJA);
    extern void app_fruit_ninja_design(gui_obj_t *obj);
    app_fruit_ninja_design(GUI_APP_ROOT_SCREEN);
    gui_return_create(GUI_APP_ROOT_SCREEN, gui_app_return_array,
                      sizeof(gui_app_return_array) / sizeof(uint32_t *), app_back2prescreen_cb, (void *)0);
}