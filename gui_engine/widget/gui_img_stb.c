/**
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
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <guidef.h>
#include <gui_img.h>
#include <string.h>
#include <gui_obj.h>
#include <draw_img.h>
#include <tp_algo.h>
#include "acc_engine.h"
#include <gui_img_stb.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "acc_init.h"

/** @defgroup WIDGET WIDGET
  * @{
  */
/*============================================================================*
 *                           Types
 *============================================================================*/
/** @defgroup WIDGET_Exported_Types WIDGET Exported Types
  * @{
  */


/** End of WIDGET_Exported_Types
  * @}
  */

/*============================================================================*
 *                           Constants
 *============================================================================*/
/** @defgroup WIDGET_Exported_Constants WIDGET Exported Constants
  * @{
  */


/** End of WIDGET_Exported_Constants
  * @}
  */

/*============================================================================*
 *                            Macros
 *============================================================================*/
/** @defgroup WIDGET_Exported_Macros WIDGET Exported Macros
  * @{
  */



/** End of WIDGET_Exported_Macros
  * @}
  */
/*============================================================================*
 *                            Variables
 *============================================================================*/
/** @defgroup WIDGET_Exported_Variables WIDGET Exported Variables
  * @{
  */


/** End of WIDGET_Exported_Variables
  * @}
  */

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
/** @defgroup WIDGET_Exported_Functions WIDGET Exported Functions
  * @{
  */
static uint8_t *decode_image(gui_stb_img_t *img)
{
    uint8_t *output;
    int source_w = 0;
    int source_h = 0;
    int num_components = 0;
    output = stbi_load_from_memory(img->data_buffer, img->data_length, &source_w, &source_h,
                                   &num_components, 0);
    gui_rgb_data_head_t head;
    memset(&head, 0x0, sizeof(head));
    head.w = source_w;
    head.h = source_h;
    head.type = RGB888;
    memcpy(output, &head, sizeof(head));
    return output;
}
static void modify_img(gui_obj_t *obj)
{
    gui_stb_img_t *img = (gui_stb_img_t *)obj;
    if (img->data_buffer == NULL ||
        img->data_length == 0 ||
        img->src_changed == false)
    {
        return;
    }
    if (img->image_format != JPEG &&
        img->image_format != BMP &&
        img->image_format != PNG)
    {
        return;
    }
    if (img->img)
    {
        if (img->img->data != 0)
        {
            gui_free(img->img->data);
            img->img->data = NULL;
        }
        img->img->data = decode_image(img);
    }
    else
    {
        img->img = gui_img_create_from_mem(obj, "stb_img", decode_image(img), 0, 0, 0, 0);
    }
    img->src_changed = false;
}
static void stb_image_destory(gui_obj_t *obj)
{
    gui_stb_img_t *img = (gui_stb_img_t *)obj;
    if (img->img != NULL)
    {
        if (img->img->data != NULL)
        {
            gui_free(img->img->data);
        }
    }
}
static void gui_stbimg_from_mem_ctor(gui_stb_img_t *this,
                                     gui_obj_t *parent,
                                     const char *name,
                                     void *addr,
                                     uint32_t size,
                                     GUI_FormatType type,
                                     int16_t x,
                                     int16_t y)
{
    //for root class
    gui_obj_t *root = (gui_obj_t *)this;
    gui_obj_ctor(root, parent, name, x, y, 0, 0);
    root->type = IMAGE_FROM_MEM;
    root->obj_destory = stb_image_destory;

    //for self
    this->src_changed = true;
    this->data_buffer = addr;
    this->data_length = size;
    this->image_format = type;
}
/*============================================================================*
 *                           Public Functions
 *============================================================================*/

void gui_stbimg_set_attribute(gui_stb_img_t *this,
                              void *addr,
                              uint32_t size,
                              GUI_FormatType type,
                              int16_t x,
                              int16_t y)
{
    this->src_changed = true;
    this->data_buffer = addr;
    this->data_length = size;
    this->image_format = type;
    this->base.x = x;
    this->base.y = y;
    modify_img((gui_obj_t *)this);
}
gui_stb_img_t *gui_stbimg_create_from_mem(void *parent,
                                          const char *name,
                                          void *addr,
                                          uint32_t size,
                                          GUI_FormatType type,
                                          int16_t x,
                                          int16_t y)
{
    GUI_ASSERT(parent != NULL);
    if (name == NULL)
    {
        name = "DEFAULT_STB_IMAGE";
    }
    gui_stb_img_t *img = gui_malloc(sizeof(gui_stb_img_t));
    GUI_ASSERT(img != NULL);
    memset(img, 0x00, sizeof(gui_stb_img_t));
    gui_stbimg_from_mem_ctor(img, (gui_obj_t *)parent, name, addr, size, type, x, y);
    gui_list_init(&(GET_BASE(img)->child_list));
    if ((GET_BASE(img)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(img)->parent)->child_list), &(GET_BASE(img)->brother_list));
    }
    GET_BASE(img)->create_done = true;
    modify_img((gui_obj_t *)img);
    return img;
}
/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */
