/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <stdio.h>
#include <time.h>
#include "cJSON.h"
#include "root_image/ui_resource.h"
#include "gui_obj.h"
#include "gui_text.h"
#include "gui_win.h"
#include "guidef.h"
#include "kb_algo.h"
#include "gui_rect.h"
#include "gui_view.h"
#include "app_main_watch.h"
#include "gui_message.h"

/*============================================================================*
 *                           Types
 *============================================================================*/


/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "watchface_view"

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void app_main_watch_ui_design(gui_view_t *view);
static void clear_mem(gui_view_t *view);
static void inform_generate_cb(void);


/*============================================================================*
 *                            Variables
 *============================================================================*/
/* VIEW */
static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *menu_view = NULL;
const static gui_view_descriptor_t *watchface_select_view = NULL;
const static gui_view_descriptor_t *bottom_view = NULL;
const static gui_view_descriptor_t *top_view = NULL;
const static gui_view_descriptor_t *test_view = NULL;
static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = app_main_watch_ui_design,
    .on_switch_out = clear_mem,
    .keep = false,
};

bool menu_style = 0;
char *cjson_content = NULL;
uint8_t json_refresh_flag = 0;
struct tm *timeinfo;
#ifndef _HONEYGUI_SIMULATOR_
static struct tm watch_time;
#endif
char time_str[] = "00:00";

watchface_type_t current_watchface_type = WATCHFACE_BIG_NUM;
void (*watchface_clear_mem)(void) = NULL;

/* FPS */
static char fps[10];
static char widget_count_string[20];
static char mem_string[20];
static char low_mem_string[20];

#ifdef _HONEYGUI_SIMULATOR_
unsigned char *resource_root = NULL;
#endif

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
    test_view = gui_view_descriptor_get("timers_view");
    menu_view = gui_view_descriptor_get("menu_view");
    watchface_select_view = gui_view_descriptor_get("watchface_select_view");
    bottom_view = gui_view_descriptor_get("bottom_view");
    top_view = gui_view_descriptor_get("app_top_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

static void gui_fps_cb(void *p)
{
    int fps_num = gui_fps();
    gui_obj_t *fps_rect = GUI_BASE(p);
    sprintf(fps, "FPS:%d", fps_num);
    GUI_WIDGET_POINTER_BY_NAME_ROOT(t_fps, "t_fps", fps_rect);
    gui_text_content_set((gui_text_t *)t_fps, fps, strlen(fps));
    int widget_count_number = gui_get_obj_count();
    sprintf(widget_count_string, "WIDGETS:%d", widget_count_number);
    GUI_WIDGET_POINTER_BY_NAME_ROOT(widget_count, "widget_count", fps_rect);
    gui_text_content_set((gui_text_t *)widget_count, widget_count_string, strlen(widget_count_string));
    uint32_t mem_number =  gui_mem_used();
    uint32_t low_mem_number =  gui_low_mem_used();
    sprintf(mem_string, "RAM:%dKB", (int)mem_number / 0x400);
    GUI_WIDGET_POINTER_BY_NAME_ROOT(mem, "mem", fps_rect);
    gui_text_content_set((gui_text_t *)mem, mem_string, strlen(mem_string));
    sprintf(low_mem_string, "lowRAM:%dKB", (int)low_mem_number / 0x400);
    GUI_WIDGET_POINTER_BY_NAME_ROOT(low_mem, "low_mem", fps_rect);
    gui_text_content_set((gui_text_t *)low_mem, low_mem_string, strlen(low_mem_string));
}

// Show the FPS, widget count, memory usage, and low memory usage
static void fps_create(void *parent)
{
    char *text;
    int font_size = 20;
    gui_rounded_rect_t *fps_rect = gui_rect_create(parent, "rect_fps",
                                                   gui_get_screen_width() / 2 - 140 / 2, 0, 140,
                                                   70, 0,
                                                   APP_COLOR_GRAY_OPACITY(150));
    gui_obj_create_timer(GUI_BASE(fps_rect), 10, true, gui_fps_cb);
    sprintf(fps, "FPS:%d", gui_fps());
    text = fps;
    gui_text_t *t_fps = gui_text_create(fps_rect, "t_fps", 10, 0, gui_get_screen_width(), font_size);
    gui_text_set(t_fps, text, GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255), strlen(text), font_size);
    gui_text_type_set(t_fps, SF_COMPACT_REGULAR_BIN, FONT_SRC_MEMADDR);
    gui_text_rendermode_set(t_fps, 2);
    gui_text_t *widget_count = gui_text_create(fps_rect, "widget_count", 10, 16, gui_get_screen_width(),
                                               font_size);
    gui_text_set(widget_count, text, GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255), strlen(text), font_size);
    gui_text_type_set(widget_count, SF_COMPACT_REGULAR_BIN, FONT_SRC_MEMADDR);
    gui_text_t *mem = gui_text_create(fps_rect, "mem", 10, 16 * 2, gui_get_screen_width(), font_size);
    gui_text_set(mem, text, GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255), strlen(text), font_size);
    gui_text_type_set(mem, SF_COMPACT_REGULAR_BIN, FONT_SRC_MEMADDR);
    gui_text_rendermode_set(mem, 2);
    gui_text_t *low_mem = gui_text_create(fps_rect, "low_mem", 10, 16 * 3, gui_get_screen_width(),
                                          font_size);
    gui_text_set(low_mem, text, GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255), strlen(text), font_size);
    gui_text_type_set(low_mem, SF_COMPACT_REGULAR_BIN, FONT_SRC_MEMADDR);
    gui_text_rendermode_set(low_mem, 2);
}

