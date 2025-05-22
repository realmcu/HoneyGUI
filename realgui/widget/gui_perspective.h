/**
\internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_perspective.h
  * @brief Perspective widget.
  * @details Create 3D widget.
  * @author howie_wang@realsil.com.cn
  * @date 2023/10/20
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
#ifndef __GUI_PERSPECTIVE_H__
#define __GUI_PERSPECTIVE_H__

#ifdef __cplusplus
extern "C" {
#endif
/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_api.h"
#include "gui_img.h"


/*============================================================================*
 *                         Types
 *============================================================================*/

#ifdef  __CC_ARM
#pragma anon_unions
#endif

/* perspective imgfile structure start*/
typedef struct
{
    IMG_SOURCE_MODE_TYPE src_mode[6];                    //!< Flag: indicate file src.
    union
    {
        char *img_path[6];               //!< Images file path.
        void *data_addr[6];              //!< Images memory address.
    };
} gui_perspective_imgfile_t;
/* perspective imgfile structure end*/

typedef struct gui_perspective
{
    gui_obj_t base;             //!< Base structure.
    draw_img_t img[6];
    int ry[6];
    int temp[6];

    union
    {
        void *file_path[6];                         // Keep the filepath for partial draw.
        void *ftl[6];
    };
    void *data[6];
    uint8_t src_mode[6];

    int16_t release_x;
    uint8_t checksum;
} gui_perspective_t;

/*============================================================================*
 *                         Constants
 *============================================================================*/


/*============================================================================*
 *                         Macros
 *============================================================================*/

#define RAD(d)        (d*3.1415926f/180.0f)     //!< Angle to rad.


/*============================================================================*
 *                         Variables
 *============================================================================*/


/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * @brief Set the perspective image's blend mode.
 *
 * @param perspective The perspective widget pointer.
 * @param img_index The perspective image's index.
 * @param mode The enumeration value of the mode is BLEND_MODE_TYPE.
 *
 */
void gui_perspective_set_mode(gui_perspective_t *perspective,
                              uint8_t            img_index,
                              BLEND_MODE_TYPE    mode);

/**
 * @brief Set perspective image.
 *
 * @param perspective The perspective widget pointer.
 * @param img_file The image file data, set flg_fs true when using filesystem.
 */
void gui_perspective_set_img(gui_perspective_t *perspective, gui_perspective_imgfile_t *img_file);

/**
  * @brief Create 3D perspective, images can be loaded from filesystem or memory address.
  * @param parent Parent widget.
  * @param name Widget name.
  * @param img_file The image file data, set flg_fs true when using filesystem.
  * @param x Left.
  * @param y Top.
  * @return gui_perspective_t* Widget pointer.
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
gui_perspective_t *gui_perspective_create(void                      *parent,
                                          const char                *name,
                                          gui_perspective_imgfile_t *img_file,
                                          int16_t                    x,
                                          int16_t                    y);


#ifdef __cplusplus
}
#endif

#endif
