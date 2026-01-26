/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_CANVAS_IMG_H__
#define __GUI_CANVAS_IMG_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "gui_canvas.h"
#include "gui_img.h"


/*============================================================================*
 *                         Types
 *============================================================================*/

/** @brief  canvas structure */
typedef struct _gui_canvas_img
{
    gui_canvas_t mCanvas;
    bool bImageActive;
    uint8_t *bpImageBuff;
    uint8_t  cRed;
    uint8_t  cGreen;
    uint8_t  cBlue;
    gui_img_t *bpImageCtr;
    bool (*fpCanvas_img_update_cb)(struct _gui_canvas_img *);
} gui_canvas_img_t;

/*============================================================================*
 *                         Constants
 *============================================================================*/


/*============================================================================*
 *                         Macros
 *============================================================================*/


/*============================================================================*
 *                         Variables
 *============================================================================*/


/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * @brief create a canvas image widget used to drawing graphics in nanovg.
 *
 * @param parent the father widget nested in.
 * @param name this canvas widget's name.
 * @param addr
 * @param x the X-axis coordinate relative to parent widget
 * @param y the Y-axis coordinate relative to parent widget
 * @param w width
 * @param h height
 * @return gui_canvas_img_t*
 */
gui_canvas_img_t *gui_canvas_img_create(void *parent,  const char *name, void *addr,
                                        int16_t x, int16_t y, int16_t w, int16_t h);

/**
 * @brief set the callback function for drawing specific shapes.
 *
 * @param this this widget pointer
 * @param cb the callback function for drawing specific shapes
 */
void gui_canvas_img_set_canvas_cb(gui_canvas_img_t *this,
                                  void (*cb)(gui_canvas_t *this));

void gui_canvas_img_set_update_cb(gui_canvas_img_t *this, bool (*cb)(gui_canvas_img_t *this));

void gui_canvas_img_set_TransColor(gui_canvas_img_t *this, unsigned char r, unsigned char g,
                                   unsigned char b);

#ifdef __cplusplus
}
#endif

#endif

