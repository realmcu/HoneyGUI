#include "gui_app.h"
#include "gui_win.h"
#include "gui_page.h"
#include "gui_api.h"
#include "root_image_hongkong/ui_resource.h"
#include "guidef.h"
#include "app_hongkong.h"

// #define CURRENT_VIEW_NAME "watchface_market_view"

// static gui_view_t *current_view = NULL;

const static gui_view_descriptor_t *watchface_view = NULL;
// void watchface_mask_design(gui_view_t* view);

// static gui_view_descriptor_t const descriptor =
// {
//     /* change Here for current view */
//     .name = (const char *)CURRENT_VIEW_NAME,
//     .pView = &current_view,
//     .design_cb = watchface_mask_design,
// };

// static int gui_view_descriptor_register_init(void)
// {
//     gui_view_descriptor_register(&descriptor);
//     gui_log("File: %s, Function: %s\n", __FILE__, __func__);
//     return 0;
// }
// static GUI_INIT_VIEW_DESCRIPTOR_REGISTER(gui_view_descriptor_register_init);

static int gui_view_get_other_view_descriptor_init(void)
{
    /* you can get other view descriptor point here */
    watchface_view = gui_view_descriptor_get("watchface_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

#define APP_WATCHFACE_MARKET_MAX_COUNT 10

static char **xml_file_array = NULL;
extern char watchface_path[];

static char **find_all_xml_files(const char *dirPath, int *xml_file_counts)
{
    *xml_file_counts = 0;
    char **xml_file_array = 0;
    gui_fs_dir *dir = 0;
    struct gui_fs_dirent *entry;
    if ((dir = gui_fs_opendir(dirPath)) == NULL)
    {
        gui_log("opendir(%s) failed", dirPath); return xml_file_array;
    }
    xml_file_array = gui_malloc(APP_WATCHFACE_MARKET_MAX_COUNT * sizeof(char *));
    while ((entry = gui_fs_readdir(dir)) != NULL)
    {

        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 &&
            strcmp(entry->d_name, "widget.ts") != 0)
        {
            //gui_log("debug line%d , entry->d_name = %s\n", __LINE__, entry->d_name);
            char *path2 = gui_malloc(strlen(dirPath) + strlen(entry->d_name) + 10);
            sprintf(path2, "%s/%s", dirPath, entry->d_name);
            gui_fs_dir *dirr = 0;
            dirr = gui_fs_opendir(path2);
            if ((dirr) == NULL)
            {
                gui_log("opendir(%s) failed", path2); gui_free(path2); continue;;
            }
            struct gui_fs_dirent *entryy;
            while ((entryy = gui_fs_readdir(dirr)) != NULL)
            {
                if (strstr(entryy->d_name, ".xml") != NULL)
                {
                    //gui_log("debug line%d , entryy->d_name = %s\n", __LINE__, entryy->d_name);
                    char *path = gui_malloc(strlen(path2) + strlen(entryy->d_name) + 10);
                    sprintf(path, "%s/%s", path2, entryy->d_name);
                    int count = (*xml_file_counts);
                    xml_file_array[count] = path;
                    (*xml_file_counts)++;


                }

            }
            gui_fs_closedir(dirr);
            gui_free(path2);
        }


    }
    gui_fs_closedir(dir);
    xml_file_array[*xml_file_counts] = NULL;
    return xml_file_array;
}
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(win_press_animation_cb)
{
    gui_win_t *win = this_widget;
    gui_win_set_opacity(win, (1.0f - animate->progress_percent) * (255 - 150) + 150);
}
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(win_release_animation_cb)
{
    gui_win_t *win = this_widget;
    gui_win_set_opacity(win, (animate->progress_percent) * (255 - GUI_BASE(
                                                                win)->opacity_value) + GUI_BASE(win)->opacity_value);
}
static GUI_EVENT_CALLBACK_FUNCTION_DEFINE(win_press_cb)
{
    gui_win_t *win = obj;
    gui_win_set_animate(win, 400, 0, win_press_animation_cb, NULL);
}
static GUI_EVENT_CALLBACK_FUNCTION_DEFINE(win_release_cb)
{
    gui_win_t *win = obj;
    gui_win_set_animate(win, 400, 0, win_release_animation_cb, NULL);
}
gui_win_t *win;
gui_view_t *view_parent = NULL;
// gui_win_t *win2;
static GUI_EVENT_CALLBACK_FUNCTION_DEFINE(win_click_cb)
{
    extern uint8_t watchface_index;
    extern char watchface_path[];
    if (param == NULL)
    {
        if (strcmp(GUI_BASE(obj)->name, "wf_UI") == 0)
        {
            watchface_index = 1;
            // gui_view_switch_direct(view_parent, watchface_view, VIEW_ANIMATION_8, VIEW_ANIMATION_5);
        }
        else if (strcmp(GUI_BASE(obj)->name, "wf_ring") == 0)
        {
            watchface_index = 3;
            // gui_view_switch_direct(view_parent, watchface_ring_view, VIEW_ANIMATION_8, VIEW_ANIMATION_5);
        }
        else if (strcmp(GUI_BASE(obj)->name, "wf_butterfly") == 0)
        {
            watchface_index = 2;
            // gui_view_switch_direct(view_parent, watchface_butterfly_view, VIEW_ANIMATION_8, VIEW_ANIMATION_5);
        }
    }
    else
    {
        watchface_index = 3;
        sprintf(watchface_path, "%s", (char *)param);
        gui_log("%s\n", watchface_path);
        if (strstr(watchface_path, "watchface1.xml"))
        {
            watchface_index = 0;
        }
        else if (strstr(watchface_path, "watchface2.xml"))
        {
            watchface_index = 5;
        }
        else if (strstr(watchface_path, "watchface3.xml"))
        {
            watchface_index = 4;
        }
        else
        {
            watchface_index = 0;
        }
    }
    gui_view_switch_direct(view_parent, watchface_view, VIEW_ANIMATION_8, VIEW_ANIMATION_5);
    int count = 0;
    while (1)
    {
        if (xml_file_array[count])
        {
            // gui_log("count = %d, param = 0x%x, path = %s\n", count, xml_file_array[count],
            //         xml_file_array[count]);
            gui_free(xml_file_array[count++]);
        }
        else
        {
            break;
        }
    }
    xml_file_array = NULL;
}

// if use this APP on EVB, need to add romfs.c, romfs.h and update gui_port_filesystem.c
void WATCHFACE_MARKET_list(gui_view_t *view)
{
    view_parent = view;
    win = gui_win_create(view, 0, 0, 0, gui_get_screen_width(),
                         gui_get_screen_height());

    gui_page_t *page = gui_page_create(win, 0, 0, 0, gui_get_screen_width(), 0);
    int count = 0;
    const int space_y = 250;
    const int space_x = 200;
    const int col = 2;
    xml_file_array = find_all_xml_files(gui_get_path_by_relative("app"), &count);
    for (size_t i = 0; i < count / col + 1; i++)
    {
        for (size_t ii = 0; ii < col; ii++)
        {
            if (i * col + ii >= count)
            {
                {
                    gui_win_t *win = gui_win_create(page, "wf_UI",
                                                    0 * space_x + (gui_get_screen_width() - space_x * 2) / 2,
                                                    2 * space_y + 5, space_x - 1, space_y - 1);

                    gui_img_t *img = gui_img_create_from_mem(win, 0, UI_CLOCK_FACE_MAIN_BIN, 0, 0, 0, 0);
                    gui_img_scale(img, 0.5, 0.5);
                    gui_img_set_mode(img, IMG_SRC_OVER_MODE);
                    gui_win_press(win, win_press_cb, NULL);
                    gui_win_release(win, win_release_cb, NULL);
                    gui_win_click(win, win_click_cb, NULL);
                }
                {
                    gui_win_t *win = gui_win_create(page, "wf_ring",
                                                    1 * space_x + (gui_get_screen_width() - space_x * 2) / 2,
                                                    2 * space_y + 5, space_x - 1, space_y - 1);

                    gui_img_t *img = gui_img_create_from_mem(win, 0, WATCHFACE_RING_BIN, 0, 0, 0, 0);
                    gui_img_set_mode(img, IMG_SRC_OVER_MODE);
                    gui_win_press(win, win_press_cb, NULL);
                    gui_win_release(win, win_release_cb, NULL);
                    gui_win_click(win, win_click_cb, NULL);
                }
                {
                    gui_win_t *win = gui_win_create(page, "wf_butterfly",
                                                    0 * space_x + (gui_get_screen_width() - space_x * 2) / 2,
                                                    3 * space_y + 5, space_x - 1, space_y - 1);

                    gui_img_t *img = gui_img_create_from_mem(win, 0, WATCHFACE_BUTTERFLY_BIN, 0, 0, 0, 0);
                    gui_img_set_mode(img, IMG_SRC_OVER_MODE);
                    gui_win_press(win, win_press_cb, NULL);
                    gui_win_release(win, win_release_cb, NULL);
                    gui_win_click(win, win_click_cb, NULL);
                }
                return;
            }
            char *file_output = gui_dom_get_preview_image_file(xml_file_array[i * col + ii]);
            gui_win_t *win = gui_win_create(page, 0, ii * space_x + (gui_get_screen_width() - space_x * 2) / 2,
                                            i * space_y + 5, space_x - 1, space_y - 1);
            const uint8_t *img_addr = gui_get_file_address(file_output);
            if (img_addr)
            {
                gui_img_t *img = gui_img_create_from_mem(win, 0, gui_get_file_address(file_output), 0, 0, 0, 0);
                gui_img_set_mode(img, IMG_SRC_OVER_MODE);
                gui_win_press(win, win_press_cb, NULL);
                gui_win_release(win, win_release_cb, NULL);
                gui_win_click(win, win_click_cb, xml_file_array[i * col + ii]);
            }
        }
    }
}

// void watchface_mask_design(gui_view_t* view)
// {
//     gui_view_switch_on_event(view, app_down_view, VIEW_STILL, VIEW_TRANSPLATION,
//                               GUI_EVENT_TOUCH_MOVE_UP);
//     gui_view_switch_on_event(view, app_up_view, VIEW_STILL, VIEW_TRANSPLATION,
//                               GUI_EVENT_TOUCH_MOVE_DOWN);
//     gui_view_switch_on_event(view, activity_view, VIEW_CUBE, VIEW_CUBE,
//                               GUI_EVENT_TOUCH_MOVE_LEFT);
//     gui_view_switch_on_event(view, watchface_select_view, VIEW_ANIMATION_8, VIEW_ANIMATION_5,
//                               GUI_EVENT_TOUCH_LONG);

//     extern void create_tree_nest(const char *xml, void *obj);
//     create_tree_nest((void *)watchface_path, view);
// }
