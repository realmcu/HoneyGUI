/**
\internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_img_stb.c
  * @brief stb image widget
  * @details stb image widget is used to show image which is not binary file on the screen
  * @author howie_wang@realsil.com.cn
  * @date 2023/10/25
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
#ifndef __GUI_IMG_STB_H__
#define __GUI_IMG_STB_H__
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


/*============================================================================*
 *                         Types
 *============================================================================*/

/** @brief  stb img widget gif information structure */
typedef struct gui_img_stb_gif_info
{
    uint32_t init_time_ms;
    uint32_t *delay_ms;
    uint32_t cycle_index;
    uint32_t delay_ms_sum;
    uint32_t total_frame;
    uint32_t current_frame;
} gui_gif_info_t;

/** @brief  stb img widget information structure */
typedef struct gui_img_stb
{
    gui_obj_t base;
    gui_img_t *img;
    bool src_changed;
    GUI_FormatType input_format;
    GUI_FormatType output_format;
    void *data_buffer;
    uint32_t data_length;
    gui_gif_info_t *gif_info;
    int16_t offset_x;
    int16_t offset_y;
} gui_stb_img_t;

/* User defined session identifier */
typedef struct
{
    uint8_t *data;     /* Pointer to the jpeg data */
    uint32_t size;      /* Size of the jpeg data */
    uint32_t index;     /* Current position in jpeg data */
    uint8_t *fbuf;  /* Pointer to the work area */
    uint8_t *image; /* JPG image read into this buffer */
} IODEV;

/*============================================================================*
 *                         Constants
 *============================================================================*/


/*============================================================================*
 *                         Macros
 *============================================================================*/

//Use tjpgd to replace jpeg decoding in stb
// #define TJPG


/*============================================================================*
 *                         Variables
 *============================================================================*/


/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * @brief set stb image widget attribute
 *
 * @param this stb image widget
 * @param addr image address
 * @param size image file size by Byte
 * @param type image type
 * @param x X-axis coordinate
 * @param y Y-axis coordinate
 */
void gui_img_stb_set_attribute(gui_stb_img_t  *this,
                               void           *addr,
                               uint32_t        size,
                               GUI_FormatType  type,
                               int16_t         x,
                               int16_t         y);
/**
 * @brief set stb image widget attribute without free rgb data
 *
 * @param this stb image widget
 * @param addr image address
 * @param size image file size by Byte
 * @param type image type
 * @param x X-axis coordinate
 * @param y Y-axis coordinate
 */
void gui_img_stb_set_attribute_static(gui_stb_img_t  *this,
                                      void           *addr,
                                      uint32_t        size,
                                      GUI_FormatType  type,
                                      int16_t         x,
                                      int16_t         y);
/**
 * @brief Creat a image widget with buffer.
 * @note This widget is use to display image which needs to decode.
 * @note The data of image can be static char or data transformed by BLE.
 * @param parent The father widget which the scroll text nested in.
 * @param name The widget's name.
 * @param addr The data address of image.
 * @param size The data size of image.The unit is bytes.
 * @param input_type The input type of image. bmp 11, jpeg 12, png 13,
 * @param output_type The output type of image. RGB565 RGB888,
 * @param x The X-axis coordinate of the text box.
 * @param x The Y-axis coordinate of the text box.
 * @return gui_stb_img_t*
 */
gui_stb_img_t *gui_img_stb_create_from_mem(void           *parent,
                                           const char     *name,
                                           void           *addr,
                                           uint32_t        size,
                                           GUI_FormatType  input_type,
                                           GUI_FormatType  output_type,
                                           int16_t         x,
                                           int16_t         y);

/**
 * @brief set output_format
 * @note RGB565 or RGB888
 * @param this stb image widget
 * @param output output format
 */
void gui_img_stb_set_output_format(gui_stb_img_t  *this, GUI_FormatType output);


#ifdef __cplusplus
}
#endif

#endif
