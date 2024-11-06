/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file
  * @brief
  * @details
  * @author
  * @date
  * @version
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <string.h>
#include "guidef.h"
#include "gui_matrix.h"
#include "gui_obj.h"
#include "nanovg.h"
#include "gui_canvas.h"
#include "gui_fb.h"

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

static void gui_canvas_prepare(gui_canvas_t *this)
{
    GUI_UNUSED(this);
    gui_dispdev_t *dc = gui_get_dc();
    gui_obj_t *root = (gui_obj_t *)this;

    uint32_t cx = dc->fb_width / 2;
    uint32_t cy = dc->fb_height / 2;

    GUI_UNUSED(root);
    GUI_UNUSED(cx);
    GUI_UNUSED(cy);
    int last = this->checksum;
    this->checksum = 0;
    this->checksum = gui_obj_checksum(0, (uint8_t *)this, sizeof(gui_canvas_t));
    if (last != this->checksum || this->render)
    {
        gui_fb_change();
    }
}

static void gui_canvas_widget_nanovg_draw_cb(gui_obj_t *obj)
{
    gui_canvas_t *this = (gui_canvas_t *)obj;
    gui_dispdev_t *dc = gui_get_dc();
    NVGcontext *vg;

    if (this->nanovg_canvas_cb != NULL)
    {
        extern NVGcontext *nvgCreateAGGE(uint32_t w,
                                         uint32_t h,
                                         uint32_t stride,
                                         enum     NVGtexture format,
                                         uint8_t *data);
        extern void nvgDeleteAGGE(NVGcontext * ctx);

        vg = nvgCreateAGGE(dc->fb_width, dc->fb_height, dc->fb_width * (dc->bit_depth >> 3),
                           (dc->bit_depth >> 3) == 2 ? NVG_TEXTURE_BGR565 : NVG_TEXTURE_BGRA, dc->frame_buf);
        nvgBeginFrame(vg, dc->fb_width, dc->fb_height, 1);

        this->vg = vg;

        nvgScissor(vg, 0, 0, dc->fb_width, dc->fb_height);
        nvgResetTransform(vg);
        nvgTranslate(vg, 0, - (float)dc->fb_height * (float)dc->section_count);
        nvgTransformxyz(vg, \
                        obj->matrix->m[0][0], obj->matrix->m[1][0], obj->matrix->m[0][1],
                        obj->matrix->m[1][1], obj->matrix->m[0][2], obj->matrix->m[1][2],
                        obj->matrix->m[2][0], obj->matrix->m[2][1], obj->matrix->m[2][2]);

        this->nanovg_canvas_cb(this);

        nvgEndFrame(vg);
        nvgDeleteAGGE(vg);
    }
}

static void gui_canvas_widget_nanovg_end(gui_obj_t *obj)
{
    gui_canvas_t *this = (gui_canvas_t *)obj;
    GUI_UNUSED(this);
}

static void gui_canvas_widget_nanovg_destory(gui_obj_t *obj)
{

}

static void gui_canvas_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            gui_canvas_prepare((gui_canvas_t *)obj);
            break;

        case OBJ_DRAW:
            gui_canvas_widget_nanovg_draw_cb(obj);
            break;

        case OBJ_END:
            gui_canvas_widget_nanovg_end(obj);
            break;

        case OBJ_DESTORY:
            gui_canvas_widget_nanovg_destory(obj);
            break;

        default:
            break;
        }
    }
}

static void gui_canvas_widget_nanovg_ctor(gui_canvas_t *this,
                                          gui_obj_t    *parent,
                                          const char   *name,
                                          void         *data,
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
    root->type = CANVAS;
    root->obj_cb = gui_canvas_cb;
    root->has_prepare_cb = true;
    root->has_draw_cb = true;
    root->has_end_cb = true;
    root->has_destroy_cb = true;
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

void gui_canvas_set_canvas_cb(gui_canvas_t *this, void (*cb)(gui_canvas_t *this))
{
    this->nanovg_canvas_cb = cb;
}

gui_canvas_t *gui_canvas_create(void       *parent,
                                const char *name,
                                void       *data,
                                int16_t     x,
                                int16_t     y,
                                int16_t     w,
                                int16_t     h)
{
    gui_dispdev_t *dc = gui_get_dc();

    GUI_ASSERT(parent != NULL);
    if (name == NULL)
    {
        name = "WIDGET_nanovg";
    }

    gui_canvas_t *this = gui_malloc(sizeof(gui_canvas_t));
    GUI_ASSERT(this != NULL);
    memset(this, 0x00, sizeof(gui_canvas_t));

    gui_canvas_widget_nanovg_ctor(this, (gui_obj_t *)parent, name, data, x, y, w, h);

    gui_list_init(&(GET_BASE(this)->child_list));
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list),
                               &(GET_BASE(this)->brother_list));
    }

    GET_BASE(this)->create_done = true;
    return this;
}
#include "draw_img.h"
const uint8_t *gui_canvas_output(int format, bool compression, int image_width, int image_height,
                                 gui_canvas_render_function renderer)
{
    int pixel_length = 4;
    int data_length = 0;
    uint8_t *buffer = 0;
    uint8_t *output_data = 0;
    switch (format)
    {
    case GUI_CANVAS_OUTPUT_PNG:
    case GUI_CANVAS_OUTPUT_JPG:
        {
            data_length = image_width * image_height * pixel_length;
            buffer = gui_lower_malloc(data_length);
        }
        break;
    case GUI_CANVAS_OUTPUT_RGBA:
        {
            data_length = image_width * image_height * pixel_length + sizeof(gui_rgb_data_head_t);
            output_data = gui_lower_malloc(data_length);
            buffer = output_data + sizeof(gui_rgb_data_head_t);
            gui_rgb_data_head_t *head = (void *)output_data;
            head->scan = 0;
            head->align = 0;
            head->resize = 0;
            head->compress = 0;
            head->rsvd = 0;
            head->type = ARGB8888;
            head->version = 0;
            head->rsvd2 = 0;
            head->w = image_width;
            head->h = image_height;
        }
        break;
    case GUI_CANVAS_OUTPUT_RGB565:
        {
            pixel_length = 2;
            data_length = image_width * image_height * pixel_length + sizeof(gui_rgb_data_head_t);
            output_data = gui_lower_malloc(data_length);
            buffer = output_data + sizeof(gui_rgb_data_head_t);
            gui_rgb_data_head_t *head = (void *)output_data;
            head->scan = 0;
            head->align = 0;
            head->resize = 0;
            head->compress = 0;
            head->rsvd = 0;
            head->type = RGB565;
            head->version = 0;
            head->rsvd2 = 0;
            head->w = image_width;
            head->h = image_height;
        }
        break;
    default:
        break;
    }

    {
        NVGcontext *vg = 0;
        extern NVGcontext *nvgCreateAGGE(uint32_t w,
                                         uint32_t h,
                                         uint32_t stride,
                                         enum     NVGtexture format,
                                         uint8_t *data);
        extern void nvgDeleteAGGE(NVGcontext * ctx);
        vg = nvgCreateAGGE(image_width, image_height, image_width * (pixel_length),
                           (pixel_length) == 2 ? NVG_TEXTURE_BGR565 : NVG_TEXTURE_BGRA, buffer);
        nvgBeginFrame(vg, image_width, image_height, 1);

        nvgResetTransform(vg);

        renderer(vg);

        nvgEndFrame(vg);
        nvgDeleteAGGE(vg);
    }
    return output_data;
}
/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */
