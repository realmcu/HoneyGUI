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
#include "gui_rect.h"
/*============================================================================*
 *                           Types
 *============================================================================*/
typedef struct note_design_param
{
    // void **click_cb;
    uint32_t **img_data_array;
} note_design_param_t;
/*============================================================================*
 *                            Macros
 *============================================================================*/
#define SCREEN_WIDTH (int16_t)gui_get_screen_width()
#define SCREEN_HEIGHT (int16_t)gui_get_screen_height()
#define CURRENT_VIEW_NAME "workout_view"

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void workout_view_design(gui_view_t *view);

/*============================================================================*
 *                            Variables
 *============================================================================*/
static gui_list_t *list;
static int16_t list_offset_his = 0;
static gui_view_t *current_view = NULL;
static note_design_param_t *design_p = NULL;
static gui_win_t *workout_ctr_win = NULL;
static bool is_workout_started = false;
static int16_t selected_workout_index = 0;

int16_t g_selected_workout_index = 0;
/*============================================================================*
 *                           Private Functions
 *============================================================================*/
GUI_VIEW_INSTANCE(CURRENT_VIEW_NAME, false, workout_view_design, NULL);
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
static void workout_target_icon_press_cb(void *p)
{
    GUI_UNUSED(p);
    // gui_view_switch_direct(current_view, "workout_target_view",
    //                        SWITCH_IN_ANIMATION_FADE, SWITCH_OUT_ANIMATION_FADE);
}
static void back_to_list_view(void)
{
    gui_log("back_to_list_view \n");
    is_workout_started = false;
    gui_obj_show(GUI_BASE(list), true);
    gui_obj_show(GUI_BASE(workout_ctr_win), false);

    GUI_WIDGET_POINTER_BY_NAME_ROOT(scrollbar_bg, "scrollbar_bg", current_view);
    GUI_WIDGET_POINTER_BY_NAME_ROOT(scrollbar, "scrollbar", current_view);
    if (scrollbar_bg) { gui_obj_show(scrollbar_bg, true); }
    if (scrollbar) { gui_obj_show(scrollbar, true); }

    GUI_WIDGET_POINTER_BY_NAME_ROOT(selected_icon, "selected_workout_icon", current_view);
    GUI_WIDGET_POINTER_BY_NAME_ROOT(selected_text, "selected_workout_text", current_view);
    if (selected_icon) { gui_obj_show(selected_icon, false); }
    if (selected_text) { gui_obj_show(selected_text, false); }
}

static void workout_ctr_win_slide_cb(void *obj, gui_event_t e)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    if (is_workout_started)
    {
        back_to_list_view();
    }
}

static void workout_view_icon_press_cb(void *p)
{
    GUI_UNUSED(p);
}
static void workout_media_press_cb(void *p)
{
    GUI_UNUSED(p);
}
static void workout_remind_press_cb(void *p)
{
    GUI_UNUSED(p);
}
static void workout_start_press_cb(void *p)
{
    GUI_UNUSED(p);
    g_selected_workout_index = selected_workout_index;
    gui_view_switch_direct(current_view, "workout_start_view",
                           SWITCH_IN_ANIMATION_FADE, SWITCH_OUT_ANIMATION_FADE);
}
char *workout_text_array[] =
{
    "",                      // index 0: none
    "outdoor walk",          // index 1
    "outdoor run",           // index 2
    "badminton",             // index 3
    "basketball",            // index 4
    "bicycle",               // index 5
    "indoor run",            // index 6
    "indoor walk",           // index 7
    "jump rope",             // index 8
    "on foot",               // index 9
    "outdoor climb",         // index 10
    "sit up",                // index 11
    "swim",                  // index 12
    "taekwondo",             // index 13
    "weightlifting",         // index 14
    "yoga",                  // index 15
    "dance",                 // index 16
    "stretch",               // index 17
};
const int WORKOUT_TEXT_ARRAY_SIZE = sizeof(workout_text_array) / sizeof(workout_text_array[0]);
static void note_click_cb(void *obj, gui_event_t e)
{
    GUI_UNUSED(e);
    gui_list_note_t *note = (gui_list_note_t *)obj;
    selected_workout_index = note->index;

    if (selected_workout_index > 0 && selected_workout_index < WORKOUT_TEXT_ARRAY_SIZE)
    {
        is_workout_started = true;
        gui_obj_show(GUI_BASE(list), false);

        GUI_WIDGET_POINTER_BY_NAME_ROOT(scrollbar_bg, "scrollbar_bg", current_view);
        GUI_WIDGET_POINTER_BY_NAME_ROOT(scrollbar, "scrollbar", current_view);
        if (scrollbar_bg) { gui_obj_show(scrollbar_bg, false); }
        if (scrollbar) { gui_obj_show(scrollbar, false); }
        GUI_WIDGET_POINTER_BY_NAME_ROOT(selected_icon, "selected_workout_icon", current_view);
        GUI_WIDGET_POINTER_BY_NAME_ROOT(selected_text, "selected_workout_text", current_view);
        if (selected_icon)
        {
            gui_img_set_image_data((gui_img_t *)selected_icon,
                                   (const uint8_t *)design_p->img_data_array[selected_workout_index]);
            gui_obj_show(selected_icon, true);
        }
        if (selected_text)
        {
            gui_text_content_set((gui_text_t *)selected_text,
                                 (char *)workout_text_array[selected_workout_index],
                                 strlen(workout_text_array[selected_workout_index]));
            gui_obj_show(selected_text, true);
        }

        gui_obj_show(GUI_BASE(workout_ctr_win), true);
    }
}

