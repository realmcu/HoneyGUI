#include "root_image_hongkong/ui_resource.h"
#include <gui_img.h>
#include "gui_curtainview.h"
#include "gui_tabview.h"
#include "gui_tab.h"
#include "gui_canvas.h"

// static void canvas_cb_black(gui_canvas_t *canvas)
// {
//     nvgRect(canvas->vg, 0, -448, 368, 448 * 2);
//     nvgFillColor(canvas->vg, nvgRGBA(0, 0, 0, 150));
//     nvgFill(canvas->vg);
//     nvgBeginPath(canvas->vg);
//     nvgRoundedRect(canvas->vg, 50, 440, (368 - 50 * 2), 5, 2);
//     nvgFillColor(canvas->vg, nvgRGBA(255, 255, 255, 150));
//     nvgFill(canvas->vg);
// }
// void tabview_up_design(void *parent_widget)
// {
//     gui_canvas_t *canvas = gui_canvas_create(parent_widget, "canvas", 0, 0, 0, 368, 448);
//     gui_canvas_set_canvas_cb(canvas, canvas_cb_black);
//     gui_tabview_t *tv_up = gui_tabview_create(parent_widget, "tabview_up", 0, 0, 0, 0);
//     gui_tabview_set_style(tv_up, CLASSIC);
//     gui_tab_t *tb_control0 = gui_tab_create(tv_up, "tb_control0",    0, 0, 0, 0, 0, 0);
//     gui_tab_t *tb_control1 = gui_tab_create(tv_up, "tb_control1",    0, 0, 0, 0, 1, 0);
//     extern void page_tb_control0(void *parent);
//     extern void page_tb_control1(void *parent);
//     page_tb_control0(tb_control0);
//     page_tb_control1(tb_control1);
// }

/* Revised part */
#include "gui_win.h"
#include "gui_text.h"
#include "gui_page.h"
#include "gui_app.h"
#include "tp_algo.h"
#include "gui_canvas_img.h"

#define PAGE_NAME "_page_name"
#define SCREEN_WIDTH 368
#define SCREEN_HEIGHT 448
#define SCREEN_X_OFF 0
#define SCREEN_Y_OFF 0
#define TAB_HEIGHT 220
#define TAB_INTERVAL 20

static void *font_size_32_bin_addr = SOURCEHANSANSSC_SIZE32_BITS4_FONT_BIN;
static void *font_size_24_bin_addr = SOURCEHANSANSSC_SIZE24_BITS4_FONT_BIN;
static gui_page_t *pg;
static gui_win_t *win_clear, *win_design;
static gui_tabview_t *tv_array[2] = {0, 0};
static char *tabview_array[] =
{
    "tv_message",
    "tv_os_information"
};

typedef struct
{
    uint8_t R;
    uint8_t G;
    uint8_t B;
} view_more_RGB;
view_more_RGB rgb_1 = {39, 43, 44};
view_more_RGB rgb_2 = {39, 43, 44};
view_more_RGB rgb_3 = {39, 43, 44};
static void canvas_cb_clear(gui_canvas_t *canvas)
{
    nvgRoundedRect(canvas->vg, 31 + SCREEN_X_OFF, 0, 305, 80,
                   50); // the X/Y-axis coordinate relative to parent widget
    nvgFillColor(canvas->vg, nvgRGBA(39, 43, 44, 255));
    nvgFill(canvas->vg);
    nvgBeginPath(canvas->vg);
}

static void canvas_cb_message(gui_canvas_t *canvas)
{
    nvgRoundedRect(canvas->vg, 0, 0, 350, 220, 35); // the X/Y-axis coordinate relative to parent widget
    nvgFillColor(canvas->vg, nvgRGBA(39, 43, 44, 255));
    nvgFill(canvas->vg);
    nvgBeginPath(canvas->vg);
}

