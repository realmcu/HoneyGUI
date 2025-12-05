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
#include "gui_lite_geometry_arc.h"
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
static gui_view_t *current_view = NULL;


static gui_win_t *workout_start_ctr_remind = NULL;
// static gui_img_t *workout_start_ctr_item = NULL;
extern char *workout_text_array[];
extern const int WORKOUT_TEXT_ARRAY_SIZE;
/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static void time_update_cb(void *obj)
{
    gui_text_content_set(obj, time_str, strlen(time_str));
}

static void note_design(gui_obj_t *obj, void *p)
{
    (void)p;
    GUI_UNUSED(obj);
    uint16_t index = ((gui_list_note_t *)obj)->index;
    switch (index)
    {
    case 0:
        break;
    case 1:
        break;
    default:
        break;
    }

}

static void workout_start_ctr_design(gui_view_t *view)
{
    current_view = view;
    gui_obj_t *parent = GUI_BASE(view);
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);

    gui_view_switch_on_event(current_view, "watchface_view",
                             SWITCH_OUT_ANIMATION_FADE,
                             SWITCH_IN_ANIMATION_FADE,
                             GUI_EVENT_KB_SHORT_CLICKED);

    {
        workout_start_ctr_remind = gui_win_create(parent, "workout_start_ctr_remind", 0, 0, SCREEN_WIDTH,
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

        int space = 5;
        int length = 115;
        list = gui_list_create(parent, "list", 0, 0, 0, 0, length, space, VERTICAL, note_design, NULL,
                               false);
        gui_list_set_style(list, LIST_CLASSIC);
        gui_list_set_note_num(list, 1);
        // gui_list_set_offset(list, list_offset_his);
        gui_list_set_out_scope(list, 80);
    }

}