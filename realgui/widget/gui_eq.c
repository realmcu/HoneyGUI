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
#include <math.h>
#include "gui_matrix.h"
#include "gui_obj.h"
#include "nanovg.h"
#include "gui_eq.h"


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

static float time = 0;

extern NVGcontext *nvgCreateAGGE(uint32_t w, uint32_t h, uint32_t stride, enum NVGtexture format,
                                 uint8_t *data);
extern void nvgDeleteAGGE(NVGcontext *ctx);

/*============================================================================*
 *                           Private Functions
 *============================================================================*/

static void gui_eq_draw_graph(NVGcontext *vg,
                              float       x,
                              float       y,
                              float       w,
                              float       h,
                              float       t)
{
    float samples[6];
    float sx[6], sy[6];
    float dx = w / 5.0f;
    int i;

    samples[0] = (1 + sinf(t * 1.2345f + cosf(t * 0.33457f) * 0.44f)) * 0.5f;
    samples[1] = (1 + sinf(t * 0.68363f + cosf(t * 1.3f) * 1.55f)) * 0.5f;
    samples[2] = (1 + sinf(t * 1.1642f + cosf(t * 0.33457f) * 1.24f)) * 0.5f;
    samples[3] = (1 + sinf(t * 0.56345f + cosf(t * 1.63f) * 0.14f)) * 0.5f;
    samples[4] = (1 + sinf(t * 1.6245f + cosf(t * 0.254f) * 0.3f)) * 0.5f;
    samples[5] = (1 + sinf(t * 0.345f + cosf(t * 0.03f) * 0.6f)) * 0.5f;

    for (i = 0; i < 6; i++)
    {
        sx[i] = x + i * dx;
        sy[i] = y + h * samples[i] * 0.8f;
    }

    nvgBeginPath(vg);
    nvgMoveTo(vg, sx[0], sy[0]);
    for (i = 1; i < 6; i++)
    {
        nvgBezierTo(vg, sx[i - 1] + dx * 0.5f, sy[i - 1], sx[i] - dx * 0.5f, sy[i], sx[i], sy[i]);
    }

    nvgStrokeColor(vg, nvgRGBA(0, 160, 192, 255));
    nvgStrokeWidth(vg, 3.0f);
    nvgStroke(vg);
}

static void gui_eq_prepare(gui_obj_t *obj)
{
    GUI_UNUSED(obj);
    gui_dispdev_t *dc = gui_get_dc();
    gui_obj_t *root = (gui_obj_t *)obj;

    uint32_t cx = dc->fb_width / 2;
    uint32_t cy = dc->fb_height / 2;
    GUI_UNUSED(root);
    GUI_UNUSED(cx);
    GUI_UNUSED(cy);
}

static void gui_eq_draw_cb(gui_obj_t *obj)
{
    gui_eq_t *this = (gui_eq_t *)obj;
    gui_dispdev_t *dc = gui_get_dc();
    NVGcontext *vg;
    GUI_UNUSED(this);
    GUI_UNUSED(dc);
    float x = this->x;
    float y = this->y;
    float w = this->w;
    float h = this->h;

    vg = nvgCreateAGGE(dc->fb_width, dc->fb_height, dc->fb_width * (dc->bit_depth >> 3),
                       (dc->bit_depth >> 3) == 2 ? NVG_TEXTURE_BGR565 : NVG_TEXTURE_BGRA, dc->frame_buf);
    nvgBeginFrame(vg, dc->fb_width, dc->fb_height, 1);

    nvgResetTransform(vg);
    nvgTransform(vg, obj->matrix->m[0][0], obj->matrix->m[1][0], obj->matrix->m[0][1],
                 obj->matrix->m[1][1], obj->matrix->m[0][2], obj->matrix->m[1][2]);

    time = time + 0.01f;
    gui_eq_draw_graph(vg, x, y, w, h, time);

    nvgEndFrame(vg);
    nvgDeleteAGGE(vg);
}

static void gui_eq_end(gui_obj_t *obj)
{

}

static void gui_eq_destory(gui_obj_t *obj)
{

}

static void gui_eq_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            gui_eq_prepare(obj);
            break;

        case OBJ_DRAW:
            gui_eq_draw_cb(obj);
            break;

        case OBJ_END:
            gui_eq_end(obj);
            break;

        case OBJ_DESTORY:
            gui_eq_destory(obj);
            break;

        default:
            break;
        }
    }
}

static void gui_eq_ctor(gui_eq_t   *this,
                        gui_obj_t  *parent,
                        const char *name,
                        int16_t     x,
                        int16_t     y,
                        int16_t     w,
                        int16_t     h)
{
    gui_obj_t *root = (gui_obj_t *)this;
    gui_obj_ctor(root, parent, name, x, y, w, h);

    root->type = VG_LITE_CLOCK;
    root->obj_cb = gui_eq_cb;
    root->has_prepare_cb = true;
    root->has_draw_cb = true;
    root->has_end_cb = true;
    root->has_destory_cb = true;
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

void gui_eq_set(gui_eq_t *this,
                float     x,
                float     y,
                float     w,
                float     h,
                float     t)
{
    this->x = x;
    this->y = y;
    this->h = h;
    this->w = w;
    this->t = t;
}

gui_eq_t *gui_eq_create(void       *parent,
                        const char *name,
                        int16_t     x,
                        int16_t     y,
                        int16_t     w,
                        int16_t     h)
{
    gui_eq_t *this;

    GUI_ASSERT(parent != NULL);
    if (name == NULL)
    {
        name = "eq";
    }

    this = gui_malloc(sizeof(gui_eq_t));
    GUI_ASSERT(this != NULL);
    memset(this, 0x00, sizeof(gui_eq_t));

    gui_eq_ctor(this, (gui_obj_t *)parent, name, x, y, w, h);

    gui_list_init(&(GET_BASE(this)->child_list));
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list),
                               &(GET_BASE(this)->brother_list));
    }

    this->x = x;
    this->y = y;
    this->h = h;
    this->w = w;
    this->t = 0;

    GET_BASE(this)->create_done = true;
    return this;
}

