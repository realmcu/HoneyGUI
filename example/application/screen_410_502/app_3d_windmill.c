/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "root_image_hongkong/ui_resource.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_text.h"
#include "time.h"
#include "tp_algo.h"
#include <math.h>
#include "cJSON.h"
#include "app_hongkong.h"
#include "gui_view.h"
#include "gui_list.h"
#include "gui_3d.h"

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


static void update_bubbles()
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
            bubbles[i].driftX = rand() % dc->screen_width;
            bubbles[i].driftY = rand() % dc->screen_height + dc->screen_height;
            bubbles[i].scale = (rand() % 701) / 1000.0f + 0.1f;
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

static void update_windmill_animation()
{
    touch_info_t *tp = tp_get_info();

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

static void windmill_global_cb(gui_3d_t *this)
{
    gui_3d_camera_UVN_initialize(&this->camera, gui_point_4d(0, 0, 0), gui_point_4d(0, 0, 40), 1, 32767,
                                 90, this->base.w, this->base.h);

    gui_3d_world_inititalize(&this->world, 0, 0, 40, 0, rot_y_angle, 0, 5);

}
static gui_3d_matrix_t windmill_face_cb(gui_3d_t *this, size_t face_index)
{
    gui_3d_matrix_t face_matrix;
    gui_3d_matrix_t transform_matrix;

    gui_3d_calculator_matrix(&face_matrix, 0, 0, 0, gui_3d_point(0, 0, 0), gui_3d_vector(0, 0, 1),
                             rot_z_angle, 1);

    transform_matrix = gui_3d_matrix_multiply(face_matrix, this->world);

    return transform_matrix;

}

static void windmill_app(gui_view_t *view)
{
    gui_dispdev_t *dc = gui_get_dc();
    gui_obj_t *obj = GUI_BASE(view);
    gui_view_switch_on_event(view, menu_view, SWITCH_OUT_ANIMATION_FADE,
                             SWITCH_IN_ANIMATION_FADE,
                             GUI_EVENT_KB_SHORT_CLICKED);

    gui_img_t *bg = gui_img_create_from_mem(obj, "background", WINDMILL_BACKGROUND_BIN, 0, 0, 0, 0);

    gui_img_t *stick = gui_img_create_from_mem(obj, "stick", WINDMILL_STICK_BIN, 202, 246, 0, 0);

    gui_3d_t *windmill_3d = gui_3d_create(obj, "3d-widget", DESC_WINDMILL_BIN,
                                          GUI_3D_DRAW_FRONT_AND_BACK, 15, 52,
                                          380, 380);

    gui_3d_set_global_transform_cb(windmill_3d, (gui_3d_global_transform_cb)windmill_global_cb);
    gui_3d_set_face_transform_cb(windmill_3d, (gui_3d_face_transform_cb)windmill_face_cb);

    for (int i = 0; i < NUM_BUBBLES; i++)
    {
        bubbles[i].driftX = rand() % dc->screen_width;
        bubbles[i].driftY = rand() % dc->screen_height + dc->screen_height;
        bubbles[i].scale = (rand() % 701) / 1000.0f + 0.1f;
        bubbles[i].img = gui_img_create_from_mem(obj, "bubble", BUBBLE_BIN, 0, 0, 0,
                                                 0);

        gui_img_translate(bubbles[i].img, bubbles[i].driftX, bubbles[i].driftY);
        gui_img_scale(bubbles[i].img, bubbles[i].scale, bubbles[i].scale);
    }

    gui_obj_create_timer(&(windmill_3d->base), 10, true, update_windmill_animation);
    gui_obj_start_timer(&(windmill_3d->base));

}
