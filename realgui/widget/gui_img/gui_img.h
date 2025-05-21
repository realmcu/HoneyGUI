/**
\internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_img.h
  * @brief image widget header file
  * @details image widget is used to show image on the screen
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
#ifndef __GUI_IMG_H__
#define __GUI_IMG_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "guidef.h"
#include "gui_api.h"
#include "draw_img.h"
#include "gui_matrix.h"
#include "gui_obj.h"

/*============================================================================*
 *                         Types
 *============================================================================*/

/** @brief  image widget structure */
#ifdef  __CC_ARM
#pragma anon_unions
#endif
typedef struct gui_img_transform
{

    float degrees;                      //!< float gui_img_get_transform_degrees(gui_img_t *img);
    float c_x;                          //!< center of image x; float gui_img_get_transform_c_x(gui_img_t *img);
    float c_y;                          //!< center of image y;  float gui_img_get_transform_c_y(gui_img_t *img);
    float scale_x;                      //!< float gui_img_get_transform_scale_x(gui_img_t *img);
    float scale_y;                      //!< float gui_img_get_transform_scale_y(gui_img_t *img);
    float t_x;                          //!< translate of screen x;  float gui_img_get_transform_t_x(gui_img_t *img);
    float t_y;                          //!< translate of screen y;  float gui_img_get_transform_t_y(gui_img_t *img);
    float t_x_old;
    float t_y_old;
} gui_img_transform_t;

typedef struct gui_img
{
    gui_obj_t base;
    draw_img_t *draw_img;
    gui_img_transform_t *transform;
    void *data;                         // this means address or filesystem path
    union
    {
        void *filename;                         // keep the filepath for partial draw
        void *ftl;
    };
    gui_animate_t *animate;
    uint16_t scope_x1;
    uint16_t scope_x2;
    uint16_t scope_y1;
    uint16_t scope_y2;
    int16_t ax, ay;
    uint32_t opacity_value : 8;
    uint32_t blend_mode    : 3;
    uint32_t src_mode      : 3;
    uint32_t high_quality  : 1;
    uint32_t press_flag    : 1;         //!< press to change picture to the highlighted
    uint32_t release_flag  : 1;
    uint32_t scope         : 1;
    uint32_t scope_flag    : 1;
    uint32_t tp_block      : 1;
    uint32_t load          : 1;
    uint8_t checksum;
    uint8_t animate_array_length;
} gui_img_t;


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
 * @brief load the image to read it's width.
 *
 * @param _this the image widget pointer.
 * @return uint16_t image's width.
 */
uint16_t gui_img_get_width(gui_img_t *_this);

/**
 * @brief load the image to read it's hight.
 *
 * @param _this the image widget pointer.
 * @return uint16_t image's height.
 */
uint16_t gui_img_get_height(gui_img_t *_this);

/**
 * @brief refresh the image size from src.
 *
 * @param _this the image widget pointer.
 */
void gui_img_refresh_size(gui_img_t *_this);

/**
 * @brief set the image's location.
 *
 * @param _this the image widget pointer.
 * @param x the x coordinate.
 * @param y the y coordinate.
 */
void gui_img_set_location(gui_img_t *_this, uint16_t x, uint16_t y);

/**
 * @brief set the image's blend mode.
 *
 * @param _this the image widget pointer.
 * @param mode the enumeration value of the mode is BLEND_MODE_TYPE.
 *
 */
void gui_img_set_mode(gui_img_t *_this, BLEND_MODE_TYPE mode);

/**
 * @brief set x,y and file path.
 *
 * @param _this image widget.
 * @param name change widget name.
 * @param addr change picture address.
 * @param x X-axis coordinate.
 * @param y Y-axis coordinate.
 */
void gui_img_set_attribute(gui_img_t  *_this,
                           const char *name,
                           void       *addr,
                           int16_t     x,
                           int16_t     y);

/**
 * @brief rotate the image around the center of the circle.
 *
 * @param _this the image widget pointer.
 * @param degrees clockwise rotation absolute angle.
 * @param c_x the X-axis coordinates of the center of the circle.
 * @param c_y the Y-axis coordinates of the center of the circle.
 */
