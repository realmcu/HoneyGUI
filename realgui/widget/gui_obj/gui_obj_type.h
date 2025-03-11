/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_obj.h
  * @brief  object widget
  * @details create a object
  * @author howie_wang@realsil.com.cn
  * @date 2025/03/07
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
#ifndef __GUI_OBJ_TYPE_H__
#define __GUI_OBJ_TYPE_H__



#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"


/*============================================================================*
 *                         Types
 *============================================================================*/

typedef enum t_obj_type
{
    SCREEN                   = 0x0000,
    WINDOW                   = 0x0001,
    TABVIEW                  = 0x0002,
    TAB                      = 0x0003,
    CURTAINVIEW              = 0x0004,
    CURTAIN                  = 0x0005,
    IMAGE                    = 0x0006,
    BUTTON                   = 0x0007,
    ICONLIST                 = 0x0008,
    ICON                     = 0x0009,
    IMAGE_FROM_MEM           = 0x000A,
    TEXTBOX                  = 0x000B,
    SCROLLTEXTBOX            = 0x000C,
    SEEKBAR                  = 0x000D,
    PROGRESSBAR              = 0x000E,
    CLICKSWITCH              = 0x000F,
    PAGE                     = 0x0010,
    SCROLL_WHEEL             = 0x0011,
    PAGEBAR                  = 0x0012,
    RETURNWIDGET             = 0x0013,
    RECTANGLE                = 0x0014,
    WINDOWWITHBORDER         = 0x0015,
    CANVAS                   = 0x0016,
    VG_LITE_CLOCK            = 0x0017,
    CUBE                     = 0x0018,
    GRID                     = 0x0019,
    RADIO                    = 0x001A,
    RADIOSWITCH              = 0x001B,
    MACRO_CANVAS_ARC         = 0x001C,
    JAVASCRIPT               = 0x001D,
    MOVIE                    = 0x001E,
    IMAGE_SCOPE              = 0x001F,
    CARDVIEW                 = 0x0020,
    CARD                     = 0x0021,
    PAGELIST                 = 0x0022,
    PAGELISTVIEW             = 0x0023,
    MACRO_ANIMATETRANSFORM   = 0x0024,
    HONEYCOMB_LIST           = 0x0025,
    WHEEL_LIST               = 0x0026,
    QRCODE                   = 0x0027,
    GALLERY                  = 0x0028,
    TURN_TABLE               = 0x0029,
    KEYBOARD                 = 0x002A,
    MACRO_MOTORIZED_CURTAIN  = 0x002B,
    MULTI_LEVEL              = 0x002C,
    U8G2                     = 0x002D,
    MACRO_ONCLICK            = 0x002E,
    MACRO_BACKICON           = 0x002F,
    CANVAS_IMG               = 0x0030,
    VG_LITE_SOCCER           = 0x0031,
    SLIDER                   = 0x0032,
    MACRO_ONCHANGE           = 0x0033,
    TYPE_SCROLL_WHEEL_NEW    = 0x0034,
    MACRO_ONON               = 0x0035,
    MACRO_ONOFF              = 0x0036,
    MACRO_ONLOAD,
    MACRO_CALENDAR,
    MACRO_CIRCLE,
    MACRO_SECTOR,
    MACRO_ROUNDED_RECT,
    MACRO_ONTIME,
    MACRO_COMBO,
    MACRO_PAGE_LIST_NEW,
    MACRO_ON_PERIPHERAL,
    GUI_VENDOR_TYPE,
    MACRO_CHART,
    VIEW,
    MACRO_ONCOMPLETE,
    MACRO_KEY,
    MACRO_ONSELECT,
} T_OBJ_TYPE;
typedef enum
{
    OBJ_INPUT_PREPARE,
    OBJ_PREPARE,
    OBJ_DRAW,
    OBJ_END,
    OBJ_DESTROY,
} T_OBJ_CB_TYPE;

typedef struct _gui_obj_t
{
    const char *name;
    struct _gui_obj_t *parent;//point to father obj

    int16_t x;
    int16_t y;
    int16_t w;
    int16_t h;

    gui_list_t  child_list;
    gui_list_t  brother_list;

    //start of run time
    void (* obj_cb)(struct _gui_obj_t *obj, T_OBJ_CB_TYPE cb_type);
    //end of run time
    T_OBJ_TYPE type;                      //no need this , only use name
    uint32_t active               : 1;    // this_widget flag means obj location in screen
    uint32_t not_show             : 1;

    uint32_t skip_tp_left_hold    : 1;
    uint32_t skip_tp_right_hold   : 1;
    uint32_t skip_tp_up_hold      : 1;
    uint32_t skip_tp_down_hold    : 1;
    uint32_t skip_tp_short        : 1;
    uint32_t skip_tp_long         : 1;
    uint32_t skip_tp_pressed      : 1;
    uint32_t gesture              : 1;

    uint32_t create_done          : 1;
    uint32_t event_dsc_cnt        : 5;
    uint32_t opacity_value        : 8;
    uint32_t has_input_prepare_cb : 1;
    uint32_t has_prepare_cb       : 1;
    uint32_t has_draw_cb          : 1;
    uint32_t has_end_cb           : 1;
    uint32_t has_destroy_cb       : 1;
    uint32_t magic                : 4;
    gui_event_dsc_t *event_dsc;

    gui_matrix_t *matrix;

} gui_obj_t;


/*============================================================================*
*                         Macros
*============================================================================*/


/*============================================================================*
 *                         Functions
 *============================================================================*/



#ifdef __cplusplus
}
#endif

#endif
