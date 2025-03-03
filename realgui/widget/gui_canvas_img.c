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
#include <guidef.h>
#include <string.h>
#include <gui_matrix.h>
#include <gui_obj.h>
#include <nanovg.h>
#include "gui_canvas_img.h"


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

extern NVGcontext *nvgCreateAGGE(uint32_t w, uint32_t h, uint32_t stride, enum NVGtexture format,
                                 uint8_t *data);
extern void nvgDeleteAGGE(NVGcontext *ctx);


static void widget_nanovg_canvas_img_prepare(gui_canvas_img_t *this)
{
    if (this->bpImageBuff && this->fpCanvas_img_update_cb)
    {
        if (this->fpCanvas_img_update_cb(this))
        {
            this->bImageActive = false;
        }
    }
}
extern NVGcontext *nvgCreateAGGE(uint32_t w, uint32_t h, uint32_t stride, enum NVGtexture format,
                                 uint8_t *data);
extern void nvgDeleteAGGE(NVGcontext *ctx);
// #define CHECK_REFRESH
static void widget_nanovg_canvas_img_draw_cb(gui_obj_t *obj)
{
#ifdef CHECK_REFRESH
    uint32_t start_us, end_us;
#endif
//  uint32_t nRpCount = 0;
    uint32_t ulTransColor = 0;
    uint32_t ulCount, total;
    gui_canvas_t *this = (gui_canvas_t *)obj;
    gui_canvas_img_t *mpCanvasImg = (gui_canvas_img_t *) obj;
    gui_dispdev_t *dc;

    if (this->nanovg_canvas_cb != NULL && mpCanvasImg->bpImageBuff && !mpCanvasImg->bImageActive)
    {
#ifdef CHECK_REFRESH
#ifndef __WIN32
        start_us = sys_timestamp_get_us();
#endif
#endif
        dc = gui_get_dc();
        NVGcontext *vg = nvgCreateAGGE(this->base.w, this->base.h, this->base.w * (dc->bit_depth >> 3),
                                       ((dc->bit_depth >> 3) == 2 ? NVG_TEXTURE_BGR565 : NVG_TEXTURE_BGRA),
                                       (mpCanvasImg->bpImageBuff + sizeof(struct gui_rgb_data_head)));
        nvgBeginFrame(vg, this->base.w, this->base.h, 1);
        this->vg = vg;
        nvgResetTransform(vg);
        if ((mpCanvasImg->cBlue) || (mpCanvasImg->cGreen) || (mpCanvasImg->cRed))
        {
            nvgBeginPath(vg);
            nvgRect(vg, 0, 0, this->base.w, this->base.h);
            nvgFillColor(vg, nvgRGBA(mpCanvasImg->cRed, mpCanvasImg->cGreen, mpCanvasImg->cBlue, 255));
            nvgFill(vg);
            nvgClosePath(vg);
            ulTransColor = 0;
            if (dc->bit_depth >> 3 == 4)
            {
                uint32_t *mpBuff = (uint32_t *)(mpCanvasImg->bpImageBuff + sizeof(struct gui_rgb_data_head));
                ulTransColor = mpBuff[0];
            }
            else if (dc->bit_depth >> 3 == 2)
            {
                uint16_t *mpBuff = (uint16_t *)(mpCanvasImg->bpImageBuff + sizeof(struct gui_rgb_data_head));
                ulTransColor = mpBuff[0];
            }

        }
        this->nanovg_canvas_cb(this);
        nvgEndFrame(vg);
        nvgDeleteAGGE(vg);
        if ((mpCanvasImg->cBlue) || (mpCanvasImg->cGreen) || (mpCanvasImg->cRed))
        {
            if (dc->bit_depth >> 3 == 4)
            {
                uint32_t *mpBuff = (uint32_t *)(mpCanvasImg->bpImageBuff + sizeof(struct gui_rgb_data_head));
                ulCount = 0;
//              nRpCount = 0;
                total = this->base.w * this->base.h;
                while (ulCount < total)
                {
                    if (mpBuff[0] == ulTransColor)
                    {
                        mpBuff[0] = 0;
//                      nRpCount++;
                    }
                    mpBuff++;
                    ulCount++;
                }
            }
            else if (dc->bit_depth >> 3 == 2)
            {
                uint16_t *mpBuff = (uint16_t *)(mpCanvasImg->bpImageBuff + sizeof(struct gui_rgb_data_head));
                ulCount = 0;
                total = this->base.w * this->base.h;

                while (ulCount < total)
                {
                    if (mpBuff[0] == ulTransColor)
                    {
                        mpBuff[0] = 0;
                    }
                    mpBuff++;
                    ulCount++;
                }
            }
        }

        mpCanvasImg->bpImageCtr->base.not_show = false;
        mpCanvasImg->bImageActive = true;
#ifdef CHECK_REFRESH
#ifndef __WIN32
        end_us = sys_timestamp_get_us();
        end_us -= start_us;
        gui_log("draw_time: %d.%d ms", end_us / 1000, end_us % 1000);
#endif
#endif
    }
}