static void canvas_cb_option(gui_canvas_t *canvas)
{
    nvgRoundedRect(canvas->vg, 0, 0, 170, 220, 35); // the X/Y-axis coordinate relative to parent widget
    nvgFillColor(canvas->vg, nvgRGBA(39, 43, 44, 255));
    nvgFill(canvas->vg);
    nvgBeginPath(canvas->vg);
}

static void canvas_cb_os_information(gui_canvas_t *canvas)
{
    nvgRoundedRect(canvas->vg, 0, 0, 350, 220, 35); // the X/Y-axis coordinate relative to parent widget
    nvgFillColor(canvas->vg, nvgRGBA(39, 43, 44, 255)); //
    nvgFill(canvas->vg);
    nvgBeginPath(canvas->vg);
}
static void canvas_cb_draw_line(gui_canvas_t *canvas)
{
    nvgRoundedRect(canvas->vg, 0, 0, 48, 6, 4); // the X/Y-axis coordinate relative to parent widget
    nvgFillColor(canvas->vg, nvgRGBA(39, 43, 44, 255));
    nvgFill(canvas->vg);
    nvgBeginPath(canvas->vg);
}

static void win_clear_cb(void)
{
    float percent = win_clear->animate->progress_percent;
    if (tv_array[0] && tv_array[1])
    {
        GUI_BASE(tv_array[1])->x = -percent * SCREEN_WIDTH;
        if (percent > 0.2f)
        {
            GUI_BASE(tv_array[0])->x = -(1.25 * percent - 0.25) * SCREEN_WIDTH;
        }
        if (percent >= 1.0f)
        {
            gui_obj_tree_free(tv_array[0]);
            gui_obj_tree_free(tv_array[1]);
            tv_array[0] = 0;
            tv_array[1] = 0;
            GUI_BASE(pg)->h = GUI_BASE(pg)->h - 2 * (TAB_HEIGHT + TAB_INTERVAL);
        }
        return;
    }
    if (tv_array[0])
    {
        GUI_BASE(tv_array[0])->x = -percent * SCREEN_WIDTH;
        if (percent >= 1.0f)
        {
            gui_obj_tree_free(tv_array[0]);
            tv_array[0] = 0;
            GUI_BASE(pg)->h = GUI_BASE(pg)->h - (TAB_HEIGHT + TAB_INTERVAL);
        }
        return;
    }
    if (tv_array[1])
    {
        GUI_BASE(tv_array[1])->x = -percent * SCREEN_WIDTH;
        if (percent >= 1.0f)
        {
            gui_obj_tree_free(tv_array[1]);
            tv_array[1] = 0;
            GUI_BASE(pg)->h = GUI_BASE(pg)->h - (TAB_HEIGHT + TAB_INTERVAL);
        }
    }
}

static void tv_clear(void *obj, gui_event_t e)
{
    gui_obj_tree_get_widget_by_name(&(gui_current_app()->screen), tabview_array[0],
                                    (void *)&tv_array[0]);
    gui_obj_tree_get_widget_by_name(&(gui_current_app()->screen), tabview_array[1],
                                    (void *)&tv_array[1]);
    if (tv_array[0])
    {
        if (tv_array[0]->cur_id.x == 0)
        {
            gui_obj_t *obj = 0;
            gui_obj_tree_get_widget_by_name(&(gui_current_app()->screen), "tb_option", (void *)&obj);
            obj->not_show = 1;
        }
    }
    if (tv_array[1])
    {
        if (tv_array[1]->cur_id.x == 0)
        {
            gui_obj_t *obj = 0;
            gui_obj_tree_get_widget_by_name(&(gui_current_app()->screen), "tb_option1", (void *)&obj);
            obj->not_show = 1;
        }
    }
    if (tv_array[0] || tv_array[1])
    {
        gui_win_set_animate(win_clear, 500, 0, (gui_animate_callback_t)win_clear_cb, NULL);
    }
}

