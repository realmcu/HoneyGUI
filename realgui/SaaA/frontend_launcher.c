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
#include "gui_api.h"
#include "rvd.h"
#include "gui_switch.h"
#include "gui_combo.h"
#define APP_0
// #define APP_C1
// #define APP_C2
// #define APP_C3
// #define APP_C4
// #define APP_C5




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
extern void *get_app_xml(void);
#include "draw_font.h"
// #define ENABLE_APP_AUTOSTART
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
#ifdef ENABLE_APP_AUTOSTART
                    text = "  ";
#endif
                    gui_button_t *button = gui_button_create(g, 0, 0, 100, 100, img1, img1, text, BUTTON_BG_ICON, 0);
                    gui_button_press(button, (gui_event_cb_t)button_cb, button);
                    gui_button_release(button, (gui_event_cb_t)button_release_cb, button);
#ifdef ENABLE_APP_AUTOSTART
                    gui_img_set_opacity((void *)button->img, 0);
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
#include "rvd.h"
// Static array to store paths of XML files, with size defined by XML_ARRAY_SIZE
static char *rvd_xml_array[XML_ARRAY_SIZE];
int rvd_scan_xml_files()
{

    char *dirPath = (void *)gui_get_path_by_relative("/app"); // Base directory path to scan
    gui_fs_dir *dir = 0; // Directory handle
    struct gui_fs_dirent *entry; // Directory entry
    int index = 0; // Index to track number of XML files found
    rvd_free_xml_array();
    // Clear the XML array to ensure no stale data
    memset(rvd_xml_array, 0, XML_ARRAY_SIZE * sizeof(rvd_xml_array[0]));
    // Open the base directory
    if ((dir = gui_fs_opendir(dirPath)) == NULL)
    {
        gui_log("gui_fs_opendir(%s) failed", dirPath);
        gui_free(dirPath);
        return index; // Exit if directory cannot be opened
    }
    // Iterate through directory entries
    while ((entry = gui_fs_readdir(dir)) != NULL)
    {
        // Skip ".", "..", "widget.ts", and "system" directories
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 &&
            strcmp(entry->d_name, "widget.ts") != 0)
        {
            if (GUI_STRINGS_EQUAL(entry->d_name, "system"))
            {
                continue; // Skip the "system" directory
            }
            // Construct path to subdirectory
            char path2[100];
            sprintf(path2, "%s/%s", dirPath, entry->d_name);
            gui_fs_dir *dirr = 0;
            // Open subdirectory
            if ((dirr = gui_fs_opendir(path2)) == NULL)
            {
                gui_log("gui_fs_opendir(%s) failed", path2);
                gui_free(dirPath);
                return index; // Exit if subdirectory cannot be opened
            }
            struct gui_fs_dirent *entryy;
            // Iterate through subdirectory entries
            while ((entryy = gui_fs_readdir(dirr)) != NULL)
            {
                // Check for files with ".xml" extension
                if (strstr(entryy->d_name, ".xml") != NULL)
                {
                    // Construct full path to XML file
                    char path[100];
                    sprintf(path, "%s/%s", path2, entryy->d_name);
                    // Store path in array if within size limit
                    if (index < XML_ARRAY_SIZE)
                    {
                        gui_log("rvd_xml_array %d:%s\n", index, path);
                        rvd_xml_array[index++] = gui_strdup(path); // Duplicate and store path
                    }
                    else
                    {
                        gui_log("More than XML_ARRAY_SIZE\n"); // Log if array is full
                    }
                }
            }
            gui_fs_closedir(dirr); // Close subdirectory
        }
    }
    gui_fs_closedir(dir); // Close base directory
    gui_free(dirPath);
    return index; // Return number of XML files found
}
char **rvd_get_xml_array()
{
    return rvd_xml_array;
}
void rvd_free_xml_array()
{
    for (size_t i = 0; i < XML_ARRAY_SIZE; i++)
    {
        if (rvd_get_xml_array()[i])
        {
            gui_free(rvd_get_xml_array()[i]);
            rvd_get_xml_array()[i] = 0;
        }
    }
}
int rvd_startup_app(int app_index)
{
    // Scan for XML files to populate rvd_xml_array
    int app_count = rvd_scan_xml_files();
    // Check if no XML files were found
    if (app_count <= 0)
    {
        gui_log("rvd_startup_app failed, no app");
        return RVD_ERROR_NO_APP; // Return error if no apps available
    }
    // Check if app_index is out of range
    if (app_index >= app_count)
    {
        return RVD_ERROR_INDEX_INVALID; // Return error if index is invalid
    }
    // Get application structure for XML-based app
    gui_app_t *app = get_app_xml();
    app->xml = rvd_get_xml_array()[app_index]; // Set app's XML file path
    // Switch to or start the application
    if (gui_current_app())
    {
        gui_app_switch(gui_current_app(), app); // Switch to new app if one is running
    }
    else
    {
        gui_app_startup(app); // Start new app if none is running
    }
    return 0;
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
#if defined _WIN32
#ifdef ENABLE_MONKEY_TEST
#include "test_cmd.h"
void return_home(void *msg)
{
    gui_app_t *app = gui_current_app();
    tabview_main->cur_id.x = 0;
    tabview_main->cur_id.y = 0;
    gui_app_switch(app, get_app_launcher_frontend());
    gui_log("return_home done \n");
}
void cmd_return_home(void)
{
    gui_server_exec_cb(return_home);
}
GUI_EXPORT_CMD(return_home, cmd_return_home);
#endif
#endif

static int button_hover_index;
static char *button_hover_group_name;
static unsigned char main_page_hover_index = 0;
static unsigned char tv_main_page_hover_index = 0;
static void gui_obj_tree_count_by_type_and_group(gui_obj_t *obj, T_OBJ_TYPE type, int *count,
                                                 const char *group_name);
static void get_first_active_radio(gui_obj_t *obj, gui_obj_t **rst);
static int gui_page_tree_get_radio_gap(gui_obj_t *page, gui_obj_t **top_radio_window);
int rvd_get_button_hover_index()
{
    return button_hover_index;
}
void rvd_set_button_hover_index(int index)
{
    button_hover_index = index;
}
gui_app_t *get_current_rvd_app(void)
{
    extern void *get_app_xml(void);
    gui_app_t *app = get_app_xml();
    // Verify if app matches current GUI context
    if (app != gui_current_app())
    {
        return 0;
    }
    else
    {
        return app;
    }
}
static void obj_tree_get_widget_by_type_and_index(gui_obj_t *root,
                                                  T_OBJ_TYPE type, gui_obj_t **output, int index, int *count, int *flag, const char *group_name)
{
    if (!root)
    {
        return;
    }

    if (*flag)
    {
        return;
    }

    gui_list_t *node = NULL;
    gui_list_t *tmp = NULL;
    gui_list_for_each_safe(node, tmp, &root->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        if (obj->magic != GUI_MAGIC_NUMBER)
        {
            gui_log("list NULL @line:%d, @%p", __LINE__, root);
            gui_log("@name:%s, @type:%d\n", root->name, root->type);
            return;
        }
        if (obj->not_show == 1)
        {
            continue;
        }
        if (type == obj->type &&
            GUI_STRINGS_EQUAL(group_name, ((radio_t *)(rvd_get_radio_data(obj)))->radio_name))
        {
            gui_log("%d,%d,%s\n", *count, index, obj->name);
            if (index == *count)
            {
                *output = obj;
                *flag = 1;
                *count = 0;
                return;
            }
            else
            {
                (*count)++;
            }
        }
        if (*flag)
        {
            return;
        }
        else
        {
            obj_tree_get_widget_by_type_and_index(obj, type, output, index, count, flag, group_name);
        }


    }
}
static void radio_get_widget_by_type_and_index(gui_obj_t *root, T_OBJ_TYPE type, gui_obj_t **output,
                                               int index, const char *group_name)
{
    int count = 0;
    int flag = 0;
    obj_tree_get_widget_by_type_and_index(root, type, output, index, &count, &flag, group_name);
}
static int hover_combo_index = 0;
static int hover_combo_index_last = 0;
static bool hover_combo_up;
static bool hover_combo_enter;
static void rvd_hover_combo(bool up, gui_obj_t *root)
{

    int hover_count = 0;
    {
        gui_list_t *node = NULL;
        gui_list_t *tmp = NULL;
        gui_list_for_each_safe(node, tmp, &root->child_list)
        {
            gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
            if (obj->magic != GUI_MAGIC_NUMBER)
            {
                gui_log("list NULL @line:%d, @%p", __LINE__, root);
                gui_log("@name:%s, @type:%d\n", root->name, root->type);
                return;
            }
            if (obj->not_show == 1)
            {
                continue;
            }
            // for (size_t i = 0; i < GUI_ARRAY_SIZE(hover_types); i++)
            {
                if (obj->type == BUTTON)
                {
                    hover_count++;
                }
            }
        }
    }

    if (hover_combo_up != up && hover_combo_enter)
    {
        if (up)
        {
            hover_combo_index++;
        }
        else
        {
            hover_combo_index--;
        }
        if (hover_combo_index >= hover_count)
        {
            hover_combo_index = 0;
        }
        else if (hover_combo_index < 0)
        {
            hover_combo_index = hover_count - 1;
        }
    }
    if (!hover_combo_enter)
    {
        hover_combo_enter = true;
    }

    {
        int current = 0;
        gui_obj_t *hover_obj = 0;
        gui_list_t *node = NULL;
        gui_list_t *tmp = NULL;
        gui_list_for_each_safe(node, tmp, &root->child_list)
        {
            gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
            if (obj->magic != GUI_MAGIC_NUMBER)
            {
                gui_log("list NULL @line:%d, @%p", __LINE__, root);
                gui_log("@name:%s, @type:%d\n", root->name, root->type);
                return;
            }
            if (obj->not_show == 1)
            {
                continue;
            }
            // for (size_t i = 0; i < GUI_ARRAY_SIZE(hover_types); i++)
            {
                if (obj->type == BUTTON)
                {

                    if (current == hover_combo_index)
                    {
                        if (obj->type == BUTTON)
                        {
                            gui_button_t *button = (void *)obj;
                            button->img->data = button->on_pic_addr;

                        }
                        hover_obj = obj;
                        hover_combo_index_last = hover_combo_index;


                        if (!up)
                        {
                            hover_combo_index++;
                        }
                        else
                        {
                            hover_combo_index--;
                        }
                        hover_combo_up = up;
                        if (hover_combo_index >= hover_count)
                        {
                            hover_combo_index = 0;
                        }
                        else if (hover_combo_index < 0)
                        {
                            hover_combo_index = hover_count - 1;
                        }

                        {
                            int count = 0;
                            gui_list_t *node = NULL;
                            gui_list_t *tmp = NULL;
                            gui_list_for_each_safe(node, tmp, &root->child_list)
                            {
                                gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
                                if (obj->magic != GUI_MAGIC_NUMBER)
                                {
                                    gui_log("list NULL @line:%d, @%p", __LINE__, root);
                                    gui_log("@name:%s, @type:%d\n", root->name, root->type);
                                    return;
                                }
                                if (obj->not_show == 1)
                                {
                                    continue;
                                }
                                if (obj != hover_obj)
                                {
                                    if (obj->type == BUTTON)
                                    {
                                        gui_button_t *button_off = (void *)obj;
                                        button_off->img->data = button_off->off_pic_addr;
                                    }


                                }

                            }
                        }
                        return;
                    }
                    current++;
                }
            }
        }
    }

}
static int hover_widgets_index = 0;
static int hover_widgets_index_last = 0;
static bool hover_first;
static T_OBJ_TYPE hover_types[] = {BUTTON, MACRO_COMBO};
static void rvd_hover_widgets(bool up, gui_obj_t *root)
{
    int hover_count = 0;
    {
        gui_list_t *node = NULL;
        gui_list_t *tmp = NULL;
        gui_list_for_each_safe(node, tmp, &root->child_list)
        {
            gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
            if (obj->magic != GUI_MAGIC_NUMBER)
            {
                gui_log("list NULL @line:%d, @%p", __LINE__, root);
                gui_log("@name:%s, @type:%d\n", root->name, root->type);
                return;
            }
            if (obj->not_show == 1)
            {
                continue;
            }
            for (size_t i = 0; i < GUI_ARRAY_SIZE(hover_types); i++)
            {
                if (obj->type == hover_types[i])
                {
                    hover_count++;
                }
            }
        }
    }


    {
        int current = 0;
        gui_obj_t *hover_obj = 0;
        gui_list_t *node = NULL;
        gui_list_t *tmp = NULL;
        gui_list_for_each_safe(node, tmp, &root->child_list)
        {
            gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
            if (obj->magic != GUI_MAGIC_NUMBER)
            {
                gui_log("list NULL @line:%d, @%p", __LINE__, root);
                gui_log("@name:%s, @type:%d\n", root->name, root->type);
                return;
            }
            if (obj->not_show == 1)
            {
                continue;
            }
            for (size_t i = 0; i < GUI_ARRAY_SIZE(hover_types); i++)
            {
                if (obj->type == hover_types[i])
                {
                    if (current == hover_widgets_index)
                    {
                        /* hover */
                        if (obj->type == MACRO_COMBO)
                        {
                            gui_combo_t *combo = (void *)obj;
                            if (combo->fold == 0)
                            {
                                gui_log("fold\n");

                                rvd_hover_combo(up, (void *)combo->window_select);


                                return;
                            }
                            else
                            {
                                gui_log("fold 1\n");
                                combo->button_selector->switch_picture->data = combo->button_selector->on_pic_addr;
                            }

                            //combo->button_selector->switch_picture->data = combo->button_selector->on_pic_addr;

                        }
                        else if (obj->type == BUTTON)
                        {
                            gui_button_t *button = (void *)obj;
                            button->img->data = button->on_pic_addr;

                        }
                        hover_obj = obj;
                        hover_widgets_index_last = hover_widgets_index;
                        if (!up)
                        {
                            hover_widgets_index++;
                        }
                        else
                        {
                            hover_widgets_index--;
                        }


                        if (hover_widgets_index >= hover_count)
                        {
                            hover_widgets_index = 0;
                        }
                        else if (hover_widgets_index < 0)
                        {
                            hover_widgets_index = hover_count - 1;
                        }

                        {
                            int count = 0;
                            gui_list_t *node = NULL;
                            gui_list_t *tmp = NULL;
                            gui_list_for_each_safe(node, tmp, &root->child_list)
                            {
                                gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
                                if (obj->magic != GUI_MAGIC_NUMBER)
                                {
                                    gui_log("list NULL @line:%d, @%p", __LINE__, root);
                                    gui_log("@name:%s, @type:%d\n", root->name, root->type);
                                    return;
                                }
                                if (obj->not_show == 1)
                                {
                                    continue;
                                }
                                if (obj != hover_obj)
                                {
                                    if (obj->type == BUTTON)
                                    {
                                        gui_button_t *button_off = (void *)obj;
                                        button_off->img->data = button_off->off_pic_addr;
                                    }
                                    else if (obj->type == MACRO_COMBO)
                                    {
                                        gui_combo_t *combo = (void *)obj;
                                        combo->button_selector->switch_picture->data = combo->button_selector->off_pic_addr;

                                    }

                                }

                            }
                        }
                        return;
                    }
                    current++;
                }
            }
        }
    }

}
static void rvd_clear_buttons(gui_obj_t *root)
{
    {

        gui_list_t *node = NULL;
        gui_list_t *tmp = NULL;
        gui_list_for_each_safe(node, tmp, &root->child_list)
        {
            gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
            if (obj->magic != GUI_MAGIC_NUMBER)
            {
                gui_log("list NULL @line:%d, @%p", __LINE__, root);
                gui_log("@name:%s, @type:%d\n", root->name, root->type);
                return;
            }
            // if (obj->not_show == 1)
            // {
            //     continue;
            // }
            //for (size_t i = 0; i < GUI_ARRAY_SIZE(hover_types); i++)
            {
                if (obj->type == BUTTON)
                {

                    {

                        {
                            gui_button_t *button = (void *)obj;
                            button->img->data = button->off_pic_addr;

                        }
                    }

                }
            }
            rvd_clear_buttons(obj);
        }

    }
}
static void hover_widgets(bool up, const char **widget_name_array, int widget_count)
{
    if (up)
    {
        hover_widgets_index++;
    }
    else
    {
        hover_widgets_index--;
    }
    if (hover_widgets_index >= widget_count)
    {
        hover_widgets_index = 0;
    }
    if (hover_widgets_index < 0)
    {
        hover_widgets_index = widget_count - 1;
    }

    {
        //for (size_t i = 0; i < widget_count; i++)

        GUI_WIDGET_POINTER_BY_NAME(obj, widget_name_array[hover_widgets_index]);



        if (obj->type == MACRO_COMBO)
        {
            gui_combo_t *combo = (void *)obj;
            combo->button_selector->switch_picture->data = combo->button_selector->on_pic_addr;
        }
        else if (obj->type == BUTTON)
        {
            gui_button_t *button = (void *)obj;
            button->img->data = button->on_pic_addr;
        }


        for (size_t i = 0; i < widget_count; i++)
        {
            if (i != hover_widgets_index)
            {
                GUI_WIDGET_POINTER_BY_NAME(obj, widget_name_array[i]);
                if (obj->type == MACRO_COMBO)
                {
                    gui_combo_t *combo = (void *)obj;
                    combo->button_selector->switch_picture->data = combo->button_selector->off_pic_addr;
                }
                else if (obj->type == BUTTON)
                {
                    gui_button_t *button = (void *)obj;
                    button->img->data = button->off_pic_addr;
                }
            }


        }


    }

}
static int get_id_in_page(gui_page_t *page)
{
    if (page->align_hight > 0)
    {
        return (-GUI_BASE(page)->y + page->top_offset) / page->align_hight;
    }
    return 0;

}
static void click_widgets(const char **widget_name_array)
{
    GUI_WIDGET_POINTER_BY_NAME(obj, widget_name_array[hover_widgets_index_last]);
    if (obj->type == MACRO_COMBO)
    {
        gui_combo_t *combo = (void *)obj;
        bool enter = 0;
        int enter_count = 0;
        {
            gui_obj_t *root = (void *)combo->window_select;
            gui_list_t *node = NULL;
            gui_list_t *tmp = NULL;
            gui_list_for_each_safe(node, tmp, &root->child_list)
            {
                gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
                if (obj->magic != GUI_MAGIC_NUMBER)
                {
                    gui_log("list NULL @line:%d, @%p", __LINE__, root);
                    gui_log("@name:%s, @type:%d\n", root->name, root->type);
                    return;
                }
                if (obj->not_show == 1)
                {
                    continue;
                }
                //for (size_t i = 0; i < GUI_ARRAY_SIZE(hover_types); i++)
                {
                    if (obj->type == BUTTON)
                    {
                        enter_count++;
                        gui_button_t *button_off = (void *)obj;
                        if (button_off->img->data != button_off->off_pic_addr)
                        {
                            enter = 1;
                        }
                    }
                }
            }
        }
        if (enter == 0 || combo->fold == 1)
        {
            GUI_API(gui_switch_t).turn_on(combo->button_selector);
            gui_log("click combo\n");
            hover_widgets_index = hover_widgets_index_last;
            hover_combo_index = 0;
            hover_combo_enter = false;
            //rvd_clear_buttons(combo);
        }
        else
        {
            gui_log("click combo button\n");
            {
                int count = 0;
                gui_obj_t *root = (void *)combo->window_select;
                gui_list_t *node = NULL;
                gui_list_t *tmp = NULL;
                gui_list_for_each_safe(node, tmp, &root->child_list)
                {
                    gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
                    if (obj->magic != GUI_MAGIC_NUMBER)
                    {
                        gui_log("list NULL @line:%d, @%p", __LINE__, root);
                        gui_log("@name:%s, @type:%d\n", root->name, root->type);
                        return;
                    }
                    if (obj->not_show == 1)
                    {
                        continue;
                    }
                    //for (size_t i = 0; i < GUI_ARRAY_SIZE(hover_types); i++)
                    {
                        if (obj->type == BUTTON)
                        {
                            if (count == hover_combo_index_last)
                            {
                                GUI_TYPE(gui_button_t, obj)->override_click = 1;
                                return;
                            }

                            count++;


                        }
                    }
                }
            }
        }



    }
    else if (obj->type == BUTTON)
    {
        gui_button_t *button = (void *)obj;
        gui_obj_enable_event(obj, GUI_EVENT_TOUCH_CLICKED);
    }
}
int rvd_click_button()
{
#if defined(APP_0) || defined(APP_C2)|| defined(APP_C4)
    gui_log("rvd_click_button%d\n", button_hover_index);
    gui_win_t *win = 0;
    gui_app_t *app = get_current_rvd_app();
    if (!app)
    {
        return GUI_ERROR_NULL;
    }
    // Get root screen object
    gui_obj_t *screen = &(app->screen);
    gui_obj_t *rst = 0;
    get_first_active_radio(screen, &rst);
    if (rst == 0)
    {
        /* code */
        return GUI_ERROR_NULL;
    }
    radio_get_widget_by_type_and_index((gui_obj_t *)get_current_rvd_app(), RADIO, (void *)&win,
                                       button_hover_index, button_hover_group_name);
    if (!win)
    {
        return GUI_ERROR_NULL;
    }
    radio_t *param = rvd_get_radio_data((void *)win);
    //param->img->data = (void *)param->pic;
    param->click = 1;
    return GUI_SUCCESS;
#endif
#if defined(APP_C3)
    GUI_WIDGET_POINTER_BY_NAME(obj, "tabview0")

    gui_tabview_t *tabview = (void *)obj;
    if (tabview->cur_id.x == 1 && tabview->cur_id.y == 0)
    {
        GUI_WIDGET_POINTER_BY_NAME(obj, "page0")
        int id = get_id_in_page((void *)obj);
        gui_log("rvd_click_button%d\n", id);
        gui_win_t *win = 0;
        gui_obj_tree_get_widget_by_type_and_index(obj, WINDOW, (void *)&win, id);
        if (!win)
        {
            return GUI_ERROR_NULL;
        }
        win->override_click = 1;
        return GUI_SUCCESS;

    }
    else if (tabview->cur_id.x == 0 && tabview->cur_id.y == 0)
    {
        const char *widget_name_array[] = {"combobox0", "button0"};
        click_widgets(widget_name_array);
    }

#endif
}

int rvd_click_button_down()
{
#if defined(APP_0) || defined(APP_C2)
    // Log the index of the hovered button for debugging
    gui_log("rvd_click_button%d\n", button_hover_index);

    // Initialize pointer for the window (radio button widget)
    gui_win_t *win = 0;

    // Get the current application instance
    gui_app_t *app = get_current_rvd_app();
    if (!app)
    {
        // Return error if no application instance is found
        return GUI_ERROR_NULL;
    }

    // Get the root screen object from the application
    gui_obj_t *screen = &(app->screen);

    // Initialize pointer to store the first active radio button
    gui_obj_t *rst = 0;
    // Retrieve the first active radio button from the screen
    get_first_active_radio(screen, &rst);
    if (rst == 0)
    {
        // Return error if no active radio button is found
        return GUI_ERROR_NULL;
    }

    // Retrieve the radio button widget by type and index, using the hover group name
    radio_get_widget_by_type_and_index((gui_obj_t *)get_current_rvd_app(), RADIO, (void *)&win,
                                       button_hover_index, button_hover_group_name);
    if (!win)
    {
        // Return error if the radio button widget is not found
        return GUI_ERROR_NULL;
    }

    // Get the radio button's associated data
    radio_t *param = rvd_get_radio_data((void *)win);

    // Set the press flag to indicate the button is pressed
    param->press = 1;

    // Return success to indicate the operation completed
    return GUI_SUCCESS;
#endif

}
int rvd_hover_button(int index, int button_count)
{
    if (button_count <= 0)
    {
        return GUI_ERROR;
    }
    if (0 > index)
    {
        index = button_count - 1;
    }
    if (button_count <= index)
    {
        index = 0;
    }
    gui_log("rvd_hover_button%d\n", index);
    gui_win_t *win = 0;
    radio_get_widget_by_type_and_index((gui_obj_t *)get_current_rvd_app(), RADIO, (void *)&win,
                                       button_hover_index, button_hover_group_name);
    if (!win)
    {
        return GUI_ERROR_NULL;
    }

    radio_t *param = rvd_get_radio_data((void *)win);
    //param->img->data = (void *)param->pic;
#if defined(APP_C4)
    param->hover = 1;

    if (GUI_STRINGS_EQUAL(param->radio_name, "RadioButtonGroup4"))
    {
        main_page_hover_index = index;
    }
    else if (GUI_STRINGS_EQUAL(param->radio_name, "RadioButton"))
    {
        tv_main_page_hover_index = index;
    }
#else
    extern void rvd_win_radio_hover(gui_obj_t *obj);
    rvd_win_radio_hover((void *)win);
#endif
    return GUI_SUCCESS;
}
int rvd_hover_button_root(int index, int button_count, gui_obj_t *root)
{
    if (button_count <= 0)
    {
        return GUI_ERROR;
    }
    if (0 > index)
    {
        index = button_count - 1;
    }
    if (button_count <= index)
    {
        index = 0;
    }
    gui_log("rvd_hover_button%d\n", index);
    gui_win_t *win = 0;
    radio_get_widget_by_type_and_index(root, RADIO, (void *)&win,
                                       button_hover_index, button_hover_group_name);
    if (!win)
    {
        return GUI_ERROR_NULL;
    }

    radio_t *param = rvd_get_radio_data((void *)win);
    //param->img->data = (void *)param->pic;
    //param->hover = 1;
    extern void rvd_win_radio_hover(gui_obj_t *obj);
    rvd_win_radio_hover((void *)win);
    // if (GUI_STRINGS_EQUAL(param->radio_name, "RadioButtonGroup4"))
    // {
    //     main_page_hover_index = index;
    // }
    // else if (GUI_STRINGS_EQUAL(param->radio_name, "RadioButton"))
    // {
    //     tv_main_page_hover_index = index;
    // }

    return GUI_SUCCESS;
}
static int rvd_hover_down_default()
{
    int button_count = rvd_get_button_count();
    if (button_count <= 0)
    {
        return GUI_ERROR;
    }
    button_hover_index++;
    if (button_count <= button_hover_index)
    {
        button_hover_index = 0;
#if defined(APP_C4)
        GUI_WIDGET_POINTER_BY_NAME(obj, "tabview0")
        if (obj)
        {
            gui_tabview_t *tabview = (void *)obj;
            if (tabview->cur_id.x == 0 && tabview->cur_id.y == 0)
            {
                gui_tabview_jump_tab(tabview, 0, 1);
            }

        }
#endif
    }
    gui_log("rvd_hover_down_button%d\n", button_hover_index);
    return rvd_hover_button(button_hover_index, button_count);
}
static int rvd_hover_down_default_noloop()
{
    int button_count = rvd_get_button_count();
    if (button_count <= 0)
    {
        return GUI_ERROR;
    }
    button_hover_index++;
    if (button_count <= button_hover_index)
    {
        button_hover_index = button_count - 1;
        // GUI_WIDGET_POINTER_BY_NAME(obj, "tabview0")
        // if (obj)
        // {
        //     gui_tabview_t *tabview = (void *)obj;
        //     if (tabview->cur_id.x == 0 && tabview->cur_id.y == 0)
        //     {
        //         gui_tabview_jump_tab(tabview, 0, 1);
        //     }

        // }
    }
    gui_log("rvd_hover_down_button%d\n", button_hover_index);
    return rvd_hover_button(button_hover_index, button_count);
}
static int rvd_hover_down_root(gui_obj_t *root)
{
    int button_count = rvd_get_button_count_root(root);
    if (button_count <= 0)
    {
        return GUI_ERROR;
    }
    button_hover_index++;
    if (button_count <= button_hover_index)
    {
        button_hover_index = 0;
        // GUI_WIDGET_POINTER_BY_NAME(obj, "tabview0")
        // if (obj)
        // {
        //     gui_tabview_t *tabview = (void *)obj;
        //     if (tabview->cur_id.x == 0 && tabview->cur_id.y == 0)
        //     {
        //         gui_tabview_jump_tab(tabview, 0, 1);
        //     }

        // }
    }
    gui_log("rvd_hover_down_button%d\n", button_hover_index);
    return rvd_hover_button_root(button_hover_index, button_count, root);
}
static int rvd_hover_page(gui_page_t *page)
{
    gui_obj_t *radio_win = 0;
    int gap = gui_page_tree_get_radio_gap((void *)page, &radio_win);
    gui_log("rvd_hover_page%d\n", gap);
    int radio_h = radio_win->h;
    int screen_h = page->default_h;
    int active_radio_count = (int)((screen_h - radio_win->x + gap - 1) / gap);
    page->gesture_flag = 1;
    int count_out_of_page = rvd_get_button_count() - rvd_get_button_count_root((void *)page);
    if (active_radio_count <= button_hover_index - count_out_of_page)
    {
        gui_page_set_offset(page, -(button_hover_index - count_out_of_page - active_radio_count + 1)*gap);
    }
    else
    {
        gui_page_set_offset(page, 0);
    }
    return 0;
}

#define INVALID_INDEX UINT8_MAX
#if defined(APP_C4)
static unsigned char  digit_tab_grid[4][3] = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {INVALID_INDEX, 9, INVALID_INDEX}};
static unsigned char  play_tab_grid[3][3] = {{0, 1, 2}, {3, 4, 4}, {INVALID_INDEX, INVALID_INDEX, INVALID_INDEX}};

