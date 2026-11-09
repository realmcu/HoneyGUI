/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "gui_text.h"
#include "font_mem.h"
#include "tp_algo.h"
#include "gui_fb.h"
#include "gui_vfs.h"

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
static gui_missing_glyph_mode_t font_missing_mode = GUI_MISSING_GLYPH_SKIP;

/* Run-time overrideable substitute chain (default FFFD, 25A1, space).
 * Rest of the array is zero-initialized, i.e. 0x0000-terminated. */
uint32_t gui_text_substitute_chain[GUI_TEXT_SUBSTITUTE_CHAIN_MAX + 1] = {0xFFFD, 0x25A1, 0x0020};


/*============================================================================*
 *                           Private Functions
 *============================================================================*/

static void gui_text_font_load(gui_text_t *text, gui_text_rect_t *rect)
{
    switch (text->font_type)
    {
    case GUI_FONT_SRC_BMP:
        {
            gui_font_mem_load(text, rect);
        }
        break;

    default:
        gui_log("Unsupported font source type: %d\n", text->font_type);
        break;
    }
}

static void gui_text_font_draw(gui_text_t *text, gui_text_rect_t *rect)
{
    switch (text->font_type)
    {
    case GUI_FONT_SRC_BMP:
        {
            gui_font_mem_draw(text, rect);
        }
        break;

    default:
        gui_log("Unsupported font source type: %d\n", text->font_type);
        break;
    }
}

static void gui_text_font_unload(gui_text_t *text)
{
    switch (text->font_type)
    {
    case GUI_FONT_SRC_BMP:
        {
            // gui_font_mem_unload(text);
        }
        break;

    default:
        gui_log("Unsupported font source type: %d\n", text->font_type);
        break;
    }
}

static void gui_text_font_destroy(gui_text_t *text)
{
    switch (text->font_type)
    {
    case GUI_FONT_SRC_BMP:
        {
            gui_font_mem_destroy(text);
        }
        break;

    default:
        gui_log("Unsupported font source type: %d\n", text->font_type);
        break;
    }
}

static void gui_text_resize_rect(gui_text_t *text, gui_text_rect_t *rect)
{
    if (text->thai)
    {
        rect->y1 -= text->font_height / 2;
        rect->y2 += text->font_height / 2;
    }
}

static void gui_text_input_process(gui_obj_t *obj)
{
    touch_info_t *tp = tp_get_info();
    gui_text_t *this = (gui_text_t *)obj;
    GUI_UNUSED(tp);
    GUI_UNUSED(this);

}
static void gui_text_prepare(gui_obj_t *obj)
{
    gui_text_t *this = (void *)obj;
    uint8_t last;

    /* --- Lightweight section: always runs (even when hidden) --- */
    this->offset_x = obj->draw_x;
    this->offset_y = obj->draw_y;

    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_PRESSED, "touch");
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_RELEASED, "touch");
    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_CLICKED, "touch");

    last = this->checksum;
    this->checksum = 0;
    this->checksum = gui_obj_checksum(0, (uint8_t *)this, sizeof(gui_text_t));

    if (last != this->checksum || this->content_refresh)
    {
        gui_fb_change();
    }

    /* --- Heavy section: skip when hidden to avoid wasted work --- */
    if (obj->hidden)
    {
        return;
    }

    if (this->len > 0)
    {
        gui_text_rect_t rect;
        rect.x1 = this->offset_x;
        rect.y1 = this->offset_y;
        rect.x2 = rect.x1 + obj->w - 1;
        rect.y2 = rect.y1 + obj->h - 1;
        gui_text_font_load(this, &rect);
    }
}

static void gui_text_read_scope(gui_text_t *text, gui_text_rect_t *rect)
{
    text->scope = 0;
    gui_obj_t *o = (gui_obj_t *)text;

    while (o->parent != NULL)
    {
        o = o->parent;
        if ((o->type == WINDOW && GUI_TYPE(gui_win_t, o)->scope) ||
            o->type == LIST || o->type == LIST_NOTE)
        {
            text->scope = 1;
            rect->xboundleft = _UI_MAX(o->draw_x, rect->xboundleft);
            rect->xboundright = _UI_MIN(o->draw_x + o->w - 1, rect->xboundright);
            rect->yboundtop = _UI_MAX(o->draw_y, rect->yboundtop);
            rect->yboundbottom = _UI_MIN(o->draw_y + o->h - 1, rect->yboundbottom);
        }
    }
}