/* Regenerate current_view*/
static void regenerate_current_view(void *msg)
{
    GUI_UNUSED(msg);

    gui_view_t *current_view = gui_view_get_current();
    const gui_view_descriptor_t *descriptor = current_view->descriptor;
    gui_obj_t *parent = current_view->base.parent;
    gui_obj_tree_free(GUI_BASE(current_view));

    gui_view_create(parent, descriptor->name, 0, 0, 0, 0);
}

static void json_refreash(void)
{
    uint16_t degree = xorshift16() % 359;
    uint16_t move = xorshift16() % 20000;
    uint16_t ex = xorshift16() % 60;
    uint16_t stand = xorshift16() % 30;
    move = move < 1 ? 1 : move;
    ex = ex < 1 ? 1 : ex;
    stand = stand < 1 ? 1 : stand;
    uint16_t AM12 = xorshift16() % 120;
    uint16_t AM6 = xorshift16() % 120;
    uint16_t PM12 = xorshift16() % 120;
    uint16_t PM6 = xorshift16() % 120;
    // gui_log("!cjson_content: %x %x %x %x %x\n", cjson_content[0], cjson_content[1], cjson_content[2], cjson_content[3], cjson_content[4]);

    cJSON *root = cJSON_Parse(cjson_content);

    if (!root)
    {
        gui_log("json_refreash Error parsing JSON!\r\n");
        return;
    }
    cJSON *compass_array = cJSON_GetObjectItem(root, "compass");
    if (compass_array != NULL && cJSON_GetArraySize(compass_array) > 0)
    {
        cJSON *compass_item = cJSON_GetArrayItem(compass_array, 0);
        cJSON_ReplaceItemInObject(compass_item, "degree", cJSON_CreateNumber(degree));
    }

    cJSON *activity_array = cJSON_GetObjectItem(root, "activity");
    if (activity_array != NULL && cJSON_GetArraySize(activity_array) > 0)
    {
        cJSON *activity_item = cJSON_GetArrayItem(activity_array, 0);
        cJSON_ReplaceItemInObject(activity_item, "move", cJSON_CreateNumber(move));
        cJSON_ReplaceItemInObject(activity_item, "exercise", cJSON_CreateNumber(ex));
        cJSON_ReplaceItemInObject(activity_item, "stand", cJSON_CreateNumber(stand));
    }

    cJSON *heart_rate_array = cJSON_GetObjectItem(root, "heart_rate");
    if (heart_rate_array != NULL && cJSON_GetArraySize(heart_rate_array) > 0)
    {
        cJSON *heart_rate_item = cJSON_GetArrayItem(heart_rate_array, 0);
        {
            AM12 = AM12 > 60 ? AM12 : 68;
            cJSON_ReplaceItemInObject(heart_rate_item, "AM12", cJSON_CreateNumber(AM12));
        }
        {
            AM6 = AM6 > 60 ? AM6 : 73;
            cJSON_ReplaceItemInObject(heart_rate_item, "AM6", cJSON_CreateNumber(AM6));
        }
        {
            PM12 = PM12 > 60 ? PM12 : 82;
            cJSON_ReplaceItemInObject(heart_rate_item, "PM12", cJSON_CreateNumber(PM12));
        }
        {
            PM6 = PM6 > 60 ? PM6 : 94;
            cJSON_ReplaceItemInObject(heart_rate_item, "PM6", cJSON_CreateNumber(PM6));
        }
    }
    char *temp = cJSON_PrintUnformatted(root);
    sprintf(cjson_content, "%s", temp);
    gui_free(temp);
    cJSON_Delete(root);
    json_refresh_flag = 0b1111;
    // gui_log("json_refresh_flag %x, line: %d\n", json_refresh_flag, __LINE__);
    // gui_log("cjson_content: %s\n", cjson_content);
}

// Update the watch time and the JSON data
static void win_cb(void *param)
{
    (void)param;
#if defined _HONEYGUI_SIMULATOR_
    time_t rawtime;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    static uint8_t count = 0;
    count++;
    if (count == 5)
    {
        count = 0;
        // json_refreash();
    }
#else
    timeinfo = &watch_time;
#endif
    sprintf(time_str, "%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min);

    // kb_button_cb();
    // inform_generate_cb();
}

// Send information to the top view
static void inform_generate_cb(void)
{
    static char *content = NULL;
    static char *time = NULL;
    static uint8_t count = 0;

    if (count < 5)
    {
        count++;
        return;
    }
    count = 0;

    if (!content)
    {
        content = gui_malloc(60);
        sprintf(content, "Watch will attempt to install this update later tonight.");

        time = gui_malloc(10);
    }
    if (timeinfo)
    {
        sprintf(time, "%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min);
    }

    // information_t payload =
    // {
    //     "watchOS 10.3.1",
    //     content,
    //     time,
    //     OS
    // };
    // extern void add_information(information_t *payload);
    // add_information(&payload);
}

static void clear_mem(gui_view_t *view)
{
    GUI_UNUSED(view);
    if (watchface_clear_mem)
    {
        watchface_clear_mem();
    }
}

static void app_main_watch_ui_design(gui_view_t *view)
{
    gui_view_switch_on_event(view, bottom_view->name, SWITCH_INIT_STATE,
                             SWITCH_IN_FROM_BOTTOM_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_UP);
    gui_view_switch_on_event(view, top_view->name, SWITCH_OUT_STILL_USE_BLUR,
                             SWITCH_IN_FROM_TOP_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_DOWN);
    gui_view_switch_on_event(view, watchface_select_view->name, SWITCH_OUT_ANIMATION_ZOOM,
                             SWITCH_IN_ANIMATION_ZOOM,
                             GUI_EVENT_TOUCH_LONG);
    gui_view_switch_on_event(view, menu_view->name, SWITCH_OUT_ANIMATION_FADE,
                             SWITCH_IN_ANIMATION_FADE,
                             GUI_EVENT_KB_SHORT_CLICKED);

    // select watchface
    if (current_watchface_type == WATCHFACE_BIG_NUM)
    {
        extern void create_watchface_number(gui_view_t *view);
        create_watchface_number((void *)view);
    }
    else if (current_watchface_type == WATCHFACE_SPORT)
    {
        extern void create_watchface_sport(gui_view_t *view);
        create_watchface_sport((void *)view);
    }
    else if (current_watchface_type == WATCHFACE_VIDEO)
    {
        extern void create_watchface_earth(gui_view_t *view);
        create_watchface_earth((void *)view);
    }
    // gui_win_t *win_kb = gui_win_create(view, "win_kb", 0, 0, 0, 0);
    // gui_obj_create_timer(GUI_BASE(win_kb), 10, true, kb_button_cb);
}

extern const unsigned char _binary_root_0x704D1400_bin_start[];
extern const unsigned char _binary_root_0x704D1400_bin_end[];
extern const unsigned char _binary_root_0x704D1400_bin_size[];

static int app_init(void)
{
#ifdef _HONEYGUI_SIMULATOR_
    resource_root = (unsigned char *)_binary_root_0x704D1400_bin_start;
#endif

    cjson_content = gui_malloc(700);
    memcpy(cjson_content, CJSON_DATA_BIN, 700);
    json_refreash();

    gui_win_t *win = gui_win_create(gui_obj_get_root(), 0, 0, 0, 0, 0);
    fps_create(gui_obj_get_root());
    gui_obj_create_timer(GUI_BASE(win), 1000, true, win_cb);
    gui_obj_start_timer(GUI_BASE(win));
    win_cb(NULL);

    gui_view_create(win, descriptor.name, 0, 0, 0, 0);

    return 0;
}
GUI_INIT_APP_EXPORT(app_init);

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
/* Send message to regenerate current_view asynchronously */
void msg_2_regenerate_current_view(void)
{
    gui_msg_t msg =
    {
        .event = GUI_EVENT_USER_DEFINE,
        .cb = regenerate_current_view,
    };
    gui_send_msg_to_server(&msg);
}

/* Generate a pseudo-random number */
uint16_t xorshift16(void)
{
    static uint16_t seed = 12345;
    seed ^= seed << 6;
    seed ^= seed >> 9;
    seed ^= seed << 2;
    return seed;
}