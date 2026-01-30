/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

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
#include "cJSON.h"
#include "app_main_watch.h"
#include "gui_view.h"
#include "gui_list.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "rainbow_digital_view"
#define DOT_NUM 15

/*============================================================================*
 *                           Types
 *============================================================================*/
typedef struct
{
    float dx;  // X-axis offset
    float dy;  // Y-axis offset
} LayerOffset;

typedef struct
{
    gui_img_t *layers[5];  // 5 image objects for layers
    int x_base;            // Base X coordinate of the main layer
    int y_base;            // Base Y coordinate of the main layer
} RainbowDigit;

typedef struct
{
    LayerOffset current_offsets[5];       // Current offsets
    LayerOffset target_offsets[5];        // Target offsets
    int move_direction;                   // Shared movement direction
} SharedAnimationState;

typedef struct
{
    gui_img_t *img;
    float drift_x;
    float drift_y;
    int direction_x;
    int direction_y;
    bool is_opacity_increase;
} Dot;

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void rainbow_digital_app(gui_view_t *view);

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
    .on_switch_in = rainbow_digital_app,
};

/* Digit Layer Offsets */
static const LayerOffset DEFAULT_LAYER_OFFSETS[5] =
{
    {0.0f,  0.0f},   // layer[0] Main layer fixed
    {0.0f,  -3.0f},  // layer[1]
    {-3.0f, 0.0f},   // layer[2]
    {3.0f,  3.0f},   // layer[3]
    {0.0f,  0.0f}    // layer[4]
};
static const LayerOffset MAX_OFFSET_LIMITS[5] =
{
    {0.0f,  0.0f},   // layer[0] Placeholder unused
    {10.0f, 10.0f},  // layer[1] Maximum offset limit
    {20.0f, 20.0f},  // layer[2]
    {30.0f, 30.0f},  // layer[3]
    {40.0f, 40.0f}   // layer[4]
};

/* Dot Image Binaries */
static void *dot_img_bin[5] = {0};

/* Create Digits and Dots */
static RainbowDigit digits[4];
static SharedAnimationState shared_state;
static Dot dots[DOT_NUM];

/* Animation Variables */
static const float damping = 0.1f;      // Damping coefficient
static bool is_animating = false; // Animation state
static int reached_target_count = 0; // Counter for layers reaching the target

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


// Initialize a single digit
static void init_digit(RainbowDigit *digit, gui_win_t *win, uint8_t *img_bin[5], int x_base,
                       int y_base)
{
    digit->x_base = x_base;
    digit->y_base = y_base;

    for (int i = 4; i >= 0; i--)
    {
        digit->layers[i] = gui_img_create_from_mem(win, 0, img_bin[i],
                                                   x_base + DEFAULT_LAYER_OFFSETS[i].dx,
                                                   y_base + DEFAULT_LAYER_OFFSETS[i].dy, 0, 0);
    }
}


static void update_shared_offsets(float deltaX, float deltaY)
{
    shared_state.move_direction = 1;
    for (int i = 1; i < 5; i++)
    {
        // Dynamic offset = Default offset + touch delta * layer factor
        shared_state.current_offsets[i].dx = DEFAULT_LAYER_OFFSETS[i].dx + (deltaX * 0.2f * i);
        shared_state.current_offsets[i].dy = DEFAULT_LAYER_OFFSETS[i].dy + (deltaY * 0.2f * i);

        // Limit maximum offset
        shared_state.current_offsets[i].dx = fmaxf(-MAX_OFFSET_LIMITS[i].dx, fminf(MAX_OFFSET_LIMITS[i].dx,
                                                                                   shared_state.current_offsets[i].dx));
        shared_state.current_offsets[i].dy = fmaxf(-MAX_OFFSET_LIMITS[i].dy, fminf(MAX_OFFSET_LIMITS[i].dy,
                                                                                   shared_state.current_offsets[i].dy));

        shared_state.target_offsets[i].dx = -shared_state.current_offsets[i].dx;
        shared_state.target_offsets[i].dy = -shared_state.current_offsets[i].dy;
    }

    shared_state.target_offsets[0].dx = -shared_state.current_offsets[1].dx / 2.0f;
    shared_state.target_offsets[0].dy = -shared_state.current_offsets[1].dy / 2.0f;
}

