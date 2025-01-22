#include "guidef.h"
#include "gui_obj.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include <gui_app.h>
#include "lvgl.h"
#include "gui_server.h"
#include "gui_components_init.h"
#include <sys/types.h>
#include <pthread.h>
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "lv_port_fs.h"

#include <stdio.h>
#include <time.h>
#include "app_main.h"


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

lv_obj_t *scr_watchface;
lv_obj_t *scr_up_curtain;
lv_obj_t *scr_down_curtain;
lv_obj_t *scr_left_curtain;
lv_obj_t *scr_right_curtain;


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

uint8_t resource_root[1024 * 1024 * 20];
static void app_dialing_ui_design(gui_app_t *app)
{
#if defined _WIN32
    extern int open(const char *file, int flags, ...);
    extern int read(int fd, void *buf, size_t len);
    extern int close(int fd);
    defaultPath = "realgui\\example\\screen_410_502_lvgl\\root_image_lvgl\\root\\";
    int fd;
    fd = open("./realgui/example/screen_410_502_lvgl/root_image_lvgl/root(0x4400000).bin", 0);
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

    //initialize curtains
    lv_watchface_init();
    lv_up_curtain_init();
    lv_down_curtain_init();
    lv_left_curtain_init();
    lv_right_curtain_init();

    lv_timer_t *timer = lv_timer_create(read_json_cb, 3000, NULL);
    lv_timer_set_repeat_count(timer, -1);
    lv_timer_ready(timer);

    lv_disp_load_scr(scr_watchface);
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

