/*
 * File      : gui_vg_lite_clock.h
 */
#ifndef __GUI_VG_LITE_CLOCK_H__
#define __GUI_VG_LITE_CLOCK_H__



#ifdef __cplusplus
extern "C" {
#endif

#include <guidef.h>
#include <gui_api.h>
#include "vg_lite.h"
#include "vg_lite_platform.h"





typedef struct gui_vg_lite_clock
{
    gui_obj_t base;
    //int32_t i, j, k, m;
    //float angle;
    float c_x;
    float c_y;
    float scale_x;
    float scale_y;
    float t_x;
    float t_y;
} gui_vg_lite_clock_t;




gui_vg_lite_clock_t *gui_vg_lite_clock_create(void *parent,  const char *name, void *addr,
                                              int16_t x, int16_t y, int16_t w, int16_t h);

void gui_vg_lite_clock_ctor(gui_vg_lite_clock_t *this, gui_obj_t *parent, const char *name,
                            void *addr,
                            int16_t x,
                            int16_t y, int16_t w, int16_t h);

void gui_vg_lite_scale(gui_vg_lite_clock_t *this, float scale_x, float scale_y);
void gui_vg_lite_translate(gui_vg_lite_clock_t *this, float t_x, float t_y);


#ifdef __cplusplus
}
#endif

#endif