#endif
static int rvd_hover_down_grid(unsigned char grid_index[][3], unsigned char grid_row,
                               unsigned char grid_col)
{
    int button_count = rvd_get_button_count();
    if (button_count <= 0)
    {
        return GUI_ERROR;
    }

    //search the hover index in grid
    int grid_x = 0, grid_y = 0;
    for (int i = 0; i < grid_row; i++)
    {
        for (int j = 0; j < grid_col; j++)
        {
            if (grid_index[i][j] == button_hover_index)
            {
                grid_x = i;
                grid_y = j;
                break;
            }
        }
    }
    for (size_t i = 0; i < grid_row; i++)
    {
        grid_x++;
        if (grid_row <= grid_x)
        {
            grid_x = 0;
        }
        int button_hover_index_temp = grid_index[grid_x][grid_y];
        if (button_hover_index_temp != INVALID_INDEX)
        {
            button_hover_index = button_hover_index_temp;
            break;
        }

    }
    gui_log("rvd_hover_down_grid_button%d\n", button_hover_index);
    return rvd_hover_button(button_hover_index, button_count);
}
static int rvd_hover_up_grid(unsigned char grid_index[][3], unsigned char grid_row,
                             unsigned char grid_col)
{
    int button_count = rvd_get_button_count();
    if (button_count <= 0)
    {
        return GUI_ERROR;
    }

    //search the hover index in grid
    int grid_x = 0, grid_y = 0;
    for (int i = 0; i < grid_row; i++)
    {
        for (int j = 0; j < grid_col; j++)
        {
            if (grid_index[i][j] == button_hover_index)
            {
                grid_x = i;
                grid_y = j;
                break;
            }
        }
    }

    int button_hover_index_temp  = INVALID_INDEX;
    for (size_t i = 0; i < grid_row; i++)
    {
        grid_x--;
        if (grid_row <= grid_x)
        {
            grid_x = 0;
        }
        else if (grid_x < 0)
        {
            grid_x = grid_row - 1;
        }

        int button_hover_index_temp = grid_index[grid_x][grid_y];
        if (button_hover_index_temp != INVALID_INDEX)
        {
            button_hover_index = button_hover_index_temp;
            break;
        }

    }
    gui_log("rvd_hover_down_grid_button%d\n", button_hover_index);
    return rvd_hover_button(button_hover_index, button_count);
}

