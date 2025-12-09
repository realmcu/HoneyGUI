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
#define CURRENT_VIEW_NAME "workout_start_view"
#define SCREEN_WIDTH (int16_t)gui_get_screen_width()
#define SCREEN_HEIGHT (int16_t)gui_get_screen_height()
/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void workout_start_design(gui_view_t *view);

extern int16_t g_selected_workout_index;

/*============================================================================*
 *                           GUI_VIEW_INSTANCE
 *============================================================================*/
GUI_VIEW_INSTANCE(CURRENT_VIEW_NAME, false, workout_start_design, NULL);

/*============================================================================*
 *                            Variables
 *============================================================================*/
static gui_list_t *list = NULL;
static gui_view_t *current_view = NULL;
static gui_lite_arc_t *countdown_arc = NULL;
static gui_text_t *countdown_text = NULL;
static bool is_countdown_finished = false;
static int countdown_value = 3;
static char countdown_str[4] = "3";

static gui_win_t *workout_start_remind = NULL;
static gui_img_t *workout_start_item = NULL;
// example: example_workout_time_text
static gui_text_t *workout_time_text = NULL;
uint32_t workout_milliseconds = 0;
static char workout_time_str[20] = "00:00:00";


static char calories_str[20] = "0";
static char heart_rate_str[20] = "0";
static char distance_str[20] = "0.00";

static int8_t page_index = 0;
bool is_workout_stopped = false;
uint32_t stopped_workout_milliseconds = 0;
/*============================================================================*
 *                           Private Functions
 *============================================================================*/

static void countdown_timer_cb(void *obj)
{
    GUI_UNUSED(obj);

    if (is_countdown_finished)
    {
        return;
    }

    if (countdown_value > 0)
    {
        countdown_value--;

        if (countdown_value > 0)
        {
            sprintf(countdown_str, "%d", countdown_value);
            gui_text_content_set(countdown_text, countdown_str, strlen(countdown_str));

            float remaining = (float)countdown_value / 3.0f;
            int end_angle = 270 + (int)(360.0f * remaining);
            gui_lite_arc_set_end_angle(countdown_arc, end_angle);
        }
        else
        {
            is_countdown_finished = true;

            gui_obj_show(GUI_BASE(countdown_arc), false);
            gui_obj_show(GUI_BASE(countdown_text), false);
            gui_obj_show(GUI_BASE(workout_start_remind), true);

            if (workout_time_text)
            {
                gui_obj_show(GUI_BASE(workout_time_text), true);
            }

            workout_milliseconds = 0;
        }
    }
}
static void time_update_cb(void *obj)
{
    gui_text_content_set(obj, time_str, strlen(time_str));
}

static void workout_time_update_cb(void *obj)
{
    GUI_UNUSED(obj);

    if (!is_countdown_finished)
    {
        return;
    }

    workout_milliseconds += 10;

    uint32_t minutes = workout_milliseconds / 60000;
    uint32_t seconds = (workout_milliseconds % 60000) / 1000;
    uint32_t centiseconds = (workout_milliseconds % 1000) / 10;

    sprintf(workout_time_str, "%02u:%02u:%02u", minutes, seconds, centiseconds);

    if (workout_time_text)
    {
        gui_text_content_set(workout_time_text, workout_time_str, strlen(workout_time_str));
    }
}

typedef enum
{
    WORKOUT_TYPE_NONE = 0,
    WORKOUT_TYPE_OUTDOOR_WALK,
    WORKOUT_TYPE_OUTDOOR_RUN,
    WORKOUT_TYPE_BADMINTON,
    WORKOUT_TYPE_BASKETBALL,
    WORKOUT_TYPE_BICYCLE,
    WORKOUT_TYPE_INDOOR_RUN,
    WORKOUT_TYPE_INDOOR_WALK,
    WORKOUT_TYPE_JUMP_ROPE,
    WORKOUT_TYPE_ON_FOOT,
    WORKOUT_TYPE_OUTDOOR_CLIMB,
    WORKOUT_TYPE_SIT_UP,
    WORKOUT_TYPE_SWIM,
    WORKOUT_TYPE_TAEKWONDO,
    WORKOUT_TYPE_WEIGHTLIFT,
    WORKOUT_TYPE_YOGA,
    WORKOUT_TYPE_DANCE,
    WORKOUT_TYPE_STRETCH
} workout_type_t;

typedef struct
{
    uint32_t calories;
    uint32_t heart_rate;
    float distance;
    uint32_t steps;
    uint32_t duration;
} workout_data_t;

