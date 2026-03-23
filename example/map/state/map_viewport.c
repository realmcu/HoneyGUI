#include "map_viewport.h"
#include "../map_main.h"
#include "../core/nav_api.h"
#include "../render/render_api.h"
#include "../platform/map_platform.h"
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static map_viewport_t g_viewport = {0};
static bool g_initialized = false;

bool map_viewport_init(void)
{
    if (g_initialized)
    {
        return true;
    }

    if (!g_map)
    {
        return false;
    }

    g_viewport.center_lat = (g_map->header.min_lat + g_map->header.max_lat) / 2.0;
    g_viewport.center_lon = (g_map->header.min_lon + g_map->header.max_lon) / 2.0;
    g_viewport.zoom = 1.0f;

    g_initialized = true;
    return true;
}

void map_viewport_set(double center_lat, double center_lon, float zoom)
{
    if (!g_initialized)
    {
        return;
    }

    g_viewport.center_lat = center_lat;
    g_viewport.center_lon = center_lon;
    g_viewport.zoom = zoom;

    map_viewport_clamp_bounds();
}

void map_viewport_get(double *center_lat, double *center_lon, float *zoom)
{
    if (center_lat)
    {
        *center_lat = g_viewport.center_lat;
    }
    if (center_lon)
    {
        *center_lon = g_viewport.center_lon;
    }
    if (zoom)
    {
        *zoom = g_viewport.zoom;
    }
}

void map_viewport_pan(int16_t dx, int16_t dy)
{
    if (!g_initialized)
    {
        MAP_LOG_WARN("[VIEWPORT] Pan failed: not initialized\n");
        return;
    }

    if (!g_renderer)
    {
        MAP_LOG_WARN("[VIEWPORT] Pan failed: no renderer\n");
        return;
    }

    int width = MAP_WIDTH;
    int height = MAP_HEIGHT;

    float lat_range = g_renderer->view_max_lat - g_renderer->view_min_lat;
    float lon_range = g_renderer->view_max_lon - g_renderer->view_min_lon;

    float meters_per_pixel_lat = (lat_range / height) * 111320.0f;
    float meters_per_pixel_lon = (lon_range / width) * 111320.0f *
                                 cosf(g_viewport.center_lat * M_PI / 180.0f);

    float dlat = dy * meters_per_pixel_lat / 111320.0f;
    float dlon = -dx * meters_per_pixel_lon / (111320.0f * cosf(g_viewport.center_lat * M_PI / 180.0f));

    double old_lat = g_viewport.center_lat;
    double old_lon = g_viewport.center_lon;

    g_viewport.center_lat += dlat;
    g_viewport.center_lon += dlon;

    map_viewport_clamp_bounds();

    MAP_LOG_DEBUG("[VIEWPORT] Pan: dx=%d, dy=%d -> dlat=%.6f, dlon=%.6f (%.6f,%.6f -> %.6f,%.6f)\n",
                  dx, dy, dlat, dlon, old_lat, old_lon, g_viewport.center_lat, g_viewport.center_lon);
}

void map_viewport_zoom_at(float scale, int16_t x, int16_t y)
{
    (void)x;
    (void)y;

    if (!g_initialized)
    {
        return;
    }

    g_viewport.zoom *= scale;

    if (g_viewport.zoom < 0.5f)
    {
        g_viewport.zoom = 0.5f;
    }
    if (g_viewport.zoom > 5.0f)
    {
        g_viewport.zoom = 5.0f;
    }
}

void map_viewport_reset(void)
{
    if (!g_initialized)
    {
        return;
    }

    if (!g_map)
    {
        return;
    }

    g_viewport.center_lat = (g_map->header.min_lat + g_map->header.max_lat) / 2.0;
    g_viewport.center_lon = (g_map->header.min_lon + g_map->header.max_lon) / 2.0;
    g_viewport.zoom = 1.0f;
}

void map_viewport_apply_to_renderer(void)
{
    if (!g_initialized)
    {
        return;
    }

    if (!g_map || !g_renderer)
    {
        return;
    }

    float lat_span = (g_map->header.max_lat - g_map->header.min_lat) * 0.2f * g_viewport.zoom;
    float lon_span = (g_map->header.max_lon - g_map->header.min_lon) * 0.2f * g_viewport.zoom;

    renderer_set_viewport(g_renderer,
                          g_viewport.center_lat - lat_span,
                          g_viewport.center_lon - lon_span,
                          g_viewport.center_lat + lat_span,
                          g_viewport.center_lon + lon_span);
}

bool map_viewport_clamp_bounds(void)
{
    if (!g_map)
    {
        return false;
    }

    bool clamped = false;

    if (g_viewport.center_lat < g_map->header.min_lat)
    {
        g_viewport.center_lat = g_map->header.min_lat;
        clamped = true;
    }
    if (g_viewport.center_lat > g_map->header.max_lat)
    {
        g_viewport.center_lat = g_map->header.max_lat;
        clamped = true;
    }
    if (g_viewport.center_lon < g_map->header.min_lon)
    {
        g_viewport.center_lon = g_map->header.min_lon;
        clamped = true;
    }
    if (g_viewport.center_lon > g_map->header.max_lon)
    {
        g_viewport.center_lon = g_map->header.max_lon;
        clamped = true;
    }

    return clamped;
}

void map_viewport_deinit(void)
{
    g_initialized = false;
}