/**
 * @brief Retrieves the current tab from the specified tabview.
 *
 * This function iterates through the child list of the given tabview object to find a tab
 * that matches the current tab ID (both x and y coordinates). If a matching tab is found,
 * it is returned. If the tabview is NULL or no matching tab is found, the function returns NULL.
 *
 * @param tabview Pointer to the tabview from which to retrieve the current tab.
 * @return Pointer to the current gui_tab_t if found, otherwise NULL.
 */

static gui_tab_t *gui_tabview_get_current_tab(gui_tabview_t *tabview)
{
    if (tabview == NULL)
    {
        return NULL;
    }
    gui_list_t *node = NULL;
    gui_list_t *tmp = NULL;
    gui_obj_t *root = (void *)tabview;
    gui_list_for_each_safe(node, tmp, &root->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        if (obj->magic != GUI_MAGIC_NUMBER)
        {
            gui_log("list NULL @line:%d, @%p", __LINE__, root);
            gui_log("@name:%s, @type:%d\n", root->name, root->type);
            return NULL;
        }
        if (obj->type == TAB)
        {
            gui_tab_t *tab = GUI_TYPE(gui_tab_t, obj);
            if (tab->id.x == tabview->cur_id.x && tab->id.y == tabview->cur_id.y)
            {
                return tab;
            }
        }
    }
    gui_log("gui_tabview_get_current_tab fail\n");
    return NULL;
}
static int rvd_hover_down_page()
{
    GUI_WIDGET_POINTER_BY_NAME(obj, "tabview0");
    gui_tabview_t *tabview = (void *)obj;
    gui_tab_t *cur_tab = gui_tabview_get_current_tab(tabview);
    GUI_WIDGET_POINTER_BY_TYPE(page_obj, PAGE, (void *)cur_tab);
    //{ if (cur_tab == ((void *)0)) { gui_log("%s, %d, obj is NULL\n", __FUNCTION__, 15578); } else if (((gui_obj_t *)cur_tab)->magic != 0x0b) { gui_log("%s, %d, obj magic is invalid %p\n", __FUNCTION__, 15578, (void *)cur_tab); } } gui_obj_t *page_obj = 0; extern void gui_obj_tree_get_widget_by_type(gui_obj_t *, T_OBJ_TYPE , gui_obj_t **); gui_obj_tree_get_widget_by_type(cur_tab, PAGE, &page_obj); { if (page_obj == ((void *)0)) { gui_log("%s, %d, obj is NULL\n", __FUNCTION__, 15578); } else if (((gui_obj_t *)page_obj)->magic != 0x0b) { gui_log("%s, %d, obj magic is invalid %p\n", __FUNCTION__, 15578, (void *)page_obj); } }
    if (page_obj)
    {
        GUI_TYPE(gui_page_t, page_obj)->wheel_pressed = 1;
        GUI_TYPE(gui_page_t, page_obj)->wheel_up = 0;
        return GUI_SUCCESS;
    }
    return GUI_ERROR_NULL;
}
static int rvd_hover_up_page()
{
    GUI_WIDGET_POINTER_BY_NAME(obj, "tabview0");
    gui_tabview_t *tabview = (void *)obj;
    gui_tab_t *cur_tab = gui_tabview_get_current_tab(tabview);
    GUI_WIDGET_POINTER_BY_TYPE(page_obj, PAGE, (void *)cur_tab);
    //{ if (cur_tab == ((void *)0)) { gui_log("%s, %d, obj is NULL\n", __FUNCTION__, 15578); } else if (((gui_obj_t *)cur_tab)->magic != 0x0b) { gui_log("%s, %d, obj magic is invalid %p\n", __FUNCTION__, 15578, (void *)cur_tab); } } gui_obj_t *page_obj = 0; extern void gui_obj_tree_get_widget_by_type(gui_obj_t *, T_OBJ_TYPE , gui_obj_t **); gui_obj_tree_get_widget_by_type(cur_tab, PAGE, &page_obj); { if (page_obj == ((void *)0)) { gui_log("%s, %d, obj is NULL\n", __FUNCTION__, 15578); } else if (((gui_obj_t *)page_obj)->magic != 0x0b) { gui_log("%s, %d, obj magic is invalid %p\n", __FUNCTION__, 15578, (void *)page_obj); } }
    if (page_obj)
    {
        GUI_TYPE(gui_page_t, page_obj)->wheel_pressed = 1;
        GUI_TYPE(gui_page_t, page_obj)->wheel_up = 1;
        return GUI_SUCCESS;
    }
    return GUI_ERROR_NULL;
}