static workout_data_t current_workout_data =
{
    .calories = 0,
    .heart_rate = 0,
    .distance = 0.0,
    .steps = 0,
    .duration = 0
};
static bool workout_has_distance(int16_t workout_type)
{
    switch (workout_type)
    {
    case WORKOUT_TYPE_OUTDOOR_WALK:
    case WORKOUT_TYPE_OUTDOOR_RUN:
    case WORKOUT_TYPE_BICYCLE:
    case WORKOUT_TYPE_INDOOR_RUN:
    case WORKOUT_TYPE_INDOOR_WALK:
    case WORKOUT_TYPE_ON_FOOT:
    case WORKOUT_TYPE_OUTDOOR_CLIMB:
        return true;
    default:
        return false;
    }
}
static bool workout_has_steps(int16_t workout_type)
{
    switch (workout_type)
    {
    case WORKOUT_TYPE_OUTDOOR_WALK:
    case WORKOUT_TYPE_OUTDOOR_RUN:
    case WORKOUT_TYPE_INDOOR_WALK:
    case WORKOUT_TYPE_INDOOR_RUN:
    case WORKOUT_TYPE_ON_FOOT:
    case WORKOUT_TYPE_JUMP_ROPE:
        return true;
    default:
        return false;
    }
}

// simulate workout data update callback
static void update_workout_data_cb(void *obj)
{
    GUI_UNUSED(obj);

    if (!is_countdown_finished) { return; }

    static uint32_t update_counter = 0;
    update_counter++;

    current_workout_data.duration = workout_milliseconds / 1000;

    if (update_counter % 10 == 0)
    {
        switch (g_selected_workout_index)
        {
        case WORKOUT_TYPE_OUTDOOR_WALK:
        case WORKOUT_TYPE_INDOOR_WALK:
            current_workout_data.calories += 1;
            current_workout_data.heart_rate = 90 + (rand() % 20);
            current_workout_data.distance += 0.02f;
            current_workout_data.steps += 2;
            break;

        case WORKOUT_TYPE_OUTDOOR_RUN:
        case WORKOUT_TYPE_INDOOR_RUN:
            current_workout_data.calories += 2;
            current_workout_data.heart_rate = 120 + (rand() % 40);
            current_workout_data.distance += 0.05f;
            current_workout_data.steps += 3;
            break;

        case WORKOUT_TYPE_BICYCLE:
            current_workout_data.calories += 2;
            current_workout_data.heart_rate = 100 + (rand() % 30);
            current_workout_data.distance += 0.1f;
            current_workout_data.steps = 0;
            break;

        case WORKOUT_TYPE_YOGA:
        case WORKOUT_TYPE_STRETCH:
            current_workout_data.calories += 1;
            current_workout_data.heart_rate = 75 + (rand() % 15);
            current_workout_data.distance = 0.0f;
            current_workout_data.steps = 0;
            break;

        case WORKOUT_TYPE_JUMP_ROPE:
            current_workout_data.calories += 3;
            current_workout_data.heart_rate = 110 + (rand() % 35);
            current_workout_data.distance = 0.0f;
            current_workout_data.steps += 5;
            break;

        default:
            current_workout_data.calories += 1;
            current_workout_data.heart_rate = 85 + (rand() % 25);
            current_workout_data.distance += 0.02f;
            current_workout_data.steps += 2;
            break;
        }

        if (current_workout_data.heart_rate > 200) { current_workout_data.heart_rate = 200; }
    }
}
static void update_data_display_cb(void *obj)
{
    GUI_UNUSED(obj);
    gui_obj_t *data_container = (gui_obj_t *)obj;
    if (!data_container) { return; }

    GUI_WIDGET_POINTER_BY_NAME_ROOT(calories_text, "calories_value", data_container);
    if (calories_text)
    {
        sprintf(calories_str, "%u", current_workout_data.calories);
        gui_text_content_set((gui_text_t *)calories_text, calories_str, strlen(calories_str));
    }

    GUI_WIDGET_POINTER_BY_NAME_ROOT(heart_text, "heart_value", data_container);
    if (heart_text)
    {
        sprintf(heart_rate_str, "%u", current_workout_data.heart_rate);
        gui_text_content_set((gui_text_t *)heart_text, heart_rate_str, strlen(heart_rate_str));
    }

    if (workout_has_distance(g_selected_workout_index))
    {
        GUI_WIDGET_POINTER_BY_NAME_ROOT(dist_text, "distance_value", data_container);
        if (dist_text)
        {
            sprintf(distance_str, "%.2f", (double)current_workout_data.distance);
            gui_text_content_set((gui_text_t *)dist_text, distance_str, strlen(distance_str));
        }
    }
}

