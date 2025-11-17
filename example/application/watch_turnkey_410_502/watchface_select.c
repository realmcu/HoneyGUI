#if 1
#include "root_image/ui_resource.h"
#include "guidef.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "app_main_watch.h"
#include "gui_view_instance.h"
#include "gui_list.h"
#include "gui_text.h"

#define CURRENT_VIEW_NAME "watchface_select_view"

static gui_view_t *current_view = NULL;
static void watchface_select_design(gui_view_t *view);
static gui_list_t *list;

static void watchface_select_on_switch_in(gui_view_t *view)
{
    current_view = view;
    gui_log("current_view initialized: %p\n", current_view);
}
GUI_VIEW_INSTANCE(CURRENT_VIEW_NAME, false, watchface_select_design, NULL);

static void switch_app_cb(void *obj)
{
    const char *obj_name = ((gui_obj_t *)obj)->name;
    if (strcmp(obj_name, "BIG_NUM") == 0)
    {
        extern watchface_type_t current_watchface_type;
        current_watchface_type = WATCHFACE_BIG_NUM;

        gui_view_switch_direct(current_view, gui_view_descriptor_get("watchface_view"),
                               SWITCH_OUT_ANIMATION_ZOOM,
                               SWITCH_IN_ANIMATION_ZOOM);
    }
    else if (strcmp(obj_name, "SPORT") == 0)
    {
        extern watchface_type_t current_watchface_type;
        current_watchface_type = WATCHFACE_SPORT;

        gui_view_switch_direct(current_view, gui_view_descriptor_get("watchface_view"),
                               SWITCH_OUT_ANIMATION_ZOOM,
                               SWITCH_IN_ANIMATION_ZOOM);
    }
    else if (strcmp(obj_name, "VIDEO") == 0)
    {
        extern watchface_type_t current_watchface_type;
        current_watchface_type = WATCHFACE_VIDEO;

        gui_view_switch_direct(current_view, gui_view_descriptor_get("watchface_view"),
                               SWITCH_OUT_ANIMATION_ZOOM,
                               SWITCH_IN_ANIMATION_ZOOM);
    }
}

static void note_design(gui_obj_t *obj, void *p)
{
    (void)p;
    uint16_t index = ((gui_list_note_t *)obj)->index - 1;

    const char *watchface_names[] = {"Big Number", "Sport", "Video", "Add Watchface",};
    const void *watchface_icons[] = {WATCHFACE_BIG_NUM_BIN, WATCHFACE_SPORT_BIN, WATCHFACE_VIDEO_BIN, ADD_WATCHFACE_BIN,};

    uint16_t offset_x = 0;
    uint16_t offset_y = 110;

    if (index == 0)
    {
        gui_img_t *img = gui_img_create_from_mem(obj, "BIG_NUM", watchface_icons[index], offset_x, offset_y,
                                                 0, 0);

        gui_text_t *label = gui_text_create(obj, "watchface_big_num", offset_x + 40, offset_y - 30, 200,
                                            40);
        gui_text_set(label, watchface_names[index], GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255),
                     strlen(watchface_names[index]), 20);
        gui_text_type_set(label, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(label, LEFT);

        gui_obj_add_event_cb(img, (gui_event_cb_t)switch_app_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
    }
    if (index == 1)
    {
        gui_img_t *img = gui_img_create_from_mem(obj, "SPORT", watchface_icons[index], offset_x, offset_y,
                                                 0,
                                                 0);

        gui_text_t *label = gui_text_create(obj, "watchface_sport", offset_x + 40, 80, 200, 40);
        gui_text_set(label, watchface_names[index], GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255),
                     strlen(watchface_names[index]), 20);
        gui_text_type_set(label, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(label, LEFT);

        gui_obj_add_event_cb(img, (gui_event_cb_t)switch_app_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
    }
    if (index == 2)
    {
        gui_img_t *img = gui_img_create_from_mem(obj, "VIODE", watchface_icons[index], offset_x, offset_y,
                                                 0, 0);

        gui_text_t *label = gui_text_create(obj, "watchface_video", offset_x + 40, offset_y - 30, 200, 40);
        gui_text_set(label, watchface_names[index], GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255),
                     strlen(watchface_names[index]), 20);
        gui_text_type_set(label, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(label, LEFT);

        GUI_UNUSED(img);

        gui_log("Under development\n");
    }
    if (index == 3)
    {
        gui_img_t *img = gui_img_create_from_mem(obj, "watchface_img3", watchface_icons[index], offset_x,
                                                 offset_y, 0, 0);

        gui_text_t *label = gui_text_create(obj, "watchface_add", offset_x + 40, offset_y - 30, 200, 40);
        gui_text_set(label, watchface_names[index], GUI_FONT_SRC_TTF, gui_rgb(255, 255, 255),
                     strlen(watchface_names[index]), 20);
        gui_text_type_set(label, SF_COMPACT_TEXT_BOLD_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(label, LEFT);

        GUI_UNUSED(img);
    }
    if (index == 4)
    {
        gui_log("Under development\n");
    }
}
static void watchface_select_design(gui_view_t *view)
{
    gui_obj_t *parent = GUI_BASE(view);
    current_view = view;
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    // gui_view_switch_on_event(parent, gui_view_descriptor_get("watchface_view"), SWITCH_OUT_TO_TOP_USE_TRANSLATION,
    //                          SWITCH_IN_ANIMATION_ZOOM,
    //                          GUI_EVENT_TOUCH_CLICKED);

    int space = 10;
    int length = 200;
    list = gui_list_create(parent, "list", -100, 0, 0, 0, length, space, HORIZONTAL, note_design, NULL,
                           false);
    gui_list_set_style(list, LIST_CLASSIC);
    gui_list_set_note_num(list, 5);
    gui_list_set_out_scope(list, 50);

}
#endif
