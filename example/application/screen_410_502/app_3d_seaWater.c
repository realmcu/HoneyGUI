/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "guidef.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_gray.h"
#include "gui_obj.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "gui_view.h"
#include "gui_canvas.h"
#include "gui_3d.h"
#include "gui_matrix.h"
#include "tp_algo.h"
#include "root_image/ui_resource.h"
#include "app_main_watch.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "seawater_view"
#define ROTATION_INCREMENT 0.5f
#define POSITION_INCREMENT 2.0f
#define CAN_COUNT 4
#define NUM_CAN_TIME_PAIRS 4
#define WAVE_DURATION_MS 5000
#define MAX_BUBBLES 20

/*============================================================================*
*                             Types
*============================================================================*/
typedef struct
{
    gui_img_t *img;
    float scale;
    int start_x;
    int start_y;
    uint32_t create_time;
} Bubble;

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void app_ui_wave_nums_design(gui_view_t *view);
static void cleanup_resources(gui_view_t *view);

/*============================================================================*
 *                            Variables
 *============================================================================*/
/* View Management */
static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *menu_view = NULL;
static const gui_view_descriptor_t descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,

    .on_switch_in = app_ui_wave_nums_design,
    .on_switch_out = cleanup_resources,

    .keep = false,
};

/* Gui Obj */
static gui_canvas_t *canvas_wave = NULL;
static gui_win_t *can0_window = NULL;
static gui_win_t *can1_window = NULL;
static gui_win_t *can2_window = NULL;
static gui_win_t *can3_window = NULL;
static gui_win_t *bubble_window = NULL;

/* Animate Param */
static float fish_angle = 0.0f;
static float fish_time = 0.0f;
static bool wave_active = false;
static uint32_t wave_start_time = 0;

static float touch_x = -1;
static float touch_time = 0;
static float wave_alpha = 0.0f;
static float can_positions[CAN_COUNT] = {55, 140, 225, 315};
static float can_wave_y[CAN_COUNT] = {0}; // Wave height at each can position
static float can_current_pos[CAN_COUNT] = {0}; // Current can positions
static float can_rotation_targets[NUM_CAN_TIME_PAIRS] = {-10, 4, -5, 5};
static float can_current_rotation[NUM_CAN_TIME_PAIRS] = {0};

static int16_t fish_x[4] = {-70, 70, -50, -45};
static int16_t fish_y[4] = {50, 40, 60, 45};

static const void *display_time_resource_def[] =
{
    NUM0_BIN,
    NUM1_BIN,
    NUM2_BIN,
    NUM3_BIN,
    NUM4_BIN,
    NUM5_BIN,
    NUM6_BIN,
    NUM7_BIN,
    NUM8_BIN,
    NUM9_BIN,
};

/* Bubble */
static Bubble *bubbles = NULL;
static int active_bubble_count = 0;
static uint32_t fish_last_bubble_time[4] = {0};
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
static void return_to_menu()
{
    gui_view_switch_direct(current_view, menu_view, SWITCH_OUT_ANIMATION_FADE,
                           SWITCH_IN_ANIMATION_FADE);
}