static void pagelist_clear(gui_page_t *parent)
{
    gui_text_t *clear_text;
    gui_canvas_t *canvas = gui_canvas_create(parent, "canvas_clear", 0, 0, 40, SCREEN_WIDTH, 80);
    gui_canvas_set_canvas_cb(canvas, canvas_cb_clear);
    // gui_canvas_img_t *canvas = gui_canvas_img_create(parent, "canvas_clear", 0, 0, 40, SCREEN_WIDTH, 80);
    // gui_canvas_img_set_canvas_cb(canvas, canvas_cb_clear);
    // gui_canvas_img_set_update_cb(canvas, NULL);

    // text
    char *clear_content = "clear all";
    clear_text = gui_text_create(canvas, "clear all", 0, 0, 0, 0);
    gui_text_set(clear_text, (void *)clear_content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(clear_content),
                 32);
    gui_text_type_set(clear_text, font_size_32_bin_addr, FONT_SRC_MEMADDR);
    gui_text_mode_set(clear_text, MID_CENTER);

    gui_obj_add_event_cb(canvas, (gui_event_cb_t)tv_clear, GUI_EVENT_1, NULL);
}

static void win_message_clear_cb(gui_obj_t *obj)
{
    float percent = win_clear->animate->progress_percent;
    obj->x = (0 - percent * 2) * SCREEN_WIDTH;

    if (percent >= 0.5f)
    {
        gui_obj_t *tv_os = 0;
        gui_obj_tree_get_widget_by_name(&(gui_current_app()->screen), tabview_array[1], (void *)&tv_os);
        tv_os->y = GUI_BASE(obj)->y + (TAB_HEIGHT + TAB_INTERVAL) * 2 * (1 - percent);
        gui_log("y = %d\r\n", tv_os->y);
    }
    if (percent >= 1.0f)
    {
        gui_obj_tree_free(obj);
        GUI_BASE(pg)->h = GUI_BASE(pg)->h - (TAB_HEIGHT + TAB_INTERVAL);
    }
}

static void tv_message_clear(void *obj, gui_event_t e)
{
    gui_win_set_animate(win_clear, 500, 0, (gui_animate_callback_t)win_message_clear_cb, obj);
}

static void canvas_cb_message_bg(gui_canvas_t *canvas)
{
    nvgRoundedRect(canvas->vg, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
                   50); // the X/Y-axis coordinate relative to parent widget
    nvgFillColor(canvas->vg, nvgRGBA(1, 1, 1, 255));
    nvgFill(canvas->vg);
    nvgBeginPath(canvas->vg);
}


static void canvas_cb_view_more_1(gui_canvas_t *canvas)
{
    nvgRoundedRect(canvas->vg, 9 + SCREEN_X_OFF, 0, 350, 80,
                   15); // the X/Y-axis coordinate relative to parent widget
    nvgFillColor(canvas->vg, nvgRGBA(rgb_1.R, rgb_1.G, rgb_1.B, 255)); //R G B
    nvgFill(canvas->vg);
    nvgBeginPath(canvas->vg);
}
static void canvas_cb_view_more_2(gui_canvas_t *canvas)
{
    nvgRoundedRect(canvas->vg, 9 + SCREEN_X_OFF, 0, 350, 80,
                   15); // the X/Y-axis coordinate relative to parent widget
    nvgFillColor(canvas->vg, nvgRGBA(rgb_2.R, rgb_2.G, rgb_2.B, 255)); //R G B
    nvgFill(canvas->vg);
    nvgBeginPath(canvas->vg);
}
static void canvas_cb_view_more_3(gui_canvas_t *canvas)
{
    nvgRoundedRect(canvas->vg, 9 + SCREEN_X_OFF, 0, 350, 80,
                   15); // the X/Y-axis coordinate relative to parent widget
    nvgFillColor(canvas->vg, nvgRGBA(rgb_3.R, rgb_3.G, rgb_3.B, 255)); //R G B
    nvgFill(canvas->vg);
    nvgBeginPath(canvas->vg);
}

