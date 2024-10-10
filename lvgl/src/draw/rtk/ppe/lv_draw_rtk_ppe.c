/**
 * @file lv_draw_rtk_ppe.c
 *
 */

/*********************
 *      INCLUDES
 *********************/


#if LV_USE_GPU_RTK_PPE
#include <math.h>
#include "lv_draw_rtk_ppe.h"
#include "lv_draw_rtk_ppe_rect.h"
#include "lv_draw_rtk_ppe_blend.h"
#include "trace.h"

//#if LV_COLOR_DEPTH != 32
//#include "../../../core/lv_refr.h"
//#endif

/*********************
 *      DEFINES
 *********************/
#define LV_PPE_SIZE_LIMIT 100

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_draw_ppe_rect(lv_draw_ctx_t *draw_ctx, const lv_draw_rect_dsc_t *dsc,
                             const lv_area_t *coords);
static lv_res_t lv_draw_ppe_bg(lv_draw_ctx_t *draw_ctx, const lv_draw_rect_dsc_t *dsc,
                               const lv_area_t *coords);
static lv_res_t lv_draw_ppe_img(lv_draw_ctx_t *draw_ctx, const lv_draw_img_dsc_t *draw_dsc,
                                const lv_area_t *coords, const void *src);
static void lv_draw_ppe_img_decoded(lv_draw_ctx_t *draw_ctx, const lv_draw_img_dsc_t *dsc,
                                    const lv_area_t *coords, const uint8_t *map_p, lv_img_cf_t cf);
static void lv_draw_ppe_blend(lv_draw_ctx_t *draw_ctx, const lv_draw_sw_blend_dsc_t *dsc);
/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/
#include "lv_img.h"
extern lv_img_dsc_t img_benchmark_cogwheel_rgb;
/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lv_draw_ppe_ctx_init(lv_disp_drv_t *drv, lv_draw_ctx_t *draw_ctx)
{
    lv_draw_sw_ctx_t *draw_sw_ctx = (lv_draw_sw_ctx_t *) draw_ctx;
    lv_draw_sw_init_ctx(drv, draw_ctx);
    draw_ctx->draw_img = lv_draw_ppe_img;
    draw_ctx->draw_rect = lv_draw_ppe_rect;
    draw_ctx->draw_img_decoded = lv_draw_ppe_img_decoded;
    draw_sw_ctx->blend = lv_draw_ppe_blend;
}

