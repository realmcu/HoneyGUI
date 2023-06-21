/*
 * File      : gui_magic_img.h
 */
#ifndef __GUI_MAGIC_IMG_H__
#define __GUI_MAGIC_IMG_H__



#ifdef __cplusplus
extern "C" {
#endif

#include <guidef.h>
#include <gui_img.h>


typedef struct gui_magic_img
{
    gui_img_t base;

    float degrees;
    int16_t w;
    int16_t h;
    float c_x;
    float c_y;
    float scale_x;
    float scale_y;
    float t_x;
    float t_y;
    float p_x;
    float p_y;
    gui_animate_t *animate;
} gui_magic_img_t;




gui_magic_img_t *gui_magic_img_create_from_mem(void *parent,  const char *name, void *addr,
                                               int16_t x, int16_t y, int16_t w, int16_t h);

void gui_magic_img_from_mem_ctor(gui_magic_img_t *this, gui_obj_t *parent, const char *name,
                                 void *addr, int16_t x, int16_t y, int16_t w, int16_t h);

void gui_img_rotation(gui_magic_img_t *img, float degrees, float c_x, float c_y);
void gui_img_scale_add(gui_magic_img_t *img, float scale_x, float scale_y);
void gui_img_scale(gui_magic_img_t *img, float scale_x, float scale_y);
void gui_img_translate(gui_magic_img_t *img, float t_x, float t_y);
void gui_img_set_opacity(gui_magic_img_t *this, unsigned char opacity_value);
void magic_img_get_new_area(gui_obj_t *obj, gui_img_t *img, struct gui_dispdev *dc);
gui_magic_img_t *gui_svg_create_from_mem(void *parent, void *data, uint32_t data_size,
                                         int16_t x, int16_t y, int16_t w, int16_t h);
void gui_magicimage_set_animate(void *o, uint32_t dur, int repeatCount, void *callback, void *p);
gui_magic_img_t *gui_img_create_from_fs(void *parent, const char *file, int16_t x, int16_t y);
void *gui_get_file_address(const char *file);
#ifdef __cplusplus
}
#endif

#endif

