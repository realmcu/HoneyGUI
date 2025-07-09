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
#define CURRENT_VIEW_NAME "applist_view"
#define APP_NUM 6

/*============================================================================*
*                             Types
*============================================================================*/
typedef struct
{
    int16_t pos_x;
    int16_t pos_y;
} Position;

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void applist_app(gui_view_t *view);

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
    .on_switch_in = applist_app,
};

/* 3D Applications List */
static gui_3d_t *app_3d_list[APP_NUM];  // Static array
static const Position app_positions[APP_NUM] =
{
    {0, 50},
    {100, 50},
    {200, 50},
    {0, 200},
    {100, 200},
    {200, 200}
};

/* Animation Variables */
static float rot_y_angle = 0.0f;   // Current angle
static float velocity = 0.0f;       // Angular velocity
static bool isAnimating = false;    // Whether animation is ongoing
static const float amplitude = 30.0f;     // Initial amplitude
static const float damping =
    0.95f;        // Damping coefficient (between 0.9~0.99, higher values result in longer oscillation)
static const float frequency =
    0.2f;      // Oscillation frequency (higher values result in faster oscillation)

static bool click_amplify = false;
static float click_on_rot_y = 0.0f;
static float click_on_shift_z = 0.0f;

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


static void update_applist_animation()
{
    touch_info_t *tp = tp_get_info();

    if (tp->pressed)
    {
        isAnimating = true;        // Start animation
        rot_y_angle = 0.0f;        // Reset angle
        velocity = amplitude * frequency; // Initial velocity
    }

    if (isAnimating)
    {
        // Harmonic motion + damping decay
        float acceleration = -frequency * frequency *
                             rot_y_angle; // Acceleration (towards equilibrium point)
        velocity += acceleration;  // Update velocity
        velocity *= damping;      // Apply damping decay
        rot_y_angle += velocity;   // Update angle

        // Stop animation when oscillation is small enough
        if (fabsf(rot_y_angle) < 0.1f && fabsf(velocity) < 0.1f)
        {
            rot_y_angle = 0.0f;    // Ensure complete stop
            velocity = 0.0f;
            isAnimating = false;
        }
    }
}

static void applist_global_cb(gui_3d_t *this)
{
    gui_3d_camera_UVN_initialize(&this->camera, gui_point_4d(0, 0, 0), gui_point_4d(0, 0, 15), 1, 32767,
                                 90, this->base.w, this->base.h);

    gui_3d_world_inititalize(&this->world, 0, 0, 15, 0, 0, 0, 5);

}

static gui_3d_matrix_t applist_face_cb(gui_3d_t *this, size_t face_index)
{
    gui_3d_matrix_t face_matrix;
    gui_3d_matrix_t transform_matrix;

    gui_3d_calculator_matrix(&face_matrix, 0, 0, 0, gui_3d_point(0, 0, 0), gui_3d_vector(0, 1, 0),
                             rot_y_angle, 1);

    transform_matrix = gui_3d_matrix_multiply(face_matrix, this->world);

    return transform_matrix;
}

static void app_click_on_cb(gui_3d_t *this)
{
    if (click_on_rot_y < 720.f)
    {
        click_on_rot_y += 10.0f;
    }
    if (click_on_shift_z < 5.0f)
    {
        click_on_shift_z += 0.2f;
    }
    this->base.x = 0;
    this->base.y = 46;
    this->base.w = 410;
    this->base.h = 410;
    gui_3d_camera_UVN_initialize(&this->camera, gui_point_4d(0, 0, 0), gui_point_4d(0, 0, 15), 1,
                                 32767,
                                 90, this->base.w, this->base.h);

    gui_3d_world_inititalize(&this->world, 0, 0, 15.0f - click_on_shift_z, 0, click_on_rot_y, 0, 5);
}

static int get_app_index(gui_3d_t *target)
{
    for (int i = 0; i < APP_NUM; i++)
    {
        if (app_3d_list[i] == target)
        {
            return i;
        }
    }
    return -1;
}

static void gui_app_switch(gui_3d_t *this)
{
    gui_log("clicked app: %s\n", this->base.name);

    if (!click_amplify)
    {
        click_amplify = true;
        for (int i = 0; i < APP_NUM; i++)
        {
            gui_3d_t *app = app_3d_list[i];

            if (app != this)
            {
                app->base.not_show = true;
            }
        }

        click_on_rot_y = 0.0f;
        click_on_shift_z = 0.0f;
        gui_3d_set_global_transform_cb(this, (gui_3d_global_transform_cb)app_click_on_cb);
    }
    else
    {
        click_amplify = false;

        int clicked_index = get_app_index(this);
        this->base.x = app_positions[clicked_index].pos_x;
        this->base.y = app_positions[clicked_index].pos_y;
        this->base.w = 200;
        this->base.h = 200;
        gui_3d_set_global_transform_cb(this, (gui_3d_global_transform_cb)applist_global_cb);

        for (int i = 0; i < APP_NUM; i++)
        {
            app_3d_list[i]->base.not_show = false;
        }
    }

}


static void applist_app(gui_view_t *view)
{
    gui_obj_t *obj = GUI_BASE(view);
    gui_view_switch_on_event(view, menu_view, SWITCH_OUT_ANIMATION_FADE,
                             SWITCH_IN_ANIMATION_FADE,
                             GUI_EVENT_KB_SHORT_CLICKED);

    click_amplify = false;
    app_3d_list[0] = gui_3d_create(obj, "app1", DESC_APP_BIN, GUI_3D_DRAW_FRONT_ONLY,
                                   app_positions[0].pos_x, app_positions[0].pos_y, 200, 200);

    app_3d_list[1] = gui_3d_create(obj, "app2", DESC_APP_BIN, GUI_3D_DRAW_FRONT_ONLY,
                                   app_positions[1].pos_x, app_positions[1].pos_y, 200, 200);
    gui_3d_set_face_image(app_3d_list[1], 5, ALIPAY_BIN);

    app_3d_list[2] = gui_3d_create(obj, "app3", DESC_APP_BIN, GUI_3D_DRAW_FRONT_ONLY,
                                   app_positions[2].pos_x, app_positions[2].pos_y, 200, 200);
    gui_3d_set_face_image(app_3d_list[2], 5, MUSIC_BIN);

    app_3d_list[3] = gui_3d_create(obj, "app4", DESC_APP_BIN, GUI_3D_DRAW_FRONT_ONLY,
                                   app_positions[3].pos_x, app_positions[3].pos_y, 200, 200);
    gui_3d_set_face_image(app_3d_list[3], 5, QQ_BIN);

    app_3d_list[4] = gui_3d_create(obj, "app5", DESC_APP_BIN, GUI_3D_DRAW_FRONT_ONLY,
                                   app_positions[4].pos_x, app_positions[4].pos_y, 200, 200);
    gui_3d_set_face_image(app_3d_list[4], 5, VOICE_BIN);

    app_3d_list[5] = gui_3d_create(obj, "app6", DESC_APP_BIN, GUI_3D_DRAW_FRONT_ONLY,
                                   app_positions[5].pos_x, app_positions[5].pos_y, 200, 200);
    gui_3d_set_face_image(app_3d_list[5], 5, WECHAT_BIN);


    for (int i = 0; i < APP_NUM; i++)
    {
        gui_3d_set_global_transform_cb(app_3d_list[i], (gui_3d_global_transform_cb)applist_global_cb);
        gui_3d_set_face_transform_cb(app_3d_list[i], (gui_3d_face_transform_cb)applist_face_cb);

        gui_3d_on_click(app_3d_list[i], (gui_event_cb_t)gui_app_switch, app_3d_list[i]);
    }

    gui_obj_create_timer(&(app_3d_list[0]->base), 10, true, update_applist_animation);

}