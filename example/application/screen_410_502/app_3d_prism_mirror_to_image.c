/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "guidef.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "app_main_watch.h"
#include "gui_components_init.h"
#include "gui_view.h"
#include "tp_algo.h"
#include "root_image/ui_resource.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "image_view"

/*============================================================================*
*                             Types
*============================================================================*/


/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void app_ui_view_image_design(gui_view_t *view);

/*============================================================================*
 *                            Variables
 *============================================================================*/
/* View Management */
static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *menu_view = NULL;
static const gui_view_descriptor_t *prism_view = NULL;
static const gui_view_descriptor_t descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,

    .on_switch_in = app_ui_view_image_design,
    .on_switch_out = NULL,

    .keep = false,
};

extern int16_t face_flags_rotation;

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
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
    menu_view = gui_view_descriptor_get("menu_view");
    prism_view = gui_view_descriptor_get("prism3d_mirror_view");

    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

// static void return_to_menu()
// {
//     gui_view_switch_direct(current_view, menu_view, SWITCH_OUT_ANIMATION_FADE,
//                            SWITCH_IN_ANIMATION_FADE);
// }

// static void return_timer_cb()
// {
//     touch_info_t *tp = tp_get_info();
//     GUI_RETURN_HELPER(tp, gui_get_dc()->screen_width, return_to_menu)
// }

static void app_ui_view_image_design(gui_view_t *view)
{
    // touch_info_t *tp = tp_get_info();
    // gui_obj_t *obj = GUI_BASE(view);
    // gui_obj_create_timer(obj, 10, true, return_timer_cb);
    gui_view_switch_on_event(view, menu_view, SWITCH_OUT_ANIMATION_FADE,
                             SWITCH_IN_ANIMATION_FADE,
                             GUI_EVENT_KB_SHORT_CLICKED);

    gui_img_t *image = NULL;
    if (face_flags_rotation == 0)
    {
        image = gui_img_create_from_mem(view, "image0", PRISM_FACE5_BIN, 0, 0,
                                        410, 502);
        gui_img_scale(image, 2, 2);
    }
    if (face_flags_rotation == 1)
    {
        image = gui_img_create_from_mem(view, "image1", PRISM_FACE0_BIN, 0, 0, 410,
                                        502);
        gui_img_scale(image, 2, 2);
    }
    if (face_flags_rotation == 2)
    {
        image = gui_img_create_from_mem(view, "image2", PRISM_FACE1_BIN, 0, 0,
                                        410, 502);
        gui_img_scale(image, 2, 2);
    }
    if (face_flags_rotation == 3)
    {
        image = gui_img_create_from_mem(view, "image3", PRISM_FACE2_BIN, 0, 0,
                                        410,
                                        502);
        gui_img_scale(image, 2, 2);
    }
    if (face_flags_rotation == 4)
    {
        image = gui_img_create_from_mem(view, "image4", PRISM_FACE3_BIN, 0, 0,
                                        410, 502);
        gui_img_scale(image, 2, 2);
    }
    if (face_flags_rotation == 5)
    {
        image = gui_img_create_from_mem(view, "image5", PRISM_FACE4_BIN, 0, 0,
                                        410, 502);
        gui_img_scale(image, 2, 2);
    }
    gui_img_set_mode(image, IMG_SRC_OVER_MODE);
    gui_view_set_animate_step(view, 1000);
    gui_view_switch_on_event(view, prism_view, SWITCH_OUT_NONE_ANIMATION,
                             SWITCH_OUT_NONE_ANIMATION,
                             GUI_EVENT_TOUCH_CLICKED);
}
