#include "root_image_hongkong/ui_resource.h"
#include <gui_img.h>
#include "gui_curtainview.h"
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

#define PAGE_NAME "_page_name"
#define SCREEN_WIDTH 410
#define SCREEN_HEIGHT 502
#define SCREEN_X_OFF 21
#define SCREEN_Y_OFF 18
#define TAB_HEIGHT 220
#define TAB_INTERVAL 20
#define TAB_START 140
#define TAB_ARRAY_NUM 10

typedef enum
{
    MESSAGE = 0,
    OS,
} app_name;

static void *font_size_32_bin_addr = SOURCEHANSANSSC_SIZE32_BITS1_FONT_BIN;
static void *font_size_24_bin_addr = SOURCEHANSANSSC_SIZE24_BITS1_FONT_BIN;
static gui_page_t *pg;
static gui_win_t *win_animate, *win_design;
static gui_obj_t *tv_array[TAB_ARRAY_NUM] = {0};
static gui_curtain_t *ct_parent;

static void win_clear_all_cb(void)
{
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
        }
        index++;
    }
}

static void tv_clear_all(void *obj, gui_event_t e)
{
    uint8_t index = 0;
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
                                                                   31 + SCREEN_X_OFF, 40, 305, 80, 40, gui_rgb(39, 43, 44));
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
    float percent = win_animate->animate->progress_percent;
    obj->x = (0 - percent * 2) * SCREEN_WIDTH;
    uint8_t index = 0;
    while (1)
    {
        if (tv_array[index++] == obj)
        {
            break;
        }
    }
    if (percent >= 1.0f)
    {
        gui_obj_tree_free(obj);
        GUI_BASE(pg)->h = GUI_BASE(pg)->h - (TAB_HEIGHT + TAB_INTERVAL);
        while (1)
        {
            tv_array[index - 1] = tv_array[index];
            if (!tv_array[index])
            {
                break;
            }
            index++;
        }
    }
    if (percent > 0.5f)
    {
        while (tv_array[index])
        {
            tv_array[index]->y = TAB_START + (index - 1 + 2 * (1 - percent)) * (TAB_HEIGHT + TAB_INTERVAL);
            index++;
            // gui_log("y = %d\r\n", tv_os->y);
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
        // gui_obj_tree_free(win);
    }
}

static void view_more_cb(gui_obj_t *win)
{
    // enter animation
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
                if (tp->x > 16 + SCREEN_X_OFF && tp->x < 95 + SCREEN_X_OFF)
                {
                    gui_win_set_animate(win_animate, 500, 0, (gui_animate_callback_t)cancel_cb, win);
                }
            }
            else
            {
                uint16_t base_y = 140, h = 80, interval = 10;
                if (tp->x > 9 + SCREEN_X_OFF && tp->x < 359 + SCREEN_X_OFF)
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

void pagelist_create(const char *informer, const char *content, const char *time, app_name app)
{
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
            gui_log("Can't add more tab!!! %s, %d\n", __FUNCTION__, __LINE__);
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

    gui_tabview_t *tv = gui_tabview_create(pg, "0", 0, TAB_START, SCREEN_WIDTH, 220);
    gui_tab_t *tb_inform = gui_tab_create(tv, "tab_0", 0, 0, 0, 0, 0, 0);
    gui_tab_t *tb_option = gui_tab_create(tv, "tab_1", 0, 0, 0, 0, 1, 0);
    tv_array[0] = GUI_BASE(tv);
    // message
    gui_text_t *text_informer, *text_content, *text_time;
    gui_canvas_round_rect_t *canvas = gui_canvas_round_rect_create(GUI_BASE(tb_inform),
                                                                   "0", 9 + SCREEN_X_OFF, 0, 350, 220, 35, gui_rgb(39, 43, 44));
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
                                                                        9 + SCREEN_X_OFF, 0, 170,
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
        win = gui_win_create(ct_parent, "0", -SCREEN_WIDTH, 0, SCREEN_WIDTH,
                             SCREEN_HEIGHT);
        gui_canvas_rect_t *canvas_bg = gui_canvas_rect_create(GUI_BASE(win), "0", 0, 0,
                                                              SCREEN_WIDTH,
                                                              SCREEN_HEIGHT, gui_rgb(0, 0, 0));

        char *content = "Cancel";
        gui_text_t *text = gui_text_create(win, "cancel",  16 + SCREEN_X_OFF, 15, 80, 32);
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
                text = gui_text_create(win, "message",  16 + SCREEN_X_OFF, 70, 0, 0);
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
                text = gui_text_create(win, "0",  16 + SCREEN_X_OFF, 100, 0, 0);
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
                                                                       9 + SCREEN_X_OFF, 140, 350, 80, 15, gui_rgb(39, 43, 44));
        content = "Don't remind for an hour";
        text = gui_text_create(canvas, "text1",  0, 24, 0, 0);
        gui_text_set(text, (void *)content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                     strlen(content),
                     32);
        gui_text_type_set(text, font_size_32_bin_addr, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, CENTER);

        canvas = gui_canvas_round_rect_create(GUI_BASE(win), "canvas_2", 9 + SCREEN_X_OFF, 230, 350, 80, 15,
                                              gui_rgb(39, 43, 44));
        content = "Don't remind today";
        text = gui_text_create(canvas, "text1",  0, 24, 0, 0);
        gui_text_set(text, (void *)content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                     strlen(content),
                     32);
        gui_text_type_set(text, font_size_32_bin_addr, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, CENTER);

        canvas = gui_canvas_round_rect_create(GUI_BASE(win), "canvas_3", 9 + SCREEN_X_OFF, 320, 350, 80, 15,
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
                                                                         189 + SCREEN_X_OFF, 0,
                                                                         170, 220, 35, gui_rgb(39, 43, 44));

    text = gui_text_create(canvas_right, "0",  0, 0, 0, 0);
    gui_text_set(text, (void *)option_content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(option_content),
                 32);
    gui_text_type_set(text, font_size_32_bin_addr, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, MID_CENTER);

    gui_obj_add_event_cb(tv, (gui_event_cb_t)tv_clear, GUI_EVENT_1, win);
}

