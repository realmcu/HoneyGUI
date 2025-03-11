#include "guidef.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "lvgl.h"
#include <sys/types.h>
#include <pthread.h>
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "lv_port_fs.h"

#include <stdio.h>
#include <time.h>
#include "app_main.h"
#include "lv_custom_tile_slide.h"
#include "lv_custom_tile_snapshot.h"

#include "kb_algo.h"
#ifdef __WIN32
#include <gui_app.h>
#include "gui_server.h"
#include "gui_components_init.h"
#endif

static void lvgl_loop(void *tihs)
{
    while ((1))
    {
        pthread_testcancel();
        lv_timer_handler();                 //! run lv task at the max speed
    }
}

static void port_log(lv_log_level_t level, const char *buf)
{
    gui_log("[LVGL]%s", buf);
}

static void lv_tick(void)
{
    lv_tick_inc(10);
}

lv_obj_t *tileview;
bool tileview_scrolling = false;
lv_obj_t *tile_center;
lv_obj_t *tile_up;
lv_obj_t *tile_down;
lv_obj_t *tile_left;
lv_obj_t *tile_right;

lv_obj_t *scr_watchface;
lv_obj_t *scr_up_curtain;
lv_obj_t *scr_down_curtain;
lv_obj_t *scr_left_curtain;
lv_obj_t *scr_right_curtain;
lv_obj_t *scr_app_menu;

#if LVGL_USE_CJSON
char *cjson_content = NULL;

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
    canvas_update_flag = 0b1101;
    // gui_log("canvas_update_flag %x, line: %d\n", canvas_update_flag, __LINE__);
//    gui_log("cjson_content: %s\n", cjson_content);
}
#endif
void read_json_cb(lv_timer_t *timer)
{
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
}
#endif
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
    if (!content)
    {
        content = gui_malloc(200);
        sprintf(content,
                "Never gonna give you up. Never gonna let you down. Never gonna run around and desert you. Never gonna give you up. Never gonna let you down. Never gonna run around and desert you.");
    }
    time_t rawtime;
    time(&rawtime);
    struct tm *timeinfo = localtime(&rawtime);
    char time[10];
    if (timeinfo)
    {
        sprintf(time, "%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min);
    }

    information_t payload =
    {
        "101010",
        content,
        time,
        MESSAGE
    };
    extern void pagelist_create(information_t *payload);
    pagelist_create(&payload);
}