int rvd_hover_down_button()
{
#if defined(APP_C2)
    GUI_WIDGET_POINTER_BY_NAME(obj, "tabview0");
    gui_tabview_t *tabview = (void *)obj;
    gui_tab_t *cur_tab = gui_tabview_get_current_tab(tabview);
    GUI_WIDGET_POINTER_BY_TYPE(page_obj, PAGE, (void *)cur_tab);
    if (page_obj)
    {
        int rst = rvd_hover_down_default_noloop();



        bool in_page = 0;
        {
            gui_obj_t *win = 0;
            radio_get_widget_by_type_and_index((gui_obj_t *)get_current_rvd_app(), RADIO, (void *)&win,
                                               button_hover_index, button_hover_group_name);
            if (!win)
            {
                return GUI_ERROR_NULL;
            }
            while (win->parent)
            {
                win = win->parent;
                if (win->type == PAGE)
                {
                    in_page = 1;
                }

            }

        }
        if (in_page)
        {
            return rvd_hover_page((void *)page_obj);
        }
        else
        {
            return rst;
        }


    }
    else
    {
        return rvd_hover_down_default();
    }
#endif
#if defined(APP_C4)
    GUI_WIDGET_POINTER_BY_NAME(obj, "tabview0");
    gui_tabview_t *tabview = (void *)obj;

    if (tabview->cur_id.x == 1 && tabview->cur_id.y == 0)
    {
        return rvd_hover_down_grid(digit_tab_grid, 4, 3);
    }
    else if (tabview->cur_id.x == 4 && tabview->cur_id.y == 0)
    {
        return rvd_hover_down_grid(play_tab_grid, 3, 3);
    }
    else
    {
        return rvd_hover_down_default();
    }
#endif

#ifdef APP_0
    return rvd_hover_down_default();
#endif
#ifdef APP_C5
    return rvd_hover_down_page();
#endif
#ifdef APP_C3

    GUI_WIDGET_POINTER_BY_NAME(obj, "tabview0");
    gui_tabview_t *tabview = (void *)obj;
    if (tabview->cur_id.x == 1 && tabview->cur_id.y == 0)
    {
        return rvd_hover_down_page();
    }
    else if (tabview->cur_id.x == 0 && tabview->cur_id.y == 0)
    {
        // const char *widget_name_array[]={"combobox0", "button0"};
        // hover_widgets(1, widget_name_array, 2);
        gui_tab_t *cur_tab = gui_tabview_get_current_tab(tabview);
        rvd_hover_widgets(0, (void *)cur_tab);
    }
#endif
}