static void gui_text_draw(gui_obj_t *obj)
{
    gui_text_t *text = (gui_text_t *)obj;
    struct gui_dispdev *dc;
    gui_text_rect_t draw_rect = {0};

    if (text->len == 0 || text->data == NULL)
    {
        text->content_refresh = false;
        text->layout_refresh = false;
        return;
    }

    dc = gui_get_dc();
    draw_rect.x1 = text->offset_x;
    draw_rect.y1 = text->offset_y;
    draw_rect.x2 = draw_rect.x1 + obj->w - 1;
    draw_rect.y2 = draw_rect.y1 + obj->h - 1;

    draw_rect.xboundleft = draw_rect.x1;
    draw_rect.xboundright = draw_rect.x2;
    draw_rect.yboundtop = draw_rect.y1;
    draw_rect.yboundbottom = draw_rect.y2;

    gui_text_resize_rect(text, &draw_rect);

    gui_text_read_scope(text, &draw_rect);
    if (text->scope_self)
    {
        int16_t ox = text->scope_absolute ? 0 : text->offset_x;
        int16_t oy = text->scope_absolute ? 0 : text->offset_y;
        draw_rect.xboundleft = _UI_MAX(ox + text->scope_rect.x1, draw_rect.xboundleft);
        draw_rect.xboundright = _UI_MIN(ox + text->scope_rect.x2, draw_rect.xboundright);
        draw_rect.yboundtop = _UI_MAX(oy + text->scope_rect.y1, draw_rect.yboundtop);
        draw_rect.yboundbottom = _UI_MIN(oy + text->scope_rect.y2, draw_rect.yboundbottom);
        text->scope = 1;
    }
    if (text->scope ?
        gui_text_scope_rect_hit(&draw_rect, &dc->section) :
        gui_text_rect_hit(&draw_rect, &dc->section))
    {
        gui_text_font_draw(text, &draw_rect);
    }
}

static void gui_text_end(gui_obj_t *obj)
{
    gui_text_t *text = (gui_text_t *)obj;
    gui_text_font_unload(text);
}

static void gui_text_destroy(gui_obj_t *obj)
{
    gui_text_t *text = (gui_text_t *)obj;

    gui_text_font_destroy(text);
}

static void gui_text_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_INPUT_PROCESS:
            gui_text_input_process(obj);
            break;
        case OBJ_PRE_PROCESS:
            gui_text_prepare(obj);
            break;

        case OBJ_PROCESS:
            gui_text_draw(obj);
            break;

        case OBJ_POST_PROCESS:
            gui_text_end(obj);
            break;

        case OBJ_DESTROY:
            gui_text_destroy(obj);
            break;

        default:
            break;
        }
    }
}

void gui_text_ctor(gui_text_t *this,
                   gui_obj_t  *parent,
                   const char *name,
                   int16_t     x,
                   int16_t     y,
                   int16_t     w,
                   int16_t     h)
{
    gui_obj_t *root = (gui_obj_t *)this;
    w = (w == 0 ? parent->w : w);
    h = (h == 0 ? parent->h : h);
    gui_obj_ctor(root, parent, name, x, y, w, h);

    root->type = TEXTBOX;
    root->obj_cb = gui_text_cb;
    root->has_input_process_cb = true;
    root->has_pre_process_cb = true;
    root->has_process_cb = true;
    root->has_post_process_cb = true;
    root->has_destroy_cb = true;
    //for self
    this->mode = LEFT;
    this->inputable = false;
    this->font_mode = FONT_SRC_MEMADDR;
    this->content_refresh = false;
    this->layout_refresh = false;
    this->rendermode = 2;
}

