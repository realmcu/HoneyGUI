/**
\internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_curtain.h
  * @brief create a curtain effect widget,which should be nested in a curtainview.
  * @details Slide to extend and retract curtains
  * @author triton_yu@realsil.com.cn
  * @date 2023/10/24
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
#ifndef __GUI_CURTAIN_H__
#define __GUI_CURTAIN_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <guidef.h>
#include <gui_fb.h>
#include "gui_curtainview.h"
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

/** @brief  curtain structure */
typedef struct gui_curtain
{
    gui_obj_t base; //!< base structure
    gui_curtain_enum_t orientation;
    float scope;
    void (*ctor)(struct gui_curtain *this, gui_obj_t *parent, const char *filename, int16_t x,
                 int16_t y,
                 int16_t w, int16_t h, gui_curtain_enum_t orientation, float scope); //!< constructor
} gui_curtain_t;
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
/** @defgroup WIDGET_Exported_TEST_Functions WIDGET Exported Functions
  * @brief
  * @{
  */

/**
 * @brief Create a curtain effect widget, which should be nested in a curtainview.
 *
 * @param parent the father widget nested in(hould be a curtainview.)
 * @param filename this curtain widget's name.
 * @param x the X-axis coordinate relative to parent widget
 * @param y the Y-axis coordinate relative to parent widget
 * @param w width
 * @param h height
 * @param orientation  the orientation of the curtain,refer to gui_curtain_enum_t.
 * @param scope The range in which curtains can be expanded
 * @return gui_curtain_t*
 */
gui_curtain_t *gui_curtain_create(void *parent, const char *filename, int16_t x, int16_t y,
                                  int16_t w, int16_t h, gui_curtain_enum_t orientation, float scope);


/** End of WIDGET_Exported_TEST_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */


#ifdef __cplusplus
}
#endif

#endif
