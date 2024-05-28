/**
\internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_qbcode.h
  * @brief qbcode widget header file
  * @details qbcode widget is the widget used to create or display qrcode and barcode
  * @author yuyin1_zhang@realsil.com.cn
  * @date 2024/04/17
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
#ifndef _GUI_QBCODE_H_
#define _GUI_QBCODE_H_
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <string.h>
#include "guidef.h"
#include "gui_obj.h"
#include "gui_img.h"

/** @defgroup WIDGET WIDGET
  * @brief
  * @{
  */

/*============================================================================*
 *                         Types
 *============================================================================*/

/** @brief  T_QBCODE_ENCODE_TYPE structure */
typedef enum
{
    QRCODE_ENCODE_TEXT,
    QRCODE_ENCODE_BINARY,
    BARCODE_ENCODE_TEXT,
} T_QBCODE_ENCODE_TYPE;

/** @brief  T_QBCODE_DISPLAY_TYPE structure */
typedef enum
{
    QRCODE_DISPLAY_SECTION,
    QRCODE_DISPLAY_IMAGE,
    BARCODE_DISPLAY_SECTION,
    BARCODE_DISPLAY_IMAGE,
} T_QBCODE_DISPLAY_TYPE;

/** @brief  qrcode widget structure */
typedef struct gui_qbcode
{
    gui_obj_t base;
    int16_t offset_x;
    int16_t offset_y;
    uint16_t border_size;
    float scale_x;
    float scale_y;
    void *data;
    void *image_data;
    gui_img_t *qbcode_img;
    T_QBCODE_DISPLAY_TYPE type;
    T_QBCODE_ENCODE_TYPE encodeType;
} gui_qbcode_t;

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
 * @brief Create a qrcode widget.
 *
 * @param parent the father widget
 * @param filename this qrcode widget's name.
 * @param x the X-axis coordinate relative to parent widget
 * @param y the Y-axis coordinate relative to parent widget
 * @param w qrcode image display width including border
 * @param h qrcode image display height including border
 * @param type QRCODE_DISPLAY_SECTION (gui in real-time) or QRCODE_DISPLAY_IMAGE(save in psRAM by default) or BARCODE_DISPLAY_SECTION or BARCODE_DISPLAY_IMAGE.
 * @param encodeType QRCODE_ENCODE_TEXT or QRCODE_ENCODE_BINARY or BARCODE_ENCODE_TEXT supported.
 * @return gui_qbcode_t* success, NULL failed.
 * @note  this function just create a qrcode object, qbcode data and param should be config by gui_qbcode_config() api.
 */

gui_qbcode_t *gui_qbcode_create(void                  *parent,
                                const char            *name,
                                int16_t                x,
                                int16_t                y,
                                int16_t                w,
                                int16_t                h,
                                T_QBCODE_DISPLAY_TYPE  type,
                                T_QBCODE_ENCODE_TYPE   encodeType);

/**
 * @brief config qbcode data and border param for a qbcode object.
 *
 * @param qbcode a qbcode object pointer.
 * @param data  input data encoded for qbcode
 * @param data_len input data length for encode data
 * @param border_size qrode border size, can be 1, 2, 3... by qbcode size, default white color border;
 * @return null
 */
void gui_qbcode_config(gui_qbcode_t *qbcode, uint8_t *data, uint32_t data_len, uint8_t border_size);

/**
 * for example:
 *
 *  qbcode width 320, height 320, T_QBCODE_DISPLAY_TYPE qbcode_DISPLAY_SECTION, T_QBCODE_ENCODE_TYPE qbcode_ENCODE_TEXT, border size 2;
 *  gui_qbcode_t * qbcode = gui_qbcode_create(parent, "qbcode", 454/2 - 320/2, 454/2 - 320/2, 320, 320, qbcode_DISPLAY_SECTION, qbcode_ENCODE_TEXT);
 *  char* str = "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890";
    gui_qbcode_config(qbcode, str, strlen(str), 2);
 *
 */

#ifdef __cplusplus
}
#endif

#endif
