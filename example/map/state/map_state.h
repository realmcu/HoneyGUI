#ifndef MAP_STATE_H
#define MAP_STATE_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    MAP_STATE_IDLE,
    MAP_STATE_PANNING,
    MAP_STATE_ANIMATING,
    MAP_STATE_ZOOMING,
} map_state_t;

typedef struct
{
    map_state_t state;
    bool needs_render;
    float velocity_x;
    float velocity_y;
} map_state_context_t;

bool map_state_init(void);

void map_state_tick(void);

void map_state_deinit(void);

map_state_context_t *map_state_get_context(void);

void map_state_request_render(void);

#ifdef __cplusplus
}
#endif

#endif
