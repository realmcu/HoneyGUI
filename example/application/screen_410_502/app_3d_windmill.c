/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "root_image/ui_resource.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_text.h"
#include "time.h"
#include "tp_algo.h"
#include <math.h>
#include "app_main_watch.h"
#include "gui_view.h"
#include "gui_lite3d.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "windmill_view"
#define NUM_BUBBLES 30

/*============================================================================*
 *                           Types
 *============================================================================*/
typedef struct
{
    float driftX;    // Horizontal drift
    float driftY;    // Vertical drift
    float scale;     // Scale factor, size is 96*scale
    gui_img_t *img;  // Bubble's image object
} Bubble;

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void windmill_app(gui_view_t *view);

/*============================================================================*
 *                            Variables
 *============================================================================*/
/* View Management */
static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *menu_view = NULL;
static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = windmill_app,
};

/* Bubbles Management */
static Bubble bubbles[NUM_BUBBLES];

/* Animation Variables */
static float rot_y_angle = 0.0f;
static float rot_z_angle = 0.0f;
static bool touch_started = false;
static int pressing_time = 1;

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
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);


static void update_bubbles(void)
{
    gui_dispdev_t *dc = gui_get_dc();

    for (int i = 0; i < NUM_BUBBLES; i++)
    {
        // Adjust horizontal drift based on rot_y_angle and external speed
        bubbles[i].driftX += rot_y_angle * pressing_time / 10.0f;
        bubbles[i].driftY -= 8.0f * (1.0f - bubbles[i].scale);

        gui_img_translate(bubbles[i].img, bubbles[i].driftX, bubbles[i].driftY);

        // Reset bubble position if it drifts off-screen vertically
        if (bubbles[i].driftY < -100)
        {
            bubbles[i].driftX = xorshift16() % dc->screen_width;
            bubbles[i].driftY = xorshift16() % dc->screen_height + dc->screen_height;
            bubbles[i].scale = (xorshift16() % 701) / 1000.0f + 0.1f;
            gui_img_scale(bubbles[i].img, bubbles[i].scale, bubbles[i].scale);
        }
        // Wrap around horizontally if bubble goes beyond screen borders
        if (bubbles[i].driftX < -dc->screen_width)
        {
            bubbles[i].driftX += 2 * dc->screen_width;
        }
        else if (bubbles[i].driftX > dc->screen_width)
        {
            bubbles[i].driftX -= 2 * dc->screen_width;
        }
    }
}

static void update_windmill_animation(void *param)
{
    touch_info_t *tp = tp_get_info();
    (void)param;

    if (tp->pressed || tp->pressing)
    {
        if (!touch_started)
        {
            // Record the pressing time when touch started
            pressing_time++;
            touch_started = true;
        }

        rot_y_angle += tp->deltaX / 5.0f;

        if (rot_y_angle > 45.0f)
        {
            rot_y_angle = 45.0f;
        }
        else if (rot_y_angle < -45.0f)
        {
            rot_y_angle = -45.0f;
        }

        // Calculate speed based on touch movement
        rot_z_angle += 10.0f + pressing_time;
    }
    else
    {
        touch_started = false;
        pressing_time = 1;

        rot_z_angle += 5.0f;

        // Gradually reset rot_y_angle towards zero
        if (rot_y_angle > 0.0f)
        {
            rot_y_angle -= 0.2f;
        }
        else if (rot_y_angle < 0.0f)
        {
            rot_y_angle += 0.2f;
        }
    }

    update_bubbles();

}

static void windmill_global_cb(l3_model_t *this)
{
    l3_camera_UVN_initialize(&this->camera, l3_4d_point(0, 0, 0), l3_4d_point(0, 0, 40), 1, 32767,
                             90, this->viewPortWidth, this->viewPortHeight);

    l3_world_initialize(&this->world, 0, 0, 40, 0, rot_y_angle, 0, 5);

}
static l3_4x4_matrix_t windmill_face_cb(l3_model_t *this, size_t face_index)
{
    (void)face_index; // unused parameter
    l3_4x4_matrix_t face_matrix;
    l3_4x4_matrix_t transform_matrix;

    l3_calculator_4x4_matrix(&face_matrix, 0, 0, 0, l3_4d_point(0, 0, 0), l3_4d_vector(0, 0, 1),
                             rot_z_angle, 1);

    l3_4x4_matrix_mul(&this->world, &face_matrix, &transform_matrix);

    return transform_matrix;

}

static void windmill_app(gui_view_t *view)
{
    gui_dispdev_t *dc = gui_get_dc();
    gui_obj_t *obj = GUI_BASE(view);
    gui_view_switch_on_event(view, menu_view, SWITCH_OUT_ANIMATION_FADE,
                             SWITCH_IN_ANIMATION_FADE,
                             GUI_EVENT_KB_SHORT_CLICKED);

    gui_img_create_from_mem(obj, "background", WINDMILL_BACKGROUND_BIN, 0, 0, 0, 0);

    gui_img_create_from_mem(obj, "stick", WINDMILL_STICK_BIN, 202, 246, 0, 0);

    l3_model_t *windmill_3d = l3_create_model(DESC_WINDMILL_BIN, L3_DRAW_FRONT_AND_BACK, 15, 52, 380,
                                              380);
    l3_set_global_transform(windmill_3d, (l3_global_transform_cb)windmill_global_cb);
    l3_set_face_transform(windmill_3d, (l3_face_transform_cb)windmill_face_cb);
    gui_lite3d_t *lite3d_windmill = gui_lite3d_create(obj, "lite3d_windmill", windmill_3d, 0, 0, 410,
                                                      502);

    for (int i = 0; i < NUM_BUBBLES; i++)
    {
        bubbles[i].driftX = xorshift16() % dc->screen_width;
        bubbles[i].driftY = xorshift16() % dc->screen_height + dc->screen_height;
        bubbles[i].scale = (xorshift16() % 701) / 1000.0f + 0.1f;
        bubbles[i].img = gui_img_create_from_mem(obj, "bubble", BUBBLE_BIN, 0, 0, 0,
                                                 0);

        gui_img_translate(bubbles[i].img, bubbles[i].driftX, bubbles[i].driftY);
        gui_img_scale(bubbles[i].img, bubbles[i].scale, bubbles[i].scale);
    }

    gui_obj_create_timer(&(lite3d_windmill->base), 10, true, update_windmill_animation);
    gui_obj_start_timer(&(lite3d_windmill->base));

}
