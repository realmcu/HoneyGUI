/*
 * File      : gui_eq.h
 */
#ifndef __GUI_EQ_H__
#define __GUI_EQ_H__



#ifdef __cplusplus
extern "C" {
#endif

#include <guidef.h>
#include <gui_api.h>



typedef struct gui_eq
{
    gui_obj_t base;
    float x;
    float y;
    float w;
    float h;
    float t;
} gui_eq_t;




gui_eq_t *gui_eq_create(void *parent,  const char *name, int16_t x, int16_t y, int16_t w,
                        int16_t h);

void gui_eq_set(gui_eq_t *this, float x, float y, float w, float h, float t);

#ifdef __cplusplus
}
#endif

#endif