int rvd_hover_button_by_index(int index)
{
    int button_count = rvd_get_button_count();
    if (button_count <= 0)
    {
        return GUI_ERROR;
    }
    button_hover_index = index;
    if (button_count <= button_hover_index)
    {
        button_hover_index = 0;
    }
    gui_log("rvd_hover_button%d\n", button_hover_index);
    return rvd_hover_button(button_hover_index, button_count);
}
int rvd_hover_button_by_range_ahead(int start, int end)
{
    int button_count = rvd_get_button_count();
    if (button_count <= 0)
    {
        return GUI_ERROR;
    }
    if (start < 0 || end < 0 || start > end || end > button_count || start > button_count)
    {
        return GUI_ERROR;
    }
    button_hover_index++;
    if (button_hover_index < start)
    {
        button_hover_index = end;
    }
    if (button_hover_index > end)
    {
        button_hover_index = start;
    }




    gui_log("rvd_hover_button%d\n", button_hover_index);
    int ret = rvd_hover_button(button_hover_index, button_count);

    return ret;
}
int rvd_hover_button_by_range_back(int start, int end)
{
    int button_count = rvd_get_button_count();
    if (button_count <= 0)
    {
        return GUI_ERROR;
    }
    if (start < 0 || end < 0 || start > end || end > button_count || start > button_count)
    {
        return GUI_ERROR;
    }
    button_hover_index--;
    if (button_hover_index < start)
    {
        button_hover_index = end;
    }
    if (button_hover_index > end)
    {
        button_hover_index = start;
    }




    gui_log("rvd_hover_button%d\n", button_hover_index);
    int ret = rvd_hover_button(button_hover_index, button_count);

    return ret;
}
//add default hover up func

static int rvd_hover_up_default()
{
    int button_count = rvd_get_button_count();
    if (button_count <= 0)
    {
        return GUI_ERROR;
    }
    button_hover_index--;
    if (0 > button_hover_index)
    {
        button_hover_index = button_count - 1;
#if defined(APP_C4)
        GUI_WIDGET_POINTER_BY_NAME(obj, "tabview0")
        if (obj)
        {
            gui_tabview_t *tabview = (void *)obj;
            if (tabview->cur_id.x == 0 && tabview->cur_id.y == 1)
            {
                gui_tabview_jump_tab(tabview, 0, 0);
            }
        }
#endif
    }
    gui_log("rvd_hover_up_button%d\n", button_hover_index);
    return rvd_hover_button(button_hover_index, button_count);
}
static int rvd_hover_up_default_noloop()
{
    int button_count = rvd_get_button_count();
    if (button_count <= 0)
    {
        return GUI_ERROR;
    }
    button_hover_index--;
    if (0 > button_hover_index)
    {
        button_hover_index = 0;
        // GUI_WIDGET_POINTER_BY_NAME(obj, "tabview0")
        // if (obj)
        // {
        //     gui_tabview_t *tabview = (void *)obj;
        //     if (tabview->cur_id.x == 0 && tabview->cur_id.y == 1)
        //     {
        //         gui_tabview_jump_tab(tabview, 0, 0);
        //     }

        // }
    }
    gui_log("rvd_hover_up_button%d\n", button_hover_index);
    return rvd_hover_button(button_hover_index, button_count);
}
int rvd_hover_up_button()
{
#ifdef APP_C4
    GUI_WIDGET_POINTER_BY_NAME(obj, "tabview0");
    gui_tabview_t *tabview = (void *)obj;
    if (tabview->cur_id.x == 1 && tabview->cur_id.y == 0)
    {
        return rvd_hover_up_grid(digit_tab_grid, 4, 3);
    }
    else if (tabview->cur_id.x == 4 && tabview->cur_id.y == 0)
    {
        return rvd_hover_up_grid(play_tab_grid, 3, 3);
    }
    else
    {
        return rvd_hover_up_default();
    }
#endif
#ifdef APP_C2
    GUI_WIDGET_POINTER_BY_NAME(obj, "tabview0");
    gui_tabview_t *tabview = (void *)obj;
    gui_tab_t *cur_tab = gui_tabview_get_current_tab(tabview);
    //GUI_WIDGET_POINTER_BY_TYPE_LOG(page_obj, PAGE, cur_tab);
    { if (cur_tab == ((void *)0)) { gui_log("%s, %d, obj is NULL\n", __FUNCTION__, 15578); } else if (((gui_obj_t *)cur_tab)->magic != 0x0b) { gui_log("%s, %d, obj magic is invalid %p\n", __FUNCTION__, 15578, (void *)cur_tab); } } gui_obj_t
    *page_obj = 0; extern void gui_obj_tree_get_widget_by_type(gui_obj_t *, T_OBJ_TYPE, gui_obj_t **);
    gui_obj_tree_get_widget_by_type((void *)cur_tab, PAGE, &page_obj); { if (page_obj == ((void *)0)) { gui_log("%s, %d, obj is NULL\n", __FUNCTION__, 15578); } else if (((gui_obj_t *)page_obj)->magic != 0x0b) { gui_log("%s, %d, obj magic is invalid %p\n", __FUNCTION__, 15578, (void *)page_obj); } }
    if (page_obj)
    {
        rvd_hover_up_default_noloop();
        return rvd_hover_page((void *)page_obj);
    }
    {
        return rvd_hover_up_default();
    }
#endif
#ifdef APP_0
    return rvd_hover_up_default();
#endif
#ifdef APP_C5
    return rvd_hover_up_page();
#endif
#ifdef APP_C3
    GUI_WIDGET_POINTER_BY_NAME(obj, "tabview0");
    gui_tabview_t *tabview = (void *)obj;
    if (tabview->cur_id.x == 1 && tabview->cur_id.y == 0)
    {
        return rvd_hover_up_page();
    }
    else if (tabview->cur_id.x == 0 && tabview->cur_id.y == 0)
    {
        // const char *widget_name_array[]={"combobox0", "button0"};
        // hover_widgets(1, widget_name_array, 2);
        gui_tab_t *cur_tab = gui_tabview_get_current_tab(tabview);
        rvd_hover_widgets(1, (void *)cur_tab);
    }
#endif
}
int rvd_get_main_page_hover_index()
{
    return main_page_hover_index;
}
int rvd_get_tv_main_page_hover_index()
{
    return tv_main_page_hover_index;
}
static
void rvd_click_back_sync()
{
#ifdef APP_C4
    GUI_WIDGET_POINTER_BY_NAME(obj, "tabview0")
    if (obj)
    {
        gui_tabview_t *tabview = (void *)obj;
        if (!(tabview->cur_id.x == 2 && tabview->cur_id.y == 0))
        {
            gui_tabview_jump_tab(tabview, 2, 0);
            gui_log("rvd_click_back%d, %d\n", tabview->last_id.x, tabview->last_id.y);
            rvd_hover_button_by_index(main_page_hover_index);
        }

    }
#endif
#ifdef APP_0
    GUI_WIDGET_POINTER_BY_TYPE_SOFT(obj, TABVIEW, &(gui_current_app()->screen));
    gui_tabview_back((void *)obj, 0, 0);
#endif
}

void rvd_click_back()
{
    //  gui_msg_t msg =
    // {
    //     .event = GUI_EVENT_USER_DEFINE,
    //     .payload = 0,
    //     .cb = rvd_click_back_sync,
    // };
    // // Send message to GUI server
    // extern bool gui_send_msg_to_server(gui_msg_t *msg);
    // gui_send_msg_to_server(&msg);
    rvd_click_back_sync();

}

void rvd_hover_right()
{
#if defined(APP_C4)
    GUI_WIDGET_POINTER_BY_NAME(obj, "tabview0");
    gui_tabview_t *tabview = (void *)obj;

    if (tabview->cur_id.x == 1 && tabview->cur_id.y == 0)
    {
        int index = rvd_get_button_hover_index();
        gui_log("rvd_click_right%d\n", index);
        if (index < 0)
        {
            return;
        }
        if (index >= 0 && index <= 2)
        {
            rvd_hover_button_by_range_ahead(0, 2);
        }
        else if (index >= 3 && index <= 5)
        {
            rvd_hover_button_by_range_ahead(3, 5);
        }
        else if (index >= 6 && index <= 8)
        {
            rvd_hover_button_by_range_ahead(6, 8);
        }
        else if (index >= 9 && index <= 9)
        {
            rvd_hover_button_by_range_ahead(9, 9);
        }

    }
    else if (tabview->cur_id.x == 4 && tabview->cur_id.y == 0)
    {
        int index = rvd_get_button_hover_index();
        gui_log("rvd_click_right%d\n", index);
        if (index < 0)
        {
            return;
        }
        if (index >= 0 && index <= 2)
        {
            rvd_hover_button_by_range_ahead(0, 2);
        }
        else if (index >= 3 && index <= 4)
        {
            rvd_hover_button_by_range_ahead(3, 4);
        }



    }
#endif
}
void rvd_hover_left()
{
#if defined(APP_C4)
    GUI_WIDGET_POINTER_BY_NAME(obj, "tabview0");
    gui_tabview_t *tabview = (void *)obj;

    if (tabview->cur_id.x == 1 && tabview->cur_id.y == 0)
    {
        int index = rvd_get_button_hover_index();
        gui_log("rvd_click_right%d\n", index);
        if (index < 0)
        {
            return;
        }
        if (index >= 0 && index <= 2)
        {
            rvd_hover_button_by_range_back(0, 2);
        }
        else if (index >= 3 && index <= 5)
        {
            rvd_hover_button_by_range_back(3, 5);
        }
        else if (index >= 6 && index <= 8)
        {
            rvd_hover_button_by_range_back(6, 8);
        }
        else if (index >= 9 && index <= 10)
        {
            rvd_hover_button_by_range_back(9, 10);
        }

    }
    else if (tabview->cur_id.x == 4 && tabview->cur_id.y == 0)
    {
        int index = rvd_get_button_hover_index();
        gui_log("rvd_click_right%d\n", index);
        if (index < 0)
        {
            return;
        }
        if (index >= 0 && index <= 2)
        {
            rvd_hover_button_by_range_back(0, 2);
        }
        else if (index >= 3 && index <= 4)
        {
            rvd_hover_button_by_range_back(3, 4);
        }
        else if (index >= 6 && index <= 6)
        {
            rvd_hover_button_by_range_back(6, 6);
        }


    }
#endif
}
static int tab_index = 0;
static void tabview_jump_by_id(gui_obj_t *tabview, int id)
{
    int count = 0;
    gui_list_t *node = NULL;
    gui_list_t *tmp = NULL;
    gui_obj_t *object = tabview;
    gui_list_for_each_safe(node, tmp, &object->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        if (obj->magic != GUI_MAGIC_NUMBER)
        {
            gui_log("list NULL @line:%d, @%p", __LINE__, object);
            gui_log("@name:%s, @type:%d\n", object->name, object->type);
            return;
        }
        if (obj->type == TAB)
        {
            if (count == id)
            {
                gui_log("%djump %d %d\n", id, GUI_TYPE(gui_tab_t, obj)->id.x, GUI_TYPE(gui_tab_t, obj)->id.y);
                gui_tabview_jump_tab((void *)tabview, GUI_TYPE(gui_tab_t, obj)->id.x, GUI_TYPE(gui_tab_t,
                                                                                               obj)->id.y);
                return;
            }
            else
            {
                (count)++;
            }

        }
    }
}

