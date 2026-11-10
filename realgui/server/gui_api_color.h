/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_API_COLOR_H__
#define __GUI_API_COLOR_H__
#ifdef __cplusplus
extern "C" {
#endif


/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "guidef.h"

/*============================================================================*
 *                         Macros
 *============================================================================*/

/** @brief white color in gui_color_t structure */
#define APP_COLOR_WHITE                     gui_rgba(UINT8_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX)
/** @brief red color in gui_color_t structure */
#define APP_COLOR_RED                       gui_rgba(UINT8_MAX, 0, 0, UINT8_MAX)
/** @brief green color in gui_color_t structure */
#define APP_COLOR_GREEN                     gui_rgba(0, UINT8_MAX, 0, UINT8_MAX)
/** @brief blue color in gui_color_t structure */
#define APP_COLOR_BLUE                      gui_rgba(0, 0, UINT8_MAX, UINT8_MAX)

/**
 * @link https://www.rapidtables.com/web/color/RGB_Color.html
 * @brief APP_COLOR_##HTML / CSS Name       Decimal Code(R,G,B)
*/
#define APP_COLOR_CRIMSON                   gui_rgb(220,20,60)
#define APP_COLOR_FIREBRICK                 gui_rgb(178,34,34)
#define APP_COLOR_WHITE_OPACITY             gui_rgba(255,255,255,150)
#define APP_COLOR_SILVER                    gui_rgb(192,192,192)
#define APP_COLOR_SILVER_OPACITY(opacity)   gui_rgba(192,192,192, opacity)
#define APP_COLOR_BLACK                     gui_rgb(0,0,0)
#define APP_COLOR_GRAY                      gui_rgb(128,128,128)
#define APP_COLOR_BLACK_OPACITY(opacity)    gui_rgba(0,0,0, opacity)
#define APP_COLOR_GRAY_OPACITY(opacity)     gui_rgba(128,128,128, opacity)
#define APP_COLOR_TOMATO                    gui_rgb(255,99,71)
#define APP_COLOR_YELLOW                    gui_rgb(255,255,0)
#define APP_COLOR_ORANGE                    gui_rgb(255,165,0)
#define APP_COLOR_SPRING_GREEN              gui_rgb(0,255,127)
#define APP_COLOR_CYAN                      gui_rgb(0,255,255)
#define APP_COLOR_PURPLE                    gui_rgb(128,0,128)


/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * @brief Get a color value from red, green, blue and opacity values.
 *
 * @param red  Range of unsigned char
 * @param green Range of unsigned char
 * @param blue Range of unsigned char
 * @param opacity Range of unsigned char
 * @return gui_color_t
 */
gui_color_t gui_rgba(unsigned char red,
                     unsigned char green,
                     unsigned char blue,
                     unsigned char opacity);

/**
 * @brief Get a color value from red, green and blue.
 *
 * @param red  Range of unsigned char
 * @param green Range of unsigned char
 * @param blue Range of unsigned char
 * @return gui_color_t
 */
gui_color_t gui_rgb(unsigned char red, unsigned char green, unsigned char blue);

/**
 * @brief Parses a CSS color string and converts it to a gui_color_t structure
 *
 * https://www.w3schools.com/cssref/css_colors_legal.php
 * Supports the following formats:
 * - Hex (#RRGGBB or #RRGGBBAA)
 * - RGB (rgb(R, G, B))
 * - RGBA (rgba(R, G, B, A))
 * - HSL (hsl(H, S%, L%))
 * - HSLA (hsla(H, S%, L%, A))
 * - Named colors (e.g., "red", "blue") https://www.w3schools.com/colors/colors_names.asp
 *
 * @param color_str CSS color string
 * @return gui_color_t structure with the parsed color, or a structure with all components set to 0 if invalid
 */
gui_color_t gui_color_css(const char *color_str);

/**
 *
 */
void gui_memset16(uint16_t *addr, uint16_t pixel, uint32_t len);

/**
 *
 */
void gui_memset32(uint32_t *addr, uint32_t pixel, uint32_t len);

/**
 * @brief clear framebuffer with color
 */
void gui_fb_clear(uint8_t *addr, gui_color_t color, uint32_t len/*pixel count*/);  //argb8888
#ifdef __cplusplus
}
#endif
#endif
