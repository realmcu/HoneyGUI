/**
 * @file lv_draw_vglite_rect.c
 *
 */


/*********************
 *      INCLUDES
 *********************/


#if LV_USE_GPU_RTK_VG_LITE
#include "lv_draw_rtk_vglite_rect.h"
#include "lv_vglite_rtk_buf.h"
#include <math.h>

/*********************
 *      DEFINES
 *********************/
/*********************
 *      DEFINES
 *********************/

/* Path data sizes for different elements */
#define CUBIC_PATH_DATA_SIZE 7 /* 1 opcode, 6 arguments */
#define LINE_PATH_DATA_SIZE 3  /* 1 opcode, 2 arguments */
#define MOVE_PATH_DATA_SIZE 3  /* 1 opcode, 2 arguments */
#define CLOSE_PATH_DATA_SIZE 1
#define END_PATH_DATA_SIZE 1   /* 1 opcode, 0 arguments */
/* Maximum possible rectangle path size
 * is in the rounded rectangle case:
 * - 1 move for the path start
 * - 4 cubics for the corners
 * - 4 lines for the sides
 * - 1 end for the path end */
#define RECT_PATH_DATA_MAX_SIZE (8 * MOVE_PATH_DATA_SIZE + 8 * CUBIC_PATH_DATA_SIZE + 24 * LINE_PATH_DATA_SIZE + 1 * END_PATH_DATA_SIZE + 8 * CLOSE_PATH_DATA_SIZE)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**
 * Generates path data for rectangle drawing.
 *
 * @param[in/out] path The path data to initialize
 * @param[in/out] path_size The resulting size of the created path data
 * @param[in] dsc The style descriptor for the rectangle to be drawn
 * @param[in] coords The coordinates of the rectangle to be drawn
 */
static void lv_vglite_create_bg_path_data(int32_t *path_data, uint32_t *path_data_size,
                                          lv_coord_t radius,
                                          const lv_area_t *coords);
static void lv_vglite_create_rect_path_data(float *path_data, uint32_t *path_data_size,
                                            int32_t radius, float width,
                                            const lv_area_t *coords, const lv_draw_rect_dsc_t *dsc);
