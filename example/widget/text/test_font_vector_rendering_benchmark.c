/**
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file test_font_vector_rendering_benchmark.c
  * @brief font rendering test
  * @details
  * @author luke_sun@realsil.com.cn
  * @date 2025/04/16
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2025 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "test_font.h"
/*============================================================================*
 *                           Types
 *============================================================================*/


/*============================================================================*
 *                           Constants
 *============================================================================*/


/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "test_font_vector_rendering_benchmark"
#define FONT_NUM 15

/*============================================================================*
 *                            Variables
 *============================================================================*/
static gui_view_t *current_view = NULL;
static const gui_view_descriptor_t *left_view = NULL;
static const gui_view_descriptor_t *right_view = NULL;

static void ui_text_font(gui_view_t *view);
static const gui_view_descriptor_t descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,

    .on_switch_in = ui_text_font,
    .on_switch_out = NULL,
};
static char *text_string[FONT_NUM] =
{
    "滕王高阁临江渚，佩玉鸣鸾罢歌舞。",
    "画栋朝飞南浦云，珠帘暮卷西山雨。",
    "闲云潭影日悠悠，物换星移几度秋。",
    "阁中帝子今何在？槛外长江空自流。",
    "滕王高阁临江渚，佩玉鸣鸾罢歌舞。",
    "画栋朝飞南浦云，珠帘暮卷西山雨。",
    "闲云潭影日悠悠，物换星移几度秋。",
    "阁中帝子今何在？槛外长江空自流。",
    "滕王高阁临江渚，佩玉鸣鸾罢歌舞。",
    "画栋朝飞南浦云，珠帘暮卷西山雨。",
    "闲云潭影日悠悠，物换星移几度秋。",
    "阁中帝子今何在？槛外长江空自流。",
    "滕王高阁临江渚，佩玉鸣鸾罢歌舞。",
    "画栋朝飞南浦云，珠帘暮卷西山雨。",
    "闲云潭影日悠悠，物换星移几度秋。",
};
/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static int gui_view_descriptor_register_init(void)
{
    gui_view_descriptor_register(&descriptor);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_REGISTER(gui_view_descriptor_register_init);

static int gui_view_get_other_view_descriptor_init(void)
{
    left_view = gui_view_descriptor_get("test_font_vector_rendering");
    right_view = gui_view_descriptor_get("test_font_bitmap_rendering_benchmark");
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

static void ui_text_font(gui_view_t *view)
{
    gui_text_t *text[FONT_NUM];
    struct gui_dispdev *dc = gui_get_dc();
    uint32_t font_size = 32;

    for (int i = 0; i < FONT_NUM; i++)
    {
        text[i] = gui_text_create(view, "text1", 0, font_size * i, 0, font_size);
        gui_text_set(text[i], text_string[i], GUI_FONT_SRC_TTF, APP_COLOR_WHITE, strlen(text_string[i]),
                     font_size);
        gui_text_type_set(text[i], font32vb4, FONT_SRC_MEMADDR);
        gui_text_rendermode_set(text[i], 2);
    }

    gui_text_t *test_name = gui_text_create(view, "test_name", 0, dc->screen_height - font_size,
                                            dc->screen_width, font_size);
    gui_text_set(test_name, (void *)descriptor.name, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                 strlen(descriptor.name), font_size);
    gui_text_type_set(test_name, font32vb4, FONT_SRC_MEMADDR);
    gui_text_rendermode_set(test_name, 2);

    gui_view_switch_on_event(view, left_view, SWITCH_OUT_TO_RIGHT_USE_CUBE,
                             SWITCH_IN_FROM_LEFT_USE_CUBE, GUI_EVENT_TOUCH_MOVE_RIGHT);
    gui_view_switch_on_event(view, right_view, SWITCH_OUT_TO_LEFT_USE_CUBE,
                             SWITCH_IN_FROM_RIGHT_USE_CUBE, GUI_EVENT_TOUCH_MOVE_LEFT);
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/


