/**
 * @file app_main.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "lvgl.h"
#include <time.h>

#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "lv_port_fs.h"

#include "app_main.h"
#include "lv_custom_tile_slide.h"
#include "lv_custom_tile_snapshot.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
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

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void enter_menu_cb(lv_event_t *event);
static void inform_generate_task_entry();

/**********************
 *  STATIC VARIABLES
 **********************/
static char *content = NULL;

lv_obj_t *tileview;
bool tileview_scrolling = false;
lv_obj_t *tile_center;
lv_obj_t *tile_up;
lv_obj_t *tile_down;
lv_obj_t *tile_left;
lv_obj_t *tile_right;
lv_obj_t *tile_right_2;

lv_obj_t *scr_watchface;
lv_obj_t *scr_up_curtain;
lv_obj_t *scr_down_curtain;
lv_obj_t *scr_left_curtain;
lv_obj_t *scr_right_curtain;
lv_obj_t *scr_right_curtain_2;
lv_obj_t *scr_app_menu;
lv_obj_t *scr_app_calendar;

uint32_t event_snapshot_creat;
uint32_t event_snapshot_delete;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void watch_demo_init(void)
{
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

    tileview = lv_tileview_create(NULL);
    lv_obj_set_style_bg_color(tileview, lv_color_make(0, 0, 0), 0);
    lv_obj_set_scrollbar_mode(tileview, LV_SCROLLBAR_MODE_OFF); // hide scroll bar
    // lv_obj_add_event_cb(tileview, (lv_event_cb_t)enter_menu_cb, LV_EVENT_ALL, NULL);

    tile_center = lv_tileview_add_tile(tileview, 1, 1, LV_DIR_ALL); // create center tile
    tile_up = lv_tileview_add_tile(tileview, 1, 0, LV_DIR_BOTTOM); // create up tile
    tile_down = lv_tileview_add_tile(tileview, 1, 2, LV_DIR_TOP); // create down tile
    tile_left = lv_tileview_add_tile(tileview, 0, 1, LV_DIR_RIGHT); // create left tile
    tile_right = lv_tileview_add_tile(tileview, 2, 1, LV_DIR_HOR); // create right tile
    tile_right_2 = lv_tileview_add_tile(tileview, 3, 1, LV_DIR_LEFT); // create right 2 tile

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

    scr_right_curtain_2 = lv_obj_create(tile_right_2);
    lv_obj_remove_style_all(scr_right_curtain_2);
    lv_obj_set_size(scr_right_curtain_2, LV_PCT(100), LV_PCT(100));

    //initialize curtains
    lv_watchface_init();
    lv_up_curtain_init();
    lv_down_curtain_init();
    lv_left_curtain_init();
    lv_right_curtain_init();
    lv_right_curtain_2_init();

    lv_tileview_set_tile_by_index(tileview, 1, 1, LV_ANIM_OFF); // start with center tile, no animation

#if WATCH_DEMO_USE_TILESLIDE
    lv_obj_add_event_cb(tileview, tileview_custom_cb, LV_EVENT_ALL, &tileview_scrolling);
#if WATCH_DEMO_USE_SNAPSHOT
    event_snapshot_creat = lv_event_register_id();
    event_snapshot_delete = lv_event_register_id();

    create_snapshot_obj_with_enent(tile_center, tile_center, event_snapshot_creat,
                                   event_snapshot_delete);
    create_snapshot_obj_with_enent(tile_up, tile_up, event_snapshot_creat, event_snapshot_delete);
    create_snapshot_obj_with_enent(tile_down, tile_down, event_snapshot_creat, event_snapshot_delete);
    create_snapshot_obj_with_enent(tile_left, tile_left, event_snapshot_creat, event_snapshot_delete);
    create_snapshot_obj_with_enent(tile_right, tile_right, event_snapshot_creat, event_snapshot_delete);
    create_snapshot_obj_with_enent(tile_right_2, tile_right_2, event_snapshot_creat,
                                   event_snapshot_delete);
#endif
#endif
    lv_screen_load(tileview);
}

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

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void inform_generate_task_entry()
{
    if (!content)
    {
        content = lv_malloc(200);
        sprintf(content,
                "Never gonna give you up. Never gonna let you down. Never gonna run around and desert you. Never gonna give you up. Never gonna let you down. Never gonna run around and desert you.");
    }
    struct tm watch_time = {0};
    struct tm *timeinfo = &watch_time;
#ifdef __WIN32
    time_t rawtime;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
#endif
    char time_str[10];
    if (timeinfo)
    {
        sprintf(time_str, "%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min);
    }

    information_t payload =
    {
        "101010",
        content,
        time_str,
        MESSAGE
    };
    extern void pagelist_create(information_t *payload);
    pagelist_create(&payload);
}

static void enter_menu_cb(lv_event_t *event)
{
    lv_indev_t *indev = lv_indev_get_next(NULL);
    while (indev)
    {
        if (lv_indev_get_type(indev) == LV_INDEV_TYPE_KEYPAD &&
            lv_indev_get_state(indev) == LV_INDEV_STATE_PRESSED)
        {
            _ui_screen_change(&scr_app_menu, NULL, LV_SCR_LOAD_ANIM_FADE_OUT, 300, 0,
                              lv_app_menu_init, 0);
            return;
        }
        indev = lv_indev_get_next(indev);
    }
}

