/*
 * File      : gui_text.h
 */
#ifndef __GUI_TEXT_H__
#define __GUI_TEXT_H__



#ifdef __cplusplus
extern "C" {
#endif

#include <guidef.h>

/**********************
 *      TYPEDEFS
 **********************/
/*Data of text*/
typedef enum
{
    LEFT = 0,
    CENTER,
    RIGHT,
    MUTI_LEFT,
    MUTI_CENTER,
    MUTI_RIGHT,
    SCROLL_X,
    SCROLL_Y,
} TEXT_MODE;
typedef struct gui_text_line
{
    uint8_t line_char;
    uint8_t line_dx;
} gui_text_line_t;

typedef struct gui_text
{
    gui_obj_t base;
    const struct rtgui_font_engine *engine;
    uint32_t color;
    TEXT_MODE mode;
    uint16_t len;
    uint16_t font_len;
    uint16_t text_offset;
    uint8_t font_height;
    uint8_t *utf_8;
    void *data;
    char *text_type;// "rtk from mem" or "rtk from fs"
    gui_animate_t *animate;
    // uint8_t font_width;

    void *path;// address or path
    // int fd_table;
    // int fd_dot;
} gui_text_t;

/**
* @brief set the string in a text box widget.
* @note the area of the text box should be larger than that of the string to be shown.
* @param this the text box widget pointer.
* @param text the text string.
* @param font_type the font's name.
* @param color the text's color.
* @param length the text string's length.
* @param font_size the text string's font size.
* @return void
*/
void gui_text_set(gui_text_t *this, const char *text, char *text_type, uint32_t color,
                  uint16_t length, uint8_t font_size);
void gui_text_ctor(gui_text_t *this, gui_obj_t *parent, const char *name, int16_t x, int16_t y,
                   int16_t w, int16_t h);
void gui_text_move(gui_text_t *this, int16_t x, int16_t y);
/**
 * @brief set text mode of this text widget
 * @note if text line count was more than one, it will display on the left even if it was set lft or right
 * @param this the text widget pointer.
 * @param mode there was three mode: LEFT, CENTER and RIGHT.
 */
void gui_text_mode_set(gui_text_t *this, TEXT_MODE mode);
/**
 * @brief set font size or width and height
 * @note if use freetype, width and height is effective, else height will be applied as font size
 * @param this the text box widget pointer.
 * @param height font height or font size.
 * @param width font width(only be effective when freetype was used).
 */
void gui_text_size_set(gui_text_t *this, uint8_t height, uint8_t width);

/**
 * @brief create a text box widget.
 * @note the area of the text box should be larger than that of the string to be shown.
 * @param parent the father widget which the text nested in.
 * @param filename the widget's name.
 * @param x the X-axis coordinate of the text box.
 * @param x the Y-axis coordinate of the text box.
 * @param w the width of the text box.
 * @param h the hight of the text box.
 * @return return the widget object pointer
 *
 */
gui_text_t *gui_text_create(void *parent, const char *filename, int16_t x, int16_t y,
                            int16_t w, int16_t h);
/**
 * @brief set animate
 *
 * @param o text widget
 * @param dur durtion. time length of the animate
 * @param repeatCount  0:one shoot -1:endless
 * @param callback happens at every frame
 * @param p callback's parameter
 */
void gui_text_set_animate(void *o, uint32_t dur, int repeatCount, void *callback, void *p);

#ifdef __cplusplus
}
#endif

#endif

