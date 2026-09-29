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

/* No emoji path is set below, so these resolve as missing glyphs. U+1F602 is a
 * lone SMP code point; the rainbow flag is a 4 code point ZWJ sequence that must
 * collapse to one cell, not one per component. */
#define EMOJI_JOY           "\xF0\x9F\x98\x82"
#define EMOJI_RAINBOW_FLAG  "\xF0\x9F\x8F\xB3\xEF\xB8\x8F\xE2\x80\x8D\xF0\x9F\x8C\x88"
static const char *const emoji_skip_text =
    "Emoji SKIP: ab " EMOJI_JOY " cd " EMOJI_RAINBOW_FLAG " ef";
static const char *const emoji_subst_text =
    "Emoji SUBST: ab " EMOJI_JOY " cd " EMOJI_RAINBOW_FLAG " ef";

/*============================================================================*
 *                           Private Functions
 *============================================================================*/

/**
 * @brief Create one labelled row showing a missing-glyph mode.
 *
 * @param name Widget name.
 * @param y Row top coordinate.
 * @param label Mode description drawn above the sample.
 * @param sample_text Content rendered under the label.
 * @param mode Missing-glyph mode applied to the sample widget.
 */
static void create_row(const char *name, int16_t y, const char *label,
                       const char *sample_text, FONT_SRC_TYPE type, void *font,
                       gui_missing_glyph_mode_t mode)
{
    gui_text_t *tag = gui_text_create(gui_obj_get_root(), name, 10, y, 460, FONT_SIZE);
    gui_text_set(tag, (void *)label, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(label), FONT_SIZE);
    gui_text_type_set(tag, fontnoto, FONT_SRC_MEMADDR);

    gui_text_t *sample = gui_text_create(gui_obj_get_root(), name,
                                         10, y + FONT_SIZE + 2, 460, FONT_SIZE);
    gui_text_set(sample, (void *)sample_text, type, APP_COLOR_WHITE,
                 strlen(sample_text), FONT_SIZE);
    gui_text_type_set(sample, font, FONT_SRC_MEMADDR);
    gui_text_set_missing_glyph_mode(sample, mode);
}

/**
 * @brief Create one bitmap row whose label is part of the sample text.
 *
 * @param name Widget name.
 * @param y Row top coordinate.
 * @param sample_text Content including its own inline label.
 * @param mode Missing-glyph mode applied to the widget.
 */
static void create_single(const char *name, int16_t y, const char *sample_text,
                          gui_missing_glyph_mode_t mode)
{
    gui_text_t *sample = gui_text_create(gui_obj_get_root(), name, 10, y, 460, FONT_SIZE);
    gui_text_set(sample, (void *)sample_text, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(sample_text), FONT_SIZE);
    gui_text_type_set(sample, fontnoto, FONT_SRC_MEMADDR);
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
 * Two more rows render emoji without an emoji path, which makes them ordinary
 * missing glyphs: they vanish under SKIP and become boxes under SUBSTITUTE.
 * Both rows must show the same cell count, one per emoji: the ZWJ rainbow flag
 * collapses to a single cell rather than one per component.
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

    create_row("skip", 6, "BMP SKIP:", missing_text,
               GUI_FONT_SRC_BMP, fontnoto, GUI_MISSING_GLYPH_SKIP);
    create_row("subst", 76, "BMP SUBSTITUTE:", missing_text,
               GUI_FONT_SRC_BMP, fontnoto, GUI_MISSING_GLYPH_SUBSTITUTE);
    create_row("inherit", 146, "BMP INHERIT (global):", missing_text,
               GUI_FONT_SRC_BMP, fontnoto, GUI_MISSING_GLYPH_INHERIT);
    create_row("ttf", 216, "TTF SUBSTITUTE:", missing_text,
               GUI_FONT_SRC_TTF, fontnotovec, GUI_MISSING_GLYPH_SUBSTITUTE);

    /* No emoji path configured: emoji are just code points no font carries, so
     * they follow the same mode as any other missing glyph. Label is inline to
     * keep both rows on screen. */
    create_single("emoji_skip", 286, emoji_skip_text, GUI_MISSING_GLYPH_SKIP);
    create_single("emoji_subst", 320, emoji_subst_text, GUI_MISSING_GLYPH_SUBSTITUTE);

    /* Word wrap must still break on real spaces only: a substituted glyph keeps
     * its original code point, so it is never a break opportunity. */
    const char *wrap_text =
        "wrap \xED\x95\x9C\xEA\xB5\xAD\xEC\x96\xB4\xED\x95\x9C\xEA\xB5\xAD test wrapping here";
    gui_text_t *wrap = gui_text_create(gui_obj_get_root(), "wrap", 10, 360, 300, 110);
    gui_text_set(wrap, (void *)wrap_text, GUI_FONT_SRC_BMP, APP_COLOR_WHITE,
                 strlen(wrap_text), FONT_SIZE);
    gui_text_type_set(wrap, fontnoto, FONT_SRC_MEMADDR);
    gui_text_mode_set(wrap, MULTI_LEFT);
    gui_text_wordwrap_set(wrap, true);
    gui_text_set_missing_glyph_mode(wrap, GUI_MISSING_GLYPH_SUBSTITUTE);
}
