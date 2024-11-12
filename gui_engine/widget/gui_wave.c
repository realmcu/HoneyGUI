/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_wave.c
  * @brief wave widget
  * @details wave widget
  * @author howie_wang@realsil.com.cn
  * @date 2023/11/07
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
#include <math.h>
#include "gui_matrix.h"
#include "gui_obj.h"
#include "nanovg.h"
#include "gui_wave.h"

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

extern NVGcontext *nvgCreateAGGE(uint32_t w, uint32_t h, uint32_t stride, enum NVGtexture format,
                                 uint8_t *data);
extern void nvgDeleteAGGE(NVGcontext *ctx);

/*============================================================================*
 *                           Private Functions
 *============================================================================*/

static void gui_wave_draw_graph(NVGcontext *vg,
                                float       x,
                                float       y,
                                float       w,
                                float       h,
                                float       t)
{
    int item_count = 10;
    NVGpaint bg;
    float samples[item_count];
    float sx[item_count], sy[item_count];
    float dx = w / (item_count - 1);
    int i;

    samples[0] = (1 + sinf(t * 1.2345f + cosf(t * 0.33457f) * 0.44f)) * 0.5f;
    samples[1] = (1 + sinf(t * 0.68363f + cosf(t * 1.3f) * 1.55f)) * 0.5f;
    samples[2] = (1 + sinf(t * 1.1642f + cosf(t * 0.33457f) * 1.24f)) * 0.5f;
    samples[3] = (1 + sinf(t * 0.56345f + cosf(t * 1.63f) * 0.14f)) * 0.5f;
    samples[4] = (1 + sinf(t * 1.6245f + cosf(t * 0.254f) * 0.3f)) * 0.5f;
    samples[5] = (1 + sinf(t * 0.345f + cosf(t * 0.03f) * 0.6f)) * 0.5f;
    samples[6] = (1 + sinf(t * 1.2345f + cosf(t * 0.33457f) * 0.44f)) * 0.5f;
    samples[7] = (1 + sinf(t * 0.68363f + cosf(t * 1.3f) * 1.55f)) * 0.5f;
    samples[8] = (1 + sinf(t * 1.1642f + cosf(t * 0.33457f) * 1.24f)) * 0.5f;
    samples[9] = (1 + sinf(t * 0.345f + cosf(t * 0.03f) * 0.6f)) * 0.5f;
    for (i = 0; i < item_count; i++)
    {
        sx[i] = x + i * dx;
        sy[i] = y + h * samples[i] * 0.8f;
    }

    // Graph background
    bg = nvgLinearGradient(vg, x, y, x, y + h, nvgRGBA(0, 160, 192, 0), nvgRGBA(0, 160, 192, 255));
    nvgBeginPath(vg);
    nvgMoveTo(vg, sx[0], sy[0]);
    for (i = 1; i < item_count; i++)
    {
        nvgBezierTo(vg, sx[i - 1] + dx * 0.5f, sy[i - 1], sx[i] - dx * 0.5f, sy[i], sx[i], sy[i]);
    }

    nvgLineTo(vg, x + w, y + h);
    nvgLineTo(vg, x, y + h);
    nvgFillPaint(vg, bg);
    nvgFill(vg);

    // Graph line
    nvgBeginPath(vg);
    nvgMoveTo(vg, sx[0], sy[0] + 2);
    for (i = 1; i < item_count; i++)
    {
        nvgBezierTo(vg, sx[i - 1] + dx * 0.5f, sy[i - 1] + 2, sx[i] - dx * 0.5f, sy[i] + 2, sx[i],
                    sy[i] + 2);
    }
    nvgStrokeColor(vg, nvgRGBA(0, 0, 0, 32));
    nvgStrokeWidth(vg, 3.0f);
    nvgStroke(vg);

    nvgBeginPath(vg);
    nvgMoveTo(vg, sx[0], sy[0]);
    for (i = 1; i < item_count; i++)
    {
        nvgBezierTo(vg, sx[i - 1] + dx * 0.5f, sy[i - 1], sx[i] - dx * 0.5f, sy[i], sx[i], sy[i]);
    }

    nvgStrokeColor(vg, nvgRGBA(0, 160, 192, 255));
    nvgStrokeWidth(vg, 3.0f);
    nvgStroke(vg);

    // Graph sample pos
    for (i = 0; i < item_count; i++)
    {
        bg = nvgRadialGradient(vg, sx[i], sy[i] + 2, 3.0f, 8.0f, nvgRGBA(0, 0, 0, 32), nvgRGBA(0, 0, 0, 0));
        nvgBeginPath(vg);
        nvgRect(vg, sx[i] - 10, sy[i] - 10 + 2, 20, 20);
        nvgFillPaint(vg, bg);
        nvgFill(vg);
    }

    nvgBeginPath(vg);
    for (i = 0; i < item_count; i++)
    {
        nvgCircle(vg, sx[i], sy[i], 4.0f);
    }

    nvgFillColor(vg, nvgRGBA(0, 160, 192, 255));
    nvgFill(vg);
    nvgBeginPath(vg);
    for (i = 0; i < item_count; i++)
    {
        nvgCircle(vg, sx[i], sy[i], 2.0f);
    }

    nvgFillColor(vg, nvgRGBA(220, 220, 220, 255));
    nvgFill(vg);

    nvgStrokeWidth(vg, 1.0f);
}

