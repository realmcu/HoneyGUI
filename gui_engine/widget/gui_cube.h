/**
 \internal
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_cube.h
  * @brief cube widget
  * @details cube widget
  * @author howie_wang@realsil.com.cn
  * @date 2023/10/19
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
#ifndef __GUI_CUBE_H__
#define __GUI_CUBE_H__
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
    CUBE_SIDE_FRONT = 0,
    CUBE_SIDE_BACK,
    CUBE_SIDE_UP,
    CUBE_SIDE_DOWN,
    CUBE_SIDE_LEFT,
    CUBE_SIDE_RIGHT
} CUBE_SIDE_TYPE;

typedef struct
{
    char *img_path_front;              //!< images file path
    char *img_path_back;               //!< images file path
    char *img_path_up;                 //!< images file path
    char *img_path_down;               //!< images file path
    char *img_path_left;               //!< images file path
    char *img_path_right;              //!< images file path
} cube_img_path_t;

typedef struct
{
    void *data_addr_front;             //!< images memory address
    void *data_addr_back;              //!< images memory address
    void *data_addr_up;                //!< images memory address
    void *data_addr_down;              //!< images memory address
    void *data_addr_left;              //!< images memory address
    void *data_addr_right;             //!< images memory address
} cube_data_addr_t;

typedef struct
{
    bool flg_fs;                    //!< flag: using filesystem
    union
    {
        cube_img_path_t img_path;
        cube_data_addr_t data_addr;
    };
} gui_cube_imgfile_t;

/** @brief  CUBE widget structure */
typedef struct gui_cube
{
    gui_obj_t base;                 //!< base structure
    cube_img_path_t img_path;
    draw_img_t draw_img_front;
    draw_img_t draw_img_back;
    draw_img_t draw_img_up;
    draw_img_t draw_img_down;
    draw_img_t draw_img_left;
    draw_img_t draw_img_right;
    float c_x;
    float c_y;
    float cbsize;
    float nz0321;
    float nz4567;
    float nz5126;
    float nz0473;
    float nz7623;
    float nz0154;
    //float scale_x;
    //float scale_y;
    //float t_x;
    //float t_y;
    int16_t release_x;
    int16_t release_y;
    bool flg_fs;
    bool auto_flag;
    uint32_t interval_ms_y;
    float    step_degree_y;

    float xrot;
    float yrot;
    float zrot;
} gui_cube_t;

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

#define RAD(d)        (d*3.1415926f/180.0f) //!< angle to rad

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
 * @brief set the cube image's blend mode
 *
 * @param cube the cube widget pointer
 * @param cube_side the cube image's side
 * @param mode the enumeration value of the mode is BLEND_MODE_TYPE
 *
 */
void gui_cube_set_mode(gui_cube_t *cube, CUBE_SIDE_TYPE cube_side, BLEND_MODE_TYPE mode);

/**
 * @brief set cube image
 *
 * @param cube the cube widget pointer
 * @param img_file the image file data, set flg_fs true when using filesystem
 */
void gui_cube_set_img(gui_cube_t *cube, gui_cube_imgfile_t *img_file);

/**
 * @brief cube create, images can be loaded from filesystem or memory address
 *
 * @param parent parent widget
 * @param name  widget name
 * @param img_file the image file data, set flg_fs true when using filesystem
 * @param x  left
 * @param y top
 * @return gui_cube_t* widget pointer
 * <b>Example usage</b>
 * \code{.c}
 *{
 *    gui_cube_imgfile_t imgfile =
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
 *    gui_cube_t *cube4 = gui_cube_create(parent, "cube", &imgfile, 0, 0);
 *    gui_cube_set_size(cube4, 100);
 *    gui_cube_set_center(cube4, 200, 200);
 *}
 * \endcode
 */
gui_cube_t *gui_cube_create(void *parent,  const char *name, gui_cube_imgfile_t *img_file,
                            int16_t x,
                            int16_t y);
/**
 * @brief set center
 *
 * @param this widget pointer
 * @param c_x center x
 * @param c_y center y
 */
void gui_cube_set_center(gui_cube_t *this, float c_x, float c_y);
/**
 * @brief set size
 *
 * @param this widget pointer
 * @param size scale size
 */
void gui_cube_set_size(gui_cube_t *this, float size);

/**
 * @brief
 *
 * @param this
 * @param interval
 */
void gui_cube_auto_rotation_by_x(gui_cube_t *this, uint32_t internal_ms, float degree);

/**
 * @brief
 *
 * @param this
 * @param interval
 */
void gui_cube_auto_rotation_by_y(gui_cube_t *this, uint32_t internal_ms, float degree);

/**
 * @brief
 *
 * @param this
 * @param interval
 */
void gui_cube_auto_rotation_by_z(gui_cube_t *this, uint32_t internal_ms, float degree);

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

