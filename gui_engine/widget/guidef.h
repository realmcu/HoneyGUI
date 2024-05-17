/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file guidef.h
  * @brief base define for bee gui
  * @details base define for bee gui
  * @author howie_wang@realsil.com.cn
  * @date 2023/10/19
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUIDEF_H__
#define __GUIDEF_H__
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "gui_list.h"
#include "gui_event.h"

#if defined(__CC_ARM) || defined(__CLANG_ARM)           /* ARM Compiler */
#include <stdarg.h>
#define gui_inline                   static __inline
#elif defined (__IAR_SYSTEMS_ICC__)     /* for IAR Compiler */
#include <stdarg.h>
#define gui_inline                   static inline
#elif defined (__GNUC__)                /* GNU GCC Compiler */
#define gui_inline                   static __inline
#elif defined _WIN32
#define gui_inline                   __inline
#else
#error not supported tool chain
#endif

/** @defgroup SUBMOUDLE SUBMOUDLE
  * @brief
  * @{
  */

/*============================================================================*
 *                         Types
 *============================================================================*/
/** @defgroup SUBMOUDLE_Exported_Types SUBMOUDLE Exported Types
  * @brief
  * @{
  */

typedef struct gui_touch_port_data
{
    uint8_t  event;                 /* The touch event of the data */
    uint8_t  track_id;              /* Track id of point */
    uint16_t width;                 /* Point of width */
    uint16_t x_coordinate;          /* Point of x coordinate */
    uint16_t y_coordinate;          /* Point of y coordinate */
    uint32_t timestamp_ms;          /* The timestamp when pressing, update periodically*/
    uint16_t x_coordinate_start;    /* Point of x when pressed, update once, for tp algo v2*/
    uint16_t y_coordinate_start;    /* Point of y when pressed, update once, for tp algo v2*/
    uint32_t timestamp_ms_start;    /* The timestamp when pressed, update once, for tp algo v2*/
    void *data;
} gui_touch_port_data_t;

typedef struct touch_info
{
    int16_t deltaX;
    int16_t deltaY;
    int16_t x;
    int16_t y;
    uint32_t type;
    uint8_t pressed  : 1;
    uint8_t released : 1;
    uint8_t pressing : 1;
} touch_info_t;

typedef struct gui_kb_port_data
{
    uint8_t  event;
    uint8_t  name[10];
    uint32_t timestamp_ms_press;            /* The timestamp when pressed, update once */
    uint32_t timestamp_ms_release;          /* The timestamp when when release, update once*/
    uint32_t timestamp_ms_pressing;         /* The timestamp when pressing, update periodically*/
} gui_kb_port_data_t;

typedef struct kb_info
{
    uint32_t type;
    char *string[20];
    uint8_t pressed  : 1;
    uint8_t released : 1;
    uint8_t pressing : 1;
} kb_info_t;

typedef struct gui_wheel_port_data
{
    uint8_t  event;
    int16_t  delta;
    uint32_t timestamp_ms;             /* The timestamp when the data was received */
} gui_wheel_port_data_t;

typedef struct wheel_info
{
    uint32_t type;
    int16_t delta;
} wheel_info_t;

typedef struct gui_rect
{
    int16_t x1;
    int16_t y1;
    int16_t x2;
    int16_t y2;
} gui_rect_t;

typedef enum t_gui_dc_type
{
    DC_RAMLESS,
    DC_SINGLE,
    DC_DOUBLE,
} T_GUI_DC_TYPE;

typedef struct _gui_frame_monitor_t
{
    uint32_t start;
    void (*start_cb)(void);
    uint32_t input_prepare;
    void (*input_prepare_cb)(void);
    uint32_t draw_prepare;
    void (*draw_prepare_cb)(void);
    uint32_t draw;
    void (*draw_cb)(void);
    uint32_t end;
    void (*end_cb)(void);
} gui_frame_monitor_t;

