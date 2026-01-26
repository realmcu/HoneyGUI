/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <time.h>
#include "gui_list.h"
#include "gui_api.h"
#include "root_image/ui_resource.h"
#include "guidef.h"
#include "gui_img.h"
#include "gui_text.h"
#include "gui_canvas.h"
#include "gui_rect.h"
#include "app_main_watch.h"
#include "gui_view_instance.h"
#include "tp_algo.h"

/*============================================================================*
 *                           Types
 *============================================================================*/
typedef enum
{
    ALL_TIMERS = 0,
    ACTIVE_SINGLE,
    ACTIVE_MULTI,
    TIMER_END,
    CUSTOM_ADD,
} TIMER_PAGE;

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "timers_view"

#define SCREEN_WIDTH  (int16_t)gui_get_screen_width()
#define SCREEN_HEIGHT (int16_t)gui_get_screen_height()

#define COLOR_DEFAULT   GUI_COLOR_ARGB8888(0xFF, 0x1F, 0x1F, 0x1F) //1F1F1F
#define COLOR_THEME     GUI_COLOR_ARGB8888(0xFF, 0xF1, 0x9A, 0x38) //F19A38

#define PRESET_TIMER_NUM_MAX 12
#define ACTIVE_TIMER_NUM_MAX 5

#ifdef M_PI
#undef M_PI
#define M_PI 3.1415926f
#else
#define M_PI 3.1415926f
#endif

#define RADIUS_BIG   172
#define RADIUS_SMALL 36

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void timers_design_init(gui_view_t *view);
static void clear_mem(gui_view_t *view);

static void show_page(gui_obj_t *parent);

static void page_active_single_design(gui_obj_t *parent);
static void page_all_timers_design(gui_obj_t *parent);


/*============================================================================*
 *                           GUI_VIEW_INSTANCE
 *============================================================================*/
GUI_VIEW_INSTANCE(CURRENT_VIEW_NAME, false, timers_design_init, clear_mem);

/*============================================================================*
 *                            Variables
 *============================================================================*/
TIMER_PAGE timer_page = ALL_TIMERS;
static bool edit_mode = false;

static void **img_data_big = NULL;
static void **img_data_small = NULL;

static uint32_t preset_timer_val[PRESET_TIMER_NUM_MAX] = {60, 180, 300, 600, 900, 1800, 3600, 7200, 10800};
static char **preset_timer_str = NULL;
static uint8_t preset_timer_num = 9;
static uint32_t *active_timer_val = NULL;
static char **active_timer_str = NULL;
static uint8_t active_timer_num = 0;
static float *active_timer_rec_val = NULL;
static char **active_timer_rec_str = NULL;
static bool active_timer_status[ACTIVE_TIMER_NUM_MAX] = {false};
static char timer_end_str[10] = {0};
static uint32_t timer_end_val = 0;

static const char num_str[] = \
                              "000102030405060708091011121314151617181920212223242526272829\
303132333435363738394041424344454647484950515253545556575859";

static int page_all_timers_list_offset_his = 0;
static int page_active_multi_list_offset_his = 0;
static int page_active_single_list_offset_his = 0;

static float arc_end = 0;

static uint8_t hour_index = 0;
static uint8_t min_index = 0;
static uint8_t sec_index = 0;

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static void clear_mem(gui_view_t *view)
{
    GUI_UNUSED(view);
    timer_page = ALL_TIMERS;
    if (active_timer_val)
    {
        gui_free(active_timer_val);
        active_timer_val = NULL;
    }
    if (active_timer_rec_val)
    {
        gui_free(active_timer_rec_val);
        active_timer_rec_val = NULL;
    }
    if (preset_timer_str)
    {
        for (uint8_t i = 0; i < PRESET_TIMER_NUM_MAX; i++)
        {
            if (preset_timer_str[i])
            {
                gui_free(preset_timer_str[i]);
            }
        }
        gui_free(preset_timer_str);
        preset_timer_str = NULL;
    }
    if (active_timer_str)
    {
        for (uint8_t i = 0; i < ACTIVE_TIMER_NUM_MAX; i++)
        {
            if (active_timer_str[i])
            {
                gui_free(active_timer_str[i]);
                gui_free(active_timer_rec_str[i]);
            }
        }
        gui_free(active_timer_str);
        gui_free(active_timer_rec_str);
        active_timer_str = NULL;
        active_timer_rec_str = NULL;
    }
    if (img_data_big)
    {
        for (uint8_t i = 0; i < ACTIVE_TIMER_NUM_MAX; i++)
        {
            if (img_data_big[i])
            {
                gui_free(img_data_big[i]);
            }
        }
        gui_free(img_data_big);
        img_data_big = NULL;
    }
    if (img_data_small)
    {
        for (uint8_t i = 0; i < ACTIVE_TIMER_NUM_MAX; i++)
        {
            if (img_data_small[i])
            {
                gui_free(img_data_small[i]);
            }
        }
        gui_free(img_data_small);
        img_data_small = NULL;
    }
}

static void click_button_cancel(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);

    if (timer_page == ACTIVE_SINGLE)
    {
        if (active_timer_num)
        {
            gui_list_t *list = (void *)gui_list_entry(GUI_BASE(obj)->parent->child_list.next, gui_obj_t,
                                                      brother_list);
            if (list->offset % SCREEN_HEIGHT) { return; }
            int16_t index = list->last_created_note_index;
            active_timer_num--;
            for (uint8_t i = index; i < active_timer_num; i++)
            {
                active_timer_rec_val[i] = active_timer_rec_val[i + 1];
                active_timer_val[i] = active_timer_val[i + 1];
                active_timer_status[i] = active_timer_status[i + 1];
            }
            if (active_timer_num < 2)
            {
                timer_page = ACTIVE_SINGLE;
                page_active_single_list_offset_his = -active_timer_num * SCREEN_HEIGHT;
            }
            else
            {
                timer_page = ACTIVE_MULTI;
            }
        }
        else
        {
            timer_page = ALL_TIMERS;
        }
    }
    else
    {
        if (active_timer_num > 1)
        {
            timer_page = ACTIVE_MULTI;
        }
        else
        {
            timer_page = ACTIVE_SINGLE;
        }
    }
    show_page((void *)gui_view_get_current());
}

static void click_button_back(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    if (timer_page == ACTIVE_SINGLE)
    {
        timer_page = ACTIVE_MULTI;
    }
    else
    {
        timer_page = ALL_TIMERS;
    }
    show_page((void *)gui_view_get_current());
}

static void click_button_add(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    timer_page = ALL_TIMERS;
    show_page((void *)gui_view_get_current());
}

static void click_button_reset(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    timer_page = ACTIVE_SINGLE;
    active_timer_rec_val[active_timer_num] = timer_end_val;
    active_timer_num++;
    page_active_single_list_offset_his = -active_timer_num * SCREEN_HEIGHT;
    show_page((void *)gui_view_get_current());
}

static void click_button_delete(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    gui_img_t *bg = (void *)gui_list_entry(GUI_BASE(obj)->brother_list.next, gui_obj_t, brother_list);
    if (bg->t_x > -153.f) { return; }

    gui_list_note_t *note = (void *)GUI_BASE(obj)->parent;
    int16_t index = note->index;
    active_timer_num--;
    for (uint8_t i = index; i < active_timer_num; i++)
    {
        active_timer_rec_val[i] = active_timer_rec_val[i + 1];
        active_timer_val[i] = active_timer_val[i + 1];
        active_timer_status[i] = active_timer_status[i + 1];
    }
    if (active_timer_num <= 1)
    {
        timer_page = ACTIVE_SINGLE;
        page_active_single_list_offset_his = -active_timer_num * SCREEN_HEIGHT;
    }
    show_page((void *)gui_view_get_current());
}