static void note_design(gui_obj_t *obj, void *p)
{
    (void)obj;
    note_design_param_t *design_p = (note_design_param_t *)p;
    gui_list_note_t *note = (gui_list_note_t *)obj;
    uint16_t index = note->index;
    int16_t offset_x_icon = 130;
    int16_t offset_y_icon = -100;
    int16_t offset_x_text = 0;
    int16_t offset_y_text = -30;

    if (index == 0)
    {

    }
    else if (index < sizeof(workout_text_array) / sizeof(workout_text_array[0]))
    {
        gui_img_create_from_mem(note, 0, design_p->img_data_array[index], offset_x_icon,
                                offset_y_icon, 0, 0);
        char *text = (char *)workout_text_array[index];
        gui_text_t *t = gui_text_create(obj, 0, offset_x_text, offset_y_text, 0, 0);
        gui_text_set(t, text, GUI_FONT_SRC_TTF, gui_rgba(255, 255, 255, 255),
                     strlen(text), 36);
        gui_text_type_set(t, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, MID_CENTER);

        gui_obj_add_event_cb(obj, (gui_event_cb_t)note_click_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
    }
}
static void list_timer_cb(void *obj)
{
    gui_list_t *list = (gui_list_t *)obj;
    gui_obj_t *scrollbar_bg = gui_list_entry(GUI_BASE(obj)->brother_list.next, gui_obj_t,
                                             brother_list);
    gui_obj_t *scrollbar = gui_list_entry(scrollbar_bg->brother_list.next, gui_obj_t,
                                          brother_list);
    static uint8_t cnt = 0;

    if (is_workout_started)
    {
        return;
    }

    if (list_offset_his != list->offset)
    {
        list_offset_his = list->offset;
        int range = list->base.h - list->total_length;
        int t_y = list_offset_his * (SCROLLBAR_BG_H - SCROLLBAR_S_H) / range;
        gui_img_translate((void *)scrollbar, 0, t_y);
        gui_obj_hidden(scrollbar_bg, false);
        gui_obj_hidden(scrollbar, false);
        cnt = 0;
    }
    else
    {
        cnt++;
        if (cnt >= 50) //1s
        {
            cnt = 0;
            gui_obj_hidden(scrollbar_bg, true);
            gui_obj_hidden(scrollbar, true);
        }
    }

    // update note opacity
    int screen_center_y = SCREEN_HEIGHT / 2;

    gui_node_list_t *node;
    gui_list_for_each(node, &GUI_BASE(list)->child_list)
    {
        gui_obj_t *note = gui_list_entry(node, gui_obj_t, brother_list);

        if (note == NULL || note == GUI_BASE(list))
        {
            continue;
        }

        int note_center_y = note->y + note->h / 4;
        int distance = abs(note_center_y - screen_center_y);

        // distance to screen center
        int max_distance = SCREEN_HEIGHT / 2;
        int opacity = 255;
        if (distance > 0)
        {
            opacity = 255 - (distance * 180 / max_distance);
            if (opacity < 75) { opacity = 75; } // low opacity
        }

        // set note opacity
        gui_node_list_t *child_node;
        gui_list_for_each(child_node, &note->child_list)
        {
            gui_obj_t *child = gui_list_entry(child_node, gui_obj_t, brother_list);
            if (child != NULL)
            {
                if (child->type == IMAGE_FROM_MEM)
                {
                    gui_img_set_opacity((gui_img_t *)child, (unsigned char)opacity);
                }
                else if (child->type == TEXTBOX)
                {
                    gui_text_color_set((gui_text_t *)child, gui_rgba(255, 255, 255, (unsigned char)opacity));
                }
            }
        }
    }
}
static void workout_view_design(gui_view_t *view)
{
    current_view = view;

    is_workout_started = false;
    selected_workout_index = 0;

    const char *obj_name = gui_view_get_current()->descriptor->name;
    VIEW_SWITCH_STYLE swtich_in = SWITCH_IN_ANIMATION_FADE;
    VIEW_SWITCH_STYLE swtich_out = SWITCH_OUT_ANIMATION_FADE;
    if (strcmp(obj_name, "bottom_view") == 0)
    {
        gui_view_set_animate_step(current_view, 60);
        swtich_in = SWITCH_IN_FROM_TOP_USE_TRANSLATION;
        swtich_out = SWITCH_OUT_TO_BOTTOM_USE_TRANSLATION;
        gui_view_switch_on_event(current_view, "bottom_view",
                                 swtich_out,
                                 swtich_in,
                                 GUI_EVENT_KB_SHORT_CLICKED);
    }
    else if (strcmp(obj_name, "menu_view") == 0)
    {
        gui_view_switch_on_event(current_view, "menu_view",
                                 swtich_out,
                                 swtich_in,
                                 GUI_EVENT_KB_SHORT_CLICKED);
    }
    else
    {
        gui_view_switch_on_event(current_view, "watchface_view",
                                 swtich_out,
                                 swtich_in,
                                 GUI_EVENT_KB_SHORT_CLICKED);
    }
    gui_obj_t *parent = GUI_BASE(view);
    gui_rect_create(parent, "note", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, gui_rgba(32, 36,
                                                                                   17, 255));


    int space = 40;
    int length = 180;
    uint32_t *img_data_array[] =
    {
        0,
        WORKOUT_OUTDOOR_WALK_BIN,
        WORKOUT_OUTDOOR_RUN_ICON_BIN,
        WORKOUT_BADMINTON_ICON_BIN,
        WORKOUT_BASKETBALL_ICON_BIN,
        WORKOUT_BICYCLE_ICON_BIN,
        WORKOUT_INDOOR_RUN_ICON_BIN,
        WORKOUT_INDOOR_WALK_BIN,
        WORKOUT_JUMP_ROPE_ICON_BIN,
        WORKOUT_ON_FOOT_ICON_BIN,
        WORKOUT_OUTDOOR_CLIMB_ICON_BIN,
        WORKOUT_SIT_UP_ICON_BIN,
        WORKOUT_SWIM_ICON_BIN,
        WORKOUT_TAEKWONDO_ICON_BIN,
        WORKOUT_WEIGHTLIFTE_ICON_BIN,
        WORKOUT_YOGA_ICON_BIN,
        WORKOUT_DANCE_ICON_BIN,
        WORKOUT_STRETCH_ICON_BIN,
    };
    int array_size = sizeof(img_data_array) / sizeof(img_data_array[0]);

    design_p = gui_malloc(sizeof(note_design_param_t));
    uint32_t **data_array = gui_malloc(array_size * sizeof(uint32_t *));
    memcpy(data_array, img_data_array, array_size * sizeof(void *));
    design_p->img_data_array = data_array;
    list = gui_list_create(parent, "list", 0, 0, 0, 0, length, space, VERTICAL, note_design, design_p,
                           false);
    gui_list_set_style(list, LIST_CLASSIC);
    gui_list_set_note_num(list, array_size);
    gui_list_set_offset(list, list_offset_his);
    gui_list_set_out_scope(list, 80);
    gui_obj_create_timer(GUI_BASE(list), 10, true, list_timer_cb);
    gui_img_t *scrollbar_bg = gui_img_create_from_mem(parent, "scrollbar_bg", SCROLLBAR_BG_BIN, 395, 80,
                                                      0, 0);
    gui_img_set_mode(scrollbar_bg, IMG_SRC_OVER_MODE);
    gui_img_t *scrollbar = gui_img_create_from_mem(parent, "scrollbar", SCROLLBAR_S_BIN, 394, 80, 0, 0);

    gui_obj_hidden((void *)scrollbar_bg, true);
    gui_obj_hidden((void *)scrollbar, true);

    // create selected workout icon and init not show
    gui_img_t *selected_icon = gui_img_create_from_mem(parent, "selected_workout_icon",
                                                       WORKOUT_OUTDOOR_WALK_BIN,
                                                       130, SCREEN_HEIGHT / 2 - 100, 0, 0);
    gui_obj_show(GUI_BASE(selected_icon), false);

    gui_text_t *selected_text = gui_text_create(parent, "selected_workout_text", 0,
                                                SCREEN_HEIGHT / 2 + 30,
                                                SCREEN_WIDTH, 50);
    gui_text_set(selected_text, "", GUI_FONT_SRC_TTF, gui_rgba(255, 255, 255, 255), 0, 36);
    gui_text_type_set(selected_text, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(selected_text, MID_CENTER);
    gui_obj_show(GUI_BASE(selected_text), false);

    //-------icon--------//
    workout_ctr_win = gui_win_create(parent, "workout_ctr_win", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    // gui_obj_add_event_cb(GUI_BASE(workout_ctr_win), (gui_event_cb_t)workout_ctr_win_slide_cb,
    //                      GUI_EVENT_TOUCH_MOVE_UP, NULL);
    // gui_obj_add_event_cb(GUI_BASE(workout_ctr_win), (gui_event_cb_t)workout_ctr_win_slide_cb,
    //                      GUI_EVENT_TOUCH_MOVE_DOWN, NULL);
    uint8_t offset_icon = 18;
    gui_img_t *workout_view_icon_bg = gui_img_create_from_mem(workout_ctr_win, "workout_view_icon_bg",
                                                              WORKOUT_LEFT_CONTROL_BIN, 30, 30, 0, 0);
    gui_img_create_from_mem(workout_ctr_win, "workout_view_icon", WORKOUT_VIEW_ICON_BIN,
                            30 + offset_icon, 30 + offset_icon, 0, 0);
    gui_obj_add_event_cb(GUI_BASE(workout_view_icon_bg), (gui_event_cb_t)workout_view_icon_press_cb,
                         GUI_EVENT_TOUCH_CLICKED, NULL);

    gui_img_t *workout_target_icon_bg = gui_img_create_from_mem(workout_ctr_win,
                                                                "workout_target_icon_bg", WORKOUT_LEFT_CONTROL_BIN, 300, 30, 0, 0);
    gui_img_create_from_mem(workout_ctr_win, "workout_target_icon", WORKOUT_TARGET_ICON_BIN,
                            300 + offset_icon, 30 + offset_icon, 0, 0);
    gui_obj_add_event_cb(GUI_BASE(workout_target_icon_bg), (gui_event_cb_t)workout_target_icon_press_cb,
                         GUI_EVENT_TOUCH_CLICKED, NULL);

    gui_img_t *workout_media_icon_bg = gui_img_create_from_mem(workout_ctr_win, "workout_media_icon_bg",
                                                               WORKOUT_LEFT_CONTROL_BIN, 30, 400, 0, 0);
    gui_img_create_from_mem(workout_ctr_win, "workout_media_icon", WORKOUT_MEDIA_ICON_BIN,
                            30 + offset_icon, 400 + offset_icon, 0, 0);
    gui_obj_add_event_cb(GUI_BASE(workout_media_icon_bg), (gui_event_cb_t)workout_media_press_cb,
                         GUI_EVENT_TOUCH_CLICKED, NULL);

    gui_img_t *workout_remind_icon_bg = gui_img_create_from_mem(workout_ctr_win,
                                                                "workout_remind_icon_bg", WORKOUT_LEFT_CONTROL_BIN, 300, 400, 0, 0);
    gui_img_create_from_mem(workout_ctr_win, "workout_remind_icon", WORKOUT_REMIND_ICON_BIN,
                            300 + offset_icon, 400 + offset_icon, 0, 0);
    gui_obj_add_event_cb(GUI_BASE(workout_remind_icon_bg), (gui_event_cb_t)workout_remind_press_cb,
                         GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_img_t *workout_start_icon = gui_img_create_from_mem(workout_ctr_win,
                                                            "workout_start_icon", WORKOUT_START_ICON_BIN, 170, 400, 0, 0);
    gui_obj_add_event_cb(GUI_BASE(workout_start_icon), (gui_event_cb_t)workout_start_press_cb,
                         GUI_EVENT_TOUCH_CLICKED, NULL);

    gui_obj_show(GUI_BASE(workout_ctr_win), false);

    // ----------time-----------------//
    gui_text_t *t_time = gui_text_create(parent, "time_b", 0, 0, SCREEN_WIDTH, 80);
    gui_text_set(t_time, time_str, GUI_FONT_SRC_TTF, gui_rgba(255, 255, 255, 255),
                 strlen(time_str), 48);
    gui_text_type_set(t_time, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t_time, MID_CENTER);
    gui_text_rendermode_set(t_time, 2);



    gui_obj_create_timer(GUI_BASE(view), 30000, true, time_update_cb);
}