typedef struct gui_dispdev
{
    /* width and height */
    uint16_t screen_width;
    uint16_t screen_height;
    uint16_t fb_width;
    uint16_t fb_height;
    uint16_t driver_ic_fps;
    uint16_t driver_ic_hfp;
    uint16_t driver_ic_hbp;
    uint16_t driver_ic_active_width;
    T_GUI_DC_TYPE type;
    char gpu_type;
    /* pixel data */
    uint8_t *frame_buf;
    uint8_t *shot_buf;
    uint8_t *disp_buf_1;
    uint8_t *disp_buf_2;
    float scale_x;
    float scale_y;
    gui_rect_t section;
    uint32_t section_count;
    uint8_t bit_depth;
    void (*lcd_update)(struct gui_dispdev *dc);
    void (*virtual_lcd_update)(struct gui_dispdev *dc);
    int (*flash_seq_trans_enable)(void);
    int (*flash_seq_trans_disable)(void);
    uint32_t (*get_lcd_us)(void);
    void (*reset_lcd_timer)(void);
    void (*lcd_te_wait)(void);
    void (*lcd_power_on)(void);
    void (*lcd_power_off)(void);
    void (*lcd_draw_sync)(void);
    uint32_t frame_count;
    gui_frame_monitor_t (*lcd_frame_monitor);
} gui_dispdev_t;

typedef struct
{
    int fd;                  /* directory file */
    char buf[512];
    int num;
    int cur;
} gui_fs_dir;

struct gui_fs_dirent
{
    uint8_t d_type;           /* The type of the file */
    uint8_t d_namlen;         /* The length of the not including the terminating null file name */
    uint16_t d_reclen;        /* length of this record */
    char d_name[256];         /* The null-terminated file name */
};

struct gui_fs
{
    int (*open)(const char *file, int flags, ...);
    int (*close)(int d);
    int (*read)(int fd, void *buf, size_t len);
    int (*write)(int fd, const void *buf, size_t len);
    int (*lseek)(int fd, int offset, int whence);
    /* directory api*/
    gui_fs_dir *(*opendir)(const char *name);
    struct gui_fs_dirent *(*readdir)(gui_fs_dir *d);
    int (*closedir)(gui_fs_dir *d);
    int (*ioctl)(int fildes, int cmd, ...);
};

struct gui_indev
{
    uint16_t tp_witdh;
    uint16_t tp_height;
    uint32_t touch_timeout_ms;
    uint16_t long_button_time_ms;
    uint16_t short_button_time_ms;
    uint16_t kb_long_button_time_ms;
    uint16_t kb_short_button_time_ms;
    uint16_t quick_slide_time_ms;

    void (*ext_button_indicate)(void (*callback)(void));

    gui_touch_port_data_t *(*tp_get_data)(void);

    gui_kb_port_data_t *(*kb_get_port_data)(void);

    gui_wheel_port_data_t *(*wheel_get_port_data)(void);
};

typedef void (* log_func_t)(const char *fmt, ...);

struct gui_os_api
{
    char *name;
    void *(*thread_create)(const char *name, void (*entry)(void *param), void *param,
                           uint32_t stack_size, uint8_t priority);
    bool (*thread_delete)(void *handle);
    bool (*thread_suspend)(void *handle);
    bool (*thread_resume)(void *handle);
    bool (*thread_mdelay)(uint32_t ms);
    uint32_t (*thread_ms_get)(void);
    uint32_t (*thread_us_get)(void);
    bool (*mq_create)(void *handle, const char *name, uint32_t msg_size, uint32_t max_msgs);
    bool (*mq_send)(void *handle, void *buffer, uint32_t size, uint32_t timeout);
    bool (*mq_send_urgent)(void *handle, void *buffer, uint32_t size, uint32_t timeout);
    bool (*mq_recv)(void *handle, void *buffer, uint32_t size, uint32_t timeout);

    void *(*f_malloc)(uint32_t);
    void *(*f_realloc)(void *ptr, uint32_t);
    void (*f_free)(void *rmem);
    void *mem_addr;
    uint32_t mem_size;

    uint32_t mem_threshold_size;
    void *lower_mem_addr;
    uint32_t lower_mem_size;

    log_func_t log;
    void (*gui_tick_hook)(void);
};

typedef struct gui_matrix
{
    float m[3][3];
} gui_matrix_t;

typedef struct gui_point
{
    float p[3];
} gui_point_t;

typedef struct draw_img
{
    uint16_t img_w;
    uint16_t img_h;
    int16_t img_target_x; //display start X
    int16_t img_target_y;  //display start Y
    uint16_t img_target_w;
    uint16_t img_target_h;
    void *data;
    gui_matrix_t matrix; //seems can remve by howie
    gui_matrix_t inverse;
    uint8_t opacity_value;
    uint32_t blend_mode : 3;
    uint32_t checksum : 8;
    uint32_t src_mode : 3;
    uint32_t high_quality : 1;
    //uint32_t color_mix; //todo for QuDai
    void *acc_user;
} draw_img_t;

