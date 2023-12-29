/**
 * @file lv_draw_vglite_rect.c
 *
 */


/*********************
 *      INCLUDES
 *********************/


#if LV_USE_GPU_RTK_PPE
#include "lv_draw_rtk_ppe_rect.h"
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
                        const lv_area_t *clip_area,
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
    target.memory = draw_ctx->buf;
    target.width = lv_area_get_width(draw_ctx->buf_area);
    target.height = lv_area_get_height(draw_ctx->buf_area);
    target.format = sizeof(lv_color_t) == 2 ? PPE_RGB565 : PPE_ARGB8888;
    lv_color32_t bg_color = lv_ppe_toABGR8888(dsc->bg_color);

    if (dsc->bg_opa == 0xFF)
    {
        target.global_alpha_en = DISABLE;
    }
    else
    {
        target.global_alpha_en = ENABLE;
        target.global_alpha = dsc->bg_opa;
    }
    ppe_rect_t rect = {.left = coords->x1, .right = coords->x2, .top = coords->y1, .bottom = coords->y2};
    PPE_ERR err = PPE_Clear_Rect(&target, &rect, bg_color.full);
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

#endif /*LV_USE_GPU_RTK_VG_LITE*/
