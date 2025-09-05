/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "gui_win.h"
#include "gui_api.h"
#include "root_image/ui_resource.h"
#include "guidef.h"
#include "gui_img.h"
#include "gui_text.h"
#include "gui_list.h"
#include "gui_canvas_rect.h"
#include "gui_canvas_round_rect.h"
#include "app_main_watch.h"
#include "watch_adapt.h"

/*============================================================================*
 *                           Types
 *============================================================================*/
typedef struct record_information
{
    char name[8];
    uint16_t time;
} record_infor_t;

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define SCREEN_WIDTH  (int16_t)gui_get_screen_width()
#define SCREEN_HEIGHT (int16_t)gui_get_screen_height()
#define CURRENT_VIEW_NAME "recorder_view"

#define RECORD_NUM_MAX 20

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void create_recorder(gui_view_t *view);

/*============================================================================*
 *                            Variables
 *============================================================================*/
static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *activity_view = NULL;
const static gui_view_descriptor_t *call_dial_view = NULL;
static const gui_view_descriptor_t descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = create_recorder,
    .keep = false,
};

static gui_win_t *win_recorder = NULL;
static uint32_t record_time = 0;
static char record_time_str[10] = "00:00:00";

static gui_win_t *win_play = NULL;
static uint8_t record_file_num = 10;
static uint8_t play_index = 0;
static uint32_t play_time = 0;
static char play_time_str[10] = "00:00:00";
static char file_time_str[10] = "00:00:00";
static char play_file_name[10] = {0};
static record_infor_t record_infor[RECORD_NUM_MAX] =
{
    {"File_1", 230},
    {"File_2", 60},
    {"File_3", 180},
    {"File_4", 240},
    {"File_5", 360},
    {"File_6", 420},
    {"File_7", 150},
    {"File_8", 270},
    {"File_9", 330},
    {"File_10", 390},
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
    call_dial_view = gui_view_descriptor_get("call_dial_view");
    activity_view = gui_view_descriptor_get("activity_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

static void format_time(uint32_t seconds, char *str)
{
    uint32_t h = seconds / 3600;
    uint32_t m = (seconds % 3600) / 60;
    uint32_t s = seconds % 60;
    sprintf(str, "%02u:%02u:%02u", h, m, s);
}

static void click_to_recorder_cb(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    gui_obj_hidden(GUI_BASE(win_recorder), false);
    gui_obj_tree_free_async(win_play);

    GUI_WIDGET_POINTER_BY_NAME_ROOT(o, "img_play", win_play)
    gui_img_t *img = GUI_TYPE(gui_img_t, o);
    if (img->data == RECORD_PAUSE_BIN)
    {
        img->data = RECORD_STOP_BIN;
        gui_obj_stop_timer(GUI_BASE(o));
        gui_audio_t *gui_audio = gui_get_audio();
        if (gui_audio->record_pause)
        {
            gui_audio->record_pause();
        }
    }
}

static void click_canvas_cb(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    return;
}

static void click_play_cb(void *obj, gui_event_t e, void *param)
{
    (void)e;
    (void)param;
    if (record_file_num == 0) { return; }

    gui_img_t *img = GUI_TYPE(gui_img_t, obj);
    gui_audio_t *gui_audio = gui_get_audio();
    if (img->data == RECORD_PLAY_BIN)
    {
        img->data = RECORD_PAUSE_BIN;
        gui_obj_start_timer(GUI_BASE(obj));
        if (gui_audio->record_play)
        {
            gui_audio->record_play();
        }
    }
    else
    {
        img->data = RECORD_PLAY_BIN;
        gui_obj_stop_timer(GUI_BASE(obj));
        if (gui_audio->record_pause)
        {
            gui_audio->record_pause();
        }
    }
}

static void on_playing(void *p)
{
    play_time += 1;
    gui_img_t *img = GUI_TYPE(gui_img_t, p);
    gui_audio_t *gui_audio = gui_get_audio();
    uint32_t duration = record_infor[play_index].time;
    uint32_t time = play_time;

    format_time(time, play_time_str);
    GUI_WIDGET_POINTER_BY_NAME_ROOT(o, "play_time", win_play)
    gui_text_t *t = GUI_TYPE(gui_text_t, o);
    gui_text_content_set(t, play_time_str, strlen(play_time_str));

    GUI_WIDGET_POINTER_BY_NAME_ROOT(canvas, "time_bar", win_play)
    gui_canvas_rect_set_size((gui_canvas_rect_t *)canvas, 300 * time / duration, 3);

    bool completion_status = false;
    if (gui_audio->record_completion_status)
    {
        completion_status = gui_audio->record_completion_status();
    }
    else
    {
        completion_status = (play_time >= record_infor[play_index].time);
    }
    if (completion_status)
    {
        img->data = RECORD_PLAY_BIN;
        gui_obj_stop_timer(GUI_BASE(img));
        if (gui_audio->record_pause)
        {
            gui_audio->record_pause();
        }
    }
}

static void win_scroll_cb(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    // Disable view horizontal scroll
}

static void click_file_play(void *obj, gui_event_t e, void *param)
{
    (void)e;
    (void)param;
    gui_list_note_t *note = (gui_list_note_t *)GUI_BASE(obj)->parent;
    gui_audio_t *gui_audio = gui_get_audio();
    GUI_WIDGET_POINTER_BY_NAME_ROOT(o, "img_play", win_play)
    gui_img_t *img = GUI_TYPE(gui_img_t, o);
    if (play_index != note->index)
    {
        play_index = note->index;
        play_time = 0;
        if (gui_audio->record_play)
        {
            gui_audio->record_play();
        }
        img->data = RECORD_PAUSE_BIN;
        GUI_WIDGET_POINTER_BY_NAME_ROOT(canvas, "time_bar", win_play)
        gui_canvas_rect_set_size((gui_canvas_rect_t *)canvas, 1, 3);
        gui_obj_start_timer(o);

        {
            GUI_WIDGET_POINTER_BY_NAME_ROOT(o, "file_name", win_play)
            gui_text_t *t = GUI_TYPE(gui_text_t, o);
            sprintf(play_file_name, "%s", record_infor[play_index].name);
            gui_text_content_set(t, play_file_name, strlen(play_file_name));
        }
        {
            GUI_WIDGET_POINTER_BY_NAME_ROOT(o, "file_time", win_play)
            gui_text_t *t = GUI_TYPE(gui_text_t, o);
            format_time(record_infor[play_index].time, file_time_str);
            gui_text_content_set(t, file_time_str, strlen(file_time_str));
        }
        {
            GUI_WIDGET_POINTER_BY_NAME_ROOT(o, "play_time", win_play)
            gui_text_t *t = GUI_TYPE(gui_text_t, o);
            format_time(play_time, play_time_str);
            gui_text_content_set(t, play_time_str, strlen(play_time_str));
        }
    }
}

static void list_design(gui_obj_t *obj, void *p)
{
    (void)p;
    gui_list_note_t *note = GUI_TYPE(gui_list_note_t, obj);
    uint8_t index = note->index;
    if (index >= record_file_num) { return;}
    gui_canvas_round_rect_t *rect = gui_canvas_round_rect_create(obj, 0, 55, 0, 300, 50, 10, gui_rgb(60,
                                                                 150, 255));

    // gui_img_t *img_file = gui_img_create_from_mem(rect, 0, RECORD_FILE_BIN, 10, 10, 0, 0);
    gui_text_t *text = gui_text_create(rect, 0, 0, 0, 300, 50);
    gui_text_set(text, record_infor[index].name, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                 strlen(record_infor[index].name), 32);
    gui_text_type_set(text, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, MID_CENTER);
    gui_text_rendermode_set(text, 2);

    gui_obj_add_event_cb(rect, click_file_play, GUI_EVENT_TOUCH_CLICKED, rect);
}

static void create_recorder_play(void)
{
    gui_obj_hidden(GUI_BASE(win_recorder), true);

    if (play_index > record_file_num)
    {
        play_index = 0;
        play_file_name[0] = '\0';
    }

    if (record_file_num != 0)
    {
        sprintf(play_file_name, "%s", record_infor[play_index].name);
        format_time(record_infor[play_index].time, file_time_str);
    }

    win_play = gui_win_create(current_view, "win_play", 0, 0, 0, 0);
    gui_obj_add_event_cb(GUI_BASE(win_play), win_scroll_cb, GUI_EVENT_TOUCH_SCROLL_HORIZONTAL,
                         NULL); // Disable view horizontal scroll
    gui_obj_add_event_cb(GUI_BASE(win_play), win_scroll_cb, GUI_EVENT_TOUCH_SCROLL_VERTICAL,
                         NULL); // Disable view vertical scroll

    gui_list_t *list = gui_list_create(win_play, 0, 0, 80, 0, 0, 50, 5, VERTICAL, list_design, NULL,
                                       false);
    gui_list_set_note_num(list, record_file_num > 5 ? record_file_num + 3 : record_file_num);
    gui_list_set_style(list, LIST_CLASSIC);

    gui_canvas_rect_t *canvas_top = gui_canvas_rect_create(GUI_BASE(win_play), 0, 0, 0, SCREEN_WIDTH,
                                                           80, APP_COLOR_BLACK);
    gui_obj_add_event_cb(canvas_top, click_canvas_cb, GUI_EVENT_TOUCH_CLICKED,
                         NULL); // Disable click event of this area
    gui_text_t *text = gui_text_create(canvas_top, 0, 0, 20, SCREEN_WIDTH, 60);
    gui_text_set(text, "Recorder files", GUI_FONT_SRC_TTF, APP_COLOR_WHITE, strlen("Recorder files"),
                 48);
    gui_text_type_set(text, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, CENTER);
    gui_text_rendermode_set(text, 2);
    gui_img_t *img_ret = gui_img_create_from_mem(canvas_top, "img_ret", UI_RETURN_W_ICON_BIN, 60, 25, 0,
                                                 0);
    gui_img_set_quality(img_ret, true);
    gui_obj_add_event_cb(img_ret, click_to_recorder_cb, GUI_EVENT_TOUCH_CLICKED, NULL);

    gui_canvas_rect_t *canvas_bottom = gui_canvas_rect_create(GUI_BASE(win_play), 0, 0,
                                                              SCREEN_HEIGHT - 150, SCREEN_WIDTH, 150, APP_COLOR_BLACK);
    gui_obj_add_event_cb(canvas_bottom, click_canvas_cb, GUI_EVENT_TOUCH_CLICKED,
                         NULL); // Disable click event of this area
    gui_img_t *img_play = gui_img_create_from_mem(canvas_bottom, "img_play", RECORD_PLAY_BIN, 165, 55,
                                                  0, 0);
    gui_obj_add_event_cb(img_play, click_play_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_obj_create_timer(GUI_BASE(img_play), 1000, true, on_playing);
    gui_obj_stop_timer(GUI_BASE(img_play));
    gui_canvas_rect_create(GUI_BASE(canvas_bottom), 0, 55, 40, 300, 3, gui_rgb(150, 150, 150));
    gui_canvas_rect_create(GUI_BASE(canvas_bottom), "time_bar", 55, 40, 1, 3, APP_COLOR_WHITE);

    text = gui_text_create(canvas_bottom, "file_name", 55, 5, 0, 0);
    gui_text_set(text, play_file_name, GUI_FONT_SRC_TTF, APP_COLOR_WHITE, strlen(play_file_name), 32);
    gui_text_type_set(text, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);
    gui_text_rendermode_set(text, 2);
    text = gui_text_create(canvas_bottom, "play_time", 55, 45, 0, 0);
    gui_text_set(text, play_time_str, GUI_FONT_SRC_TTF, APP_COLOR_WHITE, strlen(play_time_str), 32);
    gui_text_type_set(text, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);
    gui_text_rendermode_set(text, 2);
    text = gui_text_create(canvas_bottom, "file_time", 275, 45, 0, 0);
    gui_text_set(text, file_time_str, GUI_FONT_SRC_TTF, APP_COLOR_WHITE, strlen(file_time_str), 32);
    gui_text_type_set(text, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);
    gui_text_rendermode_set(text, 2);
}

static void click_record_cb(void *obj, gui_event_t e, void *param)
{
    (void)e;
    (void)param;
    gui_img_t *img = GUI_TYPE(gui_img_t, obj);
    gui_audio_t *gui_audio = gui_get_audio();
    GUI_WIDGET_POINTER_BY_NAME_ROOT(o, "record_time", win_recorder)
    if (img->data == RECORD_START_BIN)
    {
        sprintf(record_time_str, "00:00:00");
        record_time = 0;
        img->data = RECORD_STOP_BIN;
        gui_obj_start_timer(GUI_BASE(o));
        if (gui_audio->record_start)
        {
            gui_audio->record_start();
        }
    }
    else
    {
        img->data = RECORD_START_BIN;
        gui_obj_stop_timer(GUI_BASE(o));
        if (gui_audio->record_stop)
        {
            gui_audio->record_stop();
        }
    }
}

static void click_enter_file_win_cb(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    GUI_WIDGET_POINTER_BY_NAME_ROOT(o, "img_record", win_recorder)

    gui_img_t *img = GUI_TYPE(gui_img_t, o);
    if (img->data == RECORD_STOP_BIN)
    {
        img->data = RECORD_START_BIN;
        gui_audio_t *gui_audio = gui_get_audio();
        if (gui_audio->record_stop)
        {
            gui_audio->record_stop();
        }
        GUI_WIDGET_POINTER_BY_NAME_ROOT(text, "record_time", win_recorder)
        sprintf(record_time_str, "00:00:00");
        record_time = 0;
        gui_obj_stop_timer(GUI_BASE(text));
    }
    create_recorder_play();
}

static void record_time_update(void *p)
{
    gui_text_t *text = (gui_text_t *)p;
    record_time++;
    format_time(record_time, record_time_str);
    gui_text_content_set(text, record_time_str, strlen(record_time_str));
}

static void create_recorder(gui_view_t *view)
{
    // view layout
    gui_view_switch_on_event(view, activity_view, SWITCH_OUT_TO_RIGHT_USE_ROTATE,
                             SWITCH_IN_FROM_LEFT_USE_ROTATE,
                             GUI_EVENT_TOUCH_MOVE_RIGHT);
    gui_view_switch_on_event(view, call_dial_view, SWITCH_OUT_TO_LEFT_USE_ROTATE,
                             SWITCH_IN_FROM_RIGHT_USE_ROTATE,
                             GUI_EVENT_TOUCH_MOVE_LEFT);

    // recorder
    win_recorder = gui_win_create(view, "win_recorder", 0, 0, 0, 0);
    record_time = 0;
    format_time(record_time, record_time_str);

    gui_text_t *text = gui_text_create(win_recorder, 0, 0, 20, SCREEN_WIDTH, 60);
    gui_text_set(text, "Recorder", GUI_FONT_SRC_TTF, APP_COLOR_WHITE, strlen("Recorder"), 48);
    gui_text_type_set(text, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, CENTER);
    gui_text_rendermode_set(text, 2);

    gui_img_t *img_recorder = gui_img_create_from_mem(win_recorder, "img_record", RECORD_START_BIN, 105,
                                                      120, 0, 0);
    gui_img_set_quality(img_recorder, true);
    gui_obj_add_event_cb(img_recorder, click_record_cb, GUI_EVENT_TOUCH_CLICKED, NULL);

    text = gui_text_create(win_recorder, "record_time", 0, 340, SCREEN_WIDTH, 50);
    gui_text_set(text, record_time_str, GUI_FONT_SRC_TTF, APP_COLOR_WHITE, strlen(record_time_str), 48);
    gui_text_type_set(text, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, CENTER);
    gui_text_rendermode_set(text, 2);
    gui_obj_create_timer(GUI_BASE(text), 1000, true, record_time_update);
    gui_obj_stop_timer(GUI_BASE(text));

    gui_canvas_round_rect_t *canvas = gui_canvas_round_rect_create(GUI_BASE(win_recorder), 0, 105, 410,
                                                                   200, 50, 10, gui_rgb(70, 80, 90));
    gui_obj_add_event_cb(canvas, click_enter_file_win_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
    // gui_img_t *img_file = gui_img_create_from_mem(canvas, 0, RECORD_FILE_BIN, 10, 10, 0, 0);
    text = gui_text_create(canvas, 0, 0, 0, 200, 50);
    gui_text_set(text, "Files", GUI_FONT_SRC_TTF, APP_COLOR_WHITE, strlen("Files"), 32);
    gui_text_type_set(text, SOURCEHANSANSSC_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, MID_CENTER);
    gui_text_rendermode_set(text, 2);
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
void local_record_stop(void)
{
    if (record_file_num < RECORD_NUM_MAX && record_time > 0)
    {
        sprintf(record_infor[record_file_num].name, "File_%d", record_file_num + 1);
        record_infor[record_file_num].time = record_time;
        record_file_num++;
    }
}