typedef struct acc_engine
{
    void (*blit)(draw_img_t *image, struct gui_dispdev *dc, gui_rect_t *rect);
    //todo
    // void (*draw_circle)(canvas_circle_t *circle, struct gui_dispdev *dc);
    // void (*draw_rectangle)(canvas_rectangle_t *r, struct gui_dispdev *dc);
    // void (*draw_arc)(canvas_arc_t *a, struct gui_dispdev *dc);
    // void (*draw_line)(canvas_line_t *l, struct gui_dispdev *dc);
    // void (*draw_polyline)(canvas_polyline_t *p, struct gui_dispdev *dc);
    // void (*draw_path)(draw_path_t *path, struct gui_dispdev *dc);
    // void (*draw_wave)(canvas_wave_t *w, struct gui_dispdev *dc);
    // void (*draw_palette_wheel)(canvas_palette_wheel_t *pw, struct gui_dispdev *dc);

    // void (*draw_svg)(void *svg, uint32_t data_length, struct gui_dispdev *dc, int x, int y,
    //                  float scale, float rotate_degree, float rotate_center_x, float rotate_center_y);
    // void (*begin_path)(canvas_path_t *data);
    // void (*move_to)(canvas_path_t *data, float x, float y);
    // void (*bezier_to)(canvas_path_t *data, float c1x, float c1y, float c2x, float c2y, float x,
    //                   float y);
    // void (*LineTo)(canvas_path_t *data, float x, float y);

} acc_engine_t;

typedef enum
{
    TOUCH_INIT                  = 0x100,
    TOUCH_HOLD_X                = 0x101,
    TOUCH_HOLD_Y                = 0x102,
    TOUCH_SHORT                 = 0x103,
    TOUCH_LONG                  = 0x104,
    TOUCH_ORIGIN_FROM_X         = 0x105,
    TOUCH_ORIGIN_FROM_Y         = 0x106,
    TOUCH_LEFT_SLIDE            = 0x107,
    TOUCH_RIGHT_SLIDE           = 0x108,
    TOUCH_UP_SLIDE              = 0x109,
    TOUCH_DOWN_SLIDE            = 0x10A,
    TOUCH_SHORT_BUTTON          = 0x10B,
    TOUCH_LONG_BUTTON           = 0x10C,
    TOUCH_UP_SLIDE_TWO_PAGE     = 0x10D,
    TOUCH_DOWN_SLIDE_TWO_PAGE   = 0x10E,
    TOUCH_INVALIDE              = 0x10F,

    KB_INIT                     = 0x200,
    KB_SHORT                    = 0x201,
    KB_LONG                     = 0x202,
    KB_INVALIDE                 = 0x203,

    WHEEL_INIT                  = 0x300,
    WHEEL_ING                   = 0x301,
    WHEEL_FINISHED              = 0x302,
    WHEEL_INVALIDE              = 0x303,
} T_GUI_INPUT_TYPE;

typedef enum t_slide_style
{
    CLASSIC          = 0x0000,
    REDUCTION        = 0x0001,
    FADE             = 0x0002,
    REDUCTION_FADE   = 0x0003,
    STACKING         = 0x0004,

    TAB_ROTATE       = 0x0005,
    TAB_CUBE         = 0x0006,
    TAB_PAGE         = 0x0007,
} T_SLIDE_STYLE;

