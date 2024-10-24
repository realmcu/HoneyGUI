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
#include <nanovg.h>
#include "gui_matrix.h"
#include "gui_obj.h"
#include "gui_svg.h"

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
static void gui_svg_prepare(gui_obj_t *obj)
{
    uint16_t cx, cy;

    GUI_UNUSED(obj);
    gui_dispdev_t *dc = gui_get_dc();
    gui_obj_t *root = (gui_obj_t *)obj;

    cx = dc->fb_width / 2;
    cy = dc->fb_height / 2;
    GUI_UNUSED(root);
    GUI_UNUSED(cx);
    GUI_UNUSED(cy);
}

#define NANOSVG_IMPLEMENTATION
#include <nanosvg.h>

static NVGcolor gui_svg_color(unsigned int c, float opacity)
{
    return nvgRGBA(c & 0xFF, (c >> 8) & 0xFF, (c >> 16) & 0xFF, opacity * 255);
}

static NVGcolor gui_svg_color4(unsigned int c, float opacity)
{
    return nvgRGBA(c & 0xFF, (c >> 8) & 0xFF, (c >> 16) & 0xFF, ((c >> 24) & 0xFF) * opacity);
}

static NVGpaint gui_svg_create_linear_gradient(NVGcontext *vg, NSVGgradient *gradient, float alpha)
{
    float inverse[6], sx, sy, ex, ey;

    nvgTransformInverse(inverse, gradient->xform);
    nvgTransformPoint(&sx, &sy, inverse, 0, 0);
    nvgTransformPoint(&ex, &ey, inverse, 0, 1);

    return nvgLinearGradient(vg, sx, sy, ex, ey,
                             gui_svg_color4(gradient->stops[0].color, alpha),
                             gui_svg_color4(gradient->stops[gradient->nstops - 1].color, alpha));
}

static NVGpaint gui_svg_create_radial_gradient(NVGcontext *vg, NSVGgradient *gradient, float alpha)
{
    float inverse[6], cx, cy, r1, r2, inr, outr;

    nvgTransformInverse(inverse, gradient->xform);
    nvgTransformPoint(&cx, &cy, inverse, 0, 0);
    nvgTransformPoint(&r1, &r2, inverse, 0, 1);
    outr = r2 - r1;

    if (gradient->nstops == 3)
    {
        inr = gradient->stops[1].offset * outr;
    }
    else
    {
        inr = 0;
    }

    NVGpaint paint = nvgRadialGradient(vg, cx, cy, inr, outr,
                                       gui_svg_color4(gradient->stops[0].color, alpha),
                                       gui_svg_color4(gradient->stops[gradient->nstops - 1].color, alpha));

    return paint;
}