void lv_draw_ppe_ctx_deinit(lv_disp_drv_t *drv, lv_draw_ctx_t *draw_ctx)
{
    lv_draw_sw_deinit_ctx(drv, draw_ctx);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void lv_draw_ppe_rect(lv_draw_ctx_t *draw_ctx, const lv_draw_rect_dsc_t *dsc,
                             const lv_area_t *coords)
{
    lv_draw_rect_dsc_t ppe_dsc;

    lv_memcpy(&ppe_dsc, dsc, sizeof(ppe_dsc));
    ppe_dsc.bg_opa = 0;
    ppe_dsc.bg_img_opa = 0;
    ppe_dsc.border_opa = 0;
    ppe_dsc.outline_opa = 0;

    /* Draw the background */
    ppe_dsc.bg_opa = dsc->bg_opa;
    if (lv_draw_ppe_bg(draw_ctx, &ppe_dsc, coords) != LV_RES_OK)
    {
        if (dsc->bg_opa > LV_OPA_MIN)
        {
            lv_draw_sw_rect(draw_ctx, &ppe_dsc, coords);
        }
    }
    ppe_dsc.bg_opa = 0;
    ppe_dsc.bg_img_opa = dsc->bg_img_opa;
    ppe_dsc.border_opa = dsc->border_opa;
    ppe_dsc.outline_opa = dsc->outline_opa;

    lv_draw_sw_rect(draw_ctx, &ppe_dsc, coords);
}

static lv_res_t lv_draw_ppe_bg(lv_draw_ctx_t *draw_ctx, const lv_draw_rect_dsc_t *dsc,
                               const lv_area_t *coords)
{
    if (dsc->bg_opa <= (lv_opa_t)LV_OPA_MIN)
    {
        return LV_RES_OK;
    }
    if (dsc->radius > 0)
    {
        return LV_RES_INV;
    }
    lv_area_t rel_coords;
    lv_area_copy(&rel_coords, coords);

    /*If the border fully covers make the bg area 1px smaller to avoid artifacts on the corners*/
    if (dsc->border_width > 1 && dsc->border_opa >= (lv_opa_t)LV_OPA_MAX && dsc->radius != 0)
    {
        rel_coords.x1 += (dsc->border_side & LV_BORDER_SIDE_LEFT) ? 1 : 0;
        rel_coords.y1 += (dsc->border_side & LV_BORDER_SIDE_TOP) ? 1 : 0;
        rel_coords.x2 -= (dsc->border_side & LV_BORDER_SIDE_RIGHT) ? 1 : 0;
        rel_coords.y2 -= (dsc->border_side & LV_BORDER_SIDE_BOTTOM) ? 1 : 0;
    }

    /* Make coordinates relative to draw buffer */
    lv_area_move(&rel_coords, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);

    lv_area_t rel_clip_area;
    lv_area_copy(&rel_clip_area, draw_ctx->clip_area);
    lv_area_move(&rel_clip_area, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);

    lv_area_t clipped_coords;
    if (!_lv_area_intersect(&clipped_coords, &rel_coords, &rel_clip_area))
    {
        return LV_RES_OK;
    }

    bool mask_any = lv_draw_mask_is_any(&rel_coords);
    lv_grad_dir_t grad_dir = dsc->bg_grad.dir;
    lv_color_t bg_color = (grad_dir == (lv_grad_dir_t)LV_GRAD_DIR_NONE) ?
                          dsc->bg_color : dsc->bg_grad.stops[0].color;
    if (bg_color.full == dsc->bg_grad.stops[1].color.full)
    {
        grad_dir = LV_GRAD_DIR_NONE;
    }

    /*
     * Most simple case: just a plain rectangle (no mask, no radius, no gradient)
     * shall be handled by draw_ctx->blend().
     *
     * Complex case: gradient or radius but no mask.
     */
    if (!mask_any)
    {
        lv_res_t res = lv_ppe_draw_bg(draw_ctx, &rel_coords, &rel_clip_area, dsc);
        if (res != LV_RES_OK)
        {
        }
        return res;
    }
    return LV_RES_INV;
}

static lv_res_t lv_draw_ppe_img(lv_draw_ctx_t *draw_ctx, const lv_draw_img_dsc_t *draw_dsc,
                                const lv_area_t *coords, const void *src)
{
    if (draw_dsc->opa <= (lv_opa_t)LV_OPA_MIN)
    {
        return LV_RES_OK;
    }
    if (draw_dsc->angle != 0)
    {
        return LV_RES_INV;
    }
    lv_area_t blend_area;
    /*Let's get the blend area which is the intersection of the area to draw and the clip area*/
    if (!_lv_area_intersect(&blend_area, coords, draw_ctx->clip_area))
    {
        return LV_RES_OK;
    }

    /*Make the blend area relative to the buffer*/
    lv_area_move(&blend_area, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);

    lv_img_dsc_t *img = (lv_img_dsc_t *)src;
    if (img->header.cf == LV_IMG_CF_ALPHA_8BIT)
    {
        return lv_ppe_alpha_only(img, draw_ctx, draw_dsc, coords);
    }
    return LV_RES_INV;
}

static void lv_draw_ppe_blend(lv_draw_ctx_t *draw_ctx, const lv_draw_sw_blend_dsc_t *dsc)
{
    lv_opa_t *mask;
    if (dsc->mask_buf == NULL) { mask = NULL; }
    if (dsc->mask_buf && dsc->mask_res == LV_DRAW_MASK_RES_TRANSP) { return; }
    else if (dsc->mask_res == LV_DRAW_MASK_RES_FULL_COVER) { mask = NULL; }
    else { mask = dsc->mask_buf; }
    if (dsc->opa <= (lv_opa_t)LV_OPA_MIN)
    {
        return;
    }

    lv_area_t blend_area;
    /*Let's get the blend area which is the intersection of the area to draw and the clip area*/
    if (!_lv_area_intersect(&blend_area, dsc->blend_area, draw_ctx->clip_area))
    {
        return;    /*Fully clipped, nothing to do*/
    }

    /*Make the blend area relative to the buffer*/
    lv_area_move(&blend_area, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);

    bool done = false;
    /*Fill/Blend only non masked, normal blended*/
    if (dsc->mask_buf == NULL && dsc->blend_mode == LV_BLEND_MODE_NORMAL)
    {
        const lv_color_t *src_buf = dsc->src_buf;
        if (src_buf == NULL)
        {
            done = (lv_ppe_fill(&blend_area, draw_ctx, dsc) == LV_RES_OK);
        }
        else
        {
            done = (lv_ppe_map(&blend_area, draw_ctx, dsc) == LV_RES_OK);
        }
    }
    //TODO: should be added in MP version
    else if ((dsc->mask_buf != NULL) && (dsc->src_buf == NULL) &&
#if LV_COLOR_DEPTH == 16
             (dsc->color.full == 0)
#elif LV_COLOR_DEPTH == 32
             (dsc->color.full == 0xFF000000)
#endif
            )
    {
        done = (lv_ppe_mask(draw_ctx, dsc) == LV_RES_OK);
    }

    if (!done)
    {
        lv_draw_sw_blend_basic(draw_ctx, dsc);
    }
}


static void lv_draw_ppe_img_decoded(lv_draw_ctx_t *draw_ctx, const lv_draw_img_dsc_t *dsc,
                                    const lv_area_t *coords, const uint8_t *map_p, lv_img_cf_t cf)
{
    if (dsc->opa <= (lv_opa_t)LV_OPA_MIN)
    {
        return;
    }

    const lv_color_t *src_buf = (const lv_color_t *)map_p;
    if (!src_buf)
    {
        lv_draw_sw_img_decoded(draw_ctx, dsc, coords, map_p, cf);
        return;
    }
    if (dsc->angle != 0)
    {
        lv_draw_sw_img_decoded(draw_ctx, dsc, coords, map_p, cf);
        return;
    }
    lv_area_t blend_area;
    lv_area_copy(&blend_area, draw_ctx->clip_area);
    /*Let's get the blend area which is the intersection of the area to draw and the clip area*/
//    if (!_lv_area_intersect(&blend_area, coords, draw_ctx->buf_area))
//    {
//        return;    /*Fully clipped, nothing to do*/
//    }
    /*Make the blend area relative to the buffer*/

    bool has_mask = lv_draw_mask_is_any(&blend_area);
    bool done = false;
    bool has_recolor = (dsc->recolor_opa != LV_OPA_TRANSP);
    if (!has_mask && has_recolor && cf == LV_IMG_CF_TRUE_COLOR)
    {
        done = (lv_ppe_blit_recolor(draw_ctx, dsc, coords, map_p, cf) == LV_RES_OK);
    }
    else if (!has_mask && !has_recolor &&
             lv_area_get_size(&blend_area) >= LV_PPE_SIZE_LIMIT)
    {
        done = (lv_ppe_blit_transform(draw_ctx, dsc, coords, map_p, cf) == LV_RES_OK);
    }

    if (!done)
    {
        lv_draw_sw_img_decoded(draw_ctx, dsc, coords, map_p, cf);
    }
}

#endif /*LV_USE_GPU_RTK_PPE*/
