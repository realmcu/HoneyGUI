/**
 * @file lv_draw_vglite_blend.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_vglite_blend.h"
#include "trace.h"
#if LV_USE_GPU_VG_LITE
#include "lv_vglite_buf.h"

/*********************
 *      DEFINES
 *********************/

/** Stride in px required by VG-Lite HW*/
#define LV_GPU_VG_LITE_STRIDE_ALIGN_PX 16U

/**
 * Enable BLIT quality degradation workaround for RT595,
 * recommended for screen's dimension > 352 pixels.
 */
#define RT595_BLIT_WRKRND_ENABLED 1

/* Internal compound symbol */
#if (defined(CPU_MIMXRT595SFFOB) || defined(CPU_MIMXRT595SFFOB_cm33) || \
    defined(CPU_MIMXRT595SFFOC) || defined(CPU_MIMXRT595SFFOC_cm33)) && \
    RT595_BLIT_WRKRND_ENABLED
#define VG_LITE_BLIT_SPLIT_ENABLED 1
#else
#define VG_LITE_BLIT_SPLIT_ENABLED 0
#endif

#if VG_LITE_BLIT_SPLIT_ENABLED
/**
* BLIT split threshold - BLITs with width or height higher than this value will be done
* in multiple steps. Value must be 16-aligned. Don't change.
*/
#define LV_GPU_NXP_VG_LITE_BLIT_SPLIT_THR 352
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**
 * Blit single image, with optional opacity.
 *
 * @param[in] dest_area Area with relative coordinates of destination buffer
 * @param[in] src_area Source area with relative coordinates of source buffer
 * @param[in] opa Opacity
 *
 * @retval LV_RES_OK Transfer complete
 * @retval LV_RES_INV Error occurred
 */
static lv_res_t lv_vglite_blit_single(const lv_area_t *dest_area, const lv_area_t *src_area,
                                      lv_opa_t opa);

/**
 * Check source memory and stride alignment.
 *
 * @param[in] src_buf Source buffer
 * @param[in] src_stride Stride of source buffer in pixels
 *
 * @retval LV_RES_OK Alignment OK
 * @retval LV_RES_INV Error occurred
 */
static lv_res_t check_src_alignment(const lv_color_t *src_buf, lv_coord_t src_stride);

/**
 * Creates matrix that translates to origin of given destination area.
 *
 * @param[in] dest_area Area with relative coordinates of destination buffer
 */
static inline void lv_vglite_set_translation_matrix(const lv_area_t *dest_area);

/**
 * Creates matrix that translates to origin of given destination area with transformation (scale or rotate).
 *
 * @param[in] dest_area Area with relative coordinates of destination buffer
 * @param[in] dsc Image descriptor
 */
static inline void lv_vglite_set_transformation_matrix(const lv_area_t *dest_area,
                                                       const lv_draw_img_dsc_t *dsc);

/**********************
 *  STATIC VARIABLES
 **********************/