static void message_cancel_cb(gui_obj_t *win)
{
    // exit animation
    float percent = win_clear->animate->progress_percent;
    win->x = -percent * SCREEN_WIDTH;

    if (percent >= 1)
    {
        gui_win_start_animation(win_design);
        gui_obj_tree_free(win);
    }
}

static void message_more_cb(gui_obj_t *win)
{
    // enter animation
    if (win->x < -(80 + SCREEN_X_OFF))
    {
        win->x = (win_clear->animate->progress_percent - 1) * SCREEN_WIDTH;
    }
    else
    {
        win->x = 0;
    }

    touch_info_t *tp = tp_get_info();
    if (tp->released)
    {
        if ((tp->y > 15 && tp->y < 47))
        {
            if (tp->x > 16 + SCREEN_X_OFF && tp->x < 95 + SCREEN_X_OFF)
            {
                gui_win_set_animate(win_clear, 500, 0, (gui_animate_callback_t)message_cancel_cb, win);
            }
        }
        else
        {
            uint16_t base_y = 140, h = 80, interval = 10;
            if (tp->x > 9 + SCREEN_X_OFF && tp->x < 359 + SCREEN_X_OFF)
            {
                uint16_t y = base_y;
                view_more_RGB temp = {255, 255, 255};
                if ((tp->y > y && tp->y < y + h))
                {
                    gui_log("click canvas1\r\n");
                    rgb_1 = temp;
                    return;
                }
                y = y + h + interval;
                if ((tp->y > y && tp->y < y + h))
                {
                    gui_log("click canvas2\r\n");
                    rgb_2 = temp;
                    return;
                }
                y = y + h + interval;
                if ((tp->y > y && tp->y < y + h))
                {
                    gui_log("click canvas3\r\n");
                    rgb_3 = temp;
                    return;
                }
            }
        }
    }
    rgb_1.R = 39;
    rgb_1.G = 43;
    rgb_1.B = 44;
    rgb_2 = rgb_1;
    rgb_3 = rgb_1;
}
// ... content
static void canvas_cb_draw_bg(gui_canvas_t *canvas);
static void view_more(gui_obj_t *obj)
{
    gui_win_stop_animation(win_design);
    GUI_BASE(pg)->gesture = 1; // stop page slide, win_design animation will restore it

    gui_obj_t *parent = 0;
    gui_obj_tree_get_widget_by_name(&(gui_current_app()->screen), "2", (void *)&parent);
    if (parent)
    {
        gui_win_t *win = gui_win_create(parent, "message_win", -SCREEN_WIDTH, 0, SCREEN_WIDTH,
                                        SCREEN_HEIGHT);
        gui_canvas_t *canvas_bg = gui_canvas_create(win, "message_background", 0, 0, 0, SCREEN_WIDTH,
                                                    SCREEN_HEIGHT);
        gui_canvas_set_canvas_cb(canvas_bg, canvas_cb_message_bg);
        // gui_canvas_img_t *canvas_bg = gui_canvas_img_create(win, "message_background", 0, 0, 0, SCREEN_WIDTH,
        //                                             SCREEN_HEIGHT);
        // gui_canvas_img_set_canvas_cb(canvas_bg, canvas_cb_message_bg);
        // gui_canvas_img_set_update_cb(canvas_bg, NULL);

        char *content = "Cancel";
        gui_text_t *text = gui_text_create(win, "cancel",  16 + SCREEN_X_OFF, 15, 80, 32);
        gui_text_set(text, (void *)content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                     strlen(content),
                     32);
        gui_text_type_set(text, font_size_32_bin_addr, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, LEFT);
        content = "00:00";
        text = gui_text_create(win, "time",  -(16 + SCREEN_X_OFF), 15, 0, 0);
        gui_text_set(text, (void *)content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                     strlen(content),
                     32);
        gui_text_type_set(text, font_size_32_bin_addr, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, RIGHT);

        if (obj->name == tabview_array[0])
        {
            content = "\"message\" dialog\n10655810010" ;
            text = gui_text_create(win, "message",  16 + SCREEN_X_OFF, 70, 0, 0);
            gui_text_set(text, (void *)content, GUI_FONT_SRC_BMP, gui_rgba(153, 153, 153, UINT8_MAX),
                         strlen(content),
                         24);
            gui_text_type_set(text, font_size_24_bin_addr, FONT_SRC_MEMADDR);
            gui_text_mode_set(text, MULTI_LEFT);

            gui_img_create_from_mem(win, "view_more person", UI_PERSON_ICON_BIN, 279 + SCREEN_X_OFF, 20, 0, 0);
        }
        else
        {
            content = "\"OS\" dialog" ;
            text = gui_text_create(win, "message",  16 + SCREEN_X_OFF, 100, 0, 0);
            gui_text_set(text, (void *)content, GUI_FONT_SRC_BMP, gui_rgba(153, 153, 153, UINT8_MAX),
                         strlen(content),
                         24);
            gui_text_type_set(text, font_size_24_bin_addr, FONT_SRC_MEMADDR);
            gui_text_mode_set(text, LEFT);

            gui_img_create_from_mem(win, "view_more watch", UI_IWATCH_32_ICON_BIN, 279 + SCREEN_X_OFF, 20, 0,
                                    0);
        }

        gui_canvas_t *canvas = gui_canvas_create(win, "canvas_1", 0, 0, 140, SCREEN_WIDTH, 80);
        gui_canvas_set_canvas_cb(canvas, canvas_cb_view_more_1);
        // gui_canvas_img_t *canvas = gui_canvas_img_create(win, "canvas_1", 0, 0, 140, SCREEN_WIDTH, 80);
        // gui_canvas_set_canvas_cb(canvas, canvas_cb_view_more_1);
        // gui_canvas_img_set_update_cb(canvas, NULL);

        content = "Don't remind for an hour";
        text = gui_text_create(canvas, "text1",  0, 24, 0, 0);
        gui_text_set(text, (void *)content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                     strlen(content),
                     32);
        gui_text_type_set(text, font_size_32_bin_addr, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, CENTER);

        canvas = gui_canvas_create(win, "canvas_2", 0, 0, 230, SCREEN_WIDTH, 80);
        gui_canvas_set_canvas_cb(canvas, canvas_cb_view_more_2);
        // canvas = gui_canvas_img_create(win, "canvas_2", 0, 0, 230, SCREEN_WIDTH, 80);
        // gui_canvas_set_canvas_cb(canvas, canvas_cb_view_more_2);
        // gui_canvas_img_set_update_cb(canvas, NULL);

        content = "Don't remind today";
        text = gui_text_create(canvas, "text1",  0, 24, 0, 0);
        gui_text_set(text, (void *)content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                     strlen(content),
                     32);
        gui_text_type_set(text, font_size_32_bin_addr, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, CENTER);

        canvas = gui_canvas_create(win, "canvas_3", 0, 0, 320, SCREEN_WIDTH, 80);
        gui_canvas_set_canvas_cb(canvas, canvas_cb_view_more_3);
        // canvas = gui_canvas_img_create(win, "canvas_3", 0, 0, 320, SCREEN_WIDTH, 80);
        // gui_canvas_set_canvas_cb(canvas, canvas_cb_view_more_3);
        // gui_canvas_img_set_update_cb(canvas, NULL);

        content = "Add this to Summary";
        text = gui_text_create(canvas, "text1",  0, 24, 0, 0);
        gui_text_set(text, (void *)content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                     strlen(content),
                     32);
        gui_text_type_set(text, font_size_32_bin_addr, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, CENTER);


        gui_win_set_animate(win_clear, 500, -1, (gui_animate_callback_t)message_more_cb, win);
    }
}

