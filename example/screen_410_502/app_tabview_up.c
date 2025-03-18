#include "root_image_hongkong/ui_resource.h"
#include <gui_img.h>
#include "gui_view.h"
#include "gui_tabview.h"
#include "gui_tab.h"
#include "gui_canvas.h"
#include "gui_win.h"
#include "gui_text.h"
#include "gui_page.h"
#include "gui_app.h"
#include "tp_algo.h"
#include "gui_canvas_img.h"
#include "gui_canvas_rect.h"
#include "gui_canvas_round_rect.h"
#include "guidef.h"
#include "def_list.h"
#include "app_hongkong.h"

#define SCREEN_WIDTH 410
#define SCREEN_HEIGHT 502
#define TAB_HEIGHT 220
#define TAB_INTERVAL 20
#define TAB_START 140
#define TAB_ARRAY_NUM 4

#define CURRENT_VIEW_NAME "app_up_view"
static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *watchface_view = NULL;
void tabview_up_design(gui_view_t *view);

static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .design_cb = tabview_up_design,
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
static gui_page_t *pg = NULL;
static gui_win_t *win_animate, *win_design;
static gui_obj_t *tv_array[TAB_ARRAY_NUM] = {0};
static gui_img_t *arrow;
static bool close_flag = 0;

static void win_clear_all_cb(void)
{
    close_flag = 1;
    float percent = win_animate->animate->progress_percent;
    uint8_t index = 0;
    while (tv_array[index])
    {
        tv_array[index]->x = -percent * SCREEN_WIDTH;
        if (percent >= 1.0f)
        {
            gui_obj_tree_free(tv_array[index]);
            tv_array[index] = NULL;
            GUI_BASE(pg)->h = GUI_BASE(pg)->h - (TAB_HEIGHT + TAB_INTERVAL);
            if (index == TAB_ARRAY_NUM - 1 || tv_array[index + 1] == NULL)
            {
                gui_list_t *node = NULL;
                gui_list_t *tmp = NULL;
                // clear "win_view_more"
                gui_list_for_each_safe(node, tmp, &(GUI_BASE(current_view)->child_list))
                {
                    gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
                    if (obj->magic != GUI_MAGIC_NUMBER)
                    {
                        break;
                    }
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
        if (index == TAB_ARRAY_NUM)
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
        gui_tabview_t *tv = (gui_tabview_t *)tv_array[index];
        if (tv->cur_id.x == 0)
        {
            gui_list_t *node = NULL;
            gui_list_t *tmp = NULL;
            gui_obj_t *obj = GUI_BASE(tv);
            gui_list_for_each_safe(node, tmp, &obj->child_list)
            {
                gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
                if (obj->magic != GUI_MAGIC_NUMBER)
                {
                    return;
                }
                if (strcmp(obj->name, "tab_1") == 0)
                {
                    obj->not_show = 1;
                }
            }
        }
        index++;
        if (index == TAB_ARRAY_NUM)
        {
            break;
        }
    }
    if (index != 0)
    {
        gui_win_set_animate(win_animate, 500, 0, (gui_animate_callback_t)win_clear_all_cb, NULL);
    }
}

static void pagelist_clear(gui_page_t *parent)
{
    gui_text_t *clear_text;
    gui_canvas_round_rect_t *canvas = gui_canvas_round_rect_create(GUI_BASE(parent), "canvas_clear",
                                                                   52, 40, 305, 80, 40, gui_rgb(39, 43, 44));
    // text
    char *clear_content = "clear all";
    clear_text = gui_text_create(canvas, "clear all", 0, 0, 0, 0);
    gui_text_set(clear_text, (void *)clear_content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(clear_content),
                 32);
    gui_text_type_set(clear_text, font_size_32_bin_addr, FONT_SRC_MEMADDR);
    gui_text_mode_set(clear_text, MID_CENTER);

    gui_obj_add_event_cb(canvas, (gui_event_cb_t)tv_clear_all, GUI_EVENT_1, NULL);
}

static void win_clear_cb(gui_obj_t *obj)
{
    close_flag = 1;
    float percent = win_animate->animate->progress_percent;
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
            if (index == TAB_ARRAY_NUM - 1)
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
        if (index == TAB_ARRAY_NUM - 1)
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
            if (index == TAB_ARRAY_NUM)
            {
                break;
            }
        }
    }
}

static void tv_clear(void *obj, gui_event_t e, void *param)
{
    gui_win_set_animate(win_animate, 500, 0, (gui_animate_callback_t)win_clear_cb, obj);
    gui_obj_tree_free(param);
}

static void cancel_cb(gui_obj_t *win)
{
    // exit animation
    float percent = win_animate->animate->progress_percent;
    win->x = -percent * SCREEN_WIDTH;

    if (percent >= 1)
    {
        gui_win_start_animation(win_design);
        gui_obj_show(win, false);
        GUI_BASE(pg)->gesture = 0;
        // gui_obj_tree_free(win);
    }
}

static void view_more_cb(gui_obj_t *win)
{
    // enter animation
    gui_obj_show(win, true);
    if (win->x < -100)
    {
        win->x = (win_animate->animate->progress_percent - 1) * SCREEN_WIDTH;
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
                    gui_win_set_animate(win_animate, 500, 0, (gui_animate_callback_t)cancel_cb, win);
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
                    gui_list_t *node = NULL;
                    gui_list_t *tmp = NULL;
                    gui_obj_t *obj = GUI_BASE(win);
                    gui_list_for_each_safe(node, tmp, &obj->child_list)
                    {
                        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
                        if (obj->magic != GUI_MAGIC_NUMBER)
                        {
                            return;
                        }
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
    gui_win_stop_animation(win_design);
    GUI_BASE(pg)->gesture = 1; // stop page slide, win_design animation will restore it
    gui_win_set_animate(win_animate, 500, -1, (gui_animate_callback_t)view_more_cb, param);
}

void pagelist_create(gui_msg_t *msg)
{
    gui_view_t *view = gui_view_get_current_view();
    if (!view || strcmp(GUI_BASE(view)->name, CURRENT_VIEW_NAME) || !pg)
    {
        // gui_log("Current APP can't add tab!!! %s, %d\n", __FUNCTION__, __LINE__);
        return;
    }

    if (close_flag)
    {
        gui_log("During close animation, can't add tab!!! %s, %d\n", __FUNCTION__, __LINE__);
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
        if (index == TAB_ARRAY_NUM)
        {
            // gui_log("Can't add more tab!!! %s, %d\n", __FUNCTION__, __LINE__);
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

    gui_tabview_t *tv = gui_tabview_create(pg, "0", 0, TAB_START, SCREEN_WIDTH, 220);
    gui_tab_t *tb_inform = gui_tab_create(tv, "tab_0", 0, 0, 0, 0, 0, 0);
    gui_tab_t *tb_option = gui_tab_create(tv, "tab_1", 0, 0, 0, 0, 1, 0);
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

    gui_obj_add_event_cb(tv, (gui_event_cb_t)tv_clear, GUI_EVENT_1, (void *)win);
}

static void win_design_cb(void)
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
        if (index == TAB_ARRAY_NUM)
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
        gui_obj_tree_get_widget_by_name(&(gui_current_app()->screen), "canvas_clear", (void *)&canvas);
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
                        gui_obj_enable_event(canvas, GUI_EVENT_1);

                        gui_log("click clear\r\n");
                        return;
                    }
                }
            }
        }

        // clear tabview
        if (tp->deltaY < 30 && tp->deltaX < 30)
        {
            for (uint8_t i = 0; i < index; i++)
            {
                gui_tabview_t *tv = (gui_tabview_t *)tv_array[i];
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
                                gui_obj_enable_event(GUI_BASE(tv), GUI_EVENT_1);
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
            gui_tabview_t *tv = (gui_tabview_t *)tv_array[i];
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
            gui_tabview_t *tv = (gui_tabview_t *)tv_array[i];
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

void tabview_up_design(gui_view_t *view)
{
    clear_mem();

    gui_view_switch_on_event(view, watchface_view, VIEW_TRANSPLATION, VIEW_STILL,
                             GUI_EVENT_TOUCH_MOVE_UP);

    gui_obj_t *parent_widget = GUI_BASE(view);
    win_design = gui_win_create(parent_widget, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    // draw background
    gui_canvas_rect_t *canvas_rect = gui_canvas_rect_create(GUI_BASE(win_design), "background", 0, 0,
                                                            SCREEN_WIDTH, SCREEN_HEIGHT, gui_rgba(76, 76, 76, 255));

    pg = gui_page_create(canvas_rect, "ct_up_page", 0, 0, 0, 0);
    // draw table content
    pagelist_clear(pg);

    for (uint8_t index = 0; index < TAB_ARRAY_NUM; index++)
    {
        tv_array[index] = NULL;
    }
    // char *content =
    //     "Never gonna give you up. Never gonna let you down. Never gonna run around and desert you. Never gonna give you up. Never gonna let you down. Never gonna run around and desert you.";
    // pagelist_create("10655810010", content, "YTD morning 10:24", MESSAGE);
    // content = "Watch will attempt to install this update later tonight.";
    // pagelist_create("watchOS 10.3.1", content, "Now", OS);
    win_animate = gui_win_create(pg, "0", 0, SCREEN_WIDTH, SCREEN_HEIGHT,
                                 40);
    arrow = gui_img_create_from_mem(canvas_rect, "arrow", UI_LINE_STILL_BIN, 181, SCREEN_HEIGHT - 25, 0,
                                    0);
    gui_img_set_mode(arrow, IMG_SRC_OVER_MODE);
    gui_win_set_animate(win_design, 1000, -1, (gui_animate_callback_t)win_design_cb, NULL);
}