static void gui_wave_prepare(gui_obj_t *obj)
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

static void gui_wave_draw_cb(gui_obj_t *obj)
{
    gui_wave_t *this = (gui_wave_t *)obj;
    gui_dispdev_t *dc = gui_get_dc();

    GUI_UNUSED(this);
    GUI_UNUSED(dc);
    float x = this->x;
    float y = this->y;
    float w = this->w;
    float h = this->h;
    float t = this->t;
    NVGcontext *vg = nvgCreateAGGE(dc->fb_width, dc->fb_height, dc->fb_width * (dc->bit_depth >> 3),
                                   (dc->bit_depth >> 3) == 2 ? NVG_TEXTURE_BGR565 : NVG_TEXTURE_BGRA, dc->frame_buf);

    nvgBeginFrame(vg, dc->fb_width, dc->fb_height, 1);
    nvgScissor(vg, 0, 0, dc->fb_width, dc->fb_height);
    nvgResetTransform(vg);
    nvgTranslate(vg, 0, - (float)dc->fb_height * (float)dc->section_count);
    nvgTransformxyz(vg, \
                    obj->matrix->m[0][0], obj->matrix->m[1][0], obj->matrix->m[0][1],
                    obj->matrix->m[1][1], obj->matrix->m[0][2], obj->matrix->m[1][2],
                    obj->matrix->m[2][0], obj->matrix->m[2][1], obj->matrix->m[2][2]);

    gui_wave_draw_graph(vg, x, y, w, h, t);

    nvgEndFrame(vg);
    nvgDeleteAGGE(vg);
}

static void gui_wave_end(gui_obj_t *obj)
{

}

static void gui_wave_destory(gui_obj_t *obj)
{

}

static void gui_wave_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            gui_wave_prepare(obj);
            break;

        case OBJ_DRAW:
            gui_wave_draw_cb(obj);
            break;

        case OBJ_END:
            gui_wave_end(obj);
            break;

        case OBJ_DESTORY:
            gui_wave_destory(obj);
            break;

        default:
            break;
        }
    }
}