static void widget_nanovg_canvas_img_end(gui_obj_t *obj)
{
//    gui_canvas_img_t *this = (gui_canvas_img_t *)obj;
    GUI_UNUSED(obj);

}
static void widget_nanovg_canvas_img_destory(gui_obj_t *obj)
{
    gui_canvas_img_t *this = (gui_canvas_img_t *)obj;
    if (this->bpImageBuff)
    {
        gui_free(this->bpImageBuff);
        this->bpImageBuff = NULL;
    }
    this->bImageActive = false;

}

static void gui_canvas_img_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            widget_nanovg_canvas_img_prepare((gui_canvas_img_t *)obj);
            break;

        case OBJ_DRAW:
            widget_nanovg_canvas_img_draw_cb(obj);
            break;

        case OBJ_END:
            widget_nanovg_canvas_img_end(obj);
            break;

        case OBJ_DESTORY:
            widget_nanovg_canvas_img_destory(obj);
            break;

        default:
            break;
        }
    }
}

static void widget_nanovg_img_ctor(gui_canvas_img_t *this, gui_obj_t *parent, const char *name,
                                   void *data,
                                   int16_t x,
                                   int16_t y, int16_t w, int16_t h)
{
    //for base class
    gui_obj_t *base = (gui_obj_t *)this;
    gui_dispdev_t *dc = gui_get_dc();
    uint32_t ulSize = 0;

    gui_obj_ctor(base, parent, name, x, y, w, h);

    //for root class
    gui_obj_t *root = (gui_obj_t *)this;
    root->type = CANVAS_IMG;
    root->obj_cb = gui_canvas_img_cb;
    root->has_prepare_cb = true;
    root->has_draw_cb = true;
    root->has_end_cb = true;
    root->has_destory_cb = true;

    ulSize = w * h * (dc->bit_depth >> 3) + sizeof(struct gui_rgb_data_head);
    this->bpImageBuff = gui_malloc(ulSize);
    GUI_ASSERT(this->bpImageBuff != NULL);
    memset(this->bpImageBuff, 0, ulSize);
    if (this->bpImageBuff != NULL)
    {
        struct gui_rgb_data_head *mpImgHeader = (struct gui_rgb_data_head *)this->bpImageBuff;
        mpImgHeader->type = (dc->bit_depth >> 3) == 2 ? RGB565 : ARGB8888;
        mpImgHeader->w = w;
        mpImgHeader->h = h;
    }
    if (this->bpImageBuff != NULL)
    {
        this->bpImageCtr = gui_img_create_from_mem(parent, "czb_image", this->bpImageBuff, x, y, w, h);
        if (this->bpImageCtr)
        {
            this->bpImageCtr->base.not_show = true;
        }
        else if (this->bpImageBuff)
        {
            gui_free(this->bpImageBuff);
            this->bpImageBuff = NULL;
        }
    }
}
/*============================================================================*
 *                           Public Functions
 *============================================================================*/

void gui_canvas_img_set_canvas_cb(gui_canvas_img_t *this,
                                  void (*cb)(gui_canvas_t *this))
{
    this->mCanvas.nanovg_canvas_cb = cb;
}

void gui_canvas_img_set_update_cb(gui_canvas_img_t *this, bool (*cb)(gui_canvas_img_t *this))
{
    this->fpCanvas_img_update_cb = cb;
}


gui_canvas_img_t *gui_canvas_img_create(void *parent,  const char *name, void *data,
                                        int16_t x,
                                        int16_t y, int16_t w, int16_t h)
{
    GUI_ASSERT(parent != NULL);
    if (name == NULL)
    {
        name = "WIDGET_canvas_img";
    }
    gui_canvas_img_t *this = gui_malloc(sizeof(gui_canvas_img_t));
    GUI_ASSERT(this != NULL);
    memset(this, 0x00, sizeof(gui_canvas_img_t));

    widget_nanovg_img_ctor(this, (gui_obj_t *)parent, name, data, x, y, w, h);

    gui_list_init(&(GET_BASE(this)->child_list));
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list),
                               &(GET_BASE(this)->brother_list));
    }

    GET_BASE(this)->create_done = true;

    return this;
}

void gui_canvas_img_set_TransColor(gui_canvas_img_t *this,  uint8_t r, uint8_t g, uint8_t b)
{

    this->cRed = r;
    this->cGreen = g;
    this->cBlue = b;
}