static void page_0_design(gui_obj_t *parent)
{
    gui_win_t *data_container = gui_win_create(parent, "workout_data_container",
                                               0, 200, SCREEN_WIDTH, SCREEN_HEIGHT - 200);

    int y_pos = 20;
    int line_height = 60;
    uint16_t text_unit_offset_x = 200;

    // calories value
    gui_text_t *calories_value = gui_text_create(data_container, "calories_value",
                                                 30, y_pos, 0, 0);
    sprintf(calories_str, "0");
    gui_text_set(calories_value, calories_str, GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255),
                 strlen(calories_str), 85);
    gui_text_type_set(calories_value, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(calories_value, LEFT);
    gui_text_rendermode_set(calories_value, 2);

    gui_text_t *calories_unit = gui_text_create(data_container, "calories_unit",
                                                text_unit_offset_x, y_pos, 0, 0);
    gui_text_set(calories_unit, "kcal", GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255),
                 strlen("kcal"), 80);
    gui_text_type_set(calories_unit, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(calories_unit, LEFT);
    gui_text_rendermode_set(calories_unit, 2);

    y_pos += line_height;

    // heart value
    gui_text_t *heart_value = gui_text_create(data_container, "heart_value",
                                              30, y_pos, 0, 0);
    sprintf(heart_rate_str, "0");
    gui_text_set(heart_value, heart_rate_str, GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255),
                 strlen(heart_rate_str), 85);
    gui_text_type_set(heart_value, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(heart_value, LEFT);
    gui_text_rendermode_set(heart_value, 2);

    // unit
    gui_text_t *heart_unit = gui_text_create(data_container, "heart_unit",
                                             text_unit_offset_x, y_pos, 0, 0);
    gui_text_set(heart_unit, "BPM", GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255),
                 strlen("BPM"), 80);
    gui_text_type_set(heart_unit, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(heart_unit, LEFT);
    gui_text_rendermode_set(heart_unit, 2);

    y_pos += line_height;

    if (workout_has_distance(g_selected_workout_index))
    {
        // value
        gui_text_t *distance_value = gui_text_create(data_container, "distance_value",
                                                     30, y_pos, 0, 0);
        sprintf(distance_str, "0.00");
        gui_text_set(distance_value, distance_str, GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255),
                     strlen(distance_str), 85);
        gui_text_type_set(distance_value, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(distance_value, LEFT);
        gui_text_rendermode_set(distance_value, 2);

        // unit
        gui_text_t *distance_unit = gui_text_create(data_container, "distance_unit",
                                                    text_unit_offset_x, y_pos, 0, 0);
        gui_text_set(distance_unit, "km", GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255),
                     strlen("km"), 80);
        gui_text_type_set(distance_unit, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(distance_unit, LEFT);
        gui_text_rendermode_set(distance_unit, 2);
    }

    gui_obj_create_timer(GUI_BASE(data_container), 100, true, update_data_display_cb);
}
static void note_design(gui_obj_t *obj, void *p)
{
    (void)p;
    GUI_UNUSED(obj);
    uint16_t index = ((gui_list_note_t *)obj)->index;
    switch (index)
    {
    case 0:
        page_0_design(obj);
        break;
    case 1:
        // page_0_design(obj);
        break;
    default:
        break;
    }

}

