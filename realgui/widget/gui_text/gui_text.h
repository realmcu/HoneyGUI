/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_text.h
  * @brief Text widget header file.
  * @details Text widget header file.
  * @author luke_sun@realsil.com.cn
  * @date 2023/10/25
  * @version v1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
 \endinternal
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_TEXT_H__
#define __GUI_TEXT_H__
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
#include "draw_font.h"
#include "gui_win.h"

/*============================================================================*
 *                         Types
 *============================================================================*/

/* TEXT_MODE start */
typedef enum
{
    /* TOP */
    LEFT               = 0x00,
    CENTER             = 0x01,
    RIGHT              = 0x02,
    MULTI_LEFT         = 0x03,
    MULTI_CENTER       = 0x04,
    MULTI_RIGHT        = 0x05,
    /* MID */
    MID_LEFT           = 0x10,
    MID_CENTER         = 0x11,
    MID_RIGHT          = 0x12,
    /* SCROLL */
    SCROLL_X           = 0x30,
    SCROLL_Y           = 0x31,
    SCROLL_Y_REVERSE   = 0x32,
    SCROLL_X_REVERSE   = 0x33,
    /* VERTICAL */
    VERTICAL_LEFT      = 0x40,
    VERTICAL_RIGHT     = 0x41,
} TEXT_MODE;
/* TEXT_MODE end */

/** @brief Font type enum. */
typedef enum
{
    GUI_FONT_SRC_BMP = 0,
    GUI_FONT_SRC_STB = 1,
    GUI_FONT_SRC_IMG = 2,
    GUI_FONT_SRC_MAT = 3,
    GUI_FONT_SRC_FT  = 4,
    GUI_FONT_SRC_TTF = 5,
} FONT_SRC_TYPE;

typedef enum
{
    FONT_SRC_MEMADDR = 0,
    FONT_SRC_FILESYS = 1,
    FONT_SRC_FTL     = 2,
} FONT_SRC_MODE;

/** @brief Text widget structure. */
typedef struct gui_text
{
    gui_obj_t base;
    gui_color_t color;
    gui_animate_t *animate;
    gui_img_t *scale_img;
    uint8_t *emoji_path;
    float min_scale;
    void *content;
    void *data;
    void *path;
    gui_matrix_t *matrix;

    uint16_t len;
    uint16_t font_len;
    uint16_t active_font_len;
    int16_t char_width_sum;
    int16_t char_height_sum;
    int16_t char_line_sum;
    int16_t offset_x;
    int16_t offset_y;

    TEXT_MODE mode;
    TEXT_CHARSET charset;
    FONT_SRC_TYPE font_type;
    FONT_SRC_MODE font_mode;
    uint8_t font_height;
    uint8_t emoji_size;
    uint8_t checksum;

    bool layout_refresh   : 1;
    bool content_refresh  : 1;
    bool use_img_blit     : 1;
    uint8_t inputable     : 1;
    uint8_t ispasswd      : 1;
    uint8_t wordwrap      : 1;
    uint8_t scope         : 1;
    uint8_t rendermode    : 2;
} gui_text_t;

/** @brief Text line structure. */
typedef struct gui_text_line
{
    uint16_t line_char;
    uint16_t line_dx;
} gui_text_line_t;

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
 * @brief Set textbox click event callback.
 *
 * @param this_widget Text widget.
 * @param event_cb Callback function.
 * @param parameter Callback parameter.
 */
void gui_text_click(gui_text_t *this_widget, gui_event_cb_t event_cb, void *parameter);

/**
 * @brief Set textbox password done event callback to handle password.
 *
 * @param this_widget Text widget.
 * @param event_cb Callback function.
 * @param parameter Callback parameter.
 */
void gui_text_pswd_done(gui_text_t *this_widget, gui_event_cb_t event_cb, void *parameter);

/**
 * @brief Set the string in a text box widget.
 * @note The font size must match the font file.
 * @param this_widget The text box widget pointer.
 * @param text The text string.
 * @param text_type Text type.
 * @param color The text's color.
 * @param length The text string's length.
 * @param font_size The text string's font size.
 */
void gui_text_set(gui_text_t    *this_widget,
                  void          *text,
                  FONT_SRC_TYPE  text_type,
                  gui_color_t    color,
                  uint16_t       length,
                  uint8_t        font_size);

/**
 * @brief Set animate.
 *
 * @param o Text widget.
 * @param dur Duration. Time length of the animate.
 * @param repeat_count 0: One shoot, -1: Endless.
 * @param callback Happens at every frame.
 * @param p Callback's parameter.
 */
void gui_text_set_animate(void     *o,
                          uint32_t  dur,
                          int       repeat_count,
                          void     *callback,
                          void     *p);

/**
 * @brief Set text mode of this_widget text widget.
 * @note If text line count was more than one, it will display on the left even if it was set left or right.
 * @param this_widget The text widget pointer.
 * @param mode There are three modes: LEFT, CENTER, and RIGHT.
 */