static TEXT_MODE transfer_to_rtl_mode(TEXT_MODE mode)
{
    switch (mode)
    {
    case LEFT:
        return RTL_LEFT;
    case CENTER:
        return RTL_CENTER;
    case RIGHT:
        return RTL_RIGHT;
    case MULTI_LEFT:
        return RTL_MULTI_LEFT;
    case MULTI_CENTER:
        return RTL_MULTI_CENTER;
    case MULTI_RIGHT:
        return RTL_MULTI_RIGHT;
    default:
        return mode;
    }
}

static void gui_text_multilanguage_check(gui_text_t *this)
{
    uint32_t *unicode_buf = NULL;
    uint16_t unicode_len = 0;
    unicode_len = process_content_by_charset(this->charset, this->content, this->len, &unicode_buf);
    if (unicode_len == 0)
    {
        if (unicode_buf)
        {
            gui_free(unicode_buf);
        }
        return;
    }
    if (content_has_ap_unicode(unicode_buf, unicode_len))
    {
        this->arabic = true;
        this->mode = transfer_to_rtl_mode(this->mode);
    }
    else if (content_has_thai_unicode(unicode_buf, unicode_len))
    {
        this->thai = true;
    }
    else if (content_has_hebrew_unicode(unicode_buf, unicode_len))
    {
        this->hebrew = true;
        this->mode = transfer_to_rtl_mode(this->mode);
    }
    if (unicode_buf)
    {
        gui_free(unicode_buf);
    }
}
/*============================================================================*
 *                           Public Functions
 *============================================================================*/

void gui_text_click(gui_text_t *this, gui_event_cb_t event_cb, void *parameter)
{
    (void)this;
    (void)event_cb;
    (void)parameter;
    //gui_obj_add_event_cb(this, event_cb, (gui_event_code_t)TXT_EVENT_CLICK, parameter);
    GUI_ASSERT(0);
}
void gui_text_pswd_done(gui_text_t *this, gui_event_cb_t event_cb, void *parameter)
{
    (void)this;
    (void)event_cb;
    (void)parameter;
    //gui_obj_add_event_cb(this, event_cb, (gui_event_code_t)TXT_EVENT_PSWD_DONE, parameter);
    GUI_ASSERT(0);
}

void gui_text_set(gui_text_t    *this,
                  void          *text,
                  FONT_SRC_TYPE  text_type,
                  gui_color_t    color,
                  uint16_t       length,
                  uint16_t       font_size)
{
    this->font_type = text_type;
    this->color = color;
    this->font_height = font_size;
    gui_text_content_set(this, text, length);
}


void gui_text_mode_set(gui_text_t *this, TEXT_MODE mode)
{
    this->mode = mode;
    this->layout_refresh = true;
}

void gui_text_input_set(gui_text_t *this, bool inputable)
{
    this->inputable = inputable;
}

void gui_text_color_set(gui_text_t *this, gui_color_t color)
{
    this->color = color;
}

void gui_text_wordwrap_set(gui_text_t *this, bool wordwrap)
{
    this->wordwrap = wordwrap;
}

void gui_text_extra_letter_spacing_set(gui_text_t *this, int8_t extra_letter_spacing)
{
    this->extra_letter_spacing = extra_letter_spacing;
}
void gui_text_extra_line_spacing_set(gui_text_t *this, int8_t extra_line_spacing)
{
    this->extra_line_spacing = extra_line_spacing;
}

void gui_text_set_line_height(gui_text_t *this, int16_t line_height)
{
    this->line_height = line_height;
}

void gui_text_set_space_width(gui_text_t *this, uint16_t space_width)
{
    GUI_ASSERT(this != NULL);
    this->space_width = space_width;
    this->layout_refresh = true;
}

void gui_text_set_missing_glyph_mode_global(gui_missing_glyph_mode_t mode)
{
    if (mode != GUI_MISSING_GLYPH_INHERIT)
    {
        font_missing_mode = mode;
    }
}

