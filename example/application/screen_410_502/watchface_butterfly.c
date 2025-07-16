#if 0
#include "guidef.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "gui_win.h"
#include "gui_server.h"
#include "app_main_watch.h"
#include "gui_canvas.h"
#include "gui_3d.h"
#include "math.h"
#include <time.h>
#include "root_image/ui_resource.h"
#include "tp_algo.h"

#define SCREEN_WIDTH  410
#define SCREEN_HEIGHT (int16_t)gui_get_screen_height()

#define CURRENT_VIEW_NAME "watchface_butterfly_view"

static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *menu_view = NULL;
void watchface_butterfly_app(gui_view_t *view);

static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = watchface_butterfly_app,
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

static float radius = 20.0f, theta = 0;
static int frame_counter = 0;
static float wing_angle = 0.0f;
static float butterfly_x = 0.0f;
static float butterfly_y = 0.0f;
static float butterfly_z = 0.0f;
static float butterfly_rz = 0.0f;
static bool fly_flag = 1;

static gui_img_t *h_hand, *m_hand, *s_hand;

static void win_touch_cb()
{
    fly_flag = !fly_flag;
}

static void win_cb(void *p)
{
    int millisecond = 0;
#ifdef __WIN32
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    uint16_t seconds = timeinfo->tm_sec;
    uint16_t minute = timeinfo->tm_min;
    uint16_t hour = timeinfo->tm_hour;
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    millisecond = spec.tv_nsec / 1000000;
#else
    // extern struct tm watch_clock_get(void);
    // extern uint16_t millisecond_cnt;
    // struct tm watch_time = watch_clock_get();
    // millisecond = millisecond_cnt;
    // uint16_t seconds = watch_time.tm_sec;
    // uint16_t minute = watch_time.tm_min;
    // uint16_t hour = watch_time.tm_hour;

    uint16_t seconds = 0;
    uint16_t minute = 0;
    uint16_t hour = 0;
#endif
    const float angle_per_second = 360.0f / 60.0f;
    gui_img_t *hand;
    {
        hand = h_hand;
        float angle_hour = (hour % 12) * M_PI / 6 + minute * M_PI / 360;
        gui_img_rotation(hand, angle_hour * 180 / M_PI);
    }
    {
        hand = m_hand;
        float angle_min  = minute * M_PI / 30 + seconds * M_PI / 1800;
        gui_img_rotation(hand, angle_min * 180 / M_PI);
    }
    {
        hand = s_hand;
        float angle_sec = (seconds + millisecond / 1000.0f) * angle_per_second;
        gui_img_rotation(hand, angle_sec);
        // gui_log("seconds: %d, millisecond: %d \n", seconds, millisecond);
    }
}

static void update_animation()
{
    frame_counter++;

    if (fly_flag)
    {
        if (radius < 20.0f)
        {
            radius += 0.5f;
        }
        theta += 0.01f;
    }
    else
    {
        if (radius > 0)
        {
            radius -= 0.5f;
            theta += 0.01f;
        }
    }
    wing_angle = 50.0f * sinf(frame_counter * 0.1f);
    butterfly_x = radius * cosf(theta);
    butterfly_y = radius * sinf(theta);
    butterfly_z = 10.0f * sinf(theta * 5.0f);
    butterfly_rz = theta * (180.0f / M_PI);
}

static void watchface_butterfly_global_cb(gui_3d_t *this)
{
    gui_dispdev_t *dc = gui_get_dc();

    gui_3d_camera_UVN_initialize(&this->camera, gui_point_4d(0, 0, 80), gui_point_4d(0, 0, 0), 1, 32767,
                                 90, this->base.w, this->base.h);

    gui_3d_world_inititalize(&this->world, butterfly_x, butterfly_y, butterfly_z, 0, 0, butterfly_rz,
                             5);

}

static gui_3d_matrix_t watchface_butterfly_face_cb(gui_3d_t *this, size_t face_index/*face offset*/)
{
    gui_3d_matrix_t face_matrix;
    gui_3d_matrix_t transform_matrix;

    if (face_index == 0)
    {
        gui_3d_calculator_matrix(&face_matrix, 0, 0, 0, gui_3d_point(0, 0, 0), gui_3d_vector(0, 1, 0),
                                 wing_angle, 1);
    }
    else if (face_index == 1)
    {
        gui_3d_calculator_matrix(&face_matrix, 0, 0, 0, gui_3d_point(0, 0, 0), gui_3d_vector(0, 1, 0),
                                 -wing_angle, 1);
    }
    else if (face_index == 2)
    {
        gui_3d_calculator_matrix(&face_matrix, 0, 0, 0, gui_3d_point(0, 0, 0), gui_3d_vector(0, 1, 0),
                                 wing_angle, 1);
    }
    else if (face_index == 3)
    {
        gui_3d_calculator_matrix(&face_matrix, 0, 0, 0, gui_3d_point(0, 0, 0), gui_3d_vector(0, 1, 0),
                                 -wing_angle, 1);
    }
    else
    {
        gui_3d_calculator_matrix(&face_matrix, 0, 0, 0, gui_3d_point(0, 0, 0), gui_3d_vector(0, 1, 0), 0,
                                 1);
    }

    transform_matrix = gui_3d_matrix_multiply(face_matrix, this->world);

    return transform_matrix;

}



void watchface_butterfly_app(gui_view_t *view)
{
    gui_obj_t *obj = GUI_BASE(view);
    // gui_obj_create_timer(obj, 10, true, return_timer_cb);
    gui_win_t *win = gui_win_create(obj, "win_wf", 0, 0, 0, 0);
    gui_obj_create_timer(GUI_BASE(win), 17, true, win_cb);
    gui_obj_start_timer(GUI_BASE(win));

    gui_img_t *img = gui_img_create_from_mem(win, "mask", W1ELLIPSE5_BIN, 204, 246, 0, 0);
    h_hand = gui_img_create_from_mem(win, "h_hand", W1UNION2_BIN, 200 + 7, 249, 0,
                                     0); // 246 + 3(mask_h)
    m_hand = gui_img_create_from_mem(win, "m_hand", W1UNION_BIN, 200 + 6, 249, 0, 0);
    s_hand = gui_img_create_from_mem(win, "s_hand", W1VECTOR7_BIN, 206, 249, 0, 0);
    gui_img_set_focus(h_hand, m_hand->base.w / 2, 249 - 165); // img target y is 165
    gui_img_set_focus(m_hand, m_hand->base.w / 2, 249 - 84);
    gui_img_set_focus(s_hand, 0, 249 - 120);

    gui_3d_t *watchface_butterfly_3d = gui_3d_create(win, "3d-widget", DESC_BUTTERFLY_BIN,
                                                     GUI_3D_DRAW_FRONT_ONLY, 0, 0, 410, 502);

    gui_3d_set_global_transform_cb(watchface_butterfly_3d,
                                   (gui_3d_global_transform_cb)watchface_butterfly_global_cb);
    gui_3d_set_face_transform_cb(watchface_butterfly_3d,
                                 (gui_3d_face_transform_cb)watchface_butterfly_face_cb);

    gui_obj_create_timer(&(watchface_butterfly_3d->base), 17, true, update_animation);
    gui_obj_start_timer(&(watchface_butterfly_3d->base));


    gui_win_click(win, (gui_event_cb_t)win_touch_cb, NULL);
}
#endif