static vg_lite_matrix_t vgmatrix;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_res_t lv_gpu_vglite_fill(const lv_area_t *dest_area, lv_color_t color, lv_opa_t opa)
{
    vg_lite_error_t err = VG_LITE_SUCCESS;
    lv_color32_t col32 = {.full = lv_color_to32(color)}; /*Convert color to RGBA8888*/
    vg_lite_color_t vgcol; /* vglite takes ABGR */
    vg_lite_buffer_t *vgbuf = lv_vglite_get_dest_buf();

    vg_lite_buffer_format_t color_format = LV_COLOR_DEPTH == 16 ? VG_LITE_BGRA8888 : VG_LITE_ABGR8888;
    if (lv_vglite_premult_and_swizzle(&vgcol, col32, opa, color_format) != LV_RES_OK)
    {
        VG_LITE_RETURN_INV("Premultiplication and swizzle failed.");
    }

    if (opa >= (lv_opa_t)LV_OPA_MAX)    /*Opaque fill*/
    {
        vg_lite_rectangle_t rect =
        {
            .x = dest_area->x1,
            .y = dest_area->y1,
            .width = lv_area_get_width(dest_area),
            .height = lv_area_get_height(dest_area)
        };

        err = vg_lite_clear(vgbuf, &rect, vgcol);
        VG_LITE_ERR_RETURN_INV(err, "Clear failed.");

        if (lv_vglite_run() != LV_RES_OK)
        {
            VG_LITE_RETURN_INV("Run failed.");
        }
    }
    else     /*fill with transparency*/
    {

        vg_lite_path_t path;
        int32_t path_data[] =   /*VG rectangular path*/
        {
            VLC_OP_MOVE, dest_area->x1,  dest_area->y1,
            VLC_OP_LINE, dest_area->x2 + 1,  dest_area->y1,
            VLC_OP_LINE, dest_area->x2 + 1,  dest_area->y2 + 1,
            VLC_OP_LINE, dest_area->x1,  dest_area->y2 + 1,
            VLC_OP_LINE, dest_area->x1,  dest_area->y1,
            VLC_OP_END
        };

        err = vg_lite_init_path(&path, VG_LITE_S32, VG_LITE_LOW, sizeof(path_data), path_data,
                                (vg_lite_float_t) dest_area->x1, (vg_lite_float_t) dest_area->y1,
                                ((vg_lite_float_t) dest_area->x2) + 1.0f, ((vg_lite_float_t) dest_area->y2) + 1.0f);
        VG_LITE_ERR_RETURN_INV(err, "Init path failed.");

        vg_lite_matrix_t matrix;
        vg_lite_identity(&matrix);

        /*Draw rectangle*/
        err = vg_lite_draw(vgbuf, &path, VG_LITE_FILL_EVEN_ODD, &matrix, VG_LITE_BLEND_SRC_OVER, vgcol);
        VG_LITE_ERR_RETURN_INV(err, "Draw rectangle failed.");

        if (lv_vglite_run() != LV_RES_OK)
        {
            VG_LITE_RETURN_INV("Run failed.");
        }

        err = vg_lite_clear_path(&path);
        VG_LITE_ERR_RETURN_INV(err, "Clear path failed.");
    }

    return LV_RES_OK;
}

lv_res_t lv_gpu_vglite_blit(lv_color_t *dest_buf, lv_area_t *dest_area, lv_coord_t dest_stride,
                            const lv_color_t *src_buf, lv_area_t *src_area, lv_coord_t src_stride,
                            lv_opa_t opa)
{
    /* Set vgmatrix. */
    lv_vglite_set_translation_matrix(dest_area);

    /* Set src_vgbuf structure. */
    lv_vglite_set_src_buf(src_buf, src_area, src_stride);

#if VG_LITE_BLIT_SPLIT_ENABLED
    lv_color_t *orig_dest_buf = dest_buf;

    lv_res_t rv = lv_vglite_blit_split(dest_buf, dest_area, dest_stride, src_buf, src_area, src_stride,
                                       opa);

    /* Restore the original dest_vgbuf memory address. */
    lv_vglite_set_dest_buf_ptr(orig_dest_buf);

    return rv;
#else
    LV_UNUSED(dest_buf);
    LV_UNUSED(dest_stride);

    return lv_vglite_blit_single(dest_area, src_area, opa);
#endif
}

