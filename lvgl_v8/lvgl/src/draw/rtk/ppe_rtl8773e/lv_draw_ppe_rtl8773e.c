/**
 * @file lv_draw_rtk_ppe_v2.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lvgl.h"
#if LV_USE_GPU_PPE_RTL8773E
#include <math.h>
#include "lv_draw_ppe_rtl8773e.h"
#include "lv_draw_ppe_rtl8773e_rect.h"
#include "lv_draw_ppe_rtl8773e_blend.h"
#include "trace.h"
#include "hal_idu.h"

//#if LV_COLOR_DEPTH != 32
//#include "../../../core/lv_refr.h"
//#endif

/*********************
 *      DEFINES
 *********************/
#define LV_PPE_SIZE_LIMIT 10

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_draw_ppe_rect(lv_draw_ctx_t *draw_ctx, const lv_draw_rect_dsc_t *dsc,
                             const lv_area_t *coords);
static lv_res_t lv_draw_ppe_rect_bg(lv_draw_ctx_t *draw_ctx, const lv_draw_rect_dsc_t *dsc,
                                    const lv_area_t *coords);
static void lv_draw_ppe_bg(lv_draw_ctx_t *draw_ctx, const lv_draw_rect_dsc_t *dsc,
                           const lv_area_t *coords);
static lv_res_t lv_draw_ppe_img(lv_draw_ctx_t *draw_ctx, const lv_draw_img_dsc_t *draw_dsc,
                                const lv_area_t *coords, const void *src);
static void lv_draw_ppe_img_decoded(lv_draw_ctx_t *draw_ctx, const lv_draw_img_dsc_t *dsc,
                                    const lv_area_t *coords, const uint8_t *map_p, lv_img_cf_t cf);
static void lv_draw_ppe_blend(lv_draw_ctx_t *draw_ctx, const lv_draw_sw_blend_dsc_t *dsc);
static void lv_draw_ppe_buffer_copy(lv_draw_ctx_t *draw_ctx,
                                    void *dest_buf, lv_coord_t dest_stride, const lv_area_t *dest_area,
                                    void *src_buf, lv_coord_t src_stride, const lv_area_t *src_area);
/**********************
 *  STATIC VARIABLES
 **********************/
static uint8_t dma_high_speed_channel = 0xA5;
static uint8_t dma_common_channel = 0xA5;
/**********************
 *      MACROS
 **********************/