static void workout_start_design(gui_view_t *view)
{
    current_view = view;
    gui_obj_t *parent = GUI_BASE(view);
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);

    // reset static variables for countdown
    countdown_arc = NULL;
    countdown_text = NULL;
    list = NULL;
    workout_time_text = NULL;

    if (!is_workout_stopped && stopped_workout_milliseconds == 0)
    {
        countdown_value = 3;
        is_countdown_finished = false;
        workout_milliseconds = 0;
        sprintf(workout_time_str, "00:00:00");

        // reset workout data
        current_workout_data.calories = 0;
        current_workout_data.heart_rate = 0;
        current_workout_data.distance = 0.0f;
        current_workout_data.steps = 0;
        current_workout_data.duration = 0;
    }
    else if (!is_workout_stopped && stopped_workout_milliseconds > 0)
    {
        is_countdown_finished = true;
        workout_milliseconds = stopped_workout_milliseconds;
        stopped_workout_milliseconds = 0;

        uint32_t minutes = workout_milliseconds / 60000;
        uint32_t seconds = (workout_milliseconds % 60000) / 1000;
        uint32_t centiseconds = (workout_milliseconds % 1000) / 10;
        sprintf(workout_time_str, "%02u:%02u:%02u", minutes, seconds, centiseconds);
    }
    else if (is_workout_stopped)
    {
        is_countdown_finished = true;
        workout_milliseconds = stopped_workout_milliseconds;

        uint32_t minutes = workout_milliseconds / 60000;
        uint32_t seconds = (workout_milliseconds % 60000) / 1000;
        uint32_t centiseconds = (workout_milliseconds % 1000) / 10;
        sprintf(workout_time_str, "%02u:%02u:%02u", minutes, seconds, centiseconds);
    }

    // gui_view_switch_on_event(current_view, "workout_start_meida_view",
    //                          SWITCH_OUT_TO_RIGHT_USE_TRANSLATION,
    //                          SWITCH_IN_FROM_LEFT_USE_TRANSLATION,
    //                          GUI_EVENT_TOUCH_MOVE_LEFT);
    gui_view_switch_on_event(current_view, "workout_start_ctr_view",
                             SWITCH_OUT_TO_LEFT_USE_TRANSLATION,
                             SWITCH_IN_FROM_RIGHT_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_RIGHT);

    gui_view_switch_on_event(current_view, "watchface_view",
                             SWITCH_OUT_ANIMATION_FADE,
                             SWITCH_IN_ANIMATION_FADE,
                             GUI_EVENT_KB_SHORT_CLICKED);
    // create countdown arc(initially full circle)
    if (!is_countdown_finished)
    {
        countdown_arc = gui_lite_arc_create(parent, "countdown_arc",
                                            SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2,
                                            150, 270, 630, 20, gui_rgb(185, 251, 79));

        countdown_text = gui_text_create(parent, "countdown_text",
                                         SCREEN_WIDTH / 2 - 60, SCREEN_HEIGHT / 2 - 55,
                                         0, 0);
        sprintf(countdown_str, "%d", countdown_value);
        gui_text_set(countdown_text, countdown_str, GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255),
                     strlen(countdown_str), 110);
        gui_text_type_set(countdown_text, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(countdown_text, LEFT);
    }

    {
        workout_start_remind = gui_win_create(parent, "workout_start_remind", 0, 0, SCREEN_WIDTH, 200);

        // exercise icon array
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

        uint8_t offset_icon = 18;
        gui_img_create_from_mem(workout_start_remind, "workout_icon_bg",
                                WORKOUT_LEFT_CONTROL_BIN, 30, 20, 0, 0);

        // show selected exercise icon
        int16_t array_size = (int16_t)(sizeof(img_data_array) / sizeof(img_data_array[0]));
        if (g_selected_workout_index > 0 && g_selected_workout_index < array_size)
        {
            workout_start_item = gui_img_create_from_mem(workout_start_remind, "workout_icon",
                                                         img_data_array[g_selected_workout_index],
                                                         30 + offset_icon, 20 + offset_icon, 0, 0);
            gui_img_scale(workout_start_item, 0.3, 0.3);
        }

        // create exercise time text
        workout_time_text = gui_text_create(workout_start_remind, "workout_time_text",
                                            30, 90,
                                            0, 0);
        gui_text_set(workout_time_text, workout_time_str, GUI_FONT_SRC_TTF, gui_rgb(255, 233, 0),
                     strlen(workout_time_str), 85);
        gui_text_type_set(workout_time_text, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(workout_time_text, LEFT);

        //------time-----------------//
        gui_text_t *t_time = gui_text_create(workout_start_remind, 0, 0, 0, 370, 80);
        gui_text_set(t_time, time_str, GUI_FONT_SRC_TTF, gui_rgb(0xFF, 0xFF, 0xFF),
                     strlen(time_str), 34);
        gui_text_type_set(t_time, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(t_time, MID_RIGHT);
        gui_text_rendermode_set(t_time, 2);
        gui_obj_create_timer((void *)t_time, 30000, -1, time_update_cb);

        if (!is_workout_stopped)
        {
            // exercise time update timer(10ms)
            gui_obj_create_timer(GUI_BASE(workout_time_text), 10, true, workout_time_update_cb);
            // create workout data update timer(100ms)
            gui_obj_create_timer(GUI_BASE(workout_start_remind), 100, true, update_workout_data_cb);
        }

        //
        list = gui_list_create(workout_start_remind, 0, 0, 0, 0, 0, SCREEN_HEIGHT, 0,
                               VERTICAL, note_design, NULL, 0);
        gui_list_set_style(list, LIST_FADE);
        gui_list_set_note_num(list, 2);
        gui_list_set_inertia(list, false);
        gui_list_set_auto_align(list, true);
        gui_list_set_factor(list, 0.1f);
        if (!gui_view_get_current())
        {
            gui_list_set_offset(list, -(page_index * SCREEN_HEIGHT));
        }

        if (is_countdown_finished)
        {
            gui_obj_show(GUI_BASE(workout_start_remind), true);
        }
        else
        {
            gui_obj_show(GUI_BASE(workout_start_remind), false);
            gui_obj_create_timer(parent, 1000, true, countdown_timer_cb);
        }
    }
}