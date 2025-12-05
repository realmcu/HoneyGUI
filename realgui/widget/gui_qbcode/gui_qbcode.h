/**
\internal
*****************************************************************************************
*     Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_qbcode.h
  * @brief Qbcode widget header file.
  * @details Qbcode widget is the widget used to create or display qrcode and barcode.
  * @author yuyin1_zhang@realsil.com.cn
  * @date 2024/04/17
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2024 Realtek Semiconductor Corporation</center></h2>
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

/*============================================================================*
 *                         Types
 *============================================================================*/

/* T_QBCODE_ENCODE_TYPE structure start */
typedef enum
{
    QRCODE_ENCODE_TEXT,
    QRCODE_ENCODE_BINARY,
    BARCODE_ENCODE_TEXT,
} T_QBCODE_ENCODE_TYPE;
/* T_QBCODE_ENCODE_TYPE structure end */

/** @brief  T_QBCODE_DISPLAY_TYPE structure. */
typedef enum
{
    QRCODE_DISPLAY_SECTION,
    QRCODE_DISPLAY_IMAGE,
    BARCODE_DISPLAY_SECTION,
    BARCODE_DISPLAY_IMAGE,
} T_QBCODE_DISPLAY_TYPE;

/** @brief  Qrcode widget structure. */
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
 * @brief Create a qrcode widget.
 *
 * @param parent Father widget.
 * @param name Qrcode widget's name.
 * @param x X-axis coordinate relative to parent widget.
 * @param y Y-axis coordinate relative to parent widget.
 * @param w Qrcode image display width including border.
 * @param h Qrcode image display height including border.
 * @param type QRCODE_DISPLAY_SECTION (gui in real-time) or QRCODE_DISPLAY_IMAGE(save in psRAM by default) or BARCODE_DISPLAY_SECTION or BARCODE_DISPLAY_IMAGE.
 * @param encodeType QRCODE_ENCODE_TEXT or QRCODE_ENCODE_BINARY or BARCODE_ENCODE_TEXT supported.
 * @return Pointer to created qbcode widget on success, NULL on failure.
 * @note This function just creates a qrcode object, qbcode data and param should be configured by gui_qbcode_config() API.
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
 * @brief Config qbcode data and border param for a qbcode object.
 *
 * @param qbcode Qbcode object pointer.
 * @param data Input data encoded for qbcode.
 * @param data_len Input data length for encode data.
 * @param border_size Qrcode border size, can be 1, 2, 3... by qbcode size, default white color border.
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
