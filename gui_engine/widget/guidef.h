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
#include "def_list.h"
#include "def_event.h"
#include "def_color.h"
#include "def_matrix.h"
#include "def_vg.h"
#include "def_file.h"



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

/*============================================================================*
 *                         Types
 *============================================================================*/

typedef struct touch_info
{
    int16_t deltaX;
    int16_t deltaY;
    int16_t x;
    int16_t y;
    int16_t history_x;
    int16_t history_y;
    uint32_t type;
    uint8_t pressed  : 1;
    uint8_t released : 1;
    uint8_t pressing : 1;
    uint8_t button_down  : 1;
    uint8_t button_up  : 1;
    uint8_t button_hold  : 1;
} touch_info_t;


typedef struct kb_info
{
    uint32_t type;
    char *string[20];
    uint8_t pressed  : 1;
    uint8_t released : 1;
    uint8_t pressing : 1;
} kb_info_t;



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

typedef enum t_gui_pfb_type
{
    PFB_Y_DIRECTION,
    PFB_X_DIRECTION,
} T_GUI_PFB_TYPE;
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

typedef struct gui_kb_port_data
{
    uint8_t  event;
    uint8_t  name[10];
    uint32_t timestamp_ms_press;            /* The timestamp when pressed, update once */
    uint32_t timestamp_ms_release;          /* The timestamp when when release, update once*/
    uint32_t timestamp_ms_pressing;         /* The timestamp when pressing, update periodically*/
} gui_kb_port_data_t;
#define GUI_WHEEL_NO_EVENT 0
#define GUI_WHEEL_BUTTON_DOWN 1
#define GUI_WHEEL_BUTTON_UP 2
#define GUI_WHEEL_SCROLL 3
typedef struct gui_wheel_port_data
{
    uint8_t  event;//0:no event//1:button down//2:button up//3:scroll
    int16_t  delta;//positive away from the user and negative toward the user
    uint32_t timestamp_ms;             /* The timestamp when the data was received */
} gui_wheel_port_data_t;
typedef struct gui_dispdev
{
    /* width and height */
    uint16_t screen_width;
    uint16_t screen_height;
    uint16_t fb_width;
    uint16_t fb_height;
    uint16_t bit_depth;
    uint16_t driver_ic_fps;
    uint16_t driver_ic_hfp;
    uint16_t driver_ic_hbp;
    uint16_t driver_ic_active_width;
    uint16_t target_w;
    uint16_t target_h;
    T_GUI_DC_TYPE type;
    T_GUI_PFB_TYPE pfb_type;
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
    uint32_t section_total;
    void (*lcd_update)(struct gui_dispdev *dc);
    void (*direct_draw_bitmap_to_lcd)(int16_t x, int16_t y, int16_t width, int16_t height,
                                      const uint8_t *bitmap);
    void (*virtual_lcd_update)(struct gui_dispdev *dc);
    int (*flash_seq_trans_enable)(void);
    int (*flash_seq_trans_disable)(void);
    uint32_t (*get_lcd_us)(void);
    void (*reset_lcd_timer)(void);
    void (*lcd_section_hook)(struct gui_dispdev *dc);
    void (*lcd_te_wait)(void);
    void (*lcd_power_on)(void);
    void (*lcd_power_off)(void);
    void (*lcd_draw_sync)(void);
    uint8_t *lcd_gram;
    uint32_t frame_count;
    gui_frame_monitor_t (*lcd_frame_monitor);
} gui_dispdev_t;
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
    uint32_t high_quality : 1;
    //uint32_t color_mix; //todo for QuDai
    void *acc_user;
} draw_img_t;


struct gui_fs_dirent
{
    uint8_t d_type;           /* The type of the file */
    uint8_t d_namlen;         /* The length of the not including the terminating null file name */
    uint16_t d_reclen;        /* length of this record */
    char *d_name;         /* The null-terminated file name */
    void *dirent;
};
typedef struct
{
    void *dir;
    struct gui_fs_dirent *dirent;
} gui_fs_dir;
/* gui_fs struct define start */
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
/* gui_fs struct define end */

/* gui_ftl struct define start */
struct gui_ftl
{
    int (*read)(uint32_t addr, uint8_t *buf, uint32_t len);
    int (*write)(uint32_t addr, const uint8_t *buf, uint32_t len);
    int (*erase)(uint32_t addr, uint32_t len);
};
/* gui_ftl struct define end */

/* gui_indev struct define start */
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
/* gui_indev struct define end */

typedef void (* log_func_t)(const char *fmt, ...);

/* gui_os_api struct define start */
typedef struct gui_os_api
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

    void (*gui_sleep_cb)(void);

    void *mem_addr;
    uint32_t mem_size;

    uint32_t mem_threshold_size;
    void *lower_mem_addr;
    uint32_t lower_mem_size;

    log_func_t log;
    void (*gui_tick_hook)(void);
} gui_os_api_t;
/* gui_os_api struct define end */

typedef struct draw_circle
{
    int16_t x;
    int16_t y;
    int16_t Radius;
    int16_t lineWidth;
    gui_color_t color;
} draw_circle_t;


/* T_GUI_INPUT_TYPE enum start*/
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
/* T_GUI_INPUT_TYPE enum end*/




typedef void (*gui_msg_cb)(void *);
typedef struct gui_msg
{
    uint16_t event;
    uint16_t sub_event;
    gui_msg_cb cb;
    void *payload;
} gui_msg_t;

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
    float last_per;
    void (* callback)(void *p, void *this_widget, struct gui_animate *animate);
    void *p;
    const char *name;
    bool animate;
    bool init;
    bool Beginning_frame;
    bool end_frame;
} gui_animate_t;
/**
 * @brief Macro that defines the parameters for an animation callback function.
 *
 * This macro specifies the parameters that an animation callback function must take.
 * These parameters typically include a generic pointer `p`, a pointer to the widget
 * being animated `this_widget`, and a pointer to an `animate` structure.
 *
 * @param p            A generic pointer parameter that can be used to pass custom data.
 * @param this_widget  A pointer to the widget that is being animated.
 * @param animate      A pointer to the structure containing animation details.
 */
#define GUI_ANIMATION_CALLBACK_PARAMETER void *p, void *this_widget, struct gui_animate *animate

/**
 * @brief Macro to define an animation callback function.
 *
 * This macro is used to define the signature of an animation callback function.
 * The `function_name` parameter specifies the name of the function. The function
 * will take the parameters defined by `GUI_ANIMATION_CALLBACK_PARAMETER`.
 *
 * @param function_name  The name of the animation callback function to define.
 */
#define GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(function_name) void function_name(GUI_ANIMATION_CALLBACK_PARAMETER)

typedef void (* gui_animate_callback_t)(void *p, void *this_widget, gui_animate_t *animate);

typedef struct _gui_rect_file_head
{
    struct gui_rgb_data_head head;
    gui_color_t color;
} gui_rect_file_head_t;

