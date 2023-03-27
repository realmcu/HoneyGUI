/*
* File      : gui_bitmap.h
*/
#ifndef __GUI_BITMAP_H__
#define __GUI_BITMAP_H__




#ifdef __cplusplus
extern "C" {
#endif

#include <guidef.h>


#include <gui_img.h>
typedef struct gui_bitmap gui_bitmap_t;
struct gui_bitmap
{
    gui_img_t base;
    uint32_t color;
    void (*ctor)(gui_bitmap_t *this, gui_obj_t *parent, const char *name, uint8_t *bitmap, int16_t x,
                 int16_t y, int16_t w, int16_t h, uint32_t color);
};
void gui_bitmap_ctor(gui_bitmap_t *this, gui_obj_t *parent, const char *name, uint8_t *bitmap,
                     int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color);
#ifdef __cplusplus
}
#endif

#endif