void gui_text_mode_set(gui_text_t *this_widget, TEXT_MODE mode);

/**
 * @brief Set inputable.
 *
 * @param this_widget The text box widget pointer.
 * @param inputable Inputable.
 */
void gui_text_input_set(gui_text_t *this_widget, bool inputable);

/**
 * @brief By setting wordwrap to enable English word wrapping.
 *
 * @param this_widget The text box widget pointer.
 * @param wordwrap Wordwrap.
 */
void gui_text_wordwrap_set(gui_text_t *this_widget, bool wordwrap);

/**
 * @brief Enable/disable matrix-based image rendering for text.
 * @param this Pointer to the text widget.
 * @param use_img_blit True = Use image matrix blitting (for complex transformations),
 *                     False = Use standard rendering.
 */
void gui_text_use_matrix_by_img(gui_text_t *this_widget, bool use_img_blit);

/**
 * @brief Set TTF raster render mode.
 *
 * @param this_widget The text box widget pointer.
 * @param rendermode Render mode. 1/2/4/8.
 */
void gui_text_rendermode_set(gui_text_t *this_widget, uint8_t rendermode);

/**
 * @brief Set text minimum scale.
 *
 * @param this The text box widget pointer.
 * @param min_scale Minimum scale.
 */
void gui_text_set_min_scale(gui_text_t *this_widget, float min_scale);

/**
 * @brief Move the text widget.
 *
 * @param this_widget The text box widget pointer.
 * @param x The X-axis coordinate of the text box.
 * @param y The Y-axis coordinate of the text box.
 */
void gui_text_move(gui_text_t *this_widget, int16_t x, int16_t y);

/**
 * @brief Set font size or width and height.
 * @note If using FreeType, width and height are effective; otherwise, height will be applied as font size.
 * @param this_widget The text widget pointer.
 * @param height Font height or font size.
 * @param width Font width (only effective when FreeType is used).
 */
void gui_text_size_set(gui_text_t *this_widget, uint8_t height, uint8_t width);

/**
 * @brief Set text font mode.
 *
 * @param this_widget The text widget pointer.
 * @param font_mode Font source mode.
 */
void gui_text_font_mode_set(gui_text_t *this_widget, FONT_SRC_MODE font_mode);

/**
 * @brief Set font type.
 * @note The type must match the font size.
 * @param this_widget The text widget pointer.
 * @param font_source The address of .ttf or .bin.
 * @param font_mode Font source mode.
 */
void gui_text_type_set(gui_text_t *this_widget, void *font_source, FONT_SRC_MODE font_mode);

/**
 * @brief Set emoji file path and emoji size.
 * @note Requires ROMFS.
 * @note Example of a full emoji image file path: "font/emoji/emoji_u1f30d.bin".
 * @param this The text widget pointer.
 * @param path Path containing folder path and file name prefix. Path example: "font/emoji/emoji_u".
 * Folder path is the emoji image file folder path, e.g., "font/emoji/".
 * File name prefix is the prefix before the filename for Unicode sorting, e.g., "emoji_u".
 * @param size Emoji image file size, e.g., 32.
 */
void gui_text_emoji_set(gui_text_t *this_widget, uint8_t *path, uint8_t size);

/**
 * @brief Set font encoding.
 * @note UTF-8 or Unicode.
 * @param this_widget The text widget pointer.
 * @param encoding_type Encoding type.
 */
void gui_text_encoding_set(gui_text_t *this_widget, TEXT_CHARSET charset);

/**
 * @brief Set text matrix.
 * @note
 * @param this_widget The text widget pointer.
 * @param encoding_type Encoding type.
 */
void gui_text_set_matrix(gui_text_t *this_widget, gui_matrix_t *matrix);

/**
 * @brief Set text content.
 *
 * @param this_widget The text widget pointer.
 * @param text The text string.
 * @param length The text string's length.
 */
void gui_text_content_set(gui_text_t *this_widget, void *text, uint16_t length);

/**
 * @brief Draw text by image so that text can be scaled.
 *
 * @param this_widget The text widget pointer.
 * @param font_img_type Color format.
 */
void gui_text_convert_to_img(gui_text_t *this_widget, GUI_FormatType font_img_type);

/**
 * @brief Create a text box widget.
 * @note The area of the text box should be larger than that of the string to be shown; otherwise, part of the text will be hidden.
 * @param parent The parent widget in which the text is nested.
 * @param filename The widget's name.
 * @param x The X-axis coordinate of the text box.
 * @param y The Y-axis coordinate of the text box.
 * @param w The width of the text box.
 * @param h The height of the text box.
 * @return Returns the widget object pointer.
 */
gui_text_t *gui_text_create(void       *parent,
                            const char *name,
                            int16_t     x,
                            int16_t     y,
                            int16_t     w,
                            int16_t     h);



#ifdef __cplusplus
}
#endif

#endif
