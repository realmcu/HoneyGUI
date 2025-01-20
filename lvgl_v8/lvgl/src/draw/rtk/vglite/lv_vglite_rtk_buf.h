/**
 * @file lv_vglite_buf.h
 *
 */

#ifndef LV_VGLITE_BUF_H
#define LV_VGLITE_BUF_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../lv_conf_internal.h"

#if LV_USE_GPU_VG_LITE
#include "vg_lite.h"
#include "../../sw/lv_draw_sw.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
/**
 * Init vglite destination buffer. It will be done once per frame.
 *
 * @param[in] buf Destination buffer address (does not require alignment for VG_LITE_LINEAR mode)
 * @param[in] area Destination buffer area (for width and height)
 * @param[in] stride Stride of destination buffer
 */
void lv_gpu_vglite_init_buf(const lv_color_t *buf, const lv_area_t *area, lv_coord_t stride);

/**
 * Get vglite destination buffer pointer.
 *
 * @retval The vglite destination buffer
 */
vg_lite_buffer_t *lv_vglite_get_dest_buf(void);

/**
 * Get vglite source buffer pointer.
 *
 * @retval The vglite source buffer
 */
vg_lite_buffer_t *lv_vglite_get_src_buf(void);

/**
 * Set vglite destination buffer address only.
 *
 * @param[in] buf Destination buffer address (does not require alignment for VG_LITE_LINEAR mode)
 */
void lv_vglite_set_dest_buf_ptr(const lv_color_t *buf);

/**
 * Set vglite source buffer address only.
 *
 * @param[in] buf Source buffer address
 */
void lv_vglite_set_src_buf_ptr(const lv_color_t *buf);

/**
 * Set vglite source buffer. It will be done only if buffer addreess is different.
 *
 * @param[in] buf Source buffer address
 * @param[in] area Source buffer area (for width and height)
 * @param[in] stride Stride of source buffer
 */
void lv_vglite_set_src_buf(const lv_color_t *buf, const lv_area_t *area, lv_coord_t stride);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_GPU_VG_LITE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_VGLITE_BUF_H*/
