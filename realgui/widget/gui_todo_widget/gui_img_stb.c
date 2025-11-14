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
        uint8_t r = rgb888_data[3 * i + 0];
        uint8_t g = rgb888_data[3 * i + 1];
        uint8_t b = rgb888_data[3 * i + 2];

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
        uint8_t *rgb888_buf = gui_malloc(source_w * source_h * 3 + head_len);
        GUI_ASSERT(rgb888_buf != NULL);
        memcpy(rgb888_buf + head_len, output, source_w * source_h * 3);

        memset(&head, 0x0, sizeof(head));
        head.w = source_w;
        head.h = source_h;
        head.type = RGB888;
        memcpy(output, &head, sizeof(head));

        gui_free(output);
        return rgb888_buf;
    }
    else if (img->output_format == RGB565)
    {
        output = stbi_load_from_memory(img->data_buffer, img->data_length, &source_w, &source_h,
                                       &num_components, 0);
        GUI_ASSERT(output != NULL);
        uint8_t *rgb565_buf = gui_malloc(source_w * source_h * 2 + head_len);
        GUI_ASSERT(rgb565_buf != NULL);
        rgb888_to_rgb565(output, rgb565_buf + head_len, source_w, source_h);

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

#ifdef TJPG
size_t input_func(JDEC *jd, uint8_t *buff, size_t nuint8_t)
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
int output_func(JDEC *jd, void *bitmap, JRECT *rect)
{
    IODEV *dev = (IODEV *)jd->device;
    if (!dev || !dev->dst || dev->width == 0 || dev->bpp == 0)
    {
        return 0;
    }

    const uint32_t rx = rect->right  - rect->left + 1;
    const uint32_t ry = rect->bottom - rect->top  + 1;

    const uint8_t *src = (const uint8_t *)bitmap;
    for (uint32_t y = 0; y < ry; ++y)
    {
        uint8_t *dst_row = dev->dst + ((rect->top + y) * dev->width + rect->left) * dev->bpp;
        memcpy(dst_row, src, rx * dev->bpp);
        src += rx * dev->bpp;
    }
    return 1;
}

static uint8_t *gui_img_tjpgd_decode_image(gui_stb_img_t *img)
{
    if (img == NULL || img->data_buffer == NULL || img->data_length == 0)
    {
        return NULL;
    }
    if ((img->output_format == RGB565 && JD_FORMAT != 1) ||
        (img->output_format == RGB888 && JD_FORMAT != 0))
    {
        gui_log("tjpgd: output_format not supported by JD_FORMAT=%d, please modify JD_FORMAT\n", JD_FORMAT);
        return NULL;
    }

    void *work = gui_malloc(TJPGD_WORKSPACE_SIZE);
    GUI_ASSERT(work != NULL);

    IODEV devid;
    memset(&devid, 0, sizeof(devid));
    devid.data = (const uint8_t *)img->data_buffer;
    devid.size = img->data_length;
    devid.index = 0;
    devid.fbuf = work;

    JDEC jd;
    memset(&jd, 0, sizeof(jd));
    jd.swap = 0;

    JRESULT ret = jd_prepare(&jd, input_func, work, TJPGD_WORKSPACE_SIZE, &devid);
    if (ret != JDR_OK)
    {
        gui_log("tjpgd: jd_prepare failed, ret = %d\n", ret);
        gui_free(work);
        return NULL;
    }

    const uint32_t w = jd.width;
    const uint32_t h = jd.height;

    gui_rgb_data_head_t head;
    memset(&head, 0x0, sizeof(head));
    head.w = w;
    head.h = h;
    head.type = img->output_format;

    int head_len = sizeof(gui_rgb_data_head_t);
    uint8_t *output = NULL;
    uint8_t bpp = img->output_format == RGB888 ? 3 : 2;

    output = gui_malloc(w * h * bpp + head_len);
    GUI_ASSERT(output != NULL);
    memcpy(output, &head, head_len);
    // gui_log("tjpgd: w = %d, h = %d, bpp = %d, output = %p\n", w, h, bpp, output);

    devid.dst = output + head_len;
    devid.width = w;
    devid.height = h;
    devid.bpp = img->output_format == RGB888 ? 3 : 2;

    ret = jd_decomp(&jd, output_func, 0);
    if (ret != JDR_OK)
    {
        gui_log("jd_decomp failed, ret = %d\n", ret);
        gui_free(output);
        gui_free(work);
        return NULL;
    }
    gui_free(work);
    return output;
}
#endif

static void gui_img_stb_modify_img(gui_obj_t *obj)
{
    gui_stb_img_t *img = (gui_stb_img_t *)obj;

    if ((img->data_buffer == NULL)
        || (img->data_length == 0)
        || (img->src_changed == false))
    {
        return;
    }

    uint8_t *decoded;
    switch (img->input_format)
    {
    case JPEG:
#ifdef TJPG
        decoded = gui_img_tjpgd_decode_image(img);
        break;
#endif
    case BMP:
    case PNG:
        decoded = gui_img_stb_decode_image(img);
        break;
    default:
        break;
    }
    if (decoded == NULL)
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
        img->img->data = decoded;
    }
    else
    {
        img->img = gui_img_create_from_mem(obj, "stb_img", decoded, 0, 0, 0, 0);
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

    uint8_t *decoded;
    switch (img->input_format)
    {
    case JPEG:
#ifdef TJPG
        decoded = gui_img_tjpgd_decode_image(img);
        break;
#endif
    case BMP:
    case PNG:
        decoded = gui_img_stb_decode_image(img);
        break;
    default:
        break;
    }
    if (decoded == NULL)
    {
        return;
    }
    if (img->img)
    {
        img->img->data = decoded;
    }
    else
    {
        img->img = gui_img_create_from_mem(obj, "stb_img", decoded, 0, 0, 0, 0);
    }
    img->src_changed = false;
}

static void gui_img_stb_image_destroy(gui_obj_t *obj)
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


static void gui_stb_image_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_DESTROY:
            gui_img_stb_image_destroy(obj);
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
