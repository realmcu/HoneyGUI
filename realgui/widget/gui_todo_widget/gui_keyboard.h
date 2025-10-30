/**
\internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_keyboard.h
  * @brief keyboard widget
  * @details keyboard widget
  * @author roy@realsil.com.cn
  * @date 2024/5/20
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
#ifndef __GUI_KEYBOARD_H__
#define __GUI_KEYBOARD_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_api.h"
#include "draw_img.h"
#include "gui_img.h"
#include "gui_button.h"
#include "gui_switch.h"
#include "gui_win.h"
#include "gui_pinyinIME.h"


/*============================================================================*
 *                         Types
 *============================================================================*/

/** @brief  image widget structure */
#ifdef  __CC_ARM
#pragma anon_unions
#endif

typedef struct
{
    uint8_t layout;
    uint8_t mode;
    uint8_t ime;

    void **img_array;
    uint16_t num_pic;
    uint16_t file_mark[2];   // func, other start point
} gui_kb_config_t;

typedef struct
{
    gui_obj_t base;

    // config
    uint32_t layout       : 3; // layout type
    uint32_t input_Method : 3; // input method type
    uint32_t mode         : 3; // input mode   EN/CN/NUM/CHAR...
    uint32_t last_mode    : 3; // input mode   EN/CN/NUM/CHAR...
    uint32_t is_cn        : 1; // en:0, cn:1
    uint32_t caps_Lk      : 1; // caps
    uint32_t symbol_Lk    : 1; // symbol lock
    uint32_t ispasswd     : 1;
    uint32_t num_pic      : 16;

    gui_win_t *win_letter;
    gui_win_t *win_num;
    gui_win_t *win_func;
    gui_win_t *win_ime;
    gui_img_t *img_bg;
    gui_img_t *img_box;
    gui_text_t *txt_display; // display buffer txt
    gui_text_t *txt_input;   // call txtbox
    gui_ime_t *ime;
    void **img_array;
    uint16_t file_mark[2];   // func, other start point
    float scale;             // refer to 640*360, keep screen horizontal full

    uint8_t checksum;
} gui_kb_t;


/*============================================================================*
 *                         Constants
 *============================================================================*/

typedef enum
{
    KB_LAYOUT_BASIC,

    KB_MODE_BASIC_ENG_LOWWER = 0,
    KB_MODE_BASIC_ENG_UPPER,
    KB_MODE_BASIC_PY,
    KB_MODE_BASIC_NUM,
    KB_MODE_BASIC_SYMBOL_EN,
    KB_MODE_BASIC_SYMBOL_CN,

// img src type to get, // src order
    KB_SRC_DEFAULT    = 0,
    KB_SRC_CLK        = 1,
    KB_SRC_LOWWER     = 0,
    KB_SRC_LOWWER_CLK = 1,
    KB_SRC_UPPER      = 2,
    KB_SRC_UPPER_CLK  = 3,

// key type
    KB_AREA_LETTER    = 0,
    KB_AREA_NUM       = 1,
    KB_AREA_FUNC      = 2,
    KB_AREA_SYMBOL    = 3,
    KB_AREA_OTHER     = 4,
// input method
    KB_METHOD_NULL = 0,
    KB_METHOD_PINYIN,

// key size
    // LAYOUT basic

// key index
    // func key index
    // LAYOUT basic
    KB_SW_IDX_CAP = 0,

    KB_BTN_IDX_DEL,
    KB_SW_IDX_SYMBOL,
    KB_SW_IDX_NUMBER,
    KB_BTN_IDX_LS,    // Left shift
    KB_BTN_IDX_SP,
    KB_BTN_IDX_RS,    // Right shift
    KB_SW_IDX_CNEN,
    KB_BTN_IDX_DONE,

    KB_BTN_IDX_COMIC,
    KB_BTN_IDX_ENTER,

    KB_SW_IDX_LOCK,

    KB_BASIC_FUNC_NUM,

// other img idx
    KB_IMG_IDX_INPUTBOX = 0,
    KB_IMG_IDX_GRIDL,
    KB_IMG_IDX_GRIDR,
    KB_IMG_IDX_BG,
    KB_IMG_IDX_CAND_LEFT,
    KB_IMG_IDX_CAND_RIGHT,

    KB_IMG_OTHER_NUM,


// event
    KB_EVENT_PSWD_DONE,


} KEYBOARD_CONST;


/*============================================================================*
 *                         Macros
 *============================================================================*/

// 32bit : 3bit pre
#define KB_VALUE_PACK(pre, idx) (((uint32_t)pre << 29)|idx)
#define KB_VALUE_AREA(val) ((uint32_t)val >> 29)
#define KB_VALUE_IDX(val) ((uint32_t)val & (0XFFFFFFFF >> 3))
/** End of WIDGET_Exported_Macros
  * @}
  */

/*============================================================================*
 *                         Variables
 *============================================================================*/


/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * @brief get input password
 * @note
 * @return password buffer
 */
char *gui_kb_get_pswd(void);

/**
 * @brief set history password
 * @note
 * @param pswd  history password string
 * @param pswd_len  history password string length
 */
void gui_kb_set_pswd(char *pswd, uint8_t pswd_len);

/**
 * @brief launch keyboard input
 * @note
 * @param obj the widget call
 */
void gui_keyboard_launch(gui_obj_t *obj);

/**
 * @brief launch keyboard input from widget event
 * @note
 * @param obj the widget call
 * @param e   the event cb
 * @param obj the widget call
 */
void gui_keyboard_launch_by_widget(void *obj, gui_event_t e, void *param);

/**
 * @brief creat an image widget from memory address
 * @note creat an image widget and set attribute
 * @param parent the father widget it nested in.
 * @param name widget name.
 * @param addr bin file address.
 * @param x the X-axis coordinate of the widget.
 * @param y the Y-axis coordinate of the widget.
 * @param w the width of the widget.
 * @param h the height of the widget.
 * @return return the widget object pointer.
 */
gui_kb_t *gui_keyboard_create(void       *parent,
                              const char *name,
                              gui_kb_config_t *config,
                              int16_t     x,
                              int16_t     y,
                              int16_t     w,
                              int16_t     h);


#ifdef __cplusplus
}
#endif

#endif
