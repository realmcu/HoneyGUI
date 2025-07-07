/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_3d.h
  * @brief 3D widget.
  * @details 3D widget.
  * @author sienna_shen@realsil.com.cn
  * @date 2025/3/7
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
#ifndef __GUI_3D_H__
#define __GUI_3D_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_api.h"
#include "gui_obj.h"
#include "def_3d_common.h"
#include "def_3d_rect.h"
#include "def_3d_tria.h"

/*============================================================================*
 *                         Types
 *============================================================================*/
typedef enum
{
    GUI_3D_DRAW_FRONT_ONLY,
    GUI_3D_DRAW_FRONT_AND_BACK,
    GUI_3D_DRAW_FRONT_AND_SORT,
} GUI_3D_DRAW_TYPE;

typedef struct gui_3d
{
    gui_obj_t base;

    gui_3d_description_t *desc;
    union
    {
        gui_3d_tria_face_t *tria_face;
        gui_3d_rect_face_t *rect_face;
    } face;

    GUI_3D_DRAW_TYPE draw_type;

    draw_img_t *img;          // Material image.
    draw_img_t *mask_img;     // Mask image for light.
    draw_img_t *combined_img; // Full frame buffer.

    gui_3d_world_t world;
    gui_3d_camera_t camera;
    gui_3d_light_t light;

    void (*global_transform_cb)(struct gui_3d *this);
    gui_3d_matrix_t (*face_transform_cb)(struct gui_3d *this, size_t face_index);

} gui_3d_t;

typedef void (*gui_3d_global_transform_cb)(gui_3d_t *this);

typedef void (*gui_3d_face_transform_cb)(gui_3d_t *this, size_t face_index/*face offset*/);


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
 * @brief Create 3D widget.
 *
 * @param parent Parent widget.
 * @param name Widget name.
 * @param desc_addr Description file data.
 * @param x The X-axis coordinate relative to parent widget.
 * @param y The Y-axis coordinate relative to parent widget.
 * @param w Width.
 * @param h Height.
 * @return The widget object pointer.
 */
gui_3d_t *gui_3d_create(void                 *parent,
                        const char           *name,
                        void                 *desc_addr,
                        GUI_3D_DRAW_TYPE      type,
                        int16_t               x,
                        int16_t               y,
                        int16_t               w,
                        int16_t               h);

/**
 * @brief Set the image for a specific face of the 3D widget.
 *
 * @param this Pointer to the 3D widget.
 * @param face_index Index of the face to set the image.
 * @param image_addr Pointer to the image data to be set for the face.
 */
void gui_3d_set_face_image(gui_3d_t *this, uint8_t face_index, void *image_addr);

/**
 * @brief Set global transform callback.
 *
 * @param this The 3D widget pointer.
 * @param cb Set callback functions for the world coordinate system, camera coordinate system,
 *           and light source for all faces.
 */
void gui_3d_set_global_transform_cb(gui_3d_t *this, gui_3d_global_transform_cb cb);

/**
 * @brief Set face transform callback.
 *
 * @param this The 3D widget pointer.
 * @param cb Set callback functions for the world coordinate system, camera coordinate system,
 *           and light source for the specified face.
 */
void gui_3d_set_face_transform_cb(gui_3d_t *this, gui_3d_face_transform_cb cb);

/**
 * @brief Set a callback function for when the 3D widget is clicked.
 *
 * @param this Pointer to the 3D widget.
 * @param callback Callback function to execute on click.
 * @param parameter Additional parameter for the callback.
 */
void gui_3d_on_click(gui_3d_t *this, void *callback, void *parameter);


#ifdef __cplusplus
}
#endif

#endif