static void tv_message_more(void *obj, gui_event_t e)
{
    view_more(obj);
    gui_log("enter view more\r\n");
}

static void pagelist_message(void *parent)
{
    gui_tabview_t *tv = gui_tabview_create(parent, tabview_array[0], 0, 140, SCREEN_WIDTH, 220);
    gui_tab_t *tb_message = gui_tab_create(tv, "tb_message", 0, 0, 0, 0, 0, 0);
    gui_tab_t *tb_option = gui_tab_create(tv, "tb_option", 0, 0, 0, 0, 1, 0);

    // message
    gui_text_t *message_number, *message_text, *message_time;

    gui_canvas_t *canvas = gui_canvas_create(tb_message, "canvas_message", 0, 9 + SCREEN_X_OFF, 0, 350,
                                             220);
    gui_canvas_set_canvas_cb(canvas, canvas_cb_message);
    // gui_canvas_img_t *canvas = gui_canvas_img_create(tb_message, "canvas_message", 0, 9 + SCREEN_X_OFF, 0, 350, 220);
    // gui_canvas_img_set_canvas_cb(canvas, canvas_cb_message);
    // gui_canvas_img_set_update_cb(canvas, NULL);

    gui_img_create_from_mem(canvas, "message icon", UI_MESSAGE_ICON_BIN, 10, -15, 0, 0);

    // text
    char *message_time_content = "YTD morning 10:24";
    message_time = gui_text_create(canvas, "message_time",  -15, 15, 0, 0);
    gui_text_set(message_time, (void *)message_time_content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(message_time_content),
                 24);
    gui_text_type_set(message_time, font_size_24_bin_addr, FONT_SRC_MEMADDR);
    gui_text_mode_set(message_time, RIGHT);

    char *message_number_content = "10655810010";
    message_number = gui_text_create(canvas, "message_number",  10, 50, 0, 0);
    gui_text_set(message_number, (void *)message_number_content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(message_number_content),
                 32);
    gui_text_type_set(message_number, font_size_32_bin_addr, FONT_SRC_MEMADDR);
    gui_text_mode_set(message_number, LEFT);

    char *message_text_content =
        "Never gonna give you up. Never gonna let you down. Never gonna run around and desert you.";
    message_text = gui_text_create(canvas, "message_number",  10, 75, 340, 0);
    gui_text_set(message_text, (void *)message_text_content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(message_text_content),
                 32);
    gui_text_type_set(message_text, font_size_32_bin_addr, FONT_SRC_MEMADDR);
    gui_text_mode_set(message_text, MULTI_LEFT);
    gui_text_wordwrap_set(message_text, true);

    // option left
    char *option_content = "...";
    canvas = gui_canvas_create(tb_option, "message_left", 0, 9 + SCREEN_X_OFF, 0, 170, 220);
    gui_canvas_set_canvas_cb(canvas, canvas_cb_option);
    // canvas = gui_canvas_img_create(tb_option, "message_left", 0, 9 + SCREEN_X_OFF, 0, 170, 220);
    // gui_canvas_img_set_canvas_cb(canvas, canvas_cb_option);
    // gui_canvas_img_set_update_cb(canvas, NULL);

    gui_text_t *text = gui_text_create(canvas, "canvas_option",  0, 0, 0, 0);
    gui_text_set(text, (void *)option_content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(option_content),
                 32);
    gui_text_type_set(text, font_size_32_bin_addr, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, MID_CENTER);

    //option right
    option_content = "X";
    canvas = gui_canvas_create(tb_option, "message_right", 0, 189 + SCREEN_X_OFF, 0, 170, 220);
    gui_canvas_set_canvas_cb(canvas, canvas_cb_option);
    // canvas = gui_canvas_img_create(tb_option, "message_right", 0, 189 + SCREEN_X_OFF, 0, 170, 220);
    // gui_canvas_img_set_canvas_cb(canvas, canvas_cb_option);
    // gui_canvas_img_set_update_cb(canvas, NULL);

    text = gui_text_create(canvas, "canvas_option",  0, 0, 0, 0);
    gui_text_set(text, (void *)option_content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(option_content),
                 32);
    gui_text_type_set(text, font_size_32_bin_addr, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, MID_CENTER);

    gui_obj_add_event_cb(tv, (gui_event_cb_t)tv_message_clear, GUI_EVENT_1, tv);
    gui_obj_add_event_cb(tv, (gui_event_cb_t)tv_message_more, GUI_EVENT_2, tv);
}

