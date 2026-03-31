/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

#include "gui_text.h"
#include "font_mem.h"
#include "font_ttf.h"
#include "font_lib_manager.h"
#include "test_font.h"

/*============================================================================*
 *                           Constants
 *============================================================================*/

#define FONT_SIZE 32

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

/**
 * @brief Test 14: Mixed-language font fallback
 *
 * Demonstrates BMP font fallback with priority chain.
 * A single text widget displays a string containing Chinese, Japanese,
 * Korean and English characters. The primary font is Chinese (HarmonyOS),
 * with NotoSansJP, NotoSansKR and NotoSans registered as fallbacks.
 */
void text_font_fallback_test(void)
{
    /* Register all fonts to font_lib (gui_text_type_set auto-registers for MEMADDR) */
    gui_font_mem_init(fontnoto);
    gui_font_mem_init(fontharmonysc);
    gui_font_mem_init(fontnotojp);
    gui_font_mem_init(fontnotokr);
    gui_font_mem_init(fontnotothai);

    /* Set fallback priority: Chinese(0) > Japanese(1) > Korean(2) > Thai(3) > English(4) */
    gui_font_set_priority((uint8_t *)fontnoto, 0);
    gui_font_set_priority((uint8_t *)fontharmonysc, 1);
    gui_font_set_priority((uint8_t *)fontnotojp, 2);
    gui_font_set_priority((uint8_t *)fontnotokr, 3);
    gui_font_set_priority((uint8_t *)fontnotothai, 4);

    /* "Hello World" in: Chinese + Japanese + Korean + Thai + English */
    char *mixed_text =
        "你好世界\nこんにちは世界\n안녕하세요 세계\nสวัสดีชาวโลก\nHello World";

    gui_text_t *text = gui_text_create(gui_obj_get_root(), "fallback_bmp",
                                       10, 10, 460, 200);
    gui_text_set(text, mixed_text, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(mixed_text), FONT_SIZE);
    gui_text_type_set(text, fontnoto, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, MULTI_LEFT);

    /* --- Vector (TTF) fallback --- */
    gui_font_ttf_init_mem(fontnotovec);
    gui_font_ttf_init_mem(fontnotoscvec);
    gui_font_ttf_init_mem(fontnotojpvec);
    gui_font_ttf_init_mem(fontnotokrvec);

    gui_font_set_priority((uint8_t *)fontnotovec, 0);
    gui_font_set_priority((uint8_t *)fontnotoscvec, 1);
    gui_font_set_priority((uint8_t *)fontnotojpvec, 2);
    gui_font_set_priority((uint8_t *)fontnotokrvec, 3);

    char *mixed_text_ttf = "你好世界 こんにちは 한국어 Hello";

    gui_text_t *text_ttf = gui_text_create(gui_obj_get_root(), "fallback_ttf",
                                           10, 220, 460, 200);
    gui_text_set(text_ttf, mixed_text_ttf, GUI_FONT_SRC_TTF, APP_COLOR_WHITE,
                 strlen(mixed_text_ttf), FONT_SIZE);
    gui_text_type_set(text_ttf, fontnotovec, FONT_SRC_MEMADDR);
    gui_text_mode_set(text_ttf, MULTI_LEFT);
}