static void gui_wave_ctor(gui_wave_t *this,
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
    root->obj_cb = gui_wave_cb;
    root->has_prepare_cb = true;
    root->has_draw_cb = true;
    root->has_end_cb = true;
    root->has_destroy_cb = true;
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

void gui_wave_set(gui_wave_t *this,
                  float       x,
                  float       y,
                  float       w,
                  float       h,
                  float       t)
{
    this->x = x;
    this->y = y;
    this->h = h;
    this->w = w;
    this->t = t;
}

gui_wave_t *gui_wave_create(void       *parent,
                            const char *name,
                            int16_t     x,
                            int16_t     y,
                            int16_t     w,
                            int16_t     h)
{
    gui_wave_t *this;

    GUI_ASSERT(parent != NULL);
    if (name == NULL)
    {
        name = "wave";
    }

    this = gui_malloc(sizeof(gui_wave_t));
    GUI_ASSERT(this != NULL);
    memset(this, 0x00, sizeof(gui_wave_t));

    gui_wave_ctor(this, (gui_obj_t *)parent, name, x, y, w, h);

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

void gui_wave_render(NVGcontext *vg, int16_t x, int16_t y, int16_t w,
                     int16_t h,
                     int16_t item_count,
                     float *samples,
                     gui_color_t  color,
                     int16_t max,
                     int16_t min)
{
    if (!samples)
    {
        return;
    }
    if (!item_count)
    {
        return;
    }
    NVGpaint bg;
    float sx[item_count], sy[item_count];
    float dx = w;
    if (item_count != 1)
    {
        dx = w / (item_count - 1);
    }

    int i;
    for (i = 0; i < item_count; i++)
    {
        sx[i] =  i * dx;
        sy[i] =  h - h * ((samples[i] - min) / (max - min));
    }

    // Graph background
    bg = nvgLinearGradient(vg, x, y, x, y + h, nvgRGBA(color.color.rgba.r, color.color.rgba.g,
                                                       color.color.rgba.b, 50), nvgRGBA(color.color.rgba.r, color.color.rgba.g, color.color.rgba.b, 240));
    nvgBeginPath(vg);
    nvgMoveTo(vg, sx[0], sy[0]);
    for (i = 1; i < item_count; i++)
    {
        nvgBezierTo(vg, sx[i - 1] + dx * 0.5f, sy[i - 1], sx[i] - dx * 0.5f, sy[i], sx[i], sy[i]);
    }

    nvgLineTo(vg, x + w, y + h);
    nvgLineTo(vg, x, y + h);
    nvgFillPaint(vg, bg);
    nvgFill(vg);

    // Graph line
    nvgBeginPath(vg);
    nvgMoveTo(vg, sx[0], sy[0] + 2);
    for (i = 1; i < item_count; i++)
    {
        nvgBezierTo(vg, sx[i - 1] + dx * 0.5f, sy[i - 1] + 2, sx[i] - dx * 0.5f, sy[i] + 2, sx[i],
                    sy[i] + 2);
    }
    nvgStrokeColor(vg, nvgRGBA(0, 0, 0, 32));
    nvgStrokeWidth(vg, 3.0f);
    nvgStroke(vg);

    nvgBeginPath(vg);
    nvgMoveTo(vg, sx[0], sy[0]);
    for (i = 1; i < item_count; i++)
    {
        nvgBezierTo(vg, sx[i - 1] + dx * 0.5f, sy[i - 1], sx[i] - dx * 0.5f, sy[i], sx[i], sy[i]);
    }

    nvgStrokeColor(vg, nvgRGBA(color.color.rgba.r, color.color.rgba.g, color.color.rgba.b,
                               color.color.rgba.a));
    nvgStrokeWidth(vg, 3.0f);
    nvgStroke(vg);

    // Graph sample pos
    for (i = 0; i < item_count; i++)
    {
        bg = nvgRadialGradient(vg, sx[i], sy[i] + 2, 3.0f, 8.0f, nvgRGBA(0, 0, 0, 32), nvgRGBA(0, 0, 0, 0));
        nvgBeginPath(vg);
        nvgRect(vg, sx[i] - 10, sy[i] - 10 + 2, 20, 20);
        nvgFillPaint(vg, bg);
        nvgFill(vg);
    }

    nvgBeginPath(vg);
    for (i = 0; i < item_count; i++)
    {
        nvgCircle(vg, sx[i], sy[i], 4.0f);
    }

    nvgFillColor(vg, nvgRGBA(color.color.rgba.r, color.color.rgba.g, color.color.rgba.b,
                             color.color.rgba.a));
    nvgFill(vg);
    nvgBeginPath(vg);
    for (i = 0; i < item_count; i++)
    {
        nvgCircle(vg, sx[i], sy[i], 2.0f);
    }

    nvgFillColor(vg, nvgRGBA(220, 220, 220, 255));
    nvgFill(vg);

    nvgStrokeWidth(vg, 1.0f);
}

