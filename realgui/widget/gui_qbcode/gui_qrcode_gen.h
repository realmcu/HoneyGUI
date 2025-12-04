/**
\internal
*****************************************************************************************
*     Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file  gui_qrcode_gen.c
  * @brief Qrcode widget.
  * @details Qrcode.
  * @author yuyin1_zhang@realsil.com.cn
  * @date 2024/4/17
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
#ifndef _GUI_QRCODE_GEN_H_
#define _GUI_QRCODE_GEN_H_

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "math.h"


/*============================================================================*
 *                         Macros
 *============================================================================*/

#define QRCODEGEN_VERSION_MIN   2  // The minimum version number supported in the QR Code Model 2 standard
#define QRCODEGEN_VERSION_MAX  25//15

/*============================================================================*
 *                         Types
 *============================================================================*/


/*============================================================================*
 *                         Constants
 *============================================================================*/


/*============================================================================*
 *                         Variables
 *============================================================================*/


/*============================================================================*
 *                         Functions
 *============================================================================*/

/*
 * @brief Returns the side length of the given QR Code, assuming that encoding succeeded.
 * The result is in the range [21, 177]. Note that the length of the array buffer
 * is related to the side length - every 'uint8_t qrcode[]' must have length at least
 * qrcodegen_BUFFER_LEN_FOR_VERSION(version), which equals ceil(size^2 / 8 + 1).
 */
int gui_qrcode_gen_get_size(const uint8_t qrcode[]);

/*
 * @brief Returns the color of the module (pixel) at the given coordinates, which is false
 * for light or true for dark. The top left corner has the coordinates (x=0, y=0).
 * If the given coordinates are out of bounds, then false (light) is returned.
 */
bool gui_qrcode_gen_get_module(const uint8_t qrcode[], int x, int y);

/**
 * @brief Obtain the encoded data.
 *
 * @param data Data that needs to be encoded.
 * @param data_length The length of data.
 * @param flag Data format, 0 representing text and 1 representing binary.
 */
uint8_t *gui_qrcode_gen_get_data(unsigned char *data, uint16_t data_length, uint8_t flag);

#ifdef __cplusplus
}
#endif

#endif /* _GUI_QRCODE_GEN_H_ */
