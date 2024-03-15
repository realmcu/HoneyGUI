#include <gui_tabview.h>
#include <gui_img.h>
#include <gui_win.h>
#include <gui_text.h>
#include <gui_obj.h>
#include <gui_curtain.h>
#include <gui_button.h>
#include "gui_canvas.h"
#include <string.h>
#include <time.h>
#include <stdio.h>
#include "gui_server.h"
#include <gui_app.h>
#include "gui_tab.h"
#include "gui_grid.h"
#include "gui_return.h"
#ifdef MODULE_VG_LITE
#include "gui_vg_lite_clock.h"
#include "gui_cube.h"
#else
#endif
#include "gui_components_init.h"
static void app_launcher_frontend_ui_design(gui_app_t *app);
static gui_app_t app_launcher_frontend =
{
    .screen =
    {
        .name = "app_launcher_frontend",
        .x    = 0,
        .y    = 0,
    },
    .active_ms = (uint32_t) - 1,
    .ui_design = app_launcher_frontend_ui_design,
};

void *get_app_launcher_frontend(void)
{
    return &app_launcher_frontend;
}
static void app_xml_ui_design(gui_app_t *app);
static gui_app_t app_xml =
{
    .screen =
    {
        .name = "app_xml",
        .x    = 0,
        .y    = 0,
    },
    /*
    * Default no sleep, no active time limitation.
    * Please set active time by api(gui_set_app_active_time) if power control is needed.
    */
    .active_ms = (uint32_t) - 1,
    .ui_design = app_xml_ui_design,
};

void *get_app_xml(void)
{
    return &app_xml;
}

#ifdef __arm__
#include "romfs.h"
#else
#include <sys/stat.h>
#include <fcntl.h>
#endif