void gui_img_rotation(gui_img_t *_this,
                      float      degrees,
                      float      c_x,
                      float      c_y);

/**
 * @brief change the size of the image, take (0, 0) as the zoom center.
 *
 * @param _this the image widget pointer.
 * @param scale_x scale in the x direction.
 * @param scale_y scale in the y direction.
 */
void gui_img_scale(gui_img_t *_this, float scale_x, float scale_y);

/**
 * @brief move image.
 *
 * @param _this the image widget pointer.
 * @param t_x new X-axis coordinate.
 * @param t_y new Y-axis coordinate.
 */
void gui_img_translate(gui_img_t *_this, float t_x, float t_y);

/**
 * @brief skew image on X-axis.
 *
 * @param _this the image widget pointer.
 * @param degrees skew angle.
 */
void gui_img_skew_x(gui_img_t *_this, float degrees);

/**
 * @brief skew image on Y-axis.
 *
 * @param _this the image widget pointer.
 * @param degrees skew angle.
 */
void gui_img_skew_y(gui_img_t *_this, float degrees);

/**
 * @brief add opacity value to the image.
 *
 * @param _this the image widget pointer.
 * @param opacity_value The opacity value ranges from 0 to 255, default 255.
 */
void gui_img_set_opacity(gui_img_t *_this, unsigned char opacity_value);

/**
 * @brief set image as tp blocker, tp will not be recived by widget under image.
 *
 * @param _this the image widget pointer.
 * @param block block or not.
 */
void gui_img_set_tp_block(gui_img_t *_this, bool block);

/**
 * @brief creat an image widget from memory address.
 * @note creat an image widget and set attribute.
 * @param parent the father widget it nested in.
 * @param name widget name.
 * @param addr bin file address.
 * @param x the X-axis coordinate of the widget.
 * @param y the Y-axis coordinate of the widget.
 * @param w the width of the widget.
 * @param h the hight of the widget.
 * @return return the widget object pointer.
 */
gui_img_t *gui_img_create_from_mem(void       *parent,
                                   const char *name,
                                   void       *addr,
                                   int16_t     x,
                                   int16_t     y,
                                   int16_t     w,
                                   int16_t     h);

/**
 * @brief creat an image widget from memory address.
 * @note creat an image widget and set attribute.
 * @param parent the father widget it nested in.
 * @param name widget name.
 * @param ftl not xip address, use ftl address.
 * @param x the X-axis coordinate of the widget.
 * @param y the Y-axis coordinate of the widget.
 * @param w the width of the widget.
 * @param h the hight of the widget.
 * @return return the widget object pointer.
 */
gui_img_t *gui_img_create_from_ftl(void       *parent,
                                   const char *name,
                                   void       *ftl,
                                   int16_t     x,
                                   int16_t     y,
                                   int16_t     w,
                                   int16_t     h);

/**
 * @brief creat an image widget from filesystem.
 *
 * @param parent the father widget it nested in.
 * @param name image widget name.
 * @param file image file path.
 * @param x the X-axis coordinate of the widget.
 * @param y the Y-axis coordinate of the widget.
 * @param w the width of the widget.
 * @param h the hight of the widget.
 * @return gui_img_t*.
 */
gui_img_t *gui_img_create_from_fs(void       *parent,
                                  const char *name,
                                  void       *file,
                                  int16_t     x,
                                  int16_t     y,
                                  int16_t     w,
                                  int16_t     h);

/**
 * @brief set animate.
 *
 * @param _this pointer.
 * @param dur animation time cost in ms.
 * @param repeat_count rounds to repeat.
 * @param callback every frame callback.
 * @param p callback's parameter.
 */
void gui_img_set_animate(gui_img_t *_this,
                         uint32_t   dur,
                         int        repeat_count,
                         void      *callback,
                         void      *p);

/**
 * @brief set the image's quality.
 *
 * @param _this the image widget pointer.
 * @param high_quality image drawn in high quality or not.
 *
 */
