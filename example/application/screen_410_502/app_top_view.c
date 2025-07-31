/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "root_image/ui_resource.h"
#include "app_main_watch.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_text.h"
#include "tp_algo.h"
#include "gui_canvas_rect.h"
#include "gui_canvas_round_rect.h"
#include "guidef.h"
#include "gui_list.h"
#include "app_main_watch.h"

/*============================================================================*
 *                           Types
 *============================================================================*/

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define SCREEN_WIDTH (int16_t)gui_get_screen_width()
#define SCREEN_HEIGHT (int16_t)gui_get_screen_height()
#define NOTE_HEIGHT 220
#define NOTE_INTERVAL 20
#define NOTE_START 140
#define INFOR_NUM_MAX 4
#define CURRENT_VIEW_NAME "app_top_view"

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void top_view_design(gui_view_t *view);

/*============================================================================*
 *                            Variables
 *============================================================================*/
static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *watchface_view = NULL;
static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = top_view_design,
    .keep = false,
};

static gui_list_t *list;
static gui_canvas_round_rect_t *canvas_clear;
static information_t *infor_rec[INFOR_NUM_MAX];
static uint8_t infor_num = 0;
static bool clear_flag = false; // 1: not create new information note
static bool in_view_more = false; // 1: not create new information note
static bool note_dur_animation = false; // 1: not create new information note
static uint16_t view_more_cnt = 0; // for view_more animation
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
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

static void cancel_cb(void *p)
{
    gui_win_t *win = (gui_win_t *)(GUI_BASE(p)->parent);
    uint16_t count_max = 300;
    view_more_cnt += win->base.timer->interval_ms;
    float percent = view_more_cnt / (float)count_max;
    win->base.x = -percent * SCREEN_WIDTH;

    if (percent >= 1.0f)
    {
        gui_obj_stop_timer(GUI_BASE(win));
        gui_obj_tree_free_async(win);
        in_view_more = false;
        view_more_cnt = 0;
    }
}

static void view_more_cb(void *p)
{
    // enter animation
    gui_win_t *win = (gui_win_t *)p;
    uint16_t count_max = 300;
    view_more_cnt += win->base.timer->interval_ms;
    float percent = 1 - view_more_cnt / (float)count_max;
    if (win->base.x < -50)
    {
        win->base.x = -percent * SCREEN_WIDTH;
    }
    else
    {
        win->base.x = 0;
        view_more_cnt = 0;
        gui_obj_stop_timer(GUI_BASE(win));
    }
}

static void canvas_color_change_cb(void *p)
{
    gui_canvas_round_rect_set_color((gui_canvas_round_rect_t *)p, gui_rgb(39, 43, 44));
}

static void view_more_click_cb(void *widget, gui_event_t e, void *param)
{
    gui_obj_t *obj = GUI_BASE(widget);
    if (strcmp(obj->name, "cancel") == 0)
    {
        gui_obj_create_timer(obj, 20, true, cancel_cb);
        return;
    }
    gui_canvas_round_rect_set_color((gui_canvas_round_rect_t *)obj, APP_COLOR_WHITE);
    gui_obj_create_timer(obj, 10, false, canvas_color_change_cb);
    gui_obj_start_timer(obj);
}

static void view_more_event_cb(void *widget, gui_event_t e, void *param)
{
    ; // This function is intentionally left empty.
}