/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_res_t lv_gpu_vglite_draw_bg(const lv_area_t *coords, const lv_area_t *clip_area,
                               const lv_draw_rect_dsc_t *dsc)
{
    vg_lite_error_t err = VG_LITE_SUCCESS;
    lv_coord_t width = lv_area_get_width(coords);
    lv_coord_t height = lv_area_get_height(coords);
    vg_lite_color_t vgcol;
    lv_coord_t radius = dsc->radius;
    vg_lite_buffer_t *vgbuf = lv_vglite_get_dest_buf();

    if (dsc->radius < 0)
    {
        return LV_RES_INV;
    }

    /*** Init path ***/
    int32_t path_data[RECT_PATH_DATA_MAX_SIZE];
    uint32_t path_data_size;
    lv_vglite_create_bg_path_data(path_data, &path_data_size, radius, coords);
    vg_lite_quality_t path_quality = dsc->radius > 0 ? VG_LITE_HIGH : VG_LITE_LOW;

    vg_lite_path_t path;
    err = vg_lite_init_path(&path, VG_LITE_S32, path_quality, path_data_size, path_data,
                            (vg_lite_float_t)clip_area->x1, (vg_lite_float_t)clip_area->y1,
                            ((vg_lite_float_t)clip_area->x2) + 1.0f, ((vg_lite_float_t)clip_area->y2) + 1.0f);
    VG_LITE_ERR_RETURN_INV(err, "Init path failed.");

    vg_lite_matrix_t matrix;
    vg_lite_identity(&matrix);

    vg_lite_matrix_t *grad_matrix;
    vg_lite_linear_gradient_t gradient;

    /*** Init Color/Gradient ***/
    if (dsc->bg_grad.dir != (lv_grad_dir_t)LV_GRAD_DIR_NONE)
    {
        uint32_t colors[2];
        uint32_t stops[2];
        lv_color32_t col32[2];

        /* Gradient setup */
        uint8_t cnt = LV_MAX(dsc->bg_grad.stops_count, 2);
        for (uint8_t i = 0; i < cnt; i++)
        {
            col32[i].full = lv_color_to32(dsc->bg_grad.stops[i].color); /*Convert color to RGBA8888*/
            stops[i] = dsc->bg_grad.stops[i].frac;

            vg_lite_buffer_format_t color_format = LV_COLOR_DEPTH == 16 ? VG_LITE_ABGR8888 : VG_LITE_ARGB8888;
            if (lv_vglite_premult_and_swizzle(&colors[i], col32[i], dsc->bg_opa, color_format) != LV_RES_OK)
            {
                VG_LITE_RETURN_INV("Premultiplication and swizzle failed.");
            }
        }

        lv_memset_00(&gradient, sizeof(vg_lite_linear_gradient_t));

        err = vg_lite_init_grad(&gradient);
        VG_LITE_ERR_RETURN_INV(err, "Init gradient failed");

        err = vg_lite_set_grad(&gradient, cnt, colors, stops);
        VG_LITE_ERR_RETURN_INV(err, "Set gradient failed.");

        err = vg_lite_update_grad(&gradient);
        VG_LITE_ERR_RETURN_INV(err, "Update gradient failed.");

        grad_matrix = vg_lite_get_grad_matrix(&gradient);
        vg_lite_identity(grad_matrix);
        vg_lite_translate((float)coords->x1, (float)coords->y1, grad_matrix);

        if (dsc->bg_grad.dir == (lv_grad_dir_t)LV_GRAD_DIR_VER)
        {
            vg_lite_scale(1.0f, (float)height / 256.0f, grad_matrix);
            vg_lite_rotate(90.0f, grad_matrix);
        }
        else     /*LV_GRAD_DIR_HOR*/
        {
            vg_lite_scale((float)width / 256.0f, 1.0f, grad_matrix);
        }
    }

    lv_color32_t bg_col32 = {.full = lv_color_to32(dsc->bg_color)}; /*Convert color to RGBA8888*/
    vg_lite_buffer_format_t color_format = LV_COLOR_DEPTH == 16 ? VG_LITE_BGRA8888 : VG_LITE_ABGR8888;
    if (lv_vglite_premult_and_swizzle(&vgcol, bg_col32, dsc->bg_opa, color_format) != LV_RES_OK)
    {
        VG_LITE_RETURN_INV("Premultiplication and swizzle failed.");
    }

    /*** Draw rectangle ***/
    if (dsc->bg_grad.dir == (lv_grad_dir_t)LV_GRAD_DIR_NONE)
    {
        err = vg_lite_draw(vgbuf, &path, VG_LITE_FILL_EVEN_ODD, &matrix, VG_LITE_BLEND_SRC_OVER, vgcol);
    }
    else
    {
        err = vg_lite_draw_gradient(vgbuf, &path, VG_LITE_FILL_EVEN_ODD, &matrix, &gradient,
                                    VG_LITE_BLEND_SRC_OVER);
    }
    VG_LITE_ERR_RETURN_INV(err, "Draw gradient failed.");

    if (lv_vglite_run() != LV_RES_OK)
    {
        VG_LITE_RETURN_INV("Run failed.");
    }

    err = vg_lite_clear_path(&path);
    VG_LITE_ERR_RETURN_INV(err, "Clear path failed.");

    if (dsc->bg_grad.dir != (lv_grad_dir_t)LV_GRAD_DIR_NONE)
    {
        err = vg_lite_clear_grad(&gradient);
        VG_LITE_ERR_RETURN_INV(err, "Clear gradient failed.");
    }

    return LV_RES_OK;
}

