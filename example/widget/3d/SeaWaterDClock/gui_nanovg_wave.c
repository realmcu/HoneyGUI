#include "nanovg.h"
#include <string.h>
#include <math.h>
#include "gui_canvas.h"
#include <tp_algo.h>
#include "gui_win.h"

#define ROTATION_INCREMENT 0.5f
#define POSITION_INCREMENT 2.0f
#define CAN_COUNT 4
#define NUM_CAN_TIME_PAIRS 4

extern gui_win_t *can0_window;
extern gui_win_t *can1_window;
extern gui_win_t *can2_window;
extern gui_win_t *can3_window;

static float touch_x = -1;
static float touch_time = 0;
static float wave_alpha = 0.0f;
static float can_positions[CAN_COUNT] = {55, 140, 225, 315};
static float can_wave_y[CAN_COUNT] = {0}; // Wave height at each can position
static float can_current_pos[CAN_COUNT] = {0}; // Current can positions
static float can_rotation_targets[NUM_CAN_TIME_PAIRS] = {-10, 4, -5, 5};
static float can_current_rotation[NUM_CAN_TIME_PAIRS] = {0};

void gui_wave_draw_graph(gui_canvas_t *canvas)
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
            float distance = fabs(xi - touch_x);
            if (distance < 50)
            {
                float decay_factor = expf(-distance * 0.02f);
                float time_decay = expf(-(t - touch_time) * 2.0f);
                touch_effect = decay_factor * 0.8f * time_decay;
            }
        }

        can_wave_y[i] = y_base - h * fmax(0, fmin(1, base_wave * 0.4f + 0.5f + touch_effect));
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

void animate_cans_based_on_wave()
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

        gui_win_t *current_can = NULL;
        switch (j)
        {
        case 0: current_can = can0_window; break;
        case 1: current_can = can1_window; break;
        case 2: current_can = can2_window; break;
        case 3: current_can = can3_window; break;
        }

        if (current_can)
        {
            matrix_translate(0, can_current_pos[j], current_can->base.matrix);
        }
    }
}

void can_animate_cb(void *parent)
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

    // Apply transformations
    gui_win_t *window = (gui_win_t *)parent;
    int can_index = -1;

    if (parent == can0_window) { can_index = 0; }
    else if (parent == can1_window) { can_index = 1; }
    else if (parent == can2_window) { can_index = 2; }
    else if (parent == can3_window) { can_index = 3; }

    if (can_index >= 0)
    {
        matrix_translate(-window->base.w / 2, -window->base.h / 2, window->base.matrix);
        matrix_rotate(can_current_rotation[can_index], window->base.matrix);
        matrix_translate(window->base.w / 2, window->base.h / 2, window->base.matrix);
    }
}