static void apply_four_layer_offsets(void)
{
    for (int d = 0; d < 4; d++)
    {
        for (int i = 1; i < 5; i++)
        {
            gui_img_translate(digits[d].layers[i], shared_state.current_offsets[i].dx,
                              shared_state.current_offsets[i].dy);
        }
    }
}

static void apply_five_layer_offsets(void)
{
    for (int d = 0; d < 4; d++)
    {
        for (int i = 0; i < 5; i++)
        {
            gui_img_translate(digits[d].layers[i], shared_state.current_offsets[i].dx,
                              shared_state.current_offsets[i].dy);
        }
    }
}


static void reset_shared_offsets(void)
{
    reached_target_count = 0;
    for (int i = 0; i < 5; i++)
    {
        float damping_effect = expf(-damping);

        LayerOffset *current_offset = &shared_state.current_offsets[i];
        LayerOffset *target_offset = &shared_state.target_offsets[i];

        // Calculate the difference for update
        float dx_diff = target_offset->dx - current_offset->dx;
        float dy_diff = target_offset->dy - current_offset->dy;

        // Adjust the offset values of the layer
        current_offset->dx += dx_diff;
        current_offset->dy += dy_diff;

        if (fabsf(dx_diff) < 0.1f && fabsf(dy_diff) < 0.1f)
        {
            shared_state.move_direction = -shared_state.move_direction; // Switch direction for elastic effect
            target_offset->dx = shared_state.move_direction * target_offset->dx * damping_effect;
            target_offset->dy = shared_state.move_direction * target_offset->dy * damping_effect;
        }

        if (fabsf(current_offset->dx) < 0.2f && fabsf(current_offset->dy) < 0.2f)
        {
            reached_target_count++;
            // If close to the default offset, reset to default offset
            current_offset->dx = 0.0f;
            current_offset->dy = 0.0f;
            shared_state.move_direction = 1; // Reset direction
        }
    }

    apply_five_layer_offsets();

    // Check if all layers have reached the target position
    if (reached_target_count >= 5)
    {
        is_animating = false;
    }

}

static void number_animation_cb(void *param)
{
    (void)param;
    touch_info_t *tp = tp_get_info();
    // gui_win_t *clock_win = (gui_win_t *)param;

    if (tp->pressed || tp->pressing)
    {
        update_shared_offsets(tp->deltaX, tp->deltaY);
        apply_four_layer_offsets();
        is_animating = false;
    }
    else if (tp->released)
    {
        is_animating = true;
    }

    if (is_animating)
    {
        // On release: Elastic reset for all digit positions
        reset_shared_offsets();
    }
}

static void dot_animation_cb(void *param)
{
    (void)param;
    for (int i = 0; i < DOT_NUM; i++)
    {
        if (dots[i].is_opacity_increase)
        {
            dots[i].img->opacity_value++;
            if (dots[i].img->opacity_value >= 255)
            {
                dots[i].is_opacity_increase = false;
            }
        }
        else
        {
            dots[i].img->opacity_value--;
            if (dots[i].img->opacity_value <= 0)
            {
                dots[i].is_opacity_increase = true;
            }
        }

        dots[i].drift_x += dots[i].direction_x * 0.5f;
        dots[i].drift_y += dots[i].direction_y * 0.5f;

        if (dots[i].drift_x < -100 || dots[i].drift_x > gui_get_dc()->screen_width ||
            dots[i].drift_y < -100 || dots[i].drift_y > gui_get_dc()->screen_height)
        {
            dots[i].img->opacity_value = 0; // Reset opacity
            dots[i].is_opacity_increase = true;
            dots[i].drift_x = xorshift16() % gui_get_dc()->screen_width;
            dots[i].drift_y = xorshift16() % gui_get_dc()->screen_height;
        }

        gui_img_translate(dots[i].img, dots[i].drift_x, dots[i].drift_y);
    }
}

