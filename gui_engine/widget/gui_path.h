/*
 * File      : gui_img.h
 */
#ifndef __GUI_PATH_H__
#define __GUI_PATH_H__



#ifdef __cplusplus
extern "C" {
#endif

#include <guidef.h>
#include <gui_api.h>
#include <draw_path.h>




typedef struct gui_path
{
    gui_obj_t base;
    struct rtgui_matrix *matrix;
    draw_path_t draw_path;
} gui_path_t;




gui_path_t *gui_path_create(void *parent,  const char *name, void *addr,
                            int16_t x, int16_t y, int16_t w, int16_t h);

void gui_path_ctor(gui_path_t *this, gui_obj_t *parent, const char *name, void *addr,
                   int16_t x,
                   int16_t y, int16_t w, int16_t h);


#ifdef __cplusplus
}
#endif

#endif