static void gui_svg_nsvg_draw(NVGcontext *vg, NSVGimage *image, float alpha)
{
    int cap;

    // iterate shapes
    for (NSVGshape *shape = image->shapes; shape != NULL; shape = shape->next)
    {
        // skip invisible shape
        if (!(shape->flags & NSVG_FLAGS_VISIBLE))
        {
            continue;
        }

        nvgBeginPath(vg);
        bool pathHole = false;

        // draw paths
        for (NSVGpath *path = shape->paths; path != NULL; path = path->next)
        {
            nvgMoveTo(vg, path->pts[0], path->pts[1]);

            for (int i = 0; i < path->npts - 1; i += 3)
            {
                float *p = &path->pts[i * 2];
                nvgBezierTo(vg, p[2], p[3], p[4], p[5], p[6], p[7]);
            }

            if (pathHole)
            {
                nvgPathWinding(vg, NVG_HOLE);
            }
            else
            {
                pathHole = true;
            }

            if (path->closed)
            {
                nvgLineTo(vg, path->pts[0], path->pts[1]);
            }
        }

        // set stroke/line
        nvgStrokeWidth(vg, shape->strokeWidth);
        switch (shape->stroke.type)
        {
        case NSVG_PAINT_COLOR:
            {
                nvgStrokeColor(vg, gui_svg_color(shape->stroke.d.color, shape->opacity * alpha));
                nvgStroke(vg);
            }
            break;

        case NSVG_PAINT_LINEAR_GRADIENT:
            {
                nvgStrokePaint(vg, gui_svg_create_linear_gradient(vg, shape->stroke.d.gradient, alpha));
                nvgStroke(vg);
            }
            break;

        case NSVG_PAINT_RADIAL_GRADIENT:
            {
                nvgStrokePaint(vg, gui_svg_create_radial_gradient(vg, shape->stroke.d.gradient, alpha));
                nvgStroke(vg);
            }
            break;

        default:
            break;
        }

        switch (shape->strokeLineCap)
        {
        case NSVG_JOIN_MITER:
            {
                cap = NVG_MITER;
            }
            break;

        case NSVG_JOIN_ROUND:
            {
                cap = NVG_ROUND;
            }
            break;

        case NSVG_JOIN_BEVEL:
            {
                cap = NVG_BEVEL;
            }
            break;

        default: cap = NVG_MITER;
            break;
        }

        nvgLineCap(vg, cap);
        nvgLineJoin(vg, shape->strokeLineJoin);

        // set fill
        switch (shape->fill.type)
        {
        case NSVG_PAINT_COLOR:
            {
                nvgFillColor(vg, gui_svg_color(shape->fill.d.color, shape->opacity * alpha));
                nvgFill(vg);
            }
            break;

        case NSVG_PAINT_LINEAR_GRADIENT:
            {
                nvgFillPaint(vg, gui_svg_create_linear_gradient(vg, shape->fill.d.gradient, alpha));
                nvgFill(vg);
            }
            break;

        case NSVG_PAINT_RADIAL_GRADIENT:
            {
                nvgFillPaint(vg, gui_svg_create_radial_gradient(vg, shape->fill.d.gradient, alpha));
                nvgFill(vg);
            }
            break;

        default:
            break;
        }
    }
}

static void gui_svg_draw_cb(gui_obj_t *obj)
{
    NVGcontext *vg;
    struct NSVGimage *image;
    gui_svg_t *this;
    gui_dispdev_t *dc;

    this = (gui_svg_t *)obj;
    dc = gui_get_dc();
    GUI_UNUSED(this);
    GUI_UNUSED(dc);

    extern NVGcontext *nvgCreateAGGE(uint32_t w, uint32_t h, uint32_t stride, enum NVGtexture format,
                                     uint8_t *data);
    extern void nvgDeleteAGGE(NVGcontext * ctx);

    vg = nvgCreateAGGE(dc->fb_width, dc->fb_height, dc->fb_width * (dc->bit_depth >> 3),
                       (dc->bit_depth >> 3) == 2 ? NVG_TEXTURE_BGR565 : NVG_TEXTURE_BGRA, dc->frame_buf);
    nvgBeginFrame(vg, dc->fb_width, dc->fb_height, 1);
    nvgScissor(vg, 0, 0, dc->fb_width, dc->fb_height);
    nvgResetTransform(vg);
    nvgTransform(vg, obj->matrix->m[0][0], obj->matrix->m[1][0], obj->matrix->m[0][1],
                 obj->matrix->m[1][1], obj->matrix->m[0][2],
                 obj->matrix->m[1][2] - dc->fb_height * dc->section_count);

    nvgTranslate(vg, this->t_x, this->t_y);
    nvgTranslate(vg, this->c_x, this->c_y);
    nvgRotate(vg, nvgDegToRad(this->degrees));
    nvgScale(vg, this->scale_x, this->scale_y);
    nvgTranslate(vg, -(float)this->c_x, -(float)this->c_y);

    if (this->filename != NULL)
    {
        image = nsvgParseFromFile(this->filename, "px", 96.0f);
    }
    else if (this->addr != NULL)
    {
        char *data = NULL;

        data = (char *)gui_malloc(this->size + 1);
        data[this->size] = '\0';
        memcpy(data, this->addr, this->size + 1);
        image = nsvgParse(data, "px", 96.0f);

        gui_free(data);
    }
    else
    {
        goto error;
    }

    gui_svg_nsvg_draw(vg, image, 1.0);
    // Delete
    nsvgDelete(image);

error:
    nvgEndFrame(vg);
    nvgDeleteAGGE(vg);
}