static void win_os_clear_cb(gui_obj_t *obj)
{
    float percent = win_clear->animate->progress_percent;
    obj->x = (0 - percent * 2) * SCREEN_WIDTH;
    if (percent >= 1.0f)
    {
        gui_obj_tree_free(obj);
        GUI_BASE(pg)->h = GUI_BASE(pg)->h - (TAB_HEIGHT + TAB_INTERVAL);
    }
}

static void tv_os_information_clear(void *obj, gui_event_t  e)
{
    gui_win_set_animate(win_clear, 250, 0, (gui_animate_callback_t)win_os_clear_cb, obj);
}

static void tv_os_information_more(void *obj, gui_event_t e)
{
    view_more(obj);
}

static void os_information(void *parent)
{
    gui_tabview_t *tv = gui_tabview_create(parent, tabview_array[1], 0, 380, SCREEN_WIDTH, 220);
    gui_tab_t *tb_message = gui_tab_create(tv, "tb_message1", 0, 0, SCREEN_WIDTH, 220, 0, 0);
    gui_tab_t *tb_option = gui_tab_create(tv, "tb_option1", 0, 0, SCREEN_WIDTH, 220, 1, 0);

    gui_text_t *os_information_text, *os_information_time, *os_version_text;

    gui_canvas_t *canvas = gui_canvas_create(tb_message, "canvas_os_information", 0, 9 + SCREEN_X_OFF,
                                             0, 350, 220);
    gui_canvas_set_canvas_cb(canvas, canvas_cb_os_information);
    // gui_canvas_img_t *canvas = gui_canvas_img_create(tb_message, "canvas_os_information", 0, 9 + SCREEN_X_OFF, 0, 350, 220);
    // gui_canvas_img_set_canvas_cb(canvas, canvas_cb_os_information);
    // gui_canvas_img_set_update_cb(canvas, NULL);

    gui_img_create_from_mem(canvas, "watch icon", UI_IWATCH_ICON_BIN, 10, -10, 0, 0);
    // text
    char *time_content = "NOW";
    os_information_time = gui_text_create(canvas, "message_time",  -15, 15, 0, 0);
    gui_text_set(os_information_time, (void *)time_content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(time_content),
                 24);
    gui_text_type_set(os_information_time, font_size_24_bin_addr, FONT_SRC_MEMADDR);
    gui_text_mode_set(os_information_time, RIGHT);

    char *os_version_content = "watchOS 10.3.1";
    os_version_text = gui_text_create(canvas, "message_number",  10, 50, 0, 0);
    gui_text_set(os_version_text, (void *)os_version_content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(os_version_content),
                 32);
    gui_text_type_set(os_version_text, font_size_32_bin_addr, FONT_SRC_MEMADDR);
    gui_text_mode_set(os_version_text, LEFT);

    char *os_information_content = "Apple Watch will attempt to install this update later tonight.";
    os_information_text = gui_text_create(canvas, "message_number",  10, 75, 340, 0);
    gui_text_set(os_information_text, (void *)os_information_content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(os_information_content),
                 32);
    gui_text_type_set(os_information_text, font_size_32_bin_addr, FONT_SRC_MEMADDR);
    gui_text_mode_set(os_information_text, MULTI_LEFT);
    gui_text_wordwrap_set(os_information_text, true);

    // option left
    char *option_content = "...";
    canvas = gui_canvas_create(tb_option, "message_left1", 0, 9 + SCREEN_X_OFF, 0, 170, 220);
    gui_canvas_set_canvas_cb(canvas, canvas_cb_option);
    // canvas = gui_canvas_img_create(tb_option, "message_left1", 0, 9 + SCREEN_X_OFF, 0, 170, 220);
    // gui_canvas_img_set_canvas_cb(canvas, canvas_cb_option);
    // gui_canvas_img_set_update_cb(canvas, NULL);

    gui_text_t *text = gui_text_create(canvas, "canvas_option1",  0, 0, 0, 0);
    gui_text_set(text, (void *)option_content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(option_content),
                 32);
    gui_text_type_set(text, font_size_32_bin_addr, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, MID_CENTER);

    //option right
    option_content = "X";
    canvas = gui_canvas_create(tb_option, "message_right1", 0, 189 + SCREEN_X_OFF, 0, 170, 220);
    gui_canvas_set_canvas_cb(canvas, canvas_cb_option);
    // canvas = gui_canvas_img_create(tb_option, "message_right1", 0, 189 + SCREEN_X_OFF, 0, 170, 220);
    // gui_canvas_img_set_canvas_cb(canvas, canvas_cb_option);
    // gui_canvas_img_set_update_cb(canvas, NULL);

    text = gui_text_create(canvas, "canvas_option1",  0, 0, 0, 0);
    gui_text_set(text, (void *)option_content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(option_content),
                 32);
    gui_text_type_set(text, font_size_32_bin_addr, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, MID_CENTER);

    gui_obj_add_event_cb(tv, (gui_event_cb_t)tv_os_information_clear, GUI_EVENT_1, tv);
    gui_obj_add_event_cb(tv, (gui_event_cb_t)tv_os_information_more, GUI_EVENT_2, tv);
}