lv_res_t lv_gpu_vglite_index(const lv_img_dsc_t *img, lv_area_t *dest_area,
                             const lv_draw_img_dsc_t *dsc)
{
    lv_vglite_set_transformation_matrix(dest_area, dsc);
    vg_lite_error_t err = VG_LITE_SUCCESS;
    vg_lite_buffer_t *dst_vgbuf = lv_vglite_get_dest_buf();
    vg_lite_buffer_t *src_vgbuf = lv_vglite_get_src_buf();

    lv_area_t src_area = {.x1 = 0, .x2 = img->header.w + 16 - img->header.w % 16 - 1,
                          .y1 = 0, .y2 = img->header.h - 1
                         };
    uint8_t *aligned_buf = NULL;
    lv_coord_t src_stride = lv_area_get_width(&src_area);
    lv_coord_t vgbuf_stride = 0;
    vg_lite_buffer_format_t format = VG_LITE_INDEX_1;
    uint32_t src_line_len = img->header.h;
    uint32_t index_tbl_size = 2;
    uint32_t colors[256];
    switch (img->header.cf)
    {
    case LV_IMG_CF_INDEXED_1BIT:
        vgbuf_stride = src_stride / 8;
        src_line_len = img->header.h / 8;
        break;
    case LV_IMG_CF_INDEXED_2BIT:
        format = VG_LITE_INDEX_2;
        vgbuf_stride = src_stride / 4;
        src_line_len = img->header.h / 4;
        index_tbl_size = 4;
        break;
    case LV_IMG_CF_INDEXED_4BIT:
        format = VG_LITE_INDEX_4;
        vgbuf_stride = src_stride / 2;
        src_line_len = img->header.h / 2;
        index_tbl_size = 16;
        break;
    case LV_IMG_CF_INDEXED_8BIT:
        format = VG_LITE_INDEX_8;
        vgbuf_stride = src_stride;
        src_line_len = img->header.h;
        index_tbl_size = 256;
        break;
    default:
        return LV_RES_INV;
    }
    lv_vglite_set_src_buf((lv_color_t *)(img->data + index_tbl_size * sizeof(lv_color_t)), &src_area,
                          src_stride);
    src_vgbuf->format = format;
    src_vgbuf->stride = vgbuf_stride;
    memcpy(colors, img->data, index_tbl_size * 4);
    vg_lite_set_CLUT(index_tbl_size, colors);
    aligned_buf = lv_mem_alloc(img->header.h * vgbuf_stride);
    if (img->header.w % 16 != 0)
    {
        for (int i = 0; i < img->header.h; i++)
        {
            memcpy(aligned_buf + i * vgbuf_stride,
                   img->data + index_tbl_size * sizeof(lv_color_t) + i * src_line_len, src_line_len);
        }
        src_vgbuf->memory = aligned_buf;
        src_vgbuf->address = (uint32_t)src_vgbuf->memory;
    }

    vg_lite_rectangle_t rect =
    {
        0, /* start x */
        0, /* start y */
        img->header.w, /* width */
        img->header.h /* height */
    };
    uint32_t color;
    vg_lite_blend_t blend;
    if (dsc->opa >= (lv_opa_t)LV_OPA_MAX)
    {
        color = 0xFFFFFFFFU;
        blend = VG_LITE_BLEND_SRC_OVER;
        src_vgbuf->transparency_mode = VG_LITE_IMAGE_TRANSPARENT;
    }
    else
    {
        color = (dsc->opa << 24) | (dsc->opa << 16) | (dsc->opa << 8) | dsc->opa;
        blend = VG_LITE_BLEND_SRC_OVER;
        src_vgbuf->image_mode = VG_LITE_MULTIPLY_IMAGE_MODE;
        src_vgbuf->transparency_mode = VG_LITE_IMAGE_TRANSPARENT;
    }
    err = vg_lite_blit_rect(dst_vgbuf, src_vgbuf, &rect, &vgmatrix, blend, color, VG_LITE_FILTER_POINT);
    if (err != VG_LITE_SUCCESS)
    {
        VG_LITE_RETURN_INV("Blit rectangle failed.");
    }

    if (lv_vglite_run() != LV_RES_OK)
    {
        VG_LITE_RETURN_INV("Run failed.");
    }

    if (aligned_buf != NULL)
    {
        lv_mem_free(aligned_buf);
    }
    return LV_RES_OK;
}

