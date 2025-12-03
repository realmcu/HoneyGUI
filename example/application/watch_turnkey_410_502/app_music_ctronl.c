/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "root_image/ui_resource.h"
#include "app_main_watch.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_text.h"
#include "tp_algo.h"
#include "gui_canvas.h"
#include "gui_canvas_rect.h"
#include "gui_canvas_round_rect.h"
#include "guidef.h"
#include "gui_list.h"
#include "app_main_watch.h"
#include "gui_view_instance.h"
#include "gui_lite_geometry_round_rect.h"
/*============================================================================*
 *                           Types
 *============================================================================*/

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define SCREEN_WIDTH (int16_t)gui_get_screen_width()
#define SCREEN_HEIGHT (int16_t)gui_get_screen_height()
#define CURRENT_VIEW_NAME "music_ctr_view"

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void music_ctr_design(gui_view_t *view);

/*============================================================================*
 *                            Variables
 *============================================================================*/
gui_img_t *img_music_ctr = NULL;
static bool is_playing = false;
gui_text_t *img_music_text = NULL;
/*============================================================================*
 *                           Private Functions
 *============================================================================*/
GUI_VIEW_INSTANCE(CURRENT_VIEW_NAME, false, music_ctr_design, NULL);
static void time_update_cb(void *p)
{
    GUI_UNUSED(p);
    if (!timeinfo)
    {
        return;
    }
    else
    {
        {
            GUI_WIDGET_POINTER_BY_NAME_ROOT(t_time, "time_b", gui_view_get_current());
            gui_text_content_set((gui_text_t *)t_time, time_str, strlen(time_str));
        }

    }
}
static void music_pre_press_cb(void *p)
{
    GUI_UNUSED(p);
    gui_log("music_pre_press_cb\n");
}
static void music_next_press_cb(void *p)
{
    GUI_UNUSED(p);
    gui_log("music_next_press_cb\n");
}
static void music_return_press_cb(void *p)
{
    GUI_UNUSED(p);
    gui_view_switch_direct(gui_view_get_current(), "music_view",
                           SWITCH_IN_ANIMATION_FADE, SWITCH_OUT_ANIMATION_FADE);
}
static void music_ctr_press_cb(void *p)
{
    GUI_UNUSED(p);
    is_playing = !is_playing;
    if (is_playing)
    {
        gui_img_set_image_data(img_music_ctr, PAUSE_BIN);
        gui_text_content_set(img_music_text, "Now playing", strlen("Now playing"));
    }
    else
    {
        gui_img_set_image_data(img_music_ctr, PLAY_BIN);
        gui_text_content_set(img_music_text, "Not played", strlen("Not played"));
    }
    gui_img_refresh_size(img_music_ctr);
}
static void music_ctr_design(gui_view_t *view)
{
    gui_obj_t *parent = GUI_BASE(view);


    // ----------time-----------------//
    gui_text_t *t_time = gui_text_create(parent, "time_b", 0, 0, SCREEN_WIDTH, 80);
    gui_text_set(t_time, time_str, GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255),
                 strlen(time_str), 60);
    gui_text_type_set(t_time, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t_time, MID_CENTER);
    gui_text_rendermode_set(t_time, 2);
    gui_obj_create_timer(GUI_BASE(view), 30000, true, time_update_cb);


    gui_img_t *img_music_bg = gui_img_create_from_mem(parent, "img_music_bg", RECTANGLE86_BIN, 100, 100,
                                                      0, 0);
    gui_img_scale(img_music_bg, 0.7, 0.7);
    gui_img_t *img_music_pre = gui_img_create_from_mem(parent, "img_music_pre", MUSIC_PRE_BIN, 30, 400,
                                                       0, 0);
    gui_obj_add_event_cb(GUI_BASE(img_music_pre), (gui_event_cb_t)music_pre_press_cb,
                         GUI_EVENT_TOUCH_CLICKED, NULL);

    gui_img_t *img_music_next = gui_img_create_from_mem(parent, "img_music_next", MUSIC_NEXT_BIN, 300,
                                                        400, 0, 0);
    gui_obj_add_event_cb(GUI_BASE(img_music_next), (gui_event_cb_t)music_next_press_cb,
                         GUI_EVENT_TOUCH_CLICKED, NULL);

    {
        img_music_ctr = gui_img_create_from_mem(parent, "img_music_ctr", PLAY_BIN, 180, 410, 0, 0);
        gui_obj_add_event_cb(GUI_BASE(img_music_ctr), (gui_event_cb_t)music_ctr_press_cb,
                             GUI_EVENT_TOUCH_CLICKED, NULL);

        img_music_text = gui_text_create(parent, "battery_text", 115, 340, 0, 0);
        gui_text_set(img_music_text, "Not played", GUI_FONT_SRC_TTF, APP_COLOR_WHITE, strlen("Not played"),
                     42);
        gui_text_type_set(img_music_text, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(img_music_text, LEFT);
    }


    gui_img_t *img_music_return = gui_img_create_from_mem(parent, "img_music_return", DELETE_ICON_BIN,
                                                          30, 30, 0, 0);
    gui_obj_add_event_cb(GUI_BASE(img_music_return), (gui_event_cb_t)music_return_press_cb,
                         GUI_EVENT_TOUCH_CLICKED, NULL);

}


