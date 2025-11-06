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

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void app_menu_design(gui_view_t *view);
static void clear_menu(gui_view_t *view);
static void switch_app_activity(void *obj, gui_event_t e, void *param);
static void switch_app_heart_rate(void *obj, gui_event_t e, void *param);

/*============================================================================*
 *                            Variables
 *============================================================================*/
static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *watchface_view = NULL;
const static gui_view_descriptor_t *activity_view = NULL;
const static gui_view_descriptor_t *heartrate_view = NULL;

static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = app_menu_design,
    .on_switch_out = clear_menu,
};

static note_design_param_t *design_p = NULL;
static int16_t list_offset_his = 0;

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
static int gui_view_descriptor_register_init(void)
{
    gui_view_descriptor_register(&descriptor);
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_REGISTER(gui_view_descriptor_register_init);

static int gui_view_get_other_view_descriptor_init(void)
{
    /* you can get other view descriptor point here */
    watchface_view = gui_view_descriptor_get("watchface_view");
    activity_view = gui_view_descriptor_get("activity_view");
    heartrate_view = gui_view_descriptor_get("heartrate_view");

    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

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
    gui_view_switch_direct(current_view, activity_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}

static void switch_app_heart_rate(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    gui_view_switch_direct(current_view, heartrate_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}

static void note_design(gui_obj_t *obj, void *p)
{
    note_design_param_t *design_p = (note_design_param_t *)p;
    gui_list_note_t *note = (gui_list_note_t *)obj;
    uint16_t index = note->index;
    if (index < sizeof(text_array) / sizeof(text_array[0]))// - 1)
    {
        gui_img_t *img = gui_img_create_from_mem(note, 0, design_p->img_data_array[index], 10, 0, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        char *text = (char *)text_array[index];
        int font_size = 42;
        gui_text_t *t = gui_text_create(note, "txt", 140, 0, 300, 120);
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
        // gui_img_t *img = gui_img_create_from_mem(note, 0, CELLULAR_MENU_CARD_BIN, 87, 0, 0, 0);
        // gui_obj_add_event_cb(img, (gui_event_cb_t)switch_menu_style_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
    }
}

static void list_timer_cb(void *obj)
{
    gui_list_t *list = (gui_list_t *)obj;
    list_offset_his = list->offset;
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
static void app_menu_design(gui_view_t *view)
{
    gui_view_switch_on_event(view, watchface_view, SWITCH_OUT_ANIMATION_FADE,
                             SWITCH_IN_ANIMATION_FADE,
                             GUI_EVENT_KB_SHORT_CLICKED);
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
    void *click_cb[] =
    {
        switch_app_activity,
        switch_app_heart_rate,
    };
    design_p = gui_malloc(sizeof(note_design_param_t));
    void **func_cb = gui_malloc(array_size * sizeof(void *));
    memset(func_cb, 0, array_size * sizeof(void *));
    memcpy(func_cb, click_cb, sizeof(click_cb));
    uint32_t **data_array = gui_malloc(array_size * sizeof(uint32_t *));
    memcpy(data_array, img_data_array, array_size * sizeof(void *));
    design_p->click_cb = func_cb;
    design_p->img_data_array = data_array;

    int length = 120;
    uint8_t space = 5;
    gui_list_t *list = gui_list_create(view, "list", 0, 0, 0, 0, length, space,
                                       VERTICAL, note_design, design_p, 1);
    gui_list_set_style(list, LIST_CIRCLE);
    gui_list_set_note_num(list, array_size);
    gui_list_set_offset(list, list_offset_his);
    gui_list_set_out_scope(list, 50);
    gui_list_set_bar_color(list, APP_COLOR_SILVER);
    gui_obj_create_timer(GUI_BASE(list), 10, true, list_timer_cb);
}