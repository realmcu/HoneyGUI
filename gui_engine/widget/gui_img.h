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
#include <gui_matrix.h>


typedef struct gui_img
{
    gui_obj_t base;
    draw_img_t draw_img;

    float degrees;
    float c_x;//center of image x
    float c_y;//center of image y
    float scale_x;
    float scale_y;
    float t_x;//center of screen x
    float t_y;//center of screen y
    gui_animate_t *animate;
} gui_img_t;





gui_img_t *gui_img_create_from_mem(void *parent,  const char *name, void *addr,
                                   int16_t x, int16_t y, int16_t w, int16_t h);


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

void gui_img_rotation(gui_img_t *img, float degrees, float c_x, float c_y);
void gui_img_scale_add(gui_img_t *img, float scale_x, float scale_y);
void gui_img_scale(gui_img_t *img, float scale_x, float scale_y);
void gui_img_translate(gui_img_t *img, float t_x, float t_y);
void gui_img_set_opacity(gui_img_t *this, unsigned char opacity_value);


#ifdef __cplusplus
}
#endif

#endif

