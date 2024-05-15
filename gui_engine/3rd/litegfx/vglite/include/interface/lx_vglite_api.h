/******************************************************************************
* Copyright (c) 2023 Shanghai QDay Technology Co., Ltd.
* All rights reserved.
*
* This file is part of the LiteGFX 0.0.1 distribution.
*
* This software is licensed under terms that can be found in the LICENSE file in
* the root directory of this software component.
* If no LICENSE file comes with this software, it is provided AS-IS.
*
* Author:LiteGFX Team
* Date:2023.11.08
*******************************************************************************/

#ifndef LX_VGLITE_API_H
#define LX_VGLITE_API_H

#if defined(__cplusplus)
extern "C" {
#endif

/*********************
*      INCLUDES
*********************/
#include <stdint.h>
#include <stdio.h>

/*********************
*      DEFINES
*********************/
#define LX_VGWIDGET_INS_DEMO lx_vglite_load_demo_widget()/*仅供调试*/
#define LX_VGWIDGET_INS_BUTTERFLY lx_vglite_load_butterfly_widget()/*蝴蝶翩翩(数字时钟)特效*/
#define LX_VGWIDGET_INS_SOCCER lx_vglite_load_soccer_widget()/*球型+粒子特效*/
#define LX_VGWIDGET_INS_FLOWER_FALL lx_vglite_load_flower_fall_widget()/*落花纷飞(数字时钟)特效*/

#define LX_VGTRANS_INS_DEMO  lx_vglite_load_demo_transition()/*仅供调试*/

/**********************
*      TYPEDEFS
**********************/


enum LX_CMD_ID
{
    LX_CMD_ID_INIT = 0,
    LX_CMD_ID_GET_IMAGE,
    LX_CMD_ID_FREE_IMAGE,
    LX_CMD_ID_SET_SELECTED,
    LX_CMD_ID_SET_NUMBER,
    LX_CMD_ID_CLICK,
    LX_CMD_ID_REMOVE,
    LX_CMD_ID_BATTERY,
    LX_CMD_ID_HEARTRATE,
    LX_CMD_ID_KAL,
    LX_CMD_ID_STEP,
    LX_CMD_ID_TEMPR_LOW,//最低温度
    LX_CMD_ID_TEMPR_HI,//最高温度
    LX_CMD_ID_TEMPR_CURR,//当前温度
    LX_CMD_ID_TEMPR_UINT,//温度单位，0：摄氏度，1：华氏度
    LX_CMD_ID_WEATHER,//天气图标的ID
    LX_CMD_ID_PROGRESS,//进度
    LX_CMD_ID_COLOR,//颜色
    LX_CMD_ID_SHOW_TIME,//是否显示时间
    LX_CMD_ID_FIXED_BUTTON,//是否显示固定表盘的按钮
    LX_CMD_ID_TIME_FORMAT,//时间制式，0：24小时制，1：12小时制
};

typedef bool (*lx_vglite_event_cb_t)(void *context_p, uint8_t event, uintptr_t param1,
                                     uintptr_t param2);

typedef struct lx_vglite_context
{
    lx_vglite_event_cb_t event_callback;
    uintptr_t user_data;
} lx_vglite_context_t;

enum LX_VGLITE_EVENT
{
    LX_VGLITE_EVENT_INIT = 0,
    LX_VGLITE_EVENT_DEINIT,
    LX_VGLITE_EVENT_SETUP,
    LX_VGLITE_EVENT_TEARDOWN,
    LX_VGLITE_EVENT_RESUME,
    LX_VGLITE_EVENT_PAUSE,
    LX_VGLITE_EVENT_UPDATE,
    LX_VGLITE_EVENT_RENDER,
    LX_VGLITE_EVENT_TOUCH,
    LX_VGLITE_EVENT_SET_IMAGE,
    LX_VGLITE_EVENT_SET_PARAM,
    LX_VGLITE_EVENT_SET_USER_CALLBACK,
};

/*Image color format*/
typedef enum
{
    LX_CANVAS_CF_RGB565 = 0,
    LX_CANVAS_CF_RGB888,
} lx_canvas_format_t;

typedef struct lx_canvas_buffer
{
    lx_canvas_format_t format;
    int16_t w;
    int16_t h;
    void *buffer;
} lx_canvas_buffer_t;

/*Image color format*/
typedef enum
{
    LX_IMG_CF_RGB565 = 0,
    LX_IMG_CF_RGBA5658,
    LX_IMG_CF_RGB888,
    LX_IMG_CF_RGBA8888
} lx_img_cf_t;

typedef struct lx_vglite_image
{
    lx_img_cf_t format;
    int16_t w;
    int16_t h;
    uint32_t size;
    void *data;
} lx_vglite_image_t;

typedef struct lx_point
{
    int16_t x; /* x coord */
    int16_t y; /* y coord */
} lx_point_t;

typedef struct lx_rect
{
    int16_t x; /* x coord */
    int16_t y; /* y coord */
    int16_t w; /* row width in pixel */
    int16_t h; /* column height in pixel */
} lx_rect_t;

typedef enum
{
    LX_TOUCH_UP = 0,
    LX_TOUCH_DOWN,
    LX_TOUCH_MOVE
} lx_touch_state_t;

typedef uintptr_t (*lx_vglite_user_cb_t)(uint16_t cmd, uintptr_t param, uintptr_t user_data);

/**********************
* GLOBAL PROTOTYPES
**********************/
uintptr_t lx_vglite_load_demo_widget(void);
uintptr_t lx_vglite_load_butterfly_widget(void);
uintptr_t lx_vglite_load_soccer_widget(void);
uintptr_t lx_vglite_load_flower_fall_widget(void);

/*
    功能：特效实例初始化
    返回类型：uintptr_t，该方法返回的为该特效实例的 handler，通过 handler 可以对该特效实例
    进行系列操作
    参 数 1 ： uintptr_t instance，该参数为特效类型实例，例如传入参数“LX_VGWIDGET_INS_PRISM”
    参数 void *context，该参数待定
    参数 3、4：vglite buffer 对应的宽和高
*/
uintptr_t lx_vglite_init(uintptr_t instance, void *context, int16_t width, int16_t height);

/*
    功能：释放特效实例
    参数：uintptr_t handler，待释放特效实例对应的 handler
*/
void lx_vglite_deinit(uintptr_t handler);

/*
    功能：特效界面构建、布局
    参数：uint32_t handler，该特效实例对应的 handler
*/
void lx_vglite_setup(uintptr_t handler);

/*
    功能：特效界面恢复
    参数：uint32_t handler，该特效实例对应的 handler
*/
void lx_vglite_resume(uintptr_t handler);

/*
    功能：特效界面暂停
    参数：uint32_t handler，该特效实例对应的 handler
*/
void lx_vglite_pause(uintptr_t handler);

/*
    功能：特效界面销毁
    参数：uint32_t handler，该特效实例对应的 handler
*/
void lx_vglite_teardown(uintptr_t handler);

/*
    功能：特效界面更新
    参数：uint32_t handler，该特效实例对应的 handler
    注：须以每秒 30 帧（默认值，可根据特效情况微调）的频率调用本接口，调用完本接口后需要触发界面渲染
*/
void lx_vglite_update(uintptr_t handler);

/*
    功能：特效界面渲染
    参数 1：uint32_t handler，该特效实例对应的 handler
    参数 2～5：渲染区域坐标（通过 lvgl 接口渲染时，会进行分块）
*/
void lx_vglite_render(uintptr_t handler, int16_t x1, int16_t x2, int16_t y1, int16_t y2);

/*
    功能：填充特效所需的图像
    参数 1：uint32_t handler，该特效实例对应的 handler
    参数 2：uint16_t index，图像对应的索引值
    参数 3：void* data，图像 buffer 的地址
    参数 4：lx_img_cf_t format，图像格式（lx_img_cf_t 定义 见《lx_vglite_api.h》）
    参数 5、6：图像的宽和高
    注：须在 lx_vglite_setup()方法调用前填充
*/
void lx_vglite_set_image(uintptr_t handler, uint16_t index, void *data, lx_img_cf_t format,
                         int16_t w, int16_t h, uint32_t size);

/*
    功能：传递触摸屏事件
    参数 1：uint32_t handler，该特效实例对应的 handler
    参数 2： lx_touch_state_t state,触摸状态（lx_touch_state_t 定义 见《lx_vglite_api.h》）
    参数 3、4：触摸坐标
*/
void lx_vglite_touch(uintptr_t handler, lx_touch_state_t state, int16_t x, int16_t y);

/*
    功能：向特效传递需要的参数，比如当前选中项，电池电量等
    参数 1：uint32_t handler，该特效实例对应的 handler
    参数 2：uint16_t cmd，参数 id，（见《lx_vglite_api.h》中定义的“LX_CMD_ID”）
    参数 3：uint32_t param，参数值
*/
void lx_vglite_set_param(uintptr_t handler, uint16_t cmd, uintptr_t param);

/*
    功能：向特效注册回调，以便接受特效传递的参数/指令，比如当前选中项等
    参数 1：uintptr_t handler，该特效实例对应的 handler
    参数 2：lx_vglite_user_cb_t callback，需要注册的回调函数(lx_vglite_user_cb_t 定义见《lx_vglite_api.h》)
    参数 3：uintptr_t user_data，用户数据，特效调用回调函数时会带上该值
*/
void lx_vglite_set_user_callback(uintptr_t handler, lx_vglite_user_cb_t callback,
                                 uintptr_t user_data);

uintptr_t lx_vglite_load_demo_transition(void);

/*
    功能：过场特效构建
    参数 1：uintptr_t handler，该特效实例对应的 handler
    参数 bool return_back，false时为入场特效，true时为出场特效
*/
void lx_vglite_trans_setup(uintptr_t handler, bool return_back);

/*
    功能：过场特效销毁
    参数：uintptr_t handler，该特效实例对应的 handler
*/
void lx_vglite_trans_teardown(uintptr_t handler);

/*
    功能：过场特效更新
    参数 1：uintptr_t handler，该特效实例对应的 handler
    参数 2：vg_lite_buffer_t *buf，该 vglite buffer 用于渲染过场特效的某一帧画面
    参数 3：uint8_t percent，过场特效进展的百分比，特效会根据当前进展的百分比渲染画面
*/
void lx_vglite_trans_update(uintptr_t handler, void *context, uint8_t percent);

/*
    功能：过场特效渲染
    参数 1：uintptr_t handler，该特效实例对应的 handler
    参数 2～5：渲染区域坐标
*/
void lx_vglite_trans_render(uintptr_t handler, int16_t x1, int16_t y1, int16_t x2, int16_t y2);

/*
    功能：设置过场特效旧界面的图像与新界面的图像
    参数 1：uintptr_t handler，该特效实例对应的 handler
    参数 2：uint16_t index，index 为 0 则为旧的界面，index 为 1 则为新的界面
    参数 3：void* data,图像 buffer 的地址
    参数 4：lx_img_cf_t format，图像格式（lx_img_cf_t 定义 见《lx_vglite_api.h》）
    参数 5、6：图像的宽和高
    注：须在 lx_vglite_trans_setup()方法调用前设置
*/
void lx_vglite_trans_set_src(uintptr_t handler, uint16_t index, void *data, lx_img_cf_t format,
                             int16_t w, int16_t h);

/**********************
* INTERNAL PROTOTYPES
**********************/
void *lx_vglite_malloc(size_t sz);
void lx_vglite_free(void *ptr);
lx_vglite_context_t *lx_vglite_context_create(void);
void lx_vglite_context_free(lx_vglite_context_t *context_p);
void lx_vglite_set_event_callback(lx_vglite_context_t *ctx, lx_vglite_event_cb_t callback,
                                  uintptr_t user_data);

#if defined(__cplusplus)
}
#endif

#endif // LX_VGLITE_API_H