void gui_img_set_quality(gui_img_t *_this, bool high_quality);

/**
 * @brief convert a tree to a image data.
 *
 * @param obj tree root.
 * @param matrix null if no need to transform.
 */
void gui_img_tree_convert_to_img(gui_obj_t *obj, gui_matrix_t *matrix, uint8_t *shot_buf);

/**
 * @brief convert a tree to a image data in root widget's size (RGB format).
 *
 * @param obj tree root.
 * @param matrix null if no need to transform.
 * @param shot_buf image data buffer.
 */
void gui_img_tree_convert_to_img_root_size(gui_obj_t *obj, gui_matrix_t *matrix, uint8_t *shot_buf);

/**
 * @brief get the transform scale in the X direction for a GUI image.
 *
 * @param img pointer to the GUI image object.
 *
 * @return the scale in the X direction.
 */
float gui_img_get_transform_scale_x(gui_img_t *img);

/**
 * @brief get the transform scale in the Y direction for a GUI image.
 *
 * @param img pointer to the GUI image object.
 *
 * @return the scale in the Y direction.
 */
float gui_img_get_transform_scale_y(gui_img_t *img);

/**
 * @brief get the rotation angle in degrees for a GUI image.
 *
 * @param img pointer to the GUI image object.
 *
 * @return the rotation angle in degrees.
 */
float gui_img_get_transform_degrees(gui_img_t *img);

/**
 * @brief get the center X coordinate for rotate of a GUI image.
 *
 * @param img pointer to the GUI image object.
 *
 * @return the center X coordinate for transformations.
 */
float gui_img_get_transform_c_x(gui_img_t *img);

/**
 * @brief get the center Y coordinate for rotate of a GUI image.
 *
 * @param img pointer to the GUI image object.
 *
 * @return the center Y coordinate for transformations.
 */
float gui_img_get_transform_c_y(gui_img_t *img);

/**
 * @brief get the translation in the X direction for a GUI image.
 *
 * @param img pointer to the GUI image object.
 *
 * @return the translation in the X direction.
 */
float gui_img_get_transform_t_x(gui_img_t *img);

/**
 * @brief get the translation in the Y direction for a GUI image.
 *
 * @param img pointer to the GUI image object.
 *
 * @return the translation in the Y direction.
 */
float gui_img_get_transform_t_y(gui_img_t *img);

/**
 * @brief append an animation to a GUI image.
 *
 * @param _this pointer to the GUI image object.
 * @param dur duration of the animation in milliseconds.
 * @param repeat_count number of times the animation should repeat.
 * @param callback (void *p, void *this_widget, gui_animate_t *animate)Function to be called at each the animation frame.
 * @param p user data to be passed to the callback function.
 * @param name aniamte name.
 */
void gui_img_append_animate(gui_img_t              *_this,
                            uint32_t                dur,
                            int                     repeat_count,
                            gui_animate_callback_t  callback,
                            void                   *p,
                            const char              *name);

/**
 * @brief Sets the image data for a specified image widget.
 *
 * This function assigns the given image data to the specified image widget.
 * The image data might correspond to various formats, and the format
 * should be compatible with the handling of `gui_img_t`.
 *
 * @param widget          The pointer to the image widget (`gui_img_t`) for which the image data is to be set.
 * @param image_data_pointer  The pointer to the image data to be set to the widget.
 *                            The data should persist as long as the widget needs it or until it is explicitly updated.
 */
void gui_img_set_image_data(gui_img_t *widget, const uint8_t *image_data_pointer);

/**
 * @brief Gets the image data from a specified image widget.
 *
 * This function returns the current image data that is set in the specified image widget.
 *
 * @param widget The pointer to the image widget (`gui_img_t`) from which the image data should be retrieved.
 *
 * @return A pointer to the image data currently set in the widget. If no image data is set, the result may be `NULL`.
 */
const uint8_t *gui_img_get_image_data(gui_img_t *widget);


#ifdef __cplusplus
}
#endif

#endif
