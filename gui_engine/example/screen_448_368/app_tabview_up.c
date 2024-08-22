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

#define PAGE_NAME "_page_name"

static void *font_size_32_bin_addr = SOURCEHANSANSSC_SIZE32_BITS4_FONT_BIN;
static void *font_size_24_bin_addr = SOURCEHANSANSSC_SIZE24_BITS4_FONT_BIN;
gui_page_t *pg;

static void canvas_cb_clear(gui_canvas_t *canvas)
{
    nvgRoundedRect(canvas->vg, 0, 0, 305, 80, 50); // the X/Y-axis coordinate relative to parent widget
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

static void canvas_cb_os_information(gui_canvas_t *canvas)
{
    nvgRoundedRect(canvas->vg, 0, 0, 350, 220, 35); // the X/Y-axis coordinate relative to parent widget
    nvgFillColor(canvas->vg, nvgRGBA(39, 43, 44, 255));
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

static void pagelist_clear(gui_page_t *parent)
{
    gui_text_t *clear_text;
    gui_canvas_t *canvas = gui_canvas_create(parent, "canvas_clear", 0, 31, 40, 368, 80);
    gui_canvas_set_canvas_cb(canvas, canvas_cb_clear);
    // text
    char *clear_content = "clear all";
    clear_text = gui_text_create(canvas, "clear all",  -31, 0, 0, 0);
    gui_text_set(clear_text, (void *)clear_content, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(clear_content),
                 32);
    gui_text_type_set(clear_text, font_size_32_bin_addr, FONT_SRC_MEMADDR);
    gui_text_mode_set(clear_text, MID_CENTER);
}

static void pagelist_message(void *parent)
{
    gui_text_t *message_number, *message_text, *message_time;

    gui_canvas_t *canvas = gui_canvas_create(parent, "canvas_message", 0, 9, 140, 350, 220);
    gui_canvas_set_canvas_cb(canvas, canvas_cb_message);

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
}

static void os_information(void *parent)
{
    gui_text_t *os_information_text, *os_information_time, *os_version_text;

    gui_canvas_t *canvas = gui_canvas_create(parent, "canvas_os_information", 0, 9, 380, 350, 220);
    gui_canvas_set_canvas_cb(canvas, canvas_cb_os_information);

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
}

static void line(void *parent)
{
    gui_canvas_t *canvas = gui_canvas_create(parent, "line", 0, 160, 620, 368, 10);
    gui_canvas_set_canvas_cb(canvas, canvas_cb_draw_line);
}

static void canvas_cb_draw_bg(gui_canvas_t *canvas)
{
    nvgRoundedRect(canvas->vg, 0, 0, 368, 448, 50); // the X/Y-axis coordinate relative to parent widget
    nvgFillColor(canvas->vg, nvgRGBA(255, 255, 255, 76));
    nvgFill(canvas->vg);
    nvgBeginPath(canvas->vg);
}

static void page_cb(gui_win_t *win)
{
    gui_obj_t *obj = GUI_BASE(pg);
    bool flag = false;

    touch_info_t *tp = (touch_info_t *)(uintptr_t)tp_get_info();
    static bool hold;
    if (tp->pressed)
    {
        hold = 1;
    }
    if (tp->released)
    {
        hold = 0;
    }
    if (hold)
    {
        /******* @down slide ************************************ @BOTTOM *******************/
        flag = tp->deltaY < 0 && obj->y != 0 &&
               obj->y <= (pg->start_y - (obj->h - (int)gui_get_screen_height()));
        if (flag || tp->y > 448 - 50)
        {
            gui_page_t *page = 0;
            gui_obj_tree_get_widget_by_name(&(gui_current_app()->screen), PAGE_NAME, (void *)&page);
            GUI_BASE(page)->gesture = 1;
            gui_curtain_t *c_up = 0;
            gui_obj_tree_get_widget_by_name(&(gui_current_app()->screen), "2", (void *)&c_up);
            if (c_up)
            {
                GUI_BASE(c_up)->gesture = 0;
            }
            win->scope = 1;
            gui_log("curtain down\n");
        }
        else
        {
            gui_page_t *page = 0;
            gui_obj_tree_get_widget_by_name(&(gui_current_app()->screen), PAGE_NAME, (void *)&page);
            GUI_BASE(page)->gesture = 0;
            // gui_curtainview_t *ct = 0;
            // gui_obj_tree_get_widget_by_name(&(gui_current_app()->screen), "ct", (void *)&ct);
            gui_curtain_t *c_up = 0;
            gui_obj_tree_get_widget_by_name(&(gui_current_app()->screen), "2", (void *)&c_up);
            // if (ct)
            // {
            //     GUI_BASE(ct)->gesture = 1;
            // }
            if (c_up)
            {
                GUI_BASE(c_up)->gesture = 1;
            }
            gui_log("page down\n");
            win->scope = 0;
        }
    }
}

void tabview_up_design(void *parent_widget)
{
    gui_win_t *win = gui_win_create(parent_widget, 0, 0, 0, 368, 448);
    win->scope = 1;

    // draw transparently black background
    gui_canvas_t *canvas = gui_canvas_create(win, "background", 0, 0, 0, 368, 448);
    gui_canvas_set_canvas_cb(canvas, canvas_cb_draw_bg);

    pg = gui_page_create(win, PAGE_NAME, 0, 0, 0, 0);
    gui_win_set_animate(win, 1000, -1, page_cb, win);
    pagelist_clear(pg);
    pagelist_message(pg);
    os_information(pg);
    line(pg);
}
