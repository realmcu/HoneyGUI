/*
 * File      : gui_img_scope.h
 */
#ifndef __GUI_IMG_SCOPE_H__
#define __GUI_IMG_SCOPE_H__



#ifdef __cplusplus
extern "C" {
#endif

#include <guidef.h>
#include <gui_api.h>
#include <draw_img.h>
#include "gui_img.h"



typedef struct gui_img_scope
{
    gui_img_t base;
    uint16_t scope_x1;
    uint16_t scope_x2;
    uint16_t scope_y1;
    uint16_t scope_y2;
} gui_img_scope_t;


/**
 * @brief create a picture with scope
 *
 * @param parent parent widget
 * @param addr picture address
 * @param x the x coordinate
 * @param y the y coordinate
 * @return gui_img_scope_t*
 */
gui_img_scope_t *gui_img_scope_create(void *parent, void *addr, int16_t x, int16_t y);
#ifdef __cplusplus
}
#endif

#endif

