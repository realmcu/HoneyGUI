#ifndef __POST_PROCESS_H__
#define __POST_PROCESS_H__
#ifdef __cplusplus
extern "C" {
#endif

#include "guidef.h"
#include "gui_obj.h"

extern void (*blur_prepare)(gui_rect_t *rect, void **mem);
extern void (*blur_depose)(void **mem);

typedef enum
{
    BLUR_HORIZONTAL,
    BLUR_HORIZONTAL_REVERSE,
    BLUR_VERTICAL,
    BLUR_VERTICAL_REVERSE
} BLUR_DIRECTION;

typedef struct
{
    gui_rect_t area;
    uint8_t blur_degree;
    BLUR_DIRECTION dir;
    void *cache_mem;
} post_process_param;

void gauss_blur_post_process_add(post_process_param *event);

void post_process_handle(void);

void gauss_blur_post_process_end(void);

void pre_process_handle(post_process_param *event);

#ifdef __cplusplus
}
#endif

#endif