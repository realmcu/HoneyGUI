/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include "gui_components_init.h"
#include "gui_fb.h"
#include "gui_obj.h"
#include "test_cases.h"

typedef uint32_t (*test_case_func_t)(void);

static const test_case_func_t test_cases[] =
{
    test_case_00,
    test_case_01,
};

static uint32_t test_case_index = 0;
static uint32_t expected_render_time_ms = 0;
static bool wait_for_render = false;

void test_app_timer_cb(void *param)
{
    if (!wait_for_render)
    {
        expected_render_time_ms = test_cases[test_case_index]();
        wait_for_render = true;
        return;
    }

    uint32_t actual_render_time_ms = gui_fb_render_time_ms();

    if (actual_render_time_ms > expected_render_time_ms)
    {
        gui_log("test_case_%02u render time: %u ms, expected: %u ms! --- FAIL ---\n",
                (unsigned int)test_case_index,
                (unsigned int)actual_render_time_ms,
                (unsigned int)expected_render_time_ms);
    }
    else
    {
        gui_log("test_case_%02u render time: %u ms, expected: %u ms! --- PASS ---\n",
                (unsigned int)test_case_index,
                (unsigned int)actual_render_time_ms,
                (unsigned int)expected_render_time_ms);
    }


    test_case_index++;
    if (test_case_index >= (sizeof(test_cases) / sizeof(test_cases[0])))
    {
        test_case_index = 0;
        wait_for_render = false;
        gui_obj_stop_timer((gui_obj_t *)param);
        return;
    }
    wait_for_render = false;
}

static int test_init(void)
{
    gui_obj_create_timer(gui_obj_get_root(), 1000, true, test_app_timer_cb);
    gui_obj_start_timer(gui_obj_get_root());

    return 0;
}

GUI_INIT_APP_EXPORT(test_init);
/* embedded test app end */
