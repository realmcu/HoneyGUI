#include "guidef.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_img.h"
#include "gui_gray.h"
#include "gui_obj.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "gui_server.h"
#include "gui_view.h"
#include "gui_canvas.h"
#include "gui_3d.h"
#include "gui_matrix.h"

#include <tp_algo.h>
#include <math.h>
#include "gui_components_init.h"
#include "root_image_hongkong/ui_resource.h"
#include "app_hongkong.h"

static gui_3d_t *fish0 = NULL;
static gui_3d_t *fish0_shadow = NULL;
static gui_3d_t *fish1 = NULL;
static gui_3d_t *fish1_shadow = NULL;
static gui_3d_t *fish2 = NULL;
static gui_3d_t *fish2_shadow = NULL;
static gui_3d_t *fish3 = NULL;
static gui_3d_t *fish3_shadow = NULL;

static gui_win_t *fish0_window = NULL;
static gui_win_t *fish1_window = NULL;
static gui_win_t *fish2_window = NULL;
static gui_win_t *fish3_window = NULL;
static gui_win_t *lotus_leaf_win = NULL;
static gui_win_t *leaf_win0 = NULL;
static gui_win_t *leaf_win1 = NULL;
static gui_win_t *leaf_win2 = NULL;
static gui_win_t *leaf_win3 = NULL;
static gui_win_t *wave_win = NULL;

static gui_img_t *lotus = NULL;
static gui_img_t *lotus_leaf = NULL;
static gui_img_t *leaf0 = NULL;
static gui_img_t *leaf1 = NULL;
static gui_img_t *leaf2 = NULL;
static gui_img_t *leaf3 = NULL;
static gui_img_t *wave_img = NULL;


//x:80 ~ -80,y:110 ~ -80
static float fish_angle = 0.0f;

static float fish0_x = 0.0f;
static float fish0_y = 0.0f;

static float fish1_x = 0;
static float fish1_y = 0;

static float fish2_x = 0;
static float fish2_y = 0;

static float fish3_x = 0;
static float fish3_y = 0;

static float fish0_rz = 0.0f;
static float fish1_rz = 0.0f;
static float fish2_rz = 0.0f;
static float fish3_rz = 0.0f;

#define CURRENT_VIEW_NAME "pandkoi_view"


static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *menu_view = NULL;
static void app_ui_pond_koi_design(gui_view_t *view);

static const gui_view_descriptor_t descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,

    .on_switch_in = app_ui_pond_koi_design,
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

// static void return_timer_cb()
// {
//     touch_info_t *tp = tp_get_info();
//     GUI_RETURN_HELPER(tp, gui_get_dc()->screen_width, return_to_menu)
// }

static bool koi_moving_to_target = true;
static float target_dx = 30.0f;
static float target_dy = 20.0f;
static float source_dx = 0.0f;
static float source_dy = 0.0f;
static float move_speed = 0.01f;
static float fish_time = 0.0f;
void fish0_animate_cb()
{
    touch_info_t *tp = tp_get_info();
    gui_dispdev_t *dc = gui_get_dc();
    static uint32_t last_update = 0;
    const uint32_t interval = 3000;

    uint32_t now = gui_ms_get();

    // if (tp->pressed)
    // {
    //     target_dx = (tp->x - dc->screen_width / 2) / 2.5f;
    //     target_dy = (tp->y - dc->screen_height / 2) / 2.5f;
    //     move_speed = 0.05f; // Reset move speed on touch
    //     koi_moving_to_target = true;
    // }
    if (now - last_update > interval)
    {
        target_dx = rand() % 160 - 80;
        target_dy = rand() % 190 - 80;
        last_update = now;

        koi_moving_to_target = true;
    }
    else
    {
        move_speed = 0.05f;
    }

    if (koi_moving_to_target)
    {
        float dx = target_dx - source_dx;
        float dy = target_dy - source_dy;

        float distance = sqrtf(dx * dx + dy * dy);

        if (distance > 10.0f)
        {
            // Acceleration and deceleration
            float speed_factor = fminf(distance / 40.0f, 1.0f);
            source_dx += dx * move_speed * speed_factor;
            source_dy += dy * move_speed * speed_factor;

            // Caculate new rotate angle
            float desired_angle = atan2f(dy, dx) * (180.0f / M_PI_F) + 90;
            float angle_difference = desired_angle - fish0_rz;

            if (angle_difference > 180.0f)
            {
                angle_difference -= 360.0f;
            }
            if (angle_difference < -180.0f)
            {
                angle_difference += 360.0f;
            }
            fish0_rz += angle_difference * 0.1f;

            // Adjust fish flapping frequency based on speed
            fish_time += 0.2f + speed_factor * 0.2f;
            fish_angle = 3.0f * sinf(fish_time);

            fish0_x = source_dx;
            fish0_y = source_dy;
        }
        else
        {
            koi_moving_to_target = false;
        }
    }
    else
    {
        // frame_counter++;
        fish_time += 0.1f;
        fish_angle = 1.01f * sinf(fish_time);
    }
}