static void create_view_more(void *obj, gui_event_t e, void *param)
{
    if (in_view_more) {return;}
    in_view_more = true;
    information_t *inform = (information_t *)param;
    const char *informer = inform->informer;
    const char *content = inform->content;
    const char *time = inform->time;
    app_name app = inform->app;

    gui_win_t *win = gui_win_create(current_view, "win_view_more", -SCREEN_WIDTH, 0, SCREEN_WIDTH,
                                    SCREEN_HEIGHT);
    gui_obj_create_timer(GUI_BASE(win), 20, true, view_more_cb);
    gui_obj_add_event_cb(GUI_BASE(win), (gui_event_cb_t)view_more_event_cb,
                         GUI_EVENT_TOUCH_SCROLL_VERTICAL, NULL); // stop list scroll
    gui_canvas_rect_t *canvas_bg = gui_canvas_rect_create(GUI_BASE(win), 0, 0, 0,
                                                          SCREEN_WIDTH,
                                                          SCREEN_HEIGHT, gui_rgb(0, 0, 0));
    {
        char *content = "Cancel";
        gui_text_t *text = gui_text_create(win, "cancel",  37, 35, 80, 32);
        gui_text_set(text, (void *)content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                     strlen(content),
                     32);
        gui_text_type_set(text, SOURCEHANSANSSC_SIZE32_BITS1_FONT_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, LEFT);
        gui_obj_add_event_cb(GUI_BASE(text), view_more_click_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
    }

    gui_text_t *text = gui_text_create(win, "time", -60, 35, 0, 0);
    gui_text_set(text, (void *)(time), GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(time),
                 32);
    gui_text_type_set(text, SOURCEHANSANSSC_SIZE32_BITS1_FONT_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, RIGHT);

    switch (app)
    {
    case MESSAGE:
        {
            char name[40];
            sprintf(name, "\"message\" dialog\n%s", informer);
            text = gui_text_create(win, "message",  37, 90, 0, 0);
            gui_text_set(text, (void *)name, GUI_FONT_SRC_BMP, gui_rgb(153, 153, 153),
                         strlen(name),
                         24);
            gui_text_type_set(text, SOURCEHANSANSSC_SIZE24_BITS1_FONT_BIN, FONT_SRC_MEMADDR);
            gui_text_mode_set(text, MULTI_LEFT);
            gui_img_create_from_mem(win, 0, UI_PERSON_ICON_BIN, 360, 40, 0, 0);
        }
        break;
    case OS:
        {
            content = "\"OS\" dialog" ;
            text = gui_text_create(win, 0, 37, 120, 0, 0);
            gui_text_set(text, (void *)content, GUI_FONT_SRC_BMP, gui_rgb(153, 153, 153),
                         strlen(content),
                         24);
            gui_text_type_set(text, SOURCEHANSANSSC_SIZE24_BITS1_FONT_BIN, FONT_SRC_MEMADDR);
            gui_text_mode_set(text, LEFT);
            gui_img_t *img = gui_img_create_from_mem(win, 0, UI_IWATCH_ICON_BIN, 360, 40, 0, 0);
            gui_img_scale(img, 0.75f, 0.75f);
        }
        break;
    default:
        break;
    }

    // options
    gui_canvas_round_rect_t *canvas = gui_canvas_round_rect_create(GUI_BASE(win), "canvas_1",
                                                                   30, 160, 350, 80, 15, gui_rgb(39, 43, 44));
    content = "Don't remind for an hour";
    text = gui_text_create(canvas, "text1",  0, 24, 0, 0);
    gui_text_set(text, (void *)content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(content),
                 32);
    gui_text_type_set(text, SOURCEHANSANSSC_SIZE32_BITS1_FONT_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, CENTER);
    gui_obj_add_event_cb(GUI_BASE(canvas), view_more_click_cb, GUI_EVENT_TOUCH_CLICKED, NULL);

    canvas = gui_canvas_round_rect_create(GUI_BASE(win), "canvas_2", 30, 250, 350, 80, 15,
                                          gui_rgb(39, 43, 44));
    content = "Don't remind today";
    text = gui_text_create(canvas, "text1",  0, 24, 0, 0);
    gui_text_set(text, (void *)content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(content),
                 32);
    gui_text_type_set(text, SOURCEHANSANSSC_SIZE32_BITS1_FONT_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, CENTER);
    gui_obj_add_event_cb(GUI_BASE(canvas), view_more_click_cb, GUI_EVENT_TOUCH_CLICKED, NULL);

    canvas = gui_canvas_round_rect_create(GUI_BASE(win), "canvas_3", 30, 340, 350, 80, 15,
                                          gui_rgb(39, 43, 44));
    content = "Add this to Summary";
    text = gui_text_create(canvas, "text1",  0, 24, 0, 0);
    gui_text_set(text, (void *)content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(content),
                 32);
    gui_text_type_set(text, SOURCEHANSANSSC_SIZE32_BITS1_FONT_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, CENTER);
    gui_obj_add_event_cb(GUI_BASE(canvas), view_more_click_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
}

static void clear_list_note(gui_list_note_t *note)
{
    gui_node_list_t *node = NULL;
    uint8_t index = 0;
    gui_list_for_each(node, &(list->base.child_list))
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        gui_list_note_t *list_note = (gui_list_note_t *)obj;
        if (list_note == note) {break;}

        list_note->start_y -= (list->note_length + list->space);
        list_note->index -= 1;
        gui_obj_move(obj, 0, list_note->start_y);
        index++;
    }
    gui_obj_tree_free_async(GUI_BASE(note));
    index = infor_num - 1 - index;
    gui_free(infor_rec[index]);
    // gui_log("free infor_rec[%d]\n", index);
    while (index < infor_num - 1)
    {
        infor_rec[index] = infor_rec[index + 1];
        infor_rec[index + 1] = NULL;
        index++;
    }
    infor_num--;
    gui_list_set_note_num(list, infor_num);
    clear_flag = false;
}

// animation after sliding note
static void note_timer_cb(void *widget)
{
    gui_obj_t *obj = GUI_BASE(widget);
    gui_list_note_t *note = (gui_list_note_t *)obj;
    uint8_t step = 30;
    if (abs(note->start_x - note->t_x) >= SCREEN_WIDTH)
    {
        // gui_log("clear note\n");
        clear_list_note(note);
        gui_obj_stop_timer(obj);
        note_dur_animation = false;
    }
    else if (note->t_x - note->start_x >= SCREEN_WIDTH / 2)
    {
        note->t_x += step;
        clear_flag = true;
    }
    else if (note->t_x - note->start_x <= -SCREEN_WIDTH / 2)
    {
        note->t_x -= step;
        clear_flag = true;
    }
    else if (note->t_x < note->start_x - step) {note->t_x += step;}
    else if (note->t_x > note->start_x + step) {note->t_x -= step;}
    else
    {
        note->t_x = note->start_x;
        gui_obj_stop_timer(obj);
        note_dur_animation = false;
    }
}

static void note_pressing_cb(void *widget)
{
    if (clear_flag || in_view_more) {return;}
    gui_obj_t *obj = GUI_BASE(widget);
    gui_list_note_t *note = (gui_list_note_t *)obj;
    struct touch_info *tp = tp_get_info();
    note->t_x = note->start_x + tp->deltaX;
    note_dur_animation = true;
}

static void note_released_cb(void *widget)
{
    if (clear_flag || in_view_more) {return;}
    gui_obj_t *obj = GUI_BASE(widget);
    gui_obj_create_timer(obj, 20, true, note_timer_cb);
    gui_obj_start_timer(obj);
}

static void create_inform_note(gui_obj_t *obj, void *param)
{
    gui_log("create_inform_note\n");
    uint16_t index = ((gui_list_note_t *)obj)->index;
    information_t *inform = ((information_t **)param)[index];
    const char *informer = inform->informer;
    const char *content = inform->content;
    const char *time = inform->time;
    app_name app = inform->app;

    gui_canvas_round_rect_t *canvas = gui_canvas_round_rect_create(obj, "bg", 30, 0, 350,
                                                                   220,
                                                                   35, gui_rgb(39, 43, 44));
    gui_obj_add_event_cb(obj, (gui_event_cb_t)create_view_more, GUI_EVENT_TOUCH_CLICKED,
                         (void *)inform);
    // message
    void *img_path = NULL;
    switch (app)
    {
    case MESSAGE:
        {
            img_path = UI_MESSAGE_ICON_BIN;
        }
        break;
    case OS:
        {
            img_path = UI_IWATCH_ICON_BIN;
        }
        break;
    default:
        break;
    }
    gui_img_create_from_mem(canvas, "message icon", img_path, 10, 0, 0, 0);

    // text
    gui_text_t *text_time = gui_text_create(canvas, "time",  -15, 15, 0, 0);
    gui_text_set(text_time, (void *)time, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(time),
                 24);
    gui_text_type_set(text_time, SOURCEHANSANSSC_SIZE24_BITS1_FONT_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text_time, RIGHT);

    gui_text_t *text_informer = gui_text_create(canvas, "informer",  10, 50, 0, 0);
    gui_text_set(text_informer, (void *)informer, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(informer),
                 32);
    gui_text_type_set(text_informer, SOURCEHANSANSSC_SIZE32_BITS1_FONT_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text_informer, LEFT);

    gui_text_t *text_content = gui_text_create(canvas, "content",  10, 75, 340, 0);
    uint8_t content_len = strlen(content);
    if (content_len > 100)
    {
        char *content_new = gui_malloc(105);
        strncpy(content_new, content, 100);
        strcat(content_new, " ...");
        gui_text_set(text_content, (void *)content_new, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                     strlen(content_new),
                     32);
    }
    else
    {
        gui_text_set(text_content, (void *)content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                     strlen(content),
                     32);
    }
    gui_text_type_set(text_content, SOURCEHANSANSSC_SIZE32_BITS1_FONT_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text_content, MULTI_LEFT);
    gui_text_wordwrap_set(text_content, true);

    gui_obj_add_event_cb(obj, (gui_event_cb_t)note_pressing_cb, GUI_EVENT_TOUCH_SCROLL_HORIZONTAL,
                         NULL);
    gui_obj_add_event_cb(obj, (gui_event_cb_t)note_released_cb, GUI_EVENT_TOUCH_RELEASED, NULL);
    gui_obj_add_event_cb(obj, (gui_event_cb_t)create_view_more, GUI_EVENT_TOUCH_CLICKED, obj);
}

static void list_timer_cb(void *param)
{
    gui_view_t *next_view = gui_view_get_next();
    if (!note_dur_animation && !clear_flag &&
        (next_view == current_view || next_view == NULL))
    {
        gui_list_set_note_num(list, infor_num);
    }

    if (current_view != gui_view_get_current())
    {
        gui_node_list_t *node = NULL;
        gui_list_for_each(node, &(list->base.child_list))
        {
            gui_obj_t *o = gui_list_entry(node, gui_obj_t, brother_list);
            gui_list_note_t *list_note = (gui_list_note_t *)o;
            if (list->base.y + list_note->base.y + list->note_length > gui_get_dc()->screen_height)
            {
                gui_obj_hidden(o, true);
            }
        }
    }
    else
    {
        gui_node_list_t *node = NULL;
        gui_list_for_each(node, &(list->base.child_list))
        {
            gui_obj_t *o = gui_list_entry(node, gui_obj_t, brother_list);
            gui_list_note_t *list_note = (gui_list_note_t *)o;
            gui_obj_hidden(o, false);
        }
    }
}

static void clear_all_timer_cb(void *widget)
{
    gui_list_note_t *note;
    uint8_t step = 30;
    gui_node_list_t *node = NULL;
    gui_list_for_each(node, &(list->base.child_list))
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        gui_list_note_t *list_note = (gui_list_note_t *)obj;
        list_note->t_x -= step;
        note = list_note;
    }
    if (abs(note->start_x - note->t_x) >= SCREEN_WIDTH)
    {
        gui_obj_child_free(GUI_BASE(list));
        while (infor_num)
        {
            infor_num--;
            gui_free(infor_rec[infor_num]);
            infor_rec[infor_num] = NULL;
        }
        gui_list_set_note_num(list, infor_num);
        gui_obj_stop_timer(GUI_BASE(widget));
        clear_flag = false;
    }
}