static void gui_wave_draw_graph(gui_canvas_t *canvas)
{
    NVGcontext *vg = canvas->vg;
    float y_base = 245;
    float w = 410;
    float h = 30;
    float t = gui_ms_get() * 0.001f;

    // Handle touch
    touch_info_t *tp = tp_get_info();
    if (tp->pressed)
    {
        touch_x = tp->x;
        touch_time = t;
        wave_alpha = 1.0f;
    }

    // Fade out wave
    if (wave_alpha > 0)
    {
        float elapsed = t - touch_time;
        wave_alpha = expf(-elapsed * 0.5f);
        if (wave_alpha < 0.01f)
        {
            wave_alpha = 0.0f;
            touch_x = -1;
        }
    }

    // Calculate wave only at can positions
    for (int i = 0; i < CAN_COUNT; i++)
    {
        float xi = can_positions[i];
        // Base wave
        float base_wave = 0.4f * sinf(xi * 0.05f + t * 1.5f)
                          + 0.2f * sinf(xi * 0.1f + t * 2.0f);

        // Touch effect
        float touch_effect = 0;
        if (touch_x >= 0)
        {
            float distance = fabsf(xi - touch_x);
            if (distance < 50)
            {
                float decay_factor = expf(-distance * 0.02f);
                float time_decay = expf(-(t - touch_time) * 2.0f);
                touch_effect = decay_factor * 0.8f * time_decay;
            }
        }

        can_wave_y[i] = y_base - h * fmaxf(0, fminf(1, base_wave * 0.4f + 0.5f + touch_effect));
    }

    // Draw wave (optional, can be removed if not needed)
    if (wave_alpha > 0)
    {
        NVGpaint bg = nvgLinearGradient(vg, 0, y_base - h, 0, y_base,
                                        nvgRGBA(0, 160, 192, (int)(50 * wave_alpha)),
                                        nvgRGBA(0, 160, 192, (int)(200 * wave_alpha)));

        nvgBeginPath(vg);
        // Simplified drawing - just connect the can positions
        for (int i = 0; i < CAN_COUNT; i++)
        {
            if (i == 0) { nvgMoveTo(vg, can_positions[i], can_wave_y[i]); }
            else { nvgLineTo(vg, can_positions[i], can_wave_y[i]); }
        }
        nvgLineTo(vg, w, y_base);
        nvgLineTo(vg, 0, y_base);
        nvgFillPaint(vg, bg);
        nvgFill(vg);
    }
}

static void wave_animate_cb()
{
    touch_info_t *tp = tp_get_info();
    uint32_t current_time = gui_ms_get();

    if (tp->pressed)
    {
        wave_active = true;
        wave_start_time = current_time;
        gui_canvas_set_canvas_cb(canvas_wave, gui_wave_draw_graph);
    }

    if (wave_active && (current_time - wave_start_time > WAVE_DURATION_MS))
    {
        wave_active = false;
        gui_canvas_set_canvas_cb(canvas_wave, NULL);
    }
}

static void animate_cans_based_on_wave()
{
    float y_base = 230;

    for (int j = 0; j < CAN_COUNT; j++)
    {
        if (can_wave_y[j] == 0 || wave_alpha == 0)
        {
            continue;
        }

        float target_pos = (y_base - can_wave_y[j]) * 3.0f; // Scale wave height to position

        // Smooth transition
        if (can_current_pos[j] < target_pos)
        {
            can_current_pos[j] += POSITION_INCREMENT;
            if (can_current_pos[j] > target_pos)
            {
                can_current_pos[j] = target_pos;
            }
        }
        else if (can_current_pos[j] > target_pos)
        {
            can_current_pos[j] -= POSITION_INCREMENT;
            if (can_current_pos[j] < target_pos)
            {
                can_current_pos[j] = target_pos;
            }
        }
    }
}

static void can_update_param_cb()
{
    touch_info_t *tp = tp_get_info();
    static uint32_t last_bubble_time = 0;

    if (tp->pressed)
    {
        for (int i = 0; i < NUM_CAN_TIME_PAIRS; i++)
        {
            float rotation = ((rand() % 11) - 5) / 1.0f;
            can_rotation_targets[i] = rotation;
        }
    }

    // Update can positions
    animate_cans_based_on_wave();

    // Update rotations
    for (int i = 0; i < NUM_CAN_TIME_PAIRS; i++)
    {
        if (can_current_rotation[i] < can_rotation_targets[i])
        {
            can_current_rotation[i] += ROTATION_INCREMENT;
            if (can_current_rotation[i] > can_rotation_targets[i])
            {
                can_current_rotation[i] = can_rotation_targets[i];
            }
        }
        else if (can_current_rotation[i] > can_rotation_targets[i])
        {
            can_current_rotation[i] -= ROTATION_INCREMENT;
            if (can_current_rotation[i] < can_rotation_targets[i])
            {
                can_current_rotation[i] = can_rotation_targets[i];
            }
        }
    }
}

static void init_bubble_pool()
{
    bubbles = gui_malloc(sizeof(Bubble) * MAX_BUBBLES);
    memset(bubbles, 0, sizeof(Bubble) * MAX_BUBBLES);
    active_bubble_count = 0;
}