void gui_text_set_missing_glyph_mode(gui_text_t *this, gui_missing_glyph_mode_t mode)
{
    GUI_ASSERT(this != NULL);
    if (this->missing_glyph_mode != (uint8_t)mode)
    {
        /* Substitution changes which cells exist, so chr[] needs a rebuild. */
        this->content_refresh = true;
    }
    this->missing_glyph_mode = (uint8_t)mode;
}

bool gui_text_substitute_enabled(const gui_text_t *text)
{
    gui_missing_glyph_mode_t mode = (text != NULL && text->missing_glyph_mode != 0)
                                    ? (gui_missing_glyph_mode_t)text->missing_glyph_mode
                                    : font_missing_mode;
    return mode == GUI_MISSING_GLYPH_SUBSTITUTE;
}

void gui_text_substitute_chain_set(const uint32_t *cps, uint8_t n)
{
    if (cps == NULL || n == 0)
    {
        return;
    }
    if (n > GUI_TEXT_SUBSTITUTE_CHAIN_MAX)
    {
        n = GUI_TEXT_SUBSTITUTE_CHAIN_MAX;
    }
    memcpy(gui_text_substitute_chain, cps, n * sizeof(uint32_t));
    gui_text_substitute_chain[n] = 0x0000;
}

void gui_text_rendermode_set(gui_text_t *this, uint8_t rendermode)
{
    uint8_t mode = this->rendermode;

    switch (rendermode)
    {
    case 1:
        mode = 0;
        break;
    case 2:
        mode = 1;
        break;
    case 4:
        mode = 2;
        break;
    case 8:
        mode = 3;
        break;

    default:
        break;
    };

    if (this->rendermode != mode)
    {
        /* raster_prec drives the glyph bitmap size, so a resident cross-frame
         * cache would be read back at the wrong dimensions. Force a rebuild. */
        this->content_refresh = true;
    }
    this->rendermode = mode;
}

void gui_text_bold_set(gui_text_t *this, uint8_t bold_weight, BOLD_MODE bold_mode)
{
    GUI_ASSERT(this != NULL);
    if (this->bold_weight != bold_weight || this->bold_mode != bold_mode)
    {
        /* Dilation margin grows the glyph bitmap, so a resident cross-frame cache
         * would be read back at the wrong dimensions. Force a rebuild. */
        this->content_refresh = true;
    }
    this->bold_weight = bold_weight;
    this->bold_mode = bold_mode;
    this->layout_refresh = true;
}

void gui_text_move(gui_text_t *this, int16_t x, int16_t y)
{
    this->base.x = x;
    this->base.y = y;
    this->layout_refresh = true;
}

void gui_text_size_set(gui_text_t *this, uint16_t height, uint16_t width)
{
    (void)width;
    this->font_height = height;
    this->content_refresh = true;
    this->layout_refresh = true;
}

void gui_text_font_mode_set(gui_text_t *this, FONT_SRC_MODE font_mode)
{
    this->font_mode = font_mode;
}

void gui_text_type_set(gui_text_t *this, void *font_source, FONT_SRC_MODE font_mode)
{
    if (font_mode == FONT_SRC_MEMADDR)
    {
        uint8_t flag = *((uint8_t *)font_source + 1);
        if (flag == FONT_FILE_BMP_FLAG)
        {
            gui_font_mem_init_mem(font_source);
        }
        else
        {
            gui_log("Unsupported font file type: %d\n", flag);
        }
    }
    else if (font_mode == FONT_SRC_FTL)
    {
        uint8_t flag = 0;
        gui_ftl_read((uintptr_t)(uint8_t *)font_source + 1, &flag, sizeof(uint8_t));
        if (flag == FONT_FILE_BMP_FLAG)
        {
            gui_font_mem_init_ftl(font_source);
        }
        else
        {
            gui_log("Unsupported font file type: %d\n", flag);
        }
    }
    else if (font_mode == FONT_SRC_FILESYS)
    {
        /* Auto-initialize FS font (lazy loading with ref counting) */
        gui_vfs_file_t *file = gui_vfs_open((const char *)font_source, GUI_VFS_READ);
        if (file != NULL)
        {
            uint8_t header[2];
            gui_vfs_read(file, header, 2);
            gui_vfs_close(file);

            if (header[1] == FONT_FILE_BMP_FLAG)
            {
                gui_font_mem_init_fs(font_source);
            }
            else
            {
                gui_log("Unsupported font file type: %d\n", header[1]);
            }
        }

        /* An XIP-mapped font is registered by address; render it as MEMADDR. */
        FONT_LIB_NODE *node = gui_font_lib_find((uint8_t *)font_source);
        if (node != NULL && node->src_mode == FONT_SRC_MEMADDR)
        {
            font_source = node->font_file;
            font_mode = FONT_SRC_MEMADDR;
        }
    }
    this->path = font_source;
    this->font_mode = font_mode;
}

