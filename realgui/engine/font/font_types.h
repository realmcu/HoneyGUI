/**
 \internal
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file font_types.h
  * @brief Common font type definitions
  * @details Shared type definitions for font engine modules
  * @author luke_sun
  * @date 2025/01/08
  * @version v1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2025 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
 \endinternal
  */

#ifndef __FONT_TYPES_H__
#define __FONT_TYPES_H__

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                         Types
 *============================================================================*/

/**
 * @brief Font source mode enumeration
 */
typedef enum
{
    FONT_SRC_MEMADDR = 0,   /**< Memory address mode */
    FONT_SRC_FILESYS = 1,   /**< File system mode */
    FONT_SRC_FTL     = 2,   /**< FTL mode */
} FONT_SRC_MODE;

/**
 * @brief Font source type enumeration
 */
typedef enum
{
    GUI_FONT_SRC_BMP = 0x00,    /**< Bitmap font */
    GUI_FONT_SRC_IMG = 0x01,    /**< Image font */
    GUI_FONT_SRC_MAT = 0x02,    /**< Matrix font */

    GUI_FONT_SRC_TTF = 0x10,    /**< TTF font */
    GUI_FONT_SRC_STB = 0x11,    /**< STB font */

    GUI_FONT_SRC_CUS = 0x20,    /**< Custom font */
} FONT_SRC_TYPE;

#ifdef __cplusplus
}
#endif

#endif /* __FONT_TYPES_H__ */