static void create_bubble(int x, int y)
{
    if (active_bubble_count >= MAX_BUBBLES) { return; }

    for (int i = 0; i < MAX_BUBBLES; i++)
    {
        Bubble *bubble = bubbles + i;
        if (!bubble->img || bubble->img->base.not_show)
        {
            if (!bubble->img)
            {
                bubble->img = gui_img_create_from_mem(bubble_window, "bubble", DROPLET_BIN, x, y, 0, 0);
                if (!bubble->img) { return; }
            }
            else
            {
                bubble->img->base.x = x;
                bubble->img->base.y = y;
                bubble->img->base.not_show = false;
            }

            bubble->scale = 0.3f + (rand() % 7) / 20.0f;
            bubble->img->opacity_value = 150 + rand() % 105;
            bubble->start_x = x;
            bubble->start_y = y;
            bubble->create_time = gui_ms_get();

            gui_img_scale(bubble->img, bubble->scale, bubble->scale);
            gui_img_set_opacity(bubble->img, bubble->img->opacity_value);

            active_bubble_count++;
            break;
        }
    }
}

static void update_all_bubbles()
{
    uint32_t current_time = gui_ms_get();

    for (int i = 0; i < MAX_BUBBLES; i++)
    {
        Bubble *bubble = bubbles + i;
        if (bubble->img && !bubble->img->base.not_show)
        {
            uint32_t elapsed = current_time - bubble->create_time;

            int y_offset = (int)(elapsed / 20.0f);
            bubble->img->base.y = bubble->start_y - y_offset;
            bubble->img->base.x = bubble->start_x + 5 * sinf(elapsed / 200.0f);
            bubble->scale += 0.002f;
            bubble->img->opacity_value = 255 - (elapsed / 10);

            gui_img_scale(bubble->img, bubble->scale, bubble->scale);
            gui_img_set_opacity(bubble->img,
                                bubble->img->opacity_value > 0 ? bubble->img->opacity_value : 0);

            if (elapsed > 2500 || bubble->img->opacity_value <= 0)
            {
                bubble->img->base.not_show = true;
                active_bubble_count--;
            }
        }
    }
}

static void cleanup_resources(gui_view_t *view)
{
    gui_free(bubbles);
    active_bubble_count = 0;
}

static int fish_x_to_screen(float fish_x)
{
    float a = 80.0f;   //left fish's max x (most left)
    float b = -90.0f;  //right fish's min x (most right)
    float c = 0.0f;    //set the screen's left edge to 0
    float d = 410.0f;  // set the screen's right edge to 410

    float screen_x = c + ((fish_x - a) / (b - a)) * (d - c);

    if (screen_x < 0) { screen_x = 0; }
    if (screen_x > 410) { screen_x = 410; }

    return (int)screen_x;
}

static void fish_animate_cb()
{
    touch_info_t *tp = tp_get_info();

    float fish_pos_x[4];

    if (tp->released)
    {
        for (int i = 0; i < 4; i++)
        {
            float pos = rand() % 161;;
            fish_pos_x[i] = pos;
        }
        if (fish_x[0] > 90)
        {
            // uint16_t i = rand() % 161; //-90~70
            fish_x[0] = 70 - fish_pos_x[0]; // -90~70
            // -50~-30
        }
        if (fish_x[1] < -80)
        {
            // uint16_t i = rand() % 161; //-90~70
            fish_x[1] = fish_pos_x[1] - 70; // -90~70
        }
        if (fish_x[2] > 90)
        {
            fish_x[2] = 70 - fish_pos_x[2]; // -90~70
        }
        if (fish_x[3] > 90)
        {
            fish_x[3] = 70 - fish_pos_x[3]; // -90~70
        }

    }
    else
    {
        uint32_t current_time = gui_ms_get();
        if ((current_time - fish_last_bubble_time[0] > 200) && (fish_x[0] < 70))
        {
            create_bubble(fish_x_to_screen(fish_x[0]), (fish_y[0] + 80));
            fish_last_bubble_time[0] = current_time;
        }

        if (current_time - fish_last_bubble_time[1] > 400)
        {
            create_bubble(fish_x_to_screen(fish_x[1]), (fish_y[1] + 80));
            fish_last_bubble_time[1] = current_time;
        }

        if (current_time - fish_last_bubble_time[2] > 600)
        {
            create_bubble(fish_x_to_screen(fish_x[2]), (fish_y[2] + 80));
            fish_last_bubble_time[2] = current_time;
        }

        if (current_time - fish_last_bubble_time[3] > 800)
        {
            create_bubble(fish_x_to_screen(fish_x[3]), (fish_y[3] + 80));
            fish_last_bubble_time[3] = current_time;
        }
        {
            fish_x[0] += 1;
            fish_time += 0.1f;
            fish_angle = 10.0f * sinf(fish_time);
        }
        {
            fish_x[1] -= 1;
            fish_time += 0.1f;
            fish_angle = 10.0f * sinf(fish_time);
        }
        {
            fish_x[2] += 1.2f;
            fish_time += 0.1f;
            fish_angle = 10.0f * sinf(fish_time);
        }
        {
            fish_x[3] += 1;
            fish_time += 0.1f;
            fish_angle = 10.0f * sinf(fish_time);
        }
    }
    update_all_bubbles();
}

