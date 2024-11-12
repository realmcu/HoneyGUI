/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_widget_litegfx.h
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
#ifndef __GUI_WIDGET_LITEGFX_H__
#define __GUI_WIDGET_LITEGFX_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <guidef.h>
#include <gui_api.h>
#include <gui_obj.h>
#include <lx_vglite_api.h>


/*============================================================================*
 *                         Types
 *============================================================================*/

/** @brief  ... */
typedef struct gui_widget_litegfx
{
    gui_obj_t base;
    uintptr_t handler;
    uintptr_t instance;
    lx_vglite_user_cb_t callback;
} gui_widget_litegfx_t;



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
 * @brief  Create a LiteGFX GUI widget.
 *
 * This function creates a new LiteGFX GUI widget and initializes it with the provided parameters.
 *
 * @param parent     Pointer to the parent widget.
 * @param name       The name of the widget, used for identification.
 * @param instance   Instance identifier, typically used to associate with underlying hardware or resources.
 * @param cb         User callback function that will be called when events occur on the widget.
 * @param x          X coordinate of the widget.
 * @param y          Y coordinate of the widget.
 * @param w          Width of the widget.
 * @param h          Height of the widget.
 *
 * @return gui_widget_litegfx_t* Pointer to the newly created widget. Returns NULL if the creation fails.
 */
gui_widget_litegfx_t *gui_widget_litegfx_create(void *parent,  const char *name,
                                                uintptr_t instance,
                                                lx_vglite_user_cb_t cb,
                                                int16_t x, int16_t y, int16_t w, int16_t h);


/**
 * @brief Set the image for a LiteGFX GUI widget.
 *
 * This function sets an image for a given LiteGFX GUI widget at the specified index.
 *
 * @param obj   Pointer to the GUI object for which the image is being set.
 * @param index Index at which the image should be set.
 * @param data  Pointer to the image data.
 *
 * @return void
 */
void gui_widget_litegfx_set_image(gui_obj_t *obj, uint16_t index, void *data);

/**
 * @brief Set a parameter for a LiteGFX GUI widget.
 *
 * This function sets a parameter for a given LiteGFX GUI widget using a specified command.
 *
 * @param obj   Pointer to the GUI object for which the parameter is being set.
 * @param cmd   Command that specifies which parameter is being set.
 * @param param Value of the parameter to be set, passed as an uintptr_t.
 *
 * @return void
 */
void gui_widget_litegfx_set_param(gui_obj_t *obj, uint16_t cmd, uintptr_t param);



#ifdef __cplusplus
}
#endif

#endif

