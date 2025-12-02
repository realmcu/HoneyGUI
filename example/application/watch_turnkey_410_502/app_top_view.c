/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "root_image/ui_resource.h"
#include "app_main_watch.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_text.h"
#include "tp_algo.h"
#include "gui_lite_geometry_round_rect.h"
#include "guidef.h"
#include "gui_list.h"
#include "app_main_watch.h"
#include "gui_view_instance.h"
/*============================================================================*
 *                           Types
 *============================================================================*/

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define SCREEN_WIDTH (int16_t)gui_get_screen_width()
#define SCREEN_HEIGHT (int16_t)gui_get_screen_height()
#define NOTE_HEIGHT 220
#define NOTE_INTERVAL 20
#define NOTE_START 140
#define INFOR_NUM_MAX 4
#define CURRENT_VIEW_NAME "app_top_view"

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void top_view_design(gui_view_t *view);

/*============================================================================*
 *                           GUI_VIEW_INSTANCE
 *============================================================================*/
GUI_VIEW_INSTANCE(CURRENT_VIEW_NAME, false, top_view_design, NULL);

/*============================================================================*
 *                            Variables
 *============================================================================*/
static gui_list_t *list;
// static gui_canvas_round_rect_t *canvas_clear;
static bool is_cleared  = false;
// static gui_text_t *no_notification_text = NULL;
/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static void clear_all_note_cb(gui_event_t *e)
{
    (void)e;
    is_cleared = true;

    // if (no_notification_text == NULL) {
    //     no_notification_text = gui_text_create(GUI_BASE(list), "no_notification",
    //                                           0, 0, 0, 0);
    // }
    // gui_text_set(no_notification_text, "No notification", GUI_FONT_SRC_TTF,
    //              gui_rgb(200, 200, 200), strlen("No notification"), 36);
    // gui_text_type_set(no_notification_text, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
    // gui_text_mode_set(no_notification_text, MID_CENTER);

}
static void create_clear_note(void *parent)
{
    gui_lite_round_rect_t *rect_bg = gui_lite_round_rect_create(GUI_BASE(parent), "canvas_clear", 52,
                                                                40, 305, 80, 40, gui_rgba(39, 43, 44, 255));
    gui_obj_add_event_cb(GUI_BASE(rect_bg), (gui_event_cb_t)clear_all_note_cb,
                         GUI_EVENT_TOUCH_CLICKED, NULL);
    // text
    char *clear_content = "clear all";
    gui_text_t *clear_text = gui_text_create(rect_bg, "clear all", 0, 0, 0, 0);
    gui_text_set(clear_text, (void *)clear_content, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                 strlen(clear_content),
                 32);
    gui_text_type_set(clear_text, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(clear_text, MID_CENTER);
}
static void note_design(gui_obj_t *obj, void *p)
{
    (void)p;
    uint16_t index = ((gui_list_note_t *)obj)->index;
    int16_t offset_X = 29;
    if (is_cleared && index > 0)
    {
        return;
    }
    if (index == 0)
    {
        create_clear_note(obj);

    }
    else if (index == 1)
    {
        // note_activity
        {
            gui_lite_round_rect_t *rect_bg = gui_lite_round_rect_create(obj, "rect_bg", offset_X, 0, 352, 120,
                                                                        30, gui_rgba(98, 101, 98, 255 * 0.7));

            gui_text_t *sport_text0 = gui_text_create(rect_bg, "ac_text0", 230, 10, 0, 0);
            gui_text_set(sport_text0, "1 hour ago", GUI_FONT_SRC_TTF, gui_rgb(200, 200, 200),
                         strlen("1 hour ago"), 24);
            gui_text_type_set(sport_text0, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
            gui_text_mode_set(sport_text0, LEFT);

            gui_text_t *sport_text = gui_text_create(rect_bg, "ac_text1", 30, 50, 0, 0);
            gui_text_set(sport_text, "come on", GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255),
                         strlen("come on"), 48);
            gui_text_type_set(sport_text, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
            gui_text_mode_set(sport_text, LEFT);
            gui_img_create_from_mem(rect_bg, "activity", APP_ACTIVITY_ICON_BIN, 20, -50, 0, 0);
        }
    }
    else if (index == 2)
    {
        // note_music
        gui_lite_round_rect_t *rect_bg = gui_lite_round_rect_create(obj, "rect_bg", offset_X, 0, 352, 120,
                                                                    30, gui_rgba(98, 101, 98, 255 * 0.7));
        gui_img_t *img = gui_img_create_from_mem(rect_bg, "music", APP_MUSIC_ICON_BIN, 20,
                                                 -50, 0, 0);
        gui_img_scale(img, 0.85, 0.85);
        gui_text_t *sport_text0 = gui_text_create(rect_bg, "music_text0", 184, 10, 0, 0);
        gui_text_set(sport_text0, "45 mimute ago", GUI_FONT_SRC_TTF, gui_rgb(200, 200, 200),
                     strlen("45 mimute ago"), 24);
        gui_text_type_set(sport_text0, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(sport_text0, LEFT);

        gui_text_t *sport_text = gui_text_create(rect_bg, "music_text1", 30, 50, 0, 0);
        gui_text_set(sport_text, "music", GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255),
                     strlen("music"), 48);
        gui_text_type_set(sport_text, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(sport_text, LEFT);
    }
}
static void top_view_design(gui_view_t *view)
{
    gui_view_switch_on_event(view, "watchface_view",
                             SWITCH_OUT_TO_TOP_USE_TRANSLATION,
                             SWITCH_IN_STILL_USE_BLUR,
                             GUI_EVENT_TOUCH_MOVE_UP);
    // gui_view_set_opacity(view, 128);
    gui_obj_t *parent = GUI_BASE(view);
    // draw background
    if (current_watchface_type == WATCHFACE_BIG_NUM)
    {
        gui_lite_round_rect_create(GUI_BASE(parent), "background", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0,
                                   gui_rgba(219, 122, 147, 255 * 0.5));
    }
    else
    {

        gui_lite_round_rect_create(GUI_BASE(parent), "background", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0,
                                   gui_rgba(76, 76, 76, 255 * 0.5));
    }

    // create_clear_note(view);

    GUI_UNUSED(list);
    int space = 20;
    int length = 157;
    list = gui_list_create(parent, "list", 0, 0, 0, 0, length, space, VERTICAL, note_design, NULL,
                           false);
    gui_list_set_style(list, LIST_CLASSIC);
    gui_list_set_note_num(list, 3);
}


