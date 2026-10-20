/**
 * @file hml_label.c
 * @brief HML Label Component (hg_label)
 *
 * Attribute mapping follows the designer's LabelGenerator. gui_text keeps the
 * pointers it is given: the text points into the resident XML tree and the
 * font path is pooled.
 */

#include "../hml_component.h"
#include "../hml_utils.h"
#include "gui_text.h"
#include "gui_scroll_text.h"
#include "gui_api.h"
#include <string.h>

static TEXT_MODE hml_label_mode(ezxml_t node)
{
    const char *h = hml_attr_str(node, "hAlign", "LEFT");
    const char *v = hml_attr_str(node, "vAlign", "TOP");
    bool multi = hml_attr_bool(node, "wordWrap", false);
    bool mid = (strcmp(v, "MID") == 0 || strcmp(v, "MIDDLE") == 0);
    int col = !strcmp(h, "CENTER") ? 1 : (!strcmp(h, "RIGHT") ? 2 : 0);

    static const TEXT_MODE table[2][2][3] =
    {
        { { LEFT, CENTER, RIGHT },             { MULTI_LEFT, MULTI_CENTER, MULTI_RIGHT } },
        { { MID_LEFT, MID_CENTER, MID_RIGHT }, { MULTI_MID_LEFT, MULTI_MID_CENTER, MULTI_MID_RIGHT } },
    };
    return table[mid ? 1 : 0][multi ? 1 : 0][col];
}

static TEXT_MODE hml_scroll_mode(ezxml_t node)
{
    bool reverse = hml_attr_bool(node, "scrollReverse", false);
    if (strcmp(hml_attr_str(node, "scrollDirection", "horizontal"), "horizontal") != 0)
    {
        return reverse ? SCROLL_Y_REVERSE : SCROLL_Y;
    }
    const char *v = hml_attr_str(node, "vAlign", "TOP");
    if (strcmp(v, "MID") == 0)
    {
        return reverse ? SCROLL_X_MID_REVERSE : SCROLL_X_MID;
    }
    return reverse ? SCROLL_X_REVERSE : SCROLL_X;
}

gui_obj_t *hml_create_label(gui_obj_t *parent, ezxml_t node)
{
    const char *id = hml_attr_str(node, "id", "label");
    int x = hml_attr_int(node, "x", 0);
    int y = hml_attr_int(node, "y", 0);
    int w = hml_attr_width(node, 100);
    int h = hml_attr_height(node, 30);
    const char *text = hml_attr_str(node, "text", "");
    int font_size = hml_attr_int(node, "fontSize", 16);
    gui_color_t color = hml_attr_color(node, "color", 0xFFFFFFFF);
    bool vector = strcmp(hml_attr_str(node, "fontType", "bitmap"), "vector") == 0;
    FONT_SRC_TYPE src_type = vector ? GUI_FONT_SRC_TTF : GUI_FONT_SRC_BMP;
    bool scroll = hml_attr_bool(node, "enableScroll", false);
    uint16_t len = (uint16_t)strlen(text);

    char font_buf[160];
    const char *font = NULL;
    if (hml_resolve_font_path(node, font_buf, sizeof(font_buf)))
    {
        font = hml_strpool_intern(font_buf);
    }

    gui_text_t *txt;
    if (scroll)
    {
        gui_scroll_text_t *st = gui_scroll_text_create(parent, id, x, y, w, h);
        if (!st) { return NULL; }
        gui_scroll_text_set(st, (void *)text, src_type, color, len, (uint16_t)font_size);
        if (font) { gui_scroll_text_type_set(st, (void *)font, FONT_SRC_FILESYS); }
        gui_scroll_text_scroll_set(st, hml_scroll_mode(node),
                                   (uint16_t)hml_attr_int(node, "scrollStartOffset", 0),
                                   (uint16_t)hml_attr_int(node, "scrollEndOffset", 0),
                                   (uint16_t)hml_attr_int(node, "scrollInterval", 3000),
                                   (uint32_t)hml_attr_int(node, "scrollDuration", 0));
        txt = (gui_text_t *)st;
    }
    else
    {
        txt = gui_text_create(parent, id, x, y, w, h);
        if (!txt) { return NULL; }
        gui_text_set(txt, (void *)text, src_type, color, len, (uint16_t)font_size);
        if (font) { gui_text_type_set(txt, (void *)font, FONT_SRC_FILESYS); }
        gui_text_mode_set(txt, hml_label_mode(node));
    }

    int letter = hml_attr_int(node, "letterSpacing", 0);
    if (letter != 0) { gui_text_extra_letter_spacing_set(txt, (int8_t)letter); }
    int line = hml_attr_int(node, "lineSpacing", 0);
    if (line != 0) { gui_text_extra_line_spacing_set(txt, (int8_t)line); }
    if (hml_attr_bool(node, "wordBreak", false)) { gui_text_wordwrap_set(txt, true); }

    return (gui_obj_t *)txt;
}
