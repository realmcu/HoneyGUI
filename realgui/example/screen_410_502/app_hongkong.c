#include <gui_tabview.h>
#include "gui_card.h"
#include <gui_obj.h>
#include <gui_win.h>
#include <gui_text.h>
#include <gui_curtain.h>
#include "root_image_hongkong/ui_resource.h"
#include <gui_app.h>
#include "gui_tab.h"
#include "app_hongkong.h"
#include "gui_win.h"
#include "gui_server.h"
#include "gui_components_init.h"
#include <stdio.h>
#include "guidef.h"
#include "wheel_algo.h"
#include "kb_algo.h"
#include <time.h>
#include "gui_fps.h"
#include "gui_view.h"
#include "cJSON.h"

#define CURRENT_VIEW_NAME "watchface_view"

static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *menu_view = NULL;
const static gui_view_descriptor_t *app_control_view = NULL;
const static gui_view_descriptor_t *app_down_view = NULL;
const static gui_view_descriptor_t *app_up_view = NULL;
const static gui_view_descriptor_t *activity_view = NULL;
const static gui_view_descriptor_t *watchface_select_view = NULL;

static void watchface_design(gui_view_t *view);
static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .design_cb = watchface_design,
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
    menu_view = gui_view_descriptor_get("menu_view");
    app_control_view = gui_view_descriptor_get("app_control_view");
    app_down_view = gui_view_descriptor_get("app_down_view");
    app_up_view = gui_view_descriptor_get("app_up_view");
    activity_view = gui_view_descriptor_get("activity_view");
    watchface_select_view  = gui_view_descriptor_get("watchface_select_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

static void app_hongkong_ui_design(gui_app_t *app);

bool control_flag = 0;
static bool enter_menu_flag = 0;

char watchface_path[100];
uint8_t watchface_index = 1;

static gui_app_t app_hongkong =
{
    .screen =
    {
        .name = "app_hongkong",
        .x    = 0,
        .y    = 0,
        .parent = NULL,
    },
    .ui_design = app_hongkong_ui_design,
    .active_ms = 1000000,
    // .shutdown_animation_flag = GUI_APP_ANIMATION_10,
    // .startup_animation_flag = GUI_APP_ANIMATION_9,
};

gui_app_t *get_app_hongkong(void)
{
    return &app_hongkong;
}

static void kb_button_cb()
{
    extern gui_kb_port_data_t *kb_get_data(void);
    gui_kb_port_data_t *kb = kb_get_data();
    static uint32_t time_press = 0;
    static uint8_t hold = 0;
    static bool press_his = 0;
    static uint32_t release_his = 0;
    if (hold)
    {
        if (kb->event == GUI_KB_EVENT_UP)
        {
            hold = 0;
            uint32_t time = kb->timestamp_ms_release - time_press;
            if (time <= 300)
            {
                // gui_log("pressing time = %d\n", time);
                if (press_his && (kb->timestamp_ms_release - release_his) < 1000)
                {
                    gui_log("change menu style\n");
                    extern uint8_t menu_style;
                    menu_style++;
                    menu_style %= 3;
                }
                press_his = !press_his;
                release_his = kb->timestamp_ms_release;
                return;
            }
            else
            {
                gui_log("pressing time = %d\n", time);
                press_his = 0;
                gui_view_switch_direct(gui_view_get_current_view(), menu_view, VIEW_ANIMATION_6, VIEW_ANIMATION_1);
            }
        }
    }
    if (kb->event == GUI_KB_EVENT_DOWN && !hold)
    {
        time_press = kb->timestamp_ms_press;
        hold = 1;
    }
    // kb_info_t *kb = kb_get_info();
    // if (kb->short_click_twice)
    // {
    //     gui_log("change menu style\n");
    //     extern uint8_t menu_style;
    //     menu_style++;
    //     menu_style %= 3;
    // }
}


char *cjson_content = NULL;
uint8_t canvas_update_flag = 0;
struct tm *timeinfo;
static struct tm watch_time;

static uint16_t seed = 12345;
uint16_t xorshift16()
{
    seed ^= seed << 6;
    seed ^= seed >> 9;
    seed ^= seed << 2;
    return seed;
}

#if defined __WIN32
const char *filename =
    "./realgui/example/web/peripheral_simulation/json/simulation_data.json";
/* read CJSON to string */
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
    char *content = (char *)malloc(length + 1);
    if (content)
    {
        fread(content, 1, length, file);
        content[length] = '\0';
    }
    fclose(file);
    return content;
}
#else
// #include "tuya_ble_feature_weather.h"
// #include "watch_clock.h"
#include "cJSON.h"
void json_refreash()
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
    canvas_update_flag = 0b1111;
    // gui_log("canvas_update_flag %x, line: %d\n", canvas_update_flag, __LINE__);
//    gui_log("cjson_content: %s\n", cjson_content);
}
#endif

void win_cb(void *p, void *this_widget, gui_animate_t *animate)
{
    if (animate->Beginning_frame)
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
            free(temp);
        }
        canvas_update_flag = 0b1111;
        // gui_log("canvas_update_flag %x\n", canvas_update_flag);
#else
        // extern struct tm watch_clock_get(void);
        // watch_time = watch_clock_get();
        timeinfo = &watch_time;
        // gui_log("time %d:%d\r\n", timeinfo->tm_hour, timeinfo->tm_min);
        // gui_log("date %d:%d\r\n", timeinfo->tm_mon + 1, timeinfo->tm_mday);
        // json_refreash();
//        tuya_ble_feature_weather_data_request(WKT_TEMP | WKT_THIHG | WKT_TLOW | WKT_CONDITION, 5);
#endif
    }
    // kb_button_cb();
}

