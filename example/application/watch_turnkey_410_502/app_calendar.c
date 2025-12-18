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
#include "gui_rect.h"
/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "calendar_view"
#define SCREEN_WIDTH (int16_t)gui_get_screen_width()
#define SCREEN_HEIGHT (int16_t)gui_get_screen_height()
/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void calendar_design(gui_view_t *view);

extern int16_t g_selected_workout_index;

/*============================================================================*
 *                           GUI_VIEW_INSTANCE
 *============================================================================*/
GUI_VIEW_INSTANCE(CURRENT_VIEW_NAME, false, calendar_design, NULL);

/*============================================================================*
 *                            Variables
 *============================================================================*/
static gui_list_t *list = NULL;
static gui_view_t *current_view = NULL;
static gui_win_t *calendar_win = NULL;
static gui_win_t *header_win = NULL;

// Calendar data
static int current_year = 2024;
static int current_month = 9;  // September
static int current_day = 13;
static int display_year = 2024;
static int display_month = 9;

static char time_display[10] = "10:09";
static char month_name[20] = "September";
static char weekday_name[20] = "Friday";

static const char *month_names[] =
{
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};

static const char *weekday_names[] =
{
    "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
};

/*============================================================================*
 *                           Private Functions
 *============================================================================*/

// Get days in month
static int get_days_in_month(int year, int month)
{
    int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (month == 2 && ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)))
    {
        return 29; // Leap year
    }
    return days[month - 1];
}

// Get first day of week for month (0=Sunday, 6=Saturday)
static int get_first_weekday(int year, int month)
{
    // Zeller's congruence algorithm
    int q = 1; // First day of month
    int m = month;
    int y = year;

    if (m < 3)
    {
        m += 12;
        y -= 1;
    }

    int k = y % 100;
    int j = y / 100;
    int h = (q + ((13 * (m + 1)) / 5) + k + (k / 4) + (j / 4) - (2 * j)) % 7;

    // Convert to 0=Sunday format
    int weekday = (h + 6) % 7;
    return weekday;
}

// Get current weekday
static int get_weekday(int year, int month, int day)
{
    int q = day;
    int m = month;
    int y = year;

    if (m < 3)
    {
        m += 12;
        y -= 1;
    }

    int k = y % 100;
    int j = y / 100;
    int h = (q + ((13 * (m + 1)) / 5) + k + (k / 4) + (j / 4) - (2 * j)) % 7;

    return (h + 6) % 7;
}

static void time_update_cb(void *obj)
{
    gui_text_content_set(obj, time_str, strlen(time_str));
}

// Update header month display based on scroll position
static void update_header_month_cb(void *obj)
{
    GUI_UNUSED(obj);

    if (!list || !header_win) { return; }

    // Get current scroll offset
    int offset = 0;
    // int offset = gui_list_get_offset(list);
    int note_length = SCREEN_HEIGHT - 80;

    // Calculate which page is currently visible (0=prev, 1=current, 2=next)
    int page_index = 1; // Default to current month
    if (offset > -note_length / 2)
    {
        page_index = 0; // Previous month
    }
    else if (offset < -note_length * 1.5f)
    {
        page_index = 2; // Next month
    }

    // Calculate the month to display
    int show_year = current_year;
    int show_month = current_month;

    if (page_index == 0)
    {
        // Previous month
        show_month--;
        if (show_month < 1)
        {
            show_month = 12;
            show_year--;
        }
    }
    else if (page_index == 2)
    {
        // Next month
        show_month++;
        if (show_month > 12)
        {
            show_month = 1;
            show_year++;
        }
    }

    // Update display month if changed
    if (show_year != display_year || show_month != display_month)
    {
        display_year = show_year;
        display_month = show_month;

        // Update header text
        sprintf(month_name, "%s", month_names[display_month - 1]);
        GUI_WIDGET_POINTER_BY_NAME_ROOT(month_text, "month_text", header_win);
        if (month_text)
        {
            gui_text_content_set((gui_text_t *)month_text, month_name, strlen(month_name));
        }
    }
}

