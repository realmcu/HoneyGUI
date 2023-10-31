/**
 * @file lv_vglite_buf.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#if LV_USE_GPU_RTK_VG_LITE
#include "lv_vglite_rtk_buf.h"

/*********************
 *      DEFINES
 *********************/

#if LV_COLOR_DEPTH == 16
#define VG_LITE_PX_FMT VG_LITE_RGB565
#elif LV_COLOR_DEPTH == 32
#define VG_LITE_PX_FMT VG_LITE_BGRA8888
#else
#error Only 16bit and 32bit color depth are supported. Set LV_COLOR_DEPTH to 16 or 32.
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static inline void lv_vglite_set_dest_buf(const lv_color_t *buf, const lv_area_t *area,
                                          lv_coord_t stride);
static inline void lv_vglite_set_buf_ptr(vg_lite_buffer_t *vgbuf, const lv_color_t *buf);
static inline void lv_vglite_set_buf(vg_lite_buffer_t *vgbuf, const lv_color_t *buf,
                                     const lv_area_t *area, lv_coord_t stride);

/**********************
 *  STATIC VARIABLES
 **********************/

static vg_lite_buffer_t dest_vgbuf;
static vg_lite_buffer_t src_vgbuf;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_gpu_vglite_init_buf(const lv_color_t *buf, const lv_area_t *area, lv_coord_t stride)
{
    lv_vglite_set_dest_buf(buf, area, stride);
}

vg_lite_buffer_t *lv_vglite_get_dest_buf(void)
{
    return &dest_vgbuf;
}

vg_lite_buffer_t *lv_vglite_get_src_buf(void)
{
    return &src_vgbuf;
}

void lv_vglite_set_dest_buf_ptr(const lv_color_t *buf)
{
    lv_vglite_set_buf_ptr(&dest_vgbuf, buf);
}

void lv_vglite_set_src_buf_ptr(const lv_color_t *buf)
{
    lv_vglite_set_buf_ptr(&src_vgbuf, buf);
}

void lv_vglite_set_src_buf(const lv_color_t *buf, const lv_area_t *area, lv_coord_t stride)
{
    if (src_vgbuf.memory != (void *)buf)
    {
        lv_vglite_set_buf(&src_vgbuf, buf, area, stride);
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static inline void lv_vglite_set_dest_buf(const lv_color_t *buf, const lv_area_t *area,
                                          lv_coord_t stride)
{
    lv_vglite_set_buf(&dest_vgbuf, buf, area, stride);
}

static inline void lv_vglite_set_buf_ptr(vg_lite_buffer_t *vgbuf, const lv_color_t *buf)
{
    vgbuf->memory = (void *)buf;
    vgbuf->address = (uint32_t)vgbuf->memory;
}

static inline void lv_vglite_set_buf(vg_lite_buffer_t *vgbuf, const lv_color_t *buf,
                                     const lv_area_t *area, lv_coord_t stride)
{
    vgbuf->format = VG_LITE_PX_FMT;
    vgbuf->tiled = VG_LITE_LINEAR;
    vgbuf->image_mode = VG_LITE_NORMAL_IMAGE_MODE;
    vgbuf->transparency_mode = VG_LITE_IMAGE_OPAQUE;

    vgbuf->width = (int32_t)lv_area_get_width(area);
    vgbuf->height = (int32_t)lv_area_get_height(area);
    vgbuf->stride = (int32_t)(stride) * sizeof(lv_color_t);

    lv_memset_00(&vgbuf->yuv, sizeof(vgbuf->yuv));

    vgbuf->memory = (void *)buf;
    vgbuf->address = (uint32_t)vgbuf->memory;
    vgbuf->handle = NULL;
}

#endif /*LV_USE_GPU_RTK_VG_LITE*/