static void canvas_cb_draw_bg(gui_canvas_t *canvas)
{
    nvgRoundedRect(canvas->vg, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
                   50); // the X/Y-axis coordinate relative to parent widget
    nvgFillColor(canvas->vg, nvgRGBA(255, 255, 255, 76));
    nvgFill(canvas->vg);
    nvgBeginPath(canvas->vg);
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
            gui_tabview_t *tv = 0;
            for (uint8_t i = 0; i < sizeof(tabview_array) / sizeof(*tabview_array); i++)
            {
                gui_obj_tree_get_widget_by_name(&(gui_current_app()->screen), tabview_array[i], (void *)&tv);
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
    win_design = gui_win_create(parent_widget, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    // draw transparently black background
    gui_canvas_t *canvas = gui_canvas_create(win_design, "background", 0, 0, 0, SCREEN_WIDTH,
                                             SCREEN_HEIGHT);
    gui_canvas_set_canvas_cb(canvas, canvas_cb_draw_bg);
    // gui_canvas_img_t *canvas = gui_canvas_img_create(win_design, "background", 0, 0, 0, SCREEN_WIDTH,
    //                                          SCREEN_HEIGHT);
    // gui_canvas_img_set_canvas_cb(canvas, canvas_cb_draw_bg);
    // gui_canvas_img_set_update_cb(canvas, NULL);

    // draw bottom line
    canvas = gui_canvas_create(win_design, "line", 0, 160 + SCREEN_X_OFF, SCREEN_HEIGHT - 25,
                               SCREEN_WIDTH, 10);
    gui_canvas_set_canvas_cb(canvas, canvas_cb_draw_line);
    // canvas = gui_canvas_img_create(win_design, "line", 0, 160 + SCREEN_X_OFF, SCREEN_HEIGHT - 25, SCREEN_WIDTH, 10);
    // gui_canvas_img_set_canvas_cb(canvas, canvas_cb_draw_line);
    // gui_canvas_img_set_update_cb(canvas, NULL);

    pg = gui_page_create(win_design, PAGE_NAME, 0, 0, 0, 0);
    // draw table content
    pagelist_clear(pg);
    pagelist_message(pg);
    os_information(pg);

    // bottom space to slide when tv all clear
    win_clear = gui_win_create(pg, "spare", 0, SCREEN_HEIGHT, SCREEN_WIDTH,
                               40);

    gui_win_set_animate(win_design, 1000, -1, (gui_animate_callback_t)win_design_cb, NULL);
}
