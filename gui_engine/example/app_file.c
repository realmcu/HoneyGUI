/*************************app data start**********************/
/*1. gui head file*/
#include <gui_img.h>
#include <gui_win.h>
#include <gui_text.h>
#include <gui_obj.h>
#include <gui_iconlist.h>
#include "gui_server.h"
#include <gui_app.h>
#include <string.h>
#include "rtk_gui_resource.h"
/*2. app pointer*/
struct get_file_name
{
    uint8_t name_type;
    const char *name;
};

static     gui_win_t *win;
static     uint32_t win_height;
static     uint32_t win_width;
static     uint32_t icon_height;
static     const char *font_type;
static     uint32_t text_color;
static     struct get_file_name path_node[10];
static     uint8_t path_node_count;

static void app_file_ui_design(gui_app_t *app);
//    file
static gui_app_t app_file =
{
    .screen =
    {
        .name = "app_file",
        .x    = 0,
        .y    = 0,
    },
    .ui_design = app_file_ui_design,
};

void *get_app_file(void)
{
    return &app_file;
}
/*5. trigger callback function*/
static void file_click_cb(void *obj, gui_event_t e);
static void win_base_page_cb(void);
static void create_menu(void);
/*************************app data end************************/
/*************************user data start**********************/


/*************************user data end************************/
uint32_t get_file_count(const char *pathname)
{


    uint32_t file_count = 0;

    /* list directory */
    gui_fs_DIR *dir = gui_fs_opendir(pathname);
    if (dir != NULL)
    {
        while ((gui_fs_readdir(dir)) != NULL)
        {
            file_count++;
        }
        gui_fs_closedir(dir);
    }
    return file_count;
}
struct get_file_name *last_file_name_buffer;
uint32_t last_file_count;
void close_file_app_free(void)
{
    if (last_file_name_buffer != NULL)
    {
        for (size_t i = 0; i < last_file_count; i++)
        {
            gui_free((void *)(last_file_name_buffer[i].name));
        }
        gui_free(last_file_name_buffer);
    }
    last_file_name_buffer = NULL;
    last_file_count = 0;
}
struct get_file_name *get_file_name(const char *pathname, uint32_t file_count)
{
    if (last_file_name_buffer != NULL)
    {
        for (size_t i = 0; i < last_file_count; i++)
        {
            gui_free((void *)(last_file_name_buffer[i].name));
        }

        gui_free(last_file_name_buffer);
    }
    uint32_t name_count = 0;
    struct get_file_name *file_name_buffer = gui_malloc(sizeof(struct get_file_name) * file_count);
    memset(file_name_buffer, 0, sizeof(struct get_file_name) * file_count);
    last_file_name_buffer = file_name_buffer;
    last_file_count = file_count;
    /* list directory */
    gui_fs_DIR *dir = gui_fs_opendir(pathname);
    if (dir != NULL)
    {
        struct gui_fs_dirent *ptr;
        while ((ptr = gui_fs_readdir(dir)) != NULL)
        {
            file_name_buffer[name_count].name = gui_strdup(ptr->d_name);
            if (ptr->d_type == 1)
            {
                file_name_buffer[name_count].name_type = 0;
            }
            else if (ptr->d_type == 2)
            {
                file_name_buffer[name_count].name_type = 1;
            }
            name_count++;
        }
        gui_fs_closedir(dir);
    }
    if (name_count == file_count)
    {
        return file_name_buffer;
    }
    return NULL;
}
static void win_base_page_cb(void)
{
    close_file_app_free();
    extern void *get_launcher_app(void);
    gui_app_t *app = get_app_file();
    gui_switch_app(app, get_launcher_app());
}
static void return_to_last_page_cb(void *obj, gui_event_t e)
{
    gui_app_t *app = get_app_file();
    struct get_file_name *file;
    if (path_node_count == 0)
    {
        win_base_page_cb();
        return;
    }
    else
    {
        gui_free((void *)(path_node[path_node_count].name));
        file = &(path_node[--(path_node_count)]);
    }
    if (file->name_type == 1)
    {
        gui_tree_free(win);
        create_menu();
    }
    //gui_exec(&(app->screen));

}

static void file_click_cb(void *obj, gui_event_t e)
{
    gui_app_t *app = get_app_file();
    struct get_file_name *file = obj;
    if (file->name_type == 1)
    {
        path_node_count++;
        memcpy(&(path_node[path_node_count]), obj, sizeof(struct get_file_name));
        path_node[path_node_count].name = gui_strdup(path_node[path_node_count].name);
        gui_tree_free(win);
        create_menu();
    }
}
void create_menu()
{
    gui_app_t *app = get_app_file();
    size_t full_path_length = 0;
    for (size_t i = 0; i < path_node_count + 1; i++)
    {
        full_path_length += strlen(path_node[i].name);
        full_path_length ++;
    }
    char full_path[full_path_length + 1];
    memset(full_path, 0, full_path_length + 1);
    for (size_t i = 0; i < path_node_count + 1; i++)
    {
        strcat(full_path, path_node[i].name);
        strcat(full_path, "/");
    }
    uint32_t file_count = get_file_count(full_path);
    struct get_file_name *file_child = get_file_name(full_path, file_count);
    win = gui_win_create(&(app->screen), "file", 0, 0, win_width, win_height);
    gui_obj_add_event_cb(win, return_to_last_page_cb, GUI_EVENT_TOUCH_TOUCH_RIGHT_SLIDE, NULL);
    gui_iconlist_t *menu = gui_iconlist_create(win, "menu", 0, 0, win_width,
                                               win_height, NULL, NULL);
    menu->set_background(menu, ICONBACK_BIN, ICONBACKHIGHLIGHT_BIN);
    for (size_t i = 0; i < file_count; i++)
    {
        gui_icon_t *row = gui_icon_create(menu, "row", 0, 0, win_width, icon_height, i);
        gui_obj_add_event_cb(row, file_click_cb, GUI_EVENT_TOUCH_CLICKED, &(file_child[i]));
        gui_text_t *text = gui_text_create(row, "text", 2, 15, win_width, icon_height);
        gui_text_set(text, file_child[i].name, (char *)(font_type), text_color,
                     strlen(file_child[i].name), 32);
    }
    //gui_exec(&(app->screen));
}
static void app_file_ui_design(gui_app_t *app)
{
    font_type = "rtk_font_mem32";
    text_color = 0xffff;
    icon_height = 65;
    win_height = 320;
    win_width = 320;
    path_node[0].name = "/";
    path_node[0].name_type = 1;
    path_node_count = 0;
    create_menu();
}