#undef NANOSVG_IMPLEMENTATION

static void gui_svg_end(gui_obj_t *obj)
{

}

static void gui_svg_destory(gui_obj_t *obj)
{

}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
void gui_svg_rotation(gui_svg_t *svg,
                      float      degrees,
                      float      c_x,
                      float      c_y)
{
    svg->degrees = degrees;
    svg->c_x = c_x;
    svg->c_y = c_y;
}

void gui_svg_scale(gui_svg_t *svg, float scale_x, float scale_y)
{
    svg->scale_x = scale_x;
    svg->scale_y = scale_y;
}

void gui_svg_translate(gui_svg_t *svg, float t_x, float t_y)
{
    svg->t_x = t_x;
    svg->t_y = t_y;
}

void gui_svg_set_opacity(gui_svg_t *svg, unsigned char opacity_value)
{

}

static void gui_svg_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            {
                gui_svg_prepare(obj);
            }
            break;

        case OBJ_DRAW:
            {
                gui_svg_draw_cb(obj);
            }
            break;

        case OBJ_END:
            {
                gui_svg_end(obj);
            }
            break;

        case OBJ_DESTORY:
            {
                gui_svg_destory(obj);
            }
            break;

        default:
            break;
        }
    }
}

gui_svg_t *gui_svg_create_from_mem(void       *parent,
                                   const char *name,
                                   uint8_t    *addr,
                                   uint32_t    size,
                                   int16_t     x,
                                   int16_t     y,
                                   int16_t     w,
                                   int16_t     h)
{
    gui_svg_t *this;
    gui_obj_t *base;
    gui_obj_t *root;
    GUI_ASSERT(parent != NULL);

    if (name == NULL)
    {
        name = "svg";
    }

    this = gui_malloc(sizeof(gui_svg_t));
    GUI_ASSERT(this != NULL);
    memset(this, 0x00, sizeof(gui_svg_t));

    //svg_ctor
    //for base class
    base = (gui_obj_t *)this;
    gui_obj_ctor(base, parent, name, x, y, w, h);

    //for root class
    root = (gui_obj_t *)this;
    root->type = VG_LITE_CLOCK;
    root->obj_cb = gui_svg_cb;
    root->has_prepare_cb = true;
    root->has_draw_cb = true;
    root->has_end_cb = true;
    root->has_destroy_cb = true;

    //for self
    this->addr = addr;
    this->size = size;
    this->scale_x = 1.0;
    this->scale_y = 1.0;
    //svg_ctor

    gui_list_init(&(GET_BASE(this)->child_list));
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list),
                               &(GET_BASE(this)->brother_list));
    }

    GET_BASE(this)->create_done = true;

    return this;
}

gui_svg_t *gui_svg_create_from_file(void       *parent,
                                    const char *name,
                                    const char *filename,
                                    int16_t     x,
                                    int16_t     y,
                                    int16_t     w,
                                    int16_t     h)
{
    gui_svg_t *this;
    gui_obj_t *base;
    gui_obj_t *root;
    GUI_ASSERT(parent != NULL);

    if (name == NULL)
    {
        name = "svg";
    }

    this = gui_malloc(sizeof(gui_svg_t));
    GUI_ASSERT(this != NULL);
    memset(this, 0x00, sizeof(gui_svg_t));

    //svg_ctor
    //for base class
    base = (gui_obj_t *)this;
    gui_obj_ctor(base, parent, name, x, y, w, h);

    //for root class
    root = (gui_obj_t *)this;
    root->type = VG_LITE_CLOCK;
    root->obj_cb = gui_svg_cb;
    root->has_prepare_cb = true;
    root->has_draw_cb = true;
    root->has_end_cb = true;
    root->has_destroy_cb = true;

    //for self
    this->filename = filename;
    this->scale_x = 1.0;
    this->scale_y = 1.0;
    //svg_ctor

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
