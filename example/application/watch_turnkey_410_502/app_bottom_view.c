/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <time.h>
#include "cJSON.h"
#include "root_image/ui_resource.h"
#include "app_main_watch.h"
#include "gui_text.h"
#include "gui_win.h"
#include "gui_img.h"
#include "gui_list.h"
#include "gui_view_instance.h"
#include "gui_rect.h"
#include "gui_arc.h"
#include "gui_canvas.h"
/*============================================================================*
 *                            Macros
 *============================================================================*/
#define SCREEN_WIDTH (int16_t)gui_get_screen_width()
#define SCREEN_HEIGHT (int16_t)gui_get_screen_height()
#define CURRENT_VIEW_NAME "bottom_view"
#define __WIN1_NAME    "win_timecard"
#define __WIN0_NAME    "win_timecircle"

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void bottom_view_design(gui_view_t *view);
static void clear_bottom_view(gui_view_t *view);

/*============================================================================*
 *                           GUI_VIEW_INSTANCE
 *============================================================================*/
GUI_VIEW_INSTANCE(CURRENT_VIEW_NAME, false, bottom_view_design, clear_bottom_view);

/*============================================================================*
 *                            Variables
 *============================================================================*/
static gui_view_t *current_view = NULL;
const char *month[12] =
{
    "JAN",
    "FEB",
    "MAR",
    "APR",
    "MAY",
    "JUN",
    "JUL",
    "AUG",
    "SEP",
    "OCT",
    "NOV",
    "DEC"
};
const char *day[7] =
{
    "SUN",
    "MON",
    "TUE",
    "WED",
    "THU",
    "FRI",
    "SAT"
};


static char date_content[] = "WED\nNOV\n19";
static char date_timecard_content[10] = "Sun 0";

static uint8_t *img_data_activity = NULL;
static char *move_content = NULL;
static char *ex_content = NULL;
static char *stand_content = NULL;
uint8_t battery_level = 0;
static gui_list_t *list = NULL;
static gui_text_t *battery_text_widget = NULL;
char battery_content[4] = "100";
/*============================================================================*
 *                           Private Functions
 *============================================================================*/

static void clear_bottom_view(gui_view_t *view)
{
    (void)view;
    if (img_data_activity)
    {
        gui_lower_free(img_data_activity);
        img_data_activity = NULL;
    }
    if (move_content)
    {
        gui_free(move_content);
        gui_free(ex_content);
        gui_free(stand_content);
        move_content = NULL;
        ex_content = NULL;
        stand_content = NULL;
    }
    battery_text_widget = NULL;
}

static void time_update_cb(void)
{
    if (!timeinfo)
    {
        return;
    }
    else
    {
        {
            GUI_WIDGET_POINTER_BY_NAME_ROOT(obj, "time_s", current_view);
            gui_text_content_set((gui_text_t *)obj, time_str, strlen(time_str));
        }
        {
            GUI_WIDGET_POINTER_BY_NAME_ROOT(obj, "date_s", current_view);
            sprintf(date_timecard_content, "%s %d",  day[timeinfo->tm_wday], timeinfo->tm_mday);
            gui_text_content_set((gui_text_t *)obj, date_timecard_content, strlen(date_timecard_content));
        }
        {
            GUI_WIDGET_POINTER_BY_NAME_ROOT(obj, "date_b", current_view);
            sprintf(date_content, "%s\n%s\n%d", day[timeinfo->tm_wday], month[timeinfo->tm_mon],
                    timeinfo->tm_mday);
            gui_text_content_set((gui_text_t *)obj, date_content, strlen(date_content));
        }
        {
            GUI_WIDGET_POINTER_BY_NAME_ROOT(t_time, "time_b", current_view);
            gui_text_content_set((gui_text_t *)t_time, time_str, strlen(time_str));
        }

    }
}