/* acc_engine_t struct define start */
typedef struct acc_engine
{
    void (*blit)(draw_img_t *image, gui_dispdev_t *dc, gui_rect_t *rect);
    //todo
    void (*draw_circle)(draw_circle_t *circle, gui_dispdev_t *dc, gui_rect_t *rect);
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
/* acc_engine_t struct define end */

/*============================================================================*
 *                         Constants
 *============================================================================*/


/*============================================================================*
 *                         Macros
 *============================================================================*/

/** @brief
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
#define _GUI_API_DEFINE(type) typedef struct __gui_api_##type{
#define _GUI_API_DECLARE(type) } _gui_api_##type;extern _gui_api_##type _gui_api_for_##type;
#define _GUI_API_ASSIGN(type) _gui_api_##type _gui_api_for_##type ={
#define GUI_API(type)  _gui_api_for_##type
#if defined _WIN32
#define GUI_FRAME_STEP 50
#else
#define GUI_FRAME_STEP 50
#endif
#define GUI_TOUCHPAD_IMPORT_AS_TP touch_info_t *tp = tp_get_info();
#define IMPORT_GUI_TOUCHPAD  extern touch_info_t *tp_get_info(void); touch_info_t *touch = tp_get_info();
#define IMPORT_GUI_WHEEL extern touch_info_t *wheel_get_info(void);\
    touch_info_t *wheel = wheel_get_info();
#define GUI_ERROR (gui_error_t)1
#define GUI_ERROR_NULL (gui_error_t)2
#define GUI_ERROR_FORMAT (gui_error_t)3
#define GUI_SUCCESS (gui_error_t)0
typedef int gui_error_t;
#define GUI_MAGIC_NUMBER 0x0b
#define GUI_WIDGET_TRY_EXCEPT(obj) {GUI_ASSERT((obj&&GUI_BASE(obj)->magic == GUI_MAGIC_NUMBER))}
/**
 * @brief Macro to retrieve a GUI widget pointer by its name.
 *
 * This macro initializes a `gui_obj_t` pointer to `NULL`, retrieves the specified named widget
 * from the GUI object tree starting from the current application root, assigns the retrieved
 * widget to the pointer, and performs an exception check on the pointer.
 *
 * @param[in] pointer The name of the pointer variable that will hold the retrieved widget.
 * @param[in] name The name of the widget to retrieve.
 *
 * @note This macro assumes the existence of the function `gui_obj_tree_get_widget_by_name`,
 *       the function `gui_current_app()`, and the macro `GUI_WIDGET_TRY_EXCEPT`.
 *       The `gui_obj_tree_get_widget_by_name` function is declared as `extern` within the macro.
 */
#define GUI_WIDGET_POINTER_BY_NAME(pointer, name) gui_obj_t *pointer = 0;\
    extern void gui_obj_tree_get_widget_by_name(gui_obj_t *, const char *, gui_obj_t **);\
    extern gui_app_t *gui_current_app(void);\
    gui_obj_tree_get_widget_by_name((void *)gui_current_app(), name, &pointer);\
    GUI_WIDGET_TRY_EXCEPT(pointer)
/**
 * @brief Macro to retrieve a GUI widget pointer by its type.
 *
 * This macro initializes a `gui_obj_t` pointer to `NULL`, retrieves the specified type of widget
 * from the GUI object tree starting from a given root, assigns the retrieved widget to the
 * pointer, and performs an exception check on the pointer.
 *
 * @param[in] pointer The name of the pointer variable that will hold the retrieved widget.
 * @param[in] type The type of widget to retrieve.
 * @param[in] root The root of the GUI object tree where the search begins.
 *
 */
#define GUI_WIDGET_POINTER_BY_TYPE(pointer, type, root) \
    GUI_WIDGET_TRY_EXCEPT(root)\
    gui_obj_t *pointer = 0;\
    extern void gui_obj_tree_get_widget_by_type(gui_obj_t *, T_OBJ_TYPE , gui_obj_t **);\
    gui_obj_tree_get_widget_by_type(root, type, &pointer);\
    GUI_WIDGET_TRY_EXCEPT(pointer)
/**
 * @brief Macro to calculate the size of an array.
 *
 * This macro calculates the number of elements in an array by dividing the total size of the
 * array by the size of its first element.
 *
 * @param[in] array The array whose size is to be determined.
 *
 * @return The number of elements in the array.
 */
#define GUI_ARRAY_SIZE(array) (sizeof(array)/sizeof(array[0]))
/**
 * @brief Macro to check the type of a GUI widget and log an error if it doesn't match the expected type.
 *
 * This macro first performs an exception check on the given object. Then, it compares the type of the
 * GUI widget with the expected type. If the types do not match, it logs an error message. Finally,
 * it asserts that the widget type matches the expected type.
 *
 * @param[in] obj The GUI object whose type is to be checked.
 * @param[in] widget_type The expected type of the GUI widget.
 *
 */
#define GUI_WIDGET_TYPE_TRY_EXCEPT(obj,widget_type) {GUI_WIDGET_TRY_EXCEPT(obj)if ((GUI_BASE(obj)->type != widget_type))\
        {\
            extern void gui_log(const char *format, ...);\
            gui_log("GUI_WIDGET_TYPE_TRY_EXCEPT: type is %d ;",GUI_BASE(obj)->type);\
        }\
        {GUI_ASSERT((GUI_BASE(obj)->type == widget_type))}}
#define GUI_SPEED_RECODE_LENGTH 5
#define GUI_SPEED_RECODE_DEFINE int speed_recode[GUI_SPEED_RECODE_LENGTH];
#define GUI_SPEED_MAX 60
#define GUI_SPEED_MIN 7
#define GUI_SET_ANIMATE_HELPER \
    gui_animate_t *animate = this->animate;\
    \
    if (!(animate))\
    {\
        animate = gui_malloc(sizeof(gui_animate_t));\
    }\
    \
    memset((animate), 0, sizeof(gui_animate_t));\
    animate->animate = true;\
    animate->dur = dur;\
    animate->callback = (gui_animate_callback_t)callback;\
    animate->repeat_count = repeat_count;\
    animate->p = p;\
    this->animate = animate;
#define GUI_STRINGS_EQUAL(string1, string2) (!strcmp(string1, string2))
#define GUI_ANIMATION_CALLBACK(function_name) void function_name(void *p, void *this_widget, gui_animate_t *animate)


/*============================================================================*
 *                         Variables
 *============================================================================*/


/*============================================================================*
 *                         Functions
 *============================================================================*/



#ifdef __cplusplus
}
#endif

#endif

