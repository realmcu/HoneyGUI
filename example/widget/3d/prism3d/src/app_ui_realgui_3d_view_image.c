/* view prism_image start*/
#include "guidef.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "prism3d/root/homelist_dog.c"
#include "prism3d/root/homelist_line_black.c"
#include "prism3d/root/homelist_line_orange.c"
#include "prism3d/root/homelist_number.c"
#include "prism3d/root/homelist_watch_black.c"
#include "prism3d/root/homelist_watch_white.c"
#include "gui_server.h"
#include "gui_components_init.h"
#include "gui_view.h"
#include <tp_algo.h>

#define CURRENT_VIEW_NAME "image_view"

static gui_view_t *current_view = NULL;
static const gui_view_descriptor_t *prism_view = NULL;
static void app_ui_view_image_design(gui_view_t *view);

static const gui_view_descriptor_t descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,

    .on_switch_in = app_ui_view_image_design,
    .on_switch_out = NULL,

    .keep = false,
};

static int gui_view_descriptor_register_init(void)
{
    gui_view_descriptor_register(&descriptor);
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);

    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_REGISTER(gui_view_descriptor_register_init);

static int gui_view_get_other_view_descriptor_init(void)
{
    /* you can get other view descriptor point here */
    prism_view = gui_view_descriptor_get("prism_mirror_view");

    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

extern int16_t face_flags_rotation;
static void app_ui_view_image_design(gui_view_t *view)
{
    touch_info_t *tp = tp_get_info();
    gui_img_t *image;
    if (face_flags_rotation == 0)
    {
        image = gui_img_create_from_mem(view, "image0", (void *)_achomelist_watch_white, 0, 0,
                                        480, 480);
        gui_img_scale(image, 2.4, 2);
    }
    if (face_flags_rotation == 1)
    {
        image = gui_img_create_from_mem(view, "image1", (void *)_achomelist_dog, 0, 0, 480,
                                        480);
        gui_img_scale(image, 2.4, 2);
    }
    if (face_flags_rotation == 2)
    {
        image = gui_img_create_from_mem(view, "image2", (void *)_achomelist_line_black, 0, 0,
                                        480, 480);
        gui_img_scale(image, 2.4, 2);
    }
    if (face_flags_rotation == 3)
    {
        image = gui_img_create_from_mem(view, "image3", (void *)_achomelist_line_orange, 0, 0,
                                        480,
                                        480);
        gui_img_scale(image, 2.4, 2);
    }
    if (face_flags_rotation == 4)
    {
        image = gui_img_create_from_mem(view, "image4", (void *)_achomelist_number, 0, 0,
                                        480, 480);
        gui_img_scale(image, 2.4, 2);
    }
    if (face_flags_rotation == 5)
    {
        image = gui_img_create_from_mem(view, "image5", (void *)_achomelist_watch_black, 0, 0,
                                        480, 480);
        gui_img_scale(image, 2.4, 2);
    }
    gui_img_set_mode(image, IMG_BYPASS_MODE);
    gui_view_set_animate_step(view, 1000);
    gui_view_switch_on_event(view, prism_view, SWITCH_OUT_NONE_ANIMATION,
                             SWITCH_OUT_NONE_ANIMATION,
                             GUI_EVENT_TOUCH_CLICKED);

}

/* view prism_image end*/