static bool koi1_moving_to_target = true;
static float target1_dx = 0.0f;
static float target1_dy = 0.0f;
static float source1_dx = 0.0f;
static float source1_dy = 0.0f;
static float move_speed1 = 0.03f;

void fish1_animate_cb()
{
    touch_info_t *tp = tp_get_info();
    gui_dispdev_t *dc = gui_get_dc();
    static uint32_t last_update = 0;
    const uint32_t interval = 5000;

    uint32_t now = gui_ms_get();

    if (now - last_update > interval)
    {
        target1_dx = rand() % 160 - 80;
        target1_dy = rand() % 190 - 80;
        last_update = now;

        koi1_moving_to_target = true;
    }
    if (tp->pressed)
    {
        target1_dx = rand() % 160 - 80;
        target1_dy = rand() % 190 - 80;
        koi1_moving_to_target = true;
    }

    if (koi1_moving_to_target)
    {
        float dx = target1_dx - source1_dx;
        float dy = target1_dy - source1_dy;

        float distance = sqrtf(dx * dx + dy * dy);

        if (distance > 1.0f)
        {
            // Acceleration and deceleration
            float speed_factor = fminf(distance / 40.0f, 1.0f);
            source1_dx += dx * move_speed * speed_factor;
            source1_dy += dy * move_speed * speed_factor;

            // Caculate new rotate angle
            float desired_angle = atan2f(dy, dx) * (180.0f / M_PI_F) + 90;
            float angle_difference = desired_angle - fish1_rz;

            if (angle_difference > 180.0f)
            {
                angle_difference -= 360.0f;
            }
            if (angle_difference < -180.0f)
            {
                angle_difference += 360.0f;
            }
            fish1_rz += angle_difference * 0.1f;

            // Adjust fish flapping frequency based on speed
            fish_time += 0.2f + speed_factor * 0.2f;
            fish_angle = 3.0f * sinf(fish_time);

            fish1_x = source1_dx;
            fish1_y = source1_dy;
            // gui_log("fish1_x: %f, fish1_y: %f\n", fish1_x, fish1_y);
        }
        else
        {
            koi_moving_to_target = false;
        }
    }
    else
    {
        fish_time += 0.1f;
        fish_angle = 1.0f * sinf(fish_time);
    }
}
bool koi2_moving_to_target = true;
static float target2_dx = 0.0f;
static float target2_dy = 0.0f;
static float source2_dx = 0.0f;
static float source2_dy = 0.0f;
static float move_speed2 = 0.08f;
void fish2_animate_cb()
{
    touch_info_t *tp = tp_get_info();
    gui_dispdev_t *dc = gui_get_dc();
    static uint32_t last_update = 0;
    const uint32_t interval = 5000;

    uint32_t now = gui_ms_get();

    if (now - last_update > interval)
    {
        target2_dx = rand() % 160 - 80;
        target2_dy = rand() % 190 - 80;
        last_update = now;

        koi2_moving_to_target = true;
    }
    if (tp->pressed)
    {
        target2_dx = rand() % 160 - 80;
        target2_dy = rand() % 190 - 80;
        koi2_moving_to_target = true;
    }

    if (koi2_moving_to_target)
    {
        float dx = target2_dx - source2_dx;
        float dy = target2_dy - source2_dy;

        float distance = sqrtf(dx * dx + dy * dy);

        if (distance > 1.0f)
        {
            // Acceleration and deceleration
            float speed_factor = fminf(distance / 40.0f, 1.0f);
            source2_dx += dx * move_speed * speed_factor;
            source2_dy += dy * move_speed * speed_factor;

            // Caculate new rotate angle
            float desired_angle = atan2f(dy, dx) * (180.0f / M_PI_F) + 90;
            float angle_difference = desired_angle - fish2_rz;

            if (angle_difference > 180.0f)
            {
                angle_difference -= 360.0f;
            }
            if (angle_difference < -180.0f)
            {
                angle_difference += 360.0f;
            }
            fish2_rz += angle_difference * 0.1f;

            // Adjust fish flapping frequency based on speed
            fish_time += 0.2f + speed_factor * 0.2f;
            fish_angle = 3.0f * sinf(fish_time);

            fish2_x = source2_dx;
            fish2_y = source2_dy;
            // gui_log("fish1_x: %f, fish1_y: %f\n", fish1_x, fish1_y);
        }
        else
        {
            koi_moving_to_target = false;
        }
    }
    else
    {
        fish_time += 0.1f;
        fish_angle = 1.0f * sinf(fish_time);
    }

}
bool koi3_moving_to_target = true;
static float target3_dx = -20.0f;
static float target3_dy = 0.0f;
static float source3_dx = 0.0f;
static float source3_dy = 0.0f;
static float move_speed3 = 0.08f;
void fish3_animate_cb()
{
    touch_info_t *tp = tp_get_info();
    gui_dispdev_t *dc = gui_get_dc();
    static uint32_t last_update = 0;
    const uint32_t interval = 5000;

    uint32_t now = gui_ms_get();

    if (now - last_update > interval)
    {
        target3_dx = rand() % 160 - 80;
        target3_dy = rand() % 190 - 80;
        last_update = now;

        koi3_moving_to_target = true;
    }
    if (tp->pressed)
    {
        target3_dx = rand() % 160 - 80;
        target3_dy = rand() % 190 - 80;
        koi3_moving_to_target = true;
    }

    if (koi3_moving_to_target)
    {
        float dx = target3_dx - source3_dx;
        float dy = target3_dy - source3_dy;

        float distance = sqrtf(dx * dx + dy * dy);

        if (distance > 1.0f)
        {
            // Acceleration and deceleration
            float speed_factor = fminf(distance / 40.0f, 1.0f);
            source3_dx += dx * move_speed * speed_factor;
            source3_dy += dy * move_speed * speed_factor;

            // Caculate new rotate angle
            float desired_angle = atan2f(dy, dx) * (180.0f / M_PI_F) + 90;
            float angle_difference = desired_angle - fish3_rz;

            if (angle_difference > 180.0f)
            {
                angle_difference -= 360.0f;
            }
            if (angle_difference < -180.0f)
            {
                angle_difference += 360.0f;
            }
            fish3_rz += angle_difference * 0.1f;

            // Adjust fish flapping frequency based on speed
            fish_time += 0.2f + speed_factor * 0.2f;
            fish_angle = 3.0f * sinf(fish_time);

            fish3_x = source3_dx;
            fish3_y = source3_dy;
        }
        else
        {
            koi3_moving_to_target = false;
        }
    }
    else
    {
        fish_time += 0.1f;
        fish_angle = 1.0f * sinf(fish_time);
    }

}
#define MAX_OFFSET 10.0f
static float swing_phase = 0.0f;
const float SWING_SPEED = 0.01f;

