/*
 * File      : gui_img.h
 */
#ifndef __GUI_IMG_H__
#define __GUI_IMG_H__



#ifdef __cplusplus
extern "C" {
#endif

#include <guidef.h>
#include <gui_api.h>
#include <draw_img.h>


typedef struct gui_img
{
    gui_obj_t base;
    draw_img_t draw_img;
} gui_img_t;





gui_img_t *gui_img_create_from_mem(void *parent,  const char *name, void *addr,
                                   int16_t x, int16_t y, int16_t w, int16_t h);

void gui_img_from_mem_ctor(gui_img_t *this, gui_obj_t *parent, const char *name, void *addr,
                           int16_t x,
                           int16_t y, int16_t w, int16_t h);

/**
 * @brief set x,y and file path
 *
 * @param img image widget
 * @param filename change filename if using filesystem picture
 * @param addr change picture address
 * @param x X-axis coordinate
 * @param y Y-axis coordinate
 */
void gui_img_set_attribute(gui_img_t *img, const char *filename, void *addr, int16_t x,
                           int16_t y);

/**
 * @brief load the image to read it's width
 *
 * @param img the image widget pointer.
 * @return uint16_t image's width
 */
uint16_t gui_img_get_width(gui_img_t *img);

/**
 * @brief load the image to read it's hight
 *
 * @param img the image widget pointer.
 * @return uint16_t image's height
 */
uint16_t gui_img_get_height(gui_img_t *img);

/**
 * @brief set the image's location
 *
 * @param img the image widget pointer.
 * @param x the x coordinate
 * @param y the y coordinate
 */
void gui_img_set_location(gui_img_t *img, uint16_t x, uint16_t y);

/**
 * @brief set the image's mode.
 *
 * @param img the image widget pointer.
 * @param mode the enumeration value of the mode is IMAGE_MODE.
 * @return void
 *
 */
void gui_img_set_mode(gui_img_t *img, BLEND_MODE_TYPE mode);


#ifdef __cplusplus
}
#endif

#endif

