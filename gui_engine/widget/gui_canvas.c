/**
 * @file gui_canvas.c
 * @author howie_wang (howie_wang@realtek.com.cn)
 * @brief
 * @version 0.1
 * @date 2023-10-16
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <guidef.h>
#include <string.h>
#include <gui_matrix.h>
#include <gui_obj.h>
#include <nanovg.h>
#include "gui_canvas.h"


static void prepare(gui_canvas_t *this)
{
    GUI_UNUSED(this);
    gui_dispdev_t *dc = gui_get_dc();
    gui_obj_t *root = (gui_obj_t *)this;

    uint32_t cx = dc->fb_width / 2;
    uint32_t cy = dc->fb_height / 2;

    GUI_UNUSED(root);
    GUI_UNUSED(cx);
    GUI_UNUSED(cy);
}


void gui_canvas_set_canvas_cb(gui_canvas_t *this,
                              void (*cb)(gui_canvas_t *this))
{
    this->nanovg_canvas_cb = cb;
}

static void widget_nanovg_draw_cb(gui_obj_t *obj)
{
    gui_canvas_t *this = (gui_canvas_t *)obj;
    gui_dispdev_t *dc = gui_get_dc();

    if (this->nanovg_canvas_cb != NULL)
    {
        extern NVGcontext *nvgCreateAGGE(uint32_t w, uint32_t h, uint32_t stride, enum NVGtexture format,
                                         uint8_t *data);
        extern void nvgDeleteAGGE(NVGcontext * ctx);
        NVGcontext *vg = nvgCreateAGGE(dc->fb_width, dc->fb_height, dc->fb_width * (dc->bit_depth >> 3),
                                       (dc->bit_depth >> 3) == 2 ? NVG_TEXTURE_BGR565 : NVG_TEXTURE_BGRA, dc->frame_buf);
        nvgBeginFrame(vg, dc->fb_width, dc->fb_height, 1);

        this->vg = vg;
        nvgResetTransform(vg);

        nvgTranslate(vg, GET_BASE(this)->dx, GET_BASE(this)->dy);
        nvgTranslate(vg, GET_BASE(this)->tx, GET_BASE(this)->ty);
        nvgTranslate(vg, GET_BASE(this)->ax, GET_BASE(this)->ay);

        nvgTranslate(vg, dc->screen_width / 2, dc->screen_height / 2);
        nvgScale(vg, this->base.sx, this->base.sy);
        nvgTranslate(vg, -dc->screen_width / 2, -dc->screen_height / 2);


        this->nanovg_canvas_cb(this);

        nvgEndFrame(vg);
        nvgDeleteAGGE(vg);
    }
}
static void widget_nanovg_end(gui_obj_t *obj)
{
    gui_canvas_t *this = (gui_canvas_t *)obj;
    GUI_UNUSED(this);

}
static void widget_nanovg_destory(gui_obj_t *obj)
{

}



void widget_nanovg_ctor(gui_canvas_t *this, gui_obj_t *parent, const char *name,
                        void *data,
                        int16_t x,
                        int16_t y, int16_t w, int16_t h)
{
    //for base class
    gui_obj_t *base = (gui_obj_t *)this;
    gui_obj_ctor(base, parent, name, x, y, w, h);

    //for root class
    gui_obj_t *root = (gui_obj_t *)this;
    root->type = VG_LITE_CLOCK;
    root->obj_prepare = (void (*)(struct _gui_obj_t *))prepare;
    root->obj_draw = widget_nanovg_draw_cb;
    root->obj_end = widget_nanovg_end;
    root->obj_destory = widget_nanovg_destory;

    //for self

}

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
 * @return gui_canvas_t*
 */

gui_canvas_t *gui_canvas_create(void *parent,  const char *name, void *data,
                                int16_t x,
                                int16_t y, int16_t w, int16_t h)
{
    GUI_ASSERT(parent != NULL);
    if (name == NULL)
    {
        name = "WIDGET_nanovg";
    }
    gui_canvas_t *this = gui_malloc(sizeof(gui_canvas_t));
    GUI_ASSERT(this != NULL);
    memset(this, 0x00, sizeof(gui_canvas_t));

    widget_nanovg_ctor(this, (gui_obj_t *)parent, name, data, x, y, w, h);

    gui_list_init(&(GET_BASE(this)->child_list));
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list),
                               &(GET_BASE(this)->brother_list));
    }

    GET_BASE(this)->create_done = true;
    return this;
}