static void rainbow_digital_app(gui_view_t *view)
{
    if (dot_img_bin[0] == NULL)
    {
        void *dot_img[5] =
        {
            RAINBOWDCLOCK_DOT_0_BIN,
            RAINBOWDCLOCK_DOT_1_BIN,
            RAINBOWDCLOCK_DOT_2_BIN,
            RAINBOWDCLOCK_DOT_3_BIN,
            RAINBOWDCLOCK_DOT_4_BIN
        };
        memcpy(dot_img_bin, dot_img, sizeof(dot_img));
    }

    gui_dispdev_t *dc = gui_get_dc();
    gui_obj_t *obj = GUI_BASE(view);
    gui_view_switch_on_event(view, menu_view->name, SWITCH_OUT_ANIMATION_FADE,
                             SWITCH_IN_ANIMATION_FADE,
                             GUI_EVENT_KB_SHORT_PRESSED);

    gui_win_t *clock_win = gui_win_create(obj, "clock_win", 0, 0, 0, 0);

    // Digits
    uint8_t *digit1_img_bin[5] = {RAINBOWDCLOCK_NUMBER0_0_BIN,
                                  RAINBOWDCLOCK_NUMBER0_1_BIN,
                                  RAINBOWDCLOCK_NUMBER0_2_BIN,
                                  RAINBOWDCLOCK_NUMBER0_3_BIN,
                                  RAINBOWDCLOCK_NUMBER0_4_BIN
                                 };
    init_digit(&digits[0], clock_win, digit1_img_bin, 83, 93);

    uint8_t *digit2_img_bin[5] = {RAINBOWDCLOCK_NUMBER2_0_BIN,
                                  RAINBOWDCLOCK_NUMBER2_1_BIN,
                                  RAINBOWDCLOCK_NUMBER2_2_BIN,
                                  RAINBOWDCLOCK_NUMBER2_3_BIN,
                                  RAINBOWDCLOCK_NUMBER2_4_BIN
                                 };
    init_digit(&digits[1], clock_win, digit2_img_bin, 195, 93);

    uint8_t *digit3_img_bin[5] = {RAINBOWDCLOCK_NUMBER4_0_BIN,
                                  RAINBOWDCLOCK_NUMBER4_1_BIN,
                                  RAINBOWDCLOCK_NUMBER4_2_BIN,
                                  RAINBOWDCLOCK_NUMBER4_3_BIN,
                                  RAINBOWDCLOCK_NUMBER4_4_BIN
                                 };
    init_digit(&digits[2], clock_win, digit3_img_bin, 84, 261);

    uint8_t *digit4_img_bin[5] = {RAINBOWDCLOCK_NUMBER8_0_BIN,
                                  RAINBOWDCLOCK_NUMBER8_1_BIN,
                                  RAINBOWDCLOCK_NUMBER8_2_BIN,
                                  RAINBOWDCLOCK_NUMBER8_3_BIN,
                                  RAINBOWDCLOCK_NUMBER8_4_BIN
                                 };
    init_digit(&digits[3], clock_win, digit4_img_bin, 195, 261);

    // Dots
    gui_win_t *dot_win = gui_win_create(obj, "clock_win", 0, 0, 0, 0);
    for (int i = 0; i < DOT_NUM; i++)
    {
        dots[i].img = gui_img_create_from_mem(dot_win, "dot", dot_img_bin[i % 5], 0, 0, 0, 0);
        dots[i].drift_x = xorshift16() % dc->screen_width;
        dots[i].drift_y = xorshift16() % dc->screen_height;
        dots[i].direction_x = (xorshift16() % 2 == 0) ? 1 : -1;
        dots[i].direction_y = (xorshift16() % 2 == 0) ? 1 : -1;
        dots[i].is_opacity_increase = true;

        gui_img_translate(dots[i].img, dots[i].drift_x, dots[i].drift_y);
        gui_img_set_opacity(dots[i].img, 0);
        gui_img_scale(dots[i].img, 2.0f, 2.0f);
    }

    gui_obj_create_timer(GUI_BASE(clock_win), 10, true, number_animation_cb);
    gui_obj_create_timer(GUI_BASE(dot_win), 10, true, dot_animation_cb);

}
