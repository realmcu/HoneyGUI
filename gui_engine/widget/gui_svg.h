/**
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
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_WIDGET_H__
#define __GUI_WIDGET_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <guidef.h>
#include <gui_api.h>

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

/** @brief  ... */
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
/** @defgroup WIDGET_Exported_Functions WIDGET Exported Functions
  * @brief
  * @{
  */


/**
 * @brief
 *
 * @param parent
 * @param name
 * @param filename
 * @param x
 * @param y
 * @param w
 * @param h
 * @return gui_svg_t*
 */
gui_svg_t *gui_svg_create_from_file(void *parent,  const char *name, const char *filename,
                                    int16_t x, int16_t y, int16_t w, int16_t h);

/**
 * @brief
 *
 * @param parent
 * @param name
 * @param addr
 * @param size
 * @param x
 * @param y
 * @param w
 * @param h
 * @return gui_svg_t*
 */
gui_svg_t *gui_svg_create_from_mem(void *parent,  const char *name, uint8_t *addr, uint32_t size,
                                   int16_t x, int16_t y, int16_t w, int16_t h);
/**
 * @brief
 *
 * @param svg
 * @param degrees
 * @param c_x
 * @param c_y
 */
void gui_svg_rotation(gui_svg_t *svg, float degrees, float c_x, float c_y);
/**
 * @brief
 *
 * @param svg
 * @param scale_x
 * @param scale_y
 */
void gui_svg_scale(gui_svg_t *svg, float scale_x, float scale_y);
/**
 * @brief
 *
 * @param svg
 * @param t_x
 * @param t_y
 */
void gui_svg_translate(gui_svg_t *svg, float t_x, float t_y);
/**
 * @brief
 *
 * @param svg
 * @param opacity_value
 */
void gui_svg_set_opacity(gui_svg_t *svg, unsigned char opacity_value);


/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */


#ifdef __cplusplus
}
#endif

#endif

