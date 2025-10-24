/**
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file test_font_scroll_function.c
  * @brief font scroll function test
  * @details
  * @author luke_sun@realsil.com.cn
  * @date 2025/10/24
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


/*============================================================================*
 *                            Variables
 *============================================================================*/

gui_scroll_text_t *scroll_x;
gui_scroll_text_t *scroll_y;
gui_scroll_text_t *scroll_x_reverse;
gui_scroll_text_t *scroll_y_reverse;

/*============================================================================*
 *                           Private Functions
 *============================================================================*/

static void stop_cb(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(param);
    GUI_UNUSED(e);
    GUI_UNUSED(obj);

    gui_scroll_text_stop(scroll_x);
    gui_scroll_text_stop(scroll_y);
    gui_scroll_text_stop(scroll_x_reverse);
    gui_scroll_text_stop(scroll_y_reverse);
    gui_log("stop_cb\n");
}

static void reset_cb(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(param);
    GUI_UNUSED(e);
    GUI_UNUSED(obj);

    gui_scroll_text_reset(scroll_x);
    gui_scroll_text_reset(scroll_y);
    gui_scroll_text_reset(scroll_x_reverse);
    gui_scroll_text_reset(scroll_y_reverse);
    gui_log("reset_cb\n");
}

static void pause_cb(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(param);
    GUI_UNUSED(e);
    GUI_UNUSED(obj);

    gui_scroll_text_pause(scroll_x);
    gui_scroll_text_pause(scroll_y);
    gui_scroll_text_pause(scroll_x_reverse);
    gui_scroll_text_pause(scroll_y_reverse);
    gui_log("pause_cb\n");
}

static void resume_cb(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(param);
    GUI_UNUSED(e);
    GUI_UNUSED(obj);

    gui_scroll_text_resume(scroll_x);
    gui_scroll_text_resume(scroll_y);
    gui_scroll_text_resume(scroll_x_reverse);
    gui_scroll_text_resume(scroll_y_reverse);
    gui_log("resume_cb\n");
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

void text_font_scroll_function_test(void)
{
    char *test_text =
        "Everyone has the right to freedom of thought, conscience and religion; this right includes freedom to change his religion or belief, and freedom. ";

    scroll_x = gui_scroll_text_create(gui_obj_get_root(), "scroll_text", 40, 0, 400, 0);
    gui_scroll_text_set(scroll_x, test_text, GUI_FONT_SRC_BMP, APP_COLOR_WHITE, strlen(test_text), 32);
    gui_scroll_text_scroll_set(scroll_x, SCROLL_X, 200, 200, 10000, 0);
    gui_scroll_text_type_set(scroll_x, font32b2, FONT_SRC_MEMADDR);

    scroll_x_reverse = gui_scroll_text_create(gui_obj_get_root(), "scroll_text", 40, 32, 400, 0);
    gui_scroll_text_set(scroll_x_reverse, test_text, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                        strlen(test_text), 32);
    gui_scroll_text_scroll_set(scroll_x_reverse, SCROLL_X_REVERSE, 200, 200, 10000, 0);
    gui_scroll_text_type_set(scroll_x_reverse, font32b2, FONT_SRC_MEMADDR);

    scroll_y = gui_scroll_text_create(gui_obj_get_root(), "scroll_text", 40, 100, 400, 100);
    gui_scroll_text_set(scroll_y, test_text, GUI_FONT_SRC_BMP, APP_COLOR_WHITE, strlen(test_text), 32);
    gui_scroll_text_scroll_set(scroll_y, SCROLL_Y, 64, 0, 10000, 0);
    gui_scroll_text_type_set(scroll_y, font32b2, FONT_SRC_MEMADDR);
    gui_text_wordwrap_set(&scroll_y->base, true);

    scroll_y_reverse = gui_scroll_text_create(gui_obj_get_root(), "scroll_text", 40, 220, 400, 100);
    gui_scroll_text_set(scroll_y_reverse, test_text, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                        strlen(test_text), 32);
    gui_scroll_text_scroll_set(scroll_y_reverse, SCROLL_Y_REVERSE, 32, 32, 10000, 0);
    gui_scroll_text_type_set(scroll_y_reverse, font32b2, FONT_SRC_MEMADDR);
    gui_text_wordwrap_set(&scroll_y->base, true);




    gui_text_t *stop = gui_text_create(gui_obj_get_root(), "stop", 100, 400, 0, 0);
    gui_text_set(stop, "STOP", GUI_FONT_SRC_BMP, APP_COLOR_TOMATO, 4, 32);
    gui_text_type_set(stop, font32b2, FONT_SRC_MEMADDR);
    gui_obj_add_event_cb(stop, (gui_event_cb_t)stop_cb, GUI_EVENT_TOUCH_CLICKED, NULL);

    gui_text_t *reset = gui_text_create(gui_obj_get_root(), "start", 100, 440, 0, 0);
    gui_text_set(reset, "RESET", GUI_FONT_SRC_BMP, APP_COLOR_TOMATO, 6, 32);
    gui_text_type_set(reset, font32b2, FONT_SRC_MEMADDR);
    gui_obj_add_event_cb(reset, (gui_event_cb_t)reset_cb, GUI_EVENT_TOUCH_CLICKED, NULL);

    gui_text_t *pause = gui_text_create(gui_obj_get_root(), "pause", 300, 400, 0, 0);
    gui_text_set(pause, "PAUSE", GUI_FONT_SRC_BMP, APP_COLOR_CYAN, 5, 32);
    gui_text_type_set(pause, font32b2, FONT_SRC_MEMADDR);
    gui_obj_add_event_cb(pause, (gui_event_cb_t)pause_cb, GUI_EVENT_TOUCH_CLICKED, NULL);

    gui_text_t *resume = gui_text_create(gui_obj_get_root(), "resume", 300, 440, 0, 0);
    gui_text_set(resume, "RESUME", GUI_FONT_SRC_BMP, APP_COLOR_CYAN, 6, 32);
    gui_text_type_set(resume, font32b2, FONT_SRC_MEMADDR);
    gui_obj_add_event_cb(resume, (gui_event_cb_t)resume_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
}