void rvd_tab_next(void)
{
    GUI_WIDGET_POINTER_BY_TYPE_SOFT(tabview, TABVIEW, (void *)((gui_current_app())));
    if (tabview == NULL)
    {
        return;
    }

    int tab_count = 0;
    gui_obj_tree_count_by_type(tabview, TAB, &tab_count);
    if (tab_count <= 0)
    {
        return;
    }
    tab_index++;
    if (tab_index >= tab_count)
    {
        tab_index = 0;
    }
    if (tab_index < 0)
    {
        tab_index = tab_count - 1;
    }

    gui_log("rvd_tab_next%d\n", tab_index);

    tabview_jump_by_id(tabview, tab_index);
}
void rvd_tab_last(void)
{
    GUI_WIDGET_POINTER_BY_TYPE_SOFT(tabview, TABVIEW, (void *)((gui_current_app())));
    if (tabview == NULL)
    {
        return;
    }

    int tab_count = 0;
    gui_obj_tree_count_by_type(tabview, TAB, &tab_count);
    if (tab_count <= 0)
    {
        return;
    }
    tab_index--;
    if (tab_index >= tab_count)
    {
        tab_index = 0;
    }
    if (tab_index < 0)
    {
        tab_index = tab_count - 1;
    }

    gui_log("rvd_tab_last%d\n", tab_index);

    tabview_jump_by_id(tabview, tab_index);
}
static void gui_obj_tree_count_by_type_and_group(gui_obj_t *obj, T_OBJ_TYPE type, int *count,
                                                 const char *group_name)
{
    gui_list_t *node = NULL;
    gui_list_t *tmp = NULL;
    gui_obj_t *object = obj;
    gui_list_for_each_safe(node, tmp, &obj->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        if (obj->magic != GUI_MAGIC_NUMBER)
        {
            gui_log("list NULL @line:%d, @%p", __LINE__, object);
            gui_log("@name:%s, @type:%d\n", object->name, object->type);
            return;
        }
        if (obj->not_show == 1)
        {
            continue;
        }

        if (obj->type == type &&
            GUI_STRINGS_EQUAL(group_name, ((radio_t *)(rvd_get_radio_data(obj)))->radio_name))
        {
            (*count)++;
        }

        gui_obj_tree_count_by_type_and_group(obj, type, count, group_name);
    }
}

static int gui_page_tree_get_radio_gap(gui_obj_t *page, gui_obj_t **top_radio_window)
{
    gui_list_t *node = NULL;
    gui_list_t *tmp = NULL;
    gui_obj_t *object = page;
    gui_obj_t *obj = page;
    int gap = 0;
    int i = 0;
    gui_list_for_each_safe(node, tmp, &obj->child_list)
    {

        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        if (obj->magic != GUI_MAGIC_NUMBER)
        {
            gui_log("list NULL @line:%d, @%p", __LINE__, object);
            gui_log("@name:%s, @type:%d\n", object->name, object->type);
            return gap;
        }
        if (obj->not_show == 1)
        {
            continue;
        }

        if (obj->type == RADIO /*&&
            GUI_STRINGS_EQUAL(group_name, ((radio_t *)(rvd_get_radio_data(obj)))->radio_name)*/)
        {
            if (i < 2)
            {
                gap = obj->y - gap;
                if (i == 0)
                {
                    *top_radio_window = obj;
                }
            }
            else { return gap; }

        }
        i++;
    }
    return gap;
}

static void get_first_active_radio(gui_obj_t *obj, gui_obj_t **rst)
{
    gui_list_t *node = NULL;
    gui_list_t *tmp = NULL;
    gui_obj_t *object = obj;
    gui_list_for_each_safe(node, tmp, &obj->child_list)
    {
        gui_obj_t *child = gui_list_entry(node, gui_obj_t, brother_list);
        if (child->magic != GUI_MAGIC_NUMBER || child->not_show == 1)
        {
            continue;
        }
        if (child->type == RADIO)
        {
            *rst = child;
            button_hover_group_name = ((radio_t *)(rvd_get_radio_data(child)))->radio_name;
            return;
        }
        get_first_active_radio(child, rst);
    }
}

int rvd_get_button_count()
{
    int button_count = 0;
    // Get current application handle
    extern void *get_app_xml(void);
    gui_app_t *app = get_app_xml();
    // Verify if app matches current GUI context
    if (app != gui_current_app())
    {
        return 0;
    }
    // Get root screen object
    gui_obj_t *screen = &(app->screen);
    gui_obj_t *rst = 0;
    get_first_active_radio(screen, &rst);
    if (rst == 0)
    {
        /* code */
        return 0;
    }

    gui_obj_tree_count_by_type_and_group(screen, RADIO, &button_count,
                                         ((radio_t *)(rvd_get_radio_data(rst)))->radio_name);
    gui_log("rvd_get_button_count%d\n", button_count);
    return button_count;
}


int rvd_get_button_count_root(gui_obj_t *root)
{
    int button_count = 0;

    if (!root)
    {
        return 0;
    }
    // Get root screen object
    gui_obj_t *rst = 0;
    get_first_active_radio(root, &rst);
    if (rst == 0)
    {
        /* code */
        return 0;
    }

    gui_obj_tree_count_by_type_and_group(root, RADIO, &button_count,
                                         ((radio_t *)(rvd_get_radio_data(rst)))->radio_name);
    gui_log("rvd_get_button_count%d\n", button_count);
    return button_count;
}
static void set_texts_opacity_in_window(gui_obj_t *window)
{
    gui_list_t *node = NULL;
    gui_list_t *tmp = NULL;
    gui_obj_t *object = window;
    gui_list_for_each_safe(node, tmp, &object->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        if (obj->type == TEXTBOX)
        {
            gui_text_t *textbox = (void *)obj;
            textbox->scale_img->opacity_value = window->opacity_value;
        }
    }
}

static void set_imgs_opacity_in_window(gui_obj_t *window, int opacity_value)
{
    gui_list_t *node = NULL;
    gui_list_t *tmp = NULL;
    gui_obj_t *object = window;
    gui_list_for_each_safe(node, tmp, &object->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        if (obj->type == IMAGE_FROM_MEM)
        {
            gui_img_t *img = (void *)obj;
            img->opacity_value = opacity_value;
        }
        set_imgs_opacity_in_window(obj, opacity_value);
    }
}
static void set_text_convert_to_img_in_root(gui_obj_t *root)
{
    gui_list_t *node = NULL;
    gui_list_t *tmp = NULL;
    gui_obj_t *object = root;
    gui_list_for_each_safe(node, tmp, &object->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        if (obj->type == TEXTBOX)
        {
            gui_text_convert_to_img((void *)obj, ARGB8565);
        }
        set_text_convert_to_img_in_root(obj);
    }
}
static int gui_page_tree_get_win_gap(gui_obj_t *page,
                                     gui_obj_t **top_radio_window)
{
    gui_list_t *node = NULL;
    gui_list_t *tmp = NULL;
    gui_obj_t *object = page;
    gui_obj_t *obj = page;
    int gap = 0;
    int i = 0;
    gui_list_for_each_safe(node, tmp, &obj->child_list)
    {

        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        if (obj->magic != GUI_MAGIC_NUMBER)
        {
            gui_log("list NULL @line:%d, @%p", __LINE__, object);
            gui_log("@name:%s, @type:%d\n", object->name, object->type);
            return gap;
        }
        if (obj->not_show == 1)
        {
            continue;
        }

        if (obj->type == WINDOW /*&&
            GUI_STRINGS_EQUAL(group_name, ((radio_t *)( rvd_get_radio_data(obj)))->radio_name)*/)
        {
            if (i < 2)
            {
                gap = obj->y - gap;
                if (i == 0)
                {
                    *top_radio_window = obj;
                }
            }
            else { return gap; }

        }
        i++;
    }
    return gap;
}
#ifdef APP_C3

