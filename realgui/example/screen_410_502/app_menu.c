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

#define SCREEN_WIDTH 410
#define SCREEN_HEIGHT 502

#define APP_MUSIC
#define APP_FRUIT_NINJA
#define APP_BOX2D_RING
#define APP_HEART_RATE
#define APP_ACTIVITY

//cppcheck-suppress syntaxError
GUI_APP_DEFINE_NAME_ANIMATION(APP_BOX2D_RING, GUI_APP_ANIMATION_9,
                              GUI_APP_ANIMATION_10)
GUI_APP_DEFINE_NAME_ANIMATION(APP_FRUIT_NINJA, GUI_APP_ANIMATION_2, GUI_APP_ANIMATION_5)
GUI_APP_DEFINE_NAME_ANIMATION(APP_MUSIC, GUI_APP_ANIMATION_3, GUI_APP_ANIMATION_7)
GUI_APP_DEFINE_NAME_ANIMATION(APP_HEART_RATE, GUI_APP_ANIMATION_4, GUI_APP_ANIMATION_5)
GUI_APP_DEFINE_NAME_ANIMATION(APP_ACTIVITY, GUI_APP_ANIMATION_4, GUI_APP_ANIMATION_5)
// GUI_APP_DEFINE_NAME(APP_BOX2D_RING)
// GUI_APP_DEFINE_NAME(APP_FRUIT_NINJA)
// GUI_APP_DEFINE_NAME(APP_MUSIC)
// GUI_APP_DEFINE_NAME(APP_HEART_RATE)
// GUI_APP_DEFINE_NAME(APP_ACTIVITY)

extern void sidebar_app_array_fill(void *img_addr, gui_event_cb_t callback_function);

void switch_BOX2D_RING(void *obj, gui_event_t e, void *param)
{
    extern void close_box2d_ring(void);
    close_box2d_ring();

    gui_app_switch(gui_current_app(), _get_app_APP_BOX2D_RING_handle());
}

void switch_APP_FRUIT_NINJA(void *obj, gui_event_t e, void *param)
{
    gui_app_switch(gui_current_app(), _get_app_APP_FRUIT_NINJA_handle());
}

void switch_APP_MUSIC(void *obj, gui_event_t e, void *param)
{
    gui_app_switch(gui_current_app(), _get_app_APP_MUSIC_handle());
}

void switch_APP_HEART_RATE(void *obj, gui_event_t e, void *param)
{
    gui_app_switch(gui_current_app(), _get_app_APP_HEART_RATE_handle());
}

void switch_APP_ACTIVITY(void *obj, gui_event_t e, void *param)
{
    gui_app_switch(gui_current_app(), _get_app_APP_ACTIVITY_handle());
}

// static void switch_APP_CALCULATOR()
// {
//     gui_app_switch(gui_current_app(), _get_app_APP_CALCULATOR_handle());
// }

/*Define gui_app_return_array*/
static const uint32_t *gui_app_return_array[] =
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

gui_app_t *get_app_menu()
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
    gui_app_switch(gui_current_app(), get_app_hongkong());
}

static void app_back2prescreen_cb(void)
{
    extern bool sidebar_flag;
    extern bool return_to_watchface_flag;

    if (return_to_watchface_flag || sidebar_flag)
    {
        gui_app_switch(gui_current_app(), get_app_hongkong());
    }
    else
    {
        gui_app_switch(gui_current_app(), get_app_menu());
    }
}

static void app_AT_back2prescreen_cb(void)
{
    extern bool sidebar_flag;
    extern bool return_to_watchface_flag;

    app_back2prescreen_cb();
}

static void app_HR_back2prescreen_cb(void)
{
    extern void clear_heart_rate_app(void);

    app_back2prescreen_cb();
    clear_heart_rate_app();
}

static void app_FN_back2prescreen_cb(void)
{
    extern void close_FN_APP();
    close_FN_APP();

    app_back2prescreen_cb();
}

static void app_BOX2D_RING_return_cb(void)
{
    extern void close_box2d_ring(void);
    close_box2d_ring();

    app_back2prescreen_cb();
}