/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lv_draw_ppe_ctx_init(lv_disp_drv_t *drv, lv_draw_ctx_t *draw_ctx)
{
    hal_dma_channel_init(&dma_high_speed_channel, &dma_common_channel);
    lv_draw_sw_ctx_t *draw_sw_ctx = (lv_draw_sw_ctx_t *) draw_ctx;
    lv_draw_sw_init_ctx(drv, draw_ctx);
    draw_ctx->draw_img = lv_draw_ppe_img;
    draw_ctx->draw_rect = lv_draw_ppe_rect;
    draw_ctx->draw_bg = lv_draw_ppe_bg;
    draw_ctx->draw_img_decoded = lv_draw_ppe_img_decoded;
    draw_ctx->buffer_copy = lv_draw_ppe_buffer_copy;
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
    if (lv_draw_ppe_rect_bg(draw_ctx, &ppe_dsc, coords) != LV_RES_OK)
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

static void lv_draw_ppe_bg(lv_draw_ctx_t *draw_ctx, const lv_draw_rect_dsc_t *dsc,
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
    if (lv_draw_ppe_rect_bg(draw_ctx, &ppe_dsc, coords) != LV_RES_OK)
    {
        if (dsc->bg_opa > LV_OPA_MIN)
        {
            lv_draw_sw_rect(draw_ctx, &ppe_dsc, coords);
        }
    }
    ppe_dsc.bg_opa = 0;
    ppe_dsc.bg_img_opa = dsc->bg_img_opa;

    lv_draw_sw_rect(draw_ctx, &ppe_dsc, coords);
}

static lv_res_t lv_draw_ppe_rect_bg(lv_draw_ctx_t *draw_ctx, const lv_draw_rect_dsc_t *dsc,
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
    lv_grad_dir_t grad_dir = dsc->bg_grad.dir;
    lv_color_t bg_color = (grad_dir == (lv_grad_dir_t)LV_GRAD_DIR_NONE) ?
                          dsc->bg_color : dsc->bg_grad.stops[0].color;

    if (dsc->bg_grad.dir != LV_GRAD_DIR_NONE)
    {
        if (bg_color.full == dsc->bg_grad.stops[1].color.full)
        {
            grad_dir = LV_GRAD_DIR_NONE;
        }
        else
        {
            return LV_RES_INV;
        }
    }
    lv_area_t rel_coords;
    lv_area_copy(&rel_coords, coords);

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

    /*
     * Most simple case: just a plain rectangle (no mask, no radius, no gradient)
     * shall be handled by draw_ctx->blend().
     *
     * Complex case: gradient or radius but no mask.
     */
    if (!mask_any)
    {
        lv_res_t res = lv_ppe_draw_bg(draw_ctx, &clipped_coords, dsc);
        return res;
    }
    return LV_RES_INV;
}

static void lv_draw_ppe_buffer_copy(lv_draw_ctx_t *draw_ctx,
                                    void *dest_buf, lv_coord_t dest_stride, const lv_area_t *dest_area,
                                    void *src_buf, lv_coord_t src_stride, const lv_area_t *src_area)
{
    LV_UNUSED(draw_ctx);
    lv_ppe_buffer_copy(dest_buf, dest_stride, dest_area, src_buf, src_stride, src_area,
                       sizeof(lv_color_t));
}

static lv_res_t lv_draw_ppe_img(lv_draw_ctx_t *draw_ctx, const lv_draw_img_dsc_t *draw_dsc,
                                const lv_area_t *coords, const void *src)
{
    bool done = false;
    if (draw_dsc->opa <= LV_OPA_MIN) { return LV_RES_OK; }

    _lv_img_cache_entry_t *cdsc = _lv_img_cache_open(src, draw_dsc->recolor, draw_dsc->frame_id);

    if (cdsc == NULL) { return LV_RES_INV; }

    lv_img_cf_t cf;
    if (lv_img_cf_is_chroma_keyed(cdsc->dec_dsc.header.cf)) { cf = LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED; }
    else
    {
        switch (cdsc->dec_dsc.header.cf)
        {
        case LV_IMG_CF_ALPHA_8BIT:
            cf = LV_IMG_CF_ALPHA_8BIT;
            break;
        case LV_IMG_CF_TRUE_COLOR:
            cf = LV_IMG_CF_TRUE_COLOR;
            break;
        case LV_IMG_CF_TRUE_COLOR_ALPHA:
            cf = LV_IMG_CF_TRUE_COLOR_ALPHA;
            break;
        case LV_IMG_CF_RGB888:
            cf = LV_IMG_CF_RGB888;
            break;
        case LV_IMG_CF_RGBA8888:
            cf = LV_IMG_CF_RGBA8888;
            break;
        case LV_IMG_CF_RGBX8888:
            cf = LV_IMG_CF_RGBX8888;
            break;
        case LV_IMG_CF_RGB565:
            cf = LV_IMG_CF_RGB565;
            break;
        case LV_IMG_CF_RGBA5658:
            cf = LV_IMG_CF_RGBA5658;
            break;
        default:
#if LV_IMG_CACHE_DEF_SIZE == 0
            lv_img_decoder_close(&cdsc->dec_dsc);
#else
            LV_UNUSED(cache);
#endif
            return LV_RES_INV;
        }
    }

    if (cdsc->dec_dsc.error_msg != NULL)
    {
        LV_LOG_WARN("Image draw error");

        LV_ASSERT(cdsc->dec_dsc.error_msg == NULL);
    }
    /*The decoder could open the image and gave the entire uncompressed image.
     *Just draw it!*/
    else if (cdsc->dec_dsc.img_data)
    {
        lv_area_t map_area_rot;
        lv_area_copy(&map_area_rot, coords);
        ppe_matrix_t matrix;
        ppe_get_identity(&matrix);
        lv_ppe_get_matrix(&matrix, coords, draw_dsc);
        int32_t w = lv_area_get_width(coords);
        int32_t h = lv_area_get_height(coords);
        lv_int_get_transformed_area(&map_area_rot, w, h, draw_dsc->angle, draw_dsc->zoom, &draw_dsc->pivot);
        map_area_rot.x1 += coords->x1;
        map_area_rot.y1 += coords->y1;
        map_area_rot.x2 += coords->x1;
        map_area_rot.y2 += coords->y1;
//        lv_ppe_get_transformed_area(&map_area_rot, w, h, &matrix);
        if (draw_dsc->angle || draw_dsc->zoom != LV_IMG_ZOOM_NONE)
        {
            ppe_matrix_inverse(&matrix);

        }
        else
        {
            matrix.m[0][2] = -matrix.m[0][2];
            matrix.m[1][2] = -matrix.m[1][2];
        }

        lv_area_t clip_com; /*Common area of mask and coords*/
        /*Out of mask. There is nothing to draw so the image is drawn successfully.*/
        if (_lv_area_intersect(&clip_com, draw_ctx->clip_area, &map_area_rot) == false)
        {
#if LV_IMG_CACHE_DEF_SIZE == 0
            lv_img_decoder_close(&cdsc->dec_dsc);
#else
            LV_UNUSED(cache);
#endif
            return LV_RES_OK;
        }

        const lv_area_t *clip_area_ori = draw_ctx->clip_area;
        draw_ctx->clip_area = &clip_com;
        if (draw_dsc->recolor_opa != LV_OPA_TRANSP)
        {
            done = (LV_RES_OK == lv_ppe_blit_recolor(draw_ctx, draw_dsc, coords, cdsc->dec_dsc.img_data, cf));
        }
        else if (draw_dsc->zoom == LV_IMG_ZOOM_NONE && draw_dsc->angle == 0 && cf == LV_IMG_CF_TRUE_COLOR &&
                 draw_dsc->opa >= LV_OPA_MAX)
        {
            done = (LV_RES_OK == lv_ppe_bare_rect_copy(draw_ctx, draw_dsc, coords, cdsc->dec_dsc.img_data));
        }
        else
        {
            done = (LV_RES_OK == lv_ppe_blend_img(draw_ctx, draw_dsc, &matrix, coords, cdsc->dec_dsc.img_data,
                                                  cf));
        }
        draw_ctx->clip_area = clip_area_ori;
    }
    /*The whole uncompressed image is not available. Try to read it line-by-line*/
    else
    {
        done = false;
    }

#if LV_IMG_CACHE_DEF_SIZE == 0
    lv_img_decoder_close(&cdsc->dec_dsc);
#else
    LV_UNUSED(cache);
#endif
    if (done)
    {
        return LV_RES_OK;
    }
    else
    {
        return LV_RES_INV;
    }
}

static void lv_draw_ppe_blend(lv_draw_ctx_t *draw_ctx, const lv_draw_sw_blend_dsc_t *dsc)
{
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
    if (dsc->mask_buf == NULL && dsc->blend_mode == LV_BLEND_MODE_NORMAL &&
        lv_area_get_size(&blend_area) >= LV_PPE_SIZE_LIMIT)
    {
        const lv_color_t *src_buf = dsc->src_buf;
        if (src_buf == NULL)
        {
            done = (lv_ppe_fill(&blend_area, draw_ctx, dsc) == LV_RES_OK);
        }
    }
    else if ((dsc->mask_buf != NULL) && (dsc->src_buf == NULL) &&
#if LV_COLOR_DEPTH == 16
             (dsc->color.full == 0)
#elif LV_COLOR_DEPTH == 32
             (dsc->color.full == 0xFF000000)
#endif
            )
    {
        //done = (lv_ppe_mask(draw_ctx, dsc) == LV_RES_OK);
    }

    if (!done)
    {
        lv_draw_sw_blend_basic(draw_ctx, dsc);
    }
}


static void lv_draw_ppe_img_decoded(lv_draw_ctx_t *draw_ctx, const lv_draw_img_dsc_t *dsc,
                                    const lv_area_t *coords, const uint8_t *map_p, lv_img_cf_t cf)
{

    const lv_color_t *src_buf = (const lv_color_t *)map_p;
    if (!src_buf)
    {
        lv_draw_sw_img_decoded(draw_ctx, dsc, coords, map_p, cf);
        return;
    }
    bool transform = (dsc->angle != 0 || dsc->zoom != LV_IMG_ZOOM_NONE);
    lv_area_t blend_area;
    if (!transform)
    {
        if (!_lv_area_intersect(&blend_area, coords, draw_ctx->buf_area))
        {
            return;    /*Fully clipped, nothing to do*/
        }
    }
    else
    {
        if (!_lv_area_intersect(&blend_area, draw_ctx->clip_area, draw_ctx->buf_area))
        {
            return;    /*Fully clipped, nothing to do*/
        }
    }
    /*Make the blend area relative to the buffer*/
    lv_area_move(&blend_area, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);

    bool has_mask = lv_draw_mask_is_any(&blend_area);
    bool done = false;
    bool has_recolor = (dsc->recolor_opa != LV_OPA_TRANSP);

    if (!has_mask && !has_recolor && !transform && cf == LV_IMG_CF_TRUE_COLOR && dsc->opa >= LV_OPA_MAX)
    {
        done = lv_ppe_bare_rect_copy(draw_ctx, dsc, coords, map_p);
    }
//    if (!has_mask && has_recolor && cf == LV_IMG_CF_TRUE_COLOR)
//    {
//        done = (lv_ppe_blit_recolor(draw_ctx, dsc, coords, map_p, cf) == LV_RES_OK);
//    }
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

#endif /*LV_USE_GPU_PPE_RTL8773E*/
