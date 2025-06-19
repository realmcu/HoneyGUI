#include "root_image_hongkong/ui_resource.h"
#include "app_hongkong.h"
#include <gui_img.h>
#include "gui_view.h"
#include "gui_canvas.h"
#include "gui_win.h"
#include "gui_text.h"
#include "tp_algo.h"
#include "gui_canvas_rect.h"
#include "gui_canvas_round_rect.h"
#include "guidef.h"
#include "def_list.h"
#include "gui_list.h"


#define SCREEN_WIDTH (int16_t)gui_get_screen_width()
#define SCREEN_HEIGHT (int16_t)gui_get_screen_height()
#define TAB_HEIGHT 220
#define TAB_INTERVAL 20
#define TAB_START 140
#define INFOR_NUM_MAX 3

#define CURRENT_VIEW_NAME "app_top_view"
static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *watchface_view = NULL;
void top_view_design(gui_view_t *view);

static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = top_view_design,
    .keep = false,
};

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

typedef enum
{
    MESSAGE = 0,
    OS,
} app_name;

typedef struct information
{
    const char *informer;
    const char *content;
    const char *time;
    app_name app;
} information_t;

static void *font_size_32_bin_addr = SOURCEHANSANSSC_SIZE32_BITS1_FONT_BIN;
static void *font_size_24_bin_addr = SOURCEHANSANSSC_SIZE24_BITS1_FONT_BIN;
// static gui_img_t *arrow;
static gui_list_t *list;
static gui_canvas_round_rect_t *canvas_clear;
static information_t *infor_rec[INFOR_NUM_MAX];
static uint8_t infor_num = 0;
static uint8_t infor_need_update_num = 0;
static bool clear_flag = false;
static bool in_view_more = false;
static bool note_dur_animation = false;


static void cancel_cb(void *p)
{
    gui_win_t *win = (gui_win_t *)p;
    static uint16_t count = 0;
    uint16_t count_max = 300;
    count += win->base.timer->interval_ms;
    float percent = count / (float)count_max;
    win->base.x = -percent * SCREEN_WIDTH;

    if (percent >= 1.0f)
    {
        gui_obj_stop_timer(GUI_BASE(win));
        gui_obj_tree_free_async(win);
        in_view_more = false;
        count = 0;
    }
}

static void view_more_cb(void *p)
{
    // enter animation
    gui_win_t *win = (gui_win_t *)p;
    static uint16_t count = 0;
    uint16_t count_max = 300;
    count += win->base.timer->interval_ms;
    float percent = 1 - count / (float)count_max;
    if (win->base.x < -50)
    {
        win->base.x = -percent * SCREEN_WIDTH;
    }
    else
    {
        win->base.x = 0;
        count = 0;
    }
    if (win->base.x == 0)
    {
        static gui_canvas_round_rect_t *canvas = NULL;
        char *canvas_name;
        touch_info_t *tp = tp_get_info();
        if (tp->released)
        {
            if ((tp->y > 35 && tp->y < 67))
            {
                if (tp->x > 37 && tp->x < 116)
                {
                    gui_obj_create_timer(GUI_BASE(win), 20, true, cancel_cb);
                    gui_obj_start_timer(GUI_BASE(win));
                }
            }
            else
            {
                uint16_t base_y = 160, h = 80, interval = 10;
                if (tp->x > 30 && tp->x < 380)
                {
                    uint16_t y = base_y;
                    if ((tp->y > y && tp->y < y + h))
                    {
                        gui_log("click canvas1\r\n");
                        canvas_name = "canvas_1";
                    }
                    y = y + h + interval;
                    if ((tp->y > y && tp->y < y + h))
                    {
                        gui_log("click canvas2\r\n");
                        canvas_name = "canvas_2";
                    }
                    y = y + h + interval;
                    if ((tp->y > y && tp->y < y + h))
                    {
                        gui_log("click canvas3\r\n");
                        canvas_name = "canvas_3";
                    }
                    gui_node_list_t *node = NULL;
                    gui_list_for_each(node, &(win->base.child_list))
                    {
                        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
                        if (strcmp(obj->name, canvas_name) == 0)
                        {
                            canvas = (gui_canvas_round_rect_t *)obj;
                            gui_canvas_round_rect_set_color(canvas, gui_rgb(255, 255, 255));
                            return;
                        }
                    }
                }
            }
        }
        if (canvas)
        {
            gui_canvas_round_rect_set_color(canvas, gui_rgb(39, 43, 44));
        }
    }
}

static void view_more_event_cb(void *widget, gui_event_t e, void *param)
{
    ; // This function is intentionally left empty.
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
        gui_obj_move(obj, 0, list_note->start_y);
        index++;
    }
    gui_obj_tree_free_async(GUI_BASE(note));
    list->total_length -= (list->note_length + list->space);
    list->total_length = list->total_length < SCREEN_HEIGHT ? SCREEN_HEIGHT : list->total_length;
    list->widget_num--;
    index = infor_num - 1 - index;
    gui_free(infor_rec[index]);
    // gui_log("free infor_rec[%d]\n", index);
    while (index < infor_num - 1)
    {
        infor_rec[index] = infor_rec[index + 1];
        index++;
    }
    infor_num--;
    clear_flag = false;
}

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

