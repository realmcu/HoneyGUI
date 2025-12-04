/**
 \internal
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file l3_common.h
  * @brief 3D transform common
  * @details 3D transform common
  * @author sienna_shen@realsil.com.cn
  * @date 2025/7/11
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2025 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
 \endinternal
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef L3_COMMON_H
#define L3_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/

#include <stdbool.h>
#include <stdint.h>
#ifdef CONFIG_SOC_SERIES_RTL87X3G //test code add by wanghao, remove later
#include "trace.h"
#include "platform_utils.h"

#define MEASURE_CPU_CYCLES(...) \
    do { \
        uint32_t _cycle_start_ = read_cpu_counter(); \
        { \
            __VA_ARGS__; \
        } \
        uint32_t _cycle_end_ = read_cpu_counter(); \
        APP_PRINT_INFO2("line = %d, cycles = %u", __LINE__, (_cycle_end_ - _cycle_start_)); \
    } while (0)

#else
#define MEASURE_CPU_CYCLES(...) \
    do { \
        { \
            __VA_ARGS__; \
        } \
    } while (0)
#endif

/*============================================================================*
 *                            Macros
 *============================================================================*/

#define L3_FACESTATE_ACTIVE 1
#define L3_FACESTATE_CLIPPED 2
#define L3_FACESTATE_BACKFACE 4

#define _UI_MIN(x, y)           (((x)<(y))?(x):(y))
#define _UI_MAX(x, y)           (((x)>(y))?(x):(y))

#ifndef M_PI_F
#define M_PI_F    3.1415926f
#endif

/*============================================================================*
 *                               Types
 *============================================================================*/

typedef enum
{
    L3_CULLMODE_NONE,
    L3_CULLMODE_CW,
    L3_CULLMODE_CCW,
} L3_CULLMODE;

typedef enum
{
    L3_FACE_TRANSFORM_LOCAL_TO_LOCAL,
    L3_FACE_TRANSFORM_LOACL_TO_GLOBAL,
    L3_FACE_TRANSFORM_GLOBAL_TO_GLOBAL,
} L3_FACE_TRANSFORM;

typedef enum
{
    LITE_3D_FACE_RECTANGLE,
    LITE_3D_FACE_TRIANGLE,
    LITE_3D_FACE_RECTANGLE_TRIANGLE,
} L3_FACE_TYPE;

typedef enum
{
    L3_FILL_IMAGE_RGB565,
    L3_FILL_IMAGE_ARGB8888,
    L3_FILL_COLOR_RGB565,
    L3_FILL_COLOR_ARGB8888,
} L3_FILL_TYPE;

typedef enum
{
    LITE_RGB565 = 0,
    LITE_ARGB8888 = 4,
} L3_IMAGE_TYPE;

typedef enum
{
    L3_DRAW_FRONT_ONLY,
    L3_DRAW_FRONT_AND_BACK,
    L3_DRAW_FRONT_AND_SORT,
} L3_DRAW_TYPE;


/** @brief
  * @{
  * The color used in the GUI:
  *
  *         bit        bit
  * L3_RGB565  15 R,G,B   0
  * BGR565  15 B,G,R   0
  * L3_RGB888  23 R,G,B   0
  * ARGB888 31 A,R,G,B 0
  * RGBA888 31 R,G,B,A 0
  * ABGR888 31 A,B,G,R 0
  *
  * The gui_color is defined as ARGB888.
  *        bit31 A,R,G,B bit0
  */

#pragma pack(1)
typedef struct _l3_color_rgb888
{
    uint8_t b;
    uint8_t g;
    uint8_t r;
} l3_color_rgb888_t;

typedef struct _l3_color_rgb565
{
    uint16_t b : 5;
    uint16_t g : 6;
    uint16_t r : 5;
} l3_color_rgb565_t;

typedef struct _l3_color_argb8565
{
    uint16_t b : 5;
    uint16_t g : 6;
    uint16_t r : 5;
    uint8_t a;
} l3_color_argb8565_t;

typedef struct _l3_color_argb8888
{
    uint8_t b; ///< Blue color component, at the lowest address
    uint8_t g; ///< Green color component
    uint8_t r; ///< Red color component
    uint8_t a; ///< Alpha channel for transparency, at the highest address
} l3_color_argb8888_t;
#pragma pack()


typedef struct l3_2d_point
{
    float x;
    float y;
} l3_2d_point_t;

typedef struct l3_3d_point
{
    float x;
    float y;
    float z;
} l3_3d_point_t;

typedef struct l3_4d_point
{
    float x;
    float y;
    float z;
    float w;
} l3_4d_point_t;

typedef struct l3_4d_vector
{
    float ux;
    float uy;
    float uz;
    float uw;
} l3_4d_vector_t;

typedef struct l3_vertex
{
    l3_4d_point_t  position;
    l3_4d_vector_t  normal;
    float    u, v;
} l3_vertex_t;

