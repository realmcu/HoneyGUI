#include "map_state.h"
#include "map_gesture.h"
#include "map_viewport.h"
#include "../map_main.h"
#include "../core/nav_api.h"
#include "../render/render_api.h"
#include "../platform/map_platform.h"

static map_state_context_t g_state_ctx = {0};
static bool g_initialized = false;

extern map_t *g_map;

bool map_state_init(void)
{
    if (g_initialized)
    {
        return true;
    }

    map_t *map = g_map;
    if (!map)
    {
        return false;
    }

    g_state_ctx.state = MAP_STATE_IDLE;
    g_state_ctx.needs_render = true;
    g_state_ctx.velocity_x = 0.0f;
    g_state_ctx.velocity_y = 0.0f;

    if (!map_gesture_init())
    {
        return false;
    }

    if (!map_viewport_init())
    {
        map_gesture_deinit();
        return false;
    }

    g_initialized = true;
    return true;
}

/**
 * @brief Per-frame state machine tick
 *
 * Processes input, updates state, and renders when needed. Called every frame
 * by the GUI server hook registered during initialization.
 *
 * @note Registered via gui_register_server_hook() in map_main.c
 *
 * Flow:
 * 1. Process touch/gesture input
 * 2. Update state machine (IDLE/PANNING/ANIMATING/ZOOMING)
 * 3. Render if needs_render flag is set
 */
void map_state_tick(void)
{
    if (!g_initialized)
    {
        return;
    }

    map_gesture_process();

    switch (g_state_ctx.state)
    {
    case MAP_STATE_IDLE:
        break;

    case MAP_STATE_PANNING:
        break;

    case MAP_STATE_ANIMATING:
        break;

    case MAP_STATE_ZOOMING:
        break;
    }

    if (g_state_ctx.needs_render)
    {
        if (!g_map || !g_renderer)
        {
            MAP_LOG_WARN("[STATE] Render skipped: map=%p, renderer=%p\n", (void *)g_map, (void *)g_renderer);
            return;
        }

        MAP_LOG_DEBUG("[STATE] Rendering frame...\n");

        map_viewport_apply_to_renderer();

        map_theme_t theme = MAP_THEME_GOOGLE;
        render_options_t options;
        render_options_init(&options);
        render_map_with_options(g_renderer, g_map, &theme, &options);

        const map_platform_ops_t *ops = map_platform_get();
        if (ops && ops->display.flush)
        {
            ops->display.flush();
        }

        g_state_ctx.needs_render = false;

        MAP_LOG_DEBUG("[STATE] Frame rendered successfully\n");
    }
}

void map_state_deinit(void)
{
    if (!g_initialized)
    {
        return;
    }

    map_viewport_deinit();
    map_gesture_deinit();

    g_initialized = false;
}

map_state_context_t *map_state_get_context(void)
{
    return &g_state_ctx;
}

void map_state_request_render(void)
{
    g_state_ctx.needs_render = true;
}
