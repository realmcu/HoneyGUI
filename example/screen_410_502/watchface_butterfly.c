#include "guidef.h"
#include "gui_img.h"
#include "gui_tabview.h"
#include "gui_tab.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include <gui_app.h>
#include "gui_server.h"
#include "gui_components_init.h"
#include "gui_canvas.h"

#include "gui_3d.h"
#include "math.h"

#include <time.h>
#include "root_image_hongkong/ui_resource.h"
#include "app_hongkong.h"

#define SCREEN_WIDTH 410
#define SCREEN_HEIGHT 502

// #define CURRENT_VIEW_NAME "watchface_butterfly_view"

// static gui_view_t *current_view = NULL;
// const static gui_view_descriptor_t *app_down_view = NULL;
// const static gui_view_descriptor_t *app_up_view = NULL;
// const static gui_view_descriptor_t *activity_view = NULL;
// const static gui_view_descriptor_t *watchface_select_view = NULL;
// void create_watchface_bf(gui_view_t *view);

// static gui_view_descriptor_t const descriptor =
// {
//     /* change Here for current view */
//     .name = (const char *)CURRENT_VIEW_NAME,
//     .pView = &current_view,
//     .design_cb = create_watchface_bf,
// };

// static int gui_view_descriptor_register_init(void)
// {
//     gui_view_descriptor_register(&descriptor);
//     gui_log("File: %s, Function: %s\n", __FILE__, __func__);
//     return 0;
// }
// static GUI_INIT_VIEW_DESCRIPTOR_REGISTER(gui_view_descriptor_register_init);

// static int gui_view_get_other_view_descriptor_init(void)
// {
//     /* you can get other view descriptor point here */
//     app_down_view = gui_view_descriptor_get("app_down_view");
//     app_up_view = gui_view_descriptor_get("app_up_view");
//     activity_view = gui_view_descriptor_get("activity_view");
//     watchface_select_view  = gui_view_descriptor_get("watchface_select_view");
//     gui_log("File: %s, Function: %s\n", __FILE__, __func__);
//     return 0;
// }
// static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);


static float radius = 20.0f, theta = 0;
static int frame_counter = 0;
static float wing_angle = 0.0f;
static float butterfly_x = 0.0f;
static float butterfly_y = 0.0f;
static float butterfly_z = 0.0f;
static float butterfly_rz = 0.0f;
static bool fly_flag = 1;

static gui_img_t *h_hand, *m_hand, *s_hand;

static void callback_touch_win()
{
    fly_flag = !fly_flag;
}

static void win_cb(gui_obj_t *obj)
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
        gui_img_translate(hand, hand->base.w / 2, SCREEN_HEIGHT / 2 - hand->base.y);
        gui_img_rotation(hand, angle_hour * 180 / M_PI,  hand->base.w / 2,
                         SCREEN_HEIGHT / 2 - hand->base.y);
    }
    {
        hand = m_hand;
        float angle_min  = minute * M_PI / 30 + seconds * M_PI / 1800;
        gui_img_translate(hand, hand->base.w / 2, SCREEN_HEIGHT / 2 - hand->base.y);
        gui_img_rotation(hand, angle_min * 180 / M_PI, hand->base.w / 2,
                         SCREEN_HEIGHT / 2 - hand->base.y);
    }
    {
        hand = s_hand;
        float angle_sec = (seconds + millisecond / 1000.0f) * angle_per_second;
        gui_img_translate(hand, hand->base.w / 2, SCREEN_HEIGHT / 2 - hand->base.y);
        gui_img_rotation(hand, angle_sec,  hand->base.w / 2,
                         SCREEN_HEIGHT / 2 - hand->base.y);
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

static void cb(void *this, size_t face_index, gui_3d_world_t *world,
               gui_3d_camera_t *camera, gui_3d_light_t *light)
{
    gui_dispdev_t *dc = gui_get_dc();
    gui_3d_matrix_t face_matrix;
    gui_3d_matrix_t object_matrix;

    gui_3d_camera_UVN_initialize(camera, gui_point_4d(0, 0, 50), gui_point_4d(0, 0, 0), 1, 32767, 90,
                                 dc->screen_width, dc->screen_height);

    gui_3d_world_inititalize(&object_matrix, butterfly_x, butterfly_y, butterfly_z, 0, 0,
                             butterfly_rz + 90,
                             5);
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

    *world = gui_3d_matrix_multiply(face_matrix, object_matrix);

}



void create_watchface_bf(gui_view_t *view)
{
    // gui_view_switch_on_event(view, app_down_view, VIEW_STILL, VIEW_TRANSPLATION,
    //                           GUI_EVENT_TOUCH_MOVE_UP);
    // gui_view_switch_on_event(view, app_up_view, VIEW_STILL, VIEW_TRANSPLATION,
    //                           GUI_EVENT_TOUCH_MOVE_DOWN);
    // gui_view_switch_on_event(view, activity_view, VIEW_CUBE, VIEW_CUBE,
    //                           GUI_EVENT_TOUCH_MOVE_LEFT);
    // gui_view_switch_on_event(view, watchface_select_view, VIEW_ANIMATION_8, VIEW_ANIMATION_5,
    //                           GUI_EVENT_TOUCH_LONG);

    gui_obj_t *obj = GUI_BASE(view);
    gui_win_t *win = gui_win_create(obj, "win_wf_ring", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    gui_win_set_animate(win, 2000, -1, (gui_animate_callback_t)win_cb, win);

    void *test_3d = gui_3d_create(obj, "3d-widget", DESC_BUTTERFLY_BIN, 0, 0, 410, 502);

    gui_3d_set_local_shape_transform_cb(test_3d, 0, (gui_3d_shape_transform_cb)cb);

    gui_obj_set_timer(&(((gui_3d_base_t *)test_3d)->base), 1000, true, update_animation);

    gui_img_t *img = gui_img_create_from_mem(win, "mask", W1ELLIPSE5_BIN, 204, 246, 0, 0);
    h_hand = gui_img_create_from_mem(win, "h_hand", W1UNION2_BIN, 199, 170, 0, 0);
    m_hand = gui_img_create_from_mem(win, "m_hand", W1UNION_BIN, 200, 90, 0, 0);
    s_hand = gui_img_create_from_mem(win, "s_hand", W1VECTOR7_BIN, 206, 124, 0, 0);

    gui_win_click(win, (gui_event_cb_t)callback_touch_win, NULL);
}