lv_res_t lv_gpu_vglite_draw_border_generic(const lv_area_t *coords, const lv_area_t *clip_area,
                                           const lv_draw_rect_dsc_t *dsc, bool border)
{
    vg_lite_error_t err = VG_LITE_SUCCESS;
    vg_lite_color_t vgcol; /* vglite takes ABGR */
    lv_coord_t radius = dsc->radius;
    vg_lite_buffer_t *vgbuf = lv_vglite_get_dest_buf();

    if (radius < 0)
    {
        return LV_RES_INV;
    }

    if (border)
    {
        /* Draw border - only has radius if object has radius*/
        lv_coord_t border_half = (lv_coord_t)floor(dsc->border_width / 2.0f);
        if (radius > border_half)
        {
            radius = radius - border_half;
        }
    }
    else
    {
        /* Draw outline - always has radius, leave the same radius in the circle case */
        lv_coord_t outline_half = (lv_coord_t)ceil(dsc->outline_width / 2.0f);
        if (radius < (lv_coord_t)LV_RADIUS_CIRCLE - outline_half)
        {
            radius = radius + outline_half;
        }
    }

    vg_lite_cap_style_t cap_style = (radius) ? VG_LITE_CAP_ROUND : VG_LITE_CAP_BUTT;
    vg_lite_join_style_t join_style = (radius) ? VG_LITE_JOIN_ROUND : VG_LITE_JOIN_MITER;

    /* Choose vglite blend mode based on given lvgl blend mode */
    vg_lite_blend_t vglite_blend_mode = lv_vglite_get_blend_mode(dsc->blend_mode);

    lv_opa_t opa;
    lv_color32_t col32;
    lv_coord_t line_width;

    if (border)
    {
        opa = dsc->border_opa;
        col32.full = lv_color_to32(dsc->border_color); /*Convert color to RGBA8888*/
        line_width = dsc->border_width;
    }
    else
    {
        opa = dsc->outline_opa;
        col32.full = lv_color_to32(dsc->outline_color); /*Convert color to RGBA8888*/
        line_width = dsc->outline_width;
    }

    /*** Init path ***/
    int32_t path_data[RECT_PATH_DATA_MAX_SIZE];
    uint32_t path_data_size;
    vg_lite_path_t path;
    vg_lite_quality_t path_quality = dsc->radius > 0 ? VG_LITE_HIGH : VG_LITE_LOW;
    vg_lite_matrix_t matrix;
    vg_lite_identity(&matrix);

    lv_vglite_create_rect_path_data((float *)path_data, &path_data_size, radius, line_width * 1.0f,
                                    coords, dsc);
    err = vg_lite_init_path(&path, VG_LITE_FP32, path_quality, path_data_size, path_data,
                            (vg_lite_float_t)clip_area->x1, (vg_lite_float_t)clip_area->y1,
                            (vg_lite_float_t)clip_area->x2 + 1.0f, (vg_lite_float_t)clip_area->y2 + 1.0f);

    VG_LITE_ERR_RETURN_INV(err, "Init path failed.");


    vg_lite_buffer_format_t color_format = LV_COLOR_DEPTH == 16 ? VG_LITE_BGRA8888 : VG_LITE_ABGR8888;
    if (lv_vglite_premult_and_swizzle(&vgcol, col32, opa, color_format) != LV_RES_OK)
    {
        VG_LITE_RETURN_INV("Premultiplication and swizzle failed.");
    }
    /*** Draw border ***/
    err = vg_lite_set_draw_path_type(&path, VG_LITE_DRAW_STROKE_PATH);
    VG_LITE_ERR_RETURN_INV(err, "Set draw path type failed.");

    vg_lite_set_draw_path_type(&path, VG_LITE_DRAW_FILL_PATH);

    VG_LITE_ERR_RETURN_INV(err, "Update stroke failed.");
    err = vg_lite_draw(vgbuf, &path, VG_LITE_FILL_EVEN_ODD, &matrix, vglite_blend_mode, vgcol);
    VG_LITE_ERR_RETURN_INV(err, "Draw border failed.");


    if (lv_vglite_run() != LV_RES_OK)
    {
        VG_LITE_RETURN_INV("Run failed.");
    }

    err = vg_lite_clear_path(&path);
    VG_LITE_ERR_RETURN_INV(err, "Clear path failed.");
    return LV_RES_OK;

}

