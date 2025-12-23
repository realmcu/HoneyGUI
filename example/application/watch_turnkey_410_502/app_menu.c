/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <time.h>
#include "gui_api.h"
#include "root_image/ui_resource.h"
#include "guidef.h"
#include "gui_img.h"
#include "gui_text.h"
#include "gui_list.h"
#include "app_main_watch.h"
#include "gui_menu_cellular.h"
#include "gui_view_instance.h"
#include "gui_canvas_rect.h"

/*============================================================================*
 *                           Types
 *============================================================================*/
typedef struct note_design_param
{
    void **click_cb;
    uint32_t **img_data_array;
} note_design_param_t;

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "menu_view"
#define SCREEN_WIDTH (int16_t)gui_get_screen_width()
#define SCREEN_HEIGHT (int16_t)gui_get_screen_height()

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void app_menu_design(gui_view_t *view);
static void clear_menu(gui_view_t *view);

/*============================================================================*
 *                           GUI_VIEW_INSTANCE
 *============================================================================*/
GUI_VIEW_INSTANCE(CURRENT_VIEW_NAME, false, app_menu_design, clear_menu);

/*============================================================================*
 *                            Variables
 *============================================================================*/
static note_design_param_t *design_p = NULL;
extern uint8_t menu_style;
static int16_t list_offset_his = 0;
static gui_menu_cellular_t *menu_cellular = NULL;
static int16_t cellular_offset_x = 0;
static int16_t cellular_offset_y = 0;

static const char *text_array[] =
{
    "Activity",
    "Heart Rate",
    "Weather",
    "Reminders",
    "Message",
    "Phone",
    "Noise",
    "Music",
    "Map",
    "Stopwatch",
    "Timers",
    "Alarm Clock",
    "Audio Books",
    "Workout",
    "Calendar",
    "Compass",
    "Home",
    "Contacts",
    "Cycle Tracking",
    "Mail",
    "Now Playing",
    "News",
    "Photo",
    "Podcasts",
    "Sleep",
    "Stocks",
    "World Clock",
    "Store",
};


