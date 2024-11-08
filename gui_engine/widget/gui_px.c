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
#include "gui_obj.h"
#include "gui_fb.h"
#include "gui_px.h"
#include "tp_algo.h"

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

static void gui_px_prepare(gui_px_t *this)
{
    touch_info_t *tp = tp_get_info();
    gui_obj_t *root = (gui_obj_t *)this;
    gui_dispdev_t *dc = gui_get_dc();

    GUI_UNUSED(this);
    GUI_UNUSED(root);
    GUI_UNUSED(tp);
    GUI_UNUSED(dc);
    gui_fb_change();
}

static px_void gui_px_application_update(gui_px_t *this, px_dword elapsed)
{
    PX_ObjectUpdate(this->px_root, elapsed);
}

static px_void gui_px_application_render(gui_px_t *this, px_dword elapsed)
{
    px_surface *pRenderSurface = &this->RenderSurface;

    PX_SurfaceClear(pRenderSurface, 0, 0, pRenderSurface->width - 1, pRenderSurface->height - 1,
                    PX_COLOR_BACKGROUNDCOLOR);

    PX_ObjectRender(pRenderSurface, this->px_root, elapsed);
}

static void gui_px_draw(gui_px_t *this)
{
    touch_info_t *tp = tp_get_info();
    gui_obj_t *root = (gui_obj_t *)this;
    gui_dispdev_t *dc = gui_get_dc();
    px_surface *pRenderSurface;

    GUI_UNUSED(this);
    GUI_UNUSED(root);
    GUI_UNUSED(tp);
    GUI_UNUSED(dc);

    //time
    this->elapsed = PX_TimeGetTime() - this->timelasttime;
    this->timelasttime = PX_TimeGetTime();

    gui_px_application_update(this, this->elapsed);
    gui_px_application_render(this, this->elapsed);
    pRenderSurface = &this->RenderSurface;

    if (dc->bit_depth == 32)
    {
        memcpy(dc->frame_buf, pRenderSurface->surfaceBuffer,
               dc->fb_width * dc->fb_height * dc->bit_depth / 8);
    }
    else if (dc->bit_depth == 16)
    {
        for (uint32_t i = 0; i < dc->fb_width * dc->fb_height; i++)
        {
            px_color *pixel = pRenderSurface->surfaceBuffer + i;
            uint8_t source_alpha = pixel->_argb.a;
            uint8_t source_red = pixel->_argb.r;
            uint8_t source_green = pixel->_argb.g;
            uint8_t source_blue = pixel->_argb.b;

            color_rgb565_t *write_pixel = (color_rgb565_t *)dc->frame_buf + i;
            write_pixel->r = source_red >> 3;
            write_pixel->g = source_green >> 2;
            write_pixel->b = source_blue >> 3;
        }
    }
    else
    {
        GUI_ASSERT(NULL != NULL);
    }

}

static void gui_px_end(gui_px_t *this)
{
    touch_info_t *tp = tp_get_info();
    gui_obj_t *root = (gui_obj_t *)this;
    gui_dispdev_t *dc = gui_get_dc();

    GUI_UNUSED(this);
    GUI_UNUSED(root);
    GUI_UNUSED(tp);
    GUI_UNUSED(dc);
}

static void gui_px_destory(gui_px_t *this)
{
    touch_info_t *tp = tp_get_info();
    gui_obj_t *root = (gui_obj_t *)this;
    gui_dispdev_t *dc = gui_get_dc();

    GUI_UNUSED(this);
    GUI_UNUSED(root);
    GUI_UNUSED(tp);
    GUI_UNUSED(dc);
}

static px_bool gui_px_application_initialize(gui_px_t *this,
                                             px_int    screen_width,
                                             px_int    screen_height,
                                             void (*main_cb)(gui_px_t *this))
{
    this->mem_static = MP_Create(gui_malloc(1024 * 1024 * 2), 1024 * 1024 * 2);
    this->mp_static = &this->mem_static;
    this->mem_dynamic = MP_Create(gui_malloc(1024 * 1024 * 5), 1024 * 1024 * 5);
    this->mp_dynamic = &this->mem_dynamic;

    if (!PX_SurfaceCreate(this->mp_static, screen_width, screen_height, &this->RenderSurface))
    {
        return PX_FALSE;
    }

    PX_SurfaceClear(&this->RenderSurface, 0, 0, this->RenderSurface.width - 1,
                    this->RenderSurface.height - 1, PX_COLOR_BACKGROUNDCOLOR);

    this->px_root = PX_ObjectCreateRoot(this->mp_dynamic);

    if (main_cb != NULL)
    {
        main_cb(this);
    }
    else
    {
        GUI_ASSERT(NULL != NULL);
    }
    return PX_TRUE;
}

static void gui_px_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            gui_px_prepare((gui_px_t *)obj);
            break;

        case OBJ_DRAW:
            gui_px_draw((gui_px_t *)obj);
            break;

        case OBJ_END:
            gui_px_end((gui_px_t *)obj);
            break;

        case OBJ_DESTORY:
            gui_px_destory((gui_px_t *)obj);
            break;

        default:
            break;
        }
    }
}

static void gui_px_ctor(gui_px_t   *this,
                        gui_obj_t  *parent,
                        const char *name,
                        void (*main_cb)(gui_px_t *this),
                        int16_t     x,
                        int16_t     y,
                        int16_t     w,
                        int16_t     h)
{
    //for root class
    gui_obj_t *root = (gui_obj_t *)this;
    gui_dispdev_t *dc;

    gui_obj_ctor(root, parent, name, x, y, w, h);

    root->obj_cb = gui_px_cb;
    root->has_prepare_cb = true;
    root->has_draw_cb = true;
    root->has_end_cb = true;
    root->has_destroy_cb = true;

    //for self
    dc = gui_get_dc();
    gui_px_application_initialize(this, dc->fb_width, dc->fb_height, main_cb);
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

/**
 * @brief
 *
 * @param parent
 * @param name
 * @param data
 * @param x
 * @param y
 * @param w
 * @param h
 * @return gui_px_t*
 */
gui_px_t *gui_px_create(void       *parent,
                        const char *name,
                        void (*main_cb)(gui_px_t *this),
                        int16_t     x,
                        int16_t     y,
                        int16_t     w,
                        int16_t     h)
{
    gui_px_t *this;

    GUI_ASSERT(parent != NULL);
    if (name == NULL)
    {
        name = "PAINTER_ENGINE";
    }

    this = gui_malloc(sizeof(gui_px_t));
    GUI_ASSERT(this != NULL);
    memset(this, 0x00, sizeof(gui_px_t));

    gui_px_ctor(this, (gui_obj_t *)parent, name, main_cb, x, y, w, h);

    gui_list_init(&(GET_BASE(this)->child_list));
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list),
                               &(GET_BASE(this)->brother_list));
    }

    GET_BASE(this)->create_done = true;
    return this;
}

/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */
