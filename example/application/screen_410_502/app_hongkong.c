/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <stdio.h>
#include <time.h>
#include "cJSON.h"
#include "root_image_hongkong/ui_resource.h"
#include "gui_obj.h"
#include "gui_text.h"
#include "gui_win.h"
#include "guidef.h"
#include "kb_algo.h"
#include "wheel_algo.h"
#include "gui_canvas_rect.h"
#include "gui_view.h"
#include "gui_server.h"
#include "app_hongkong.h"

/*============================================================================*
 *                           Types
 *============================================================================*/
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

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "watchface_view"

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void watchface_design(gui_view_t *view);
#ifdef __WIN32
char *read_file(const char *file_path);
#endif
static void inform_generate_cb();
extern void clear_watchface_classic(gui_view_t *view);

/*============================================================================*
 *                            Variables
 *============================================================================*/
/* VIEW */
static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *menu_view = NULL;
const static gui_view_descriptor_t *app_control_view = NULL;
const static gui_view_descriptor_t *app_bottom_view = NULL;
const static gui_view_descriptor_t *app_top_view = NULL;
const static gui_view_descriptor_t *activity_view = NULL;
const static gui_view_descriptor_t *labubu_digital_view = NULL;
static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = watchface_design,
    .on_switch_out = clear_watchface_classic,
    .keep = false,
};

bool menu_style = 0;
char *cjson_content = NULL;
uint8_t json_refeash_flag = 0;
struct tm *timeinfo;
static struct tm watch_time;

/* FPS */
static gui_canvas_rect_t *fps_rect;
static char fps[10];
static char widget_count_string[20];
static char mem_string[20];
static char low_mem_string[20];

#ifdef _WIN32
uint8_t resource_root[1024 * 1024 * 20];
const char *filename =
    "./example/application/screen_410_502/root_image_hongkong/web/peripheral_simulation/json/simulation_data.json";
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
    menu_view = gui_view_descriptor_get("menu_view");
    app_control_view = gui_view_descriptor_get("app_control_view");
    app_bottom_view = gui_view_descriptor_get("app_bottom_view");
    app_top_view = gui_view_descriptor_get("app_top_view");
    activity_view = gui_view_descriptor_get("activity_view");
    labubu_digital_view  = gui_view_descriptor_get("labubu_digital_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

static void gui_fps_cb(void *p)
{
    int fps_num = gui_fps();
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
    int font_size = 24;
    fps_rect = gui_canvas_rect_create(parent, 0, gui_get_screen_width() / 2 - 140 / 2, 0, 140, 70,
                                      APP_COLOR_GRAY_OPACITY(150));
    gui_obj_create_timer(GUI_BASE(fps_rect), 17, true, gui_fps_cb);
    sprintf(fps, "FPS:%d", gui_fps());
    text = fps;
    gui_text_t *t_fps = gui_text_create(fps_rect, "t_fps", 10, 0, gui_get_screen_width(), font_size);
    gui_text_set(t_fps, text, GUI_FONT_SRC_BMP, gui_rgb(255, 255, 255), strlen(text), font_size);
    gui_text_type_set(t_fps, SOURCEHANSANSSC_SIZE24_BITS1_FONT_BIN, FONT_SRC_MEMADDR);
    gui_text_t *widget_count = gui_text_create(fps_rect, "widget_count", 10, 16, gui_get_screen_width(),
                                               font_size);
    gui_text_set(widget_count, text, GUI_FONT_SRC_BMP, gui_rgb(255, 255, 255), strlen(text), font_size);
    gui_text_type_set(widget_count, SOURCEHANSANSSC_SIZE24_BITS1_FONT_BIN, FONT_SRC_MEMADDR);
    gui_text_t *mem = gui_text_create(fps_rect, "mem", 10, 16 * 2, gui_get_screen_width(), font_size);
    gui_text_set(mem, text, GUI_FONT_SRC_BMP, gui_rgb(255, 255, 255), strlen(text), font_size);
    gui_text_type_set(mem, SOURCEHANSANSSC_SIZE24_BITS1_FONT_BIN, FONT_SRC_MEMADDR);
    gui_text_t *low_mem = gui_text_create(fps_rect, "low_mem", 10, 16 * 3, gui_get_screen_width(),
                                          font_size);
    gui_text_set(low_mem, text, GUI_FONT_SRC_BMP, gui_rgb(255, 255, 255), strlen(text), font_size);
    gui_text_type_set(low_mem, SOURCEHANSANSSC_SIZE24_BITS1_FONT_BIN, FONT_SRC_MEMADDR);
}

// Enter menu and change menu style by button
static void kb_button_cb()
{
    extern gui_kb_port_data_t *kb_get_data(void);
    gui_kb_port_data_t *kb = kb_get_data();
    static uint32_t time_press = 0;
    static bool hold = 0;
    static bool press_his = 0;
    static uint32_t release_his = 0;
    if (hold)
    {
        if (kb->event == GUI_KB_EVENT_UP)
        {
            hold = 0;
            uint32_t time = kb->timestamp_ms_release - time_press;
            if (time <= 150)
            {
                // Press twice quickly to change menu style
                if (press_his && (kb->timestamp_ms_release - release_his) < 1000)
                {
                    gui_log("change menu style\n");
                    menu_style = !menu_style;
                }
                press_his = !press_his;
                release_his = kb->timestamp_ms_release;
                return;
            }
            else
            {
                // Press once to enter menu
                press_his = 0;
                gui_view_switch_direct(gui_view_get_current(), menu_view, SWITCH_OUT_ANIMATION_FADE,
                                       SWITCH_IN_ANIMATION_FADE);
            }
        }
    }
    else if (kb->event == GUI_KB_EVENT_DOWN)
    {
        time_press = kb->timestamp_ms_press;
        hold = 1;
    }
}

/* Generate a pseudo-random number */
static uint16_t xorshift16()
{
    static uint16_t seed = 12345;
    seed ^= seed << 6;
    seed ^= seed >> 9;
    seed ^= seed << 2;
    return seed;
}

#ifndef __WIN32
static void json_refreash()
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
    json_refeash_flag = 0b1111;
    // gui_log("json_refeash_flag %x, line: %d\n", json_refeash_flag, __LINE__);
    // gui_log("cjson_content: %s\n", cjson_content);
}
#endif

// Update the watch time and the JSON data
static void win_cb()
{
#if defined __WIN32
    time_t rawtime;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    char *temp = cjson_content;
    cjson_content = read_file(filename);
    if (!cjson_content)
    {
        cjson_content = temp;
        perror("fopen");
    }
    else
    {
        gui_free(temp);
    }
    if (gui_view_get_next() == NULL)
    {
        json_refeash_flag = 0b1111;
    }
    // gui_log("json_refeash_flag %x\n", json_refeash_flag);
#else
    static uint8_t count = 0;
    count++;
    if (count >= 5)
    {
        count = 0;
        json_refreash();
    }
    // extern struct tm watch_clock_get(void);
    // watch_time = watch_clock_get();
    timeinfo = &watch_time;
#endif
    inform_generate_cb();
}

static void watchface_design(gui_view_t *view)
{
    /* view layout */
    gui_view_switch_on_event(view, app_bottom_view, SWITCH_INIT_STATE,
                             SWITCH_IN_FROM_BOTTOM_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_UP);
    gui_view_switch_on_event(view, app_top_view, SWITCH_OUT_STILL_USE_BLUR,
                             SWITCH_IN_FROM_TOP_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_DOWN);
    gui_view_switch_on_event(view, activity_view, SWITCH_OUT_TO_LEFT_USE_ROTATE,
                             SWITCH_IN_FROM_RIGHT_USE_ROTATE,
                             GUI_EVENT_TOUCH_MOVE_LEFT);
    gui_view_switch_on_event(view, app_control_view, SWITCH_OUT_TO_RIGHT_USE_ROTATE,
                             SWITCH_IN_FROM_LEFT_USE_ROTATE,
                             GUI_EVENT_TOUCH_MOVE_RIGHT);

    extern void create_watchface_classic(gui_view_t *view);
    create_watchface_classic(view);

    gui_win_t *win_kb = gui_win_create(view, "win_kb", 0, 0, 0, 0);
    gui_obj_create_timer(GUI_BASE(win_kb), 10, true, kb_button_cb);

    if (fps_rect)
    {
        gui_obj_start_timer(GUI_BASE(fps_rect)); //reset timer
    }
    gui_obj_start_timer(GUI_BASE(view->base.parent)); //reset timer
}

// Send information to the top view
static void inform_generate_cb()
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

    information_t payload =
    {
        "watchOS 10.3.1",
        content,
        time,
        OS
    };
    extern void add_information(information_t *payload);
    add_information(&payload);
}

static void app_hongkong_ui_design(void)
{
    gui_log("app_hongkong_ui_design\n");

#if defined __WIN32
    cjson_content = read_file(filename);
    if (!cjson_content)
    {
        perror("fopen");
    }
#else
    cjson_content = gui_malloc(700);
    memcpy(cjson_content, CJSON_DATA_BIN, 700);
    json_refreash();
#endif
    gui_win_t *win = gui_win_create(gui_obj_get_root(), "app_hongkong_win", 0, 0, 0, 0);
    gui_view_t *view = gui_view_create(win, labubu_digital_view, 0, 0, 0, 0); // watch turn on animation
    fps_create(gui_obj_get_root());
    gui_obj_create_timer(GUI_BASE(win), 1000, true, win_cb);
    win_cb();
}

static int app_init(void)
{
#ifdef _WIN32
    extern int open(const char *file, int flags, ...);
    extern int read(int fd, void *buf, size_t len);
    extern int close(int fd);
    defaultPath = "example\\application\\screen_410_502\\root_image_hongkong\\root\\";
    int fd;
    fd = open("./example/application/screen_410_502/root_image_hongkong/root(0x704D1000).bin", 0);
    if (fd > 0)
    {
        printf("open root(0x704D1000).bin Successful!\n");
        read(fd, resource_root, 1024 * 1024 * 20);
    }
    else
    {
        printf("open root(0x704D1000).bin Fail!\n");
        printf("open root(0x704D1000).bin Fail!\n");
        printf("open root(0x704D1000).bin Fail!\n");
        return 0;
    }
#endif
    app_hongkong_ui_design();

    return 0;
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
#if defined __WIN32
// Read CJSON to string
char *read_file(const char *file_path)
{
    const char *path = NULL;
    if (!file_path)
    {
        path = filename;
    }
    else
    {
        path = file_path;
    }
    FILE *file = fopen(path, "r");
    if (!file)
    {
        perror("fopen");
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *content = (char *)gui_malloc(length + 1);
    if (content)
    {
        fread(content, 1, length, file);
        content[length] = '\0';
    }
    fclose(file);
    return content;
}
#endif


GUI_INIT_APP_EXPORT(app_init);
