#ifndef MAP_MAIN_H
#define MAP_MAIN_H

#include "core/nav_api.h"
#include "render/render_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Global map data - direct access */
extern map_t *g_map;
extern renderer_t *g_renderer;

/* Map resolution constants */
#define MAP_WIDTH 480
#define MAP_HEIGHT 480

#ifdef __cplusplus
}
#endif

#endif
