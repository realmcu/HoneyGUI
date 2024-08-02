/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file
  * @brief
  * @details
  * @author
  * @date
  * @version
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
 \endinternal
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

/** @defgroup WIDGET WIDGET
 * @brief
 * @{
 */

/*============================================================================*
 *                         Types
 *============================================================================*/
/** @defgroup WIDGET_Exported_Types WIDGET Exported Types
 * @brief
 * @{
 */

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


/** End of WIDGET_Exported_Types
  * @}
  */

/*============================================================================*
 *                         Constants
 *============================================================================*/
/** @defgroup WIDGET_Exported_Constants WIDGET Exported Constants
  * @brief
  * @{
  */


/** End of WIDGET_Exported_Constants
  * @}
  */

/*============================================================================*
 *                         Macros
 *============================================================================*/
/** @defgroup WIDGET_Exported_Macros WIDGET Exported Macros
  * @brief
  * @{
  */





/** End of WIDGET_Exported_Macros
  * @}
  */

/*============================================================================*
 *                         Variables
 *============================================================================*/
/** @defgroup WIDGET_Exported_Variables WIDGET Exported Variables
  * @brief
  * @{
  */


/** End of WIDGET_Exported_Variables
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/
/** @defgroup WIDGET_Exported_GUI_Functions WIDGET Exported Functions
  * @brief
  * @{
  */

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

/** End of WIDGET_Exported_GUI_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */
void gui_canvas_img_set_update_cb(gui_canvas_img_t *this, bool (*cb)(gui_canvas_img_t *this));

void gui_canvas_img_set_TransColor(gui_canvas_img_t *this, unsigned char r, unsigned char g,
                                   unsigned char b);

#ifdef __cplusplus
}
#endif

#endif

