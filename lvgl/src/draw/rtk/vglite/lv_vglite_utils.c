/**
 * @file lv_vglite_utils.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_vglite_utils.h"

#if LV_USE_GPU_VG_LITE
#include "../../../core/lv_refr.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**
 * Clean and invalidate cache.
 */
static inline void invalidate_cache(void);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_res_t lv_vglite_run(void)
{
    invalidate_cache();

    VG_LITE_ERR_RETURN_INV(vg_lite_flush(), "Flush failed.");

    return LV_RES_OK;
}

lv_res_t lv_vglite_premult_and_swizzle(vg_lite_color_t *vg_col32, lv_color32_t lv_col32,
                                       lv_opa_t opa,
                                       vg_lite_buffer_format_t vg_col_format)
{

    lv_color32_t lv_col32_premul = lv_col32;
    if (opa <= (lv_opa_t)LV_OPA_MAX)
    {
        /* Only pre-multiply color if hardware pre-multiplication is not present */
        lv_col32_premul.ch.red = (uint8_t)(((uint16_t)lv_col32.ch.red * opa) >> 8);
        lv_col32_premul.ch.green = (uint8_t)(((uint16_t)lv_col32.ch.green * opa) >> 8);
        lv_col32_premul.ch.blue = (uint8_t)(((uint16_t)lv_col32.ch.blue * opa) >> 8);
        lv_col32_premul.ch.alpha = opa;
    }

    switch (vg_col_format)
    {
    case VG_LITE_BGRA8888:
        *vg_col32 = lv_col32_premul.full;
        break;
    case VG_LITE_RGBA8888:
        *vg_col32 = ((uint32_t)lv_col32_premul.ch.red << 24) | ((uint32_t)lv_col32_premul.ch.green << 16) |
                    ((uint32_t)lv_col32_premul.ch.blue << 8) | (uint32_t)lv_col32_premul.ch.alpha;
        break;
    case VG_LITE_ABGR8888:
        *vg_col32 = ((uint32_t)lv_col32_premul.ch.alpha << 24) | ((uint32_t)lv_col32_premul.ch.blue << 16) |
                    ((uint32_t)lv_col32_premul.ch.green << 8) | (uint32_t)lv_col32_premul.ch.red;
        break;
    case VG_LITE_ARGB8888:
        *vg_col32 = ((uint32_t)lv_col32_premul.ch.alpha << 24) | ((uint32_t)lv_col32_premul.ch.red << 16) |
                    ((uint32_t)lv_col32_premul.ch.green << 8) | (uint32_t)lv_col32_premul.ch.blue;
        break;
    default:
        return LV_RES_INV;
    }

    return LV_RES_OK;
}

vg_lite_blend_t lv_vglite_get_blend_mode(lv_blend_mode_t lv_blend_mode)
{
    vg_lite_blend_t vg_blend_mode;
    switch (lv_blend_mode)
    {
    case LV_BLEND_MODE_ADDITIVE:
        vg_blend_mode = VG_LITE_BLEND_ADDITIVE;
        break;
    case LV_BLEND_MODE_SUBTRACTIVE:
        vg_blend_mode = VG_LITE_BLEND_SUBTRACT;
        break;
    case LV_BLEND_MODE_MULTIPLY:
        vg_blend_mode = VG_LITE_BLEND_MULTIPLY;
        break;
    case LV_BLEND_MODE_REPLACE:
        vg_blend_mode = VG_LITE_BLEND_NONE;
        break;
    default:
        vg_blend_mode = VG_LITE_BLEND_SRC_OVER;
        break;
    }
    return vg_blend_mode;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static inline void invalidate_cache(void)
{
    lv_disp_t *disp = _lv_refr_get_disp_refreshing();
    if (disp->driver->clean_dcache_cb)
    {
        disp->driver->clean_dcache_cb(disp->driver);
    }
}

#endif /*LV_USE_GPU_VG_LITE*/
