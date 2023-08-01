/*
 * File      : gui_svg.h
 */
#ifndef __GUI_SVG_H__
#define __GUI_SVG_H__



#ifdef __cplusplus
extern "C" {
#endif

#include <guidef.h>
#include <gui_api.h>



typedef struct gui_svg
{
    gui_obj_t base;
    const char *filename;
    uint8_t *addr;
    uint32_t size;
    float degrees;
    float c_x;
    float c_y;
    float scale_x;
    float scale_y;
    float t_x;
    float t_y;
    gui_animate_t *animate;
} gui_svg_t;




gui_svg_t *gui_svg_create_from_file(void *parent,  const char *name, const char *filename,
                                    int16_t x, int16_t y, int16_t w, int16_t h);

gui_svg_t *gui_svg_create_from_mem(void *parent,  const char *name, uint8_t *addr, uint32_t size,
                                   int16_t x, int16_t y, int16_t w, int16_t h);

#ifdef __cplusplus
}
#endif

#endif