lv_res_t lv_gpu_vglite_alpha_only(const lv_img_dsc_t *img, lv_area_t *dest_area,
                                  const lv_draw_img_dsc_t *dsc)
{
    lv_vglite_set_transformation_matrix(dest_area, dsc);
    vg_lite_error_t err = VG_LITE_SUCCESS;
    vg_lite_buffer_t *dst_vgbuf = lv_vglite_get_dest_buf();
    vg_lite_buffer_t *src_vgbuf = lv_vglite_get_src_buf();

    lv_area_t src_area = {.x1 = 0, .x2 = img->header.w + 16 - img->header.w % 16 - 1,
                          .y1 = 0, .y2 = img->header.h - 1
                         };
    uint8_t *aligned_buf = NULL;
    lv_coord_t src_stride = lv_area_get_width(&src_area);
    lv_coord_t vgbuf_stride = 0;
    vg_lite_buffer_format_t format = VG_LITE_A4;
    uint32_t src_line_len = img->header.h;
    switch (img->header.cf)
    {
    case LV_IMG_CF_ALPHA_4BIT:
        format = VG_LITE_A4;
        vgbuf_stride = src_stride / 2;
        src_line_len = img->header.h / 2;
        break;
    case LV_IMG_CF_ALPHA_8BIT:
        format = VG_LITE_A8;
        vgbuf_stride = src_stride;
        src_line_len = img->header.h;
        break;
    default:
        return LV_RES_INV;
    }
    lv_vglite_set_src_buf((lv_color_t *)img->data, &src_area, src_stride);
    src_vgbuf->format = format;
    src_vgbuf->stride = vgbuf_stride;
    if (img->header.w % 16 != 0)
    {
        aligned_buf = lv_mem_alloc(img->header.h * vgbuf_stride);
        for (int i = 0; i < img->header.h; i++)
        {
            memcpy(aligned_buf + i * vgbuf_stride, img->data + i * src_line_len, src_line_len);
        }
        src_vgbuf->memory = aligned_buf;
        src_vgbuf->address = (uint32_t)src_vgbuf->memory;
    }

    src_vgbuf->image_mode = VG_LITE_MULTIPLY_IMAGE_MODE;
    vg_lite_rectangle_t rect =
    {
        0, /* start x */
        0, /* start y */
        img->header.w, /* width */
        img->header.h /* height */
    };
    uint32_t color;
    vg_lite_blend_t blend;
    if (dsc->opa >= (lv_opa_t)LV_OPA_MAX)
    {
        color = 0xFF000000U;
        blend = VG_LITE_BLEND_SRC_OVER;
    }
    else
    {
        color = (dsc->opa << 24) & 0xFF000000;
        blend = VG_LITE_BLEND_SRC_OVER;

    }
    err = vg_lite_blit_rect(dst_vgbuf, src_vgbuf, &rect, &vgmatrix, blend, color, VG_LITE_FILTER_POINT);
    if (err != VG_LITE_SUCCESS)
    {
        VG_LITE_RETURN_INV("Blit rectangle failed.");
    }

    if (lv_vglite_run() != LV_RES_OK)
    {
        VG_LITE_RETURN_INV("Run failed.");
    }

    if (aligned_buf != NULL)
    {
        lv_mem_free(aligned_buf);
    }
    return LV_RES_OK;
}

lv_res_t lv_gpu_vglite_blit_transform(lv_color_t *dest_buf, lv_area_t *dest_area,
                                      lv_coord_t dest_stride,
                                      const lv_color_t *src_buf, lv_area_t *src_area, lv_coord_t src_stride,
                                      const lv_draw_img_dsc_t *dsc)
{
    /* Set vgmatrix. */
    lv_vglite_set_transformation_matrix(dest_area, dsc);

    /* Set src_vgbuf structure. */
    lv_vglite_set_src_buf(src_buf, src_area, src_stride);

    LV_UNUSED(dest_buf);
    LV_UNUSED(dest_stride);

    return lv_vglite_blit_single(dest_area, src_area, dsc->opa);
}


/**********************
 *   STATIC FUNCTIONS
 **********************/
