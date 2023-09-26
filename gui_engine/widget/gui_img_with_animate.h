/*
 * File      : gui_img.h
 */
#ifndef __GUI_IMG_WITH_ANIMATE_H__
#define __GUI_IMG_WITH_ANIMATE_H__



#ifdef __cplusplus
extern "C" {
#endif

#include <guidef.h>
#include <gui_api.h>
#include "gui_img.h"
typedef struct gui_img_with_animate gui_img_with_animate_t;
struct gui_img_with_animate
{
    gui_img_t base;
    gui_animate_t *animate;
    void (*ctor)(gui_img_with_animate_t *image, gui_obj_t *parent, void *addr, int16_t x, int16_t y);
} ;
gui_img_with_animate_t *gui_img_with_animate_create(void *parent, void *resourse_address, int16_t x,
                                                    int16_t y);
void gui_img_with_animate_set_animate(gui_img_with_animate_t *o, uint32_t dur, int repeatCount,
                                      void *callback,
                                      void *p);

#ifdef __cplusplus
}
#endif
#endif