#if defined __WIN32
#include <dirent.h>
#endif
#include "draw_font.h"
gui_grid_t *g;
void button_cb(T_GUI_BUTTON *b)
{
    gui_img_set_opacity((void *)b->img, 100);

}
void button_click_cb(T_GUI_BUTTON *b)
{
    gui_app_t *app = get_app_xml();
    app->xml = b->data;
    gui_switch_app(get_app_launcher_frontend(), app);
}
void button_release_cb(T_GUI_BUTTON *b)
{
    gui_img_set_opacity((void *)b->img, 255);
    button_click_cb(b);
#ifdef RTL87x2G_DASHBOARD
    b->flag = 0;
#endif
}
void searchXmlFiles(char *dirPath, gui_app_t *app)
{
    DIR *dir = 0;
    struct dirent *entry;
    if ((dir = opendir(dirPath)) == NULL)
    {
        perror("opendir() failed"); return;
    }
    while ((entry = readdir(dir)) != NULL)
    {

        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 &&
            strcmp(entry->d_name, "widget.ts") != 0)
        {
            gui_log("debug line%d , entry->d_name = %s\n", __LINE__, entry->d_name);
            char path2[512];
            sprintf(path2, "%s/%s", dirPath, entry->d_name);
            DIR *dirr = 0;
            if ((dirr = opendir(path2)) == NULL)
            {
                perror("opendir() failed"); return;
            }
            struct dirent *entryy;
            while ((entryy = readdir(dirr)) != NULL)
            {
                if (strstr(entryy->d_name, ".xml") != NULL)
                {
                    gui_log("debug line%d , entryy->d_name = %s\n", __LINE__, entryy->d_name);
                    char path[512];
                    sprintf(path, "%s/%s", path2, entryy->d_name);
                    extern void get_app(gui_app_t *app, char **pic, char **text);
                    char *pic = "app/system/resource/icMenuBird.bin"; char *text = "bird";
                    uint32_t temp = (uint32_t)path;
                    app->xml = (char *)temp;


                    get_app(app, &pic, &text);
                    gui_log("get:%s,%s\n", pic, text);
                    void *img1;
                    {
                        img1 = gui_get_file_address(pic);
                    }
                    if (strcmp(text, "launcher") == 0)
                    {
                        continue;
                    }

                    T_GUI_BUTTON *button = gui_button_create(g, 0, 0, 100, 100, img1, img1, text, 0, 0, 0);
                    gui_button_press(button, button_cb, button);
                    gui_button_release(button, button_release_cb, button);

#ifdef RTL87x2G_DASHBOARD
                    gui_obj_event_set((void *)button, GUI_EVENT_TOUCH_RELEASED);
                    button->flag = 1;
                    gui_obj_add_event_cb(button, (gui_event_cb_t)button_release_cb, GUI_EVENT_5, button);
#endif
                    button->data = gui_strdup(path);


                    gui_button_text_move(button, 0, 70);
                    {
                        int font_size = 16;
                        gui_text_set(button->text, text, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX, UINT8_MAX, UINT8_MAX),
                                     strlen(text), font_size);
                        void *addr1 = gui_get_file_address("app/system/resource/font/arialbd_size16_bits4_font.bin");
                        gui_font_mem_init(addr1);
                        gui_text_type_set(button->text, addr1);
                    }
                }

            }
            closedir(dirr);
        }


    }
    closedir(dir);
}
void xml_get_screen(char *dirPath, char *xml_file, int *width, int *hight)
{
    DIR *dir = 0;
    struct dirent *entry;
    if ((dir = opendir(dirPath)) == NULL)
    {
        perror("opendir() failed"); return;
    }
    while ((entry = readdir(dir)) != NULL)
    {
        //printf("dname:%s\n",entry->d_name);
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 &&
            strcmp(entry->d_name, "widget.ts") != 0)
        {
            char path2[512];
            sprintf(path2, "%s/%s", dirPath, entry->d_name);
            DIR *dirr = 0;
            if ((dirr = opendir(path2)) == NULL)
            {
                perror("opendir() failed"); return;
            }
            struct dirent *entryy;
            while ((entryy = readdir(dirr)) != NULL)
            {
                //printf("ddname:%s\n",entryy->d_name);
                if (strstr(entryy->d_name, ".xml") != NULL)
                {
                    char path[512];
                    sprintf(path, "%s/%s", path2, entryy->d_name);
                    extern void get_app(gui_app_t *app, char **pic, char **text);
                    extern void get_app_by_file(char *xml, char *pic, char *text);
                    char *pic = "app/system/resource/icMenuBird.bin"; char *text = "bird";

                    gui_log("debug line%d\n", __LINE__);
                    get_app_by_file(path, (char *)&pic, (char *)&text); gui_log("get:%s,%s\n", pic, text);
                    if (strcmp(text, "launcher") == 0)
                    {
                        extern void get_screen_size(char *xml, int *widgt, int *hight);
                        get_screen_size(path, width, hight);

                        return;
                    }

//                    button->text->path = gui_get_file_address("app/system/resource/font/malgunbd.ttf");
                }

            }
            closedir(dirr);
        }


    }
    closedir(dir);
}
static void app_launcher_frontend_ui_design(gui_app_t *app)
{
    gui_log("app_launcher_frontend_ui_design\n");
    //gui_font_stb_init(gui_get_file_address("app/system/resource/font/malgunbd.ttf"));


    g = gui_grid_create(&app->screen, 100, 100, 3, 4, 100, 100);
    char *apppath = "app";
    char *path = gui_malloc(strlen(apppath) + strlen(GUI_ROOT_FOLDER) + 1);
    sprintf(path, "%s%s", GUI_ROOT_FOLDER, apppath);
    searchXmlFiles(path, app);

}
static void app_xml_ui_design(gui_app_t *app)
{
    extern void create_tree(gui_app_t *app);
    create_tree(app);
#if defined(_WIN32)
    // gui_return_create(&app->screen);
#endif
}
GUI_INIT_APP_EXPORT(gui_server_init);