static void win_design_cb(void)
{
    gui_obj_t *obj = GUI_BASE(pg);
    bool flag = false;
    int16_t y;
    int16_t h;

    touch_info_t *tp = tp_get_info();
    static bool hold;
    if (tp->pressed)
    {
        hold = 1;
    }
    if (tp->released)
    {
        hold = 0;

        // clear all
        gui_obj_t *canvas = 0;
        gui_obj_tree_get_widget_by_name(&(gui_current_app()->screen), "canvas_clear", (void *)&canvas);
        if (tp->deltaY < 30 && tp->deltaX < 30) //prevent slide from triggering clear
        {
            if (canvas)
            {
                y = canvas->y + obj->y;
                h = canvas->h;
                if ((tp->y > y && tp->y < y + h))
                {
                    if (tp->x > 31 + SCREEN_X_OFF && tp->x < 336 + SCREEN_X_OFF)
                    {
                        gui_obj_event_set(canvas, GUI_EVENT_1);

                        gui_log("click clear\r\n");
                        return;
                    }
                }
            }
        }

        // clear tabview
        if (tp->deltaY < 30 && tp->deltaX < 30)
        {
            uint8_t index = 0;
            while (1)
            {
                if (!tv_array[index])
                {
                    break;
                }
                index++;
            }
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
                            if (tp->x > 9 + SCREEN_X_OFF && tp->x < 179 + SCREEN_X_OFF)
                            {
                                gui_obj_event_set(GUI_BASE(tv), GUI_EVENT_2);
                                // gui_log("click left\r\n");
                            }
                            else if (tp->x > 189 + SCREEN_X_OFF && tp->x < 359 + SCREEN_X_OFF)
                            {
                                gui_obj_event_set(GUI_BASE(tv), GUI_EVENT_1);
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
            gui_page_t *page = 0;
            gui_obj_tree_get_widget_by_name(&(gui_current_app()->screen), PAGE_NAME, (void *)&page);
            GUI_BASE(page)->gesture = 1;
            gui_curtain_t *c_up = 0;
            gui_obj_tree_get_widget_by_name(&(gui_current_app()->screen), "2",
                                            (void *)&c_up); //  can find "2" in "app_tb_clock.c"
            if (c_up)
            {
                GUI_BASE(c_up)->gesture = 0;
            }
            // gui_log("curtain down\n");
        }
        else
        {
            gui_page_t *page = 0;
            gui_obj_tree_get_widget_by_name(&(gui_current_app()->screen), PAGE_NAME, (void *)&page);
            GUI_BASE(page)->gesture = 0;
            gui_curtain_t *c_up = 0;
            gui_obj_tree_get_widget_by_name(&(gui_current_app()->screen), "2", (void *)&c_up);
            if (c_up)
            {
                GUI_BASE(c_up)->gesture = 1;
            }
            // gui_log("page down\n");
        }
    }

}

void tabview_up_design(void *parent_widget)
{
    ct_parent = (gui_curtain_t *)parent_widget;
    win_design = gui_win_create(parent_widget, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    // draw black background
    gui_canvas_rect_t *canvas_rect = gui_canvas_rect_create(GUI_BASE(win_design), "background", 0, 0,
                                                            SCREEN_WIDTH, SCREEN_HEIGHT, gui_rgba(255, 255, 255, 76));

    // draw bottom line
    gui_canvas_round_rect_t *canvas_line = gui_canvas_round_rect_create(GUI_BASE(win_design), "line",
                                                                        160 + SCREEN_X_OFF, SCREEN_HEIGHT - 25, 48, 6, 4, gui_rgb(39, 43, 44));

    pg = gui_page_create(win_design, PAGE_NAME, 0, 0, 0, 0);
    // draw table content
    pagelist_clear(pg);

    for (uint8_t index = 0; index < TAB_ARRAY_NUM; index++)
    {
        tv_array[index] = NULL;
    }
    char *content =
        "Never gonna give you up. Never gonna let you down. Never gonna run around and desert you. Never gonna give you up. Never gonna let you down. Never gonna run around and desert you.";
    pagelist_create("10655810010", content, "YTD morning 10:24", MESSAGE);
    // pagelist_create("10655810010", content, "YTD morning 10:24", MESSAGE);
    // pagelist_create("10655810010", content, "YTD morning 10:24", MESSAGE);
    content = "Watch will attempt to install this update later tonight.";
    pagelist_create("watchOS 10.3.1", content, "Now", OS);
    // pagelist_create("watchOS 10.3.1", content, "Now", OS);
    // pagelist_create("watchOS 10.3.1", content, "Now", OS);
    // bottom space to slide when tv all clear
    win_animate = gui_win_create(pg, "0", 0, SCREEN_HEIGHT, SCREEN_WIDTH,
                                 40);

    gui_win_set_animate(win_design, 1000, -1, (gui_animate_callback_t)win_design_cb, NULL);
}