typedef struct l3_line
{
    l3_4d_point_t  point; // start point
    l3_4d_point_t  direct; // end point
} l3_line_t;

typedef struct
{
    int v_idx, vt_idx, vn_idx;
} l3_vertex_index_t;

typedef struct
{
    float x, y, z;
} l3_vertex_coordinate_t;

typedef struct
{
    float u, v;
} l3_texcoord_coordinate_t;

typedef enum
{
    L3_MODEL_TYPE_OBJ,
    L3_MODEL_TYPE_GLTF,
    L3_MODEL_TYPE_UNKNOWN,
} L3_MODEL_TYPE;

typedef struct
{
    uint16_t magic;      // "3D"  = 0x3344
    uint8_t model_type;  // 0: obj, 1: gltf
    uint8_t version;
    uint32_t file_size;
    uint8_t face_type;   // 0: rect, 1: triangle, 2: other
    uint8_t payload_offset;
    uint8_t extension[6];

} l3_desc_file_head_t;



typedef struct l3_rect_face
{
    uint32_t            state;
    l3_vertex_t      transform_vertex[4];
    // l3_4d_vector_t   normal; //face normal
} l3_rect_face_t;

typedef struct
{
    uint32_t            state;
    l3_vertex_t      transform_vertex[3];
    // l3_4d_vector_t   normal; //face normal
} l3_tria_face_t;

typedef union l3_matrix_3x3_union
{
    struct
    {
        float _11, _12, _13;
        float _21, _22, _23;
        float _31, _32, _33;
    } e;
    float m[3][3];
} l3_matrix_3x3_union_t;

typedef struct l3_3x3_matrix
{
    l3_matrix_3x3_union_t u;
} l3_3x3_matrix_t;

typedef union l3_matrix_4x4_union
{
    struct
    {
        float _11, _12, _13, _14;
        float _21, _22, _23, _24;
        float _31, _32, _33, _34;
        float _41, _42, _43, _44;
    } e;
    float m[4][4];
} l3_matrix_4x4_union_t;

typedef struct l3_4x4_matrix
{
    l3_matrix_4x4_union_t u;
} l3_4x4_matrix_t;


typedef l3_4x4_matrix_t l3_world_t;

typedef struct l3_camera
{
    float near_z;
    float far_z;
    float fov;
    float d;

    l3_4d_point_t position;
    l3_4d_point_t targetDirection;

    float viewplane_width, viewplane_height;
    float viewport_width, viewport_height, viewport_center_x, viewport_center_y;
    float aspect_ratio;

    l3_4x4_matrix_t mat_cam, mat_per, mat_scr;

    float *zbuffer;
} l3_camera_t;


typedef struct l3_light
{
    bool initialized;
    float included_angle;
    float blend_ratio;
    l3_4d_point_t position;
    l3_4d_point_t targetDirection;
    l3_color_argb8888_t color;
} l3_light_t;

typedef struct l3_img_head
{
    unsigned char scan : 1;
    unsigned char align : 1;
    unsigned char resize: 2; //0-no resize;1-50%(x&y);2-70%;3-80%
    unsigned char compress: 1;
    unsigned char rsvd : 3;
    char type;
    short w;
    short h;
    char version;
    char rsvd2;
    // void *data;
} l3_img_head_t;


typedef struct l3_draw_rect_img
{
    uint16_t img_w;
    uint16_t img_h;
    int16_t img_target_x; //display start X
    int16_t img_target_y;  //display start Y
    uint16_t img_target_w;
    uint16_t img_target_h;
    void *data;
    l3_3x3_matrix_t matrix; //seems can remve by howie
    l3_3x3_matrix_t inverse;
    uint8_t opacity_value;
    uint32_t checksum : 8;
    uint32_t blend_mode : 5;
    uint32_t high_quality : 1;
    uint32_t fg_color_set;  //A8 image set color
    uint32_t bg_color_fix;  //bg color fix for A8 image
    uint8_t alpha_mix;      //alpha mix for A8 image
    void *acc_user;
} l3_draw_rect_img_t;

typedef struct l3_draw_tria_img
{
    l3_vertex_t p0;
    l3_vertex_t p1;
    l3_vertex_t p2;

    void *fill_data;
    L3_FILL_TYPE fill_type;
} l3_draw_tria_img_t;

typedef struct l3_rect
{
    int16_t x1;
    int16_t y1;
    int16_t x2;
    int16_t y2;
} l3_rect_t;

typedef struct l3_canvas
{
    uint16_t bit_depth;
    /* pixel data */
    uint8_t *frame_buf;

    l3_rect_t section;

} l3_canvas_t;




/*============================================================================*
 *                            Functions
 *============================================================================*/

l3_4d_point_t l3_4d_point(float x, float y, float z);
l3_4d_vector_t l3_4d_vector(float ux, float uy, float uz);
float l3_4d_point_dot(l3_4d_point_t p1, l3_4d_point_t p2);

