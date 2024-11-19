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
#include <string.h>
#include "gui_img.h"
#include "gui_obj.h"
#include "draw_img.h"
#include "tp_algo.h"
#include "gui_img_stb.h"
#define STB_IMAGE_IMPLEMENTATION

#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#define STBI_NO_HDR
#define STBI_NO_LINEAR
#define STBI_NO_GIF
#define STBI_NO_PIC
#define STBI_NO_THREAD_LOCALS

#include "gui_api.h"
#define STBI_MALLOC(sz)           gui_malloc(sz)
#define STBI_REALLOC(p,newsz)     gui_realloc(p, newsz)
#define STBI_FREE(p)              gui_free(p)
#define STB_IMAGE_STATIC
#include "stb_image.h"
#ifdef TJPG
#define TJPGD_IMPLEMENTATION
#include "../3rd/tjpgd/tjpgd.h"
#endif

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

static void rgb888_to_rgb565(const uint8_t *src, uint8_t *dst, size_t source_w, size_t source_h)
{
    const uint8_t *rgb888_data = src;
    uint16_t *rgb565_data = (uint16_t *)(dst);

    for (size_t i = 0; i < source_w * source_h; i++)
    {
        uint8_t r = rgb888_data[3 * i + 2];
        uint8_t g = rgb888_data[3 * i + 1];
        uint8_t b = rgb888_data[3 * i + 0];

        uint16_t r5 = (r >> 3) & 0x1F;
        uint16_t g6 = (g >> 2) & 0x3F;
        uint16_t b5 = (b >> 3) & 0x1F;

        rgb565_data[i] = (r5 << 11) | (g6 << 5) | b5;
    }
}

static uint8_t *gui_img_stb_decode_image(gui_stb_img_t *img)
{
    if (img == NULL || img->data_buffer == NULL || img->data_length <= 0)
    {
        return NULL; // Argument check
    }
    uint8_t *output;
    int source_w = 0, source_h = 0, num_components = 0;
    gui_rgb_data_head_t head;
    int head_len = sizeof(gui_rgb_data_head_t);
    if (img->output_format == RGB888)
    {
        output = stbi_load_from_memory(img->data_buffer, img->data_length, &source_w, &source_h,
                                       &num_components, 0);
        GUI_ASSERT(output != NULL);
        memset(&head, 0x0, sizeof(head));
        head.w = source_w;
        head.h = source_h;
        head.type = RGB888;
        memcpy(output, &head, sizeof(head));

        return output;
    }
    else if (img->output_format == RGB565)
    {
        output = stbi_load_from_memory(img->data_buffer, img->data_length, &source_w, &source_h,
                                       &num_components, 0);
        GUI_ASSERT(output != NULL);
        uint8_t *rgb565_buf = gui_malloc(source_w * source_h * 2 + head_len);
        GUI_ASSERT(rgb565_buf != NULL);
        rgb888_to_rgb565(output + head_len, rgb565_buf + head_len, source_w, source_h);

        memset(&head, 0x0, sizeof(head));
        head.w = source_w;
        head.h = source_h;
        head.type = RGB565;
        memcpy(rgb565_buf, &head, sizeof(head));

        gui_free(output);
        return rgb565_buf;
    }
    return NULL;
}

static void gui_img_stb_modify_img(gui_obj_t *obj)
{
    gui_stb_img_t *img = (gui_stb_img_t *)obj;

    if ((img->data_buffer == NULL)
        || (img->data_length == 0)
        || (img->src_changed == false))
    {
        return;
    }
#ifdef TJPG
    if (img->input_format == JPEG)
    {
        return;
    }
#endif
    if ((img->input_format != JPEG)
        && (img->input_format != BMP)
        && (img->input_format != PNG))
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
        img->img->data = gui_img_stb_decode_image(img);
    }
    else
    {
        img->img = gui_img_create_from_mem(obj, "stb_img", gui_img_stb_decode_image(img), 0, 0, 0, 0);
    }

    img->src_changed = false;
}
static void gui_img_stb_modify_img_static(gui_obj_t *obj)
{
    gui_stb_img_t *img = (gui_stb_img_t *)obj;

    if ((img->data_buffer == NULL)
        || (img->data_length == 0)
        || (img->src_changed == false))
    {
        return;
    }
#ifdef TJPG
    if (img->input_format == JPEG)
    {
        return;
    }
#endif
    if ((img->input_format != JPEG)
        && (img->input_format != BMP)
        && (img->input_format != PNG))
    {
        return;
    }

    if (img->img)
    {
        img->img->data = gui_img_stb_decode_image(img);
    }
    else
    {
        img->img = gui_img_create_from_mem(obj, "stb_img", gui_img_stb_decode_image(img), 0, 0, 0, 0);
    }

    img->src_changed = false;
}
static void gui_img_stb_image_destory(gui_obj_t *obj)
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

