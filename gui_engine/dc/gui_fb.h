/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_fb.h
  * @brief Framebuffer update
  * @details The entire update process
  * @author howie_wang@realsil.com.cn
  * @date 2023/10/19
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_FB_H__
#define __GUI_FB_H__

#ifdef __cplusplus
extern "C" {
#endif


/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <guidef.h>
#include <gui_api.h>


/** @defgroup DC DC
  * @brief
  * @{
  */

/*============================================================================*
 *                         Types
 *============================================================================*/
/** @defgroup DC_Exported_Types DC Exported Types
  * @brief
  * @{
  */



/** End of DC_Exported_Types
  * @}
  */

/*============================================================================*
 *                         Constants
 *============================================================================*/
/** @defgroup DC_Exported_Constants DC Exported Constants
  * @brief
  * @{
  */


/** End of DC_Exported_Constants
  * @}
  */

/*============================================================================*
 *                         Macros
 *============================================================================*/
/** @defgroup DC_Exported_Macros DC Exported Macros
  * @brief
  * @{
  */




/** End of DC_Exported_Macros
  * @}
  */

/*============================================================================*
 *                         Variables
 *============================================================================*/
/** @defgroup DC_Exported_Variables DC Exported Variables
  * @brief
  * @{
  */


/** End of DC_Exported_Variables
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/
/** @defgroup DC_Exported_Functions DC Exported Functions
  * @brief
  * @{
  */

/**
 * @brief entire update process
 *
 * @param parent the widget tree
 */
void gui_fb_disp(struct _gui_obj_t *parent);


/**
 * @brief this means framebuffer have change, need update framebuffer
 *
 */
void gui_fb_change(void);



/** End of DC_Exported_Functions
  * @}
  */

/** End of DC
  * @}
  */



#ifdef __cplusplus
}
#endif

#endif