l3_4d_vector_t l3_4d_vector_unit(l3_4d_vector_t v);
l3_4d_vector_t l3_4d_vector_cross(l3_4d_vector_t v1, l3_4d_vector_t v2);
l3_4d_vector_t l3_4d_point_sub(l3_4d_point_t p1, l3_4d_point_t p2);

bool l3_generate_3x3_matrix(l3_2d_point_t *src, l3_2d_point_t *dst, float *ret);
void l3_3x3_matrix_identity(l3_3x3_matrix_t *m);
void l3_3x3_matrix_translate(l3_3x3_matrix_t *m, float t_x, float t_y);
void l3_3x3_matrix_inverse(l3_3x3_matrix_t *m);
void l3_3x3_matrix_mul_3d_point(l3_3x3_matrix_t *m, l3_3d_point_t *p);

void l3_4x4_matrix_identity(l3_4x4_matrix_t *m);
void l3_4x4_matrix_zero(l3_4x4_matrix_t *m);
void l3_4x4_matrix_translate(l3_4x4_matrix_t *m, float t_x, float t_y, float t_z);
void l3_4x4_matrix_rotateY(l3_4x4_matrix_t *m, float rotY);
void l3_4x4_matrix_rotateX(l3_4x4_matrix_t *m, float rotX);
void l3_4x4_matrix_rotateZ(l3_4x4_matrix_t *m, float rotZ);
void l3_4x4_matrix_scale(l3_4x4_matrix_t *m, float scale_x, float scale_y, float scale_z);
void l3_4x4_matrix_compose_trs(l3_4x4_matrix_t *m, float translation[3], float rotation[4],
                               float scale[3]);

l3_4d_point_t l3_4x4_matrix_mul_4d_point(l3_4x4_matrix_t *mat, l3_4d_point_t p);
bool l3_4x4_matrix_mul(l3_4x4_matrix_t *input_left, l3_4x4_matrix_t *input_right,
                       l3_4x4_matrix_t *output);
bool l3_calulate_draw_img_target_area(l3_draw_rect_img_t *img, l3_rect_t *rect);

void l3_camera_build_UVN_matrix(l3_camera_t *camera);

/**
 * @brief Adjusts the winding order of a triangle's vertices to ensure correct face culling.
 * @param p0 Pointer to the first vertex of the triangle.
 * @param p1 Pointer to the second vertex of the triangle.
 * @param p2 Pointer to the third vertex of the triangle.
 */
void l3_adjust_triangle_winding(l3_vertex_t *p0, l3_vertex_t *p1, l3_vertex_t *p2);

/**
 * @brief Initializes the world matrix with translation, rotation, and scaling.
 * @param world Pointer to the world matrix.
 * @param x Translation along the x-axis.
 * @param y Translation along the y-axis.
 * @param z Translation along the z-axis.
 * @param rotX Rotation around the X-axis in degrees.
 * @param rotY Rotation around the Y-axis in degrees.
 * @param rotZ Rotation around the Z-axis in degrees.
 * @param scale Scaling factor.
 */
void l3_world_initialize(l3_4x4_matrix_t *world, float x, float y, float z, float rotX,
                         float rotY, float rotZ, float scale);

/**
 * @brief Initializes the camera with UVN parameters.
 * @param camera Pointer to the camera structure.
 * @param cameraPosition Position of the camera.
 * @param cameraTarget Target point the camera is looking at.
 * @param near Near clipping plane distance.
 * @param far Far clipping plane distance.
 * @param fov Field of view angle in degrees.
 * @param viewPortWidth Width of the viewport.
 * @param viewPortHeight Height of the viewport.
 * @return True if initialization is successful, false otherwise.
 */
bool l3_camera_UVN_initialize(l3_camera_t *camera, l3_4d_point_t cameraPosition,
                              l3_4d_point_t cameraTarget, float near, float far, float fov, float viewPortWidth,
                              float viewPortHeight);

/**
 * @brief Calculates a 4x4 transformation matrix based on translation, rotation, and scaling.
 * @param matrix Pointer to the matrix to calculate.
 * @param tx Translation along the x-axis.
 * @param ty Translation along the y-axis.
 * @param tz Translation along the z-axis.
 * @param point Reference point for rotation.
 * @param vector Rotation axis vector.
 * @param degrees Rotation angle in degrees.
 * @param scale Scaling factor.
 */
void l3_calculator_4x4_matrix(l3_4x4_matrix_t *matrix, \
                              float tx, float ty, float tz, \
                              l3_4d_point_t point, l3_4d_vector_t vector, float degrees, \
                              float scale);


void *l3_malloc(size_t size);
void l3_free(void *ptr);
int l3_ftl_read(uintptr_t addr, uint8_t *buf, uint32_t len);
uint32_t l3_get_time_ms(void);

#ifdef __cplusplus
}
#endif

#endif