#ifdef TJPG
uint32_t input_func(JDEC *jd, uint8_t *buff, uint32_t nuint8_t)
{
    IODEV *dev = (IODEV *)jd->device;
    /* Calculate available size left in the jpg data */
    uint32_t uint8_tsLeft = dev->size - dev->index;
    if (buff)
    {
        /* Copy uint8_ts from jpeg data */
        if (nuint8_t >= uint8_tsLeft)
        {
            nuint8_t = uint8_tsLeft;
        }
        memcpy(buff, dev->data + dev->index, nuint8_t);
        dev->index += nuint8_t;
        return nuint8_t;
    }
    else
    {
        /* Skip uint8_ts in jpeg data */
        if (nuint8_t >= uint8_tsLeft)
        {
            dev->index = dev->size;
            return uint8_tsLeft;
        }
        dev->index += nuint8_t;
        return nuint8_t;
    }
}
int output_func(JDEC *jd, void *bitmap, JRECT *rect, gui_obj_t *obj)
{
    gui_dispdev_t *dc = gui_get_dc();
    gui_stb_img_t *img = (gui_stb_img_t *)obj;
    uint8_t dc_uint8_ts_per_pixel = dc->bit_depth >> 3;
    /* Convert Y to RGB and put it into the frame buffer */
    if (dc_uint8_ts_per_pixel == 2)
    {

    }
    else if (dc_uint8_ts_per_pixel == 4)
    {
        uint32_t *writebuf = (uint32_t *)dc->frame_buf;
        int img_x1, img_x2, img_y1, img_y2;
        img_x1 = rect->left + img->offset_x;
        img_x2 = rect->right + img->offset_x;
        img_y1 = rect->top + img->offset_y;
        img_y2 = rect->bottom + img->offset_y;
        // gui_log("rect left %d right %d top %d bot %d \n",img_x1,img_x2,img_y1,img_y2);
        if (img_x1 >= dc->fb_width ||
            img_x2 <= 0 ||
            img_y1 >= dc->fb_height ||
            img_y2 <= 0)
        {
            return 1;
        }
        int x_start, x_end, y_start, y_end;
        int x_left, y_up;
        int bitmap_width = img_x2 - img_x1 + 1;

        x_start = _UI_MAX(img_x1, 0);
        x_left = x_start - img_x1;
        x_end = _UI_MIN(img_x2, dc->fb_width - 1);

        y_start = _UI_MAX(img_y1, 0);
        y_up = y_start - img_y1;
        y_end = _UI_MIN(img_y2, dc->fb_height - 1);

        bitmap = (void *)((uint8_t *)bitmap + (bitmap_width * y_up + x_left) * 3);
        for (uint32_t y = y_start; y <= y_end; y++)
        {
            uint8_t *src = (uint8_t *)bitmap;
            int write_off = (y - dc->section.y1) * dc->fb_width;
            for (uint32_t x = x_start; x <= x_end; x++)
            {
                memcpy(writebuf + write_off + x, src, 3);
                src = src + 3;
            }
            bitmap = (void *)((uint8_t *)bitmap + (3 * bitmap_width));
        }
    }
    return 1;
}
static void gui_img_tjpgd_image_prepare(gui_obj_t *obj)
{
    gui_stb_img_t *this = (gui_stb_img_t *)obj;
    gui_point3f_t point = {0, 0, 1};
    matrix_multiply_point(obj->matrix, &point);
    this->offset_x = point.p[0];
    this->offset_y = point.p[1];
}