#define PAGE_SCALE (0.2f)
#define PAGE_MOVE_H (120.0f)
#define PAGE_MOVE_V (75.0f)
#define PAGE_SCALE_MIN (0.3F)
static void deal_win_in_page(gui_page_t *page)
{
    gui_list_t *node = NULL;
    gui_obj_t *object = (void *)page;
    int window_index = 0;
    extern int gui_page_tree_get_win_gap(gui_obj_t *page,
                                         gui_obj_t **top_radio_window);
    gui_obj_t *top_window = NULL;
    static int gap = 0;
    if (gap == 0)
    {
        gap = gui_page_tree_get_win_gap((void *)page, &top_window);
    }


    gui_list_for_each(node, &object->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        if (obj->type == WINDOW)
        {
            int ay = window_index * gap;
            window_index++;
            if (ay + page->base.y >= (float)(0 - obj->h) &&
                ay + page->base.y < (float)gui_get_screen_height())
            {
                float scale = (1 - PAGE_SCALE * _UI_ABS(((float)(ay + page->base.y + (float)(obj->h / 2)) -
                                                         (float)(75)) / (float)(75)));
                if (scale > 0.9f) { scale = 0.9f; }
                if (scale < PAGE_SCALE_MIN)
                {
                    gui_obj_hidden(obj, 1);
                    continue;
                }
                else
                {
                    gui_obj_hidden(obj, 0);
                }

                GUI_TYPE(gui_win_t, obj)->scale = scale;
                GUI_TYPE(gui_win_t, obj)->scale_y = scale;
                obj->opacity_value = 255 * scale;
                //set_texts_opacity_in_window(obj);
                set_imgs_opacity_in_window(obj, obj->opacity_value);
                int obj_y = 0;
                if (((float)(ay + page->base.y + (float)(obj->h / 2)) -
                     (float)(75)) > 0)
                {
                    obj_y = ay - (int)(PAGE_MOVE_V * (1 - scale) * (1 - scale));
                }
                else
                {
                    obj_y = ay + (int)(PAGE_MOVE_V * (1 - scale) * (1 - scale));
                }
                //gui_log("ay:%d, obj_y:%d,gap:%d\n", ay, obj_y,gap);
                gui_obj_move(obj, obj->x, obj_y);
                //obj->not_show = false;
            }
            else
            {
                //obj->not_show = true;
            }
        }
    }
}
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(page_callback)
{
    deal_win_in_page(this_widget);
}
static GUI_EVENT_CALLBACK_FUNCTION_DEFINE(wincb)
{
    gui_log("wincb:%s", GUI_BASE(obj)->name);
}
static void C3_init(void)
{
    GUI_WIDGET_POINTER_BY_NAME(page0, "page0");// cppcheck-suppress unknownMacro
    set_text_convert_to_img_in_root((void *)page0);
    gui_page_set_animate((gui_page_t *)page0, 1000, -1, page_callback, page0);
    GUI_TYPE(gui_page_t, page0)->top_offset = 50;
    GUI_TYPE(gui_page_t, page0)->bottom_offset = -150;
    gui_page_center_alignment((void *)page0, 50);
    {
        GUI_WIDGET_POINTER_BY_NAME(win, "win0")// cppcheck-suppress unknownMacro
        gui_win_click((gui_win_t *)win, wincb, 0);
    }
    {
        GUI_WIDGET_POINTER_BY_NAME(win, "win1")// cppcheck-suppress unknownMacro
        gui_win_click((gui_win_t *)win, wincb, 0);
    }
    {
        GUI_WIDGET_POINTER_BY_NAME(win, "win2")// cppcheck-suppress unknownMacro
        gui_win_click((gui_win_t *)win, wincb, 0);
    }
    {
        GUI_WIDGET_POINTER_BY_NAME(win, "win3")// cppcheck-suppress unknownMacro
        gui_win_click((gui_win_t *)win, wincb, 0);
    }
    {
        GUI_WIDGET_POINTER_BY_NAME(win, "win4")// cppcheck-suppress unknownMacro
        gui_win_click((gui_win_t *)win, wincb, 0);
    }
    {
        GUI_WIDGET_POINTER_BY_NAME(win, "win5")// cppcheck-suppress unknownMacro
        gui_win_click((gui_win_t *)win, wincb, 0);
    }
}

#endif
int rvd_hover_button_sync(int index, int button_count)
{
    if (button_count <= 0)
    {
        return GUI_ERROR;
    }
    if (0 > index)
    {
        index = button_count - 1;
    }
    if (button_count <= index)
    {
        index = 0;
    }
    gui_log("rvd_hover_button_sync######%d\n", index);
    gui_win_t *win = 0;
    radio_get_widget_by_type_and_index((gui_obj_t *)get_current_rvd_app(), RADIO, (void *)&win,
                                       button_hover_index, button_hover_group_name);
    if (!win)
    {
        return GUI_ERROR_NULL;
    }

    radio_t *param = rvd_get_radio_data((gui_obj_t *)win);
    //param->img->data = (void *)param->pic;
    param->hover = 1;
    extern void rvd_win_radio_hover(gui_obj_t *obj);
    rvd_win_radio_hover((void *)win);
    if (GUI_STRINGS_EQUAL(param->radio_name, "RadioButtonGroup4"))
    {
        main_page_hover_index = index;
    }
    else if (GUI_STRINGS_EQUAL(param->radio_name, "RadioButton"))
    {
        tv_main_page_hover_index = index;
    }

    return GUI_SUCCESS;
}
int rvd_hover_button_by_index_sync(int index)
{
    int button_count = rvd_get_button_count();
    if (button_count <= 0)
    {
        return GUI_ERROR;
    }
    button_hover_index = index;
    if (button_count <= button_hover_index)
    {
        button_hover_index = 0;
    }
    gui_log("rvd_hover_button%d\n", button_hover_index);
    return rvd_hover_button_sync(button_hover_index, button_count);
}
#ifdef APP_C4

static GUI_EVENT_CALLBACK_FUNCTION_DEFINE(on_click_jump_cb_tabview)
{


    GUI_WIDGET_POINTER_BY_NAME(obj_tabview, "tabview0");
    gui_tabview_t *tabview = (void *)obj_tabview;
//gui_log("$$$$$$$$on_click_jump_cb_tabview %d %d\n" ,tabview->cur_id.x, tabview->cur_id.y);
    if (tabview->cur_id.x == 1 && tabview->cur_id.y == 0)
    {
        rvd_hover_button_by_index_sync(4);
    }
    else if (tabview->cur_id.x == 4 && tabview->cur_id.y == 0)
    {
        rvd_hover_button_by_index_sync(1);
    }
    else if (tabview->cur_id.x == 3 && tabview->cur_id.y == 0)
    {
        rvd_hover_button_by_index_sync(0);
    }
    else if (tabview->cur_id.x == 0 && tabview->cur_id.y == 1)
    {
        rvd_hover_button_by_index_sync(0);
    }
    else if ((tabview->cur_id.x == 0 && tabview->cur_id.y == 0) && (tabview->last_id.x == 2 &&
                                                                    tabview->last_id.y == 0))
    {
        rvd_hover_button_by_index_sync(0);
    }
    else if ((tabview->cur_id.x == 0 && tabview->cur_id.y == 0) && (tabview->last_id.x == 0 &&
                                                                    tabview->last_id.y == 1))
    {
        rvd_hover_button_by_index_sync(2);
    }
    else if (tabview->cur_id.x == 2 && tabview->cur_id.y == 0)
    {
        rvd_hover_button_by_index(rvd_get_main_page_hover_index());
    }
    else
    {
        rvd_hover_button_by_index(0);
    }
}
static void C4_init(void)
{
    gui_update_screen_force(true);
    GUI_WIDGET_POINTER_BY_NAME(tabview, "tabview0");
    gui_tabview_jump_tab((void *)tabview, 2, 0);
    gui_tabview_tab_change((void *)tabview, on_click_jump_cb_tabview, NULL);
}
#endif
#ifdef APP_C2
#include <math.h> // Required for cosf function used in easing calculations

// Forward declaration of animation callback for radio button events
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(win_radio_cb);
// Forward declaration of function to override radio button behavior
static void rvd_override_radio(gui_obj_t *root);
// Event callback for seekbar completion
static GUI_EVENT_CALLBACK_FUNCTION_DEFINE(on_click_jump_cb_tabview)
{


    GUI_WIDGET_POINTER_BY_NAME(obj_tabview, "tabview0");// cppcheck-suppress unknownMacro
    gui_tabview_t *tabview = (void *)obj_tabview;
    gui_log("$$$$$$$$on_click_jump_cb_tabview %d %d\n", tabview->cur_id.x, tabview->cur_id.y);
    if (tabview->cur_id.x == 11 && tabview->cur_id.y == 0)
    {
        rvd_hover_button_by_index_sync(1);
    }
    else
    {
        rvd_hover_button_by_index(0);
    }
}
static GUI_EVENT_CALLBACK_FUNCTION_DEFINE(callback)
{
    gui_log("seekbar complete."); // Log message when seekbar animation completes
}