static void create_view_more(void *obj, gui_event_t e, void *param)
{
    if (in_view_more) {return;}
    in_view_more = true;
    information_t *inform = (information_t *)param;
    const char *informer = inform->informer;
    const char *content = inform->content;
    const char *time = inform->time;
    app_name app = inform->app;

    gui_win_t *win = gui_win_create(gui_obj_get_root(), "win_view_more", -SCREEN_WIDTH, 0, SCREEN_WIDTH,
                                    SCREEN_HEIGHT);
    gui_obj_create_timer(GUI_BASE(win), 20, true, view_more_cb);
    gui_obj_add_event_cb(GUI_BASE(win), (gui_event_cb_t)view_more_event_cb,
                         GUI_EVENT_TOUCH_SCROLL_VERTICAL, NULL);
    gui_canvas_rect_t *canvas_bg = gui_canvas_rect_create(GUI_BASE(win), "0", 0, 0,
                                                          SCREEN_WIDTH,
                                                          SCREEN_HEIGHT, gui_rgb(0, 0, 0));
    {
        char *content = "Cancel";
        gui_text_t *text = gui_text_create(win, "cancel",  37, 35, 80, 32);
        gui_text_set(text, (void *)content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                     strlen(content),
                     32);
        gui_text_type_set(text, font_size_32_bin_addr, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, LEFT);
    }

    gui_text_t *text = gui_text_create(win, "time", -60, 35, 0, 0);
    gui_text_set(text, (void *)(time), GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(time),
                 32);
    gui_text_type_set(text, font_size_32_bin_addr, FONT_SRC_MEMADDR);
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
            gui_text_type_set(text, font_size_24_bin_addr, FONT_SRC_MEMADDR);
            gui_text_mode_set(text, MULTI_LEFT);
            gui_img_create_from_mem(win, "0", UI_PERSON_ICON_BIN, 360, 40, 0, 0);
        }
        break;
    case OS:
        {
            content = "\"OS\" dialog" ;
            text = gui_text_create(win, "0", 37, 120, 0, 0);
            gui_text_set(text, (void *)content, GUI_FONT_SRC_BMP, gui_rgb(153, 153, 153),
                         strlen(content),
                         24);
            gui_text_type_set(text, font_size_24_bin_addr, FONT_SRC_MEMADDR);
            gui_text_mode_set(text, LEFT);
            gui_img_create_from_mem(win, "0", UI_IWATCH_32_ICON_BIN, 360, 40, 0, 0);
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
    gui_text_type_set(text, font_size_32_bin_addr, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, CENTER);

    canvas = gui_canvas_round_rect_create(GUI_BASE(win), "canvas_2", 30, 250, 350, 80, 15,
                                          gui_rgb(39, 43, 44));
    content = "Don't remind today";
    text = gui_text_create(canvas, "text1",  0, 24, 0, 0);
    gui_text_set(text, (void *)content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(content),
                 32);
    gui_text_type_set(text, font_size_32_bin_addr, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, CENTER);

    canvas = gui_canvas_round_rect_create(GUI_BASE(win), "canvas_3", 30, 340, 350, 80, 15,
                                          gui_rgb(39, 43, 44));
    content = "Add this to Summary";
    text = gui_text_create(canvas, "text1",  0, 24, 0, 0);
    gui_text_set(text, (void *)content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(content),
                 32);
    gui_text_type_set(text, font_size_32_bin_addr, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, CENTER);
}

static void create_inform_note(information_t *inform)
{
    gui_log("create_inform_note\n");
    const char *informer = inform->informer;
    const char *content = inform->content;
    const char *time = inform->time;
    app_name app = inform->app;

    gui_list_note_t *note = gui_list_add_note(list, true);
    gui_canvas_round_rect_t *canvas = gui_canvas_round_rect_create(GUI_BASE(note), "0", 30, 0, 350, 220,
                                                                   35, gui_rgb(39, 43, 44));
    gui_obj_add_event_cb(GUI_BASE(note), (gui_event_cb_t)create_view_more, GUI_EVENT_TOUCH_CLICKED,
                         (void *)inform);
    // message
    gui_text_t *text_informer, *text_content, *text_time;
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
    text_time = gui_text_create(canvas, "time",  -15, 15, 0, 0);
    gui_text_set(text_time, (void *)time, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(time),
                 24);
    gui_text_type_set(text_time, font_size_24_bin_addr, FONT_SRC_MEMADDR);
    gui_text_mode_set(text_time, RIGHT);

    text_informer = gui_text_create(canvas, "informer",  10, 50, 0, 0);
    gui_text_set(text_informer, (void *)informer, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(informer),
                 32);
    gui_text_type_set(text_informer, font_size_32_bin_addr, FONT_SRC_MEMADDR);
    gui_text_mode_set(text_informer, LEFT);

    text_content = gui_text_create(canvas, "content",  10, 75, 340, 0);
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
    gui_text_type_set(text_content, font_size_32_bin_addr, FONT_SRC_MEMADDR);
    gui_text_mode_set(text_content, MULTI_LEFT);
    gui_text_wordwrap_set(text_content, true);
    // gui_text_convert_to_img(message_text, RGB565);

    gui_obj_add_event_cb(note, (gui_event_cb_t)note_pressing_cb, GUI_EVENT_TOUCH_SCROLL_HORIZONTAL,
                         NULL);
    gui_obj_add_event_cb(note, (gui_event_cb_t)note_released_cb, GUI_EVENT_TOUCH_RELEASED, NULL);
    gui_obj_add_event_cb(note, (gui_event_cb_t)create_view_more, GUI_EVENT_TOUCH_CLICKED, note);
}

