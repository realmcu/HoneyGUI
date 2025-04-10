#include <gui_tabview.h>
#include <gui_img.h>
#include <gui_win.h>
#include <gui_text.h>
#include <gui_obj.h>
#include <gui_curtain.h>
#include <gui_button.h>

#include <string.h>
#include <time.h>
#include <stdio.h>
#include "gui_server.h"
#include <gui_app.h>

#include "gui_grid.h"

#include "gui_components_init.h"
#include "gui_api.h"


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

#if defined ENABLE_RTK_GUI_SCRIPT_AS_A_APP
static int app_init(void)
{
    gui_server_init();
    gui_app_startup(&app_launcher_frontend);
    return 0;
}

GUI_INIT_APP_EXPORT(app_init);
#endif



extern void *get_app_xml(void);


#include "draw_font.h"
gui_grid_t *g;
void button_cb(gui_button_t *b)
{
    gui_img_set_opacity((void *)b->img, 100);

}
void button_click_cb(gui_button_t *b)
{
    gui_app_t *app = get_app_xml();
    app->xml = b->data;
    gui_app_switch(get_app_launcher_frontend(), app);
}
void button_release_cb(gui_button_t *b)
{
    gui_img_set_opacity((void *)b->img, 255);
    button_click_cb(b);
#ifdef ENABLE_APP_AUTOSTART
    b->flag = 0;
#endif
}
void searchXmlFiles(char *dirPath, gui_app_t *app)
{
    gui_fs_dir *dir = 0;
    struct gui_fs_dirent *entry;
    if ((dir = gui_fs_opendir(dirPath)) == NULL)
    {
        gui_log("gui_fs_opendir(%s) failed", dirPath); return;
    }
    while ((entry = gui_fs_readdir(dir)) != NULL)
    {

        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 &&
            strcmp(entry->d_name, "widget.ts") != 0)
        {
            gui_log("debug line%d , entry->d_name = %s\n", __LINE__, entry->d_name);
            char path2[512];
            sprintf(path2, "%s/%s", dirPath, entry->d_name);
            gui_fs_dir *dirr = 0;
            if ((dirr = gui_fs_opendir(path2)) == NULL)
            {
                gui_log("gui_fs_opendir(%s) failed", path2); return;
            }
            struct gui_fs_dirent *entryy;
            while ((entryy = gui_fs_readdir(dirr)) != NULL)
            {
                if (strstr(entryy->d_name, ".xml") != NULL)
                {
                    gui_log("debug line%d , entryy->d_name = %s\n", __LINE__, entryy->d_name);
                    char path[512];
                    sprintf(path, "%s/%s", path2, entryy->d_name);
                    extern void get_app(gui_app_t *app, char **pic, char **text);
                    char *pic = 0;
                    char *text = "app";
                    app->xml = path;


                    get_app(app, &pic, &text);
                    gui_log("get:%s,%s\n", pic, text);
                    void *img1;
                    extern const uint8_t *gui_dom_get_image_file_address(const char *image_file_path);
                    img1 = (void *)gui_dom_get_image_file_address(pic);
                    if (strcmp(text, "launcher") == 0)
                    {
                        continue;
                    }

                    gui_button_t *button = gui_button_create(g, 0, 0, 100, 100, img1, img1, text, BUTTON_BG_ICON, 0);
                    gui_button_press(button, (gui_event_cb_t)button_cb, button);
                    gui_button_release(button, (gui_event_cb_t)button_release_cb, button);

#ifdef ENABLE_APP_AUTOSTART
                    gui_obj_enable_event((void *)button, GUI_EVENT_TOUCH_RELEASED);
                    button->flag = 1;
                    gui_obj_add_event_cb(button, (gui_event_cb_t)button_release_cb, GUI_EVENT_5, button);
#endif
                    button->data = gui_strdup(path);


                    gui_button_text_move(button, -70, 73);
                    {
                        int font_size = 16;
                        gui_text_set(button->text, text, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX, UINT8_MAX, UINT8_MAX),
                                     strlen(text), font_size);
                        void *addr1 = gui_get_file_address("app/system/resource/font/arialbd_size16_bits4_font.bin");
                        //gui_font_mem_init(addr1);
                        gui_text_type_set(button->text, addr1, FONT_SRC_MEMADDR);
                        gui_text_mode_set(button->text, CENTER);
                        GUI_BASE(button->text)->w = 70 * 3;
                    }
                }

            }
            gui_fs_closedir(dirr);
        }


    }
    gui_fs_closedir(dir);
}
void xml_get_screen(char *dirPath, char *xml_file, int *width, int *hight)
{
    extern void gui_port_fs_init(void);
    gui_port_fs_init();
    gui_fs_dir *dir = 0;
    struct gui_fs_dirent *entry;
    if ((dir = gui_fs_opendir(dirPath)) == NULL)
    {
        perror("gui_fs_opendir() failed"); return;
    }
    while ((entry = gui_fs_readdir(dir)) != NULL)
    {
        //printf("dname:%s\n",entry->d_name);
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 &&
            strcmp(entry->d_name, "widget.ts") != 0)
        {
            char path2[512];
            sprintf(path2, "%s/%s", dirPath, entry->d_name);
            gui_fs_dir *dirr = 0;
            if ((dirr = gui_fs_opendir(path2)) == NULL)
            {
                perror("gui_fs_opendir() failed"); return;
            }
            struct gui_fs_dirent *entryy;
            while ((entryy = gui_fs_readdir(dirr)) != NULL)
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
            gui_fs_closedir(dirr);
        }


    }
    gui_fs_closedir(dir);
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
    gui_app_append(app);
    gui_fb_change();
}