// Initialization function for C2 application
static void C2_init(void)
{
    // Get pointer to "imageseekbar0" widget and add event callback for GUI_EVENT_4

    {
        GUI_WIDGET_POINTER_BY_NAME(obj, "imageseekbar0")// cppcheck-suppress unknownMacro
        gui_obj_add_event_cb(obj, (gui_event_cb_t)callback, GUI_EVENT_4, 0);
    }

    // Get pointer to "tabview0" widget and disable touch input

    {
        GUI_WIDGET_POINTER_BY_NAME(obj, "tabview0")// cppcheck-suppress unknownMacro
        gui_tabview_tp_disable((gui_tabview_t *)obj, 1);
        gui_tabview_tab_change((void *)obj, on_click_jump_cb_tabview, NULL);
    }

    // Get pointer to "page0" widget, convert text to image, and override radio behavior
    {
        GUI_WIDGET_POINTER_BY_NAME(obj, "page0");// cppcheck-suppress unknownMacro
        set_text_convert_to_img_in_root((void *)obj);
        rvd_override_radio((void *)obj);
    }
    {
        gui_log("C2_init\n");
        GUI_WIDGET_POINTER_BY_NAME(obj, "imgDevIcon1");// cppcheck-suppress unknownMacro
        gui_log("imgdevicon1 w:%d, h:%d\n", obj->w, obj->h);
    }
}
#define WIN_RADIO_SCALE_MIN (0.97f) // Minimum scale for radio button press animation
// Animation callback for radio button press (shrink phase)
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(win_radio_press_highlight_ani_down)
{
    // On the first frame, create a mask image if it doesn't exist
    if (animate->Beginning_frame)
    {
        gui_obj_t *pointer = 0;
        gui_obj_tree_get_widget_by_name(this_widget, "_rvd_radio_mask", &pointer);
        if (pointer == 0)
        {
            // Get "rect0" widget and create a mask image with SRC_OVER mode
            GUI_WIDGET_POINTER_BY_NAME(rect0, "rect0")
            gui_img_set_mode(gui_img_create_from_mem(this_widget, "_rvd_radio_mask",
                                                     GUI_TYPE(gui_img_t, rect0)->data, 0, 0, 0, 0),
                             IMG_SRC_OVER_MODE);
        }
    }

    // Calculate scale using unified cosine easing
    float percent = animate->progress_percent;
    float min_scale = WIN_RADIO_SCALE_MIN; // Adjustable minimum scale (default: 0.97)
    float scale;
    float delta = 1.0f - min_scale; // Calculate scale change amount

    // Unified easing: scale from 1.0 to min_scale using cosine function
    float t = percent;
    scale = 1.0f - delta * (1.0f - cosf(3.14159f * t / 2.0f));
    gui_log("scale:%f\n", scale);
    // Apply scale to both X and Y axes
    GUI_TYPE(gui_win_t, this_widget)->scale = scale;
    GUI_TYPE(gui_win_t, this_widget)->scale_y = scale;
}

// Animation callback for radio button release (expand phase)
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(win_radio_press_highlight_ani_up)
{
    // Get current scale as the minimum scale for the release animation
    float percent = animate->progress_percent;
    float min_scale = GUI_TYPE(gui_win_t, this_widget)->scale;
    if (min_scale == 0.0f)
    {
        min_scale = WIN_RADIO_SCALE_MIN; // Fallback to default if scale is zero
    }

    float scale;
    float delta = 1.0f - min_scale; // Calculate scale change amount

    // Unified easing: scale from min_scale to 1.0 using cosine function
    float t = percent;
    scale = min_scale + delta * (1.0f - cosf(3.14159f * t / 2.0f));

    // Apply scale to both X and Y axes
    GUI_TYPE(gui_win_t, this_widget)->scale = scale;
    GUI_TYPE(gui_win_t, this_widget)->scale_y = scale;

    // On the last frame, remove the mask image
    if (animate->end_frame)
    {
        gui_obj_t *pointer = 0;
        gui_obj_tree_get_widget_by_name(this_widget, "_rvd_radio_mask", &pointer);
        if (pointer)
        {
            gui_obj_tree_free(pointer); // Free the mask image
        }
    }
}

// Animation callback for handling radio button events
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(win_radio_cb)
{
    gui_log("win_radio_cb\n");
    radio_t *param = p; // Get radio button parameters
    if (param->check)
    {
        // Enable GUI_EVENT_6 when check event occurs
        gui_obj_enable_event(this_widget, GUI_EVENT_6);
        param->check = 0;
    }
    else if (param->hover)
    {
        // Enable hover event for radio button
        gui_obj_enable_event(this_widget, GUI_EVENT_WIN_RADIO_HOVER);
        param->hover = 0;
    }
    else if (param->click)
    {
        gui_log("param->click\n");
        // Trigger release animation (expand) on click
        GUI_TYPE(gui_win_t, this_widget)->animate_array[1]->callback = win_radio_press_highlight_ani_up;
        gui_obj_restart_animate(GUI_TYPE(gui_win_t, this_widget)->animate_array[1]);
        gui_obj_enable_event(this_widget, GUI_EVENT_TOUCH_PRESSED);
        param->click = 0;
    }
    else if (param->press)
    {
        gui_log("param->press\n");
        // Trigger press animation (shrink) on press
        gui_log("param->press\n");
        GUI_TYPE(gui_win_t, this_widget)->animate_array[1]->callback = win_radio_press_highlight_ani_down;
        gui_obj_restart_animate(GUI_TYPE(gui_win_t, this_widget)->animate_array[1]);
        param->press = 0;
    }
    else
    {
        // Disable events if no specific action is detected
        gui_obj_enable_event(this_widget, GUI_EVENT_INVALIDE);
    }
}

// Function to override radio button behavior in the widget tree
static void rvd_override_radio(gui_obj_t *root)
{
    gui_list_t *node = NULL;
    gui_list_t *tmp = NULL;
    // Iterate through the child list of the root widget
    gui_list_for_each_safe(node, tmp, &root->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        // Validate widget magic number
        if (obj->magic != GUI_MAGIC_NUMBER)
        {
            gui_log("list NULL @line:%d, @%p", __LINE__, root);
            gui_log("@name:%s, @type:%d\n", root->name, root->type);
            return;
        }

        // If the widget is a radio button, set its animation callback
        if (obj->type == RADIO)
        {
            GUI_TYPE(gui_win_t, obj)->animate_array[0]->callback = win_radio_cb;
        }

        // Recursively apply to child widgets
        rvd_override_radio(obj);
    }
}
#endif
#ifdef APP_C5
#define PAGE_SCALE (0.7f)
#define PAGE_MOVE_H (120.0f)
#define PAGE_MOVE_V (120.0f)
#define PAGE_SCALE_MIN (0.3F)
static void deal_win_in_page(gui_page_t *page)
{
    gui_list_t *node = NULL;
    gui_obj_t *object = (void *)page;
    int window_index = 0;
    extern int gui_page_tree_get_win_gap(gui_obj_t *page,
                                         gui_obj_t **top_radio_window);
    gui_obj_t *top_window = NULL;
    static int gap = 0;
    if (gap == 0)
    {
        gap = gui_page_tree_get_win_gap((void *)page, &top_window);
    }


    gui_list_for_each(node, &object->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        if (obj->type == WINDOW)
        {
            int ay = window_index * gap;
            window_index++;
            if (ay + page->base.y >= (float)(0 - obj->h) &&
                ay + page->base.y < (float)gui_get_screen_height())
            {
                float scale = (1 - PAGE_SCALE * _UI_ABS(((float)(ay + page->base.y + (float)(obj->h / 2)) -
                                                         (float)(page->default_h / 2)) / (float)(page->default_h / 2)));
                if (scale < PAGE_SCALE_MIN)
                {
                    gui_obj_hidden(obj, 1);
                    continue;
                }
                else
                {
                    gui_obj_hidden(obj, 0);
                }

                GUI_TYPE(gui_win_t, obj)->scale = scale;
                GUI_TYPE(gui_win_t, obj)->scale_y = scale;
                obj->opacity_value = 255 * scale;
                //set_texts_opacity_in_window(obj);
                set_imgs_opacity_in_window(obj, obj->opacity_value);
                int obj_y = 0;
                if (((float)(ay + page->base.y + (float)(obj->h / 2)) -
                     (float)(page->default_h / 2)) > 0)
                {
                    obj_y = ay - (int)(PAGE_MOVE_V * (1 - scale) * (1 - scale));
                }
                else
                {
                    obj_y = ay + (int)(PAGE_MOVE_V * (1 - scale) * (1 - scale));
                }
                //gui_log("ay:%d, obj_y:%d,gap:%d\n", ay, obj_y,gap);
                gui_obj_move(obj, (1 - scale)*PAGE_MOVE_H, obj_y);
                //obj->not_show = false;
            }
            else
            {
                //obj->not_show = true;
            }
        }
    }
}
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(page_callback)
{
    deal_win_in_page(this_widget);
}
static void C5_init(void)
{
    GUI_WIDGET_POINTER_BY_NAME(page0, "page0");
    set_text_convert_to_img_in_root((void *)page0);
    gui_page_set_animate((void *)page0, 1000, -1, page_callback, page0);
    GUI_TYPE(gui_page_t, page0)->top_offset = page0->h / 2 - 34;
    GUI_TYPE(gui_page_t, page0)->bottom_offset = -(page0->h / 2 - 34);
    gui_page_center_alignment((void *)page0, page0->h / 5);


}

#endif
int rvd_tab_first(void)
{
#ifdef APP_C2
    GUI_WIDGET_POINTER_BY_NAME(obj, "tabview0");
    gui_tabview_t *tabview = (void *)obj;

    if (tabview->cur_id.x == 11 && tabview->cur_id.y == 0)
    {
        gui_tab_t *cur_tab = gui_tabview_get_current_tab(tabview);
        GUI_WIDGET_POINTER_BY_TYPE(page_obj, PAGE, (void *)cur_tab);
        rvd_hover_button_by_index(1);

        return rvd_hover_page((void *)page_obj);
    }
    else if (tabview->cur_id.x == 12 && tabview->cur_id.y == 0)
    {
        gui_tab_t *cur_tab = gui_tabview_get_current_tab(tabview);
        GUI_WIDGET_POINTER_BY_TYPE(page_obj, PAGE, (void *)cur_tab);
        rvd_hover_button_by_index(0);

        return rvd_hover_page((void *)page_obj);
    }
    else
    {
        return rvd_hover_button_by_index(0);
    }
#endif
    return GUI_ERROR;
}

void rvd_custom_init(void)
{
#ifdef APP_C3
    C3_init();
#endif
#ifdef APP_C2
    C2_init();
#endif
#ifdef APP_C5
    C5_init();
#endif
#ifdef APP_C4
    C4_init();
#endif
}
void close_gui_task(void)
{
    gui_app_t *app = get_app_xml();
    app->active_ms = 0;
}
extern bool gui_send_msg_to_server(gui_msg_t *msg);
void restart_gui_task(void)
{
    gui_app_t *app = get_app_xml();
    app->active_ms = 0xFFFFFFFF;
    gui_msg_t msg;
    msg.event = GUI_EVENT_DISPLAY_ON;
    gui_send_msg_to_server(&msg);
}