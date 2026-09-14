/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include "gui_components_init.h"
#include "gui_fb.h"
#include "gui_obj.h"
#include "gui_server.h"
#include "test_cases.h"

typedef uint32_t (*test_case_func_t)(void);

static const test_case_func_t test_cases[] =
{
    test_case_00,
    test_case_01,
    test_case_02,
    test_case_03,
    test_case_04,
    test_case_05,
    test_case_06,
    test_case_07,
    test_case_08,
    test_case_09,
    test_case_10,
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
    /* The GUI server sleeps the frame loop after keep_active_time (5 s by
     * default) without touch/key input, which stops the root timer and stalls
     * the run part way through. */
    gui_set_keep_active_time(0xFFFFFFFF);

    /* Splits the number gui_fb_render_time_ms() reports.  gui_fb_draw() then
     * logs its own half ("fb draw time"), which is blit + QSPI flush, so
     * render_time - fb_draw_time is the prepare half, i.e. the one-off software
     * rasterisation of the shape buffers.  Also unlocks 4 one-shot
     * "[geom dirty]" lines showing the dirty-region count per frame. */
    // gui_get_dc()->draw_fb_measure_enable = true;

    gui_obj_create_timer(gui_obj_get_root(), 200, true, test_app_timer_cb);
    gui_obj_start_timer(gui_obj_get_root());

    return 0;
}

GUI_INIT_APP_EXPORT(test_init);
/* embedded test app end */
