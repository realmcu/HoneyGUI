/*
 * File      : draw_path.h
 */
#ifndef __DRAW_PATH_H__
#define __DRAW_PATH_H__

#include <guidef.h>
#include <gui_api.h>


#ifdef __cplusplus
extern "C" {
#endif

typedef enum path_blend
{
    PATH_BLEND_NONE,     /*! S, i.e. no blending. */
    PATH_BLEND_SRC_OVER, /*! S + (1 - Sa) * D */
    PATH_BLEND_DST_OVER, /*! (1 - Da) * S + D */
    PATH_BLEND_SRC_IN,   /*! Da * S */
    PATH_BLEND_DST_IN,   /*! Sa * D */
    PATH_BLEND_SCREEN,   /*! S + D - S * D */
    PATH_BLEND_MULTIPLY, /*! S * (1 - Da) + D * (1 - Sa) + S * D */
    PATH_BLEND_ADDITIVE, /*! S + D */
    PATH_BLEND_SUBTRACT, /*! D * (1 - S) */
} path_blend_t;

typedef enum path_fill
{
    FILL_NON_ZERO,  /*! Non-zero fill rule. A pixel is drawn if it crosses at least one path pixel. */
    FILL_EVEN_ODD,  /*! Even-odd fill rule. A pixel is drawn it it crosses an odd number of path pixels. */
} path_fill_t;

typedef struct path_linear_gradient
{
    uint32_t colors[16];      /*! Colors for stops. */
    uint32_t count;                     /*! Count of colors, up to 16. */
    uint32_t stops[16];       /*! Color stops, value from 0 to 255. */
} path_linear_gradient_t;



typedef enum path_quality
{
    PATH_HIGH,   /*! High quality 16x anti-aliasing path. */
    PATH_UPPER,  /*! Upper quality 8x anti-aliasing path. */
    PATH_MEDIUM, /*! Medium quality 4x anti-aliasing path. */
    PATH_LOW,    /*! Low quality pat without any anti-aliasing. */
} path_quality_t;

typedef struct hw_memory
{
    void     *handle;               /*! gpu memory object handle. */
    void     *memory;               /*! logical memory address. */
    uint32_t  address;              /*! GPU memory address. */
    uint32_t  bytes;                /*! Size of memory. */
    uint32_t  property;             /*! Currently bit0 is used for path upload:
                                         1 to enable auto path data uploading;
                                         0 to disable path data uploading (always embedded into command buffer).
                                         */
} hw_memory_t;

typedef struct path_point         *path_point_ptr;
typedef struct path_point
{
    /* X coordinate. */
    float                      x;

    /* Y coordinate. */
    float                      y;

    /* Flatten flag for flattened path. */
    uint8_t                              flatten_flag;

    /* Curve type for stroke path. */
    uint8_t                              curve_type;

    /* X tangent. */
    float                      tangentX;

    /* Y tangent. */
    float                      tangentY;

    /* Length of the line. */
    float                      length;

    /* Pointer to next point node. */
    path_point_ptr               next;

    /* Pointer to previous point node. */
    path_point_ptr               prev;

#define centerX                 tangentX
#define centerY                 tangentY

} path_point_t;

typedef struct sub_path     *sub_path_ptr;
typedef struct sub_path
{
    /* Pointer to next sub path. */
    sub_path_ptr             next;

    /* Number of points. */
    uint32_t                         point_count;

    /* Point list. */
    path_point_ptr           point_list;

    /* Last point. */
    path_point_ptr           last_point;

    /* Whether is path is closed. */
    uint8_t                          closed;

    /* Sub path length. */
    float                  length;
}
sub_path_t;

typedef enum cap_style
{
    CAP_BUTT,  /*! The Butt end cap style terminates each segment with a line perpendicular to the tangent at each endpoint. */
    CAP_ROUND, /*! The Round end cap style appends a semicircle with a diameter equal to the line width centered around each endpoint. */
    CAP_SQUARE /*! The Square end cap style appends a rectangle with two sides of length equal to the line width
                               perpendicular to the tangent, and two sides of length equal to half the line width parallel
                               to the tangent, at each endpoint. */
} cap_style_t;

typedef enum join_style
{
    JOIN_MITER,/*! The Miter join style appends a trapezoid with one vertex at the intersection point of the two original
                           lines, two adjacent vertices at the outer endpoints of the two "fattened" lines and a fourth vertex at
                           the extrapolated intersection point of the outer perimeters of the two "fattened" lines. */
    JOIN_ROUND,/*! The Round join style appends a wedge-shaped portion of a circle,centered at the intersection point
                           of the two original lines, having a radius equal to half the line width. */
    JOIN_BEVEL /*! The Bevel join style appends a triangle with two vertices at the outer endpoints of the two "fattened"
                           lines and a third vertex at the intersection point of the two original lines. */
}
join_style_t;

typedef struct stroke_conversion
{
    /* Stroke parameters */
    cap_style_t                stroke_cap_style;
    join_style_t               stroke_join_style;
    float                    stroke_line_width;
    float                    stroke_miter_limit;
    float                   *stroke_dash_pattern;
    uint32_t                           stroke_dash_pattern_count;
    float                    stroke_dash_phase;
    float                    stroke_dash_initial_length;
    uint32_t                           stroke_dash_initial_index;

    float                    half_line_width;

    /* Total length of stroke dash patterns. */
    float                    stroke_dash_pattern_length;

    /* For fast checking. */
    float                    stroke_miter_limit_square;

    /* Temp storage of stroke subPath. */
    path_point_ptr             path_point_list;
    path_point_ptr             path_last_point;
    uint32_t                           point_count;
    path_point_ptr             left_stroke_point;
    path_point_ptr             last_right_stroke_point;
    path_point_ptr             stroke_point_list;
    path_point_ptr             stroke_last_point;
    uint32_t                           stroke_point_count;

    /* Sub path list. */
    sub_path_ptr               stroke_sub_path_list;

    /* Last sub path. */
    sub_path_ptr               last_stroke_sub_path;

    /* Swing area handling. */
    uint8_t                            swing_need_to_handle;
    uint32_t                           swing_handling;
    uint8_t                            swing_counter_clockwise;
    float                    swing_stroke_deltax;
    float                    swing_stroke_deltay;
    path_point_ptr             swing_start_point;
    path_point_ptr             swing_start_stroke_point;
    float                    swing_accu_length;
    float                    swing_center_length;
    uint32_t                           swing_count;

    float                    stroke_path_length;
    uint32_t                           stroke_path_size;
    /* The stroke line is fat line. */
    uint8_t                            is_fat;
    uint8_t                            closed;
}
stroke_conversion_t;

/* draw path type. */
typedef enum draw_path_type
{
    DRAW_FILL_PATH = 0, /*! draw fill path. */
    DRAW_STROKE_PATH,   /*! draw stroke path. */
    DRAW_FILL_STROKE_PATH, /*! draw both fill and stroke path. */
} draw_path_type_t;

typedef struct path_des
{
    float bounding_box[4];    /*! Bounding box specified as left, top, right, and bottom. */
    path_quality_t quality;          /*! Quality hint for the path. */
    path_quality_t format;            /*! Coordinate format. */
    hw_memory_t
    uploaded;       /*! Path data that has been upload into GPU addressable memory. */
    int32_t path_length;                /*! Number of bytes in the path data. */
    void *path;                         /*! Pointer to the physical description of the path. */
    int8_t path_changed;               /* Indicate whether path data is synced with command buffer (uploaded) or not. */
    int8_t pdata_internal;             /*! Indicate whether path data memory is allocated by driver. */
    stroke_conversion_t
    stroke_conversion; /*! Refer to the definition by <code>vg_lite_stroke_conversion_t</code>.*/
    draw_path_type_t
    path_type;            /*! Refer to the definition by <code>vg_lite_draw_path_type_t</code>. */
    void *stroke_path_data;            /*! Pointer to the physical description of the stroke path. */
    int32_t stroke_path_size;          /*! Number of bytes in the stroke path data. */
} path_des_t;

typedef struct draw_path
{
    void *path;
    path_fill_t fill_rule;
    struct rtgui_matrix *matrix;
    path_blend_t blend;
    uint32_t color;
    path_linear_gradient_t *grad;

} draw_path_t;





#ifdef __cplusplus
}
#endif

#endif