/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static void clear_menu(gui_view_t *view)
{
    (void)view;
    if (design_p)
    {
        gui_free(design_p->click_cb);
        gui_free(design_p->img_data_array);
        gui_free(design_p);
        design_p = NULL;
    }
}
static void switch_app_activity(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    gui_view_switch_direct(gui_view_get_current(), "activity_view",
                           SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}

static void switch_app_heart_rate(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    gui_view_switch_direct(gui_view_get_current(), "heartrate_view",
                           SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}
static void switch_app_weather(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    gui_view_switch_direct(gui_view_get_current(), "weather_view",
                           SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}
static void switch_app_reminders(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    // gui_view_switch_direct(gui_view_get_current(), "reminders_view", SWITCH_OUT_ANIMATION_FADE, SWITCH_IN_ANIMATION_FADE);
}

static void switch_app_message(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    // gui_view_switch_direct(gui_view_get_current(), "message_view", SWITCH_OUT_ANIMATION_FADE, SWITCH_IN_ANIMATION_FADE);
}

static void switch_app_phone(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    // gui_view_switch_direct(gui_view_get_current(), "phone_view", SWITCH_OUT_ANIMATION_FADE, SWITCH_IN_ANIMATION_FADE);
}

static void switch_app_noise(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    // gui_view_switch_direct(gui_view_get_current(), "noise_view", SWITCH_OUT_ANIMATION_FADE, SWITCH_IN_ANIMATION_FADE);
}

static void switch_app_music(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    gui_view_switch_direct(gui_view_get_current(), "music_view",
                           SWITCH_OUT_ANIMATION_FADE, SWITCH_IN_ANIMATION_FADE);
}

static void switch_app_map(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    // gui_view_switch_direct(gui_view_get_current(), "map_view", SWITCH_OUT_ANIMATION_FADE, SWITCH_IN_ANIMATION_FADE);
}

static void switch_app_stopwatch(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    gui_view_switch_direct(gui_view_get_current(), "stopwatch_view",
                           SWITCH_OUT_ANIMATION_FADE, SWITCH_IN_ANIMATION_FADE);
}

static void switch_app_timers(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    // gui_view_switch_direct(gui_view_get_current(), "timers_view", SWITCH_OUT_ANIMATION_FADE, SWITCH_IN_ANIMATION_FADE);
}

static void switch_app_alarm_clock(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    // gui_view_switch_direct(gui_view_get_current(), "alarm_clock_view", SWITCH_OUT_ANIMATION_FADE, SWITCH_IN_ANIMATION_FADE);
}

static void switch_app_audio_books(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    // gui_view_switch_direct(gui_view_get_current(), "audio_books_view", SWITCH_OUT_ANIMATION_FADE, SWITCH_IN_ANIMATION_FADE);
}

static void switch_app_workout(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    gui_view_switch_direct(gui_view_get_current(), "workout_view", SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}

static void switch_app_calendar(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    // gui_view_switch_direct(gui_view_get_current(), "calendar_view", SWITCH_OUT_ANIMATION_FADE,
    //                        SWITCH_IN_ANIMATION_FADE);
}

static void switch_app_compass(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    gui_view_switch_direct(gui_view_get_current(), "compass_view", SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}

static void switch_app_home(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    // gui_view_switch_direct(gui_view_get_current(), "home_view", SWITCH_OUT_ANIMATION_FADE, SWITCH_IN_ANIMATION_FADE);
}

static void switch_app_contacts(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    // gui_view_switch_direct(gui_view_get_current(), "contacts_view", SWITCH_OUT_ANIMATION_FADE, SWITCH_IN_ANIMATION_FADE);
}

static void switch_app_cycle_tracking(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    // gui_view_switch_direct(gui_view_get_current(), "cycle_tracking_view", SWITCH_OUT_ANIMATION_FADE, SWITCH_IN_ANIMATION_FADE);
}

static void switch_app_mail(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    // gui_view_switch_direct(gui_view_get_current(), "mail_view", SWITCH_OUT_ANIMATION_FADE, SWITCH_IN_ANIMATION_FADE);
}

static void switch_app_now_playing(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    // gui_view_switch_direct(gui_view_get_current(), "now_playing_view", SWITCH_OUT_ANIMATION_FADE, SWITCH_IN_ANIMATION_FADE);
}

static void switch_app_news(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    // gui_view_switch_direct(gui_view_get_current(), "news_view", SWITCH_OUT_ANIMATION_FADE, SWITCH_IN_ANIMATION_FADE);
}

static void switch_app_photo(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    // gui_view_switch_direct(gui_view_get_current(), "photo_view", SWITCH_OUT_ANIMATION_FADE, SWITCH_IN_ANIMATION_FADE);
}

static void switch_app_podcasts(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    // gui_view_switch_direct(gui_view_get_current(), "podcasts_view", SWITCH_OUT_ANIMATION_FADE, SWITCH_IN_ANIMATION_FADE);
}

static void switch_app_sleep(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    // gui_view_switch_direct(gui_view_get_current(), "sleep_view", SWITCH_OUT_ANIMATION_FADE, SWITCH_IN_ANIMATION_FADE);
}

static void switch_app_stocks(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    // gui_view_switch_direct(gui_view_get_current(), "stocks_view", SWITCH_OUT_ANIMATION_FADE, SWITCH_IN_ANIMATION_FADE);
}

static void switch_app_world_clock(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    // gui_view_switch_direct(gui_view_get_current(), "world_clock_view", SWITCH_OUT_ANIMATION_FADE, SWITCH_IN_ANIMATION_FADE);
}

static void switch_app_store(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    // gui_view_switch_direct(gui_view_get_current(), "store_view", SWITCH_OUT_ANIMATION_FADE, SWITCH_IN_ANIMATION_FADE);
}
static void switch_menu_style_cb(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    if (gui_view_get_next() != NULL) { return; }

    gui_obj_t *parent = GUI_BASE(gui_view_get_current())->parent;
    gui_obj_tree_free(GUI_BASE(gui_view_get_current()));
    clear_menu(gui_view_get_current());

    menu_style = (menu_style + 1) % 2;
    gui_view_create(parent, "menu_view", 0, 0, 0, 0);
}
static void note_design(gui_obj_t *obj, void *p)
{
    note_design_param_t *design_p = (note_design_param_t *)p;
    gui_list_note_t *note = (gui_list_note_t *)obj;
    uint16_t index = note->index;
    if (index < sizeof(text_array) / sizeof(text_array[0]) - 1)
    {
        gui_img_t *img = gui_img_create_from_mem(note, 0, design_p->img_data_array[index], 10, 0, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        char *text = (char *)text_array[index];
        int font_size = 42;
        gui_text_t *t = gui_text_create(note, "txt", 140, 0, 300, 100);
        gui_text_set(t, text, GUI_FONT_SRC_TTF, APP_COLOR_WHITE, strlen(text), font_size);
        gui_text_type_set(t, SF_COMPACT_REGULAR_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, MID_LEFT);
        gui_text_rendermode_set(t, 2);
        if (design_p->click_cb[index] != NULL)
        {
            gui_obj_add_event_cb(note, (gui_event_cb_t)design_p->click_cb[index], GUI_EVENT_TOUCH_CLICKED,
                                 NULL);
        }
    }
    else
    {
        gui_img_t *img = gui_img_create_from_mem(note, 0, CELLULAR_MENU_CARD_BIN, 87, 0, 0, 0);
        gui_obj_add_event_cb(img, (gui_event_cb_t)switch_menu_style_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
    }
}
static void cellular_timer_cb(void)
{
    cellular_offset_x = menu_cellular->hor_offset;
    cellular_offset_y = menu_cellular->ver_offset;
}
static void canvas_timer_cb(void *p)
{
    gui_obj_t *obj = (gui_obj_t *)p;
    matrix_translate((float)menu_cellular->hor_offset, (float)menu_cellular->ver_offset, obj->matrix);

    cellular_timer_cb();
}
static void list_timer_cb(void *obj)
{
    gui_list_t *list = (gui_list_t *)obj;
    gui_obj_t *scrollbar_bg = gui_list_entry(GUI_BASE(obj)->parent->child_list.prev, gui_obj_t,
                                             brother_list);
    gui_obj_t *scrollbar = gui_list_entry(scrollbar_bg->child_list.next, gui_obj_t,
                                          brother_list);
    static uint8_t cnt = 0;
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
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
static void app_menu_design(gui_view_t *view)
{
    gui_win_t *win = gui_win_create(view, "win_app_menu", 0, 0, 0, 0);
    gui_view_switch_on_event(view, "watchface_view", SWITCH_OUT_ANIMATION_FADE,
                             SWITCH_IN_ANIMATION_FADE,
                             GUI_EVENT_KB_SHORT_CLICKED);
    if (menu_style == 0)
    {
        void *click_cb[] =
        {
            switch_app_activity,
            switch_app_heart_rate,
            switch_app_weather,
            switch_app_reminders,
            switch_app_message,
            switch_app_phone,
            switch_app_noise,
            switch_app_music,
            switch_app_map,
            switch_app_stopwatch,
            switch_app_timers,
            switch_app_alarm_clock,
            switch_app_audio_books,
            switch_app_workout,
            switch_app_calendar,
            switch_app_compass,
            switch_app_home,
            switch_app_contacts,
            switch_app_cycle_tracking,
            switch_app_mail,
            switch_app_now_playing,
            switch_app_news,
            switch_app_photo,
            switch_app_podcasts,
            switch_app_sleep,
            switch_app_stocks,
            switch_app_world_clock,
            switch_app_store,

            // switch_menu_style_cb, //must last
        };
        uint32_t *img_data_array[] =
        {
            APP_ACTIVITY_ICON_BIN,
            APP_HEART_RATE_ICON_BIN,
            APP_WEATHER_ICON_BIN,
            APP_REMINDERS_ICON_BIN,
            APP_MESSAGE_ICON_BIN,
            APP_PHONE_ICON_BIN,
            APP_NOISE_ICON_BIN,
            APP_MUSIC_ICON_BIN,
            APP_MAP_ICON_BIN,
            APP_STOPWATCH_ICON_BIN,
            APP_TIMERS_ICON_BIN,
            APP_ALARM_CLOCK_ICON_BIN,
            APP_AUDIO_BOOKS_ICON_BIN,
            APP_WORKOUT_ICON_BIN,
            APP_CALENDAR_ICON_BIN,
            APP_COMPASS_ICON_BIN,
            APP_HOME_ICON_BIN,
            APP_CONTACTS_ICON_BIN,
            APP_CYCLE_TRACKING_ICON_BIN,
            APP_MAIL_ICON_BIN,
            APP_NOW_PLAYING_ICON_BIN,
            APP_NEWS_ICON_BIN,
            APP_PHOTO_ICON_BIN,
            APP_PODCASTS_ICON_BIN,
            APP_SLEEP_ICON_BIN,
            APP_STOCKS_ICON_BIN,
            APP_WORLD_CLOCK_ICON_BIN,
            APP_STORE_ICON_BIN,
        };
        int array_size = sizeof(img_data_array) / sizeof(img_data_array[0]);

        design_p = gui_malloc(sizeof(note_design_param_t));
        void **func_cb = gui_malloc(array_size * sizeof(void *));
        memset(func_cb, 0, array_size * sizeof(void *));
        memcpy(func_cb, click_cb, sizeof(click_cb));
        uint32_t **data_array = gui_malloc(array_size * sizeof(uint32_t *));
        memcpy(data_array, img_data_array, array_size * sizeof(void *));
        design_p->click_cb = func_cb;
        design_p->img_data_array = data_array;

        int length = 100;
        uint8_t space = 5;
        gui_list_t *list = gui_list_create(view, "list", 0, 0, 0, 0, length, space,
                                           VERTICAL, note_design, design_p, 0);
        gui_list_set_style(list, LIST_CIRCLE);
        gui_list_set_note_num(list, array_size);
        gui_list_set_offset(list, list_offset_his);
        gui_list_set_out_scope(list, 50);
        gui_obj_create_timer(GUI_BASE(list), 10, true, list_timer_cb);

        gui_img_t *scrollbar_bg = gui_img_create_from_mem(view, 0, SCROLLBAR_BG_BIN, 395, 30, 0, 0);
        gui_img_set_mode(scrollbar_bg, IMG_SRC_OVER_MODE);
        gui_img_t *scrollbar = gui_img_create_from_mem(scrollbar_bg, 0, SCROLLBAR_S_BIN, -1, 0, 0, 0);
        gui_img_set_mode(scrollbar, IMG_SRC_OVER_MODE);
        gui_obj_hidden((void *)scrollbar_bg, true);
        gui_obj_hidden((void *)scrollbar, true);
    }
    else
    {
        uint32_t *cellilar_img_data_array[] =
        {
            APP_ACTIVITY_ICON_BIN,
            APP_HEART_RATE_ICON_BIN,
            APP_WEATHER_ICON_BIN,
            APP_REMINDERS_ICON_BIN,
            APP_MESSAGE_ICON_BIN,
            APP_PHONE_ICON_BIN,
            APP_NOISE_ICON_BIN,
            APP_MUSIC_ICON_BIN,
            APP_MAP_ICON_BIN,
            APP_STOPWATCH_ICON_BIN,
            APP_TIMERS_ICON_BIN,
            APP_ALARM_CLOCK_ICON_BIN,
            APP_AUDIO_BOOKS_ICON_BIN,
            APP_WORKOUT_ICON_BIN,
            APP_CALENDAR_ICON_BIN,
            APP_COMPASS_ICON_BIN,
            APP_HOME_ICON_BIN,
            APP_CONTACTS_ICON_BIN,
            APP_CYCLE_TRACKING_ICON_BIN,
            APP_MAIL_ICON_BIN,
            APP_NOW_PLAYING_ICON_BIN,
            APP_NEWS_ICON_BIN,
            APP_PHOTO_ICON_BIN,
            APP_PODCASTS_ICON_BIN,
            APP_SLEEP_ICON_BIN,
            APP_STOCKS_ICON_BIN,
            APP_WORLD_CLOCK_ICON_BIN,
            APP_STORE_ICON_BIN,
        };
        int array_size = sizeof(cellilar_img_data_array) / sizeof(cellilar_img_data_array[0]);
        int16_t icon_size = 100;
        gui_canvas_rect_create(GUI_BASE(win), "background", 0, 0,
                               SCREEN_WIDTH, SCREEN_HEIGHT, gui_rgba(0, 0, 0, 255));
        gui_menu_cellular_t *menu = gui_menu_cellular_create(win, icon_size, cellilar_img_data_array,
                                                             array_size);
        menu_cellular = menu;
        menu_cellular->ver_offset_min -= icon_size;
        gui_menu_cellular_offset(menu, cellular_offset_x, cellular_offset_y);

        {
            struct gui_menu_cellular_gesture_parameter gesture_parameter_array[] =
            {
                {switch_app_activity, NULL}, {switch_app_heart_rate, NULL}, {switch_app_weather, NULL}, {switch_app_reminders, NULL},
                {switch_app_message, NULL}, {switch_app_phone, NULL}, {switch_app_noise, NULL}, {switch_app_music, NULL},
                {switch_app_map, NULL}, {switch_app_stopwatch, NULL}, {switch_app_timers, NULL}, {switch_app_alarm_clock, NULL},
                {switch_app_audio_books, NULL}, {switch_app_workout, NULL}, {switch_app_calendar, NULL},
                {switch_app_compass, NULL}, {switch_app_home, NULL}, {switch_app_contacts, NULL}, {switch_app_cycle_tracking, NULL},
                {switch_app_mail, NULL}, {switch_app_now_playing, NULL},
                {switch_app_news, NULL}, {switch_app_photo, NULL}, {switch_app_podcasts, NULL}, {switch_app_sleep, NULL},
                {switch_app_stocks, NULL}, {switch_app_world_clock, NULL}, {switch_app_store, NULL},
            };
            gui_menu_cellular_on_click(menu, gesture_parameter_array,
                                       sizeof(gesture_parameter_array) / sizeof(gesture_parameter_array[0]));
        }
        {
            int h = array_size / 7 * icon_size * 2 + (array_size % 7) / 3 * icon_size + 50;
            gui_win_t *win_canvas = gui_win_create(view, "win_canvas", 125, h, 200, 100);
            gui_img_t *img = gui_img_create_from_mem(win_canvas, 0, LIST_MENU_CARD_BIN, 0, 0, 0, 0);
            gui_obj_create_timer(GUI_BASE(win_canvas), 10, true, canvas_timer_cb);
            gui_obj_add_event_cb(img, (gui_event_cb_t)switch_menu_style_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
        }

    }
}
