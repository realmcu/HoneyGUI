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
            GUI_STRINGS_EQUAL(group_name, ((radio_t *)(GUI_TYPE(gui_win_t, obj)->animate->p))->radio_name))
        {
            if (index == *count)
            {
                *output = obj;
                *flag = 1;
                return;
            }
            (*count)++;
            //gui_log("%d,%d\n", *count, index);
        }

        obj_tree_get_widget_by_type_and_index(obj, type, output, index, count, flag, group_name);
    }
}
static void radio_get_widget_by_type_and_index(gui_obj_t *root, T_OBJ_TYPE type, gui_obj_t **output,
                                               int index, const char *group_name)
{
    int count = 0;
    int flag = 0;
    obj_tree_get_widget_by_type_and_index(root, type, output, index, &count, &flag, group_name);
}
int rvd_click_button()
{
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
    radio_t *param = win->animate->p;
    //param->img->data = (void *)param->pic;
    param->click = 1;
    return GUI_SUCCESS;
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

    radio_t *param = win->animate->p;
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

static int rvd_hover_page(gui_page_t *page)
{
    gui_obj_t *radio_win = 0;
    int gap = gui_page_tree_get_radio_gap((void *)page, &radio_win);
    gui_log("rvd_hover_page%d\n", gap);
    int radio_h = radio_win->h;
    int screen_h = gui_get_screen_height();
    int active_radio_count = (screen_h - radio_win->x) / gap;
    page->gesture_flag = 1;
    if (active_radio_count <= button_hover_index)
    {
        gui_page_set_offset(page, -(button_hover_index - active_radio_count + 1)*gap);
    }
    else
    {
        gui_page_set_offset(page, 0);
    }
    return 0;
}

#define INVALID_INDEX UINT8_MAX
//static unsigned char  digit_tab_grid[4][3] = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {INVALID_INDEX, 9, 10}};
//static unsigned char  play_tab_grid[3][3] = {{0, 1, 2}, {3, 4, INVALID_INDEX}, {5, 5, 5}};
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

int rvd_hover_down_button()
{
    // GUI_WIDGET_POINTER_BY_NAME(obj, "tabview0");
    //  gui_tabview_t *tabview = (void *)obj;
    // gui_tab_t *cur_tab = gui_tabview_get_current_tab(tabview);
    // //GUI_WIDGET_POINTER_BY_TYPE_LOG(page_obj, PAGE, cur_tab);
    // { if (cur_tab == ((void *)0)) { gui_log("%s, %d, obj is NULL\n", __FUNCTION__, 15578); } else if (((gui_obj_t *)cur_tab)->magic != 0x0b) { gui_log("%s, %d, obj magic is invalid %p\n", __FUNCTION__, 15578, (void *)cur_tab); } } gui_obj_t *page_obj = 0; extern void gui_obj_tree_get_widget_by_type(gui_obj_t *, T_OBJ_TYPE , gui_obj_t **); gui_obj_tree_get_widget_by_type(cur_tab, PAGE, &page_obj); { if (page_obj == ((void *)0)) { gui_log("%s, %d, obj is NULL\n", __FUNCTION__, 15578); } else if (((gui_obj_t *)page_obj)->magic != 0x0b) { gui_log("%s, %d, obj magic is invalid %p\n", __FUNCTION__, 15578, (void *)page_obj); } }
    // if (page_obj)
    // {
    //      rvd_hover_down_default();
    //      return rvd_hover_page(page_obj);
    // }

    // if (tabview->cur_id.x == 1 && tabview->cur_id.y == 0)
    // {
    //     return rvd_hover_down_grid(digit_tab_grid, 4, 3);
    // }
    // else if (tabview->cur_id.x == 4 && tabview->cur_id.y == 0)
    // {
    //     return rvd_hover_down_grid(play_tab_grid, 3, 3);
    // }
    // else
    {
        return rvd_hover_down_default();
    }
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
        button_hover_index = start;
    }
    if (button_hover_index > end)
    {
        button_hover_index = end;
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
        button_hover_index = start;
    }
    if (button_hover_index > end)
    {
        button_hover_index = end;
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
    // GUI_WIDGET_POINTER_BY_NAME(obj, "tabview0");
    // gui_tabview_t *tabview = (void *)obj;
    // if (tabview->cur_id.x == 1 && tabview->cur_id.y == 0)
    // {
    //     return rvd_hover_up_grid(digit_tab_grid, 4, 3);
    // }
    // else if (tabview->cur_id.x == 4 && tabview->cur_id.y == 0)
    // {
    //     return rvd_hover_up_grid(play_tab_grid, 3, 3);
    // }
    // else

    //  GUI_WIDGET_POINTER_BY_NAME(obj, "tabview0");
    //  gui_tabview_t *tabview = (void *)obj;
    // gui_tab_t *cur_tab = gui_tabview_get_current_tab(tabview);
    // //GUI_WIDGET_POINTER_BY_TYPE_LOG(page_obj, PAGE, cur_tab);
    // { if (cur_tab == ((void *)0)) { gui_log("%s, %d, obj is NULL\n", __FUNCTION__, 15578); } else if (((gui_obj_t *)cur_tab)->magic != 0x0b) { gui_log("%s, %d, obj magic is invalid %p\n", __FUNCTION__, 15578, (void *)cur_tab); } } gui_obj_t *page_obj = 0; extern void gui_obj_tree_get_widget_by_type(gui_obj_t *, T_OBJ_TYPE , gui_obj_t **); gui_obj_tree_get_widget_by_type(cur_tab, PAGE, &page_obj); { if (page_obj == ((void *)0)) { gui_log("%s, %d, obj is NULL\n", __FUNCTION__, 15578); } else if (((gui_obj_t *)page_obj)->magic != 0x0b) { gui_log("%s, %d, obj magic is invalid %p\n", __FUNCTION__, 15578, (void *)page_obj); } }
    // if (page_obj)
    // {
    //      rvd_hover_up_default();
    //      return rvd_hover_page(page_obj);
    // }

    {
        return rvd_hover_up_default();
    }
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
    // GUI_WIDGET_POINTER_BY_NAME(obj, "tabview0")
    // if (obj)
    // {
    //     gui_tabview_t *tabview = (void *)obj;

    //      if ((tabview->cur_id.x == 0 && tabview->cur_id.y == 1))
    //     {
    //         gui_tabview_jump_tab(tabview, 0, 0);
    //         gui_log("rvd_click_back%d, %d\n", tabview->last_id.x, tabview->last_id.y);
    //         rvd_hover_button_by_index(0);
    //     }
    //     else if (!(tabview->cur_id.x == 2 && tabview->cur_id.y == 0))
    //     {
    //         gui_tabview_jump_tab(tabview, 2, 0);
    //         gui_log("rvd_click_back%d, %d\n", tabview->last_id.x, tabview->last_id.y);
    //         rvd_hover_button_by_index(main_page_hover_index);
    //     }

    // }
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
    // GUI_WIDGET_POINTER_BY_NAME(obj, "tabview0");
    // gui_tabview_t *tabview = (void *)obj;

    // if (tabview->cur_id.x == 1 && tabview->cur_id.y == 0)
    // {
    //     int index = rvd_get_button_hover_index();
    //     gui_log("rvd_click_right%d\n", index);
    //     if (index < 0)
    //     {
    //         return;
    //     }
    //     if (index >= 0 && index <= 2)
    //     {
    //         rvd_hover_button_by_range_ahead(0, 2);
    //     }
    //     else if (index >= 3 && index <= 5)
    //     {
    //         rvd_hover_button_by_range_ahead(3, 5);
    //     }
    //     else if (index >= 6 && index <= 8)
    //     {
    //         rvd_hover_button_by_range_ahead(6, 8);
    //     }
    //     else if (index >= 9 && index <= 10)
    //     {
    //         rvd_hover_button_by_range_ahead(9, 10);
    //     }

    // }
    // else if (tabview->cur_id.x == 4 && tabview->cur_id.y == 0)
    // {
    //     int index = rvd_get_button_hover_index();
    //     gui_log("rvd_click_right%d\n", index);
    //     if (index < 0)
    //     {
    //         return;
    //     }
    //     if (index >= 0 && index <= 2)
    //     {
    //         rvd_hover_button_by_range_ahead(0, 2);
    //     }
    //     else if (index >= 3 && index <= 4)
    //     {
    //         rvd_hover_button_by_range_ahead(3, 4);
    //     }
    //     else if (index >= 6 && index <= 6)
    //     {
    //         rvd_hover_button_by_range_ahead(6, 6);
    //     }


    // }

}
void rvd_hover_left()
{
    // GUI_WIDGET_POINTER_BY_NAME(obj, "tabview0");
    // gui_tabview_t *tabview = (void *)obj;

    // if (tabview->cur_id.x == 1 && tabview->cur_id.y == 0)
    // {
    //     int index = rvd_get_button_hover_index();
    //     gui_log("rvd_click_right%d\n", index);
    //     if (index < 0)
    //     {
    //         return;
    //     }
    //     if (index >= 0 && index <= 2)
    //     {
    //         rvd_hover_button_by_range_back(0, 2);
    //     }
    //     else if (index >= 3 && index <= 5)
    //     {
    //         rvd_hover_button_by_range_back(3, 5);
    //     }
    //     else if (index >= 6 && index <= 8)
    //     {
    //         rvd_hover_button_by_range_back(6, 8);
    //     }
    //     else if (index >= 9 && index <= 10)
    //     {
    //         rvd_hover_button_by_range_back(9, 10);
    //     }

    // }
    // else if (tabview->cur_id.x == 4 && tabview->cur_id.y == 0)
    // {
    //     int index = rvd_get_button_hover_index();
    //     gui_log("rvd_click_right%d\n", index);
    //     if (index < 0)
    //     {
    //         return;
    //     }
    //     if (index >= 0 && index <= 2)
    //     {
    //         rvd_hover_button_by_range_back(0, 2);
    //     }
    //     else if (index >= 3 && index <= 4)
    //     {
    //         rvd_hover_button_by_range_back(3, 4);
    //     }
    //     else if (index >= 6 && index <= 6)
    //     {
    //         rvd_hover_button_by_range_back(6, 6);
    //     }


    // }
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
            GUI_STRINGS_EQUAL(group_name, ((radio_t *)(GUI_TYPE(gui_win_t, obj)->animate->p))->radio_name))
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
            GUI_STRINGS_EQUAL(group_name, ((radio_t *)(GUI_TYPE(gui_win_t, obj)->animate->p))->radio_name)*/)
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
            button_hover_group_name = ((radio_t *)(GUI_TYPE(gui_win_t, child)->animate->p))->radio_name;
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

    gui_obj_tree_count_by_type_and_group(screen, RADIO, &button_count, ((radio_t *)(GUI_TYPE(gui_win_t,
                                                                                    rst)->animate->p))->radio_name);
    gui_log("rvd_get_button_count%d\n", button_count);
    return button_count;
}

