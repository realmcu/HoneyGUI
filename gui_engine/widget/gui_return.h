/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_return.h
  * @brief slide to return to home
  * @details return to launcher
  * @author triton_yu@realsil.com.cn
  * @date 2023/11/8
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
 \endinternal
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_RETURN_H__
#define __GUI_RETURN_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <guidef.h>
#include <gui_img.h>


/*============================================================================*
 *                         Types
 *============================================================================*/

typedef struct gui_return gui_return_t;
struct gui_return
{
    gui_obj_t base;
    gui_img_t *return_img_left;
    gui_img_t *return_img_right;
    void (*onReturn)(void *func);
    bool hit;
    void (*ctor)(gui_return_t *this, gui_obj_t *parent, const char *widgetame);
};


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
 * @brief create a return widget.
 * @param parent the father widget it nested in.
 * @return return the widget object pointer.
 *
 */
gui_return_t *gui_return_create(void *parent);



#ifdef __cplusplus
}
#endif

#endif


