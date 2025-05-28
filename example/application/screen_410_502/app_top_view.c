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


#define SCREEN_WIDTH 410
#define SCREEN_HEIGHT 502
#define TAB_HEIGHT 220
#define TAB_INTERVAL 20
#define TAB_START 140
#define INFOR_NUM_MAX 2

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
static bool close_flag = 0;

#if 0
static void win_clear_all_cb(void *p)
{
    close_flag = 1;
    static uint16_t count = 0;
    uint16_t count_max = 500;
    count += win->base.timer->interval_ms;
    float percent = count / (float)count_max;
    if (percent >= 1.0f) {gui_obj_stop_timer(GUI_BASE(p));}
    uint8_t index = 0;
    while (tv_array[index])
    {
        tv_array[index]->x = -percent * SCREEN_WIDTH;
        if (percent >= 1.0f)
        {
            gui_obj_tree_free(tv_array[index]);
            tv_array[index] = NULL;
            GUI_BASE(pg)->h = GUI_BASE(pg)->h - (TAB_HEIGHT + TAB_INTERVAL);
            if (index == INFOR_NUM_MAX - 1 || tv_array[index + 1] == NULL)
            {
                gui_node_list_t *node = NULL;
                gui_node_list_t *tmp = NULL;
                // clear "win_view_more"
                gui_list_for_each_safe(node, tmp, &(GUI_BASE(current_view)->child_list))
                {
                    gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
                    if (strcmp("win_view_more", obj->name) == 0)
                    {
                        gui_obj_tree_free(obj);
                    }
                }
                close_flag = 0;
                return;
            }
        }
        index++;
        if (index == INFOR_NUM_MAX)
        {
            break;
        }
    }
}


static void tv_clear_all(void *obj, gui_event_t e)
{
    uint8_t index = 0;
    close_flag = 1;
    while (tv_array[index])
    {
        gui_noteview_t *tv = (gui_noteview_t *)tv_array[index];
        if (tv->cur_id.x == 0)
        {
            gui_node_list_t *node = NULL;
            gui_node_list_t *tmp = NULL;
            gui_obj_t *obj = GUI_BASE(tv);
            gui_list_for_each_safe(node, tmp, &obj->child_list)
            {
                gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
                if (strcmp(obj->name, "note_1") == 0)
                {
                    obj->not_show = 1;
                }
            }
        }
        index++;
        if (index == INFOR_NUM_MAX)
        {
            break;
        }
    }
    if (index != 0)
    {
        gui_obj_create_timer(GUI_BASE(win_animate), 20, true, win_clear_all_cb);
        gui_obj_start_timer(GUI_BASE(win_animate));
    }
}

static void win_clear_cb(gui_obj_t *obj)
{
    close_flag = 1;
    static uint16_t count = 0;
    uint16_t count_max = 500;
    count += win->base.timer->interval_ms;
    float percent = count / (float)count_max;
    if (percent >= 1.0f) {gui_obj_stop_timer(GUI_BASE(p));}
    obj->x = (0 - percent * 2) * SCREEN_WIDTH;
    uint8_t index = 0;
    while (1)
    {
        if (tv_array[index] == obj)
        {
            break;
        }
        index++;
    }
    if (percent >= 1.0f)
    {
        gui_obj_tree_free(obj);
        GUI_BASE(pg)->h = GUI_BASE(pg)->h - (TAB_HEIGHT + TAB_INTERVAL);
        while (1)
        {
            if (index == INFOR_NUM_MAX - 1)
            {
                tv_array[index] = NULL;
                break;
            }

            tv_array[index] = tv_array[index + 1];
            if (!tv_array[index])
            {
                break;
            }
            index++;
        }
        close_flag = 0;
    }
    if (percent > 0.5f)
    {
        if (index == INFOR_NUM_MAX - 1)
        {
            return;
        }
        index++;
        while (tv_array[index])
        {
            if (percent > 0.95f)
            {
                tv_array[index]->y = TAB_START + (index - 1) * (TAB_HEIGHT + TAB_INTERVAL);
            }
            else
            {
                tv_array[index]->y = TAB_START + (index - 1 + 2 * (1 - percent)) * (TAB_HEIGHT + TAB_INTERVAL);
            }
            index++;
            // gui_log("y = %d\r\n", tv_os->y);
            if (index == INFOR_NUM_MAX)
            {
                break;
            }
        }
    }
}