static void app_menu_design(gui_app_t *app)
{
    extern void page_tb_control_enter(void *parent);
    page_tb_control_enter(&app->screen);
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

    gui_menu_cellular_t *cell = gui_menu_cellular_create(win, 100, array,
                                                         sizeof(array) / sizeof(uint32_t *));
    // gui_menu_cellular_offset((void *)cell, -200, -416);
    {
        extern bool return_to_watchface_flag;
        return_to_watchface_flag = false;
        struct gui_menu_cellular_gesture_parameter gesture_parameter_array[] =
        {
            {switch_APP_HEART_RATE, (void *)1},
            {switch_APP_MUSIC, (void *)1}, {switch_APP_FRUIT_NINJA, (void *)1}, {switch_BOX2D_RING, (void *)1}, {switch_APP_ACTIVITY, (void *)1},
            {switch_APP_HEART_RATE, (void *)1},
            {switch_APP_MUSIC, (void *)1}, {switch_APP_FRUIT_NINJA, (void *)1}, {switch_BOX2D_RING, (void *)1}, {switch_APP_ACTIVITY, (void *)1},
            {switch_APP_HEART_RATE, (void *)1},
            {switch_APP_MUSIC, (void *)1}, {switch_APP_FRUIT_NINJA, (void *)1}, {switch_BOX2D_RING, (void *)1}, {switch_APP_ACTIVITY, (void *)1},
            {switch_APP_HEART_RATE, (void *)1},
            {switch_APP_MUSIC, (void *)1}, {switch_APP_FRUIT_NINJA, (void *)1}, {switch_BOX2D_RING, (void *)1}, {switch_APP_ACTIVITY, (void *)1},
            {switch_APP_HEART_RATE, (void *)1},
            {switch_APP_MUSIC, (void *)1}, {switch_APP_FRUIT_NINJA, (void *)1}, {switch_BOX2D_RING, (void *)1}, {switch_APP_ACTIVITY, (void *)1},
            {switch_APP_HEART_RATE, (void *)1},
            {switch_APP_MUSIC, (void *)1}, {switch_APP_FRUIT_NINJA, (void *)1}, {switch_BOX2D_RING, (void *)1}, {switch_APP_ACTIVITY, (void *)1},
            {switch_APP_HEART_RATE, (void *)1},
            {switch_APP_MUSIC, (void *)1}, {switch_APP_FRUIT_NINJA, (void *)1}, {switch_BOX2D_RING, (void *)1}, {switch_APP_ACTIVITY, (void *)1},
            {switch_APP_HEART_RATE, (void *)1},
            {switch_APP_MUSIC, (void *)1}, {switch_APP_FRUIT_NINJA, (void *)1}, {switch_BOX2D_RING, (void *)1}, {switch_APP_ACTIVITY, (void *)1},
            {switch_APP_HEART_RATE, (void *)1},
            {switch_APP_MUSIC, (void *)1}, {switch_APP_FRUIT_NINJA, (void *)1}, {switch_BOX2D_RING, (void *)1}, {switch_APP_ACTIVITY, (void *)1},
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
    extern void page_tb_control_enter(void *parent);
    page_tb_control_enter(&app->screen);

    extern void app_box2d_ring_ui_design(gui_obj_t *obj);
    app_box2d_ring_ui_design(GUI_APP_ROOT_SCREEN);
    gui_return_create(GUI_APP_ROOT_SCREEN, gui_app_return_array,
                      sizeof(gui_app_return_array) / sizeof(uint32_t *), app_BOX2D_RING_return_cb, (void *)0);
}

GUI_APP_ENTRY(APP_MUSIC)
{
    sidebar_app_array_fill(UI_CLOCK_MUSIC_ICON_BIN, switch_APP_MUSIC);
    extern void page_tb_control_enter(void *parent);
    page_tb_control_enter(&app->screen);

    extern void app_music_ui_design(gui_obj_t *obj);
    app_music_ui_design(GUI_APP_ROOT_SCREEN);
    gui_return_create(GUI_APP_ROOT_SCREEN, gui_app_return_array,
                      sizeof(gui_app_return_array) / sizeof(uint32_t *), app_back2prescreen_cb, (void *)0);
    // gui_fps_create(GUI_APP_ROOT_SCREEN);
}

GUI_APP_ENTRY(APP_FRUIT_NINJA)
{
    sidebar_app_array_fill(UI_CLOCK_FRUIT_NINJA_ICON_BIN, switch_APP_FRUIT_NINJA);
    extern void page_tb_control_enter(void *parent);
    page_tb_control_enter(&app->screen);

    extern void app_fruit_ninja_design(gui_obj_t *obj);
    app_fruit_ninja_design(GUI_APP_ROOT_SCREEN);
    gui_return_create(GUI_APP_ROOT_SCREEN, gui_app_return_array,
                      sizeof(gui_app_return_array) / sizeof(uint32_t *), app_FN_back2prescreen_cb, (void *)0);
}

GUI_APP_ENTRY(APP_HEART_RATE)
{
    sidebar_app_array_fill(UI_CLOCK_HEARTRATE_ICON_BIN, switch_APP_HEART_RATE);
    extern void page_tb_control_enter(void *parent);
    page_tb_control_enter(&app->screen);

    extern void heart_rate_app(gui_obj_t *obj);
    heart_rate_app(GUI_APP_ROOT_SCREEN);
    gui_return_create(GUI_APP_ROOT_SCREEN, gui_app_return_array,
                      sizeof(gui_app_return_array) / sizeof(uint32_t *), app_HR_back2prescreen_cb, (void *)0);
}

GUI_APP_ENTRY(APP_ACTIVITY)
{
    sidebar_app_array_fill(UI_CLOCK_ACTIVITY_ICON_BIN, switch_APP_ACTIVITY);
    extern void page_tb_control_enter(void *parent);
    page_tb_control_enter(&app->screen);

    extern void activity_app(gui_obj_t *obj);
    activity_app(GUI_APP_ROOT_SCREEN);
    gui_return_create(GUI_APP_ROOT_SCREEN, gui_app_return_array,
                      sizeof(gui_app_return_array) / sizeof(uint32_t *), app_AT_back2prescreen_cb, (void *)0);
}