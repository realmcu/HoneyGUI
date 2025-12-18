/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "root_image/ui_resource.h"
#include "guidef.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "app_main_watch.h"
#include "gui_view_instance.h"
#include "gui_list.h"
#include "gui_text.h"
#include "gui_arc.h"
/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "workout_start_ctr_view"
#define SCREEN_WIDTH (int16_t)gui_get_screen_width()
#define SCREEN_HEIGHT (int16_t)gui_get_screen_height()
/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void workout_start_ctr_design(gui_view_t *view);

extern int16_t g_selected_workout_index;

/*============================================================================*
 *                           GUI_VIEW_INSTANCE
 *============================================================================*/
GUI_VIEW_INSTANCE(CURRENT_VIEW_NAME, false, workout_start_ctr_design, NULL);

/*============================================================================*
 *                            Variables
 *============================================================================*/
static gui_list_t *list = NULL;
static int16_t list_offset_his = 0;
static gui_view_t *current_view = NULL;
static gui_img_t *workout_stop_icon = NULL;
static gui_img_t *workout_restart_icon = NULL;
static gui_text_t *workout_stop_text = NULL;
static bool workout_stop_state = false;

static gui_win_t *workout_start_ctr_remind = NULL;
extern char *workout_text_array[];
extern const int WORKOUT_TEXT_ARRAY_SIZE;
extern bool is_workout_stopped;
extern uint32_t stopped_workout_milliseconds;
extern uint32_t workout_milliseconds;
/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static void time_update_cb(void *obj)
{
    gui_text_content_set(obj, time_str, strlen(time_str));
}
static void workout_start_ctr_finish_press_cb(void *obj)
{
    (void)obj;
    gui_view_switch_direct(current_view, "workout_view",
                           SWITCH_IN_ANIMATION_FADE, SWITCH_OUT_ANIMATION_FADE);
}
static void workout_start_ctr_stop_press_cb(void *obj)
{
    (void)obj;

    if (workout_stop_state == false)
    {
        gui_obj_hidden(GUI_BASE(workout_stop_icon), true);
        gui_obj_hidden(GUI_BASE(workout_restart_icon), false);
        workout_stop_state = true;

        stopped_workout_milliseconds = workout_milliseconds;
        is_workout_stopped = true;
    }
    else
    {
        gui_obj_hidden(GUI_BASE(workout_stop_icon), false);
        gui_obj_hidden(GUI_BASE(workout_restart_icon), true);
        workout_stop_state = false;

        is_workout_stopped = false;
    }

    gui_view_switch_direct(current_view, "workout_start_view",
                           SWITCH_IN_ANIMATION_FADE, SWITCH_OUT_ANIMATION_FADE);
}
static void note_design(gui_obj_t *obj, void *p)
{
    (void)p;
    GUI_UNUSED(obj);
    uint16_t index = ((gui_list_note_t *)obj)->index;
    uint16_t bg_ctr_offset_x = 30;
    uint16_t bg_ctr_offset_y = 110;
    if (index == 0)
    {
        gui_img_t *workout_start_ctr_finish = gui_img_create_from_mem(obj, 0,
                                                                      WORKOUT_START_CTR_ICON_BG_A8_BIN, bg_ctr_offset_x, bg_ctr_offset_y, 0, 0);
        gui_img_a8_recolor(workout_start_ctr_finish, 0x3B1B18);
        gui_img_create_from_mem(workout_start_ctr_finish, 0, WORKOUT_START_FINISH_ICON_BIN, 60, 30, 0, 0);
        gui_text_t *finish_text = gui_text_create(obj, "ac_text1", 80, 210, 0, 0);
        gui_text_set(finish_text, "finish", GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255),
                     strlen("finish"), 36);
        gui_text_type_set(finish_text, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(finish_text, LEFT);
        gui_obj_add_event_cb(GUI_BASE(workout_start_ctr_finish),
                             (gui_event_cb_t)workout_start_ctr_finish_press_cb,
                             GUI_EVENT_TOUCH_CLICKED, NULL);

        gui_img_t *workout_start_ctr_stop = gui_img_create_from_mem(obj, 0,
                                                                    WORKOUT_START_CTR_ICON_BG_A8_BIN, bg_ctr_offset_x + 200, bg_ctr_offset_y, 0, 0);
        gui_img_a8_recolor(workout_start_ctr_stop, 0x463412);
        workout_stop_icon = gui_img_create_from_mem(workout_start_ctr_stop, "workout_stop_icon",
                                                    WORKOUT_START_STOP_ICON_BIN, 60, 25, 0, 0);
        workout_restart_icon = gui_img_create_from_mem(workout_start_ctr_stop, "workout_restart_icon",
                                                       WORKOUT_START_RESTART_ICON_BIN, 60, 25, 0, 0);

        if (workout_stop_state)
        {
            gui_obj_hidden(GUI_BASE(workout_stop_icon), true);
            gui_obj_hidden(GUI_BASE(workout_restart_icon), false);
        }
        else
        {
            gui_obj_hidden(GUI_BASE(workout_stop_icon), false);
            gui_obj_hidden(GUI_BASE(workout_restart_icon), true);
        }

        workout_stop_text = gui_text_create(obj, "workout_stop_text", 260, 210, 0, 0);
        if (workout_stop_state)
        {
            gui_text_set(workout_stop_text, "continue", GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255),
                         strlen("continue"), 36);
        }
        else
        {
            gui_text_set(workout_stop_text, "stop", GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255),
                         strlen("stop"), 36);
        }
        gui_text_type_set(workout_stop_text, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(workout_stop_text, LEFT);
        gui_obj_add_event_cb(GUI_BASE(workout_start_ctr_stop),
                             (gui_event_cb_t)workout_start_ctr_stop_press_cb,
                             GUI_EVENT_TOUCH_CLICKED, NULL);
    }
    if (index == 1)
    {
        gui_img_t *workout_start_ctr_new = gui_img_create_from_mem(obj, 0, WORKOUT_START_CTR_ICON_BG_A8_BIN,
                                                                   bg_ctr_offset_x, bg_ctr_offset_y, 0, 0);
        gui_img_a8_recolor(workout_start_ctr_new, 0x3D3237);
        gui_img_create_from_mem(workout_start_ctr_new, 0, WORKOUT_START_ADD_ICON_BIN, 60, 30, 0, 0);
        gui_text_t *new_text = gui_text_create(obj, "ac_text1", 80, 210, 0, 0);
        gui_text_set(new_text, "add", GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255),
                     strlen("add"), 36);
        gui_text_type_set(new_text, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(new_text, LEFT);

        gui_img_t *workout_start_ctr_segment = gui_img_create_from_mem(obj, 0,
                                                                       WORKOUT_START_CTR_ICON_BG_A8_BIN, bg_ctr_offset_x + 200, bg_ctr_offset_y, 0, 0);
        gui_img_a8_recolor(workout_start_ctr_segment, 0x383F12);

        gui_text_t *segment_text0 = gui_text_create(workout_start_ctr_segment, "ac_text1", 70, 30, 0, 0);
        gui_text_set(segment_text0, "1", GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255),
                     strlen("1"), 40);
        gui_text_type_set(segment_text0, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(segment_text0, LEFT);

        gui_text_t *segment_text = gui_text_create(obj, "ac_text1", 260, 210, 0, 0);
        gui_text_set(segment_text, "segment", GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255),
                     strlen("segment"), 36);
        gui_text_type_set(segment_text, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(segment_text, LEFT);
    }
    if (index == 2)
    {
        gui_img_t *workout_start_ctr_disconnect = gui_img_create_from_mem(obj, 0,
                                                                          WORKOUT_START_CTR_ICON_BG_A8_BIN, bg_ctr_offset_x, bg_ctr_offset_y, 0, 0);
        gui_img_a8_recolor(workout_start_ctr_disconnect, 0x47363E);
        gui_img_create_from_mem(workout_start_ctr_disconnect, 0, WORKOUT_START_DISCONNECT_ICON_BIN, 50, 20,
                                0, 0);
        gui_text_t *disconnect_text = gui_text_create(obj, "ac_text1", 60, 210, 0, 0);
        gui_text_set(disconnect_text, "disconnect", GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255),
                     strlen("disconnect"), 36);
        gui_text_type_set(disconnect_text, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(disconnect_text, LEFT);

        gui_img_t *workout_start_ctr_sfaty = gui_img_create_from_mem(obj, 0,
                                                                     WORKOUT_START_CTR_ICON_BG_A8_BIN, bg_ctr_offset_x + 200, bg_ctr_offset_y, 0, 0);
        gui_img_a8_recolor(workout_start_ctr_sfaty, 0x463412);
        gui_img_create_from_mem(workout_start_ctr_sfaty, 0, WORKOUT_SAFETY_CONFIRM_BIN, 50, 25, 0, 0);
        gui_text_t *sfaty_text = gui_text_create(obj, "ac_text1", 280, 210, 0, 0);
        gui_text_set(sfaty_text, "sfaty", GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255),
                     strlen("sfaty"), 36);
        gui_text_type_set(sfaty_text, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(sfaty_text, LEFT);
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
static void workout_start_ctr_design(gui_view_t *view)
{
    current_view = view;
    gui_obj_t *parent = GUI_BASE(view);

    gui_view_switch_on_event(current_view, "watchface_view",
                             SWITCH_OUT_ANIMATION_FADE,
                             SWITCH_IN_ANIMATION_FADE,
                             GUI_EVENT_KB_SHORT_CLICKED);

    {
        workout_start_ctr_remind = gui_win_create(current_view, "workout_start_ctr_remind", 0, 0,
                                                  SCREEN_WIDTH,
                                                  200);
        if (g_selected_workout_index > 0 && g_selected_workout_index < WORKOUT_TEXT_ARRAY_SIZE)
        {
            gui_text_t *sport_text = gui_text_create(current_view, "ac_text1", - 40,
                                                     60, 0, 0);
            gui_text_set(sport_text, workout_text_array[g_selected_workout_index], GUI_FONT_SRC_TTF,
                         gui_rgb(217, 255, 0),
                         strlen(workout_text_array[g_selected_workout_index]), 40);
            gui_text_type_set(sport_text, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
            gui_text_mode_set(sport_text, RIGHT);
        }

        //------time-----------------//
        gui_text_t *t_time = gui_text_create(workout_start_ctr_remind, 0, 0, 0, 370, 80);
        gui_text_set(t_time, time_str, GUI_FONT_SRC_TTF, gui_rgb(0xFF, 0xFF, 0xFF),
                     strlen(time_str), 34);
        gui_text_type_set(t_time, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(t_time, MID_RIGHT);
        gui_text_rendermode_set(t_time, 2);
        gui_obj_create_timer((void *)t_time, 30000, -1, time_update_cb);

        int space = 30;
        int length = 120;
        list = gui_list_create(parent, "list", 0, 0, 0, 0, length, space, VERTICAL, note_design, NULL,
                               false);
        gui_list_set_style(list, LIST_CLASSIC);
        gui_list_set_note_num(list, 3);
        gui_list_set_offset(list, list_offset_his);
        gui_list_set_out_scope(list, 80);
        gui_obj_create_timer(GUI_BASE(list), 10, true, list_timer_cb);
        gui_img_t *scrollbar_bg = gui_img_create_from_mem(parent, 0, SCROLLBAR_BG_BIN, 395, 80, 0, 0);
        gui_img_set_mode(scrollbar_bg, IMG_SRC_OVER_MODE);
        gui_img_t *scrollbar = gui_img_create_from_mem(parent, 0, SCROLLBAR_L_BIN, 394, 80, 0, 0);
        gui_img_set_mode(scrollbar, IMG_SRC_OVER_MODE);
    }

}