static void can_animate_cb(void *p)
{
    gui_obj_t *obj = (gui_obj_t *)p;
    uint8_t index = 0;
    if ((gui_win_t *)obj == can0_window) ;
    else if ((gui_win_t *)obj == can1_window) {index = 1;}
    else if ((gui_win_t *)obj == can2_window) {index = 2;}
    else if ((gui_win_t *)obj == can3_window) {index = 3;}
    matrix_translate(0, can_current_pos[index], obj->matrix);
    matrix_translate(-obj->w / 2, -obj->h / 2, obj->matrix);
    matrix_rotate(can_current_rotation[index], obj->matrix);
    matrix_translate(obj->w / 2, obj->h / 2, obj->matrix);
}

static void app_animate_cb(void *p)
{
    fish_animate_cb();
    wave_animate_cb();
    can_update_param_cb();
}

static void fish_global_cb(gui_3d_t *this)
{
    gui_dispdev_t *dc = gui_get_dc();
    static uint8_t index = 0;

    gui_3d_camera_UVN_initialize(&this->camera, gui_point_4d(0, 0, 0), gui_point_4d(0, 0, 80), 1, 32767,
                                 90, this->base.w, this->base.h);

    gui_3d_world_inititalize(&this->world, -fish_x[index], fish_y[index], 80, 0, 0, 0,
                             5);
    index++;
    index %= 4;
}

static gui_3d_matrix_t fish_face_cb(gui_3d_t *this, size_t face_index/*face offset*/)
{
    gui_3d_matrix_t face_matrix;
    gui_3d_matrix_t transform_matrix;

    if (face_index == 0)
    {
        gui_3d_calculator_matrix(&face_matrix, 0, 0, 0, gui_3d_point(0, 0, 0), gui_3d_vector(0, 1, 0),
                                 fish_angle, 1);
    }

    transform_matrix = gui_3d_matrix_multiply(face_matrix, this->world);

    return transform_matrix;
}

