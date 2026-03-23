#ifndef MAP_GESTURE_H
#define MAP_GESTURE_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    MAP_GESTURE_NONE,
    MAP_GESTURE_PAN,
    MAP_GESTURE_DOUBLE_TAP,
    MAP_GESTURE_PINCH,
} map_gesture_type_t;

typedef struct
{
    map_gesture_type_t type;
    union
    {
        struct
        {
            int16_t dx;
            int16_t dy;
        } pan;
        struct
        {
            int16_t x;
            int16_t y;
        } tap;
        struct
        {
            float scale;
            int16_t center_x;
            int16_t center_y;
        } pinch;
    } data;
} map_gesture_event_t;

bool map_gesture_init(void);

void map_gesture_process(void);

void map_gesture_deinit(void);

#ifdef __cplusplus
}
#endif

#endif