static void gui_img_tjpgd_image_draw(gui_obj_t *obj)
{
    gui_stb_img_t *img = (gui_stb_img_t *)obj;
    if (img->input_format != JPEG)
    {
        return;
    }
    JDEC jd;                /* Create a work area for JPEG decompression */
    IODEV devid;
    void *work = gui_malloc(3100); /* Pointer to the work area */

    devid.data = img->data_buffer;
    devid.size = img->data_length;
    devid.index = 0;
    devid.fbuf = work;

    if (jd_prepare(&jd, input_func, work, 3100, &devid) == JDR_OK)
    {
        /* Start to decompress */
        jd_decomp(&jd, output_func, 0, obj);
    }
    gui_free(work);
}
#endif

static void gui_stb_image_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
#ifdef TJPG
        case OBJ_PREPARE:
            gui_img_tjpgd_image_prepare(obj);
            break;

        case OBJ_DRAW:
            gui_img_tjpgd_image_draw(obj);
            break;
#endif
        case OBJ_DESTORY:
            gui_img_stb_image_destory(obj);
            break;

        default:
            break;
        }
    }
}

static void gui_img_stb_from_mem_ctor(gui_stb_img_t  *this,
                                      gui_obj_t      *parent,
                                      const char     *name,
                                      void           *addr,
                                      uint32_t        size,
                                      GUI_FormatType  input_type,
                                      GUI_FormatType  output_type,
                                      int16_t         x,
                                      int16_t         y)
{
    //for root class
    gui_obj_t *root = (gui_obj_t *)this;
    gui_obj_ctor(root, parent, name, x, y, 0, 0);
    root->type = IMAGE_FROM_MEM;
    root->obj_cb = gui_stb_image_cb;
#ifdef TJPG
    root->has_prepare_cb = true;
    root->has_draw_cb = true;
#endif
    root->has_destroy_cb = true;

    //for self
    this->src_changed = true;
    this->data_buffer = addr;
    this->data_length = size;
    this->input_format = input_type;
    this->output_format = output_type;
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
void gui_img_stb_set_attribute(gui_stb_img_t  *this,
                               void           *addr,
                               uint32_t        size,
                               GUI_FormatType  input_type,
                               int16_t         x,
                               int16_t         y)
{
    this->src_changed = true;
    this->data_buffer = addr;
    this->data_length = size;
    this->input_format = input_type;
    this->base.x = x;
    this->base.y = y;
    gui_img_stb_modify_img((gui_obj_t *)this);
}
void gui_img_stb_set_attribute_static(gui_stb_img_t  *this,
                                      void           *addr,
                                      uint32_t        size,
                                      GUI_FormatType  input_type,
                                      int16_t         x,
                                      int16_t         y)
{
    this->src_changed = true;
    this->data_buffer = addr;
    this->data_length = size;
    this->input_format = input_type;
    this->base.x = x;
    this->base.y = y;
    gui_img_stb_modify_img_static((gui_obj_t *)this);
}

void gui_img_stb_set_output_format(gui_stb_img_t  *this, GUI_FormatType output_type)
{
    this->output_format = output_type;
}

gui_stb_img_t *gui_img_stb_create_from_mem(void           *parent,
                                           const char     *name,
                                           void           *addr,
                                           uint32_t        size,
                                           GUI_FormatType  input_type,
                                           GUI_FormatType  output_type,
                                           int16_t         x,
                                           int16_t         y)
{
    GUI_ASSERT(parent != NULL);

    if (name == NULL)
    {
        name = "DEFAULT_STB_IMAGE";
    }

    gui_stb_img_t *img = gui_malloc(sizeof(gui_stb_img_t));
    GUI_ASSERT(img != NULL);

    memset(img, 0x00, sizeof(gui_stb_img_t));
    gui_img_stb_from_mem_ctor(img, (gui_obj_t *)parent, name, addr, size, input_type, output_type, x,
                              y);

    gui_list_init(&(GET_BASE(img)->child_list));
    if ((GET_BASE(img)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(img)->parent)->child_list), &(GET_BASE(img)->brother_list));
    }

    GET_BASE(img)->create_done = true;
    gui_img_stb_modify_img((gui_obj_t *)img);

    return img;
}
