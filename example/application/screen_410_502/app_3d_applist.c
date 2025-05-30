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

#define CURRENT_VIEW_NAME "applist_view"

static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *menu_view = NULL;
void applist_app(gui_view_t *view);

static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = applist_app,
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
    menu_view = gui_view_descriptor_get("menu_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);


static void return_to_menu()
{
    gui_view_switch_direct(current_view, menu_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}

static void return_timer_cb()
{
    touch_info_t *tp = tp_get_info();
    GUI_RETURN_HELPER(tp, gui_get_dc()->screen_width, return_to_menu)
}


#define MAX_APPS 10  // Maximum number of 3D objects stored

gui_3d_t *app_3d_list[MAX_APPS];  // Static array
size_t app_count = 0;             // Current number of stored objects

static float rot_y_angle = 0.0f;   // Current angle
static float velocity = 0.0f;       // Angular velocity
static bool isAnimating = false;    // Whether animation is ongoing
static float amplitude = 30.0f;     // Initial amplitude
static float damping =
    0.95f;        // Damping coefficient (between 0.9~0.99, higher values result in longer oscillation)
static float frequency =
    0.2f;      // Oscillation frequency (higher values result in faster oscillation)

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
        if (fabs(rot_y_angle) < 0.1f && fabs(velocity) < 0.1f)
        {
            rot_y_angle = 0.0f;    // Ensure complete stop
            velocity = 0.0f;
            isAnimating = false;
        }
    }
}
static void applist_global_cb(gui_3d_t *this)
{
    gui_3d_camera_UVN_initialize(&this->camera, gui_point_4d(0, 0, 5), gui_point_4d(0, 0, 0), 1, 32767,
                                 90, this->base.w, this->base.h);

    gui_3d_world_inititalize(&this->world, 0, 0, 20, 0, 0, 0, 5);

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

static void add_to_applist(gui_3d_t *app)
{
    if (app_count < MAX_APPS)
    {
        app_3d_list[app_count++] = app;
    }
}

static float click_on_rot_y = 0.0f;
static float click_on_shift_z = 0.0f;
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
    this->base.y = 0;
    this->base.w = 410;
    this->base.h = 410;
    gui_3d_camera_UVN_initialize(&this->camera, gui_point_4d(0, 0, 5), gui_point_4d(0, 0, 0), 1, 32767,
                                 90, this->base.w, this->base.h);

    gui_3d_world_inititalize(&this->world, 0, 0, 20 - click_on_shift_z, 0, click_on_rot_y, 0, 5);

}
static void gui_app_switch(gui_3d_t *this)
{
    gui_log("clicked app: %s\n", this->base.name);

    for (int i = 0; i < app_count; i++)
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


void applist_app(gui_view_t *view)
{
    gui_obj_t *obj = GUI_BASE(view);
    gui_obj_create_timer(obj, 10, true, return_timer_cb);

    gui_3d_t *app1_3d = gui_3d_create(obj, "app1", DESC_APP_BIN,
                                      GUI_3D_DRAW_FRONT_ONLY, 0, 50, 200, 200);

    gui_3d_t *app2_3d = gui_3d_create(obj, "app2", (void *)DESC_APP_BIN,
                                      GUI_3D_DRAW_FRONT_ONLY, 100, 50, 200, 200);
    gui_3d_set_face_image(app2_3d, 0, ALIPAY_BIN);

    gui_3d_t *app3_3d = gui_3d_create(obj, "app3", (void *)DESC_APP_BIN,
                                      GUI_3D_DRAW_FRONT_ONLY, 200, 50, 200, 200);
    gui_3d_set_face_image(app3_3d, 0, MUSIC_BIN);

    gui_3d_t *app4_3d = gui_3d_create(obj, "app4", (void *)DESC_APP_BIN,
                                      GUI_3D_DRAW_FRONT_ONLY, 0, 200, 200, 200);
    gui_3d_set_face_image(app4_3d, 0, QQ_BIN);

    gui_3d_t *app5_3d = gui_3d_create(obj, "app5", (void *)DESC_APP_BIN,
                                      GUI_3D_DRAW_FRONT_ONLY, 100, 200, 200, 200);
    gui_3d_set_face_image(app5_3d, 0, VOICE_BIN);

    gui_3d_t *app6_3d = gui_3d_create(obj, "app6", (void *)DESC_APP_BIN,
                                      GUI_3D_DRAW_FRONT_ONLY, 200, 200, 200, 200);
    gui_3d_set_face_image(app6_3d, 0, WECHAT_BIN);


    add_to_applist(app1_3d);
    add_to_applist(app2_3d);
    add_to_applist(app3_3d);
    add_to_applist(app4_3d);
    add_to_applist(app5_3d);
    add_to_applist(app6_3d);

    for (int i = 0; i < app_count; i++)
    {
        gui_3d_set_global_transform_cb(app_3d_list[i], (gui_3d_global_transform_cb)applist_global_cb);
        gui_3d_set_face_transform_cb(app_3d_list[i], (gui_3d_face_transform_cb)applist_face_cb);

        gui_3d_on_click(app_3d_list[i], (gui_event_cb_t)gui_app_switch, app_3d_list[i]);
    }


    gui_obj_create_timer(&(app1_3d->base), 10, true, update_applist_animation);
    gui_obj_start_timer(&(app1_3d->base));

}