void gui_text_encoding_set(gui_text_t *this, TEXT_CHARSET charset)
{
    this->charset = charset;
}

void gui_text_set_cache(gui_text_t *this, bool enable)
{
    this->font_cache_enable = enable;
    /* Force a full rebuild so the policy applies next frame; the rebuild frees
     * any resident static cache and nulls chr[].buf, so font_cache_static
     * self-heals (NULL buf short-circuits the hit path). */
    this->content_refresh = true;
}

void gui_text_content_set(gui_text_t *this, void *text, uint16_t length)
{
    this->content = (uint8_t *)text;
    this->len = length;
    this->char_width_sum = 0;
    this->char_line_sum = 0;
    this->content_refresh = true;
    this->layout_refresh = true;
    this->arabic = false;
    this->thai = false;
    this->hebrew = false;
    gui_text_multilanguage_check(this);
}

void gui_text_set_scope(gui_text_t *this, int16_t x, int16_t y, int16_t w, int16_t h)
{
    this->scope_self = true;
    this->scope_absolute = false;
    this->scope_rect.x1 = x;
    this->scope_rect.y1 = y;
    this->scope_rect.x2 = x + w - 1;
    this->scope_rect.y2 = y + h - 1;
}

void gui_text_set_scope_absolute(gui_text_t *this, int16_t x, int16_t y, int16_t w, int16_t h)
{
    this->scope_self = true;
    this->scope_absolute = true;
    this->scope_rect.x1 = x;
    this->scope_rect.y1 = y;
    this->scope_rect.x2 = x + w - 1;
    this->scope_rect.y2 = y + h - 1;
}

void gui_text_set_font_blend_mode(gui_text_t *this, BLEND_MODE_TYPE blend_mode)
{
    this->font_blend_mode = blend_mode;
}

void gui_text_layout_measure(gui_text_t *this_widget)
{
    if (this_widget == NULL || this_widget->len == 0)
    {
        return;
    }

    gui_text_rect_t rect = {0};
    rect.x1 = 0;
    rect.y1 = 0;
    rect.x2 = this_widget->base.w - 1;
    rect.y2 = this_widget->base.h - 1;
    rect.xboundleft = 0;
    rect.xboundright = 32767;
    rect.yboundtop = 0;
    rect.yboundbottom = 32767;

    this_widget->content_refresh = true;
    this_widget->layout_refresh = true;

    gui_text_font_load(this_widget, &rect);
    gui_text_font_unload(this_widget);

    this_widget->content_refresh = true;
    this_widget->layout_refresh = true;
}

gui_text_t *gui_text_create(void       *parent,
                            const char *name,
                            int16_t     x,
                            int16_t     y,
                            int16_t     w,
                            int16_t     h)
{
    gui_text_t *text;

    GUI_ASSERT(parent != NULL);
    if (name == NULL)
    {
        name = "DEFAULT_TEXT";
    }

    text = gui_malloc(sizeof(gui_text_t));
    GUI_ASSERT(text != NULL);
    memset(text, 0, sizeof(gui_text_t));

    gui_text_ctor(text, parent, name, x, y, w, h);
    text->font_blend_mode = IMG_SRC_OVER_MODE;
    gui_list_init(&(GET_BASE(text)->child_list));
    if ((GET_BASE(text)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(text)->parent)->child_list), &(GET_BASE(text)->brother_list));
    }
    GET_BASE(text)->create_done = true;

    return text;
}
