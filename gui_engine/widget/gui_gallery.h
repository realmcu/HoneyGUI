/**
\internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_gallery.h
  * @brief gallery widget
  * @details create gallery widget
  * @author roy@realsil.com.cn
  * @date 2024/05/08
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
#ifndef __GUI_GALLERY_H__
#define __GUI_GALLERY_H__

#ifdef __cplusplus
extern "C" {
#endif
/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_api.h"
#include "gui_img.h"

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

/** @brief  ... */


#ifdef  __CC_ARM
#pragma anon_unions
#endif

typedef struct
{
    float center_percent;     // center area percent
    float side_scale;         // side img default scale
    float side_pos_percent;   // side img midline position percent

    void **img_array;
    void *data_main_bg;
    void *data_center_bg;
    uint8_t num_pic;
    uint8_t src_mode[3];   // main_bg, center_bg, pic

} gui_gallery_config_t;

typedef struct
{
    gui_obj_t base;             //!< base structure
    gui_img_t *img_main_bg;
    gui_img_t *img_center_bg;
    gui_img_t *img[3];        // left(up), right(down), center
    gui_img_t *img_bottom[2];   // left(up), right(down)

    void **img_array;
    uint32_t num_pic    : 8;
    uint32_t idx_center : 8;  // count from 0
    uint32_t idx_hold   : 8;  // count from 0, idx center before hold
    uint32_t loop_en    : 1;
    uint32_t src_mode   : 3;
    float center_percent;     // center area percent
    float side_scale;         // side img default scale
    float side_pos_percent;   // side img midline position percent
    int16_t pos_l;            // midline
    int16_t pos_c;
    int16_t pos_r;


    int16_t release_x;
    uint8_t checksum;
} gui_gallery_t;

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

#define RAD(d)        (d*3.1415926f/180.0f)     //!< angle to rad

#define DEFAULT_CENTER_PERCENT (0.667f)     //!< center area percent
#define DEFAULT_SIDE_POS_PERCENT (0.1f)     //!< side area percent
#define DEFAULT_SIDE_SCALE (0.4f)

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

typedef enum
{
    GALLERY_IMG_IDX_LEFT = 0,
    GALLERY_IMG_IDX_CENTER = 1,
    GALLERY_IMG_IDX_RIGHT = 2,
    GALLERY_IMG_IDX_BTM_LEFT = 0,
    GALLERY_IMG_IDX_BTM_RIGHT = 1,

    GALLERY_LEFT  = 0,
    GALLERY_RIGHT = 1,
    GALLERY_CENTER = 2,
} GALLERY_IMG_IDX;



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
 * @brief set the perspective image's blend mode
 *
 * @param perspective the perspective widget pointer
 * @param img_index the perspective image's index
 * @param mode the enumeration value of the mode is BLEND_MODE_TYPE
 *
 */
void gui_gallery_set_mode(gui_gallery_t *perspective,
                          uint8_t            img_index,
                          BLEND_MODE_TYPE    mode);

/**
 * @brief set perspective image
 *
 * @param perspective the perspective widget pointer
 * @param img_file the image file data, set flg_fs true when using filesystem
 */
void gui_gallery_set_img(gui_gallery_t *perspective, gui_gallery_config_t *config);

/**
  * @brief  create 3D perspective, images can be loaded from filesystem or memory address
  * @param  parent parent widget
  * @param  name  widget name
  * @param  img_file the image file data, set flg_fs true when using filesystem
  * @param  x  left
  * @param  y top
  * @return gui_perspective_t* widget pointer
  *
  *
  * <b>Example usage</b>
  * \code{.c}
  * void perspctive_example(void *parent)
  * {
  *     gui_perspective_imgfile_t imgfile =
  *     {
  *         .flg_fs = true,
  *         .img_path[0] = "Clockn.bin",
  *         .img_path[1] = "Weather.bin",
  *         .img_path[2] = "Music.bin",
  *         .img_path[3] = "QuickCard.bin",
  *         .img_path[4] = "HeartRate.bin",
  *         .img_path[5] = "Activity.bin"
  *     };
  *     img_test = gui_perspective_create(canvas, "test", &imgfile, 0, 0);
  * }
  * \endcode
  */
gui_gallery_t *gui_gallery_create(void                      *parent,
                                  const char                *name,
                                  gui_gallery_config_t *config,
                                  int16_t                    x,
                                  int16_t                    y,
                                  int16_t                    w,
                                  int16_t                    h);
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