static void enter_menu_cb(lv_event_t *event)
{
    kb_info_t *kb = kb_get_info();
    if (kb->pressed)
    {
        _ui_screen_change(&scr_app_menu, NULL, LV_SCR_LOAD_ANIM_FADE_IN, 500, 0,
                          lv_app_menu_init, 0);
    }
}
#ifdef __WIN32
uint8_t resource_root[1024 * 1024 * 20];
#endif
static void app_dialing_ui_design(gui_app_t *app)
{
#if defined _WIN32
    extern int open(const char *file, int flags, ...);
    extern int read(int fd, void *buf, size_t len);
    extern int close(int fd);
    defaultPath = "realgui\\example\\screen_410_502_lvgl\\root_image_lvgl\\root\\";
    int fd;
    fd = open("./realgui/example/screen_410_502_lvgl/root_image_lvgl/root(0x253E400).bin", 0);
    if (fd > 0)
    {
        printf("open root(0x4400000).bin Successful!\n");
        read(fd, resource_root, 1024 * 1024 * 20);
    }
    else
    {
        printf("open root(0x4400000).bin Fail!\n");
        printf("open root(0x4400000).bin Fail!\n");
        printf("open root(0x4400000).bin Fail!\n");
    }
#endif

    if (lv_is_initialized() == true)
    {
        return;
    }
    gui_set_tick_hook(lv_tick);
    lv_init();
    lv_log_register_print_cb((lv_log_print_g_cb_t)port_log);
    lv_port_disp_init();
    lv_port_indev_init();
    // lv_port_fs_init();

    LV_LOG("LVGL start \n");
#if LVGL_USE_CJSON
    {
        lv_timer_t *timer = lv_timer_create(read_json_cb, 3000, NULL);
        lv_timer_set_repeat_count(timer, -1);
        lv_timer_ready(timer);
    }
#endif
    {
        lv_timer_t *timer = lv_timer_create(inform_generate_task_entry, 3000, NULL);
        lv_timer_set_repeat_count(timer, -1);
        lv_timer_ready(timer);
    }

    // lv_disp_load_scr(scr_watchface);

    tileview = lv_tileview_create(NULL);
    lv_obj_set_style_bg_color(tileview, lv_color_hex(0x000000), 0);
    lv_obj_set_scrollbar_mode(tileview, LV_SCROLLBAR_MODE_OFF); // hide scroll bar
    lv_obj_add_event_cb(tileview, (lv_event_cb_t)enter_menu_cb, LV_EVENT_ALL, NULL);

    tile_center = lv_tileview_add_tile(tileview, 1, 1, LV_DIR_ALL); // create center tile
    tile_up = lv_tileview_add_tile(tileview, 1, 0, LV_DIR_BOTTOM); // create up tile
    tile_down = lv_tileview_add_tile(tileview, 1, 2, LV_DIR_TOP); // create down tile
    tile_left = lv_tileview_add_tile(tileview, 0, 1, LV_DIR_RIGHT); // create left tile
    tile_right = lv_tileview_add_tile(tileview, 2, 1, LV_DIR_LEFT); // create right tile

    scr_watchface = lv_obj_create(tile_center);
    lv_obj_remove_style_all(scr_watchface);
    lv_obj_set_size(scr_watchface, LV_PCT(100), LV_PCT(100));

    scr_up_curtain = lv_obj_create(tile_up);
    lv_obj_remove_style_all(scr_up_curtain);
    lv_obj_set_size(scr_up_curtain, LV_PCT(100), LV_PCT(100));

    scr_down_curtain = lv_obj_create(tile_down);
    lv_obj_remove_style_all(scr_down_curtain);
    lv_obj_set_size(scr_down_curtain, LV_PCT(100), LV_PCT(100));

    scr_left_curtain = lv_obj_create(tile_left);
    lv_obj_remove_style_all(scr_left_curtain);
    lv_obj_set_size(scr_left_curtain, LV_PCT(100), LV_PCT(100));

    scr_right_curtain = lv_obj_create(tile_right);
    lv_obj_remove_style_all(scr_right_curtain);
    lv_obj_set_size(scr_right_curtain, LV_PCT(100), LV_PCT(100));

    //initialize curtains
    lv_watchface_init();
    lv_up_curtain_init();
    lv_down_curtain_init();
    lv_left_curtain_init();
    lv_right_curtain_init();

    lv_tileview_set_tile_by_index(tileview, 1, 1, LV_ANIM_OFF); // start with center tile, no animation

#if WATCH_DEMO_USE_TILESLIDE
    lv_obj_add_event_cb(tileview, tileview_custom_cb, LV_EVENT_ALL, &tileview_scrolling);
#if WATCH_DEMO_USE_SNAPSHOT
    lv_obj_t *snapshot_cneter = lv_image_create(tile_center);
    lv_obj_add_flag(snapshot_cneter, LV_OBJ_FLAG_HIDDEN);

    lv_obj_t *snapshot_up = lv_image_create(tile_up);
    lv_obj_add_flag(snapshot_up, LV_OBJ_FLAG_HIDDEN);

    lv_obj_t *snapshot_down = lv_image_create(tile_down);
    lv_obj_add_flag(snapshot_down, LV_OBJ_FLAG_HIDDEN);

    lv_obj_t *snapshot_left = lv_image_create(tile_left);
    lv_obj_add_flag(snapshot_left, LV_OBJ_FLAG_HIDDEN);

    lv_obj_t *snapshot_right = lv_image_create(tile_right);
    lv_obj_add_flag(snapshot_right, LV_OBJ_FLAG_HIDDEN);

    event_snapshot_creat = lv_event_register_id();
    event_snapshot_delete = lv_event_register_id();

    lv_obj_add_event_cb(snapshot_cneter, snapshot_custom_cb_creat, event_snapshot_creat, tile_center);
    lv_obj_add_event_cb(snapshot_cneter, snapshot_custom_cb_delete, event_snapshot_delete, tile_center);

    lv_obj_add_event_cb(snapshot_up, snapshot_custom_cb_creat, event_snapshot_creat, tile_up);
    lv_obj_add_event_cb(snapshot_up, snapshot_custom_cb_delete, event_snapshot_delete, tile_up);

    lv_obj_add_event_cb(snapshot_down, snapshot_custom_cb_creat, event_snapshot_creat, tile_down);
    lv_obj_add_event_cb(snapshot_down, snapshot_custom_cb_delete, event_snapshot_delete, tile_down);

    lv_obj_add_event_cb(snapshot_left, snapshot_custom_cb_creat, event_snapshot_creat, tile_left);
    lv_obj_add_event_cb(snapshot_left, snapshot_custom_cb_delete, event_snapshot_delete, tile_left);

    lv_obj_add_event_cb(snapshot_right, snapshot_custom_cb_creat, event_snapshot_creat, tile_right);
    lv_obj_add_event_cb(snapshot_right, snapshot_custom_cb_delete, event_snapshot_delete, tile_right);

#endif
#endif
    lv_scr_load(tileview);
}

static gui_app_t app_lvgl =
{
    .screen =
    {
        .name = "app_ui_lvgl",
        .x    = 0,
        .y    = 0,
        .parent = NULL,
    },
    .ui_design = app_dialing_ui_design,
    .thread_entry = lvgl_loop,
    .active_ms = 1000000,
    .lvgl = true,
};

static int app_init(void)
{
    gui_server_init();
    gui_app_startup(&app_lvgl);
    return 0;
}

GUI_INIT_APP_EXPORT(app_init);