static void app_ui_wave_nums_design(gui_view_t *view)
{
    srand((uint32_t)gui_ms_get());
    gui_view_switch_on_event(view, menu_view, SWITCH_OUT_ANIMATION_FADE,
                             SWITCH_IN_ANIMATION_FADE,
                             GUI_EVENT_KB_SHORT_CLICKED);

    gui_img_create_from_mem(view, "background", SEABACKGROUND_BIN, 0, 0, 0, 0);

    gui_3d_t *fish0 = gui_3d_create(view, "3d-fish0", DESC_SEAFISH0_BIN, GUI_3D_DRAW_FRONT_ONLY, 0, 0,
                                    410, 502);
    gui_3d_t *fish1 = gui_3d_create(view, "3d-fish1", DESC_SEAFISH1_BIN, GUI_3D_DRAW_FRONT_ONLY, 0, 0,
                                    410, 502);
    gui_3d_t *fish2 = gui_3d_create(view, "3d-fish2", DESC_SEAFISH2_BIN, GUI_3D_DRAW_FRONT_ONLY, 0, 0,
                                    410, 502);
    gui_3d_t *fish3 = gui_3d_create(view, "3d-fish3", DESC_SEAFISH3_BIN, GUI_3D_DRAW_FRONT_ONLY, 0, 0,
                                    410, 502);

    gui_3d_set_global_transform_cb(fish0, (gui_3d_global_transform_cb)fish_global_cb);
    gui_3d_set_face_transform_cb(fish0, (gui_3d_face_transform_cb)fish_face_cb);
    gui_3d_set_global_transform_cb(fish1, (gui_3d_global_transform_cb)fish_global_cb);
    gui_3d_set_face_transform_cb(fish1, (gui_3d_face_transform_cb)fish_face_cb);
    gui_3d_set_global_transform_cb(fish2, (gui_3d_global_transform_cb)fish_global_cb);
    gui_3d_set_face_transform_cb(fish2, (gui_3d_face_transform_cb)fish_face_cb);
    gui_3d_set_global_transform_cb(fish3, (gui_3d_global_transform_cb)fish_global_cb);
    gui_3d_set_face_transform_cb(fish3, (gui_3d_face_transform_cb)fish_face_cb);

    can0_window = gui_win_create(view, "can_window", 70, 190, 64, 116);
    gui_img_t *can0 = gui_img_create_from_mem(can0_window, "can0", CANSKIN_PURPLE_BIN, 0, 0, 0, 0);
    gui_img_t *time0 = gui_img_create_from_mem(can0_window, "time0",
                                               (void *)display_time_resource_def[2], 16,
                                               32, 0, 0);


    can1_window = gui_win_create(view, "can_window", 155, 190, 64, 116);
    gui_img_t *can1 = gui_img_create_from_mem(can1_window, "can1", CANSKIN_YELLOW_BIN, 0, 0, 0, 0);
    gui_img_t *time1 = gui_img_create_from_mem(can1_window, "can1",
                                               (void *)display_time_resource_def[0], 16,
                                               32, 0, 0);

    can2_window = gui_win_create(view, "can_window", 240, 190, 64, 116);
    gui_img_t *can2 = gui_img_create_from_mem(can2_window, "can2", CANSKIN_GREEN_BIN, 0, 0, 0, 0);
    gui_img_t *time2 = gui_img_create_from_mem(can2_window, "can2",
                                               (void *)display_time_resource_def[2], 16,
                                               32, 0, 0);

    can3_window = gui_win_create(view, "can_window", 330, 190, 64, 116);
    gui_img_t *can3 = gui_img_create_from_mem(can3_window, "can3", CANSKIN_RED_BIN, 0, 0, 0, 0);
    gui_img_t *time3 = gui_img_create_from_mem(can3_window, "can3",
                                               (void *)display_time_resource_def[5], 16,
                                               32, 0, 0);

    wave_active = false;
    gui_img_t *waterface_bg = gui_img_create_from_mem(view, "waterface", WATERFACTOR_BG_BIN, 0, 240, 0,
                                                      244);
    gui_img_set_mode(waterface_bg, IMG_SRC_OVER_MODE);
    gui_img_set_opacity(waterface_bg, 100);

    canvas_wave = gui_canvas_create(view, "canvas", 0, 0, 0, 410, 502);

    //bubble
    bubble_window = gui_win_create(view, "bubble_window", 0, 240, 410, 240);
    init_bubble_pool();

    gui_obj_create_timer(&(view->base), 10, true, app_animate_cb);
    gui_obj_start_timer(&(view->base));
    gui_obj_create_timer(&(can0_window->base), 10, true, can_animate_cb);
    gui_obj_start_timer(&(can0_window->base));
    gui_obj_create_timer(&(can1_window->base), 10, true, can_animate_cb);
    gui_obj_start_timer(&(can1_window->base));
    gui_obj_create_timer(&(can2_window->base), 10, true, can_animate_cb);
    gui_obj_start_timer(&(can2_window->base));
    gui_obj_create_timer(&(can3_window->base), 10, true, can_animate_cb);
    gui_obj_start_timer(&(can3_window->base));
}