lv_res_t lv_gpu_vglite_mask(const lv_draw_sw_blend_dsc_t *dsc)
{
    vg_lite_error_t err = VG_LITE_SUCCESS;
    vg_lite_buffer_t *dst_vgbuf = lv_vglite_get_dest_buf();
    vg_lite_buffer_t *src_vgbuf = lv_vglite_get_src_buf();

    uint8_t *aligned_buf = NULL;
    lv_area_t aligned_src_area;
    lv_coord_t aligned_src_stride;

    lv_vglite_set_translation_matrix(dsc->mask_area);
    uint32_t image_width = lv_area_get_width(dsc->mask_area);
    uint32_t image_height = lv_area_get_height(dsc->mask_area);
    lv_vglite_set_src_buf((lv_color_t *)dsc->mask_buf, dsc->mask_area, image_height);
    src_vgbuf->stride = image_width;
    src_vgbuf->format = VG_LITE_A8;
    if (image_width % 16)
    {
        aligned_src_stride = image_width + 16 - image_width % 16;
        aligned_buf = lv_mem_alloc(image_height * aligned_src_stride);
        for (int i = 0; i < image_height; i++)
        {
            lv_memcpy(aligned_buf + aligned_src_stride * i, dsc->mask_buf + image_width * i, image_width);
        }
        src_vgbuf->width = aligned_src_stride;
        src_vgbuf->stride = aligned_src_stride;
        src_vgbuf->memory = aligned_buf;
        src_vgbuf->address = (uint32_t)src_vgbuf->memory;
    }

    src_vgbuf->image_mode = VG_LITE_MULTIPLY_IMAGE_MODE;
    vg_lite_rectangle_t rect =
    {
        0, /* start x */
        0, /* start y */
        image_width, /* width */
        image_height /* height */
    };
    uint32_t color;
    vg_lite_blend_t blend;
    if (dsc->opa >= (lv_opa_t)LV_OPA_MAX)
    {
        color = dsc->color.full;
        blend = VG_LITE_BLEND_SRC_OVER;
    }
    else
    {
        lv_color_t m_color = {.full = dsc->color.full};
        m_color.ch.alpha = 0;
        m_color.full = m_color.full * dsc->opa / 0xFF;
        m_color.ch.alpha = dsc->opa;
        color = m_color.full;
        blend = VG_LITE_BLEND_SRC_OVER;

    }
    err = vg_lite_blit_rect(dst_vgbuf, src_vgbuf, &rect, &vgmatrix, blend, color, VG_LITE_FILTER_POINT);
    if (err != VG_LITE_SUCCESS)
    {
        DBG_DIRECT("Blit rectangle failed . %d", err);
        VG_LITE_RETURN_INV("Blit rectangle failed.");
    }

    if (lv_vglite_run() != LV_RES_OK)
    {
        DBG_DIRECT("Run failed.");
        VG_LITE_RETURN_INV("Run failed.");
    }

    if (aligned_buf != NULL)
    {
        lv_mem_free(aligned_buf);
    }
    return LV_RES_OK;
}

static lv_res_t lv_vglite_blit_single(const lv_area_t *dest_area, const lv_area_t *src_area,
                                      lv_opa_t opa)
{
    vg_lite_error_t err = VG_LITE_SUCCESS;
    vg_lite_buffer_t *dst_vgbuf = lv_vglite_get_dest_buf();
    vg_lite_buffer_t *src_vgbuf = lv_vglite_get_src_buf();

    lv_color_t *aligned_buf = NULL;
    lv_area_t aligned_src_area;
    lv_coord_t aligned_src_stride;

    if (check_src_alignment(src_vgbuf->memory, src_vgbuf->stride / sizeof(lv_color_t)) != LV_RES_OK)
    {

        aligned_src_area.x1 = src_area->x1;
        aligned_src_area.x2 = src_area->x2 + 16 - (src_area->x2 + 1) % 16;
        aligned_src_area.y1 = src_area->y1;
        aligned_src_area.y2 = src_area->y2;
        aligned_src_stride = src_vgbuf->stride / sizeof(lv_color_t) + 16 - src_vgbuf->stride / sizeof(
                                 lv_color_t) % 16;
        aligned_buf = lv_mem_alloc((int32_t)lv_area_get_height(&aligned_src_area) * aligned_src_stride *
                                   sizeof(lv_color_t));
        if (aligned_buf == NULL)
        {
            VG_LITE_RETURN_INV("Check src alignment failed.");
        }
        for (int i = 0; i < (int32_t)lv_area_get_height(&aligned_src_area); i++)
        {
            memcpy(aligned_buf + i * aligned_src_stride,
                   (lv_color_t *)src_vgbuf->memory + i * src_vgbuf->stride / sizeof(lv_color_t), src_vgbuf->stride);
        }
        lv_vglite_set_src_buf(aligned_buf, &aligned_src_area, aligned_src_stride);
    }

    vg_lite_rectangle_t rect =
    {
        (uint32_t)src_area->x1, /* start x */
        (uint32_t)src_area->y1, /* start y */
        (uint32_t)lv_area_get_width(src_area), /* width */
        (uint32_t)lv_area_get_height(src_area) /* height */
    };
//    DBG_DIRECT("x1 %d y1 %d", src_area->x1, src_area->y1);
    uint32_t color;
    vg_lite_blend_t blend;
    if (opa >= (lv_opa_t)LV_OPA_MAX)
    {
        color = 0xFFFFFFFFU;
        blend = VG_LITE_BLEND_SRC_OVER;
        src_vgbuf->transparency_mode = VG_LITE_IMAGE_TRANSPARENT;
    }
    else
    {
        color = (opa << 24) | (opa << 16) | (opa << 8) | opa;
        blend = VG_LITE_BLEND_SRC_OVER;
        src_vgbuf->image_mode = VG_LITE_MULTIPLY_IMAGE_MODE;
        src_vgbuf->transparency_mode = VG_LITE_IMAGE_TRANSPARENT;
    }

//    bool scissoring = lv_area_get_width(dest_area) < lv_area_get_width(src_area) ||
//                      lv_area_get_height(dest_area) < lv_area_get_height(src_area);
//    if(scissoring) {
//        DBG_DIRECT("dst %d -> %d, %d -> %d w %d h %d", dest_area->x1, dest_area->x2, dest_area->y1, dest_area->y2, lv_area_get_width(dest_area), lv_area_get_height(dest_area));
//        DBG_DIRECT("src %d -> %d, %d -> %d w %d h %d", src_area->x1, src_area->x2, src_area->y1, src_area->y2, lv_area_get_width(src_area), lv_area_get_height(src_area));
//        vg_lite_enable_scissor();
//        vg_lite_set_scissor((int32_t)dest_area->x1, (int32_t)dest_area->y1,
//                            (int32_t)lv_area_get_width(dest_area),
//                            (int32_t)lv_area_get_height(dest_area));
//    }
    err = vg_lite_blit_rect(dst_vgbuf, src_vgbuf, &rect, &vgmatrix, blend, color, VG_LITE_FILTER_POINT);
    if (err != VG_LITE_SUCCESS)
    {
//        if(scissoring)
//            vg_lite_disable_scissor();
        VG_LITE_RETURN_INV("Blit rectangle failed.");
    }

    if (lv_vglite_run() != LV_RES_OK)
    {
//        if(scissoring)
//            vg_lite_disable_scissor();
        VG_LITE_RETURN_INV("Run failed.");
    }

//    if(scissoring)
//        vg_lite_disable_scissor();

    if (aligned_buf != NULL)
    {
        lv_mem_free(aligned_buf);
    }
    return LV_RES_OK;
}

