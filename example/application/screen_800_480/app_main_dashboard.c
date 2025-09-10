/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <stdio.h>
#include <time.h>
#include "cJSON.h"
#include "gui_obj.h"
#include "gui_text.h"
#include "gui_win.h"
#include "guidef.h"
#include "kb_algo.h"
#include "gui_canvas_rect.h"
#include "gui_view.h"
#include "app_main_dashboard.h"
#include "shell.h"

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


/*============================================================================*
 *                            Variables
 *============================================================================*/
/* VIEW */
const static gui_view_descriptor_t *start_view = NULL;
const static gui_view_descriptor_t *dashboard_view = NULL;

/* FPS */
static char fps[10];
static char widget_count_string[20];
static char mem_string[20];
static char low_mem_string[20];

#ifdef _WIN32
uint8_t *resource_root = NULL;

static gui_dashboard_t dashboard_info =
{
    .bt_status = 1,
    .wifi_status = 0,
    .led0_status = 1,
    .led1_status = 0,
    .led2_status = 1,
    .led3_status = 0,
    .led4_status = 1,
    .led5_status = 0,
    .led_turn_l_status = 0,
    .led_turn_r_status = 1,
    .speed_val = 88,
    .power_val = 50,
    .odo_val = 1234,
    .soc_val = 75,
    .temp_val = 30,
    .location = "Suzhou",

};
#endif



/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static int gui_view_get_other_view_descriptor_init(void)
{
    /* you can get other view descriptor point here */
    start_view = gui_view_descriptor_get("start_view");
    dashboard_view = gui_view_descriptor_get("dashboard_view");
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
    int font_size = 32;
    gui_canvas_rect_t *fps_rect = gui_canvas_rect_create(parent, "rect_fps",
                                                         gui_get_screen_width() / 2 - 200 / 2, 0, 200,
                                                         70,
                                                         APP_COLOR_GRAY_OPACITY(150));
    gui_obj_create_timer(GUI_BASE(fps_rect), 10, true, gui_fps_cb);
    sprintf(fps, "FPS:%d", gui_fps());
    text = fps;
    gui_text_t *t_fps = gui_text_create(fps_rect, "t_fps", 10, 0, gui_get_screen_width(), font_size);
    gui_text_set(t_fps, text, GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255), strlen(text), font_size);
    gui_text_type_set(t_fps, HARMONYOS_SANS_BIN, FONT_SRC_MEMADDR);
    gui_text_rendermode_set(t_fps, 2);
    gui_text_t *widget_count = gui_text_create(fps_rect, "widget_count", 10, 16, gui_get_screen_width(),
                                               font_size);
    gui_text_set(widget_count, text, GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255), strlen(text), font_size);
    gui_text_type_set(widget_count, HARMONYOS_SANS_BIN, FONT_SRC_MEMADDR);
    gui_text_rendermode_set(t_fps, 2);
    gui_text_t *mem = gui_text_create(fps_rect, "mem", 10, 16 * 2, gui_get_screen_width(), font_size);
    gui_text_set(mem, text, GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255), strlen(text), font_size);
    gui_text_type_set(mem, HARMONYOS_SANS_BIN, FONT_SRC_MEMADDR);
    gui_text_rendermode_set(t_fps, 2);
    gui_text_t *low_mem = gui_text_create(fps_rect, "low_mem", 10, 16 * 3, gui_get_screen_width(),
                                          font_size);
    gui_text_set(low_mem, text, GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255), strlen(text), font_size);
    gui_text_type_set(low_mem, HARMONYOS_SANS_BIN, FONT_SRC_MEMADDR);
    gui_text_rendermode_set(t_fps, 2);
}

