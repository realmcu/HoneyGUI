/**
 * @file lv_draw_rtk_ppe_rect_v2.c
 *
 */


/*********************
 *      INCLUDES
 *********************/

#include "lvgl.h"
#if LV_USE_GPU_RTK_PPEV2
#include "lv_draw_ppe_rtl8773e_rect.h"
#include "rtl_ppe.h"
#include <math.h>
#include "trace.h"

/*********************
 *      DEFINES
 *********************/
/*********************
 *      DEFINES
 *********************/
/**********************
 *      TYPEDEFS
 **********************/
/**********************
 *  STATIC PROTOTYPES
 **********************/
/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
lv_res_t lv_ppe_draw_bg(lv_draw_ctx_t *draw_ctx, const lv_area_t *coords,
                        const lv_draw_rect_dsc_t *dsc)
{
    if (dsc->radius > 0)
    {
        return LV_RES_INV;
    }
    if (dsc->bg_grad.dir != (lv_grad_dir_t)LV_GRAD_DIR_NONE)
    {
        return LV_RES_INV;
    }
    ppe_buffer_t target;
    memset(&target, 0, sizeof(ppe_buffer_t));
    target.address = (uint32_t)draw_ctx->buf;
    target.width = lv_area_get_width(draw_ctx->buf_area);
    target.height = lv_area_get_height(draw_ctx->buf_area);
    target.format = sizeof(lv_color_t) == 2 ? PPE_RGB565 : PPE_ARGB8888;
    lv_color32_t bg_color = lv_ppe_toABGR8888(dsc->bg_color);

    target.win_x_min = coords->x1;
    target.win_x_max = coords->x2;
    target.win_y_min = coords->y1;
    target.win_y_max = coords->y2;
    target.opacity = dsc->bg_opa;
    target.stride = target.width;

    ppe_rect_t rect = {.x = coords->x1, .w = coords->x2 - coords->x1 + 1, .y = coords->y1, .h = coords->y2 - coords->y1 + 1};
    PPE_err err = PPE_Mask(&target, bg_color.full, &rect);
    if (err == PPE_SUCCESS)
    {
        return LV_RES_OK;
    }
    else
    {
        return LV_RES_INV;
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_GPU_RTK_PPEV2*/