static void list_timer_cb(void *param)
{
    if (!note_dur_animation && !clear_flag && (list->total_length == SCREEN_HEIGHT ||
                                               list->offset != (SCREEN_HEIGHT - list->total_length)))
    {
        while (infor_need_update_num)
        {
            infor_need_update_num--;
            create_inform_note(infor_rec[0]);
        }
    }

    gui_obj_t *obj = GUI_BASE(canvas_clear);
    gui_obj_move(obj, obj->x, 40 + list->offset); // move with list

    if (current_view != gui_view_get_current())
    {
        gui_node_list_t *node = NULL;
        gui_list_for_each(node, &(list->base.child_list))
        {
            gui_obj_t *o = gui_list_entry(node, gui_obj_t, brother_list);
            gui_list_note_t *list_note = (gui_list_note_t *)o;
            if (list_note->base.y + list->note_length > gui_get_dc()->screen_height)
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

    // update arrow
    // static int offset_rec = 0;
    // if (list->offset > offset_rec)
    // {
    //     gui_img_set_image_data(arrow, UI_ARROW_DOWN_BIN);
    // }
    // else if (list->offset < offset_rec)
    // {
    //     gui_img_set_image_data(arrow, UI_ARROW_UP_BIN);
    // }
    // else
    // {
    //     gui_img_set_image_data(arrow, UI_LINE_STILL_BIN);
    // }
    // offset_rec = list->offset;
    // gui_img_refresh_size(arrow);
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
        // gui_obj_child_free(GUI_BASE(list));
        gui_list_for_each(node, &(list->base.child_list))
        {
            gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
            gui_obj_tree_free_async(obj);
        }

        list->total_length = gui_get_dc()->screen_height;
        list->widget_num = 0;
        while (infor_num)
        {
            infor_num--;
            gui_free(infor_rec[infor_num]);
        }
        gui_obj_stop_timer(GUI_BASE(widget));
        clear_flag = false;
    }
}

static void clear_all_note_cb(void *obj, gui_event_t e, void *param)
{
    if (in_view_more || list->widget_num == 0) {return;}
    gui_obj_create_timer(GUI_BASE(obj), 10, true, clear_all_timer_cb);
    gui_obj_start_timer(GUI_BASE(obj));
    clear_flag = true;
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
    gui_text_type_set(clear_text, font_size_32_bin_addr, FONT_SRC_MEMADDR);
    gui_text_mode_set(clear_text, MID_CENTER);
}

void add_information(gui_msg_t *msg)
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
    infor_need_update_num++;
    infor_rec[0] = gui_malloc(sizeof(information_t));
    memcpy(infor_rec[0], msg->payload, sizeof(information_t));
    gui_log("add new information\r\n");
    // gui_log("msg->payload: 0x%x\n", msg->payload);
}

void top_view_design(gui_view_t *view)
{
    gui_view_switch_on_event(view, watchface_view, SWITCH_OUT_TO_TOP_USE_TRANSLATION,
                             SWITCH_IN_STILL_USE_BLUR,
                             GUI_EVENT_TOUCH_MOVE_UP);
    gui_view_set_opacity(view, 200);
    gui_obj_t *parent = GUI_BASE(view);
    // draw background
    gui_canvas_rect_t *canvas_bg = gui_canvas_rect_create(GUI_BASE(parent), "background", 0, 0,
                                                          SCREEN_WIDTH, SCREEN_HEIGHT, gui_rgba(76, 76, 76, 255));

    list = gui_list_create(view, "list", 0, TAB_START, SCREEN_WIDTH, SCREEN_HEIGHT, TAB_HEIGHT,
                           TAB_INTERVAL, VERTICAL);
    gui_list_set_style(list, LIST_CLASSIC);
    gui_obj_create_timer(GUI_BASE(list), 20, true, list_timer_cb);
    gui_obj_start_timer(GUI_BASE(list));

    create_clear_note(view);
    for (int8_t i = infor_num - 1; i >= 0; i--)
    {
        create_inform_note(infor_rec[i]);
    }
    infor_need_update_num = 0;

    // arrow = gui_img_create_from_mem(view, "arrow", UI_LINE_STILL_BIN, 181, SCREEN_HEIGHT - 25, 0,
    //                                 0);
    // gui_img_set_mode(arrow, IMG_SRC_OVER_MODE);
}