static void clear_all_note_cb(void *obj, gui_event_t e, void *param)
{
    if (in_view_more || list->note_num == 0) {return;}
    gui_obj_create_timer(GUI_BASE(obj), 10, true, clear_all_timer_cb);
    gui_obj_start_timer(GUI_BASE(obj));
    clear_flag = true;
}

static void canvas_clear_update_pos_cb(void *widget)
{
    gui_obj_t *obj = GUI_BASE(canvas_clear);
    gui_obj_move(obj, obj->x, 40 + list->offset); // move with list
}

static void create_clear_note(void *parent)
{
    gui_text_t *clear_text;
    canvas_clear = gui_canvas_round_rect_create(GUI_BASE(parent), "canvas_clear",
                                                52, 40, 305, 80, 40, gui_rgb(39, 43, 44));
    gui_obj_add_event_cb(GUI_BASE(canvas_clear), (gui_event_cb_t)clear_all_note_cb,
                         GUI_EVENT_TOUCH_CLICKED, NULL);
    // text
    char *clear_content = "clear all";
    clear_text = gui_text_create(canvas_clear, "clear all", 0, 0, 0, 0);
    gui_text_set(clear_text, (void *)clear_content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(clear_content),
                 32);
    gui_text_type_set(clear_text, SOURCEHANSANSSC_SIZE32_BITS1_FONT_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(clear_text, MID_CENTER);
}

static void top_view_design(gui_view_t *view)
{
    gui_view_switch_on_event(view, watchface_view, SWITCH_OUT_TO_TOP_USE_TRANSLATION,
                             SWITCH_IN_STILL_USE_BLUR,
                             GUI_EVENT_TOUCH_MOVE_UP);
    gui_view_set_opacity(view, 200);
    gui_obj_t *parent = GUI_BASE(view);
    // draw background
    gui_canvas_rect_t *canvas_bg = gui_canvas_rect_create(GUI_BASE(parent), "background", 0, 0,
                                                          SCREEN_WIDTH, SCREEN_HEIGHT, gui_rgb(76, 76, 76));
    gui_obj_create_timer(GUI_BASE(canvas_bg), 20, true, canvas_clear_update_pos_cb);

    create_clear_note(view);

    list = gui_list_create(view, "list", 0, NOTE_START, SCREEN_WIDTH, SCREEN_HEIGHT, NOTE_HEIGHT,
                           NOTE_INTERVAL, VERTICAL, create_inform_note, infor_rec, true);
    gui_list_set_style(list, LIST_CLASSIC);
    gui_list_set_bar_color(list, APP_COLOR_GRAY);
    gui_obj_create_timer(GUI_BASE(list), 20, true, list_timer_cb);
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
void add_information(information_t *payload)
{
    if (infor_num == INFOR_NUM_MAX)
    {
        // gui_log("can't add more infor!!! %s, %d\n", __FUNCTION__, __LINE__);
        return;
    }
    uint8_t index = infor_num;
    while (index)
    {
        infor_rec[index] = infor_rec[index - 1];
        index--;
    }
    infor_num++;
    infor_rec[0] = gui_lower_malloc(sizeof(information_t));
    memcpy(infor_rec[0], payload, sizeof(information_t));
    gui_log("add new information\r\n");
}