#ifdef _WIN32
void dashboard_info_update(int argc, char *argv[])
{
    if (get_dashboard_info() == NULL) { return; }
    int i = 1;
    while (i < argc - 1)
    {
        if (!strcmp(argv[i], "map"))
        {
            if (dashboard_info.map_data_update) { break; }

            extern int open(const char *file, int flags, ...);
            extern int read(int fd, void *buf, size_t len);
            extern int close(int fd);
            char path[100] = "example/application/screen_800_480/root_image_800_480/root/resource/map_";
            strcat(path, argv[++i]);
            strcat(path, ".bin");
            int fd;
            fd = open(path, 0);
            if (fd < 0)
            {
                printf("open %s Fail!\n", path);
                break;
            }
            void *map_data = gui_malloc(1024 * 500);
            ssize_t bytes_read = read(fd, map_data, 1024 * 500);
            if (bytes_read < 0)
            {
                printf("read bin file failed!\n");
                close(fd);
                break;
            }
            close(fd);

            dashboard_info.map_data_update = 1;
            dashboard_info.map_data_index ^= 1;
            if (dashboard_info.map_data[dashboard_info.map_data_index])
            {
                gui_free(dashboard_info.map_data[dashboard_info.map_data_index]);
            }
            dashboard_info.map_data[dashboard_info.map_data_index] = map_data;
        }
        else if (!strcmp(argv[i], "speed"))
        {
            dashboard_info.speed_val = atoi(argv[++i]);
        }
        else if (!strcmp(argv[i], "power"))
        {
            dashboard_info.power_val = atoi(argv[++i]);
        }
        else if (!strcmp(argv[i], "odo"))
        {
            dashboard_info.odo_val = atoi(argv[++i]);
        }
        else if (!strcmp(argv[i], "soc"))
        {
            dashboard_info.soc_val = atoi(argv[++i]);
        }
        else if (!strcmp(argv[i], "temp"))
        {
            dashboard_info.temp_val = atoi(argv[++i]);
        }
        else if (!strcmp(argv[i], "location"))
        {
            snprintf(dashboard_info.location, sizeof(dashboard_info.location), "%s", argv[++i]);
        }
        else if (!strcmp(argv[i], "turn_l"))
        {
            dashboard_info.led_turn_l_status = atoi(argv[++i]);
        }
        else if (!strcmp(argv[i], "turn_r"))
        {
            dashboard_info.led_turn_r_status = atoi(argv[++i]);
        }
        else if (!strcmp(argv[i], "bt"))
        {
            dashboard_info.bt_status = atoi(argv[++i]);
        }
        else if (!strcmp(argv[i], "wifi"))
        {
            dashboard_info.wifi_status = atoi(argv[++i]);
        }
        else if (!strcmp(argv[i], "led0"))
        {
            dashboard_info.led0_status = atoi(argv[++i]);
        }
        else if (!strcmp(argv[i], "led1"))
        {
            dashboard_info.led1_status = atoi(argv[++i]);
        }
        else if (!strcmp(argv[i], "led2"))
        {
            dashboard_info.led2_status = atoi(argv[++i]);
        }
        else if (!strcmp(argv[i], "led3"))
        {
            dashboard_info.led3_status = atoi(argv[++i]);
        }
        else if (!strcmp(argv[i], "led4"))
        {
            dashboard_info.led4_status = atoi(argv[++i]);
        }
        else if (!strcmp(argv[i], "led5"))
        {
            dashboard_info.led5_status = atoi(argv[++i]);
        }
        ++i;
    }
}
SHELL_EXPORT_CMD(
    SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN) | SHELL_CMD_DISABLE_RETURN,
    cmd, dashboard_info_update, test);
#endif

extern const unsigned char _binary_root_0x4400000_bin_start[];
extern const unsigned char _binary_root_0x4400000_bin_end[];
extern const unsigned char _binary_root_0x4400000_bin_size[];

static int app_init(void)
{
#ifdef _WIN32
    resource_root = (uint8_t *)_binary_root_0x4400000_bin_start;

    gui_dashboard_info_register(&dashboard_info);
#endif
    win_map = gui_win_create(gui_obj_get_root(), 0, 0, 0, 0, 0);
    gui_win_t *win_view = gui_win_create(gui_obj_get_root(), 0, 0, 0, 0, 0);
    gui_view_create(win_view, start_view, 0, 0, 0, 0);
    return 0;
}
GUI_INIT_APP_EXPORT(app_init);