static void click_button_stop(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    if (timer_page == ACTIVE_SINGLE)
    {
        gui_list_t *list = (void *)gui_list_entry(GUI_BASE(obj)->parent->child_list.next, gui_obj_t,
                                                  brother_list);
        if (list->offset % SCREEN_HEIGHT) { return; }
        int16_t index = list->last_created_note_index;

        gui_img_t *icon = (void *)gui_list_entry(GUI_BASE(obj)->child_list.next, gui_obj_t, brother_list);
        active_timer_status[index] = !active_timer_status[index];
        if (!active_timer_status[index])
        {
            gui_img_set_image_data(icon, STOPWATCH_BUTTON_START_BIN);
            gui_img_refresh_size(icon);
            gui_img_a8_recolor(icon, 0xFFFFFFFF);
        }
        else
        {
            gui_img_set_image_data(icon, TIMER_BUTTON_STOP_BIN);
            gui_img_refresh_size(icon);
            gui_img_a8_recolor(icon, 0xFF000000);
        }
    }
    else
    {
        gui_list_note_t *note = (void *)GUI_BASE(obj)->parent->parent;
        int16_t index = note->index;
        gui_img_t *icon = (void *)gui_list_entry(GUI_BASE(obj)->child_list.prev, gui_obj_t, brother_list);
        active_timer_status[index] = !active_timer_status[index];
        if (!active_timer_status[index])
        {
            gui_img_set_image_data(icon, STOPWATCH_BUTTON_START_BIN);
            gui_img_refresh_size(icon);
            gui_img_a8_recolor(icon, 0xFFFFFFFF);
        }
        else
        {
            gui_img_set_image_data(icon, TIMER_BUTTON_STOP_BIN);
            gui_img_refresh_size(icon);
            gui_img_a8_recolor(icon, COLOR_THEME);
        }
    }
}

static void time_update_cb(void *obj)
{
    gui_text_content_set(obj, time_str, strlen(time_str));
}

static void update_timer_str(char *str, uint32_t rec_val, uint32_t act_val)
{
    uint8_t min = (rec_val % 3600) / 60;
    uint8_t sec = rec_val % 60;
    if (act_val >= 3600)
    {
        uint8_t hour = rec_val / 3600;
        sprintf(str, "%02u:%02u:%02u", hour, min, sec);
    }
    else
    {
        sprintf(str, "%02u:%02u", min, sec);
    }
}

static void indicator_update_cb(void *obj)
{
    gui_list_t *list = obj;
    static uint8_t cnt = 0;
    static int list_offset_his = 0;
    gui_obj_t *win = gui_list_entry(list->base.brother_list.next, gui_obj_t, brother_list);
    if (list_offset_his != list->offset)
    {
        list_offset_his = list->offset;
        uint32_t color = 0xFF808080;
        gui_obj_t *page_indicator = gui_list_entry(win->child_list.next, gui_obj_t, brother_list);
        if (list_offset_his >= -(SCREEN_HEIGHT / 2)) { color = 0xFFFFFFFF; }
        gui_img_a8_recolor((void *)page_indicator, color);
        for (uint8_t i = 1; i < active_timer_num; i++)
        {
            page_indicator = gui_list_entry(page_indicator->brother_list.next, gui_obj_t, brother_list);
            if (list_offset_his >= -(SCREEN_HEIGHT / 2) - SCREEN_HEIGHT * i &&
                list_offset_his < -(SCREEN_HEIGHT / 2) - SCREEN_HEIGHT * (i - 1))
            {
                color = 0xFFFFFFFF;
            }
            else
            {
                color = 0xFF808080;
            }
            gui_img_a8_recolor((void *)page_indicator, color);
        }
        gui_obj_hidden(win, false);
    }
    else
    {
        cnt++;
        if (cnt >= 50) //1s
        {
            cnt = 0;
            gui_obj_hidden(win, true);
        }
    }
}

static void draw_arc_big_cb(NVGcontext *vg)
{
    const float line_width = 10;
    const float radius = (float)RADIUS_BIG;

    if (timer_page == TIMER_END)
    {
        nvgBeginPath(vg);
        nvgRect(vg, 0, 0, radius * 2, radius * 2);
        nvgFillColor(vg, nvgRGB(0xC3, 0x50, 0x21));
        nvgFill(vg);
    }

    nvgBeginPath(vg);
    nvgArc(vg, radius, radius, radius - line_width, -M_PI / 2,
           M_PI * (2.0f * arc_end - 0.5f), NVG_CW);
    nvgStrokeWidth(vg, line_width);
    nvgStrokeColor(vg, nvgRGB(0xE0, 0x87, 0x31));
    nvgStroke(vg);
}

/* Page active single */
static void note_active_single_timer_cb(void *obj)
{
    gui_list_note_t *note = obj;
    gui_list_t *list = (void *)note->base.parent;
    int16_t index = note->index;
    if (index != list->last_created_note_index || !active_timer_status[index]) { return; }
    gui_obj_t *t = gui_list_entry(note->base.child_list.prev, gui_obj_t, brother_list);

    update_timer_str(active_timer_rec_str[index], (uint32_t)(active_timer_rec_val[index] + 1),
                     active_timer_val[index]);
    gui_text_content_set((void *)t, active_timer_rec_str[index], strlen(active_timer_rec_str[index]));
    int image_h = RADIUS_BIG * 2;
    int image_w = RADIUS_BIG * 2;
    int pixel_bytes = 2;
    size_t buffer_size = image_h * image_w * pixel_bytes + sizeof(gui_rgb_data_head_t);
    memset(img_data_big[index], 0, buffer_size);
    arc_end = active_timer_rec_val[index] / active_timer_val[index];
    gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGB565, 0, image_w, image_h, draw_arc_big_cb,
                                      img_data_big[index]);
}


static void note_active_single_design(gui_obj_t *obj, void *p)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(p);

    gui_list_note_t *note = (void *)obj;
    int16_t index = note->index;
    gui_obj_create_timer(obj, 200, -1, note_active_single_timer_cb);
    gui_obj_start_timer(obj);

    if (img_data_big == NULL)
    {
        img_data_big = gui_malloc(ACTIVE_TIMER_NUM_MAX * sizeof(void *));
        memset(img_data_big, 0, ACTIVE_TIMER_NUM_MAX * sizeof(void *));
    }
    int image_h = RADIUS_BIG * 2;
    int image_w = RADIUS_BIG * 2;
    int pixel_bytes = 2;
    size_t buffer_size = image_h * image_w * pixel_bytes + sizeof(gui_rgb_data_head_t);
    if (img_data_big[index] == NULL)
    {
        img_data_big[index] = gui_malloc(buffer_size);
    }
    memset(img_data_big[index], 0, buffer_size);
    arc_end = active_timer_rec_val[index] / active_timer_val[index];
    gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGB565, 0, image_w, image_h, draw_arc_big_cb,
                                      img_data_big[index]);
    gui_img_create_from_mem(obj, 0, img_data_big[index], 33, 79, 0, 0);

    update_timer_str(active_timer_str[index], active_timer_val[index], active_timer_val[index]);
    gui_text_t *t_act = gui_text_create(obj, 0, 0, 300, SCREEN_WIDTH, 26);
    gui_text_set(t_act, active_timer_str[index], GUI_FONT_SRC_TTF, gui_rgb(0xF1, 0x9A, 0x38),
                 strlen(active_timer_str[index]), 26);
    gui_text_type_set(t_act, SF_COMPACT_REGULAR_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t_act, MID_CENTER);
    gui_text_rendermode_set(t_act, 2);
    update_timer_str(active_timer_rec_str[index], (uint32_t)active_timer_rec_val[index],
                     active_timer_val[index]);
    gui_text_t *t_rec = gui_text_create(obj, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    gui_text_set(t_rec, active_timer_rec_str[index], GUI_FONT_SRC_TTF, gui_rgb(0xFF, 0xFF, 0xFF),
                 strlen(active_timer_rec_str[index]), 72);
    gui_text_type_set(t_rec, SF_COMPACT_REGULAR_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t_rec, MID_CENTER);
    gui_text_rendermode_set(t_rec, 2);
}

