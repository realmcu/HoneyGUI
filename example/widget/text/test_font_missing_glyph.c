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

/* Korean, deliberately absent from every font registered below. */
static const char *const missing_text = "ABC \xED\x95\x9C\xEA\xB5\xAD\xEC\x96\xB4 DEF";

/*============================================================================*
 *                           Private Functions
 *============================================================================*/

/**
 * @brief Create one labelled row showing a missing-glyph mode.
 *
 * @param name Widget name.
 * @param y Row top coordinate.
 * @param label Mode description drawn above the sample.
 * @param mode Missing-glyph mode applied to the sample widget.
 */
static void create_row(const char *name, int16_t y, const char *label,
                       FONT_SRC_TYPE type, void *font,
                       gui_missing_glyph_mode_t mode)
{
    gui_text_t *tag = gui_text_create(gui_obj_get_root(), name, 10, y, 460, FONT_SIZE);
    gui_text_set(tag, (void *)label, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(label), FONT_SIZE);
    gui_text_type_set(tag, fontnoto, FONT_SRC_MEMADDR);

    gui_text_t *sample = gui_text_create(gui_obj_get_root(), name,
                                         10, y + FONT_SIZE + 2, 460, FONT_SIZE);
    gui_text_set(sample, (void *)missing_text, type, APP_COLOR_WHITE,
                 strlen(missing_text), FONT_SIZE);
    gui_text_type_set(sample, font, FONT_SRC_MEMADDR);
    gui_text_set_missing_glyph_mode(sample, mode);
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

/**
 * @brief Test 18: Missing-glyph substitution
 *
 * Four rows render the same string containing Korean characters that exist in
 * no registered font:
 *   1. bitmap, SKIP        - characters vanish (legacy behavior, the default)
 *   2. bitmap, SUBSTITUTE  - NotoSans has U+FFFD, so boxes appear
 *   3. bitmap, INHERIT     - follows the global mode set below
 *   4. vector, SUBSTITUTE  - NotoSans vector lacks U+FFFD and U+25A1, so the
 *                            chain degrades to U+0020 and leaves a blank
 *
 * The bottom widget checks that word wrap still breaks on real spaces only.
 *
 * @note gui_text_substitute_chain_set() is global and read when glyphs load,
 *       not when widgets are created; call it during init only. Uncomment the
 *       line below to see every row degrade to a blank instead of a box.
 */
void text_missing_glyph_test(void)
{
    /* Latin only: no Korean glyph anywhere in either chain. */
    gui_font_mem_init(fontnoto);
    gui_font_set_priority((uint8_t *)fontnoto, 0);
    gui_font_ttf_init_mem(fontnotovec);
    gui_font_set_priority((uint8_t *)fontnotovec, 0);

    /* Space-only chain: use this when the fonts carry no box glyph. */
    // static const uint32_t space_only[] = {0x0020};
    // gui_text_substitute_chain_set(space_only, 1);

    /* Global mode drives row 3; rows 1, 2 and 4 override it per widget. */
    gui_text_set_missing_glyph_mode_global(GUI_MISSING_GLYPH_SUBSTITUTE);

    create_row("skip", 6, "BMP SKIP:",
               GUI_FONT_SRC_BMP, fontnoto, GUI_MISSING_GLYPH_SKIP);
    create_row("subst", 76, "BMP SUBSTITUTE:",
               GUI_FONT_SRC_BMP, fontnoto, GUI_MISSING_GLYPH_SUBSTITUTE);
    create_row("inherit", 146, "BMP INHERIT (global):",
               GUI_FONT_SRC_BMP, fontnoto, GUI_MISSING_GLYPH_INHERIT);
    create_row("ttf", 216, "TTF SUBSTITUTE:",
               GUI_FONT_SRC_TTF, fontnotovec, GUI_MISSING_GLYPH_SUBSTITUTE);

    /* Word wrap must still break on real spaces only: a substituted glyph keeps
     * its original code point, so it is never a break opportunity. */
    const char *wrap_text =
        "wrap \xED\x95\x9C\xEA\xB5\xAD\xEC\x96\xB4\xED\x95\x9C\xEA\xB5\xAD test wrapping here";
    gui_text_t *wrap = gui_text_create(gui_obj_get_root(), "wrap", 10, 292, 300, 120);
    gui_text_set(wrap, (void *)wrap_text, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(wrap_text), FONT_SIZE);
    gui_text_type_set(wrap, fontnoto, FONT_SRC_MEMADDR);
    gui_text_mode_set(wrap, MULTI_LEFT);
    gui_text_wordwrap_set(wrap, true);
    gui_text_set_missing_glyph_mode(wrap, GUI_MISSING_GLYPH_SUBSTITUTE);
}
