#include "map_gesture.h"
#include "map_viewport.h"
#include "map_state.h"
#include "../platform/map_platform.h"
#include <math.h>

typedef struct
{
    int16_t touch_start_x;
    int16_t touch_start_y;
    int16_t touch_last_x;
    int16_t touch_last_y;
    bool touch_active;

    uint32_t last_tap_time;
    int16_t last_tap_x;
    int16_t last_tap_y;
} gesture_context_t;

static gesture_context_t g_gesture_ctx = {0};
static bool g_initialized = false;

bool map_gesture_init(void)
{
    if (g_initialized)
    {
        return true;
    }

    g_gesture_ctx.touch_active = false;
    g_gesture_ctx.last_tap_time = 0;

    g_initialized = true;
    return true;
}

void map_gesture_process(void)
{
    if (!g_initialized)
    {
        return;
    }

    const map_platform_ops_t *ops = map_platform_get();
    if (!ops || !ops->input.get_touch_state)
    {
        return;
    }

    map_touch_state_t touch;
    if (!ops->input.get_touch_state(&touch) || !touch.valid)
    {
        return;
    }

    if (touch.pressed)
    {
        if (!g_gesture_ctx.touch_active)
        {
            g_gesture_ctx.touch_active = true;

            map_state_context_t *state = map_state_get_context();
            state->state = MAP_STATE_IDLE;

            MAP_LOG_DEBUG("[GESTURE] Touch DOWN\n");
        }
        else
        {
            int16_t dx = touch.x;
            int16_t dy = touch.y;

            if (dx != 0 || dy != 0)
            {
                MAP_LOG_DEBUG("[GESTURE] Pan: deltaX=%d, deltaY=%d\n", dx, dy);

                map_viewport_pan(dx, dy);

                map_state_context_t *state = map_state_get_context();
                state->state = MAP_STATE_PANNING;
                map_state_request_render();
            }
        }
    }
    else
    {
        if (g_gesture_ctx.touch_active)
        {
            MAP_LOG_DEBUG("[GESTURE] Touch UP\n");

            g_gesture_ctx.touch_active = false;

            map_state_context_t *state = map_state_get_context();
            state->state = MAP_STATE_IDLE;
        }
    }
}

void map_gesture_deinit(void)
{
    g_initialized = false;
}
