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
#ifndef __GUI_IMG_LIVE_H__
#define __GUI_IMG_LIVE_H__
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
#include "gui_fb.h"

/** @defgroup WIDGET WIDGET
  * @brief
  * @{
  */

/*============================================================================*
 *                         Types
 *============================================================================*/
/** @defgroup WIDGET_Exported_Types WIDGET Exported Types
  * @brief
  * @{
  */

typedef enum
{
    IMG_LIVE_MJPEG,

} GUI_IMG_LIVE_ENUM;



/** @brief  stb img widget information structure */
typedef struct
{
    gui_obj_t base;
    gui_img_t *img;

    void *data;                 // mem address or filesystem path
    uint32_t num_frame;
    uint8_t **array;            // mjpeg: jpeg array
    void *frame_buff;
    float fps;
    uint32_t frame_cur;
    uint8_t img_type: 3;
    uint8_t src_mode: 3;

} gui_img_live_t;



/* Session identifier for input/output functions (name, members and usage are as user defined) */
typedef struct
{
    void *mem_addr;               /* Input stream */
    uint8_t *fbuf;          /* Output frame buffer */
    unsigned int wfbuf;     /* Width of the frame buffer [pix] */
    uint8_t state;          /* input state: 0 init, 1 inputing */
} IODEV;

/** End of WIDGET_Exported_Types
  * @}
  */

/*============================================================================*
 *                         Constants
 *============================================================================*/
/** @defgroup WIDGET_Exported_Constants WIDGET Exported Constants
  * @brief
  * @{
  */


/** End of WIDGET_Exported_Constants
  * @}
  */

/*============================================================================*
 *                         Macros
 *============================================================================*/
/** @defgroup WIDGET_Exported_Macros WIDGET Exported Macros
  * @brief
  * @{
  */

#define IMG_LIVE_FPS (25)  // default 25 fps, 40 ms
// #define IMG_LIVE_TJPGDEC

/** End of WIDGET_Exported_Macros
 * @}
 */

/*============================================================================*
 *                         Variables
 *============================================================================*/
/** @defgroup WIDGET_Exported_Variables WIDGET Exported Variables
  * @brief
  * @{
  */


/** End of WIDGET_Exported_Variables
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/
/** @defgroup WIDGET_Exported_GUI_Functions WIDGET Exported Functions
  * @brief
  * @{
  */

/**
 * @brief Creat a image live widget with buffer.
 * @note This widget is use to display live image which can be play.
 * @note The data of image can be static char or data transformed by BLE.
 * @param parent The father widget .
 * @param name The widget's name.
 * @param addr The data address of image.
 * @param x The X-axis coordinate .
 * @param y The Y-axis coordinate .
 * @param w The widget width .
 * @param h The widget hight .
 * @return gui_img_live_t*
 */
gui_img_live_t *gui_img_live_create_from_mem(void           *parent,
                                             const char     *name,
                                             void           *addr,
                                             int16_t         x,
                                             int16_t         y,
                                             int16_t         w,
                                             int16_t         h);

/** End of WIDGET_Exported_GUI_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */


#ifdef __cplusplus
}
#endif

#endif