static void lv_vglite_create_bg_path_data(int32_t *path_data, uint32_t *path_data_size,
                                          lv_coord_t radius,
                                          const lv_area_t *coords)
{
    lv_coord_t rect_width = lv_area_get_width(coords);
    lv_coord_t rect_height = lv_area_get_height(coords);

    /* Get the final radius. Can't be larger than the half of the shortest side */
    int32_t shortest_side = LV_MIN(rect_width, rect_height);
    int32_t final_radius = LV_MIN(radius, shortest_side / 2);

    /* Path data element index */
    uint8_t pidx = 0;

    if ((radius == (lv_coord_t)LV_RADIUS_CIRCLE) && (rect_width == rect_height))
    {

        /* Get the control point offset for rounded cases */
        int32_t cpoff = (int32_t)((float)final_radius * BEZIER_OPTIM_CIRCLE);

        /* Circle case */
        /* Starting point */
        path_data[pidx++] = VLC_OP_MOVE;
        path_data[pidx++] = coords->x1 + final_radius;
        path_data[pidx++] = coords->y1;

        /* Top-right arc */
        path_data[pidx++] = VLC_OP_CUBIC_REL;
        path_data[pidx++] = cpoff;
        path_data[pidx++] = 0;
        path_data[pidx++] = final_radius;
        path_data[pidx++] = final_radius - cpoff;
        path_data[pidx++] = final_radius;
        path_data[pidx++] = final_radius;

        /* Bottom-right arc*/
        path_data[pidx++] = VLC_OP_CUBIC_REL;
        path_data[pidx++] = 0;
        path_data[pidx++] = cpoff;
        path_data[pidx++] = cpoff - final_radius;
        path_data[pidx++] = final_radius;
        path_data[pidx++] = 0 - final_radius;
        path_data[pidx++] = final_radius;

        /* Bottom-left arc */
        path_data[pidx++] = VLC_OP_CUBIC_REL;
        path_data[pidx++] = 0 - cpoff;
        path_data[pidx++] = 0;
        path_data[pidx++] = 0 - final_radius;
        path_data[pidx++] = cpoff - final_radius;
        path_data[pidx++] = 0 - final_radius;
        path_data[pidx++] = 0 - final_radius;

        /* Top-left arc*/
        path_data[pidx++] = VLC_OP_CUBIC_REL;
        path_data[pidx++] = 0;
        path_data[pidx++] = 0 - cpoff;
        path_data[pidx++] = final_radius - cpoff;
        path_data[pidx++] = 0 - final_radius;
        path_data[pidx++] = final_radius;
        path_data[pidx++] = 0 - final_radius;

        /* Ending point */
        path_data[pidx++] = VLC_OP_END;
    }
    else if (radius > 0)
    {
        /* Get the control point offset for rounded cases */
        int32_t cpoff = (int32_t)((float)final_radius * BEZIER_OPTIM_CIRCLE);

        /* Rounded rectangle case */
        /* Starting point */
        path_data[pidx++] = VLC_OP_MOVE;
        path_data[pidx++] = coords->x1 + final_radius;
        path_data[pidx++] = coords->y1;

        /* Top side */
        path_data[pidx++] = VLC_OP_LINE;
        path_data[pidx++] = coords->x2 - final_radius + 1;  // Extended for VGLite
        path_data[pidx++] = coords->y1;

        /* Top-right corner */
        path_data[pidx++] = VLC_OP_CUBIC_REL;
        path_data[pidx++] = cpoff;
        path_data[pidx++] = 0;
        path_data[pidx++] = final_radius;
        path_data[pidx++] = final_radius - cpoff;
        path_data[pidx++] = final_radius;
        path_data[pidx++] = final_radius;

        /* Right side */
        path_data[pidx++] = VLC_OP_LINE;
        path_data[pidx++] = coords->x2 + 1;                 // Extended for VGLite
        path_data[pidx++] = coords->y2 - final_radius + 1;  // Extended for VGLite

        /* Bottom-right corner*/
        path_data[pidx++] = VLC_OP_CUBIC_REL;
        path_data[pidx++] = 0;
        path_data[pidx++] = cpoff;
        path_data[pidx++] = cpoff - final_radius;
        path_data[pidx++] = final_radius;
        path_data[pidx++] = 0 - final_radius;
        path_data[pidx++] = final_radius;

        /* Bottom side */
        path_data[pidx++] = VLC_OP_LINE;
        path_data[pidx++] = coords->x1 + final_radius;
        path_data[pidx++] = coords->y2 + 1;                 // Extended for VGLite

        /* Bottom-left corner */
        path_data[pidx++] = VLC_OP_CUBIC_REL;
        path_data[pidx++] = 0 - cpoff;
        path_data[pidx++] = 0;
        path_data[pidx++] = 0 - final_radius;
        path_data[pidx++] = cpoff - final_radius;
        path_data[pidx++] = 0 - final_radius;
        path_data[pidx++] = 0 - final_radius;

        /* Left side*/
        path_data[pidx++] = VLC_OP_LINE;
        path_data[pidx++] = coords->x1;
        path_data[pidx++] = coords->y1 + final_radius;

        /* Top-left corner */
        path_data[pidx++] = VLC_OP_CUBIC_REL;
        path_data[pidx++] = 0;
        path_data[pidx++] = 0 - cpoff;
        path_data[pidx++] = final_radius - cpoff;
        path_data[pidx++] = 0 - final_radius;
        path_data[pidx++] = final_radius;
        path_data[pidx++] = 0 - final_radius;

        /* Ending point */
        path_data[pidx++] = VLC_OP_END;
    }
    else
    {
        /* Non-rounded rectangle case */
        /* Starting point */
        path_data[pidx++] = VLC_OP_MOVE;
        path_data[pidx++] = coords->x1;
        path_data[pidx++] = coords->y1;

        /* Top side */
        path_data[pidx++] = VLC_OP_LINE;
        path_data[pidx++] = coords->x2 + 1; // Extended for VGLite
        path_data[pidx++] = coords->y1;

        /* Right side */
        path_data[pidx++] = VLC_OP_LINE;
        path_data[pidx++] = coords->x2 + 1; // Extended for VGLite
        path_data[pidx++] = coords->y2 + 1; // Extended for VGLite

        /* Bottom side */
        path_data[pidx++] = VLC_OP_LINE;
        path_data[pidx++] = coords->x1;
        path_data[pidx++] = coords->y2 + 1; // Extended for VGLite

        /* Left side*/
        path_data[pidx++] = VLC_OP_LINE;
        path_data[pidx++] = coords->x1;
        path_data[pidx++] = coords->y1;

        /* Ending point */
        path_data[pidx++] = VLC_OP_END;
    }

    /* Resulting path size */
    *path_data_size = pidx * sizeof(int32_t);
}