// Scroll to current month
static void scroll_to_current_month(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(e);
    GUI_UNUSED(param);

    if (list)
    {
        display_year = current_year;
        display_month = current_month;

        // Update header immediately
        sprintf(month_name, "%s", month_names[current_month - 1]);
        GUI_WIDGET_POINTER_BY_NAME_ROOT(month_text, "month_text", header_win);
        if (month_text)
        {
            gui_text_content_set((gui_text_t *)month_text, month_name, strlen(month_name));
        }

        // Scroll to middle position (current month)
        int note_length = SCREEN_HEIGHT - 80;
        gui_list_set_offset(list, -note_length);
    }
}
// Draw calendar month
static void draw_calendar_month(gui_obj_t *parent, int year, int month)
{
    int days_in_month = get_days_in_month(year, month);
    int first_weekday = get_first_weekday(year, month);

    int y_offset = 0;
    int cell_width = SCREEN_WIDTH / 7;
    int cell_height = 50;

    // Month title
    gui_text_t *month_title = gui_text_create(parent, "month_title", 20, y_offset, 0, 0);
    gui_text_set(month_title, (void *)month_names[month - 1], GUI_FONT_SRC_TTF,
                 gui_rgb(255, 100, 80), strlen(month_names[month - 1]), 36);
    gui_text_type_set(month_title, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(month_title, LEFT);
    gui_text_rendermode_set(month_title, 2);

    y_offset += 50;

    // Weekday headers (S M T W T F S)
    // Weekends (S) are gray, weekdays (M T W T F) are white
    const char *weekday_abbr[] = {"S", "M", "T", "W", "T", "F", "S"};
    for (int i = 0; i < 7; i++)
    {
        // Weekend (Sunday=0, Saturday=6) is gray, weekday is white
        gui_color_t header_color = (i == 0 || i == 6) ? gui_rgb(150, 150, 150) : gui_rgb(255, 255, 255);

        gui_text_t *weekday = gui_text_create(parent, 0,
                                              i * cell_width + cell_width / 2 - 10,
                                              y_offset, 0, 0);
        gui_text_set(weekday, (void *)weekday_abbr[i], GUI_FONT_SRC_TTF,
                     header_color, strlen(weekday_abbr[i]), 24);
        gui_text_type_set(weekday, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(weekday, LEFT);
        gui_text_rendermode_set(weekday, 2);
    }

    y_offset += 40;

    // Draw days
    int day = 1;
    int row = 0;

    while (day <= days_in_month)
    {
        for (int col = 0; col < 7 && day <= days_in_month; col++)
        {
            // Skip empty cells before first day
            if (row == 0 && col < first_weekday)
            {
                continue;
            }

            char day_str[4];
            sprintf(day_str, "%d", day);

            // Calculate text position (centered in cell)
            int text_offset = (day < 10) ? 10 : 5;  // Single digit vs double digit
            int x_pos = col * cell_width + cell_width / 2 - text_offset;
            int y_pos = y_offset + row * cell_height;

            // Check if this is current day
            bool is_today = (year == current_year && month == current_month && day == current_day);

            // Check if weekend (0=Sunday, 6=Saturday)
            bool is_weekend = (col == 0 || col == 6);

            // Determine text color: white for weekdays, gray for weekends
            gui_color_t text_color = is_weekend ? gui_rgb(150, 150, 150) : gui_rgb(255, 255, 255);

            if (is_today)
            {
                // Draw red rounded rectangle background for current day
                int bg_x = col * cell_width + 8;
                int bg_y = y_pos - 5;
                int bg_w = cell_width - 16;
                int bg_h = cell_height - 10;
                int bg_radius = 8;  // Corner radius

                gui_rect_create(parent, 0, bg_x, bg_y, bg_w, bg_h, bg_radius,
                                gui_rgb(255, 100, 80));

                // Current day always white text
                gui_text_t *day_text = gui_text_create(parent, 0, x_pos, y_pos, 0, 0);
                gui_text_set(day_text, day_str, GUI_FONT_SRC_TTF,
                             gui_rgb(255, 255, 255), strlen(day_str), 32);
                gui_text_type_set(day_text, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
                gui_text_mode_set(day_text, LEFT);
                gui_text_rendermode_set(day_text, 2);
            }
            else
            {
                gui_text_t *day_text = gui_text_create(parent, 0, x_pos, y_pos, 0, 0);
                gui_text_set(day_text, day_str, GUI_FONT_SRC_TTF,
                             text_color, strlen(day_str), 32);
                gui_text_type_set(day_text, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
                gui_text_mode_set(day_text, LEFT);
                gui_text_rendermode_set(day_text, 2);
            }

            day++;
        }
        row++;
    }
}

static void page_0_design(gui_obj_t *parent)
{
    // Previous month
    int prev_month = display_month - 1;
    int prev_year = display_year;
    if (prev_month < 1)
    {
        prev_month = 12;
        prev_year--;
    }
    draw_calendar_month(parent, prev_year, prev_month);
}

static void page_1_design(gui_obj_t *parent)
{
    // Current display month
    draw_calendar_month(parent, display_year, display_month);
}

static void page_2_design(gui_obj_t *parent)
{
    // Next month
    int next_month = display_month + 1;
    int next_year = display_year;
    if (next_month > 12)
    {
        next_month = 1;
        next_year++;
    }
    draw_calendar_month(parent, next_year, next_month);
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
        page_1_design(obj);
        break;
    case 2:
        page_2_design(obj);
        break;
    default:
        break;
    }
}

static void calendar_design(gui_view_t *view)
{
    current_view = view;
    gui_obj_t *parent = GUI_BASE(view);
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);

    // Initialize current date (in real app, get from system time)
    current_year = 2024;
    current_month = 9;
    current_day = 13;
    display_year = current_year;
    display_month = current_month;

    sprintf(month_name, "%s", month_names[current_month - 1]);
    sprintf(time_display, "10:09");

    gui_view_switch_on_event(current_view, "watchface_view",
                             SWITCH_OUT_ANIMATION_FADE,
                             SWITCH_IN_ANIMATION_FADE,
                             GUI_EVENT_KB_SHORT_CLICKED);

    // Fixed header (top 80px)
    header_win = gui_win_create(parent, "header_win", 0, 0, SCREEN_WIDTH, 80);

    // Time display (top right)
    gui_text_t *t_time = gui_text_create(header_win, "time_text",
                                         SCREEN_WIDTH - 120, 10, 0, 0);
    gui_text_set(t_time, time_display, GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255),
                 strlen(time_display), 32);
    gui_text_type_set(t_time, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t_time, LEFT);
    gui_text_rendermode_set(t_time, 2);
    gui_obj_create_timer((void *)t_time, 30000, -1, time_update_cb);

    // Month name (red)
    gui_text_t *month_text = gui_text_create(header_win, "month_text", 20, 35, 0, 0);
    gui_text_set(month_text, month_name, GUI_FONT_SRC_TTF, gui_rgb(255, 100, 80),
                 strlen(month_name), 40);
    gui_text_type_set(month_text, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(month_text, LEFT);
    gui_text_rendermode_set(month_text, 2);

    // Add click event to header to return to current month
    gui_obj_add_event_cb(header_win, scroll_to_current_month, GUI_EVENT_TOUCH_CLICKED, NULL);

    // Scrollable calendar list
    int note_length = SCREEN_HEIGHT - 80;  // Height of each month view
    uint8_t space = 0;  // No space between months
    list = gui_list_create(parent, "calendar_list", 0, 80, SCREEN_WIDTH, SCREEN_HEIGHT - 80,
                           note_length, space, VERTICAL, note_design, NULL, false);
    gui_list_set_style(list, LIST_FADE);
    gui_list_set_note_num(list, 3);  // Show 3 months (prev, current, next)
    gui_list_set_inertia(list, true);
    gui_list_set_auto_align(list, true);
    gui_list_set_factor(list, 0.3f);

    // Start at current month (middle position)
    gui_list_set_offset(list, -note_length);

    // Create timer to update header month based on scroll position
    gui_obj_create_timer(GUI_BASE(list), 100, true, update_header_month_cb);
}