static void app_hongkong_ui_design(gui_app_t *app)
{
    gui_log("app_hongkong_ui_design\n");

    enter_menu_flag = 0;

#if defined __WIN32
    cjson_content = read_file(filename);
    if (!cjson_content)
    {
        perror("fopen");
    }
#else
    cjson_content = gui_malloc(700);
    memcpy(cjson_content, TUYA_CJSON_BIN, 700);
#endif

    gui_win_t *win = gui_win_create(GUI_APP_ROOT_SCREEN, "view_win", 0, 0, 0, 0);
    gui_view_t *view = gui_view_create(win, &descriptor, 0, 0, 0, 0);

    gui_win_set_animate(win, 2000, -1, win_cb, NULL);
    gui_fps_create(GUI_APP_ROOT_SCREEN);
}

void clear_mem(void)
{
    gui_view_t *view = gui_view_get_current_view();
    if (!view)
    {
        return;
    }
    const char *name = GUI_BASE(view)->name;

    if (strcmp(name, "heartrate_view"))
    {
        extern void clear_heart_rate_cache(void);
        clear_heart_rate_cache();
    }
    if (strcmp(name, "box2d_ring_view"))
    {
        extern void close_box2d_ring(void);
        close_box2d_ring();
    }
    if (strcmp(name, "fruit_ninja_view"))
    {
        extern void close_FN(void);
        close_FN();
    }
    if (strcmp(name, "activity_view"))
    {
        extern void clear_activity(void);
        clear_activity();
    }
    if (strcmp(name, "watchface_view"))
    {
        extern void clear_clock(void);
        clear_clock();
    }
    if (strcmp(name, "app_down_view"))
    {
        extern void clear_down_view(void);
        clear_down_view();
    }
    if (strcmp(name, "music_view"))
    {
        extern void clear_music();
        clear_music();
    }
}

static void watchface_design(gui_view_t *view)
{
    clear_mem();

    gui_view_switch_on_event(view, app_down_view, VIEW_STILL, VIEW_TRANSPLATION,
                             GUI_EVENT_TOUCH_MOVE_UP);
    gui_view_switch_on_event(view, app_up_view, VIEW_STILL, VIEW_TRANSPLATION,
                             GUI_EVENT_TOUCH_MOVE_DOWN);
    gui_view_switch_on_event(view, activity_view, VIEW_CUBE, VIEW_CUBE,
                             GUI_EVENT_TOUCH_MOVE_LEFT);
    gui_view_switch_on_event(view, app_control_view, VIEW_STILL, VIEW_TRANSPLATION,
                             GUI_EVENT_TOUCH_MOVE_RIGHT);
    gui_view_switch_on_event(view, watchface_select_view, VIEW_ANIMATION_8, VIEW_ANIMATION_5,
                             GUI_EVENT_TOUCH_LONG);

    extern void page_ct_clock(gui_view_t *view);
    extern void create_tree_nest(const char *xml, void *obj);
    extern void create_watchface_bf(gui_view_t *view);
    extern void create_watchface_ring(gui_view_t *view);
    switch (watchface_index)
    {
    case 0:
        {
            create_tree_nest((void *)watchface_path, view);
        }
        break;
    case 1:
        {
            page_ct_clock(view);
        }
        break;
    case 2:
        {
            create_watchface_bf(view);
        }
        break;
    case 3:
        {
            create_watchface_ring(view);
        }
        break;
    case 4:
        {
            create_tree_nest((void *)watchface_path, view);
        }
        break;
    case 5:
        {
            create_tree_nest((void *)watchface_path, view);
        }
        break;
    default:
        page_ct_clock(view);
        break;
    }
}

// static void data_generate_task_entry()
// {
//     while(true)
//     {
//         extern void json_refreash();
//         json_refreash();
//         gui_thread_mdelay(2000);
//     }
// }

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
static char *content = NULL;
static void inform_generate_task_entry()
{
    while (true)
    {
        gui_thread_mdelay(2000);

        if (!content)
        {
            content = gui_malloc(60);
            sprintf(content, "Watch will attempt to install this update later tonight.");
        }
        extern struct tm *timeinfo;
        char time[10];
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
        extern void pagelist_create(information_t *payload);
        gui_msg_t msg =
        {
            .event = GUI_EVENT_USER_DEFINE,
            .payload = &payload,
            .cb = (gui_msg_cb)pagelist_create,
        };

        gui_send_msg_to_server(&msg);
    }
}
#if defined _WIN32
uint8_t resource_root[1024 * 1024 * 20];
#endif
static int app_init(void)
{
#if defined _WIN32
    extern int open(const char *file, int flags, ...);
    extern int read(int fd, void *buf, size_t len);
    extern int close(int fd);
    defaultPath = "realgui\\example\\screen_410_502\\root_image_hongkong\\root\\";
    int fd;
    fd = open("./realgui/example/screen_410_502/root_image_hongkong/root(0x253E400).bin", 0);
    if (fd > 0)
    {
        printf("open root(0x253E400).bin Successful!\n");
        read(fd, resource_root, 1024 * 1024 * 20);
    }
    else
    {
        printf("open root(0x253E400).bin Fail!\n");
        printf("open root(0x253E400).bin Fail!\n");
        printf("open root(0x253E400).bin Fail!\n");
        return 0;
    }
#endif
    gui_server_init();
    gui_app_startup(get_app_hongkong());
    // gui_thread_create("data_generate_task", data_generate_task_entry, 0, 1024 * 2, 2);
    gui_thread_create("inform_generate_task_entry", inform_generate_task_entry, 0, 1024 * 2, 2);
    return 0;
}

GUI_INIT_APP_EXPORT(app_init);