static void lv_vglite_create_rect_path_data(float *path_data, uint32_t *path_data_size,
                                            int32_t radius, float width,
                                            const lv_area_t *coords, const lv_draw_rect_dsc_t *dsc)
{
    lv_coord_t rect_width = lv_area_get_width(coords);
    lv_coord_t rect_height = lv_area_get_height(coords);
    float w = rect_width * 1.0f;
    float h = rect_height * 1.0f;
    /* Get the final radius. Can't be larger than the half of the shortest side */
    float shortest_side = LV_MIN(w, h);
    float final_radius = LV_MIN(radius * 1.0f, shortest_side / 2);
    char *cpath;
    /* Path data element index */
    uint8_t pidx = 0;
    float x1 = coords->x1;
    float x2 = coords->x2;
    float y1 = coords->y1;
    float y2 = coords->y2;
    if ((radius == (lv_coord_t)LV_RADIUS_CIRCLE) && (w == h))
    {

        /* Get the control point offset for rounded cases */
        float cpoff = (float)final_radius * BEZIER_OPTIM_CIRCLE;

        /* Circle case */
        /* Starting point */
        cpath = (char *)(path_data + pidx);
        *cpath = VLC_OP_MOVE;
        pidx++;
        path_data[pidx++] = x1 + final_radius;
        path_data[pidx++] = y1;

        /* Top-right arc */
        cpath = (char *)(path_data + pidx);
        *cpath = VLC_OP_CUBIC_REL;
        pidx++;
        path_data[pidx++] = cpoff;
        path_data[pidx++] = 0;
        path_data[pidx++] = final_radius;
        path_data[pidx++] = final_radius - cpoff;
        path_data[pidx++] = final_radius;
        path_data[pidx++] = final_radius;

        /* Bottom-right arc*/
        cpath = (char *)(path_data + pidx);
        *cpath = VLC_OP_CUBIC_REL;
        pidx++;
        path_data[pidx++] = 0;
        path_data[pidx++] = cpoff;
        path_data[pidx++] = cpoff - final_radius;
        path_data[pidx++] = final_radius;
        path_data[pidx++] = 0 - final_radius;
        path_data[pidx++] = final_radius;

        /* Bottom-left arc */
        cpath = (char *)(path_data + pidx);
        *cpath = VLC_OP_CUBIC_REL;
        pidx++;
        path_data[pidx++] = 0 - cpoff;
        path_data[pidx++] = 0;
        path_data[pidx++] = 0 - final_radius;
        path_data[pidx++] = cpoff - final_radius;
        path_data[pidx++] = 0 - final_radius;
        path_data[pidx++] = 0 - final_radius;

        /* Top-left arc*/
        cpath = (char *)(path_data + pidx);
        *cpath = VLC_OP_CUBIC_REL;
        pidx++;
        path_data[pidx++] = 0;
        path_data[pidx++] = 0 - cpoff;
        path_data[pidx++] = final_radius - cpoff;
        path_data[pidx++] = 0 - final_radius;
        path_data[pidx++] = final_radius;
        path_data[pidx++] = 0 - final_radius;

        /* Ending point */
        cpath = (char *)(path_data + pidx);
        *cpath = VLC_OP_END;
        pidx++;
    }
    else if (radius > 0)
    {
        /* Get the control point offset for rounded cases */
        float cpoff = (float)final_radius * BEZIER_OPTIM_CIRCLE;
        float final_radius_out = final_radius + width;
        float cpoff_out = (float)final_radius_out * BEZIER_OPTIM_CIRCLE;
        /* Rounded rectangle case */
        /* Starting point */
        /* Top side */
        if ((dsc->border_side == LV_BORDER_SIDE_FULL) || (dsc->border_side & LV_BORDER_SIDE_TOP))
        {
            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_MOVE;
            pidx++;
            path_data[pidx++] = x1 + final_radius;
            path_data[pidx++] = y1;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x2 - final_radius + 1; // Extended for VGLite
            path_data[pidx++] = y1;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x2 - final_radius + 1; // Extended for VGLite
            path_data[pidx++] = y1 - width; // Extended for VGLite

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x1 + final_radius; // Extended for VGLite
            path_data[pidx++] = y1 - width; // Extended for VGLite

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x1 + final_radius; // Extended for VGLite
            path_data[pidx++] = y1; // Extended for VGLite

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_CLOSE;
            pidx++;
        }

        /* Right side */
        if ((dsc->border_side == LV_BORDER_SIDE_FULL) || (dsc->border_side & LV_BORDER_SIDE_RIGHT))
        {
            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_MOVE;
            pidx++;
            path_data[pidx++] = x2 + 1;
            path_data[pidx++] = y1 + final_radius;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x2 + 1; // Extended for VGLite
            path_data[pidx++] = y2 - final_radius + 1; // Extended for VGLite

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x2 + 1 + width; // Extended for VGLite
            path_data[pidx++] = y2 - final_radius + 1; // Extended for VGLite

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x2 + 1 + width; // Extended for VGLite
            path_data[pidx++] = y1 + final_radius; // Extended for VGLite

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x2 + 1; // Extended for VGLite
            path_data[pidx++] = y1 + final_radius; // Extended for VGLite

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_CLOSE;
            pidx++;
        }

        /* Bottom side */
        if ((dsc->border_side == LV_BORDER_SIDE_FULL) || (dsc->border_side & LV_BORDER_SIDE_BOTTOM))
        {
            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_MOVE;
            pidx++;
            path_data[pidx++] = x2 + 1 - final_radius;
            path_data[pidx++] = y2 + 1;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x1 + final_radius;
            path_data[pidx++] = y2 + 1; // Extended for VGLite

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x1 + final_radius;
            path_data[pidx++] = y2 + 1 + width; // Extended for VGLite

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x2 + 1 - final_radius;
            path_data[pidx++] = y2 + 1 + width; // Extended for VGLite

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x2 + 1 - final_radius;
            path_data[pidx++] = y2 + 1; // Extended for VGLite

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_CLOSE;
            pidx++;
        }

        /* Left side*/
        if ((dsc->border_side == LV_BORDER_SIDE_FULL) || (dsc->border_side & LV_BORDER_SIDE_LEFT))
        {
            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_MOVE;
            pidx++;
            path_data[pidx++] = x1;
            path_data[pidx++] = y1 + final_radius;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x1 - width;
            path_data[pidx++] = y1 + final_radius;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x1 - width;
            path_data[pidx++] = y2 + 1 - final_radius;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x1;
            path_data[pidx++] = y2 + 1 - final_radius;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x1;
            path_data[pidx++] = y1 + final_radius;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_CLOSE;
            pidx++;
        }

        /* Top-left corner*/
        if ((dsc->border_side == LV_BORDER_SIDE_FULL) || (dsc->border_side & LV_BORDER_SIDE_TOP) &&
            (dsc->border_side & LV_BORDER_SIDE_LEFT))
        {
            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_MOVE;
            pidx++;
            path_data[pidx++] = x1 + final_radius;
            path_data[pidx++] = y1;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x1 + final_radius;
            path_data[pidx++] = y1 - width;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_CUBIC_REL;
            pidx++;
            path_data[pidx++] = -cpoff_out;
            path_data[pidx++] = 0;
            path_data[pidx++] = -final_radius_out;
            path_data[pidx++] = final_radius_out - cpoff_out;
            path_data[pidx++] = -final_radius_out;
            path_data[pidx++] = final_radius_out;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x1;
            path_data[pidx++] = y1 + final_radius;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_CUBIC_REL;
            pidx++;
            path_data[pidx++] = 0;
            path_data[pidx++] = -cpoff_out;
            path_data[pidx++] = final_radius - cpoff;
            path_data[pidx++] = -final_radius;
            path_data[pidx++] = final_radius;
            path_data[pidx++] = -final_radius;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_CLOSE;
            pidx++;
        }


        /* Top-right corner*/
        if ((dsc->border_side == LV_BORDER_SIDE_FULL) || (dsc->border_side & LV_BORDER_SIDE_TOP) &&
            (dsc->border_side & LV_BORDER_SIDE_RIGHT))
        {
            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_MOVE;
            pidx++;
            path_data[pidx++] = x2 - final_radius + 1;
            path_data[pidx++] = y1;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_CUBIC_REL;
            pidx++;
            path_data[pidx++] = cpoff;
            path_data[pidx++] = 0;
            path_data[pidx++] = final_radius;
            path_data[pidx++] = final_radius - cpoff;
            path_data[pidx++] = final_radius;
            path_data[pidx++] = final_radius;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x2 + 1 + width;
            path_data[pidx++] = y1 + final_radius;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_CUBIC_REL;
            pidx++;
            path_data[pidx++] = 0;
            path_data[pidx++] = -cpoff_out;
            path_data[pidx++] = cpoff_out - (final_radius_out);
            path_data[pidx++] = -final_radius_out;
            path_data[pidx++] = -final_radius_out;
            path_data[pidx++] = -final_radius_out;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x2 - final_radius + 1;
            path_data[pidx++] = y1;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_CLOSE;
            pidx++;
        }

        /* Bottom-right corner*/
        if ((dsc->border_side == LV_BORDER_SIDE_FULL) || (dsc->border_side & LV_BORDER_SIDE_RIGHT) &&
            (dsc->border_side & LV_BORDER_SIDE_BOTTOM))
        {
            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_MOVE;
            pidx++;
            path_data[pidx++] = x2 + 1;
            path_data[pidx++] = y2 - final_radius + 1;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_CUBIC_REL;
            pidx++;
            path_data[pidx++] = 0;
            path_data[pidx++] = cpoff;
            path_data[pidx++] = cpoff - final_radius;
            path_data[pidx++] = final_radius;
            path_data[pidx++] = -final_radius;
            path_data[pidx++] = final_radius;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x2 + 1 - final_radius;
            path_data[pidx++] = y2 + 1 + width;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_CUBIC_REL;
            pidx++;
            path_data[pidx++] = cpoff_out;
            path_data[pidx++] = 0;
            path_data[pidx++] = final_radius_out;
            path_data[pidx++] = cpoff_out - final_radius_out;
            path_data[pidx++] = final_radius_out;
            path_data[pidx++] = -final_radius_out;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x2 + 1;
            path_data[pidx++] = y2 - final_radius + 1;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_CLOSE;
            pidx++;
        }

        /* Bottom-left corner*/
        if ((dsc->border_side == LV_BORDER_SIDE_FULL) || (dsc->border_side & LV_BORDER_SIDE_BOTTOM) &&
            (dsc->border_side & LV_BORDER_SIDE_LEFT))
        {
            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_MOVE;
            pidx++;
            path_data[pidx++] = x1 + final_radius;
            path_data[pidx++] = y2 + 1; // Extended for VGLite

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_CUBIC_REL;
            pidx++;
            path_data[pidx++] = -cpoff;
            path_data[pidx++] = 0;
            path_data[pidx++] = - final_radius;
            path_data[pidx++] = cpoff - final_radius;
            path_data[pidx++] = -final_radius;
            path_data[pidx++] = -final_radius;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x1 - width;
            path_data[pidx++] = y2 + 1 - final_radius;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_CUBIC_REL;
            pidx++;
            path_data[pidx++] = 0;
            path_data[pidx++] = cpoff_out;
            path_data[pidx++] = final_radius_out - cpoff_out;
            path_data[pidx++] = final_radius_out;
            path_data[pidx++] = final_radius_out;
            path_data[pidx++] = final_radius_out;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x1 + final_radius;
            path_data[pidx++] = y2 + 1; // Extended for VGLite

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_CLOSE;
            pidx++;
        }

        cpath = (char *)(path_data + pidx);
        *cpath = VLC_OP_END;
        pidx++;
    }
    else
    {
        /* Non-rounded rectangle case */
        /* Starting point */
        /* Top side */
        if ((dsc->border_side == LV_BORDER_SIDE_FULL) || (dsc->border_side & LV_BORDER_SIDE_TOP))
        {
            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_MOVE;
            pidx++;
            path_data[pidx++] = x1;
            path_data[pidx++] = y1;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x2 + 1; // Extended for VGLite
            path_data[pidx++] = y1;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x2 + 1; // Extended for VGLite
            path_data[pidx++] = y1 + 1 - width; // Extended for VGLite

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x1; // Extended for VGLite
            path_data[pidx++] = y1 + 1 - width; // Extended for VGLite

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x1; // Extended for VGLite
            path_data[pidx++] = y1 + 1; // Extended for VGLite

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_CLOSE;
            pidx++;
        }

        /* Right side */
        if ((dsc->border_side == LV_BORDER_SIDE_FULL) || (dsc->border_side & LV_BORDER_SIDE_RIGHT))
        {
            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_MOVE;
            pidx++;
            path_data[pidx++] = x2 + 1;
            path_data[pidx++] = y1;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x2 + 1; // Extended for VGLite
            path_data[pidx++] = y2 + 1; // Extended for VGLite

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x2 + 1 + width; // Extended for VGLite
            path_data[pidx++] = y2 + 1; // Extended for VGLite

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x2 + 1 + width; // Extended for VGLite
            path_data[pidx++] = y1; // Extended for VGLite

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x2 + 1; // Extended for VGLite
            path_data[pidx++] = y1; // Extended for VGLite

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_CLOSE;
            pidx++;
        }

        /* Bottom side */
        if ((dsc->border_side == LV_BORDER_SIDE_FULL) || (dsc->border_side & LV_BORDER_SIDE_BOTTOM))
        {
            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_MOVE;
            pidx++;
            path_data[pidx++] = x2 + 1;
            path_data[pidx++] = y2 + 1;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x1;
            path_data[pidx++] = y2 + 1; // Extended for VGLite

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x1;
            path_data[pidx++] = y2 + 1 + width; // Extended for VGLite

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x2 + 1;
            path_data[pidx++] = y2 + 1 + width; // Extended for VGLite

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x2 + 1;
            path_data[pidx++] = y2 + 1; // Extended for VGLite

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_CLOSE;
            pidx++;
        }

        /* Left side*/
        if ((dsc->border_side == LV_BORDER_SIDE_FULL) || (dsc->border_side & LV_BORDER_SIDE_LEFT))
        {
            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_MOVE;
            pidx++;
            path_data[pidx++] = x1;
            path_data[pidx++] = y1;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x1 - width;
            path_data[pidx++] = y1;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x1 - width;
            path_data[pidx++] = y2 + 1;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x1;
            path_data[pidx++] = y2 + 1;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x1;
            path_data[pidx++] = y1;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_CLOSE;
            pidx++;
        }

        /* Top-left corner*/
        if ((dsc->border_side == LV_BORDER_SIDE_FULL) || (dsc->border_side & LV_BORDER_SIDE_TOP) &&
            (dsc->border_side & LV_BORDER_SIDE_LEFT))
        {
            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_MOVE;
            pidx++;
            path_data[pidx++] = x1;
            path_data[pidx++] = y1;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x1;
            path_data[pidx++] = y1 + 1 - width;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x1 - width;
            path_data[pidx++] = y1 + 1 - width;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x1 - width;
            path_data[pidx++] = y1;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x1;
            path_data[pidx++] = y1;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_CLOSE;
            pidx++;
        }


        /* Top-right corner*/
        if ((dsc->border_side == LV_BORDER_SIDE_FULL) || (dsc->border_side & LV_BORDER_SIDE_TOP) &&
            (dsc->border_side & LV_BORDER_SIDE_RIGHT))
        {
            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_MOVE;
            pidx++;
            path_data[pidx++] = x2 + 1;
            path_data[pidx++] = y1;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x2 + 1 + width;
            path_data[pidx++] = y1;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x2 + 1 + width;
            path_data[pidx++] = y1 + 1 - width;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x2 + 1;
            path_data[pidx++] = y1 + 1 - width;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x2 + 1;
            path_data[pidx++] = y1;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_CLOSE;
            pidx++;
        }

        /* Bottom-right corner*/
        if ((dsc->border_side == LV_BORDER_SIDE_FULL) || (dsc->border_side & LV_BORDER_SIDE_RIGHT) &&
            (dsc->border_side & LV_BORDER_SIDE_BOTTOM))
        {
            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_MOVE;
            pidx++;
            path_data[pidx++] = x2 + 1;
            path_data[pidx++] = y2 + 1;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x2 + 1;
            path_data[pidx++] = y2 + 1 + width;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x2 + 1 + width;
            path_data[pidx++] = y2 + 1 + width;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x2 + 1 + width;
            path_data[pidx++] = y2 + 1;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x2 + 1;
            path_data[pidx++] = y2 + 1;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_CLOSE;
            pidx++;
        }

        /* Bottom-left corner*/
        if ((dsc->border_side == LV_BORDER_SIDE_FULL) || (dsc->border_side & LV_BORDER_SIDE_BOTTOM) &&
            (dsc->border_side & LV_BORDER_SIDE_LEFT))
        {
            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_MOVE;
            pidx++;
            path_data[pidx++] = x1;
            path_data[pidx++] = y2 + 1;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x1 - width;
            path_data[pidx++] = y2 + 1;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x1 - width;
            path_data[pidx++] = y2 + 1 + width;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x1;
            path_data[pidx++] = y2 + 1 + width;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_LINE;
            pidx++;
            path_data[pidx++] = x1;
            path_data[pidx++] = y2 + 1;

            cpath = (char *)(path_data + pidx);
            *cpath = VLC_OP_CLOSE;
            pidx++;
        }

        cpath = (char *)(path_data + pidx);
        *cpath = VLC_OP_END;
        pidx++;
    }

    /* Resulting path size */
    *path_data_size = pidx * sizeof(float);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_GPU_RTK_VG_LITE*/
