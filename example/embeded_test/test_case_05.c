/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include "guidef.h"
#include "gui_api.h"
#include "gui_obj.h"
#include "gui_rect.h"
#include "test_cases.h"

/*
 * Calibration mode: run on the EVB, then set this to
 * max(P99 * 1.20, P99 + 2 ms) from at least 30 complete runs.
 */
#define TEST_CASE_05_EXPECTED_RENDER_TIME_MS 20U

#define TEST_CASE_05_CARD_COLUMNS 4
#define TEST_CASE_05_CARD_ROWS    5

uint32_t test_case_05(void)
{
    gui_dispdev_t *dc = gui_get_dc();
    const int margin = 12;
    const int gap = 8;
    const int card_width = (dc->screen_width - margin * 2 -
                            gap * (TEST_CASE_05_CARD_COLUMNS - 1)) / TEST_CASE_05_CARD_COLUMNS;
    const int card_height = (dc->screen_height - margin * 2 -
                             gap * (TEST_CASE_05_CARD_ROWS - 1)) / TEST_CASE_05_CARD_ROWS;

    gui_log("test_case_05 start!\n");

    gui_obj_child_free(gui_obj_get_root());
    gui_rect_create(gui_obj_get_root(), "background", 0, 0,
                    dc->screen_width, dc->screen_height, 0,
                    gui_rgba(16, 20, 28, 255));

    for (int row = 0; row < TEST_CASE_05_CARD_ROWS; row++)
    {
        for (int column = 0; column < TEST_CASE_05_CARD_COLUMNS; column++)
        {
            const int index = row * TEST_CASE_05_CARD_COLUMNS + column;
            gui_rounded_rect_t *card = gui_rect_create(
                                           gui_obj_get_root(), "card",
                                           margin + column * (card_width + gap),
                                           margin + row * (card_height + gap),
                                           card_width, card_height, 12,
                                           gui_rgba(35 + index * 4, 85 + index * 3,
                                                    145 + index * 2, 255));
            gui_rect_set_opacity(card, 220);
        }
    }

    gui_log("test_case_05 end!\n");

    return TEST_CASE_05_EXPECTED_RENDER_TIME_MS;
}
