/**
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file test_font_scroll_rendering.c
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
#define CURRENT_VIEW_NAME "test_font_scroll_rendering"
#define FONT_NUM 4

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
    left_view = gui_view_descriptor_get("test_font_bitmap_img_rendering");
    right_view = gui_view_descriptor_get("test_font_bitmap_normal_rendering");
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

static void ui_text_font(gui_view_t *view)
{
    struct gui_dispdev *dc = gui_get_dc();
    uint32_t font_size = 32;

    gui_scroll_text_t *scroll_text1 = gui_scroll_text_create(view, "scroll_text1", 50, 0, 200, 32);
    gui_scroll_text_set(scroll_text1, (void *)descriptor.name, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                        strlen(descriptor.name), font_size);
    gui_scroll_text_type_set(scroll_text1, font32b1, FONT_SRC_MEMADDR);
    gui_scroll_text_scroll_set(scroll_text1, SCROLL_X, 100, 100, 3000, 0);

    gui_scroll_text_t *scroll_text2 = gui_scroll_text_create(view, "scroll_text2", 50, 50, 200, 32);
    gui_scroll_text_set(scroll_text2, (void *)descriptor.name, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                        strlen(descriptor.name), font_size);
    gui_scroll_text_type_set(scroll_text2, font32b2, FONT_SRC_MEMADDR);
    gui_scroll_text_scroll_set(scroll_text2, SCROLL_X, 100, 100, 3000, 0);

    gui_scroll_text_t *scroll_text3 = gui_scroll_text_create(view, "scroll_text3", 50, 100, 200, 32);
    gui_scroll_text_set(scroll_text3, (void *)descriptor.name, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                        strlen(descriptor.name), font_size);
    gui_scroll_text_type_set(scroll_text3, font32b4, FONT_SRC_MEMADDR);
    gui_scroll_text_scroll_set(scroll_text3, SCROLL_X, 100, 100, 3000, 0);

    gui_scroll_text_t *scroll_text4 = gui_scroll_text_create(view, "scroll_text4", 50, 150, 200, 32);
    gui_scroll_text_set(scroll_text4, (void *)descriptor.name, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                        strlen(descriptor.name), font_size);
    gui_scroll_text_type_set(scroll_text4, font32b8, FONT_SRC_MEMADDR);
    gui_scroll_text_scroll_set(scroll_text4, SCROLL_X, 100, 100, 3000, 0);

    gui_scroll_text_t *scroll_text5 = gui_scroll_text_create(view, "scroll_text5", 50, 200, 200, 32);
    gui_scroll_text_set(scroll_text5, (void *)descriptor.name, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                        strlen(descriptor.name), font_size);
    gui_scroll_text_type_set(scroll_text5, font32vb4, FONT_SRC_MEMADDR);
    gui_scroll_text_scroll_set(scroll_text5, SCROLL_X, 100, 100, 3000, 0);


    gui_text_t *test_name = gui_text_create(view, "test_name", 0, dc->screen_height - font_size,
                                            dc->screen_width, font_size);
    gui_text_set(test_name, (void *)descriptor.name, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(descriptor.name), font_size);
    gui_text_type_set(test_name, font32b2, FONT_SRC_MEMADDR);

    gui_view_switch_on_event(view, left_view->name, SWITCH_OUT_TO_RIGHT_USE_CUBE,
                             SWITCH_IN_FROM_LEFT_USE_CUBE, GUI_EVENT_TOUCH_MOVE_RIGHT);
    gui_view_switch_on_event(view, right_view->name, SWITCH_OUT_TO_LEFT_USE_CUBE,
                             SWITCH_IN_FROM_RIGHT_USE_CUBE, GUI_EVENT_TOUCH_MOVE_LEFT);
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/