static lv_res_t check_src_alignment(const lv_color_t *src_buf, lv_coord_t src_stride)
{
    /* No alignment requirement for destination pixel buffer when using mode VG_LITE_LINEAR */

    /* Test for pointer alignment */
    if ((((uintptr_t)src_buf) % (uintptr_t)LV_ATTRIBUTE_MEM_ALIGN_SIZE) != (uintptr_t)0x0U)
        VG_LITE_RETURN_INV("Src buffer ptr (0x%x) not aligned to 0x%x bytes.",
                           (size_t)src_buf, LV_ATTRIBUTE_MEM_ALIGN_SIZE);

    /* Test for stride alignment */
    if ((src_stride % (lv_coord_t)LV_GPU_VG_LITE_STRIDE_ALIGN_PX) != 0x0U)
        VG_LITE_RETURN_INV("Src buffer stride (%d px) not aligned to %d px.",
                           src_stride, LV_GPU_VG_LITE_STRIDE_ALIGN_PX);
    return LV_RES_OK;
}

static inline void lv_vglite_set_translation_matrix(const lv_area_t *dest_area)
{
    vg_lite_identity(&vgmatrix);
    vg_lite_translate((vg_lite_float_t)dest_area->x1, (vg_lite_float_t)dest_area->y1, &vgmatrix);
}

static inline void lv_vglite_set_transformation_matrix(const lv_area_t *dest_area,
                                                       const lv_draw_img_dsc_t *dsc)
{
    lv_vglite_set_translation_matrix(dest_area);

    bool has_scale = (dsc->zoom != LV_IMG_ZOOM_NONE);
    bool has_rotation = (dsc->angle != 0);
    if (has_scale || has_rotation)
    {
        vg_lite_translate(dsc->pivot.x, dsc->pivot.y, &vgmatrix);
        if (has_rotation)
        {
            vg_lite_rotate(dsc->angle / 10.0f, &vgmatrix);    /* angle is 1/10 degree */
        }
        if (has_scale)
        {
            vg_lite_float_t scale = 1.0f * dsc->zoom / LV_IMG_ZOOM_NONE;
            vg_lite_scale(scale, scale, &vgmatrix);
        }
        vg_lite_translate(0.0f - dsc->pivot.x, 0.0f - dsc->pivot.y, &vgmatrix);
    }
}

#endif /*LV_USE_GPU_VG_LITE*/