static void lotus_leaf_animate_cb(void *parent)
{
    gui_win_t *window = (gui_win_t *)parent;

    swing_phase += SWING_SPEED;
    if (swing_phase > 2 * M_PI_F) { swing_phase -= 2 * M_PI_F; }

    float offset_x = sinf(swing_phase) * MAX_OFFSET * 0.8f;
    float offset_y = cosf(swing_phase * 0.6f) * MAX_OFFSET * 0.3f;
    float rotate_angle = sinf(swing_phase * 0.9f) * 3.0f;

    matrix_translate(offset_x, offset_y, window->base.matrix);
    matrix_rotate(rotate_angle, window->base.matrix);
}
float wave_scale = 1.0f;
float wave_opa = 255.0f;
static bool wave_animating = false;
float wave_x = 0.0f;
float wave_y = 0.0f;
static void wave_animate_cb(void *parent)
{
    touch_info_t *tp = tp_get_info();
    gui_dispdev_t *dc = gui_get_dc();

    if (tp->pressed)
    {
        if (!wave_animating)
        {
            wave_x = tp->x - 32;
            wave_y = tp->y - 32;
            wave_img = gui_img_create_from_mem(wave_win, "wave", WAVE_BIN, wave_x, wave_y, 0, 0);
            gui_img_set_mode(wave_img, IMG_SRC_OVER_MODE);
            wave_scale = 1.0f;
            wave_opa = 255.0f;
            wave_img->base.not_show = true;
            wave_animating = true;
        }

        gui_log("Wave center: (%f, %f)\n", (double)wave_x, (double)wave_y);
        // matrix_translate(wave_x, wave_x, wave_win->base.matrix);

    }
    else if (wave_animating)
    {
        wave_img->base.not_show = false;
        wave_scale += 0.1f;
        wave_opa -= 5.0f;
        gui_img_scale(wave_img, wave_scale, wave_scale);
        gui_img_set_opacity(wave_img, (uint8_t)fmaxf(wave_opa, 0));

        if (wave_opa <= 0)
        {
            wave_img->base.not_show = true;
            wave_animating = false;
        }
    }
}
static void fish0_global_cb(gui_3d_t *this)
{
    gui_dispdev_t *dc = gui_get_dc();

    gui_3d_camera_UVN_initialize(&this->camera, gui_point_4d(0, 0, 0), gui_point_4d(0, 0, 80), 1, 32767,
                                 90, this->base.w, this->base.h);

    gui_3d_world_inititalize(&this->world, fish0_x, fish0_y, 80, 0, 0, fish0_rz,
                             6);

}
static void fish0_shadow_global_cb(gui_3d_t *this)
{
    gui_dispdev_t *dc = gui_get_dc();

    gui_3d_camera_UVN_initialize(&this->camera, gui_point_4d(0, 0, 0), gui_point_4d(0, 0, 80), 1, 32767,
                                 90, this->base.w, this->base.h);

    gui_3d_world_inititalize(&this->world, fish0_x - 8, fish0_y - 10, 80, 0, 0, fish0_rz,
                             6);

}
static void fish1_global_cb(gui_3d_t *this)
{
    gui_dispdev_t *dc = gui_get_dc();

    gui_3d_camera_UVN_initialize(&this->camera, gui_point_4d(0, 0, 0), gui_point_4d(0, 0, 80), 1, 32767,
                                 90, this->base.w, this->base.h);

    gui_3d_world_inititalize(&this->world, fish1_x, fish1_y, 80, 0, 0, fish1_rz,
                             6);

}
static void fish1_shadow_global_cb(gui_3d_t *this)
{
    gui_dispdev_t *dc = gui_get_dc();

    gui_3d_camera_UVN_initialize(&this->camera, gui_point_4d(0, 0, 0), gui_point_4d(0, 0, 80), 1, 32767,
                                 90, this->base.w, this->base.h);

    gui_3d_world_inititalize(&this->world, fish1_x - 8, fish1_y - 10, 80, 0, 0, fish1_rz,
                             6);

}
static void fish2_global_cb(gui_3d_t *this)
{
    gui_dispdev_t *dc = gui_get_dc();

    gui_3d_camera_UVN_initialize(&this->camera, gui_point_4d(0, 0, 0), gui_point_4d(0, 0, 80), 1, 32767,
                                 90, this->base.w, this->base.h);

    gui_3d_world_inititalize(&this->world, fish2_x, fish2_y, 80, 0, 0, fish2_rz,
                             6);

}
static void fish2_shadow_global_cb(gui_3d_t *this)
{
    gui_dispdev_t *dc = gui_get_dc();

    gui_3d_camera_UVN_initialize(&this->camera, gui_point_4d(0, 0, 0), gui_point_4d(0, 0, 80), 1, 32767,
                                 90, this->base.w, this->base.h);

    gui_3d_world_inititalize(&this->world, fish2_x - 8, fish2_y - 10, 80, 0, 0, fish2_rz,
                             6);

}
static void fish3_global_cb(gui_3d_t *this)
{
    gui_dispdev_t *dc = gui_get_dc();

    gui_3d_camera_UVN_initialize(&this->camera, gui_point_4d(0, 0, 0), gui_point_4d(0, 0, 80), 1, 32767,
                                 90, this->base.w, this->base.h);

    gui_3d_world_inititalize(&this->world, fish3_x, fish3_y, 80, 0, 0, fish3_rz,
                             6);

}
static void fish3_shadow_global_cb(gui_3d_t *this)
{
    gui_dispdev_t *dc = gui_get_dc();

    gui_3d_camera_UVN_initialize(&this->camera, gui_point_4d(0, 0, 0), gui_point_4d(0, 0, 80), 1, 32767,
                                 90, this->base.w, this->base.h);

    gui_3d_world_inititalize(&this->world, fish3_x - 8, fish3_y - 10, 80, 0, 0, fish3_rz,
                             6);

}
static gui_3d_matrix_t fish_face_cb(gui_3d_t *this, size_t face_index/*face offset*/)
{
    gui_3d_matrix_t face_matrix;
    gui_3d_matrix_t transform_matrix;

    if (face_index == 0)
    {
        gui_3d_calculator_matrix(&face_matrix, 0, 0, 0, gui_3d_point(0, 0, 0), gui_3d_vector(0, 0, 1),
                                 0, 1);
    }
    if (face_index == 1)
    {
        gui_3d_calculator_matrix(&face_matrix, 0, 0, 0, gui_3d_point(0, 0, 0), gui_3d_vector(0, 0, 1),
                                 fish_angle, 1);
    }
    if (face_index == 2)//head
    {
        gui_3d_calculator_matrix(&face_matrix, 0, 0, 0, gui_3d_point(0, 0, 0), gui_3d_vector(0, 0, 1),
                                 0, 1);
    }
    if (face_index == 3)
    {
        gui_3d_calculator_matrix(&face_matrix, 0, 0, 0, gui_3d_point(0, -5, 0), gui_3d_vector(0, 0, 1),
                                 fish_angle, 1);
    }
    if (face_index == 4)
    {
        gui_3d_calculator_matrix(&face_matrix, 0, 0, 0, gui_3d_point(0, -8, 0), gui_3d_vector(0, 0, 1),
                                 fish_angle, 1);
    }


    transform_matrix = gui_3d_matrix_multiply(face_matrix, this->world);

    return transform_matrix;

}
static void app_ui_pond_koi_design(gui_view_t *view)
{
    wave_animating = false;
    srand((unsigned int)gui_ms_get());
    gui_obj_t *obj = GUI_BASE(view);
    // gui_obj_create_timer(obj, 10, true, return_timer_cb);
    gui_view_switch_on_event(view, menu_view, SWITCH_OUT_ANIMATION_FADE,
                             SWITCH_IN_ANIMATION_FADE,
                             GUI_EVENT_KB_SHORT_CLICKED);

    fish0_y = (float)(rand() % 191 - 80);
    fish0_x = (float)(rand() % 161 - 80);
    fish1_x = (float)(rand() % 161 - 80);
    fish1_y = (float)(rand() % 191 - 80);
    fish2_x = (float)(rand() % 161 - 80);
    fish2_y = (float)(rand() % 191 - 80);
    fish3_x = (float)(rand() % 161 - 80);
    fish3_y = (float)(rand() % 191 - 80);
    gui_img_create_from_mem(view, "img_1_test", PONDGROUND_BIN, 0, 0, 0, 0);

    fish0_window = gui_win_create(view, "fish_window", 0, 0, 48, 100);
    fish1_window = gui_win_create(view, "fish1_window", 0, 0, 48, 100);
    fish2_window = gui_win_create(view, "fish2_window", 0, 0, 48, 100);
    fish3_window = gui_win_create(view, "fish3_window", 0, 0, 48, 100);

    fish0_shadow = gui_3d_create(fish0_window, "3d-fish0", DESC_KOI_SHADOW_BIN, GUI_3D_DRAW_FRONT_ONLY,
                                 0, 0,
                                 410, 502);
    fish1_shadow = gui_3d_create(fish1_window, "3d-fish1", DESC_KOI_SHADOW_BIN, GUI_3D_DRAW_FRONT_ONLY,
                                 0, 0,
                                 410, 502);
    fish2_shadow = gui_3d_create(fish2_window, "3d-fish2", DESC_KOI_SHADOW_BIN, GUI_3D_DRAW_FRONT_ONLY,
                                 0, 0,
                                 410, 502);
    fish3_shadow = gui_3d_create(fish3_window, "3d-fish0", DESC_KOI_SHADOW_BIN, GUI_3D_DRAW_FRONT_ONLY,
                                 0, 0,
                                 410, 502);
    fish0 = gui_3d_create(fish0_window, "3d-fish0", DESC_KOI0_BIN, GUI_3D_DRAW_FRONT_ONLY, 0, 0,
                          0, 0);
    fish1 = gui_3d_create(fish1_window, "3d-fish1", DESC_KOI1_BIN, GUI_3D_DRAW_FRONT_ONLY, 0, 0,
                          410, 502);
    fish2 = gui_3d_create(fish2_window, "3d-fish2", DESC_KOI2_BIN, GUI_3D_DRAW_FRONT_ONLY, 0, 0,
                          410, 502);

    fish3 = gui_3d_create(fish3_window, "3d-fish3", DESC_KOI0_BIN, GUI_3D_DRAW_FRONT_ONLY, 0, 0, 0, 0);
    gui_3d_set_global_transform_cb(fish0, (gui_3d_global_transform_cb)fish0_global_cb);
    gui_3d_set_face_transform_cb(fish0, (gui_3d_face_transform_cb)fish_face_cb);
    gui_3d_set_global_transform_cb(fish0_shadow, (gui_3d_global_transform_cb)fish0_shadow_global_cb);
    gui_3d_set_face_transform_cb(fish0_shadow, (gui_3d_face_transform_cb)fish_face_cb);

    gui_3d_set_global_transform_cb(fish1, (gui_3d_global_transform_cb)fish1_global_cb);
    gui_3d_set_face_transform_cb(fish1, (gui_3d_face_transform_cb)fish_face_cb);
    gui_3d_set_global_transform_cb(fish1_shadow, (gui_3d_global_transform_cb)fish1_shadow_global_cb);
    gui_3d_set_face_transform_cb(fish1_shadow, (gui_3d_face_transform_cb)fish_face_cb);

    gui_3d_set_global_transform_cb(fish2, (gui_3d_global_transform_cb)fish2_global_cb);
    gui_3d_set_face_transform_cb(fish2, (gui_3d_face_transform_cb)fish_face_cb);
    gui_3d_set_global_transform_cb(fish2_shadow, (gui_3d_global_transform_cb)fish2_shadow_global_cb);
    gui_3d_set_face_transform_cb(fish2_shadow, (gui_3d_face_transform_cb)fish_face_cb);

    gui_3d_set_global_transform_cb(fish3, (gui_3d_global_transform_cb)fish3_global_cb);
    gui_3d_set_face_transform_cb(fish3, (gui_3d_face_transform_cb)fish_face_cb);
    gui_3d_set_global_transform_cb(fish3_shadow, (gui_3d_global_transform_cb)fish3_shadow_global_cb);
    gui_3d_set_face_transform_cb(fish3_shadow, (gui_3d_face_transform_cb)fish_face_cb);

    lotus_leaf_win = gui_win_create(view, "lotus_leaf_window", 200, 300, 80, 80);
    lotus = gui_img_create_from_mem(lotus_leaf_win, "lotus", LOTUS00_BIN, 0, 0, 0, 0);
    gui_img_set_mode(lotus, IMG_SRC_OVER_MODE);

    leaf_win0 = gui_win_create(view, "leaf_window0", 30, 50, 80, 80);
    leaf_win1 = gui_win_create(view, "leaf_window1", 10, 190, 112, 122);
    leaf_win2 = gui_win_create(view, "leaf_window2", 200, 20, 144, 144);
    leaf_win3 = gui_win_create(view, "leaf_window3", 300, 380, 144, 144);
    leaf0 = gui_img_create_from_mem(leaf_win0, "leaf0", LOTUS01_BIN, 0, 0, 0, 0);
    gui_img_set_mode(leaf0, IMG_SRC_OVER_MODE);
    leaf1 = gui_img_create_from_mem(leaf_win1, "leaf1", LOTUS02_BIN, 0, 0, 0, 0);
    gui_img_set_mode(leaf1, IMG_SRC_OVER_MODE);
    leaf2 = gui_img_create_from_mem(leaf_win2, "leaf2", LOTUS03_BIN, 0, 0, 0, 0);
    gui_img_set_mode(leaf2, IMG_SRC_OVER_MODE);
    leaf3 = gui_img_create_from_mem(leaf_win3, "lotus_leaf", LOTUS04_BIN, 0, 0, 0, 0);
    gui_img_set_mode(leaf3, IMG_SRC_OVER_MODE);

    wave_win = gui_win_create(view, "wave_window", 0, 0, 410, 502);

    gui_obj_create_timer(&(fish0_window->base), 17, true, fish0_animate_cb);
    gui_obj_start_timer(&(fish0_window->base));
    gui_obj_create_timer(&(fish1_window->base), 17, true, fish1_animate_cb);
    gui_obj_start_timer(&(fish1_window->base));
    gui_obj_create_timer(&(fish2_window->base), 17, true, fish2_animate_cb);
    gui_obj_start_timer(&(fish2_window->base));
    gui_obj_create_timer(&(fish3_window->base), 17, true, fish3_animate_cb);
    gui_obj_start_timer(&(fish3_window->base));
    gui_obj_create_timer(&(leaf_win0->base), 17, true, fish3_animate_cb);
    gui_obj_start_timer(&(leaf_win0->base));

    gui_obj_create_timer(&(lotus_leaf_win->base), 17, true, lotus_leaf_animate_cb);
    gui_obj_start_timer(&(lotus_leaf_win->base));
    gui_obj_create_timer(&(leaf_win0->base), 17, true, lotus_leaf_animate_cb);
    gui_obj_start_timer(&(leaf_win0->base));
    gui_obj_create_timer(&(leaf_win1->base), 17, true, lotus_leaf_animate_cb);
    gui_obj_start_timer(&(leaf_win1->base));
    gui_obj_create_timer(&(leaf_win2->base), 17, true, lotus_leaf_animate_cb);
    gui_obj_start_timer(&(leaf_win2->base));
    gui_obj_create_timer(&(leaf_win3->base), 17, true, lotus_leaf_animate_cb);
    gui_obj_start_timer(&(leaf_win3->base));

    gui_obj_create_timer(&(wave_win->base), 17, true, wave_animate_cb);
    gui_obj_start_timer(&(wave_win->base));
}