static void tv_clear(void *obj, gui_event_t e, void *param)
{
    gui_obj_create_timer(GUI_BASE(win_animate), 20, true, win_clear_cb);
    gui_obj_start_timer(GUI_BASE(win_animate));
    gui_obj_tree_free(param);
}

static void cancel_cb(void *p)
{
    // exit animation
    static uint16_t count = 0;
    uint16_t count_max = 500;
    count += win->base.timer->interval_ms;
    float percent = count / (float)count_max;
    win->x = -percent * SCREEN_WIDTH;

    if (percent >= 1.0f)
    {
        gui_obj_stop_timer(GUI_BASE(p));
        gui_obj_start_timer(GUI_BASE(win_design));
        gui_obj_show(win, false);
        GUI_BASE(pg)->gesture = 0;
        // gui_obj_tree_free(win);
    }
}

static void view_more_cb(void *p)
{
    // enter animation
    gui_win_t *win = (gui_win_t *)p;
    static uint16_t count = 0;
    uint16_t count_max = 500;
    count += win->base.timer->interval_ms;
    float percent = count / (float)count_max;
    gui_obj_show(win, true);
    if (win->x < -100)
    {
        win->x = percent * SCREEN_WIDTH;
    }
    else
    {
        win->x = 0;
    }
    if (win->x == 0)
    {
        static gui_canvas_round_rect_t *canvas = NULL;
        char *canvas_name;
        touch_info_t *tp = tp_get_info();
        if (tp->released)
        {
            if ((tp->y > 15 && tp->y < 47))
            {
                if (tp->x > 37 && tp->x < 116)
                {
                    gui_obj_create_timer(GUI_BASE(win), 20, true, cancel_cb);
                    gui_obj_start_timer(GUI_BASE(win));
                }
            }
            else
            {
                uint16_t base_y = 140, h = 80, interval = 10;
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
                    gui_node_list_t *tmp = NULL;
                    gui_obj_t *obj = GUI_BASE(win);
                    gui_list_for_each_safe(node, tmp, &obj->child_list)
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
// ... content
static void view_more_enter(void *obj, gui_event_t e, void *param)
{
    gui_obj_stop_timer(GUI_BASE(win_design));
    GUI_BASE(pg)->gesture = 1; // stop page slide, win_design animation will restore it
    gui_obj_create_timer(GUI_BASE(win_animate), 20, true, view_more_cb);
    gui_obj_start_timer(GUI_BASE(win_animate));
}

void pagelist_create(gui_msg_t *msg)
{
    gui_view_t *view = gui_view_get_current();
    if (!view || strcmp(GUI_BASE(view)->name, CURRENT_VIEW_NAME) || !pg)
    {
        // gui_log("Current APP can't add note!!! %s, %d\n", __FUNCTION__, __LINE__);
        return;
    }

    if (close_flag)
    {
        gui_log("During close animation, can't add note!!! %s, %d\n", __FUNCTION__, __LINE__);
        return;
    }
    // move current information, then add new information on the top
    uint8_t index = 0;
    while (1)
    {
        if (!tv_array[index])
        {
            break;
        }
        index++;
        if (index == INFOR_NUM_MAX)
        {
            // gui_log("Can't add more note!!! %s, %d\n", __FUNCTION__, __LINE__);
            return;
        }
    }
    if (index > 0)
    {
        while (1)
        {
            tv_array[index - 1]->y = TAB_START + index * (TAB_HEIGHT + TAB_INTERVAL);
            tv_array[index] = tv_array[index - 1];
            if ((index - 1) == 0)
            {
                break;
            }
            index--;
        }
    }
    information_t *payload = (information_t *)msg->payload;
    const char *informer = payload->informer;
    const char *content = payload->content;
    const char *time = payload->time;
    app_name app = payload->app;

    gui_noteview_t *tv = gui_noteview_create(pg, "0", 0, TAB_START, SCREEN_WIDTH, 220);
    gui_note_t *tb_inform = gui_note_create(tv, "note_0", 0, 0, 0, 0, 0, 0);
    gui_note_t *tb_option = gui_note_create(tv, "note_1", 0, 0, 0, 0, 1, 0);
    tv_array[0] = GUI_BASE(tv);
    // message
    gui_text_t *text_informer, *text_content, *text_time;
    gui_canvas_round_rect_t *canvas = gui_canvas_round_rect_create(GUI_BASE(tb_inform),
                                                                   "0", 30, 0, 350, 220, 35, gui_rgb(39, 43, 44));
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
    gui_img_create_from_mem(canvas, "message icon", img_path, 10, -15, 0, 0);

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

    // option left
    char *option_content = "...";
    gui_canvas_round_rect_t *canvas_left = gui_canvas_round_rect_create(GUI_BASE(tb_option), "0",
                                                                        30, 0, 170,
                                                                        220, 35, gui_rgb(39, 43, 44));

    gui_text_t *text = gui_text_create(canvas_left, "0",  0, 0, 0, 0);
    gui_text_set(text, (void *)option_content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(option_content),
                 32);
    gui_text_type_set(text, font_size_32_bin_addr, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, MID_CENTER);

    // view more
    gui_win_t *win;
    {
        win = gui_win_create(current_view, "win_view_more", -SCREEN_WIDTH, 0, SCREEN_WIDTH,
                             SCREEN_HEIGHT);
        gui_obj_show(win, false);
        gui_canvas_rect_t *canvas_bg = gui_canvas_rect_create(GUI_BASE(win), "0", 0, 0,
                                                              SCREEN_WIDTH,
                                                              SCREEN_HEIGHT, gui_rgb(0, 0, 0));

        char *content = "Cancel";
        gui_text_t *text = gui_text_create(win, "cancel",  37, 15, 80, 32);
        gui_text_set(text, (void *)content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                     strlen(content),
                     32);
        gui_text_type_set(text, font_size_32_bin_addr, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, LEFT);

        // content = "10:24";
        text = gui_text_create(win, "time", -60, 15, 0, 0);
        gui_text_set(text, (void *)(time), GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                     strlen(time),
                     32);
        gui_text_type_set(text, font_size_32_bin_addr, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, RIGHT);

        switch (app)
        {
        case MESSAGE:
            {
                char *name = gui_malloc(40);
                sprintf(name, "\"message\" dialog\n%s", informer);
                text = gui_text_create(win, "message",  37, 70, 0, 0);
                gui_text_set(text, (void *)name, GUI_FONT_SRC_BMP, gui_rgb(153, 153, 153),
                             strlen(name),
                             24);
                gui_text_type_set(text, font_size_24_bin_addr, FONT_SRC_MEMADDR);
                gui_text_mode_set(text, MULTI_LEFT);
                gui_img_create_from_mem(win, "0", UI_PERSON_ICON_BIN, 360, 20, 0, 0);
                // gui_free(name);
            }
            break;
        case OS:
            {
                content = "\"OS\" dialog" ;
                text = gui_text_create(win, "0",  37, 100, 0, 0);
                gui_text_set(text, (void *)content, GUI_FONT_SRC_BMP, gui_rgb(153, 153, 153),
                             strlen(content),
                             24);
                gui_text_type_set(text, font_size_24_bin_addr, FONT_SRC_MEMADDR);
                gui_text_mode_set(text, LEFT);
                gui_img_create_from_mem(win, "0", UI_IWATCH_32_ICON_BIN, 360, 20, 0, 0);
            }
            break;
        default:
            break;
        }

        // options
        gui_canvas_round_rect_t *canvas = gui_canvas_round_rect_create(GUI_BASE(win), "canvas_1",
                                                                       30, 140, 350, 80, 15, gui_rgb(39, 43, 44));
        content = "Don't remind for an hour";
        text = gui_text_create(canvas, "text1",  0, 24, 0, 0);
        gui_text_set(text, (void *)content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                     strlen(content),
                     32);
        gui_text_type_set(text, font_size_32_bin_addr, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, CENTER);

        canvas = gui_canvas_round_rect_create(GUI_BASE(win), "canvas_2", 30, 230, 350, 80, 15,
                                              gui_rgb(39, 43, 44));
        content = "Don't remind today";
        text = gui_text_create(canvas, "text1",  0, 24, 0, 0);
        gui_text_set(text, (void *)content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                     strlen(content),
                     32);
        gui_text_type_set(text, font_size_32_bin_addr, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, CENTER);

        canvas = gui_canvas_round_rect_create(GUI_BASE(win), "canvas_3", 30, 320, 350, 80, 15,
                                              gui_rgb(39, 43, 44));
        content = "Add this to Summary";
        text = gui_text_create(canvas, "text1",  0, 24, 0, 0);
        gui_text_set(text, (void *)content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                     strlen(content),
                     32);
        gui_text_type_set(text, font_size_32_bin_addr, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, CENTER);
        gui_obj_add_event_cb(tv, (gui_event_cb_t)view_more_enter, GUI_EVENT_2, (void *)win);
    }

    //option right
    option_content = "X";
    gui_canvas_round_rect_t *canvas_right = gui_canvas_round_rect_create(GUI_BASE(tb_option), "0",
                                                                         210, 0,
                                                                         170, 220, 35, gui_rgb(39, 43, 44));

    text = gui_text_create(canvas_right, "0",  0, 0, 0, 0);
    gui_text_set(text, (void *)option_content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(option_content),
                 32);
    gui_text_type_set(text, font_size_32_bin_addr, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, MID_CENTER);

    gui_obj_add_event_cb(tv, (gui_event_cb_t)tv_clear, GUI_EVENT_TOUCH_CLICKED, (void *)win);
}

static void win_design_cb(void *p)
{
    gui_obj_t *obj = GUI_BASE(pg);
    bool flag = false;
    int16_t y;
    int16_t h;
    static bool view_change = 0;
    uint8_t index = 0;

    while (1)
    {
        if (!tv_array[index])
        {
            break;
        }
        GUI_BASE(tv_array[index])->skip_tp_left_hold = 0;
        GUI_BASE(tv_array[index])->skip_tp_right_hold = 0;
        index++;
        if (index == INFOR_NUM_MAX)
        {
            break;
        }
    }

    touch_info_t *tp = tp_get_info();
    if (!GUI_BASE(current_view)->gesture && tp->deltaY < -(SCREEN_HEIGHT / 2))
    {
        view_change = 1;
    }

    static bool hold;
    if (tp->pressed)
    {
        hold = 1;
    }
    if (tp->released)
    {
        hold = 0;
        gui_img_set_image_data(arrow, UI_LINE_STILL_BIN);
        if (!view_change) //still view_up, show all pages
        {
            GUI_BASE(pg)->gesture = 0;
        }
        // clear all
        gui_obj_t *canvas = 0;
        gui_obj_tree_get_widget_by_name(current_view, "canvas_clear", (void *)&canvas);
        if (tp->deltaY < 15 && tp->deltaX < 15) //prevent slide from triggering clear
        {
            if (canvas)
            {
                y = canvas->y + obj->y;
                h = canvas->h;
                if ((tp->y > y && tp->y < y + h))
                {
                    if (tp->x > 52 && tp->x < 357)
                    {
                        gui_obj_enable_event(canvas, GUI_EVENT_TOUCH_CLICKED);

                        gui_log("click clear\r\n");
                        return;
                    }
                }
            }
        }

        // clear noteview
        if (tp->deltaY < 30 && tp->deltaX < 30)
        {
            for (uint8_t i = 0; i < index; i++)
            {
                gui_noteview_t *tv = (gui_noteview_t *)tv_array[i];
                if (tv)
                {
                    if (tv->cur_id.x == 1)
                    {
                        y = GUI_BASE(tv)->y + obj->y;
                        h = GUI_BASE(tv)->h;
                        if ((tp->y > y && tp->y < y + h))
                        {
                            if (tp->x > 30 && tp->x < 200)
                            {
                                gui_obj_enable_event(GUI_BASE(tv), GUI_EVENT_2);
                                // gui_log("click left\r\n");
                            }
                            else if (tp->x > 210 && tp->x < 380)
                            {
                                gui_obj_enable_event(GUI_BASE(tv), GUI_EVENT_TOUCH_CLICKED);
                                // gui_log("click right\r\n");
                            }
                            break;
                        }
                    }
                }
            }
        }
    }
    if (hold)
    {
        // page part
        flag =  tp->deltaY < 0 && obj->y != 0 &&
                obj->y <= (pg->start_y - (obj->h - SCREEN_HEIGHT)); //@BOTTOM
        if (flag || tp->y > SCREEN_HEIGHT - 50)
        {
            GUI_BASE(pg)->gesture = 1;
            GUI_BASE(current_view)->gesture = 0;
            // gui_log("view down\n");
        }
        else
        {
            GUI_BASE(pg)->gesture = 0;
            GUI_BASE(current_view)->gesture = 1;
            // gui_log("page down\n");
        }

        if (tp->deltaY > 15)
        {
            gui_img_set_image_data(arrow, UI_ARROW_DOWN_BIN);
        }
        else if (tp->deltaY < -15)
        {
            gui_img_set_image_data(arrow, UI_ARROW_UP_BIN);
        }
    }
    // not show pages out of screen
    if (current_view->view_switch_ready)
    {
        for (uint8_t i = 0; i < index; i++)
        {
            gui_noteview_t *tv = (gui_noteview_t *)tv_array[i];
            y = GUI_BASE(tv)->y + obj->y;
            if (y > SCREEN_HEIGHT - TAB_HEIGHT && GUI_BASE(pg)->gesture)
            {
                GUI_BASE(tv)->not_show = 1;
            }
            else
            {
                GUI_BASE(tv)->not_show = 0;
            }
        }
    }
    // enter view_up, show all pages
    else
    {
        for (uint8_t i = 0; i < index; i++)
        {
            gui_noteview_t *tv = (gui_noteview_t *)tv_array[i];
            y = GUI_BASE(tv)->y + obj->y;
            if (y > SCREEN_HEIGHT - TAB_HEIGHT)
            {
                GUI_BASE(tv)->not_show = 1;
            }
            else
            {
                GUI_BASE(tv)->not_show = 0;
            }
        }
        GUI_BASE(pg)->gesture = 0;
        view_change = 0;
    }
}
#endif
static gui_list_t *list;
static gui_canvas_round_rect_t *canvas_clear;
static information_t *infor_rec[INFOR_NUM_MAX];
static uint8_t infor_num = 0;
static bool note_need_update = false;


static void note_timer_cb(void *widget)
{
    gui_obj_t *obj = GUI_BASE(widget);
    gui_list_note_t *note = (gui_list_note_t *)obj;
    uint8_t step = 20;
    if (abs(note->start_x - note->t_x) >= SCREEN_WIDTH)
    {
        gui_log("clear note\n");
        gui_obj_stop_timer(obj);
    }
    else if (note->t_x - note->start_x >= SCREEN_WIDTH / 2)  {note->t_x += step;}
    else if (note->t_x - note->start_x <= -SCREEN_WIDTH / 2) {note->t_x -= step;}
    else if (note->t_x < note->start_x - step)               {note->t_x += step;}
    else if (note->t_x > note->start_x + step)               {note->t_x -= step;}
    else
    {
        note->t_x = note->start_x;
        gui_obj_stop_timer(obj);
    }
}

static void note_pressing_cb(void *widget)
{
    gui_obj_t *obj = GUI_BASE(widget);
    gui_list_note_t *note = (gui_list_note_t *)obj;
    struct touch_info *tp = tp_get_info();
    note->t_x = note->start_x + tp->deltaX;
}

static void note_released_cb(void *widget)
{
    gui_obj_t *obj = GUI_BASE(widget);
    gui_obj_create_timer(obj, 20, true, note_timer_cb);
    gui_obj_start_timer(obj);
}

static void create_view_more(void *obj)
{
    gui_node_list_t *node = NULL;
    uint8_t index = 0;
    gui_list_for_each(node, &(list->base.child_list))
    {
        gui_obj_t *o = gui_list_entry(node, gui_obj_t, brother_list);
        if (o == obj) {break;}
        index++;
    }

    const char *informer = infor_rec[index]->informer;
    const char *content = infor_rec[index]->content;
    const char *time = infor_rec[index]->time;
    app_name app = infor_rec[index]->app;

    gui_win_t *win = gui_win_create(current_view, "win_view_more", -SCREEN_WIDTH, 0, SCREEN_WIDTH,
                                    SCREEN_HEIGHT);
    gui_obj_show(win, false);
    gui_canvas_rect_t *canvas_bg = gui_canvas_rect_create(GUI_BASE(win), "0", 0, 0,
                                                          SCREEN_WIDTH,
                                                          SCREEN_HEIGHT, gui_rgb(0, 0, 0));
    {
        char *content = "Cancel";
        gui_text_t *text = gui_text_create(win, "cancel",  37, 15, 80, 32);
        gui_text_set(text, (void *)content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                     strlen(content),
                     32);
        gui_text_type_set(text, font_size_32_bin_addr, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, LEFT);
    }

    gui_text_t *text = gui_text_create(win, "time", -60, 15, 0, 0);
    gui_text_set(text, (void *)(time), GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(time),
                 32);
    gui_text_type_set(text, font_size_32_bin_addr, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, RIGHT);

    switch (app)
    {
    case MESSAGE:
        {
            char *name = gui_malloc(40);
            sprintf(name, "\"message\" dialog\n%s", informer);
            text = gui_text_create(win, "message",  37, 70, 0, 0);
            gui_text_set(text, (void *)name, GUI_FONT_SRC_BMP, gui_rgb(153, 153, 153),
                         strlen(name),
                         24);
            gui_text_type_set(text, font_size_24_bin_addr, FONT_SRC_MEMADDR);
            gui_text_mode_set(text, MULTI_LEFT);
            gui_img_create_from_mem(win, "0", UI_PERSON_ICON_BIN, 360, 20, 0, 0);
        }
        break;
    case OS:
        {
            content = "\"OS\" dialog" ;
            text = gui_text_create(win, "0",  37, 100, 0, 0);
            gui_text_set(text, (void *)content, GUI_FONT_SRC_BMP, gui_rgb(153, 153, 153),
                         strlen(content),
                         24);
            gui_text_type_set(text, font_size_24_bin_addr, FONT_SRC_MEMADDR);
            gui_text_mode_set(text, LEFT);
            gui_img_create_from_mem(win, "0", UI_IWATCH_32_ICON_BIN, 360, 20, 0, 0);
        }
        break;
    default:
        break;
    }

    // options
    gui_canvas_round_rect_t *canvas = gui_canvas_round_rect_create(GUI_BASE(win), "canvas_1",
                                                                   30, 140, 350, 80, 15, gui_rgb(39, 43, 44));
    content = "Don't remind for an hour";
    text = gui_text_create(canvas, "text1",  0, 24, 0, 0);
    gui_text_set(text, (void *)content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(content),
                 32);
    gui_text_type_set(text, font_size_32_bin_addr, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, CENTER);

    canvas = gui_canvas_round_rect_create(GUI_BASE(win), "canvas_2", 30, 230, 350, 80, 15,
                                          gui_rgb(39, 43, 44));
    content = "Don't remind today";
    text = gui_text_create(canvas, "text1",  0, 24, 0, 0);
    gui_text_set(text, (void *)content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(content),
                 32);
    gui_text_type_set(text, font_size_32_bin_addr, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, CENTER);

    canvas = gui_canvas_round_rect_create(GUI_BASE(win), "canvas_3", 30, 320, 350, 80, 15,
                                          gui_rgb(39, 43, 44));
    content = "Add this to Summary";
    text = gui_text_create(canvas, "text1",  0, 24, 0, 0);
    gui_text_set(text, (void *)content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(content),
                 32);
    gui_text_type_set(text, font_size_32_bin_addr, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, CENTER);

    // gui_obj_add_event_cb(tv, (gui_event_cb_t)view_more_enter, GUI_EVENT_2, (void *)win);
}

static void create_inform_note(information_t *inform)
{
    const char *informer = inform->informer;
    const char *content = inform->content;
    const char *time = inform->time;
    app_name app = inform->app;

    gui_list_note_t *note = gui_list_add_note(list, true);
    gui_canvas_round_rect_t *canvas = gui_canvas_round_rect_create(GUI_BASE(note), "0", 30, 0, 350, 220,
                                                                   35, gui_rgb(39, 43, 44));

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

    note_need_update = false;
}

static void list_timer_cb(void *param)
{
    if (note_need_update)
    {
        create_inform_note(infor_rec[0]);
    }

    gui_obj_t *obj = GUI_BASE(canvas_clear);
    gui_obj_move(obj, obj->x, 40 + list->offset); // move with list

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

static void note_clear_timer_cb(void *param)
{
    gui_obj_t *obj = GUI_BASE(param);
    uint8_t step = 30;
    if (obj->x < 0 && obj->x > -SCREEN_WIDTH)
    {
        obj->x -= step;
    }
    else if (obj->x > 0 && obj->x < SCREEN_WIDTH)
    {
        obj->x += step;
    }
    else
    {
        gui_log("note clear end\r\n");
        gui_obj_stop_timer(obj);
    }
}

static void create_clear_note(void *parent)
{
    gui_text_t *clear_text;
    canvas_clear = gui_canvas_round_rect_create(GUI_BASE(parent), "canvas_clear",
                                                52, 40, 305, 80, 40, gui_rgb(39, 43, 44));
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
    infor_rec[0] = gui_malloc(sizeof(information_t));
    memcpy(infor_rec[0], msg->payload, sizeof(information_t));
    note_need_update = true;
    gui_log("add new information\r\n");
}

void top_view_design(gui_view_t *view)
{
    gui_view_switch_on_event(view, watchface_view, SWITCH_OUT_TO_TOP_USE_TRANSLATION, SWITCH_INIT_STATE,
                             GUI_EVENT_TOUCH_MOVE_UP);

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

    // arrow = gui_img_create_from_mem(view, "arrow", UI_LINE_STILL_BIN, 181, SCREEN_HEIGHT - 25, 0,
    //                                 0);
    // gui_img_set_mode(arrow, IMG_SRC_OVER_MODE);
}

