/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_soccer.h
  * @brief soccer widget
  * @details soccer widget
  * @author howie_wang@realsil.com.cn
  * @date 2024/08/16
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
#ifndef __GUI_SOCCER_H__
#define __GUI_SOCCER_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <guidef.h>
#include <gui_api.h>
#include "gui_img.h"
#include "gui_perspective.h"


/*============================================================================*
 *                         Types
 *============================================================================*/

typedef struct
{
    float w;
    float x;
    float y;
    float z;
} gui_quaternion_t;

/** @brief  SOCCER widget structure */
#ifdef  __CC_ARM
#pragma anon_unions
#endif
typedef struct gui_soccer
{
    gui_obj_t base;                 //!< base structure
    draw_img_t draw_img[20];         // please refer to enum T_SOCCER_SIDE_TYPE.
    float xrot;
    float yrot;
    float zrot;
    float c_x;
    float c_y;
    float scsize;
    int press_face;
    gui_vertex_t normal[20];
    gui_quaternion_t rotation;
    // float nz[20];
    float slide_range;

} gui_soccer_t;

/*============================================================================*
 *                         Constants
 *============================================================================*/


/*============================================================================*
 *                         Macros
 *============================================================================*/

#define RAD(d)        (d*3.1415926f/180.0f) //!< angle to rad
#define PHI           ((1 + sqrt(5)) / 2)


/*============================================================================*
 *                         Variables
 *============================================================================*/


/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * @brief soccer create, images can be loaded from filesystem or memory address
 *
 * @param parent parent widget
 * @param name  widget name
 * @param addr the image file data
 * @param x  left
 * @param y top
 * @return gui_soccer_t* widget pointer
 *
 * <b>Example usage</b>
 * \code{.c}
 *{
 *    gui_soccer_imgfile_t imgfile =
 *    {
 *        .flg_fs = true,
 *        .img_path.img_path_front = "Activity.bin",
 *        .img_path.img_path_back = "Weather.bin",
 *        .img_path.img_path_up = "HeartRate.bin",
 *        .img_path.img_path_down = "Clockn.bin",
 *        .img_path.img_path_left = "Music.bin",
 *        .img_path.img_path_right = "QuickCard.bin"
 *    };
 *
 *    gui_soccer_t *soccer4 = gui_soccer_create(parent, "soccer", &imgfile, 0, 0);
 *    gui_soccer_set_size(soccer4, 100);
 *    gui_soccer_set_center(soccer4, 200, 200);
 *}
 * \endcode
 */
gui_soccer_t *gui_soccer_create(void               *parent,
                                const char         *name,
                                uint32_t           *frame_array[],
                                int16_t             x,
                                int16_t             y);


/**
 * @brief get the soccer image's blend mode
 *
 * @param soccer the soccer widget pointer
 * @return the soccer image's blend mode
 *
 */
BLEND_MODE_TYPE gui_soccer_get_mode(gui_soccer_t *soccer);

/**
 * @brief set the soccer image's blend mode
 *
 * @param soccer the soccer widget pointer
 * @param mode the enumeration value of the mode is BLEND_MODE_TYPE
 *
 */
void gui_soccer_set_mode(gui_soccer_t *soccer, BLEND_MODE_TYPE mode);

/**
 * @brief get the soccer image's opacity
 *
 * @param soccer the cube widget pointer
 * @return the soccer image's opacity
 *
 */
uint8_t gui_soccer_get_opacity(gui_soccer_t *soccer);

/**
 * @brief set the soccer image's opacity
 *
 * @param soccer the soccer widget pointer
 * @param opacity the soccer image's opacity
 *
 */
void gui_soccer_set_opacity(gui_soccer_t *soccer, uint8_t opacity);

/**
 * @brief set soccer image
 *
 * @param cube the soccer widget pointer
 * @param addr the image file data
 */
void gui_soccer_set_img(gui_soccer_t *soccer, uint32_t *frame_array[]);

/**
 * @brief set center
 *
 * @param this widget pointer
 * @param c_x center x
 * @param c_y center y
 */
void gui_soccer_set_center(gui_soccer_t *this, float c_x, float c_y);

/**
 * @brief set size
 *
 * @param this widget pointer
 * @param size scale size
 */
void gui_soccer_set_size(gui_soccer_t *this, float size);

/**
 * @brief set slide range
 *
 * @param this widget pointer
 * @param range slide range
 */
void gui_soccer_set_slide_range(gui_soccer_t *this, float range);

/**
 * @brief set on_click event
 *
 * @param this widget pointer
 * @param callback callback function
 * @param parameter callback function parameter
 */
void gui_soccer_on_click(gui_soccer_t *this, void *callback, void *parameter);
/**
 * @brief Creates and initializes a new GUI soccer instance (ftl address mode).
 *
 * This function sets up a new soccer GUI widget. It initializes the
 * necessary components and prepares it for display.
 *
 * @param parent Pointer to the parent GUI component. This is typically a window
 *               or a container that will hold the soccer game widget.
 * @param name A string representing the name of the GUI soccer widget. This
 *             can be used for identification purposes within the parent.
 * @param frame_array Array of pointers to uint32_t, representing the frames of the
 *                    soccer game animation or graphics (ftl address mode).
 * @param x The x-coordinate where the soccer widget will be placed within the
 *          parent component's coordinate system.
 * @param y The y-coordinate where the soccer widget will be placed within the
 *          parent component's coordinate system.
 *
 * @return A pointer to the newly created `gui_soccer_t` instance, or NULL if
 *         the creation fails due to memory allocation issues or invalid parameters.
 */
gui_soccer_t *gui_soccer_create_ftl(void               *parent,
                                    const char         *name,
                                    uint32_t           *frame_array[],
                                    int16_t             x,
                                    int16_t             y);

#ifdef __cplusplus
}
#endif

#endif
