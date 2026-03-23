#ifndef MAP_VIEWPORT_H
#define MAP_VIEWPORT_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    double center_lat;
    double center_lon;
    float zoom;
} map_viewport_t;

bool map_viewport_init(void);

void map_viewport_set(double center_lat, double center_lon, float zoom);

void map_viewport_get(double *center_lat, double *center_lon, float *zoom);

void map_viewport_pan(int16_t dx, int16_t dy);

void map_viewport_zoom_at(float scale, int16_t x, int16_t y);

void map_viewport_reset(void);

void map_viewport_apply_to_renderer(void);

bool map_viewport_clamp_bounds(void);

void map_viewport_deinit(void);

#ifdef __cplusplus
}
#endif

#endif
