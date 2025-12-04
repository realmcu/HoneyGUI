/**
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_api_ftl.h
  * @brief Application Programming Interface for UI
  * @details Flash Translation Layer
  * @author sienna_shen@realsil.com.cn
  * @date 2025/3/5
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2025 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_API_FTL_H__
#define __GUI_API_FTL_H__
#ifdef __cplusplus
extern "C" {
#endif


/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <guidef.h>

/*============================================================================*
 *                         Macros
 *============================================================================*/



/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * @brief Read from flash translation layer
 *
 * @param addr The starting address to read from.
 * @param buf Buffer to store the read data.
 * @param len Number of bytes to read.
 * @return int
 */
int gui_ftl_read(uintptr_t addr, uint8_t *buf, size_t len);

/**
 * @brief Write to the flash translation layer
 *
 * @param addr The starting address to write to.
 * @param buf The buffer containing the data to write.
 * @param len The number of bytes to write.
 * @return int
 */
int gui_ftl_write(uintptr_t addr, const uint8_t *buf, size_t len);

/**
 * @brief Erase a section of the flash translation layer
 *
 * @param addr The starting address of the section to erase.
 * @param len The number of bytes to erase.
 * @return int
 */
int gui_ftl_erase(uintptr_t addr, size_t len);

#ifdef __cplusplus
}
#endif
#endif