// draw timecard and not display
static void draw_timecard(void *parent)
{
    gui_win_t *win = gui_win_create(parent, __WIN1_NAME, 0, 0, SCREEN_WIDTH, 60);
    gui_img_t *rect_timecard = gui_img_create_from_mem(win, __WIN0_NAME, BOTTOM_TIME_CARD_BG_BIN, 35, 0,
                                                       0, 0);
    // gui_img_set_mode(rect_timecard, IMG_SRC_OVER_MODE);
    // gui_rounded_rect_t *rect_timecard = gui_rect_create(GUI_BASE(win), __WIN1_NAME, 35, 0,
    //                                                                   340, 60, 20, gui_rgba(39, 43, 44, 255 * 0.7));
    // text
    gui_text_t *timecard_date_text = gui_text_create(rect_timecard, "date_s",  15, 20, 0, 0);
    gui_text_set(timecard_date_text, (void *)date_timecard_content, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                 strlen(date_timecard_content),
                 32);


    gui_text_type_set(timecard_date_text, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(timecard_date_text, LEFT);

    gui_text_t *timecard_time_text = gui_text_create(rect_timecard, "time_s",  -10, 20, 0,
                                                     0);
    gui_text_set(timecard_time_text, (void *)time_str, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                 strlen(time_str),
                 32);
    gui_text_type_set(timecard_time_text, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(timecard_time_text, RIGHT);
    GUI_BASE(win)->not_show = 1;
    gui_win_enable_blur(win, false);
}

static void timer_cb(void *obj)
{
    (void)obj;
    static uint16_t count = 0;
    count++;
    if (count >= 300)
    {
        time_update_cb();
        count = 0;
    }

    // control timecard display
    GUI_WIDGET_POINTER_BY_NAME_ROOT(img_timecard, __WIN0_NAME, current_view);
    GUI_WIDGET_POINTER_BY_NAME_ROOT(canvas_timecard, __WIN1_NAME, current_view);
    if (list->offset < 93)
    {
        img_timecard->not_show = 1;
        canvas_timecard->not_show = 0;
        gui_win_enable_blur((void *)canvas_timecard, true);
    }
    else
    {
        img_timecard->not_show = 0;
        canvas_timecard->not_show = 1;
        gui_win_enable_blur((void *)canvas_timecard, false);
    }
}

static void switch_app_cb(void *obj)
{
    GUI_UNUSED(obj);
    const char *obj_name = ((gui_obj_t *)obj)->name;
    if (strcmp(obj_name, "note_weather") == 0)
    {
        gui_view_switch_direct(current_view, "weather_view",
                               SWITCH_IN_FROM_BOTTOM_USE_TRANSLATION,
                               SWITCH_IN_ANIMATION_FADE);
    }
    else if (strcmp(obj_name, "note_ac") == 0)
    {
        gui_view_switch_direct(current_view, "activity_view",
                               SWITCH_IN_FROM_BOTTOM_USE_TRANSLATION,
                               SWITCH_IN_ANIMATION_FADE);
    }
    else if (strcmp(obj_name, "MUSIC") == 0)
    {
        gui_view_set_animate_step(current_view, 60);
        gui_view_switch_direct(current_view, "music_view",
                               SWITCH_IN_FROM_BOTTOM_USE_TRANSLATION,
                               SWITCH_IN_ANIMATION_FADE);
    }
    else if (strcmp(obj_name, "BATTERY") == 0)
    {
        gui_view_set_animate_step(current_view, 60);
        gui_view_switch_direct(current_view, "battery_view",
                               SWITCH_IN_FROM_BOTTOM_USE_TRANSLATION,
                               SWITCH_IN_ANIMATION_FADE);
    }
    else if (strcmp(obj_name, "MESSAGE") == 0)
    {
        // gui_view_switch_direct(current_view, "app_message_view", SWITCH_IN_FROM_BOTTOM_USE_TRANSLATION,
        //                        SWITCH_IN_ANIMATION_FADE);
    }
    else if (strcmp(obj_name, "APP_MENU") == 0)
    {
        gui_view_set_animate_step(current_view, 100);
        gui_view_switch_direct(current_view, "menu_view",
                               SWITCH_OUT_ANIMATION_FADE,
                               SWITCH_IN_ANIMATION_FADE);
    }
}
static int steps_to_calories(int steps)
{
    return (int)(steps * 0.04f);
}
// Update activity arcs with new data
static void update_activity_arcs(void *obj)
{
    GUI_UNUSED(obj);

    // Simulated data for testing (remove when real data is available)
    static int sim_move = 0;
    static int sim_ex = 0;
    static int sim_stand = 0;
    static int sim_direction = 1;

    cJSON *root;
    extern char *cjson_content;

    int move_steps = 0;
    int ex_minutes = 0;
    int stand_hours = 0;

    if (!cjson_content)
    {
        // Use simulated data when no JSON available
        sim_move += sim_direction * 50;
        sim_ex += sim_direction * 2;
        sim_stand += sim_direction * 1;

        if (sim_move >= 8000 || sim_move <= 0) { sim_direction *= -1; }

        move_steps = sim_move;
        ex_minutes = sim_ex;
        stand_hours = sim_stand;

        gui_log("Using simulated data: move=%d, ex=%d, stand=%d\n", move_steps, ex_minutes, stand_hours);
    }
    else
    {
        root = cJSON_Parse(cjson_content);
        if (!root)
        {
            gui_log("Error parsing JSON!\r\n");
            return;
        }

        // parse activity array
        cJSON *activity_array = cJSON_GetObjectItemCaseSensitive(root, "activity");
        if (cJSON_IsArray(activity_array))
        {
            cJSON *act = cJSON_GetArrayItem(activity_array, 0);
            if (!act)
            {
                gui_log("get activity_array unsuccessful\n");
                cJSON_Delete(root);
                return;
            }
            else
            {
                cJSON *move = cJSON_GetObjectItemCaseSensitive(act, "move");
                cJSON *ex = cJSON_GetObjectItemCaseSensitive(act, "exercise");
                cJSON *stand = cJSON_GetObjectItemCaseSensitive(act, "stand");

                move_steps = move ? move->valueint : 0;
                ex_minutes = ex ? ex->valueint : 0;
                stand_hours = stand ? stand->valueint : 0;
            }
        }
        cJSON_Delete(root);
    }

    int calories = steps_to_calories(move_steps);

    // Update move arc (outer ring) - starts from -90° (top), goes clockwise
    GUI_WIDGET_POINTER_BY_NAME_ROOT(arc_move, "arc_move", current_view);
    if (arc_move)
    {
        float progress = (float)calories / 500.0f;
        if (progress > 1.0f) { progress = 1.0f; }
        float move_angle = -90.0f + 360.0f * progress;
        gui_arc_set_end_angle((gui_arc_t *)arc_move, move_angle);
    }

    // Update exercise arc (middle ring)
    GUI_WIDGET_POINTER_BY_NAME_ROOT(arc_ex, "arc_ex", current_view);
    if (arc_ex)
    {
        float progress = (float)ex_minutes / 60.0f;
        if (progress > 1.0f) { progress = 1.0f; }
        float ex_angle = -90.0f + 360.0f * progress;
        gui_arc_set_end_angle((gui_arc_t *)arc_ex, ex_angle);
    }

    // Update stand arc (inner ring)
    GUI_WIDGET_POINTER_BY_NAME_ROOT(arc_stand, "arc_stand", current_view);
    if (arc_stand)
    {
        float progress = (float)stand_hours / 6.0f;
        if (progress > 1.0f) { progress = 1.0f; }
        float stand_angle = -90.0f + 360.0f * progress;
        gui_arc_set_end_angle((gui_arc_t *)arc_stand, stand_angle);
    }

    // Update text labels
    {
        GUI_WIDGET_POINTER_BY_NAME_ROOT(move_text, "ac_move", current_view);
        sprintf(move_content, "%d/500 kcol",  calories);
        gui_text_content_set((gui_text_t *)move_text, move_content, strlen(move_content));
    }
    {
        GUI_WIDGET_POINTER_BY_NAME_ROOT(ex_text, "ac_ex", current_view);
        sprintf(ex_content, "%d/60 min", ex_minutes);
        gui_text_content_set((gui_text_t *)ex_text, ex_content, strlen(ex_content));
    }
    {
        GUI_WIDGET_POINTER_BY_NAME_ROOT(stand_text, "ac_stand", current_view);
        sprintf(stand_content, "%d/6 h", stand_hours);
        gui_text_content_set((gui_text_t *)stand_text, stand_content, strlen(stand_content));
    }
}
// Update battery arc with current level
static void update_battery_arc(void *obj)
{
    GUI_UNUSED(obj);

    // Simulate battery level changes for demo
    static int sim_battery = 75;
    static int sim_direction = -1;

    sim_battery += sim_direction;
    if (sim_battery <= 10 || sim_battery >= 100) { sim_direction *= -1; }

    battery_level = sim_battery;

    // Update battery arc
    GUI_WIDGET_POINTER_BY_NAME_ROOT(arc_battery, "arc_battery",
                                    current_view);
    if (arc_battery)
    {
        float progress = (float)battery_level / 100.0f;
        if (progress > 1.0f) { progress = 1.0f; }
        float battery_angle = -90.0f + 360.0f * progress;
        gui_arc_set_end_angle((gui_arc_t *)arc_battery, battery_angle);

        // Change color based on battery level
        gui_color_t color;
        if (battery_level < 20)
        {
            color = gui_rgba(255, 59, 48, 255); // Red for low battery
        }
        else
        {
            color = gui_rgba(52, 199, 89, 255); // Green for normal
        }
        gui_arc_set_color((gui_arc_t *)arc_battery, color);
    }

    // Update battery text using saved widget pointer
    if (battery_text_widget)
    {
        sprintf(battery_content, "%d", battery_level);
        gui_text_content_set(battery_text_widget, battery_content, strlen(battery_content));
    }
}
static void note_design(gui_obj_t *obj, void *p)
{
    (void)p;
    uint16_t index = ((gui_list_note_t *)obj)->index;
    int16_t offset_X = 29;

    if (index == 0)
    {
        // note_weather
        gui_img_t *img = gui_img_create_from_mem(obj, "note_weather", UI_CARD_WEATHER_BIN, offset_X, 0, 0,
                                                 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        gui_obj_add_event_cb(img, (gui_event_cb_t)switch_app_cb, GUI_EVENT_TOUCH_CLICKED, NULL);

    }
    else if (index == 1)
    {
        // note_activity
        {

            gui_img_t *rect_activity = gui_img_create_from_mem(obj, "note_ac", BOTTOM_TAB_BG_BIN, offset_X, 0,
                                                               0,
                                                               0);
            gui_img_set_mode(rect_activity, IMG_SRC_OVER_MODE);

            if (move_content == NULL)
            {
                move_content = (char *)gui_malloc(30);
                ex_content = (char *)gui_malloc(30);
                stand_content = (char *)gui_malloc(30);
            }
            sprintf(move_content, "0/500 kcol");
            gui_text_t *move_text = gui_text_create(rect_activity, "ac_move", 134, 20, 0, 0);
            gui_text_set(move_text, (void *)move_content, GUI_FONT_SRC_TTF, gui_rgb(230, 67, 79),
                         strlen(move_content), 24);
            gui_text_type_set(move_text, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
            gui_text_mode_set(move_text, LEFT);

            sprintf(ex_content, "0/60 min");
            gui_text_t *ex_text = gui_text_create(rect_activity, "ac_ex", 134, 57, 0, 0);
            gui_text_set(ex_text, (void *)ex_content, GUI_FONT_SRC_TTF, gui_rgb(186, 253, 79),
                         strlen(ex_content), 24);
            gui_text_type_set(ex_text, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
            gui_text_mode_set(ex_text, LEFT);


            sprintf(stand_content, "0/6 h");
            gui_text_t *stand_text = gui_text_create(rect_activity, "ac_stand", 134, 94, 0, 0);
            gui_text_set(stand_text, (void *)stand_content, GUI_FONT_SRC_TTF, gui_rgb(117, 230, 229),
                         strlen(stand_content), 24);
            gui_text_type_set(stand_text, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
            gui_text_mode_set(stand_text, LEFT);
            // activity arcs using gui_lite_arc
            {
                int center_x = 17 + 50;  // Local coordinates relative to rect_activity
                int center_y = 26 + 50;

                // Background arcs (subtle dark colors) - full circles
                // Move ring background: deep red-brown
                gui_arc_create(rect_activity, "arc_move_bg",
                               center_x, center_y, 42, 0, 360, 8, gui_rgba(45, 20, 25, 255));

                // Exercise ring background: deep green
                gui_arc_create(rect_activity, "arc_ex_bg",
                               center_x, center_y, 29, 0, 360, 8, gui_rgba(20, 40, 20, 255));

                // Stand ring background: deep cyan
                gui_arc_create(rect_activity, "arc_stand_bg",
                               center_x, center_y, 17, 0, 360, 8, gui_rgba(18, 35, 40, 255));

                // Foreground arcs (vibrant Apple Watch colors)
                // Start from -90° (top) for Apple Watch style
                // Move ring: bright pink-red (Apple's signature Move color)
                gui_arc_create(rect_activity, "arc_move",
                               center_x, center_y, 42, -90, -90, 8, gui_rgba(250, 50, 90, 255));

                // Exercise ring: bright lime green (Apple's Exercise color)
                gui_arc_create(rect_activity, "arc_ex",
                               center_x, center_y, 29, -90, -90, 8, gui_rgba(160, 250, 80, 255));

                // Stand ring: bright cyan (Apple's Stand color)
                gui_arc_create(rect_activity, "arc_stand",
                               center_x, center_y, 17, -90, -90, 8, gui_rgba(80, 220, 240, 255));

                // Create timer to update arcs
                gui_obj_create_timer(GUI_BASE(rect_activity), 1000, true, update_activity_arcs);
            }
            gui_obj_add_event_cb(rect_activity, (gui_event_cb_t)switch_app_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
        }
    }
    else if (index == 2)
    {
        gui_img_t *rect_app = gui_img_create_from_mem(obj, "APP_CANVAS", BOTTOM_TAB_BG_BIN, offset_X, 0, 0,
                                                      0);
        gui_img_set_mode(rect_app, IMG_SRC_OVER_MODE);
        // Information Statistics Quick Bar
        int item_size = 85;
        int item_y = 36;  // Vertical position
        int left_margin = 20;
        int spacing = 28;

        //music (left item)
        {
            int music_x = left_margin;
            gui_img_t *img = gui_img_create_from_mem(rect_app, "MUSIC", APP_MUSIC_ICON_BIN, music_x, item_y, 0,
                                                     0);
            gui_img_scale(img, 0.85, 0.85);
            gui_obj_add_event_cb(img, (gui_event_cb_t)switch_app_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
        }

        //battery (center item)
        {
            // battery_level = 75;

            int battery_x = left_margin + item_size + spacing;

            // Battery arcs using gui_lite_arc
            int center_x = battery_x + item_size / 2;  // Center of 100x100 area
            int center_y = item_y + item_size / 2;
            float radius = 36.0f;
            float line_width = 7.0f;

            // Background arc (dark gray) - full circle
            gui_arc_t *arc_battery_bg = gui_arc_create(rect_app, "BATTERY",
                                                       center_x, center_y, (int)radius, 0, 360, line_width, gui_rgba(40, 40, 40, 255));

            // Foreground arc (green for normal battery)
            gui_arc_t *arc_battery = gui_arc_create(rect_app, "arc_battery",
                                                    center_x, center_y, (int)radius, -90, -90, line_width, gui_rgba(52, 199, 89, 255));

            // Battery percentage text (centered in the circle)
            // Text is mounted under arc_battery, coordinates are relative to arc widget's origin
            sprintf(battery_content, "%d", battery_level);

            // Position text to center it in the 100x100 area
            // Text position relative to arc widget origin (left-aligned)
            battery_text_widget = gui_text_create(arc_battery, "battery_text", 20, 13, 0, 0);
            gui_text_set(battery_text_widget, battery_content, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                         strlen(battery_content), 50);
            gui_text_type_set(battery_text_widget, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
            gui_text_mode_set(battery_text_widget, LEFT);

            // Create timer to update battery arc
            gui_obj_create_timer(GUI_BASE(rect_app), 1000, true, update_battery_arc);

            gui_obj_add_event_cb(arc_battery_bg, (gui_event_cb_t)switch_app_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
        }

        //message (right item)
        {
            int message_x = left_margin + (item_size + spacing) * 2;
            gui_img_t *img = gui_img_create_from_mem(rect_app, "MESSAGE", APP_MESSAGE_ICON_BIN, message_x,
                                                     item_y, 0, 0);
            gui_img_set_mode(img, IMG_SRC_OVER_MODE);
            gui_img_scale(img, 0.85, 0.85);
            // gui_obj_add_event_cb(img, (gui_event_cb_t)switch_app_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
        }
    }
    else if (index == 3)
    {
        gui_img_t *rect_menu_bg = gui_img_create_from_mem(obj, "APP_MENU", BOTTOM_LIST_MENU_BG_BIN, 76, 0,
                                                          0,
                                                          0);
        gui_img_set_mode(rect_menu_bg, IMG_SRC_OVER_MODE);
        gui_text_t *text = gui_text_create(rect_menu_bg, 0, 70, 20, 0, 0);
        gui_text_set(text, "APP MENU", GUI_FONT_SRC_TTF, APP_COLOR_WHITE, strlen("APP MENU"), 32);
        gui_text_type_set(text, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, LEFT);
        gui_obj_add_event_cb(rect_menu_bg, (gui_event_cb_t)switch_app_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
    }
}
static void bottom_view_design(gui_view_t *view)
{
    current_view = view;
    gui_view_switch_on_event(view, "watchface_view",
                             SWITCH_OUT_TO_BOTTOM_USE_TRANSLATION,
                             SWITCH_INIT_STATE,
                             GUI_EVENT_TOUCH_MOVE_DOWN);
    gui_view_set_opacity(view, 200);
    gui_obj_t *parent = GUI_BASE(view);
    // draw background
    // gui_canvas_rect_t *canvas_bg = NULL;
    gui_rounded_rect_t *rect_menu_bg = gui_rect_create(parent, "bottom_bg", 0, 0,
                                                       SCREEN_WIDTH, SCREEN_HEIGHT, 0, gui_rgba(0, 0, 0, 255));

    if (current_watchface_type == WATCHFACE_BIG_NUM)
    {
        gui_rect_set_color(rect_menu_bg, gui_rgba(219, 122, 147, 255));
    }

    gui_obj_create_timer(GUI_BASE(rect_menu_bg), 100, true, timer_cb);

    //clock circle
    gui_win_t *win = gui_win_create(parent, __WIN0_NAME, 38, 30, 335, 180);
    gui_img_t *img_clock = gui_img_create_from_mem(win, 0, UI_CARD_CLOCKCIRCLE_BIN, 163, 0, 0, 0);
    gui_img_set_mode(img_clock, IMG_SRC_OVER_MODE);
    gui_text_t *t_time = gui_text_create(img_clock, "time_b", 0, 0, 190, 190);
    gui_text_set(t_time, time_str, GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255),
                 strlen(time_str), 60);
    gui_text_type_set(t_time, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t_time, MID_CENTER);
    gui_text_rendermode_set(t_time, 2);

    gui_text_t *date_text = gui_text_create(win, "date_b",  0, 22, 0, 0);
    gui_text_set(date_text, (void *)date_content, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                 strlen(date_content),
                 48);
    gui_text_type_set(date_text, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(date_text, MULTI_LEFT);

    // Card list
    int space = 10;
    int length = 157;
    list = gui_list_create(parent, "list", 0, 0, 0, 0, length, space, VERTICAL, note_design, NULL,
                           false);
    gui_list_set_style(list, LIST_CARD);
    gui_list_set_note_num(list, 4);
    gui_list_set_offset(list, 250);
    gui_list_set_card_stack_location(list, 20);

    draw_timecard(parent);

    time_update_cb();
}