typedef enum t_obj_type
{
    SCREEN                   = 0x0000,
    WINDOW                   = 0x0001,
    TABVIEW                  = 0x0002,
    TAB                      = 0x0003,
    CURTAINVIEW              = 0x0004,
    CURTAIN                  = 0x0005,
    IMAGE                    = 0x0006,
    BUTTON                   = 0x0007,
    ICONLIST                 = 0x0008,
    ICON                     = 0x0009,
    IMAGE_FROM_MEM           = 0x000A,
    TEXTBOX                  = 0x000B,
    SCROLLTEXTBOX            = 0x000C,
    SEEKBAR                  = 0x000D,
    PROGRESSBAR              = 0x000E,
    CLICKSWITCH              = 0x000F,
    PAGE                     = 0x0010,
    SCROLL_WHEEL             = 0x0011,
    PAGEBAR                  = 0x0012,
    RETURNWIDGET             = 0x0013,
    RECTANGLE                = 0x0014,
    WINDOWWITHBORDER         = 0x0015,
    CANVAS                   = 0x0016,
    VG_LITE_CLOCK            = 0x0017,
    VG_LITE_CUBE             = 0x0018,
    GRID                     = 0x0019,
    RADIO                    = 0x001A,
    RADIOSWITCH              = 0x001B,
    ARC                      = 0x001C,
    JAVASCRIPT               = 0x001D,
    MOVIE                    = 0x001E,
    IMAGE_SCOPE              = 0x001F,
    CARDVIEW                 = 0x0020,
    CARD                     = 0x0021,
    PAGELIST                 = 0x0022,
    PAGELISTVIEW             = 0x0023,
    MACRO_ANIMATETRANSFORM   = 0x0024,
    HONEYCOMB_LIST           = 0x0025,
    WHEEL_LIST               = 0x0026,
    QRCODE                   = 0x0027,
    GALLERY                  = 0x0028,
    TURN_TABLE               = 0x0029,
} T_OBJ_TYPE;

typedef struct gui_msg
{
    /* the event type */
    uint16_t type;
    /* user field of event */
    union
    {
        uint32_t  param;
        void      *payload;
    } u;
    void *app;
    void *cb;
} gui_msg_t;

typedef struct gui_index
{
    int8_t x;
    int8_t y;
} gui_index_t;

typedef struct gui_jump
{
    bool jump_flag;
    gui_index_t jump_id;
} gui_jump_t;

typedef struct gui_animate
{
    uint32_t dur;
    int repeat_count;
    uint32_t init_time_ms;
    uint32_t cur_time_ms;
    uint32_t current_repeat_count;
    uint32_t current_frame;
    uint32_t last_round;
    float progress_percent;
    void (* callback)(void *p);
    void *p;
    bool animate;
    bool init;
    bool Beginning_frame;
    bool end_frame;
} gui_animate_t;

typedef enum
{
    OBJ_INPUT_PREPARE,
    OBJ_PREPARE,
    OBJ_DRAW,
    OBJ_END,
    OBJ_DESTORY,
} T_OBJ_CB_TYPE;

typedef struct _gui_obj_t
{
    const char *name;
    struct _gui_obj_t *parent;//point to father obj

    int16_t x;
    int16_t y;
    int16_t w;
    int16_t h;
    gui_list_t  child_list;
    gui_list_t  brother_list;

    //start of run time
    void (* obj_cb)(struct _gui_obj_t *obj, T_OBJ_CB_TYPE cb_type);
    //end of run time
    T_OBJ_TYPE type; //no need this , only use name
    uint32_t active               : 1;    // this flag means obj location in screen
    uint32_t not_show             : 1;

    uint32_t skip_tp_left_hold    : 1;
    uint32_t skip_tp_right_hold   : 1;
    uint32_t skip_tp_up_hold      : 1;
    uint32_t skip_tp_down_hold    : 1;
    uint32_t create_done          : 1;
    uint32_t flag_3d              : 1;
    uint32_t gesture              : 1;
    uint32_t event_dsc_cnt        : 5;
    uint32_t opacity_value        : 8;
    uint32_t has_input_prepare_cb : 1;
    uint32_t has_prepare_cb       : 1;
    uint32_t has_draw_cb          : 1;
    uint32_t has_end_cb           : 1;
    uint32_t has_destroy_cb       : 1;

    gui_event_dsc_t *event_dsc;

    gui_matrix_t *matrix;

} gui_obj_t;

typedef struct _gui_color
{
    union
    {
        struct
        {
            uint8_t b; ///< Blue color component, at the lowest address
            uint8_t g; ///< Green color component
            uint8_t r; ///< Red color component
            uint8_t a; ///< Alpha channel for transparency, at the highest address
        } rgba;
        uint32_t rgba_full;
    } color;
} gui_color_t;

typedef struct gui_rgb_data_head
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
} gui_rgb_data_head_t;

typedef struct _gui_rect_file_head
{
    struct gui_rgb_data_head head;
    gui_color_t color;
} gui_rect_file_head_t;

/** End of SUBMOUDLE_Exported_Types
  * @}
  */

/*============================================================================*
 *                         Constants
 *============================================================================*/