static void page_active_list_timer(void *obj)
{
    indicator_update_cb(obj);
    gui_list_t *list = obj;
    static int16_t index_rec = 0;
    if (index_rec != list->last_created_note_index)
    {
        index_rec = list->last_created_note_index;
        GUI_WIDGET_POINTER_BY_NAME_ROOT(icon, "stop", list->base.parent);
        if (!active_timer_status[index_rec])
        {
            gui_img_set_image_data((void *)icon, STOPWATCH_BUTTON_START_BIN);
            gui_img_refresh_size((void *)icon);
            gui_img_a8_recolor((void *)icon, 0xFFFFFFFF);
        }
        else
        {
            gui_img_set_image_data((void *)icon, TIMER_BUTTON_STOP_BIN);
            gui_img_refresh_size((void *)icon);
            gui_img_a8_recolor((void *)icon, 0xFF000000);
        }
    }
}

static void page_active_single_design(gui_obj_t *parent)
{
    int16_t bg_cancel_y = 402;
    if (active_timer_num)
    {
        gui_list_t *list = gui_list_create(parent, 0, 0, 0, 0, 0, SCREEN_HEIGHT, 0, VERTICAL,
                                           note_active_single_design, NULL, 0);
        gui_list_set_style(list, LIST_FADE);
        gui_list_set_note_num(list, active_timer_num);
        gui_list_set_auto_align(list, true);
        gui_list_set_inertia(list, false);
        gui_list_set_offset(list, page_active_single_list_offset_his);
        if (active_timer_num > 1)
        {
            gui_obj_create_timer((void *)list, 10, -1, page_active_list_timer);
            gui_win_t *win = gui_win_create(parent, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
            int16_t y = 90;
            for (uint8_t i = 0; i < active_timer_num; i++)
            {
                gui_img_t *page_indicator = gui_img_create_from_mem(win, 0, PAGE_INDEX_ICON_BIN, 394, y, 0, 0);
                gui_img_a8_recolor((void *)page_indicator, 0xFF808080);
                y += 16;
            }
            gui_img_t *bg_back = gui_img_create_from_mem(parent, 0, BG_CIRCLE_72_BIN, 28, 25, 0, 0);
            gui_img_t *icon_back = gui_img_create_from_mem(bg_back, 0, TIMER_BUTTON_BACK_BIN, 21, 18, 0, 0);
            gui_img_a8_recolor(bg_back, COLOR_DEFAULT);
            gui_img_a8_recolor(icon_back, 0xFFFFFFFF);
            gui_obj_add_event_cb((void *)bg_back, click_button_back, GUI_EVENT_TOUCH_CLICKED, NULL);
        }
        gui_img_t *bg_add = gui_img_create_from_mem(parent, 0, BG_CIRCLE_72_BIN, 310, 25, 0, 0);
        gui_img_t *bg_stop = gui_img_create_from_mem(parent, 0, BG_CIRCLE_72_BIN, 310, 402, 0, 0);
        gui_img_t *icon_add = gui_img_create_from_mem(bg_add, 0, TIMER_BUTTON_ADD_BIN, 18, 18, 0, 0);
        gui_img_t *icon_stop = gui_img_create_from_mem(bg_stop, "stop", TIMER_BUTTON_STOP_BIN, 23, 20, 0,
                                                       0);
        gui_img_a8_recolor(bg_add, COLOR_DEFAULT);
        gui_img_a8_recolor(bg_stop, COLOR_THEME);
        gui_img_a8_recolor(icon_add, 0xFFFFFFFF);
        gui_img_a8_recolor(icon_stop, 0xFF000000);
        gui_obj_add_event_cb((void *)bg_add, click_button_add, GUI_EVENT_TOUCH_CLICKED, NULL);
        gui_obj_add_event_cb((void *)bg_stop, click_button_stop, GUI_EVENT_TOUCH_CLICKED, NULL);
    }
    else
    {
        gui_text_t *t_tip = gui_text_create(parent, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        gui_text_set(t_tip, "No active timer", GUI_FONT_SRC_TTF, gui_rgb(0xFF, 0xFF, 0xFF),
                     strlen("No active timer"), 50);
        gui_text_type_set(t_tip, SF_COMPACT_TEXT_MEDIUM_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(t_tip, MID_CENTER);
        gui_text_rendermode_set(t_tip, 2);

        bg_cancel_y = 25;
    }

    gui_text_t *t_time = gui_text_create(parent, 0, 0, 30, SCREEN_WIDTH, 34);
    gui_text_set(t_time, time_str, GUI_FONT_SRC_TTF, gui_rgb(0xFF, 0xFF, 0xFF),
                 strlen(time_str), 34);
    gui_text_type_set(t_time, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t_time, MID_CENTER);
    gui_text_rendermode_set(t_time, 2);
    gui_obj_create_timer((void *)t_time, 30000, -1, time_update_cb);

    gui_img_t *bg_cancel = gui_img_create_from_mem(parent, 0, BG_CIRCLE_72_BIN, 28, bg_cancel_y, 0, 0);
    gui_img_t *icon_cancel = gui_img_create_from_mem(bg_cancel, 0, TIMER_BUTTON_CANCEL_BIN, 21, 21, 0,
                                                     0);
    gui_img_a8_recolor(bg_cancel, COLOR_DEFAULT);
    gui_img_a8_recolor(icon_cancel, 0xFFFFFFFF);
    gui_obj_add_event_cb((void *)bg_cancel, click_button_cancel, GUI_EVENT_TOUCH_CLICKED, NULL);
}

/* Page active multi */
static void draw_arc_small_cb(NVGcontext *vg)
{
    const float line_width = 6;
    const float radius = (float)RADIUS_SMALL;

    nvgBeginPath(vg);
    nvgArc(vg, radius, radius, radius - line_width / 2.f, -M_PI / 2,
           M_PI * (2.0f * arc_end - 0.5f), NVG_CW);
    nvgStrokeWidth(vg, line_width);
    nvgStrokeColor(vg, nvgRGB(0xE0, 0x87, 0x31));
    nvgStroke(vg);
}

static void page_active_multi_scrollbar_update_cb(void *obj)
{
    gui_list_t *list = obj;
    gui_obj_t *mask = gui_list_entry(GUI_BASE(obj)->brother_list.next, gui_obj_t,
                                     brother_list);
    gui_obj_t *scrollbar_bg = gui_list_entry(mask->brother_list.next, gui_obj_t,
                                             brother_list);
    gui_obj_t *scrollbar = gui_list_entry(scrollbar_bg->child_list.next, gui_obj_t,
                                          brother_list);
    static uint8_t cnt = 0;

    if (page_active_multi_list_offset_his != list->offset)
    {
        page_active_multi_list_offset_his = list->offset;
        int range = list->base.h - list->total_length;
        int t_y = page_active_multi_list_offset_his * (SCROLLBAR_BG_H - SCROLLBAR_M_H) / range;
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

// Update rec_text and arc_small
static void active_multi_timer_cb(void *obj)
{
    gui_text_t *t = obj;
    gui_list_note_t *note = (void *)t->base.parent->parent;
    int16_t index = note->index;
    if (!active_timer_status[index]) { return; }

    update_timer_str(active_timer_rec_str[index], (uint32_t)(active_timer_rec_val[index] + 1),
                     active_timer_val[index]);
    gui_text_content_set((void *)t, active_timer_rec_str[index], strlen(active_timer_rec_str[index]));
    int image_h = RADIUS_SMALL * 2;
    int image_w = RADIUS_SMALL * 2;
    int pixel_bytes = 2;
    size_t buffer_size = image_h * image_w * pixel_bytes + sizeof(gui_rgb_data_head_t);
    memset(img_data_small[index], 0, buffer_size);
    arc_end = active_timer_rec_val[index] / active_timer_val[index];
    gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGB565, 0, image_w, image_h, draw_arc_small_cb,
                                      img_data_small[index]);
}

static void move_bg(void *p)
{
    const int16_t x_min = -153;
    const int16_t x_max = 0;

    touch_info_t *tp = tp_get_info();
    gui_obj_t *obj = p;
    gui_img_t *img = (gui_img_t *)obj;
    static int16_t x_rec = 0;
    if (tp->pressed &&
        (gui_obj_point_in_obj_rect(obj, tp->x + tp->deltaX, tp->y + tp->deltaY) == true))
    {
        x_rec = img->t_x;
    }
    else if (tp->pressing && tp->type == TOUCH_HOLD_X &&
             (gui_obj_point_in_obj_rect(obj, tp->x + tp->deltaX, tp->y + tp->deltaY) == true))
    {
        int16_t x = x_rec + tp->deltaX;
        if (x < x_min)
        {
            x = x_min;
        }
        else if (x > x_max)
        {
            x = x_max;
        }
        gui_img_translate(img, (float)x, 0);
    }
    else if (img->t_x != 0)
    {
        int16_t x = (int16_t)img->t_x;
        const int16_t step = 20;
        if (x >= (x_min - x_max) / 2)
        {
            x += step;
        }
        else
        {
            x -= step;
        }
        if (x <= x_min)
        {
            x = x_min;
        }
        else if (x >= x_max)
        {
            x = x_max;
        }
        gui_img_translate(img, (float)x, 0);
    }
}

static void click_bg_page_active_multi(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    gui_list_note_t *note = (void *)GUI_BASE(obj)->parent;
    page_active_single_list_offset_his = -note->index * SCREEN_HEIGHT;
    timer_page = ACTIVE_SINGLE;
    show_page((void *)gui_view_get_current());
}

static void note_active_multi_design(gui_obj_t *obj, void *p)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(p);

    gui_list_note_t *note = (void *)obj;
    int16_t index = note->index;

    gui_img_t *bg_del = gui_img_create_from_mem(obj, 0, BG_CIRCLE_100_BIN, 272, 25, 0, 0);
    gui_img_t *icon_del = gui_img_create_from_mem(bg_del, 0, TIMER_BUTTON_DELETE_BIN, 29, 24, 0, 0);
    gui_obj_add_event_cb((void *)bg_del, click_button_delete, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_img_a8_recolor(bg_del, 0xFFF4593F);
    gui_img_a8_recolor(icon_del, 0xFFFFFFFF);

    gui_img_t *bg = gui_img_create_from_mem(obj, 0, BG_ROUND_370_150_BIN, 20, 0, 0, 0);
    gui_obj_add_event_cb((void *)bg, click_bg_page_active_multi, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_img_a8_recolor(bg, COLOR_DEFAULT);
    gui_obj_create_timer((void *)bg, 10, -1, move_bg);
    gui_obj_start_timer((void *)bg);

    if (img_data_small == NULL)
    {
        img_data_small = gui_malloc(ACTIVE_TIMER_NUM_MAX * sizeof(void *));
        memset(img_data_small, 0, ACTIVE_TIMER_NUM_MAX * sizeof(void *));
    }
    int image_h = RADIUS_SMALL * 2;
    int image_w = RADIUS_SMALL * 2;
    int pixel_bytes = 2;
    size_t buffer_size = image_h * image_w * pixel_bytes + sizeof(gui_rgb_data_head_t);
    if (img_data_small[index] == NULL)
    {
        img_data_small[index] = gui_malloc(buffer_size);
    }
    memset(img_data_small[index], 0, buffer_size);
    arc_end = active_timer_rec_val[index] / active_timer_val[index];
    gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGB565, 0, image_w, image_h, draw_arc_small_cb,
                                      img_data_small[index]);

    update_timer_str(active_timer_rec_str[index], (uint32_t)active_timer_rec_val[index],
                     active_timer_val[index]);
    update_timer_str(active_timer_str[index], active_timer_val[index], active_timer_val[index]);
    gui_text_t *t_act = gui_text_create(bg, 0, 20, 26, 200, 45);
    gui_text_set(t_act, active_timer_str[index], GUI_FONT_SRC_TTF, gui_rgb(0xF1, 0x9A, 0x38),
                 strlen(active_timer_str[index]), 45);
    gui_text_type_set(t_act, SF_COMPACT_REGULAR_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t_act, MID_LEFT);
    gui_text_rendermode_set(t_act, 2);
    gui_text_t *t_rec = gui_text_create(bg, 0, 20, 87, 300, 60);
    gui_text_set(t_rec, active_timer_rec_str[index], GUI_FONT_SRC_TTF, gui_rgb(0xFF, 0xFF, 0xFF),
                 strlen(active_timer_rec_str[index]), 60);
    gui_text_type_set(t_rec, SF_COMPACT_REGULAR_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t_rec, MID_LEFT);
    gui_text_rendermode_set(t_rec, 2);
    gui_obj_create_timer((void *)t_rec, 200, -1, active_multi_timer_cb);
    gui_obj_start_timer((void *)t_rec);

    gui_img_t *bg_stop = gui_img_create_from_mem(bg, 0, BG_CIRCLE_72_BIN, 291, 39, 0, 0);
    gui_obj_add_event_cb((void *)bg_stop, click_button_stop, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_img_create_from_mem(bg_stop, 0, img_data_small[index], 0, 0, 0, 0);
    gui_img_t *icon_stop = gui_img_create_from_mem(bg_stop, 0, TIMER_BUTTON_STOP_BIN, 23, 20, 0, 0);
    if (!active_timer_status[index])
    {
        gui_img_set_image_data(icon_stop, STOPWATCH_BUTTON_START_BIN);
        gui_img_refresh_size(icon_stop);
        gui_img_a8_recolor(icon_stop, 0xFFFFFFFF);
    }
    else
    {
        gui_img_a8_recolor(icon_stop, COLOR_THEME);
    }
}

static void page_active_multi_design(gui_obj_t *parent)
{
    gui_list_t *list = gui_list_create(parent, 0, 0, 140, 0, 0, 150, 10, VERTICAL,
                                       note_active_multi_design, NULL, 0);
    gui_list_set_style(list, LIST_CLASSIC);
    gui_list_set_note_num(list, active_timer_num);
    gui_list_set_factor(list, 0.15f);
    gui_list_set_offset(list, page_active_multi_list_offset_his);

    if (active_timer_num > 2)
    {
        gui_obj_create_timer((void *)list, 10, -1, page_active_multi_scrollbar_update_cb);

        gui_img_t *mask = gui_img_create_from_mem(parent, 0, BG_RECT_410_140_BIN, 0, 0, 0, 0);
        gui_img_a8_recolor(mask, 0xFF000000);
        gui_img_t *scrollbar_bg = gui_img_create_from_mem(parent, 0, SCROLLBAR_BG_BIN, 395, 80, 0, 0);
        gui_img_set_mode(scrollbar_bg, IMG_SRC_OVER_MODE);
        gui_img_t *scrollbar = gui_img_create_from_mem(scrollbar_bg, 0, SCROLLBAR_M_BIN, -1, 0, 0, 0);
        gui_img_set_mode(scrollbar, IMG_SRC_OVER_MODE);
        gui_obj_hidden((void *)scrollbar_bg, true);
        gui_obj_hidden((void *)scrollbar, true);
    }

    gui_img_t *bg_add = gui_img_create_from_mem(parent, 0, BG_CIRCLE_72_BIN, 310, 25, 0, 0);
    gui_img_t *icon_add = gui_img_create_from_mem(bg_add, 0, TIMER_BUTTON_ADD_BIN, 18, 18, 0, 0);
    gui_img_a8_recolor(bg_add, COLOR_DEFAULT);
    gui_img_a8_recolor(icon_add, 0xFFFFFFFF);
    gui_obj_add_event_cb((void *)bg_add, click_button_add, GUI_EVENT_TOUCH_CLICKED, NULL);

    gui_text_t *t_time = gui_text_create(parent, 0, 0, 30, SCREEN_WIDTH, 34);
    gui_text_set(t_time, time_str, GUI_FONT_SRC_TTF, gui_rgb(0xFF, 0xFF, 0xFF),
                 strlen(time_str), 34);
    gui_text_type_set(t_time, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t_time, MID_CENTER);
    gui_text_rendermode_set(t_time, 2);
    gui_obj_create_timer((void *)t_time, 30000, -1, time_update_cb);
}


/* Page timer end */
static void page_timer_end_design(gui_obj_t *parent)
{
    gui_rect_create(parent, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, gui_rgb(0xC3, 0x50, 0x21));
    int16_t bg_cancel_y = 402;

    arc_end = 1.f;
    size_t buffer_size = RADIUS_BIG * 2 * RADIUS_BIG * 2 * 2 + sizeof(gui_rgb_data_head_t);
    memset(img_data_big[0], 0, buffer_size);
    gui_canvas_render_to_image_buffer(GUI_CANVAS_OUTPUT_RGB565, 0, RADIUS_BIG * 2, RADIUS_BIG * 2,
                                      draw_arc_big_cb, img_data_big[0]);
    gui_img_create_from_mem(parent, 0, img_data_big[0], 33, 79, 0, 0);

    update_timer_str(timer_end_str, timer_end_val, timer_end_val);
    gui_text_t *t_act = gui_text_create(parent, 0, 0, 300, SCREEN_WIDTH, 26);
    gui_text_set(t_act, timer_end_str, GUI_FONT_SRC_TTF, gui_rgb(0xFF, 0xFF, 0xFF),
                 strlen(timer_end_str), 26);
    gui_text_type_set(t_act, SF_COMPACT_REGULAR_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t_act, MID_CENTER);
    gui_text_rendermode_set(t_act, 2);
    gui_text_t *t_done = gui_text_create(parent, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    gui_text_set(t_done, "DONE", GUI_FONT_SRC_TTF, gui_rgb(0xFF, 0xFF, 0xFF), 4, 72);
    gui_text_type_set(t_done, SF_COMPACT_REGULAR_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t_done, MID_CENTER);
    gui_text_rendermode_set(t_done, 2);

    gui_text_t *t_time = gui_text_create(parent, 0, 0, 30, SCREEN_WIDTH, 34);
    gui_text_set(t_time, time_str, GUI_FONT_SRC_TTF, gui_rgb(0xFF, 0xFF, 0xFF),
                 strlen(time_str), 34);
    gui_text_type_set(t_time, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t_time, MID_CENTER);
    gui_text_rendermode_set(t_time, 2);
    gui_obj_create_timer((void *)t_time, 30000, -1, time_update_cb);

    gui_img_t *bg_cancel = gui_img_create_from_mem(parent, 0, BG_CIRCLE_72_BIN, 28, bg_cancel_y, 0, 0);
    gui_img_t *icon_cancel = gui_img_create_from_mem(bg_cancel, 0, TIMER_BUTTON_CANCEL_BIN, 21, 21, 0,
                                                     0);
    gui_img_a8_recolor(bg_cancel, COLOR_THEME);
    gui_img_a8_recolor(icon_cancel, 0xFFFFFFFF);
    gui_obj_add_event_cb((void *)bg_cancel, click_button_cancel, GUI_EVENT_TOUCH_CLICKED, NULL);

    gui_img_t *bg_reset = gui_img_create_from_mem(parent, 0, BG_CIRCLE_72_BIN, 310, 402, 0, 0);
    gui_img_t *icon_reset = gui_img_create_from_mem(bg_reset, 0, STOPWATCH_BUTTON_RESET_BIN, 15, 15, 0,
                                                    0);
    gui_img_a8_recolor(bg_reset, COLOR_THEME);
    gui_img_a8_recolor(icon_reset, 0xFFFFFFFF);
    gui_obj_add_event_cb((void *)icon_reset, click_button_reset, GUI_EVENT_TOUCH_CLICKED, NULL);
}

/* Page custom add timer */
static void click_button_start(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    timer_page = active_timer_num ? ACTIVE_MULTI : ACTIVE_SINGLE;
    uint32_t temp = hour_index * 3600 + min_index * 60 + sec_index;

    if (preset_timer_num == PRESET_TIMER_NUM_MAX)
    {
        for (uint8_t i = PRESET_TIMER_NUM_MAX - 1; i > 0; i--)
        {
            preset_timer_val[i - 1] = preset_timer_val[i];
            memccpy(preset_timer_str[i - 1], preset_timer_str[i], 9, sizeof(char));
        }
        preset_timer_val[preset_timer_num] = temp;
        update_timer_str(preset_timer_str[preset_timer_num], temp, temp);
    }
    else
    {
        if (preset_timer_str[preset_timer_num] == NULL)
        {
            preset_timer_str[preset_timer_num] = gui_malloc(9 * sizeof(char));
        }
        preset_timer_val[preset_timer_num] = temp;
        update_timer_str(preset_timer_str[preset_timer_num], temp, temp);
        preset_timer_num++;
    }

    if (active_timer_num < ACTIVE_TIMER_NUM_MAX)
    {
        active_timer_val[active_timer_num] = temp;
        active_timer_rec_val[active_timer_num] = (float)active_timer_val[active_timer_num];
        if (!active_timer_str[active_timer_num])
        {
            active_timer_str[active_timer_num] = gui_malloc(9 * sizeof(char));
            active_timer_rec_str[active_timer_num] = gui_malloc(9 * sizeof(char));
        }
        update_timer_str(active_timer_str[active_timer_num], active_timer_val[active_timer_num],
                         active_timer_val[active_timer_num]);
        active_timer_status[active_timer_num] = true;
        active_timer_num++;
    }
    else
    {
        timer_page = ALL_TIMERS;
    }
    show_page((void *)gui_view_get_current());
}

static void note_timer_page_custom_add_timer(void *p)
{
    gui_list_note_t *note = (void *)p;
    gui_list_t *list = (void *)note->base.parent;
    if (note->base.y > 50 && note->base.y < 90)
    {
        int16_t temp = note->index;
        uint8_t note_num = 60;
        uint8_t *index = NULL;
        if (strcmp(list->base.name, "h") == 0)
        {
            index = &hour_index;
            note_num = 13;
        }
        else if (strcmp(list->base.name, "m") == 0)
        {
            index = &min_index;
        }
        else
        {
            index = &sec_index;
        }
        temp %= note_num;
        temp += note_num;
        temp %= note_num;
        *index = temp;
    }
}

static void note_custom_add_timer_design(gui_obj_t *obj, void *p)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(p);
    gui_list_note_t *note = (void *)obj;
    gui_list_t *list = (void *)obj->parent;
    note->base.name = list->base.name;
    int16_t index = note->index;
    uint8_t note_num = 60;
    if (&list->base.brother_list == list->base.parent->child_list.next)
    {
        note_num = 13;
    }
    index %= note_num;
    index += note_num;
    index %= note_num;
    gui_text_t *t = gui_text_create(obj, 0, 0, 0, 110, 60);
    gui_text_set(t, (void *)(num_str + index * 2), GUI_FONT_SRC_TTF, gui_rgb(0xFF, 0xFF, 0xFF), 2, 60);
    gui_text_type_set(t, SF_COMPACT_REGULAR_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t, MID_CENTER);
    gui_text_rendermode_set(t, 2);

    gui_obj_create_timer(obj, 10, -1, note_timer_page_custom_add_timer);
}

static void page_custom_add_timer_design(gui_obj_t *parent)
{
    // Hour
    gui_list_t *list = gui_list_create(parent, "h", 16, 190, 110, 174, 60, 10, VERTICAL,
                                       note_custom_add_timer_design, NULL, 0);
    gui_list_set_style(list, LIST_CLASSIC);
    gui_list_set_note_num(list, 13);
    gui_list_enable_loop(list, true);
    gui_list_set_auto_align(list, true);
    list = gui_list_create(parent, "m", 151, 190, 110, 174, 60, 10, VERTICAL,
                           note_custom_add_timer_design, NULL, 0);
    gui_list_set_style(list, LIST_CLASSIC);
    gui_list_set_note_num(list, 60);
    gui_list_enable_loop(list, true);
    gui_list_set_auto_align(list, true);
    list = gui_list_create(parent, "s", 286, 190, 110, 174, 60, 10, VERTICAL,
                           note_custom_add_timer_design, NULL, 0);
    gui_list_set_style(list, LIST_CLASSIC);
    gui_list_set_note_num(list, 60);
    gui_list_enable_loop(list, true);
    gui_list_set_auto_align(list, true);

    gui_img_t *mask = gui_img_create_from_mem(parent, 0, BG_RECT_410_140_BIN, 0, 50, 0, 0);
    gui_img_a8_recolor(mask, 0xFF000000);
    mask = gui_img_create_from_mem(parent, 0, BG_RECT_410_140_BIN, 0, 364, 0, 0);
    gui_img_a8_recolor(mask, 0xFF000000);
    gui_img_t *list_cover = gui_img_create_from_mem(parent, 0, TIMER_FRAME_BIN, 16, 190, 0, 0);
    gui_img_a8_recolor(list_cover, 0xFFFFFFFF);
    list_cover = gui_img_create_from_mem(parent, 0, TIMER_FRAME_BIN, 151, 190, 0, 0);
    gui_img_a8_recolor(list_cover, 0xFFFFFFFF);
    list_cover = gui_img_create_from_mem(parent, 0, TIMER_FRAME_BIN, 286, 190, 0, 0);
    gui_img_a8_recolor(list_cover, 0xFFFFFFFF);
    gui_text_t *t_common = gui_text_create(parent, 0, 130, 272, 16, 40);
    gui_text_set(t_common, ":", GUI_FONT_SRC_TTF, gui_rgb(0xFF, 0xFF, 0xFF), 1, 40);
    gui_text_type_set(t_common, SF_COMPACT_REGULAR_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t_common, MID_CENTER);
    gui_text_rendermode_set(t_common, 2);
    t_common = gui_text_create(parent, 0, 265, 272, 16, 40);
    gui_text_set(t_common, ":", GUI_FONT_SRC_TTF, gui_rgb(0xFF, 0xFF, 0xFF), 1, 40);
    gui_text_type_set(t_common, SF_COMPACT_REGULAR_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t_common, MID_CENTER);
    gui_text_rendermode_set(t_common, 2);
    gui_img_t *bg_unit_h = gui_img_create_from_mem(parent, 0, BG_ROUND_72_50_BIN, 35, 133, 0, 0);
    gui_img_t *bg_unit_min = gui_img_create_from_mem(parent, 0, BG_ROUND_72_50_BIN, 170, 133, 0, 0);
    gui_img_t *bg_unit_s = gui_img_create_from_mem(parent, 0, BG_ROUND_72_50_BIN, 305, 133, 0, 0);
    gui_img_a8_recolor(bg_unit_h, 0xFF59DD8A);
    gui_img_a8_recolor(bg_unit_min, 0xFF59DD8A);
    gui_img_a8_recolor(bg_unit_s, 0xFF59DD8A);
    gui_text_t *t_unit = gui_text_create(bg_unit_h, 0, 0, 0, 72, 50);
    gui_text_set(t_unit, "Hour", GUI_FONT_SRC_TTF, gui_rgb(0xFF, 0xFF, 0xFF), 4, 25);
    gui_text_type_set(t_unit, SF_COMPACT_REGULAR_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t_unit, MID_CENTER);
    gui_text_rendermode_set(t_unit, 2);
    t_unit = gui_text_create(bg_unit_min, 0, 0, 0, 72, 50);
    gui_text_set(t_unit, "Min", GUI_FONT_SRC_TTF, gui_rgb(0xFF, 0xFF, 0xFF), 3, 25);
    gui_text_type_set(t_unit, SF_COMPACT_REGULAR_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t_unit, MID_CENTER);
    gui_text_rendermode_set(t_unit, 2);
    t_unit = gui_text_create(bg_unit_s, 0, 0, 0, 72, 50);
    gui_text_set(t_unit, "Sec", GUI_FONT_SRC_TTF, gui_rgb(0xFF, 0xFF, 0xFF), 3, 25);
    gui_text_type_set(t_unit, SF_COMPACT_REGULAR_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t_unit, MID_CENTER);
    gui_text_rendermode_set(t_unit, 2);


    gui_img_t *bg_start = gui_img_create_from_mem(parent, 0, BG_ROUND_325_90_BIN, 42, 380, 0, 0);
    gui_img_a8_recolor(bg_start, COLOR_THEME);
    gui_obj_add_event_cb((void *)bg_start, click_button_start, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_text_t *t_start = gui_text_create(bg_start, 0, 0, 0, 325, 90);
    gui_text_set(t_start, "Start", GUI_FONT_SRC_TTF, gui_rgb(0xFF, 0xFF, 0xFF), 5, 50);
    gui_text_type_set(t_start, SF_COMPACT_REGULAR_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t_start, MID_CENTER);
    gui_text_rendermode_set(t_start, 2);

    gui_text_t *t_time = gui_text_create(parent, 0, 0, 30, 382, 34);
    gui_text_set(t_time, time_str, GUI_FONT_SRC_TTF, gui_rgb(0xFF, 0xFF, 0xFF),
                 strlen(time_str), 34);
    gui_text_type_set(t_time, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t_time, MID_RIGHT);
    gui_text_rendermode_set(t_time, 2);
    gui_obj_create_timer((void *)t_time, 30000, -1, time_update_cb);

    gui_img_t *bg_cancel = gui_img_create_from_mem(parent, 0, BG_CIRCLE_72_BIN, 28, 25, 0, 0);
    gui_img_t *icon_cancel = gui_img_create_from_mem(bg_cancel, 0, TIMER_BUTTON_CANCEL_BIN, 21, 21, 0,
                                                     0);
    gui_img_a8_recolor(bg_cancel, COLOR_DEFAULT);
    gui_img_a8_recolor(icon_cancel, 0xFFFFFFFF);
    gui_obj_add_event_cb((void *)bg_cancel, click_button_cancel, GUI_EVENT_TOUCH_CLICKED, NULL);
}

/* Page all timers */
static void click_button_edit(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    edit_mode = !edit_mode;
    show_page((void *)gui_view_get_current());
}

static void click_button_page_all_timers(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(e);
    gui_obj_t *o = obj;
    int16_t index = (*((int16_t *)param) - 1) * 2;
    if (o->x == 210 || o->parent->x == 210) { index += 1; }

    if (edit_mode)
    {
        preset_timer_num--;
        for (uint8_t i = index; i < preset_timer_num; i++)
        {
            preset_timer_val[i] = preset_timer_val[i + 1];
            memccpy(preset_timer_str[i], preset_timer_str[i + 1], 9, sizeof(char));
        }
        show_page((void *)gui_view_get_current());
        return;
    }

    if (index >= preset_timer_num && preset_timer_num != PRESET_TIMER_NUM_MAX)
    {
        timer_page = CUSTOM_ADD;
        show_page((void *)gui_view_get_current());
    }
    else
    {
        if (active_timer_num == 0)
        {
            timer_page = ACTIVE_SINGLE;
            active_timer_val[0] = preset_timer_val[index];
            active_timer_rec_val[0] = (float)active_timer_val[0];
            if (!active_timer_str[0])
            {
                active_timer_str[0] = gui_malloc(9 * sizeof(char));
                active_timer_rec_str[0] = gui_malloc(9 * sizeof(char));
            }
            update_timer_str(active_timer_str[0], active_timer_val[0], active_timer_val[0]);
            active_timer_num = 1;
            active_timer_status[0] = true;
            page_active_single_list_offset_his = -active_timer_num * SCREEN_HEIGHT;
            show_page((void *)gui_view_get_current());
        }
        else if (active_timer_num < ACTIVE_TIMER_NUM_MAX)
        {
            timer_page = ACTIVE_MULTI;
            active_timer_val[active_timer_num] = preset_timer_val[index];
            active_timer_rec_val[active_timer_num] = (float)active_timer_val[active_timer_num];
            if (!active_timer_str[active_timer_num])
            {
                active_timer_str[active_timer_num] = gui_malloc(9 * sizeof(char));
                active_timer_rec_str[active_timer_num] = gui_malloc(9 * sizeof(char));
            }
            update_timer_str(active_timer_str[active_timer_num], active_timer_val[active_timer_num],
                             active_timer_val[active_timer_num]);
            active_timer_num++;
            active_timer_status[active_timer_num - 1] = true;
            show_page((void *)gui_view_get_current());
        }
        gui_log("click button %s\n", active_timer_str[active_timer_num - 1]);
    }
}

static void page_all_timers_scrollbar_update_cb(void *obj)
{
    gui_list_t *list = obj;
    gui_obj_t *scrollbar_bg = gui_list_entry(GUI_BASE(obj)->parent->child_list.prev, gui_obj_t,
                                             brother_list);
    gui_obj_t *scrollbar = gui_list_entry(scrollbar_bg->child_list.next, gui_obj_t,
                                          brother_list);
    static uint8_t cnt = 0;

    if (page_all_timers_list_offset_his != list->offset)
    {
        page_all_timers_list_offset_his = list->offset;
        int range = list->base.h - list->total_length;
        int t_y = page_all_timers_list_offset_his * (SCROLLBAR_BG_H - SCROLLBAR_M_H) / range;
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

static void note_all_timers_design(gui_obj_t *obj, void *p)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(p);
    gui_list_note_t *note = (void *)obj;
    gui_list_t *list = (void *)obj->parent;
    int16_t index = note->index;
    if (index == 0)
    {
        int16_t font_size = 30;
        gui_text_t *t = gui_text_create(obj, 0, 28, 125, 300, font_size);
        gui_text_set(t, "All Timers", GUI_FONT_SRC_TTF, gui_rgb(0xFF, 0xFF, 0xFF),
                     strlen("All Timers"), font_size);
        gui_text_type_set(t, SF_COMPACT_TEXT_SEMIBOLD_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, MID_LEFT);
        gui_text_rendermode_set(t, 2);
        return;
    }
    else if (index == list->note_num - 1)
    {
        gui_img_t *bg = gui_img_create_from_mem(obj, 0, BG_ROUND_200_56_BIN, 105, 0, 0, 0);
        gui_img_a8_recolor(bg, 0xFF1B1B1B);
        gui_obj_add_event_cb((void *)bg, click_button_edit, GUI_EVENT_TOUCH_CLICKED, NULL);

        int16_t font_size = 40;
        char *content = edit_mode ? "Done" : "Edit";
        gui_text_t *t = gui_text_create(bg, 0, 0, 0, 200, 56);
        gui_text_set(t, content, GUI_FONT_SRC_TTF, gui_rgb(0xFF, 0xFF, 0xFF),
                     strlen(content), font_size);
        gui_text_type_set(t, SF_COMPACT_TEXT_SEMIBOLD_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, MID_CENTER);
        gui_text_rendermode_set(t, 2);
        return;
    }
    int16_t x = 28;
    for (uint8_t i = (index - 1) * 2; i < index * 2; i++)
    {
        gui_img_t *bg = NULL;
        if (i == preset_timer_num)
        {
            bg = gui_img_create_from_mem(obj, 0, BG_CIRCLE_172_BIN, x, 0, 0, 0);
            gui_img_a8_recolor(bg, COLOR_DEFAULT);
            gui_img_t *button = gui_img_create_from_mem(bg, 0, TIMER_BUTTON_ADD_BIN, 68, 68, 0, 0);
            gui_img_a8_recolor(button, 0xFFFFFFFF);
            if (!edit_mode)
            {
                gui_obj_add_event_cb((void *)bg, click_button_page_all_timers, GUI_EVENT_TOUCH_CLICKED,
                                     &note->index);
            }
            else
            {
                gui_img_set_opacity(bg, 150);
            }
            return;
        }
        else
        {
            bg = gui_img_create_from_mem(obj, 0, TIMER_STROKE_BG_BIN, x, 0, 0, 0);
            gui_img_set_mode(bg, IMG_SRC_OVER_MODE);

            int16_t font_size = 38;
            gui_text_t *t_timer = gui_text_create(bg, 0, 0, 0, 172, 172);
            gui_text_set(t_timer, preset_timer_str[i], GUI_FONT_SRC_TTF, gui_rgb(0xFF, 0xFF, 0xFF),
                         strlen(preset_timer_str[i]), font_size);
            gui_text_type_set(t_timer, SF_COMPACT_REGULAR_BIN, FONT_SRC_MEMADDR);
            gui_text_mode_set(t_timer, MID_CENTER);
            gui_text_rendermode_set(t_timer, 2);
            if (!edit_mode)
            {
                gui_obj_add_event_cb((void *)bg, click_button_page_all_timers, GUI_EVENT_TOUCH_CLICKED,
                                     &note->index);
            }
            else if (i >= 8)
            {
                gui_img_t *delete = gui_img_create_from_mem(bg, 0, TIMER_ICON_DELETE_BIN, 0, 0, 0, 0);
                gui_img_set_mode(delete, IMG_SRC_OVER_MODE);
                gui_obj_add_event_cb((void *)delete, click_button_page_all_timers, GUI_EVENT_TOUCH_CLICKED,
                                     &note->index);
            }
        }
        x = 210;
    }
}

static void page_all_timers_design(gui_obj_t *parent)
{
    timer_page = ALL_TIMERS;
    gui_list_t *list = gui_list_create(parent, 0, 0, 0, 0, 0, 172, 10, VERTICAL, note_all_timers_design,
                                       NULL, 0);
    gui_list_set_style(list, LIST_CLASSIC);
    uint8_t note_num = 3 + preset_timer_num / 2 - preset_timer_num / PRESET_TIMER_NUM_MAX;
    gui_list_set_note_num(list, note_num);
    gui_list_set_factor(list, 0.15f);
    list->total_length -= 50;
    if (!gui_view_get_next())
    {
        gui_list_set_offset(list, page_all_timers_list_offset_his);
    }
    else
    {
        edit_mode = false;
    }
    gui_obj_create_timer((void *)list, 10, -1, page_all_timers_scrollbar_update_cb);

    gui_win_t *win = gui_win_create(parent, 0, 0, 0, SCREEN_WIDTH, 100);
    gui_win_enable_blur(win, true);

    gui_img_t *scrollbar_bg = gui_img_create_from_mem(parent, 0, SCROLLBAR_BG_BIN, 395, 80, 0, 0);
    gui_img_set_mode(scrollbar_bg, IMG_SRC_OVER_MODE);
    gui_img_t *scrollbar = gui_img_create_from_mem(scrollbar_bg, 0, SCROLLBAR_M_BIN, -1, 0, 0, 0);
    gui_img_set_mode(scrollbar, IMG_SRC_OVER_MODE);
    gui_obj_hidden((void *)scrollbar_bg, true);
    gui_obj_hidden((void *)scrollbar, true);

    gui_text_t *t_time = gui_text_create(win, 0, 0, 30, 382, 34);
    gui_text_set(t_time, time_str, GUI_FONT_SRC_TTF, gui_rgb(0xFF, 0xFF, 0xFF),
                 strlen(time_str), 34);
    gui_text_type_set(t_time, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t_time, MID_RIGHT);
    gui_text_rendermode_set(t_time, 2);
    gui_obj_create_timer((void *)t_time, 30000, -1, time_update_cb);

    gui_text_t *t_app = gui_text_create(win, 0, 0, 70, 382, 34);
    gui_text_set(t_app, "Timers", GUI_FONT_SRC_TTF, gui_rgb(0xF1, 0x9A, 0x38),
                 strlen("Timers"), 34);
    gui_text_type_set(t_app, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t_app, MID_RIGHT);
    gui_text_rendermode_set(t_app, 2);

    gui_img_t *bg_cancel = gui_img_create_from_mem(win, 0, BG_CIRCLE_72_BIN, 28, 25, 0, 0);
    gui_img_t *icon_cancel = gui_img_create_from_mem(bg_cancel, 0, TIMER_BUTTON_CANCEL_BIN, 21, 21, 0,
                                                     0);
    gui_img_a8_recolor(bg_cancel, COLOR_DEFAULT);
    gui_img_a8_recolor(icon_cancel, 0xFFFFFFFF);
    gui_obj_add_event_cb((void *)bg_cancel, click_button_cancel, GUI_EVENT_TOUCH_CLICKED, NULL);
}

static void active_timer_update(void *p)
{
    GUI_UNUSED(p);
    bool regenerate = false;
    for (uint8_t j = 0; j < active_timer_num; j++)
    {
        if (active_timer_status[j])
        {
            active_timer_rec_val[j] -= 0.2f;
            if (active_timer_rec_val[j] <= 0.00001f)
            {
                active_timer_num--;
                timer_end_val = active_timer_val[active_timer_num];
                for (uint8_t i = j; i < active_timer_num; i++)
                {
                    active_timer_rec_val[i] = active_timer_rec_val[i + 1];
                    active_timer_val[i] = active_timer_val[i + 1];
                    active_timer_status[i] = active_timer_status[i + 1];
                }
                j -= 1;
                regenerate = true;
            }
        }
    }
    if (regenerate)
    {
        timer_page = TIMER_END;
        show_page(p);
    }
}

static void show_page(gui_obj_t *parent)
{
    if (strcmp(parent->name, CURRENT_VIEW_NAME) != 0) { return; }

    if (parent->child_list.next != &(parent->child_list))
    {
        gui_obj_child_free(parent);
    }
    switch (timer_page)
    {
    case ALL_TIMERS:
        page_all_timers_design(parent);
        break;
    case ACTIVE_SINGLE:
        page_active_single_design(parent);
        break;
    case ACTIVE_MULTI:
        page_active_multi_design(parent);
        break;
    case TIMER_END:
        page_timer_end_design(parent);
        break;
    case CUSTOM_ADD:
        page_custom_add_timer_design(parent);
        break;

    default:
        page_all_timers_design(parent);
        break;
    }
}

static void timers_design_init(gui_view_t *view)
{
    gui_view_switch_on_event(view, "menu_view",
                             SWITCH_OUT_ANIMATION_FADE,
                             SWITCH_IN_ANIMATION_FADE,
                             GUI_EVENT_KB_SHORT_CLICKED);
    if (gui_view_get_current()) { memset(active_timer_status, false, ACTIVE_TIMER_NUM_MAX); }

    if (!active_timer_str)
    {
        active_timer_str = gui_malloc(ACTIVE_TIMER_NUM_MAX * sizeof(char *));
        memset(active_timer_str, 0, ACTIVE_TIMER_NUM_MAX * sizeof(char *));
        active_timer_rec_str = gui_malloc(ACTIVE_TIMER_NUM_MAX * sizeof(char *));
        memset(active_timer_rec_str, 0, ACTIVE_TIMER_NUM_MAX * sizeof(char *));
        active_timer_val = gui_malloc(ACTIVE_TIMER_NUM_MAX * sizeof(uint32_t));
        memset(active_timer_val, 0, ACTIVE_TIMER_NUM_MAX * sizeof(uint32_t));
        active_timer_rec_val = gui_malloc(ACTIVE_TIMER_NUM_MAX * sizeof(float));
        memset(active_timer_rec_val, 0, ACTIVE_TIMER_NUM_MAX * sizeof(uint32_t));
        preset_timer_str = gui_malloc(PRESET_TIMER_NUM_MAX * sizeof(char *));
        memset(preset_timer_str, 0, PRESET_TIMER_NUM_MAX * sizeof(char *));
        for (uint8_t i = 0; i < preset_timer_num; i++)
        {
            preset_timer_str[i] = gui_malloc(9 * sizeof(char));
            update_timer_str(preset_timer_str[i], preset_timer_val[i], preset_timer_val[i]);
        }
    }
    gui_obj_create_timer((void *)view, 200, -1, active_timer_update);

    show_page((void *)view);
}
