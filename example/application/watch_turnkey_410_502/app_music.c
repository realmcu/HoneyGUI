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
#define CURRENT_VIEW_NAME "music_view"

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void music_view_design(gui_view_t *view);

/*============================================================================*
 *                            Variables
 *============================================================================*/
static gui_list_t *list;
static int16_t list_offset_his = 0;
/*============================================================================*
 *                           Private Functions
 *============================================================================*/
GUI_VIEW_INSTANCE(CURRENT_VIEW_NAME, false, music_view_design, NULL);
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
            GUI_WIDGET_POINTER_BY_NAME_ROOT(t_time, "time_b", current_view);
            gui_text_content_set((gui_text_t *)t_time, time_str, strlen(time_str));
        }

    }
}
static void music_ctr_press_cb(void *p)
{
    GUI_UNUSED(p);
    gui_view_switch_direct(current_view, gui_view_descriptor_get("music_ctr_view"),
                           SWITCH_IN_ANIMATION_FADE, SWITCH_OUT_ANIMATION_FADE);
}
static void music_homeapge_press_cb(void *p)
{
    GUI_UNUSED(p);
}
static void music_broadcast_press_cb(void *p)
{
    GUI_UNUSED(p);
}
static void music_library_press_cb(void *p)
{
    GUI_UNUSED(p);
}
static void note_design(gui_obj_t *obj, void *p)
{
    (void)obj;
    (void)p;
    uint16_t index = ((gui_list_note_t *)obj)->index;
    int16_t offset_X = 29;
    int16_t offset_x_icon = 20;
    // gui_lite_round_rect_t *rect_bg = gui_lite_round_rect_create(obj, "rect_bg");
    // gui_lite_round_rect_set_style(rect_bg, offset_X, 0, 352, 115, 30, RGBA_color(98, 101, 98, 255 * 0.7));
    // gui_canvas_round_rect_t *rect_bg = gui_canvas_round_rect_create(obj, "canvas_bg",offset_X, 0, 352, 115, 30, gui_rgba(98, 101, 98, 255 * 0.7));

    if (index == 0)
    {
        // rect_bg->base.not_show = true;
        gui_text_t *sport_text = gui_text_create(current_view, "ac_text1", SCREEN_WIDTH / 2 - 40, 60, 0, 0);
        gui_text_set(sport_text, "Music", GUI_FONT_SRC_TTF, gui_rgb(231, 31, 69),
                     strlen("Music"), 40);
        gui_text_type_set(sport_text, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(sport_text, LEFT);

        gui_img_t *music_ctr = gui_img_create_from_mem(current_view, "music", MUSIC_CTR_ICON_BIN, 300,
                                                       30, 0, 0);
        gui_obj_add_event_cb(GUI_BASE(music_ctr), (gui_event_cb_t)music_ctr_press_cb,
                             GUI_EVENT_TOUCH_CLICKED, NULL);
    }
    else if (index == 1)
    {
        // note_activity
        {
            gui_canvas_round_rect_t *rect_bg = gui_canvas_round_rect_create(obj, "canvas_bg", offset_X, 0, 352,
                                                                            115, 30, gui_rgba(98, 101, 98, 255 * 0.7));
            gui_img_create_from_mem(rect_bg, "Homepage", MUSIC_HOMEPAGE_BIN, offset_x_icon,
                                    20, 0, 0);

            gui_text_t *sport_text = gui_text_create(rect_bg, "ac_text1", 130, 40, 0, 0);
            gui_text_set(sport_text, "Homepage", GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255),
                         strlen("Homepage"), 48);
            gui_text_type_set(sport_text, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
            gui_text_mode_set(sport_text, LEFT);
            gui_obj_add_event_cb(GUI_BASE(rect_bg), (gui_event_cb_t)music_homeapge_press_cb,
                                 GUI_EVENT_TOUCH_CLICKED, NULL);
        }
    }
    else if (index == 2)
    {
        // note_music
        gui_canvas_round_rect_t *rect_bg = gui_canvas_round_rect_create(obj, "canvas_bg", offset_X, 0, 352,
                                                                        115, 30, gui_rgba(98, 101, 98, 255 * 0.7));
        gui_img_create_from_mem(rect_bg, "music", MUSIC_BROADCAST_ICON_BIN, offset_x_icon,
                                20, 0, 0);

        gui_text_t *sport_text = gui_text_create(rect_bg, "music_text1", 130, 40, 0, 0);
        gui_text_set(sport_text, "Broadcast", GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255),
                     strlen("Broadcast"), 48);
        gui_text_type_set(sport_text, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(sport_text, LEFT);
        gui_obj_add_event_cb(GUI_BASE(rect_bg), (gui_event_cb_t)music_broadcast_press_cb,
                             GUI_EVENT_TOUCH_CLICKED, NULL);
    }
    else if (index == 3)
    {
        // note_music
        gui_canvas_round_rect_t *rect_bg = gui_canvas_round_rect_create(obj, "canvas_bg", offset_X, 0, 352,
                                                                        115, 30, gui_rgba(98, 101, 98, 255 * 0.7));
        gui_img_t *img = gui_img_create_from_mem(rect_bg, "music", MUSIC_LIB_BIN, offset_x_icon,
                                                 20, 0, 0);
        gui_img_scale(img, 0.7, 0.7);

        gui_text_t *sport_text = gui_text_create(rect_bg, "music_text1", 130, 40, 0, 0);
        gui_text_set(sport_text, "Library", GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255),
                     strlen("Library"), 48);
        gui_text_type_set(sport_text, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(sport_text, LEFT);
        gui_obj_add_event_cb(GUI_BASE(rect_bg), (gui_event_cb_t)music_library_press_cb,
                             GUI_EVENT_TOUCH_CLICKED, NULL);
    }
}
static void list_timer_cb(void *obj)
{
    gui_list_t *list = (gui_list_t *)obj;
    list_offset_his = list->offset;
    int range = list->base.h - list->total_length - list->out_scope;

    int t_y = list_offset_his * (SCROLLBAR_BG_H - SCROLLBAR_L_H) / range;
    gui_obj_t *scrollbar_bg = gui_list_entry(GUI_BASE(obj)->brother_list.next, gui_obj_t,
                                             brother_list);
    gui_obj_t *scrollbar = gui_list_entry(scrollbar_bg->brother_list.next, gui_obj_t,
                                          brother_list);
    if (list_offset_his <= 0)
    {
        gui_img_translate((void *)scrollbar, 0, t_y);
    }

}
static void music_view_design(gui_view_t *view)
{
    const char *obj_name = gui_view_get_current()->descriptor->name;
    VIEW_SWITCH_STYLE swtich_in = SWITCH_IN_ANIMATION_FADE;
    VIEW_SWITCH_STYLE swtich_out = SWITCH_OUT_ANIMATION_FADE;
    if (strcmp(obj_name, "bottom_view") == 0)
    {
        swtich_in = SWITCH_IN_FROM_BOTTOM_USE_TRANSLATION;
        swtich_out = SWITCH_OUT_TO_BOTTOM_USE_TRANSLATION;
    }
    gui_view_switch_on_event(view, gui_view_descriptor_get("watchface_view"),
                             swtich_out,
                             swtich_in,
                             GUI_EVENT_KB_SHORT_CLICKED);
    gui_obj_t *parent = GUI_BASE(view);



    int space = 5;
    int length = 115;
    list = gui_list_create(parent, "list", 0, 0, 0, 0, length, space, VERTICAL, note_design, NULL,
                           false);
    gui_list_set_style(list, LIST_CLASSIC);
    gui_list_set_note_num(list, 4);
    gui_list_set_offset(list, list_offset_his);
    gui_list_set_out_scope(list, 80);
    gui_obj_create_timer(GUI_BASE(list), 10, true, list_timer_cb);
    gui_img_t *scrollbar_bg = gui_img_create_from_mem(parent, 0, SCROLLBAR_BG_BIN, 395, 80, 0, 0);
    gui_img_set_mode(scrollbar_bg, IMG_SRC_OVER_MODE);
    gui_img_t *scrollbar = gui_img_create_from_mem(parent, 0, SCROLLBAR_L_BIN, 394, 80, 0, 0);
    gui_img_set_mode(scrollbar, IMG_SRC_OVER_MODE);

    // ----------time-----------------//
    gui_text_t *t_time = gui_text_create(parent, "time_b", 0, 0, SCREEN_WIDTH, 80);
    gui_text_set(t_time, time_str, GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255),
                 strlen(time_str), 60);
    gui_text_type_set(t_time, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t_time, MID_CENTER);
    gui_text_rendermode_set(t_time, 2);



    gui_obj_create_timer(GUI_BASE(view), 30000, true, time_update_cb);
}


