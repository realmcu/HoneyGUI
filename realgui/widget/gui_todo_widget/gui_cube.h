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


/*============================================================================*
 *                         Types
 *============================================================================*/

typedef enum
{
    CUBE_SIDE_FRONT = 0,
    CUBE_SIDE_BACK,
    CUBE_SIDE_UP,
    CUBE_SIDE_DOWN,
    CUBE_SIDE_LEFT,
    CUBE_SIDE_RIGHT,
    CUBE_SIDE_ALL
} T_CUBE_SIDE_TYPE;
typedef enum
{
    CUBE_NZ_0321 = 0,
    CUBE_NZ_4567 = 1,
    CUBE_NZ_5126 = 2,
    CUBE_NZ_0473 = 3,
    CUBE_NZ_7623 = 4,
    CUBE_NZ_0154 = 5,
} T_CUBE_NZ_TYPE;
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
    IMG_SOURCE_MODE_TYPE src_mode[6];                    //!< flag: indicate file source
    union
    {
        cube_img_path_t img_path;
        cube_data_addr_t data_addr;
    };
} gui_cube_imgfile_t;

typedef struct
{
    float size;
    gui_event_t event;
} gui_cube_img_event_t;

typedef struct
{
    gui_event_cb_t  click_cb_front;
    gui_event_cb_t  click_cb_back;
    gui_event_cb_t  click_cb_up;
    gui_event_cb_t  click_cb_down;
    gui_event_cb_t  click_cb_left;
    gui_event_cb_t  click_cb_right;
} gui_cube_cb_t;

/** @brief  CUBE widget structure */
#ifdef  __CC_ARM
#pragma anon_unions
#endif
typedef struct gui_cube
{
    gui_obj_t base;                 //!< base structure
    draw_img_t draw_img[6];         // please refer to enum T_CUBE_SIDE_TYPE.

    float c_x;
    float c_y;
    float cbsize;
    float nz[6];                    // please refer to enum T_CUBE_NZ_TYPE.

    //float scale_x;
    //float scale_y;
    //float t_x;
    //float t_y;
    int16_t release_x;
    int16_t release_y;
    bool auto_flag;
    uint32_t interval_ms_y;
    float    step_degree_y;

    float xrot;
    float yrot;
    float zrot;

    union
    {
        void *filename[6];                         // keep the filepath for partial draw
        void *ftl[6];
    };
    void *data[6];
    uint8_t src_mode[6];
} gui_cube_t;


/*============================================================================*
 *                         Constants
 *============================================================================*/


/*============================================================================*
 *                         Macros
 *============================================================================*/

#define RAD(d)        (d*3.1415926f/180.0f) //!< angle to rad


/*============================================================================*
 *                         Variables
 *============================================================================*/


/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * @brief get the cube image's blend mode
 *
 * @param cube the cube widget pointer
 * @param cube_side the cube image's side, CUBE_SIDE_ALL is not supported
 * @return the cube image's blend mode
 *
 */
BLEND_MODE_TYPE gui_cube_get_mode(gui_cube_t *cube, T_CUBE_SIDE_TYPE side);

/**
 * @brief set the cube image's blend mode
 *
 * @param cube the cube widget pointer
 * @param cube_side the cube image's side
 * @param mode the enumeration value of the mode is BLEND_MODE_TYPE
 *
 */
void gui_cube_set_mode(gui_cube_t *cube, T_CUBE_SIDE_TYPE cube_side, BLEND_MODE_TYPE mode);

/**
 * @brief get the cube image's opacity
 *
 * @param cube the cube widget pointer
 * @param cube_side the cube image's side, CUBE_SIDE_ALL is not supported
 * @return the cube image's opacity
 *
 */
uint8_t gui_cube_get_opacity(gui_cube_t *cube, T_CUBE_SIDE_TYPE side);

/**
 * @brief set the cube image's opacity
 *
 * @param cube the cube widget pointer
 * @param cube_side the cube image's side
 * @param opacity the cube image's opacity
 *
 */
void gui_cube_set_opacity(gui_cube_t *cube, T_CUBE_SIDE_TYPE side, uint8_t opacity);

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
gui_cube_t *gui_cube_create(void               *parent,
                            const char         *name,
                            gui_cube_imgfile_t *img_file,
                            int16_t             x,
                            int16_t             y);

/**
 * @brief set center
 *
 * @param _this widget pointer
 * @param c_x center x
 * @param c_y center y
 */
void gui_cube_set_center(gui_cube_t *_this, float c_x, float c_y);

/**
 * @brief set size
 *
 * @param _this widget pointer
 * @param size scale size
 */
void gui_cube_set_size(gui_cube_t *_this, float size);

/**
 * @brief set click cb
 *
 * @param _this widget pointer
 * @param cb_list click cb list
 */
void gui_cube_add_click_cb(gui_cube_t *_this, gui_cube_cb_t cb_list);

/**
 * @brief
 *
 * @param _this widget pointer
 * @param internal_ms rotation interval
 * @param degree angle per rotation
 */
void gui_cube_auto_rotation_by_x(gui_cube_t *_this, uint32_t internal_ms, float degree);

/**
 * @brief
 *
 * @param _this widget pointer
 * @param internal_ms rotation interval
 * @param degree angle per rotation
 */
void gui_cube_auto_rotation_by_y(gui_cube_t *_this, uint32_t internal_ms, float degree);

/**
 * @brief
 *
 * @param _this widget pointer
 * @param internal_ms rotation interval
 * @param degree angle per rotation
 */
void gui_cube_auto_rotation_by_z(gui_cube_t *_this, uint32_t internal_ms, float degree);


#ifdef __cplusplus
}
#endif

#endif
