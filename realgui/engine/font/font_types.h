/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
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

/**
 * @brief Bold mode for text rendering (1-bit field)
 */
typedef enum
{
    BOLD_HORIZONTAL = 0,   /**< Horizontal only, ~7x faster than BOLD_FULL */
    BOLD_FULL       = 1,   /**< Horizontal + vertical */
} BOLD_MODE;

#ifdef __cplusplus
}
#endif

#endif /* __FONT_TYPES_H__ */