/** @defgroup SUBMOUDLE_Exported_Constants SUBMOUDLE Exported Constants
  * @brief
  * @{
  */


/** End of SUBMOUDLE_Exported_Constants
  * @}
  */

/*============================================================================*
 *                         Macros
 *============================================================================*/
/** @defgroup SUBMOUDLE_Exported_Macros SUBMOUDLE Exported Macros
  * @brief
  * @{
  * The color used in the GUI:
  *
  *         bit        bit
  * RGB565  15 R,G,B   0
  * BGR565  15 B,G,R   0
  * RGB888  23 R,G,B   0
  * ARGB888 31 A,R,G,B 0
  * RGBA888 31 R,G,B,A 0
  * ABGR888 31 A,B,G,R 0
  *
  * The gui_color is defined as ARGB888.
  *        bit31 A,R,G,B bit0
  */


#define GUI_SWAP16(x)          ((uint16_t)(                         \
                                                                    (((uint16_t)(x) & (uint16_t)0x00ff) <<  8) |            \
                                                                    (((uint16_t)(x) & (uint16_t)0xff00) >>  8)))

#define GUI_SWAP32(x)          ((uint32_t)(                         \
                                                                    (((uint32_t)(x) & (uint32_t)0x000000ff) << 24) |            \
                                                                    (((uint32_t)(x) & (uint32_t)0x0000ff00) <<  8) |            \
                                                                    (((uint32_t)(x) & (uint32_t)0x00ff0000) >>  8) |            \
                                                                    (((uint32_t)(x) & (uint32_t)0xff000000) >> 24)))

#ifndef M_PI
#define M_PI    ((float)3.14159265358979323846)
#endif

#define _UI_MIN(x, y)           (((x)<(y))?(x):(y))
#define _UI_MAX(x, y)           (((x)>(y))?(x):(y))
#define _UI_BITBYTES(bits)      ((bits + 7)/8)
#define _UI_ABS(x)              ((x)>=0? (x):-(x))

#define GUI_UNUSED(x) (void)(x)             /* macro to get rid of 'unused parameter' warning */
#define GUI_TYPE(type, obj) ((type *)obj)

#if defined __WIN32


//#define GUI_ROOT_FOLDER "gui_engine\\example\\screen_480_480\\root\\"
extern char *defaultPath;
#define GUI_ROOT_FOLDER defaultPath
#else
#define GUI_ROOT_FOLDER "/"
#endif
#define GUI_NEW(type, constructor_cb, param) type *this = gui_malloc(sizeof(type));\
    memset(this, 0, sizeof(type));\
    constructor_cb(param);\
    /*todo enter critical section*/\
    gui_list_init(&(GET_BASE(this)->child_list));\
    if ((GET_BASE(this)->parent) != NULL)\
    {\
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list), &(GET_BASE(this)->brother_list));\
    }\
    GET_BASE(this)->create_done = true;\
    /*todo exit critical section*/\
    return this;
#define GUI_CREATE_HELPER(type, constructor_cb, param) type *this = gui_malloc(sizeof(type));\
    memset(this, 0, sizeof(type));\
    constructor_cb(param);\
    /*todo enter critical section*/\
    gui_list_init(&(GET_BASE(this)->child_list));\
    if ((GET_BASE(this)->parent) != NULL)\
    {\
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list), &(GET_BASE(this)->brother_list));\
    }\
    GET_BASE(this)->create_done = true;
#define GET_BASE(_p) ((gui_obj_t *)_p)
#define GUI_BASE(_p) ((gui_obj_t *)_p)
#if defined _WIN32
#define GUI_FRAME_STEP 50
#else
#define GUI_FRAME_STEP 50
#endif


/** End of SUBMOUDLE_Exported_Macros
  * @}
  */

/*============================================================================*
 *                         Variables
 *============================================================================*/
/** @defgroup SUBMOUDLE_Exported_Variables SUBMOUDLE Exported Variables
  * @brief
  * @{
  */


/** End of SUBMOUDLE_Exported_Variables
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/
/** @defgroup SUBMOUDLE_Exported_Functions SUBMOUDLE Exported Functions
  * @brief
  * @{
  */

/** End of SUBMOUDLE_Exported_Functions
  * @}
  */

/** End of SUBMOUDLE
  * @}
  */


#ifdef __cplusplus
}
#endif

#endif

