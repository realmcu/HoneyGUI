/*
  Copyright (C) 1997-2022 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely.
*/


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "gui_server.h"
#include "unistd.h"
#include "time.h"
#include "gui_components_init.h"
#include "rvd.h"
//#define RVD_APP_STARTUP
#define LOG_VERSION_NUM                "x.x.x"
#define COMPILE_TIME                    __DATE__", "__TIME__
#define PROJECT_NAME                   "HoneyGUI Simulator"
#define COMPANY_NAME                   "Realtek Semiconductor Corporation"

extern int rvd_hover_button_by_index(int index);
extern int rvd_get_main_page_hover_index();
extern  int rvd_get_tv_main_page_hover_index();
static GUI_EVENT_CALLBACK_FUNCTION_DEFINE(on_click_jump_cb_tabview)
{


    GUI_WIDGET_POINTER_BY_NAME(obj_tabview, "tabview0");
    gui_tabview_t *tabview = (void *)obj_tabview;

    if (tabview->cur_id.x == 1 && tabview->cur_id.y == 0)
    {
        rvd_hover_button_by_index(4);
    }
    else if (tabview->cur_id.x == 4 && tabview->cur_id.y == 0)
    {
        rvd_hover_button_by_index(1);
    }
    else if (tabview->cur_id.x == 0 && tabview->cur_id.y == 0)
    {
        rvd_hover_button_by_index(0);
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
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(page_callback);
static void set_text_convert_to_img_in_root(gui_obj_t *root);
int main(int argc, char **argv)
{
    printf("\n\n\t************** %s **************\t\n   \t <%s> \t Build Time: %s\n\n", \
           COMPANY_NAME, \
           PROJECT_NAME, \
           COMPILE_TIME);

    gui_change_default_path(argc, argv);
    gui_components_init();
#ifdef ENABLE_RTK_GUI_SCRIPT_AS_A_APP
    extern gui_app_t *get_app_launcher_frontend(void);
    gui_app_install(get_app_launcher_frontend(), get_app_launcher_frontend()->ui_design, NULL);
#ifdef RVD_APP_STARTUP
    /*Startup RVD APP (index 0)*/
    int rst = rvd_startup_app(1);
    gui_log("RVD_APP_STARTUP:%d\n", rst);
#else
    /*Startup RVD launcher*/

    gui_app_startup(get_app_launcher_frontend());
#endif
#endif
// gui_update_screen_force(true);
//     GUI_WIDGET_POINTER_BY_NAME(tabview, "tabview0");
//     gui_tabview_jump_tab((void *)tabview, 0, 0);
//     gui_tabview_tab_change((void *)tabview, on_click_jump_cb_tabview, NULL);

//     GUI_WIDGET_POINTER_BY_NAME(page0, "page0");
//    set_text_convert_to_img_in_root((void *)page0);
//     gui_page_set_animate(page0, 1000, -1, page_callback, page0);
// GUI_TYPE(gui_page_t, page0)->top_offset = page0->h/2-34;
// GUI_TYPE(gui_page_t, page0)->bottom_offset = -(page0->h/2-34);
// gui_page_center_alignment((void *)page0, page0->h/5);
    while (1)
    {
        time_t now;
        now = time(NULL);
        printf("World Time: %.*s\n", 25, ctime(&now));
        sleep(1000);
    }

    return 0;
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


