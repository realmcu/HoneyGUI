/**
*****************************************************************************************
*     Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_wave.c
  * @brief wave widget
  * @details wave widget
  * @author luke_sun@realsil.com.cn
  * @date 2024/07/03
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2024 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "gui_u8g2.h"

/*============================================================================*
 *                           Types
 *============================================================================*/


/*============================================================================*
 *                           Constants
 *============================================================================*/


/*============================================================================*
 *                            Macros
 *============================================================================*/


/*============================================================================*
 *                            Variables
 *============================================================================*/


/*============================================================================*
 *                           Private Functions
 *============================================================================*/

static void gui_convert_b1_to_rgb(uint8_t *ptr, uint32_t height, uint32_t width)
{
    gui_dispdev_t *dc = gui_get_dc();
    uint8_t dc_bytes_per_pixel = dc->bit_depth >> 3;
    if (dc_bytes_per_pixel == 2)
    {
        uint16_t *writebuf = (uint16_t *)dc->frame_buf;
        uint8_t ppb = 8;
        for (uint32_t i = 0; i < height; i++)
        {
            int dots_off = i * (width / ppb);
            for (uint32_t j = 0; j < width; j++)
            {
                if ((ptr[dots_off + j / ppb] >> (7 - j % ppb)) & 0x01)
                {
                    writebuf[i * dc->fb_width + j] = UINT16_MAX;
                }
            }
        }
    }
    else if (dc_bytes_per_pixel == 4)
    {
        uint32_t *writebuf = (uint32_t *)dc->frame_buf;
        uint8_t ppb = 8;//pixel_per_byte = 8 / rendor_mode
        for (uint32_t i = 0; i < height; i++)
        {
            int dots_off = i * (width / ppb);
            for (uint32_t j = 0; j < width; j++)
            {
                if ((ptr[dots_off + j / ppb] >> (7 - j % ppb)) & 0x01)
                {
                    writebuf[i * dc->fb_width + j] = UINT32_MAX;
                }
            }
        }
    }
}

static void gui_u8g2_prepare(gui_u8g2_t *this)
{
    GUI_UNUSED(this);
    gui_dispdev_t *dc = gui_get_dc();
    gui_obj_t *root = (gui_obj_t *)this;

    GUI_UNUSED(root);
}

static void gui_u8g2_draw_cb(gui_obj_t *obj)
{
    gui_u8g2_t *this = (gui_u8g2_t *)obj;
    gui_dispdev_t *dc = gui_get_dc();
    u8g2_t u8g2;
    this->u8g2 = &u8g2;
    uint8_t *buf = gui_malloc(this->base.w * this->base.h);
    GUI_ASSERT(buf != NULL);
    this->buf = buf;

    u8x8_t *u8x8 = u8g2_GetU8x8(&u8g2);
    u8x8_SetupDefaults(u8x8);
    u8x8_display_info_t u8x8_display_info;
    u8x8_display_info.tile_height = this->base.h / 8;
    u8x8_display_info.tile_width = this->base.w / 8;
    u8x8_display_info.pixel_height = this->base.h;
    u8x8_display_info.pixel_width = this->base.w;
    u8x8_d_helper_display_setup_memory(u8x8, &u8x8_display_info);
    u8g2_SetupBuffer(&u8g2, this->buf, this->base.h / 8, u8g2_ll_hvline_horizontal_right_lsb,
                     &u8g2_cb_r0);

    this->u8g2_draw_cb(&u8g2);
    gui_convert_b1_to_rgb(u8g2.tile_buf_ptr, this->base.h, this->base.w);
    if (this->buf)
    {
        gui_free(this->buf);
        this->buf = NULL;
    }
    this->u8g2 = NULL;
}

static void gui_u8g2_end(gui_obj_t *obj)
{
    gui_u8g2_t *this = (gui_u8g2_t *)obj;
    GUI_UNUSED(this);
}

static void gui_u8g2_destory(gui_obj_t *obj)
{

}

static void gui_u8g2_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            gui_u8g2_prepare((gui_u8g2_t *)obj);
            break;

        case OBJ_DRAW:
            gui_u8g2_draw_cb(obj);
            break;

        case OBJ_END:
            gui_u8g2_end(obj);
            break;

        case OBJ_DESTORY:
            gui_u8g2_destory(obj);
            break;

        default:
            break;
        }
    }
}

static void gui_u8g2_widget_ctor(gui_u8g2_t *this,
                                 gui_obj_t    *parent,
                                 const char   *name,
                                 int16_t       x,
                                 int16_t       y,
                                 int16_t       w,
                                 int16_t       h)
{
    gui_obj_t *base = (gui_obj_t *)this;
    gui_obj_t *root = (gui_obj_t *)this;

    //for base class
    gui_obj_ctor(base, parent, name, x, y, w, h);

    //for root class
    root->type = U8G2;
    root->obj_cb = gui_u8g2_cb;
    root->has_prepare_cb = true;
    root->has_draw_cb = true;
    root->has_end_cb = true;
    root->has_destory_cb = true;
}


/*============================================================================*
 *                           Public Functions
 *============================================================================*/

void gui_u8g2_set_draw_cb(gui_u8g2_t *this, void (*cb)(u8g2_t *u8g2))
{
    this->u8g2_draw_cb = cb;
}

gui_u8g2_t *gui_u8g2_create(void       *parent,
                            const char *name,
                            int16_t     x,
                            int16_t     y,
                            int16_t     w,
                            int16_t     h)
{
    gui_dispdev_t *dc = gui_get_dc();
    GUI_ASSERT(dc->type == DC_SINGLE);
    GUI_ASSERT(parent != NULL);
    if (name == NULL)
    {
        name = "WIDGET_u8g2";
    }

    gui_u8g2_t *this = gui_malloc(sizeof(gui_u8g2_t));
    GUI_ASSERT(this != NULL);
    memset(this, 0x00, sizeof(gui_u8g2_t));

    gui_u8g2_widget_ctor(this, (gui_obj_t *)parent, name, x, y, w, h);

    gui_list_init(&(GET_BASE(this)->child_list));
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list),
                               &(GET_BASE(this)->brother_list));
    }

    GET_BASE(this)->create_done = true;
    return this;
}
