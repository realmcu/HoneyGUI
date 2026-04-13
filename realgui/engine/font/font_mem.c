/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include <string.h>
#include "draw_font.h"
#include "font_mem.h"
#include "font_lib_manager.h"
#include "font_rendering_utils.h"
#include "gui_vfs.h"
#include "font_glyph_cache.h"

/** @brief One-shot flag: V1 deprecation notice already printed */
static bool s_v1_deprecation_warned = true;

/*============================================================================*
 *                      Font Library Access Functions
 *============================================================================*/

FONT_LIB_NODE *get_fontlib_by_size(uint16_t font_size)
{
    FONT_LIB_NODE *node = gui_font_lib_find_by_size((uint8_t)font_size, GUI_FONT_SRC_BMP);
    if (node != NULL)
    {
        return node;
    }

    /* Return first node as default */
    node = gui_font_lib_get_head();
    if (node == NULL)
    {
        gui_log("Error: No font file registered! Please call gui_font_mem_init() first.\n");
        GUI_ASSERT(node != NULL);
    }
    return node;
}

FONT_LIB_NODE *get_fontlib_by_name(uint8_t *font_file)
{
    FONT_LIB_NODE *node = gui_font_lib_find(font_file);
    if (node != NULL)
    {
        return node;
    }

    /* Return first node as default */
    node = gui_font_lib_get_head();
    if (node == NULL)
    {
        gui_log("Error: No font file registered! Please call gui_font_mem_init() first.\n");
        GUI_ASSERT(node != NULL);
    }
    return node;
}

int32_t font_index_bsearch_bmp(uintptr_t table_offset, uint32_t index_area_size, uint32_t unicode)
{
    if (index_area_size == 0)
    {
        return -1;
    }

    uint32_t count = index_area_size / sizeof(uint16_t);
    int32_t left = 0;
    int32_t right = (int32_t)count - 1;

    while (left <= right)
    {
        int32_t mid = left + (right - left) / 2;
        uint16_t mid_unicode = *(uint16_t *)(uintptr_t)(table_offset + mid * sizeof(uint16_t));

        if (mid_unicode == unicode)
        {
            return mid;
        }
        else if (mid_unicode < unicode)
        {
            left = mid + 1;
        }
        else
        {
            right = mid - 1;
        }
    }

    return -1;
}

/**
 * @brief Binary search for unicode in offset+crop index table
 * Index format: [unicode(2B), file_offset(4B)] per entry, 6 bytes total
 * @param table_offset Start address of index table
 * @param index_area_size Size of index area in bytes
 * @param unicode Unicode value to search for
 * @return File offset if found, 0xFFFFFFFF if not found
 */
static uint32_t font_index_bsearch_crop_offset(uintptr_t table_offset, uint32_t index_area_size,
                                               uint32_t unicode)
{
    if (index_area_size == 0)
    {
        return 0xFFFFFFFF;
    }

    uint32_t entry_size = 6; // unicode(2B) + offset(4B)
    uint32_t count = index_area_size / entry_size;
    int32_t left = 0;
    int32_t right = (int32_t)count - 1;

    while (left <= right)
    {
        int32_t mid = left + (right - left) / 2;
        uint16_t mid_unicode = *(uint16_t *)(uintptr_t)(table_offset + mid * entry_size);

        if (mid_unicode == unicode)
        {
            // Found: return the file offset (4 bytes after unicode)
            return *(uint32_t *)(uintptr_t)(table_offset + mid * entry_size + 2);
        }
        else if (mid_unicode < unicode)
        {
            left = mid + 1;
        }
        else
        {
            right = mid - 1;
        }
    }

    return 0xFFFFFFFF;
}

/**
 * @brief Read font data from filesystem
 * @param node Font library node
 * @param offset Offset in font file
 * @param buf Buffer to read into
 * @param size Size to read
 * @return Number of bytes read, or -1 on error
 */
static int font_fs_read(FONT_LIB_NODE *node, uint32_t offset, uint8_t *buf, uint32_t size)
{
    if (node == NULL || node->font_file == NULL)
    {
        return -1;
    }

    gui_vfs_file_t *file = node->fs_fd;
    bool temp_open = false;

    if (file == NULL)
    {
        /* Fallback: open temporarily if no cached handle */
        file = gui_vfs_open((const char *)node->font_file, GUI_VFS_READ);
        if (file == NULL)
        {
            return -1;
        }
        temp_open = true;
    }

    if (gui_vfs_seek(file, offset, GUI_VFS_SEEK_SET) < 0)
    {
        if (temp_open) { gui_vfs_close(file); }
        return -1;
    }

    int read_size = gui_vfs_read(file, buf, size);

    if (temp_open) { gui_vfs_close(file); }

    return read_size;
}

/**
 * @deprecated V1 crop glyph loading - scheduled for removal after V2 release.
 *             Use gui_font_bmp_load_glyph() (V2 bearing-based) for new fonts.
 *
 * @brief Load crop-mode glyph (4-byte header) from FTL or FS with cache
 *
 * Header format: [char_y, reserved, char_w, char_h_end]
 * char_h = char_h_end - char_y
 *
 * @return 0 on success, -1 on failure (caller should skip this char)
 */
static int load_dot_crop(mem_char_t *chr, uint8_t *font_path, FONT_LIB_NODE *node,
                         FONT_SRC_MODE font_mode, uint32_t offset,
                         uint8_t render_mode, int32_t *line_byte)
{
    const uint32_t hdr_size = 4;

    /* Try cache first */
    uint32_t cached_size = 0;
    uint8_t *cached = font_glyph_cache_get(font_path, chr->unicode,
                                           FONT_GLYPH_CACHE_BMP_DOT, &cached_size);
    if (cached)
    {
        chr->char_y = cached[0];
        chr->char_w = cached[2];
        chr->char_h = cached[3] - chr->char_y;
        *line_byte = (chr->char_w * render_mode + 8 - 1) / 8;
        chr->w = *line_byte * 8 / render_mode;
        chr->dot_addr = cached + hdr_size;
        return 0;
    }

    /* Read header */
    uint8_t header[4];
    if (font_mode == FONT_SRC_FTL)
    {
        gui_ftl_read((uintptr_t)font_path + offset, header, hdr_size);
    }
    else
    {
        if (font_fs_read(node, offset, header, hdr_size) < 0) { return -1; }
    }

    chr->char_y = header[0];
    chr->char_w = header[2];
    chr->char_h = header[3] - chr->char_y;
    *line_byte = (chr->char_w * render_mode + 8 - 1) / 8;
    chr->w = *line_byte * 8 / render_mode;

    uint32_t dot_size = *line_byte * chr->char_h;
    uint8_t *dot_buf = gui_malloc(dot_size);
    if (dot_buf == NULL) { return -1; }

    /* Read dot data */
    if (font_mode == FONT_SRC_FTL)
    {
        gui_ftl_read((uintptr_t)font_path + offset + hdr_size, dot_buf, dot_size);
    }
    else
    {
        if (font_fs_read(node, offset + hdr_size, dot_buf, dot_size) < 0)
        {
            gui_free(dot_buf);
            return -1;
        }
    }

    /* Try cache put */
    uint8_t *pool_ptr = font_glyph_cache_put(font_path, chr->unicode,
                                             FONT_GLYPH_CACHE_BMP_DOT,
                                             header, hdr_size, dot_buf, dot_size);
    if (pool_ptr)
    {
        gui_free(dot_buf);
        chr->dot_addr = pool_ptr + hdr_size;
    }
    else
    {
        chr->dot_addr = dot_buf;
    }
    return 0;
}

/**
 * @deprecated V1 non-crop glyph loading - scheduled for removal after V2 release.
 *             Use gui_font_bmp_load_glyph() (V2 bearing-based) for new fonts.
 *
 * @brief Load non-crop glyph (2-byte header) from FTL or FS with cache
 *
 * Header format: [char_w, char_h]
 *
 * @return 0 on success, -1 on failure (caller should skip this char)
 */
static int load_dot_nocrop(mem_char_t *chr, uint8_t *font_path, FONT_LIB_NODE *node,
                           FONT_SRC_MODE font_mode, uint32_t data_offset,
                           uint32_t dot_size)
{
    const uint32_t hdr_size = 2;

    /* Try cache first */
    uint32_t cached_size = 0;
    uint8_t *cached = font_glyph_cache_get(font_path, chr->unicode,
                                           FONT_GLYPH_CACHE_BMP_DOT, &cached_size);
    if (cached)
    {
        chr->char_w = (int16_t)cached[0];
        chr->char_h = (int16_t)cached[1];
        chr->dot_addr = cached + hdr_size;
        return 0;
    }

    /* Read header (2 bytes before dot data: at data_offset + 2) */
    uint8_t header[2];
    if (font_mode == FONT_SRC_FTL)
    {
        gui_ftl_read(data_offset + 2, header, hdr_size);
    }
    else
    {
        if (font_fs_read(node, data_offset + 2, header, hdr_size) < 0) { return -1; }
    }
    chr->char_w = (int16_t)header[0];
    chr->char_h = (int16_t)header[1];

    /* Read dot data (at data_offset + 4) */
    uint8_t *dot_buf = gui_malloc(dot_size);
    if (dot_buf == NULL) { return -1; }

    if (font_mode == FONT_SRC_FTL)
    {
        gui_ftl_read(data_offset + 4, dot_buf, dot_size);
    }
    else
    {
        if (font_fs_read(node, data_offset + 4, dot_buf, dot_size) < 0)
        {
            gui_free(dot_buf);
            return -1;
        }
    }

    /* Try cache put */
    uint8_t *pool_ptr = font_glyph_cache_put(font_path, chr->unicode,
                                             FONT_GLYPH_CACHE_BMP_DOT,
                                             header, hdr_size, dot_buf, dot_size);
    if (pool_ptr)
    {
        gui_free(dot_buf);
        chr->dot_addr = pool_ptr + hdr_size;
    }
    else
    {
        chr->dot_addr = dot_buf;
    }
    return 0;
}

/**
 * @brief Load V2 bearing-based glyph (6-byte header + tight bitmap).
 *
 * Header format: [bearingX(1), bearingY(1), width(1), height(1), advance(1), reserved(1)]
 * Bitmap: width x height pixels, packed by render_mode.
 *
 * @param chr          Output character struct (bearing_x, bearing_y, advance populated).
 * @param font_path    Font file path or memory address.
 * @param node         Font library node (for FTL/FS reads).
 * @param font_mode    Font source mode (MEMADDR / FTL / FILESYS).
 * @param offset       Byte offset to the glyph data in the font file.
 * @param render_mode  Bits per pixel (1/2/4/8).
 * @param line_byte    Output: aligned line byte width for rendering.
 * @return 0 on success, -1 on failure.
 */
static int gui_font_bmp_load_glyph(mem_char_t *chr, uint8_t *font_path, FONT_LIB_NODE *node,
                                   FONT_SRC_MODE font_mode, uint32_t offset,
                                   uint8_t render_mode, int32_t *line_byte)
{
    const uint32_t hdr_size = sizeof(GUI_BMP_GLYPH_HEAD_V2); /* 6 bytes */

    /* Try cache first */
    uint32_t cached_size = 0;
    uint8_t *cached = font_glyph_cache_get(font_path, chr->unicode,
                                           FONT_GLYPH_CACHE_BMP_DOT, &cached_size);
    if (cached)
    {
        GUI_BMP_GLYPH_HEAD_V2 *gh = (GUI_BMP_GLYPH_HEAD_V2 *)cached;
        chr->bearing_x = gh->bearing_x;
        chr->bearing_y = gh->bearing_y;
        chr->char_w    = gh->width;
        chr->char_h    = gh->height;
        chr->advance   = gh->advance;
        *line_byte = (chr->char_w * render_mode + 8 - 1) / 8;
        chr->w = *line_byte * 8 / render_mode;
        chr->dot_addr = cached + hdr_size;
        return 0;
    }

    /* Read 6-byte V2 glyph header */
    uint8_t header[6];
    if (font_mode == FONT_SRC_FTL)
    {
        gui_ftl_read((uintptr_t)font_path + offset, header, hdr_size);
    }
    else
    {
        if (font_fs_read(node, offset, header, hdr_size) < 0) { return -1; }
    }

    GUI_BMP_GLYPH_HEAD_V2 *gh = (GUI_BMP_GLYPH_HEAD_V2 *)header;
    chr->bearing_x = gh->bearing_x;
    chr->bearing_y = gh->bearing_y;
    chr->char_w    = gh->width;
    chr->char_h    = gh->height;
    chr->advance   = gh->advance;
    *line_byte = (chr->char_w * render_mode + 8 - 1) / 8;
    chr->w = *line_byte * 8 / render_mode;

    uint32_t dot_size = *line_byte * chr->char_h;
    if (dot_size == 0)
    {
        chr->dot_addr = NULL;
        return 0;
    }

    uint8_t *dot_buf = gui_malloc(dot_size);
    if (dot_buf == NULL) { return -1; }

    /* Read tight bitmap data after 6-byte header */
    if (font_mode == FONT_SRC_FTL)
    {
        gui_ftl_read((uintptr_t)font_path + offset + hdr_size, dot_buf, dot_size);
    }
    else
    {
        if (font_fs_read(node, offset + hdr_size, dot_buf, dot_size) < 0)
        {
            gui_free(dot_buf);
            return -1;
        }
    }

    /* Try cache put */
    uint8_t *pool_ptr = font_glyph_cache_put(font_path, chr->unicode,
                                             FONT_GLYPH_CACHE_BMP_DOT,
                                             header, hdr_size, dot_buf, dot_size);
    if (pool_ptr)
    {
        gui_free(dot_buf);
        chr->dot_addr = pool_ptr + hdr_size;
    }
    else
    {
        chr->dot_addr = dot_buf;
    }
    return 0;
}

/**
 * @brief Load emoji glyph for unicode >= 0x10000
 * @return Number of extra unicode entries consumed (for multi-codepoint emoji), 0 if no emoji
 */
static uint32_t load_emoji(mem_char_t *chr, gui_text_t *text,
                           uint32_t *unicode_buf, uint32_t uni_i, uint32_t unicode_len)
{
    if (text->emoji_path == NULL)
    {
        return 0;
    }
    char file_path[100];
    memset(file_path, 0, 100);
    snprintf(file_path, sizeof(file_path), "%s", text->emoji_path);
    uint32_t multi_len = generate_emoji_file_path_from_unicode(&unicode_buf[uni_i],
                                                               unicode_len - uni_i, file_path);
    if (multi_len == 0)
    {
        gui_log("No valid emoji\n");
        return 0;
    }
    chr->dot_addr = (void *)gui_vfs_get_file_address(file_path);
    if (chr->dot_addr != NULL)
    {
        chr->char_w = text->font_height;
        chr->char_h = text->font_height;
    }
    else
    {
        chr->char_w = 0;
        chr->char_h = 0;
    }
    return multi_len;
}

/**
 * @brief Try to load a single glyph from a specific BMP font.
 *
 * Handles all combinations of crop/non-crop, address/offset index,
 * V1/V2 headers, and MEMADDR/FTL/FILESYS source modes.
 *
 * @param font_file  Font file address or path
 * @param node       Font library node (NULL for MEMADDR)
 * @param src_mode   Font source mode
 * @param unicode    Unicode code point to look up
 * @param font_size  Font size in pixels
 * @param out_chr    Output character info (populated on success)
 * @param out_line_byte Output line byte width (for crop fonts)
 * @return 0 if glyph found, -1 if not found
 */
static int font_bmp_lookup_char(uint8_t *font_file, FONT_LIB_NODE *node,
                                FONT_SRC_MODE src_mode, uint32_t unicode,
                                uint8_t font_size, mem_char_t *out_chr,
                                int32_t *out_line_byte)
{
    GUI_FONT_HEAD_BMP *font;
    if (src_mode == FONT_SRC_MEMADDR)
    {
        font = (GUI_FONT_HEAD_BMP *)font_file;
    }
    else if (node != NULL && node->cached_data != NULL)
    {
        font = (GUI_FONT_HEAD_BMP *)node->cached_data;
    }
    else
    {
        return -1;
    }
    if (font == NULL || font->file_type != FONT_FILE_BMP_FLAG)
    {
        return -1;
    }

    uint8_t render_mode = font->render_mode ? font->render_mode : 1;
    uintptr_t table_offset = (uintptr_t)((uint8_t *)font + font->head_length);

    if (font->crop)
    {
        uint32_t offset;
        if (font->index_method == 0)
        {
            uint32_t *offset_addr = (uint32_t *)((uint8_t *)(uintptr_t)table_offset + unicode * 4);
            offset = *offset_addr;
        }
        else
        {
            offset = font_index_bsearch_crop_offset(table_offset, font->index_area_size, unicode);
        }
        if (offset == 0xFFFFFFFF)
        {
            return -1;
        }

        if (src_mode == FONT_SRC_MEMADDR)
        {
            gui_font_typo_context_t ctx = gui_font_bmp_get_typo_context(font, font_size);
            if (ctx.is_v2)
            {
                GUI_BMP_GLYPH_HEAD_V2 *gh = (GUI_BMP_GLYPH_HEAD_V2 *)(font_file + offset);
                out_chr->bearing_x = gh->bearing_x;
                out_chr->bearing_y = gh->bearing_y;
                out_chr->char_w    = gh->width;
                out_chr->char_h    = gh->height;
                out_chr->advance   = gh->advance;
                *out_line_byte = (out_chr->char_w * render_mode + 8 - 1) / 8;
                out_chr->w = *out_line_byte * 8 / render_mode;
                out_chr->dot_addr = font_file + offset + sizeof(GUI_BMP_GLYPH_HEAD_V2);
            }
            else
            {
                out_chr->dot_addr = font_file + offset + 4;
                out_chr->char_w = (uint8_t)(*(out_chr->dot_addr - 2));
                out_chr->char_y = (uint8_t)(*(out_chr->dot_addr - 4));
                out_chr->char_h = (uint8_t)(*(out_chr->dot_addr - 1)) - out_chr->char_y;
                *out_line_byte = (out_chr->char_w * render_mode + 8 - 1) / 8;
                out_chr->w = *out_line_byte * 8 / render_mode;
            }
        }
        else
        {
            gui_font_typo_context_t ctx = gui_font_bmp_get_typo_context(font, font_size);
            if (src_mode == FONT_SRC_FILESYS) { gui_font_lib_fs_open(node); }
            int rc;
            if (ctx.is_v2)
            {
                rc = gui_font_bmp_load_glyph(out_chr, font_file, node,
                                             src_mode, offset, render_mode, out_line_byte);
            }
            else
            {
                rc = load_dot_crop(out_chr, font_file, node,
                                   src_mode, offset, render_mode, out_line_byte);
            }
            if (src_mode == FONT_SRC_FILESYS) { gui_font_lib_fs_close(node); }
            if (rc < 0) { return -1; }
        }
    }
    else
    {
        /* Non-crop */
        uint8_t aliened = font_size;
        if (font_size % 8 != 0) { aliened = 8 - font_size % 8 + font_size; }
        uint32_t font_area = aliened * font_size / 8 * render_mode + 4;
        uintptr_t dot_offset_base = table_offset + font->index_area_size;

        int32_t index;
        if (font->index_method == 0)
        {
            if (unicode > 0xFFFF) { return -1; }
            uint16_t idx = *(uint16_t *)(uintptr_t)(unicode * 2 + table_offset);
            if (idx == 0xFFFF) { return -1; }
            index = (int32_t)idx;
        }
        else
        {
            index = font_index_bsearch_bmp(table_offset, font->index_area_size, unicode);
            if (index < 0) { return -1; }
        }

        if (src_mode == FONT_SRC_MEMADDR)
        {
            out_chr->dot_addr = (uint8_t *)(uintptr_t)((uintptr_t)index * font_area + dot_offset_base + 4);
            out_chr->char_w = (int16_t)(*(out_chr->dot_addr - 2));
            out_chr->char_h = (int16_t)(*(out_chr->dot_addr - 1));
            out_chr->w = aliened;
            out_chr->h = font_size;
        }
        else
        {
            uint32_t dot_size = aliened * font_size / 8 * render_mode;
            uint32_t data_offset = (uint32_t)index * font_area + dot_offset_base;
            if (src_mode == FONT_SRC_FILESYS) { gui_font_lib_fs_open(node); }
            int rc = load_dot_nocrop(out_chr, font_file, node, src_mode, data_offset, dot_size);
            if (src_mode == FONT_SRC_FILESYS) { gui_font_lib_fs_close(node); }
            if (rc < 0) { return -1; }
        }
    }

    out_chr->render_mode = render_mode;
    return 0;
}

/**
 * @brief Search for a glyph in all registered BMP fonts (fallback).
 *
 * Iterates font_lib nodes with matching font_size, ordered by priority,
 * skipping the primary font.
 */
int gui_font_bmp_fallback_search(uint32_t unicode, uint8_t font_size,
                                 uint8_t *skip_file, mem_char_t *out_chr,
                                 int32_t *out_line_byte)
{
    FONT_LIB_NODE *best = NULL;
    uint8_t best_prio = 0xFF;
    uint16_t min_prio = 0;

    while (min_prio <= 0xFF)
    {
        best = NULL;
        best_prio = 0xFF;
        FONT_LIB_NODE *node = gui_font_lib_get_head();
        while (node != NULL)
        {
            if (node->font_type == GUI_FONT_SRC_BMP &&
                node->font_size == font_size &&
                node->font_file != skip_file &&
                node->priority >= min_prio &&
                (best == NULL || node->priority < best_prio))
            {
                best = node;
                best_prio = node->priority;
            }
            node = node->next;
        }

        if (best == NULL)
        {
            break;
        }

        if (font_bmp_lookup_char(best->font_file, best, best->src_mode,
                                 unicode, font_size, out_chr, out_line_byte) == 0)
        {
            // gui_log("[BMP fallback] U+%04X: primary=%p -> fallback=%p (prio=%d)\n",
            //         unicode, skip_file, best->font_file, best->priority);
            return 0;
        }

        min_prio = (best_prio < 0xFF) ? best_prio + 1 : 0xFF + 1;
    }

    return -1;
}

void gui_font_get_dot_info(gui_text_t *text)
{
    GUI_FONT_HEAD_BMP *font;
    FONT_LIB_NODE *node = NULL;
    uintptr_t table_offset;
    uintptr_t dot_offset;
    if (text == NULL)
    {
        return;
    }
    if (text->path == NULL)
    {
        node = get_fontlib_by_size(text->font_height);
        if (node != NULL)
        {
            text->font_mode = node->src_mode;
            text->path = node->font_file;
        }
    }

    if (text->font_mode == FONT_SRC_MEMADDR)
    {
        font = (GUI_FONT_HEAD_BMP *)text->path;
        if (font != NULL)
        {
            table_offset = (uintptr_t)((uint8_t *)font + font->head_length);
            dot_offset = table_offset + font->index_area_size;
        }
    }
    else if (text->font_mode == FONT_SRC_FTL)
    {
        node = get_fontlib_by_name(text->path);
        if (node == NULL)
        {
            return;
        }
        font = (GUI_FONT_HEAD_BMP *)node->cached_data;
        if (font == NULL)
        {
            return;
        }
        table_offset = (uintptr_t)((uint8_t *)font + font->head_length);
        dot_offset = (uintptr_t)text->path + font->head_length + font->index_area_size;
    }
    else if (text->font_mode == FONT_SRC_FILESYS)
    {
        node = get_fontlib_by_name(text->path);
        if (node == NULL)
        {
            return;
        }
        font = (GUI_FONT_HEAD_BMP *)node->cached_data;
        if (font == NULL)
        {
            return;
        }
        table_offset = (uintptr_t)((uint8_t *)font + font->head_length);
        dot_offset = font->head_length + font->index_area_size;
        /* Cache file handle for batch reads */
        gui_font_lib_fs_open(node);
    }
    else
    {
        font = (GUI_FONT_HEAD_BMP *)text->path;
    }
    GUI_ASSERT(font != 0)
    if (font == NULL)
    {
        GUI_ASSERT(font != NULL);
        return;
    }
    uint8_t render_mode = 0;
    render_mode = font->render_mode;
    if (render_mode == 0)
    {
        render_mode = 1;
        gui_log("font file error ! render_mode is 0 ! \n");
    }
    uint8_t index_method = font->index_method;
    uint8_t index_unit_length = 4; //now set to 4 , todo
    bool crop = font->crop;

    uint32_t *unicode_buf = NULL;
    uint16_t unicode_len = 0;
    unicode_len = process_content_by_charset(text->charset, text->content, text->len, &unicode_buf);
    if (unicode_len == 0)
    {
        gui_log("Warning! After process, unicode len of text: %s is 0!\n", text->base.name);
        text->font_len = 0;
        return;
    }
    if (text->arabic)
    {
        unicode_len = process_ap_unicode(unicode_buf, unicode_len);
    }

    mem_char_t *chr = gui_malloc(sizeof(mem_char_t) * unicode_len);
    if (chr == NULL)
    {
        GUI_ASSERT(NULL != NULL);
        return;
    }
    if (chr)
    {
        memset(chr, 0, sizeof(mem_char_t) * unicode_len);
        for (uint16_t k = 0; k < unicode_len; k++)
        {
            chr[k].render_mode = render_mode;
        }
    }
    text->data = chr;
    text->content_refresh = false;

    int32_t all_char_w = 0;
    int32_t all_char_h = 0;
    uint32_t line_flag = 0;
    int32_t line_byte = 0;

    uint32_t uni_i = 0;
    uint32_t chr_i = 0;

    /* Build typography context once per text widget (V2 vs V1 detection) */
    gui_font_typo_context_t typo_ctx = gui_font_bmp_get_typo_context(font, text->font_height);

    /* V2: auto-expand widget height to fit at least one line.
     * In V2, font_size (em-size) < default_line_height, so the prepare-stage
     * "h = font_height" is too small.  Expand here where header is already parsed. */
    if (typo_ctx.is_v2 && text->base.h < typo_ctx.default_line_height)
    {
        text->base.h = typo_ctx.default_line_height;
    }

    /* V2: chr->h represents line height (used for layout clipping & line spacing).
     * V1: chr->h = font_height (= backSize = canvas height). */
    int16_t chr_h = typo_ctx.is_v2 ? typo_ctx.default_line_height : text->font_height;

    /* One-time V1 deprecation notice */
    if (!typo_ctx.is_v2 && !s_v1_deprecation_warned)
    {
        s_v1_deprecation_warned = true;
        gui_log("[font_mem] Warning: V1 font format detected (font_size=%d). "
                "V1 support is deprecated and will be removed in a future release. "
                "Please regenerate your font files with the V2 Font Tool.\n",
                font->font_size);
    }

    if (crop)
    {
        switch (index_method)
        {
        case 0: //address
            for (uni_i = 0; uni_i < unicode_len; uni_i++)
            {
                chr[chr_i].unicode = unicode_buf[uni_i];
                chr[chr_i].w = 0;
                chr[chr_i].h = chr_h;
                uint32_t offset = 0;
                if (chr[chr_i].unicode == 0x20 || chr[chr_i].unicode == 0x0D)
                {
                    chr[chr_i].char_w = text->font_height / 4;
                    chr[chr_i].char_h = text->font_height / 4;
                }
                else if (chr[chr_i].unicode == 0x0A)
                {
                    line_flag ++;
                    chr[chr_i].char_w = 0;
                    chr[chr_i].char_h = 0;
                }
                else if (chr[chr_i].unicode >= 0x10000)
                {
                    uint32_t multi_len = load_emoji(&chr[chr_i], text, unicode_buf, uni_i, unicode_len);
                    if (multi_len) { uni_i += multi_len - 1; }
                }
                else
                {
                    uint32_t *offset_addr = (uint32_t *)((uint8_t *)(uintptr_t)table_offset + chr[chr_i].unicode *
                                                         index_unit_length);
                    offset = *offset_addr;
                    if (offset == 0xFFFFFFFF)
                    {
                        /* Fallback: try other BMP fonts with same size */
                        if (gui_font_bmp_fallback_search(chr[chr_i].unicode, text->font_height,
                                                         (uint8_t *)text->path, &chr[chr_i], &line_byte) == 0)
                        {
                            /* Found in BMP fallback */
                        }
                        else
                        {
                            continue;
                        }
                    }
                    else if (text->font_mode == FONT_SRC_MEMADDR)
                    {
                        if (typo_ctx.is_v2)
                        {
                            /* V2: read 6-byte bearing-based header directly from memory */
                            GUI_BMP_GLYPH_HEAD_V2 *gh = (GUI_BMP_GLYPH_HEAD_V2 *)((uint8_t *)text->path + offset);
                            chr[chr_i].bearing_x = gh->bearing_x;
                            chr[chr_i].bearing_y = gh->bearing_y;
                            chr[chr_i].char_w    = gh->width;
                            chr[chr_i].char_h    = gh->height;
                            chr[chr_i].advance   = gh->advance;
                            line_byte = (chr[chr_i].char_w * render_mode + 8 - 1) / 8;
                            chr[chr_i].w = line_byte * 8 / render_mode;
                            chr[chr_i].dot_addr = (uint8_t *)text->path + offset + sizeof(GUI_BMP_GLYPH_HEAD_V2);
                        }
                        else
                        {
                            /* @deprecated V1 crop: 4-byte header */
                            chr[chr_i].dot_addr = (uint8_t *)text->path + offset + 4;
                            chr[chr_i].char_w = (uint8_t)(*(chr[chr_i].dot_addr - 2));
                            chr[chr_i].char_y = (uint8_t)(*(chr[chr_i].dot_addr - 4));
                            chr[chr_i].char_h = (uint8_t)(*(chr[chr_i].dot_addr - 1)) - chr[chr_i].char_y;
                            line_byte = (chr[chr_i].char_w * render_mode + 8 - 1) / 8;
                            chr[chr_i].w = line_byte * 8 / render_mode;
                        }
                    }
                    else if (text->font_mode == FONT_SRC_FTL ||
                             text->font_mode == FONT_SRC_FILESYS)
                    {
                        if (typo_ctx.is_v2)
                        {
                            if (gui_font_bmp_load_glyph(&chr[chr_i], text->path, node,
                                                        text->font_mode, offset,
                                                        render_mode, &line_byte) < 0)
                            {
                                continue;
                            }
                        }
                        else
                        {
                            if (load_dot_crop(&chr[chr_i], text->path, node,
                                              text->font_mode, offset,
                                              render_mode, &line_byte) < 0)
                            {
                                continue;
                            }
                        }
                    }
                }
                all_char_w += chr[chr_i].char_w;
                chr_i ++;
            }
            break;
        case 1: //offset
            {
                uint32_t index_area_size = font->index_area_size;
                for (uni_i = 0; uni_i < unicode_len; uni_i++)
                {
                    chr[chr_i].unicode = unicode_buf[uni_i];
                    chr[chr_i].w = 0;
                    chr[chr_i].h = chr_h;
                    uint32_t offset = 0;
                    if (chr[chr_i].unicode == 0x20 || chr[chr_i].unicode == 0x0D)
                    {
                        chr[chr_i].char_w = text->font_height / 4;
                        chr[chr_i].char_h = text->font_height / 4;
                    }
                    else if (chr[chr_i].unicode == 0x0A)
                    {
                        line_flag ++;
                        chr[chr_i].char_w = 0;
                        chr[chr_i].char_h = 0;
                    }
                    else if (chr[chr_i].unicode >= 0x10000)
                    {
                        uint32_t multi_len = load_emoji(&chr[chr_i], text, unicode_buf, uni_i, unicode_len);
                        if (multi_len) { uni_i += multi_len - 1; }
                    }
                    else
                    {
                        // Binary search for unicode in offset+crop index table
                        offset = font_index_bsearch_crop_offset(table_offset, index_area_size, chr[chr_i].unicode);
                        if (offset == 0xFFFFFFFF)
                        {
                            if (gui_font_bmp_fallback_search(chr[chr_i].unicode, text->font_height,
                                                             (uint8_t *)text->path, &chr[chr_i], &line_byte) == 0)
                            {
                                /* Found in BMP fallback */
                            }
                            else
                            {
                                continue;
                            }
                        }
                        else if (text->font_mode == FONT_SRC_MEMADDR)
                        {
                            if (typo_ctx.is_v2)
                            {
                                /* V2: read 6-byte bearing-based header directly from memory */
                                GUI_BMP_GLYPH_HEAD_V2 *gh = (GUI_BMP_GLYPH_HEAD_V2 *)((uint8_t *)text->path + offset);
                                chr[chr_i].bearing_x = gh->bearing_x;
                                chr[chr_i].bearing_y = gh->bearing_y;
                                chr[chr_i].char_w    = gh->width;
                                chr[chr_i].char_h    = gh->height;
                                chr[chr_i].advance   = gh->advance;
                                line_byte = (chr[chr_i].char_w * render_mode + 8 - 1) / 8;
                                chr[chr_i].w = line_byte * 8 / render_mode;
                                chr[chr_i].dot_addr = (uint8_t *)text->path + offset + sizeof(GUI_BMP_GLYPH_HEAD_V2);
                            }
                            else
                            {
                                /* @deprecated V1 crop: 4-byte header */
                                chr[chr_i].dot_addr = (uint8_t *)text->path + offset + 4;
                                chr[chr_i].char_w = (uint8_t)(*(chr[chr_i].dot_addr - 2));
                                chr[chr_i].char_y = (uint8_t)(*(chr[chr_i].dot_addr - 4));
                                chr[chr_i].char_h = (uint8_t)(*(chr[chr_i].dot_addr - 1)) - chr[chr_i].char_y;
                                line_byte = (chr[chr_i].char_w * render_mode + 8 - 1) / 8;
                                chr[chr_i].w = line_byte * 8 / render_mode;
                            }
                        }
                        else if (text->font_mode == FONT_SRC_FTL ||
                                 text->font_mode == FONT_SRC_FILESYS)
                        {
                            if (typo_ctx.is_v2)
                            {
                                if (gui_font_bmp_load_glyph(&chr[chr_i], text->path, node,
                                                            text->font_mode, offset,
                                                            render_mode, &line_byte) < 0)
                                {
                                    continue;
                                }
                            }
                            else
                            {
                                if (load_dot_crop(&chr[chr_i], text->path, node,
                                                  text->font_mode, offset,
                                                  render_mode, &line_byte) < 0)
                                {
                                    continue;
                                }
                            }
                        }
                    }
                    all_char_w += chr[chr_i].char_w;
                    chr_i ++;
                }
            }
            break;
        default:
            break;
        }
    }
    else
    {
        uint8_t aliened_font_size;
        aliened_font_size = text->font_height;
        if (text->font_height % 8 != 0)
        {
            aliened_font_size = 8 - text->font_height % 8 + text->font_height;
        }
        uint32_t font_area = aliened_font_size * text->font_height / 8 * render_mode + 4;
        switch (index_method)
        {
        case 0: //address
            for (uni_i = 0; uni_i < unicode_len; uni_i++)
            {
                chr[chr_i].unicode = unicode_buf[uni_i];
                chr[chr_i].w = aliened_font_size;
                chr[chr_i].h = text->font_height;
                uint16_t offset = 0;
                if (chr[chr_i].unicode == 0x20 || chr[chr_i].unicode == 0x0D)
                {
                    chr[chr_i].char_w = text->font_height / 4;
                    chr[chr_i].char_h = text->font_height / 4;
                }
                else if (chr[chr_i].unicode == 0x0A)
                {
                    line_flag ++;
                    chr[chr_i].char_w = 0;
                    chr[chr_i].char_h = 0;
                }
                else if (chr[chr_i].unicode >= 0x10000)
                {
                    uint32_t multi_len = load_emoji(&chr[chr_i], text, unicode_buf, uni_i, unicode_len);
                    if (multi_len) { uni_i += multi_len - 1; }
                }
                else
                {
                    offset = *(uint16_t *)(uintptr_t)(chr[chr_i].unicode * 2 + table_offset);
                    if (offset == 0xFFFF)
                    {
                        if (gui_font_bmp_fallback_search(chr[chr_i].unicode, text->font_height,
                                                         (uint8_t *)text->path, &chr[chr_i], &line_byte) == 0)
                        {
                            /* Found in BMP fallback */
                        }
                        else
                        {
                            continue;
                        }
                    }
                    else if (text->font_mode == FONT_SRC_MEMADDR)
                    {
                        /* @deprecated V1 nocrop: 2-byte header */
                        chr[chr_i].dot_addr = (uint8_t *)(uintptr_t)((uintptr_t)offset * font_area + dot_offset + 4);
                        chr[chr_i].char_w = (int16_t)(*(chr[chr_i].dot_addr - 2));
                        chr[chr_i].char_h = (int16_t)(*(chr[chr_i].dot_addr - 1));
                    }
                    else if (text->font_mode == FONT_SRC_FTL ||
                             text->font_mode == FONT_SRC_FILESYS)
                    {
                        uint32_t dot_size = aliened_font_size * text->font_height / 8 * render_mode;
                        uint32_t data_offset = (uint32_t)offset * font_area + dot_offset;
                        if (load_dot_nocrop(&chr[chr_i], text->path, node,
                                            text->font_mode, data_offset, dot_size) < 0)
                        {
                            continue;
                        }
                    }
                }
                all_char_w += chr[chr_i].char_w;
                all_char_h = all_char_h + chr[chr_i].char_h + 2;
                chr_i ++;
            }
            break;
        case 1: //offset
            {
                uint32_t index_area_size = font->index_area_size;
                for (uni_i = 0; uni_i < unicode_len; uni_i++)
                {
                    chr[chr_i].unicode = unicode_buf[uni_i];
                    chr[chr_i].w = aliened_font_size;
                    chr[chr_i].h = text->font_height;
                    chr[chr_i].char_w = text->font_height / 2;
                    chr[chr_i].char_h = text->font_height;
                    if (chr[chr_i].unicode == 0x0A)
                    {
                        line_flag ++;
                        chr[chr_i].char_w = 0;
                        chr[chr_i].char_h = 0;
                    }
                    else if (chr[chr_i].unicode == 0x20)
                    {
                        chr[chr_i].char_w = text->font_height / 4;
                        chr[chr_i].char_h = text->font_height / 4;
                    }
                    else if (chr[chr_i].unicode >= 0x10000)
                    {
                        uint32_t multi_len = load_emoji(&chr[chr_i], text, unicode_buf, uni_i, unicode_len);
                        if (multi_len) { uni_i += multi_len - 1; }
                    }
                    else
                    {
                        int32_t index = font_index_bsearch_bmp(table_offset, index_area_size, chr[chr_i].unicode);
                        if (index < 0)
                        {
                            if (gui_font_bmp_fallback_search(chr[chr_i].unicode, text->font_height,
                                                             (uint8_t *)text->path, &chr[chr_i], &line_byte) == 0)
                            {
                                /* Found in BMP fallback - skip primary font loading */
                            }
                            else
                            {
                                gui_log("Character %x not found in BMP-BIN file \n", chr[chr_i].unicode);
                                continue;
                            }
                        }
                        else if (text->font_mode == FONT_SRC_MEMADDR)
                        {
                            /* @deprecated V1 nocrop: 2-byte header */
                            chr[chr_i].dot_addr = (uint8_t *)(uintptr_t)((uintptr_t)index * font_area + dot_offset + 4);
                            chr[chr_i].char_w = (int16_t)(*(chr[chr_i].dot_addr - 2));
                            chr[chr_i].char_h = (int16_t)(*(chr[chr_i].dot_addr - 1));
                        }
                        else if (text->font_mode == FONT_SRC_FTL ||
                                 text->font_mode == FONT_SRC_FILESYS)
                        {
                            uint32_t dot_size = aliened_font_size * text->font_height / 8 * render_mode;
                            uint32_t data_offset = (uint32_t)index * font_area + dot_offset;
                            if (load_dot_nocrop(&chr[chr_i], text->path, node,
                                                text->font_mode, data_offset, dot_size) < 0)
                            {
                                continue;
                            }
                        }
                    }
                    all_char_w += chr[chr_i].char_w;
                    chr_i ++;
                }
            }
            break;
        default:
            break;
        }
    }
    text->char_width_sum = all_char_w;
    text->char_height_sum = all_char_h;
    text->char_line_sum = line_flag;
    text->font_len = chr_i;
    text->active_font_len = chr_i;
    text->char_width_sum += text->extra_letter_spacing * (text->font_len - 1);
    /* Close cached file handle after batch reads */
    if (node != NULL && text->font_mode == FONT_SRC_FILESYS)
    {
        gui_font_lib_fs_close(node);
    }
    gui_free(unicode_buf);
}

void gui_font_mem_load(gui_text_t *text, gui_text_rect_t *rect)
{
    if (text == NULL)
    {
        return;
    }
    if (text->data == NULL)
    {
        gui_font_get_dot_info(text);
    }
    else
    {
        if (text->content_refresh)
        {
            gui_font_mem_unload(text);
            gui_font_get_dot_info(text);
        }
    }
    // if (text->layout_refresh)
    {
        if (text != NULL)
        {
            gui_font_mem_layout(text, rect);
        }
    }
}

/**
 * @brief Position a single glyph using V2 bearing-based layout.
 *
 * @param chr           Glyph to position (bearing_x/y, advance must be filled)
 * @param cursor_x      Current horizontal cursor (updated on return)
 * @param line_y        Top Y of current line
 * @param baseline_px   Baseline offset from line top (pixels)
 * @param letter_spacing Extra letter spacing
 */
static void layout_position_bearing(mem_char_t *chr, int16_t *cursor_x, int16_t line_y,
                                    int16_t baseline_px, int8_t letter_spacing)
{
    chr->x = *cursor_x + chr->bearing_x;
    chr->y = line_y + baseline_px - chr->bearing_y;
    *cursor_x += chr->advance + letter_spacing;
}

void gui_font_mem_layout(gui_text_t *text, gui_text_rect_t *rect)
{
    if (text == NULL || text->data == NULL)
    {
        return;
    }
    mem_char_t *chr = text->data;
    int rect_w = rect->x2 - rect->x1 + 1;
    int rect_h = rect->y2 - rect->y1 + 1;
    uint32_t char_width_sum = text->char_width_sum;
    uint32_t char_height_sum = text->char_height_sum;
    uint32_t char_line_sum = text->char_line_sum;
    uint32_t font_len = text->font_len;
    uint32_t active_font_len = text->active_font_len;
    int16_t letter_spacing = text->extra_letter_spacing;
    int16_t line_spacing = text->extra_line_spacing;
    TEXT_MODE text_mode = text->mode;
    bool wordwrap = text->wordwrap || text->arabic || text->hebrew;
    bool need_rtl = false;

    /* Build V2 typo context (V1: is_v2=false, baseline_px=0) */
    gui_font_typo_context_t typo_ctx = {0};
    {
        GUI_FONT_HEAD_BMP *font_header = NULL;
        if (text->font_mode == FONT_SRC_MEMADDR)
        {
            font_header = (GUI_FONT_HEAD_BMP *)text->path;
        }
        else
        {
            FONT_LIB_NODE *node = get_fontlib_by_name(text->path);
            if (node != NULL)
            {
                font_header = (GUI_FONT_HEAD_BMP *)node->cached_data;
            }
        }
        if (font_header != NULL)
        {
            typo_ctx = gui_font_bmp_get_typo_context(font_header, text->font_height);
        }
    }

    if (text_mode >= RTL_RIGHT && text_mode <= RTL_MULTI_LEFT)
    {
        text_mode = (TEXT_MODE)(text_mode - (RTL_RIGHT - LEFT));
        need_rtl = true;
    }

    THAI_MARK_INFO *mark_array_out = NULL;
    uint32_t mark_count_out;
    if (text->thai)
    {
        char_width_sum -= get_thai_mark_char_width(chr, font_len);
        font_len = process_thai_char_struct(chr, font_len, &mark_array_out, &mark_count_out);
        active_font_len = font_len;
        char_width_sum -= mark_count_out * letter_spacing;
    }

    switch (text_mode)
    {
    case LEFT:
    case CENTER:
    case RIGHT:
    case MID_LEFT:
    case MID_CENTER:
    case MID_RIGHT:
        {
            char_line_sum = 1;
            int32_t align_factor = (text_mode <= RIGHT) ? (text_mode - LEFT) : (text_mode - MID_LEFT);
            uint32_t offset_y = (text_mode >= MID_LEFT && text_mode <= MID_RIGHT) ?
                                (rect_h - text->font_height) / 2 : 0;
            int offset = _UI_MAX((int32_t)((rect_w - char_width_sum) / 2), 0);

            if (typo_ctx.is_v2)
            {
                /* V2: bearing-based single-line positioning */
                int16_t cursor_x = rect->x1 + offset * align_factor;
                int16_t line_y = rect->y1 + offset_y;
                for (uint16_t i = 0; i < font_len; i++)
                {
                    layout_position_bearing(&chr[i], &cursor_x, line_y,
                                            typo_ctx.baseline_px, letter_spacing);
                    if ((chr[i].x + chr[i].char_w - 1) > rect->x2)
                    {
                        active_font_len = i;
                        break;
                    }
                }
            }
            else
            {
                /* V1: char_w stepping */
                for (uint16_t i = 0; i < font_len; i++)
                {
                    chr[i].y = rect->y1 + offset_y;
                    if (i == 0)
                    {
                        chr[i].x = rect->x1 + offset * align_factor;
                    }
                    else
                    {
                        chr[i].x = chr[i - 1].x + chr[i - 1].char_w + letter_spacing;
                    }
                    if ((chr[i].x + chr[i].char_w - 1) > rect->x2)
                    {
                        active_font_len = i;
                        break;
                    }
                }
            }
            char_height_sum = text->font_height;
            break;
        }
    case MULTI_LEFT:
    case MULTI_CENTER:
    case MULTI_RIGHT:
    case MULTI_MID_LEFT:
    case MULTI_MID_CENTER:
    case MULTI_MID_RIGHT:
        {
            gui_text_line_t *line_buf;
            int32_t line = 0;
            int32_t last_space_index = 0;
            int32_t line_start_index = 0;
            /* V2 line_height: text->line_height (explicit) > typo_ctx.default_line_height
               V1 line_height: chr[0].h + extra_line_spacing (unchanged) */
            int32_t line_height = typo_ctx.is_v2 ?
                                  ((text->line_height > 0 ? text->line_height
                                    : typo_ctx.default_line_height) + line_spacing) :
                                  (chr[0].h + line_spacing);
            int32_t align_factor = (text_mode <= MULTI_RIGHT) ?
                                   (text_mode - MULTI_LEFT) : (text_mode - MULTI_MID_LEFT);
            bool vertical_center = (text_mode >= MULTI_MID_LEFT);

            int32_t line_count = rect_h / line_height + 1;
            line_buf = gui_malloc(line_count * sizeof(gui_text_line_t));
            GUI_ASSERT(line_buf != NULL);
            memset(line_buf, 0, line_count * sizeof(gui_text_line_t));

            if (typo_ctx.is_v2)
            {
                /* V2: bearing-based multi-line layout, wrap by advance */
                int16_t cursor_x = rect->x1;
                for (uint16_t i = 0; i < font_len; i++)
                {
                    if (chr[i].unicode == 0x20)
                    {
                        last_space_index = i;
                    }

                    /* Check if advance exceeds right boundary */
                    int16_t next_cursor = cursor_x + chr[i].advance;
                    if ((next_cursor - 1) > rect->x2 && cursor_x != rect->x1)
                    {
                        if (chr[i].unicode == 0x20 && cursor_x < rect->x2)
                        {
                            /* Space at boundary, position and continue */
                            layout_position_bearing(&chr[i], &cursor_x, rect->y1 + line * line_height,
                                                    typo_ctx.baseline_px, letter_spacing);
                            chr[i].char_w = rect->x2 + 1 - chr[i].x;
                            continue;
                        }
                        if (wordwrap && i != font_len - 1 &&
                            last_space_index > line_start_index && chr[last_space_index].unicode == 0x20)
                        {
                            i = last_space_index + 1;
                        }
                        line_buf[line].index = i - 1;
                        line_buf[line].offset = (rect_w - (cursor_x - rect->x1) + letter_spacing) / 2 *
                                                align_factor;
                        line++;
                        line_start_index = i;
                        cursor_x = rect->x1;
                    }
                    else if (i > 0 && chr[i - 1].unicode == 0x0A)
                    {
                        line_buf[line].index = i - 1;
                        line_buf[line].offset = (rect_w - (cursor_x - rect->x1) + letter_spacing * 2) / 2 *
                                                align_factor;
                        line++;
                        line_start_index = i;
                        cursor_x = rect->x1;
                    }

                    layout_position_bearing(&chr[i], &cursor_x, rect->y1 + line * line_height,
                                            typo_ctx.baseline_px, letter_spacing);
                    if (chr[i].y + chr[i].h - 1 > rect->y2)
                    {
                        active_font_len = i;
                        break;
                    }
                }
            }
            else
            {
                /* V1: char_w stepping */
                for (uint16_t i = 0; i < font_len; i++)
                {
                    if (chr[i].unicode == 0x20)
                    {
                        last_space_index = i;
                    }

                    if (i == 0)
                    {
                        chr[i].x = rect->x1;
                    }
                    else
                    {
                        chr[i].x = chr[i - 1].x + chr[i - 1].char_w + letter_spacing;
                    }

                    if ((chr[i].x + chr[i].char_w - 1) > rect->x2)
                    {
                        if (chr[i].unicode == 0x20 && chr[i].x < rect->x2)
                        {
                            chr[i].y = rect->y1 + line * line_height;
                            chr[i].char_w = rect->x2 + 1 - chr[i].x;
                            continue;
                        }
                        if (wordwrap && i != font_len - 1 &&
                            last_space_index > line_start_index && chr[last_space_index].unicode == 0x20)
                        {
                            i = last_space_index + 1;
                        }
                        line_buf[line].index = i - 1;
                        line_buf[line].offset = (rect_w - chr[i].x + rect->x1 + letter_spacing) / 2 * align_factor;
                        line++;
                        line_start_index = i;
                        chr[i].x = rect->x1;
                    }
                    else if (chr[i - 1].unicode == 0x0A)
                    {
                        line_buf[line].index = i - 1;
                        line_buf[line].offset = (rect_w - chr[i].x + rect->x1 + letter_spacing * 2) / 2 *
                                                align_factor;
                        line++;
                        line_start_index = i;
                        chr[i].x = rect->x1;
                    }

                    chr[i].y = rect->y1 + line * line_height;
                    if (chr[i].y + chr[i].h - 1 > rect->y2)
                    {
                        active_font_len = i;
                        break;
                    }
                }
            }
            if (active_font_len == 0)
            {
                char_line_sum = line;
                char_height_sum = line * line_height;
                gui_free(line_buf);
                return;
            }
            line_buf[line].index = active_font_len - 1;
            if (typo_ctx.is_v2)
            {
                /* V2: last line align offset based on last glyph x + char_w */
                line_buf[line].offset = (rect_w - (chr[active_font_len - 1].x - rect->x1) -
                                         chr[active_font_len - 1].char_w) / 2 * align_factor;
            }
            else
            {
                line_buf[line].offset = (rect_w - chr[active_font_len - 1].x + rect->x1 -
                                         chr[active_font_len - 1].char_w) / 2 * align_factor;
            }
            char_height_sum = (line + 1) * line_height;
            int32_t offset_y = vertical_center ? (rect_h - char_height_sum) / 2 : 0;
            line = 0;
            for (uint16_t i = 0; i < active_font_len; i++)
            {
                chr[i].x += line_buf[line].offset;
                chr[i].y += offset_y;
                if (i >= line_buf[line].index)
                {
                    line++;
                }
            }
            char_line_sum = line;
            gui_free(line_buf);
            break;
        }
    case SCROLL_X:
    case SCROLL_X_REVERSE:
    case SCROLL_X_MID:
    case SCROLL_X_MID_REVERSE:
        {
            char_line_sum = 1;
            active_font_len = font_len;
            uint32_t offset_y = (text_mode >= SCROLL_X_MID) ?
                                (rect_h - text->font_height) / 2 : 0;
            if (typo_ctx.is_v2)
            {
                /* V2: bearing-based horizontal scroll positioning */
                int16_t cursor_x = rect->x1;
                int16_t line_y = rect->y1 + offset_y;
                for (uint16_t i = 0; i < font_len; i++)
                {
                    layout_position_bearing(&chr[i], &cursor_x, line_y,
                                            typo_ctx.baseline_px, letter_spacing);
                    if (chr[i].x > rect->xboundright)
                    {
                        active_font_len = i;
                        break;
                    }
                }
            }
            else
            {
                /* V1: char_w stepping */
                for (uint16_t i = 0; i < font_len; i++)
                {
                    chr[i].y = rect->y1 + offset_y;
                    if (i == 0)
                    {
                        chr[i].x = rect->x1;
                    }
                    else
                    {
                        chr[i].x = chr[i - 1].x + chr[i - 1].char_w + letter_spacing;
                    }
                    if (chr[i].x > rect->xboundright)
                    {
                        active_font_len = i;
                        break;
                    }
                }
            }
            char_height_sum = text->font_height;
            break;
        }
    case SCROLL_Y:
        {
            uint32_t line = 0;
            int32_t last_space_index = 0;
            int32_t line_start_index = 0;
            /* V2/V1 line height */
            int32_t line_height = typo_ctx.is_v2 ?
                                  ((text->line_height > 0 ? text->line_height
                                    : typo_ctx.default_line_height) + line_spacing) :
                                  (chr[0].h + line_spacing);
            active_font_len = font_len;

            if (typo_ctx.is_v2)
            {
                /* V2: bearing-based vertical scroll layout */
                int16_t cursor_x = rect->x1;
                for (uint16_t i = 0; i < font_len; i++)
                {
                    if (chr[i].unicode == 0x20)
                    {
                        last_space_index = i;
                    }

                    /* Check advance for line wrap */
                    int16_t next_cursor = cursor_x + chr[i].advance;
                    if ((next_cursor - 1) > rect->x2 && cursor_x != rect->x1)
                    {
                        if (chr[i].unicode == 0x20 && cursor_x < rect->x2)
                        {
                            layout_position_bearing(&chr[i], &cursor_x, rect->y1 + line * line_height,
                                                    typo_ctx.baseline_px, letter_spacing);
                            chr[i].char_w = rect->x2 + 1 - chr[i].x;
                            continue;
                        }
                        if (wordwrap && i != font_len - 1 &&
                            last_space_index > line_start_index && chr[last_space_index].unicode == 0x20)
                        {
                            i = last_space_index + 1;
                        }
                        line++;
                        line_start_index = i;
                        cursor_x = rect->x1;
                    }
                    else if (i > 0 && chr[i - 1].unicode == 0x0A)
                    {
                        line++;
                        line_start_index = i;
                        cursor_x = rect->x1;
                    }
                    layout_position_bearing(&chr[i], &cursor_x, rect->y1 + line * line_height,
                                            typo_ctx.baseline_px, letter_spacing);
                    if (text->layout_refresh == false)
                    {
                        if (chr[i].y >= rect->yboundbottom)
                        {
                            active_font_len = i;
                            break;
                        }
                    }
                }
            }
            else
            {
                /* V1: char_w stepping */
                for (uint16_t i = 0; i < font_len; i++)
                {
                    if (chr[i].unicode == 0x20)
                    {
                        last_space_index = i;
                    }
                    if (i == 0)
                    {
                        chr[i].x = rect->x1;
                    }
                    else
                    {
                        chr[i].x = chr[i - 1].x + chr[i - 1].char_w + letter_spacing;
                    }
                    if ((chr[i].x + chr[i].char_w - 1) > rect->x2)
                    {
                        if (chr[i].unicode == 0x20 && chr[i].x < rect->x2)
                        {
                            chr[i].y = rect->y1 + line * line_height;
                            chr[i].char_w = rect->x2 + 1 - chr[i].x;
                            continue;
                        }
                        if (wordwrap && i != font_len - 1 &&
                            last_space_index > line_start_index && chr[last_space_index].unicode == 0x20)
                        {
                            i = last_space_index + 1;
                        }
                        line++;
                        line_start_index = i;
                        chr[i].x = rect->x1;
                    }
                    else if (chr[i - 1].unicode == 0x0A)
                    {
                        line++;
                        line_start_index = i;
                        chr[i].x = rect->x1;
                    }
                    chr[i].y = rect->y1 + line * line_height;
                    if (text->layout_refresh == false)
                    {
                        if (chr[i].y >= rect->yboundbottom)
                        {
                            active_font_len = i;
                            break;
                        }
                    }
                }
            }
            line ++;
            if (char_line_sum <= line)
            {
                char_line_sum = line;
            }
            break;
        }
    case SCROLL_Y_REVERSE:
        {
            for (uint16_t i = 0; i < font_len; i++)
            {
                chr[i].x = rect->x1;
                if (i == 0)
                {
                    chr[i].y = rect->y2 - chr[i].char_h;
                }
                else
                {
                    chr[i].y = chr[i - 1].y - chr[i].char_h - 2;
                }
            }
            char_height_sum = font_len * chr[0].char_h + (font_len - 1) * 2;
            break;
        }
    case VERTICAL_LEFT_TOP:
    case VERTICAL_LEFT_MID:
    case VERTICAL_LEFT_BOT:
        {
            gui_text_line_t *line_buf;
            uint32_t line = 0;
            int32_t line_height = chr[0].h + line_spacing;
            int32_t align_factor = text_mode - VERTICAL_LEFT_TOP;

            int32_t line_count = rect_w / line_height + 1;
            line_buf = gui_malloc(line_count * sizeof(gui_text_line_t));
            GUI_ASSERT(line_buf != NULL);
            memset(line_buf, 0, line_count * sizeof(gui_text_line_t));

            for (uint16_t i = 0; i < font_len; i++)
            {
                if (i == 0)
                {
                    chr[i].y = rect->y1;
                }
                else
                {
                    chr[i].y = chr[i - 1].y + chr[i - 1].char_h + chr[i - 1].char_y + letter_spacing;
                }
                if ((chr[i].y + chr[i].char_y + chr[i].char_h - 1) > rect->y2
                    || (i != 0 && chr[i - 1].unicode == 0x0A))
                {
                    line_buf[line].index = i - 1;
                    line_buf[line].offset = (rect_h - chr[i].y + rect->y1 + letter_spacing) / 2 * align_factor;
                    line++;
                    chr[i].y = rect->y1;
                }
                chr[i].x = rect->x2 - line * line_height - chr[i].h + 1;
                if (chr[i].x < rect->x1)
                {
                    active_font_len = i;
                    break;
                }
            }
            if (active_font_len == 0)
            {
                char_line_sum = line;
                char_height_sum = line * line_height;
                gui_free(line_buf);
                return;
            }
            line_buf[line].index = active_font_len - 1;
            line_buf[line].offset = (rect_h - chr[active_font_len - 1].y + rect->y1 - chr[active_font_len -
                                                                                          1].char_y - chr[active_font_len - 1].char_h) / 2 * align_factor;
            line = 0;
            for (uint16_t i = 0; i < active_font_len; i++)
            {
                chr[i].y += line_buf[line].offset;

                if (i >= line_buf[line].index)
                {
                    line++;
                }
            }
            char_line_sum = line;
            char_height_sum = line * line_height;
            gui_free(line_buf);
            break;
        }
    case VERTICAL_RIGHT_TOP:
    case VERTICAL_RIGHT_MID:
    case VERTICAL_RIGHT_BOT:
        {
            gui_text_line_t *line_buf;
            uint32_t line = 0;
            int32_t line_height = chr[0].h + line_spacing;
            int32_t align_factor = VERTICAL_RIGHT_BOT - text_mode;

            int32_t line_count = rect_w / line_height + 1;
            line_buf = gui_malloc(line_count * sizeof(gui_text_line_t));
            GUI_ASSERT(line_buf != NULL);
            memset(line_buf, 0, line_count * sizeof(gui_text_line_t));

            for (uint16_t i = 0; i < font_len; i++)
            {
                if (i == 0)
                {
                    chr[i].y = rect->y2 - chr[i].char_h - chr[i].char_y;
                }
                else
                {
                    chr[i].y = chr[i - 1].y - chr[i].char_h - chr[i].char_y - letter_spacing;
                }
                if (chr[i].y < rect->y1
                    || (i != 0 && chr[i - 1].unicode == 0x0A))
                {
                    line_buf[line].index = i - 1;
                    line_buf[line].offset = (chr[i].y - rect->y1 + letter_spacing + chr[i].char_h + chr[i].char_y) / 2 *
                                            align_factor;
                    line++;
                    chr[i].y = rect->y2 - chr[i].char_h - chr[i].char_y;
                }
                chr[i].x = rect->x1 + line * line_height;
                if (chr[i].x + chr[i].h > rect->x2)
                {
                    active_font_len = i;
                    break;
                }
            }
            if (active_font_len == 0)
            {
                char_line_sum = line;
                char_height_sum = line * line_height;
                gui_free(line_buf);
                return;
            }
            line_buf[line].index = active_font_len - 1;
            line_buf[line].offset = (chr[active_font_len - 1].y - rect->y1) / 2 * align_factor;
            line = 0;
            for (uint16_t i = 0; i < active_font_len; i++)
            {
                chr[i].y -= line_buf[line].offset;

                if (i >= line_buf[line].index)
                {
                    line++;
                }
            }
            char_line_sum = line;
            char_height_sum = line * line_height;
            gui_free(line_buf);
            break;
        }
    default:
        break;
    }

    if (need_rtl)
    {
        for (uint32_t i = 0; i < active_font_len; i++)
        {
            chr[i].x = rect->x1 + rect_w - (chr[i].x - rect->x1) - chr[i].char_w;
        }
    }
    if (text->thai && mark_array_out)
    {
        active_font_len = post_process_thai_char_struct(chr, font_len, active_font_len, mark_count_out,
                                                        mark_array_out);
        gui_free(mark_array_out);
    }
    text->active_font_len = active_font_len;
    text->char_height_sum = char_height_sum;
    text->char_line_sum = char_line_sum;
    text->layout_refresh = false;
}

void gui_font_mem_unload(gui_text_t *text)
{
    if (text->data)
    {
        if (text->font_mode == FONT_SRC_FTL || text->font_mode == FONT_SRC_FILESYS)
        {
            mem_char_t *chr = text->data;
            for (int i = 0; i < text->font_len; i++)
            {
                if (chr[i].dot_addr && !font_glyph_cache_contains(chr[i].dot_addr))
                {
                    gui_free(chr[i].dot_addr);
                }
            }
        }
        for (int i = 0; i < text->font_len; i++)
        {
            mem_char_t *chr = text->data;
            if (chr[i].emoji_img != NULL)
            {
                gui_obj_tree_free(chr[i].emoji_img);
            }
        }
        gui_free(text->data);
        text->data = NULL;
    }
    return;
}

void gui_font_mem_destroy(gui_text_t *text)
{
    if (text->data)
    {
        if (text->font_mode == FONT_SRC_FTL || text->font_mode == FONT_SRC_FILESYS)
        {
            mem_char_t *chr = text->data;
            for (int i = 0; i < text->font_len; i++)
            {
                if (chr[i].dot_addr && !font_glyph_cache_contains(chr[i].dot_addr))
                {
                    gui_free(chr[i].dot_addr);
                }
            }
        }
        gui_free(text->data);
        text->data = NULL;
    }
    return;
}

static void rtk_draw_unicode(mem_char_t *chr, gui_color_t color, uint8_t render_mode,
                             gui_text_rect_t *rect, bool crop)
{
    (void)crop;
    if (chr->dot_addr == NULL) { return; }
    gui_dispdev_t *dc = gui_get_dc();

    const int x_start = _UI_MAX3(chr->x, rect->xboundleft, dc->section.x1);
    const int x_end = _UI_MIN3(
                          chr->x + chr->char_w - 1,
                          rect->xboundright ? rect->xboundright : dc->section.x2,
                          dc->section.x2
                      );

    const int y_start = _UI_MAX3(chr->y + chr->char_y, rect->yboundtop, dc->section.y1);
    const int y_end = _UI_MIN3(
                          chr->y + chr->char_y + chr->char_h - 1,
                          rect->yboundbottom ? rect->yboundbottom : dc->section.y2,
                          dc->section.y2
                      );
    if (x_start > x_end || y_start > y_end) { return; }

    draw_font_t df =
    {
        .color = color,
        .render_mode = render_mode,
        .target_buf = dc->frame_buf,
        .target_buf_stride = dc->fb_width * dc->bit_depth / 8,
        .clip_rect = {
            .x1 = x_start,
            .y1 = y_start,
            .x2 = x_end,
            .y2 = y_end,
        },
        .target_rect = {
            .x1 = dc->section.x1,
            .y1 = dc->section.y1,
            .x2 = dc->section.x2,
            .y2 = dc->section.y2,
        }

    };

    switch (dc->bit_depth)
    {
    case 16:
        df.target_format = RGB565;
        break;
    case 24:
        df.target_format = RGB888;
        // df.target_format = ARGB8565;
        break;
    case 32:
        df.target_format = ARGB8888;
        break;
    case 8:
        df.target_format = GRAY8;
        break;
    default:
        break;
    }

    font_glyph_t glyph =
    {
        .data = chr->dot_addr,
        .pos_x = chr->x,
        .pos_y = chr->y + chr->char_y,
        .width = chr->char_w,
        .height = chr->char_h,
        .stride = chr->w,
    };

    font_glyph_render(&df, &glyph);
}
void gui_font_draw_emoji(gui_text_t *text, mem_char_t *chr, void *data, int16_t x, int16_t y)
{
    if (data)
    {
        if (chr->emoji_img == NULL)
        {
            chr->emoji_img = gui_img_create_from_mem(text, "emoji", data, x, y, 0, 0);
            float x_sccle = (float)(chr->char_w) / (float)(text->emoji_size);
            float y_scale = (float)(chr->char_h) / (float)(text->emoji_size);
            gui_img_scale((gui_img_t *)chr->emoji_img, x_sccle, y_scale);
        }
    }
}
void gui_font_mem_draw(gui_text_t *text, gui_text_rect_t *rect)
{
    mem_char_t *chr = text->data;
    GUI_FONT_HEAD_BMP *font;
    if (text->font_mode == FONT_SRC_FTL || text->font_mode == FONT_SRC_FILESYS)
    {
        FONT_LIB_NODE *node = get_fontlib_by_name(text->path);
        font = (node != NULL) ? (GUI_FONT_HEAD_BMP *)node->cached_data : NULL;
    }
    else
    {
        font = (GUI_FONT_HEAD_BMP *)text->path;
    }
    if (font == NULL)
    {
        return;
    }
    uint8_t render_mode = font->render_mode;
    gui_color_t outcolor = text->color;
    outcolor.color.rgba.a = text->color.color.rgba.a * text->base.parent->opacity_value / 0xff;
    for (uint16_t i = 0; i < text->active_font_len; i++)
    {
        if (chr[i].unicode >= 0x10000)
        {
            gui_font_draw_emoji(text, chr + i, chr[i].dot_addr, chr[i].x - rect->x1, chr[i].y - rect->y1);
        }
        else
        {
            uint8_t chr_render_mode = chr[i].render_mode ? chr[i].render_mode : render_mode;
            rtk_draw_unicode(chr + i, outcolor, chr_render_mode, rect, font->crop);
        }
    }

#if 0 /* Enable to print glyph cache stats per draw call */
    {
        uint32_t hit, miss, used, total;
        font_glyph_cache_stats(&hit, &miss, &used, &total);
        gui_log("glyph cache: hit=%u miss=%u used=%u/%u\n", hit, miss, used, total);
    }
#endif
}

uint8_t gui_font_mem_init_ftl(uint8_t *font_bin_addr)
{
    if (!font_bin_addr)
    {
        return UINT8_MAX;
    }

    /* Check if already loaded */
    FONT_LIB_NODE *node = gui_font_lib_find(font_bin_addr);
    if (node != NULL && node->font_type == GUI_FONT_SRC_BMP)
    {
        gui_font_lib_addref(node);
        return 0;
    }

    uint8_t *data = gui_malloc(sizeof(GUI_FONT_HEAD_BMP));
    GUI_ASSERT(data != NULL);
    gui_ftl_read((uintptr_t)font_bin_addr, data, sizeof(GUI_FONT_HEAD_BMP));

    GUI_FONT_HEAD_BMP *font = (GUI_FONT_HEAD_BMP *)data;
    if (font->file_type != FONT_FILE_BMP_FLAG)
    {
        gui_log("this font file is not valid \n");
        gui_free(data);
        return UINT8_MAX;
    }

    uint32_t head_index_len = font->head_length + font->index_area_size;
    data = gui_realloc(data, head_index_len);
    font = (GUI_FONT_HEAD_BMP *)data;
    gui_ftl_read((uintptr_t)font_bin_addr, data, head_index_len);

    /* Register to font_lib_manager */
    node = gui_font_lib_register(font_bin_addr, font->font_size, FONT_SRC_FTL,
                                 GUI_FONT_SRC_BMP, data, head_index_len);
    if (node == NULL)
    {
        gui_free(data);
        return UINT8_MAX;
    }

    return 0;
}

uint8_t gui_font_mem_init_fs(uint8_t *font_path)
{
    if (!font_path)
    {
        return UINT8_MAX;
    }

    /* Check if already loaded */
    FONT_LIB_NODE *node = gui_font_lib_find(font_path);
    if (node != NULL && node->font_type == GUI_FONT_SRC_BMP)
    {
        gui_font_lib_addref(node);
        return 0;
    }

    /* Open font file and read header */
    gui_vfs_file_t *file = gui_vfs_open((const char *)font_path, GUI_VFS_READ);
    if (file == NULL)
    {
        gui_log("gui_font_mem_init_fs: failed to open %s\n", font_path);
        return UINT8_MAX;
    }

    /* Read font header */
    uint8_t *data = gui_malloc(sizeof(GUI_FONT_HEAD_BMP));
    if (data == NULL)
    {
        gui_vfs_close(file);
        return UINT8_MAX;
    }

    int read_size = gui_vfs_read(file, data, sizeof(GUI_FONT_HEAD_BMP));
    if (read_size != sizeof(GUI_FONT_HEAD_BMP))
    {
        gui_log("gui_font_mem_init_fs: failed to read header\n");
        gui_free(data);
        gui_vfs_close(file);
        return UINT8_MAX;
    }

    GUI_FONT_HEAD_BMP *font = (GUI_FONT_HEAD_BMP *)data;
    if (font->file_type != FONT_FILE_BMP_FLAG)
    {
        gui_log("gui_font_mem_init_fs: invalid font file\n");
        gui_free(data);
        gui_vfs_close(file);
        return UINT8_MAX;
    }

    /* Read header + index area */
    uint32_t head_index_len = font->head_length + font->index_area_size;
    data = gui_realloc(data, head_index_len);
    if (data == NULL)
    {
        gui_vfs_close(file);
        return UINT8_MAX;
    }

    gui_vfs_seek(file, 0, GUI_VFS_SEEK_SET);
    read_size = gui_vfs_read(file, data, head_index_len);
    gui_vfs_close(file);

    if (read_size != (int)head_index_len)
    {
        gui_log("gui_font_mem_init_fs: failed to read index area\n");
        gui_free(data);
        return UINT8_MAX;
    }

    font = (GUI_FONT_HEAD_BMP *)data;

    /* Register to font_lib_manager */
    node = gui_font_lib_register(font_path, font->font_size, FONT_SRC_FILESYS,
                                 GUI_FONT_SRC_BMP, data, head_index_len);
    if (node == NULL)
    {
        gui_free(data);
        return UINT8_MAX;
    }

    gui_log("gui_font_mem_init_fs: loaded %s, size=%d\n", font_path, font->font_size);
    return 0;
}

uint8_t gui_font_mem_init_mem(uint8_t *font_bin_addr)
{
    return gui_font_mem_init(font_bin_addr);
}

uint8_t gui_font_mem_init(uint8_t *font_bin_addr)
{
    if (!font_bin_addr)
    {
        return UINT8_MAX;
    }

    /* Check if already loaded */
    FONT_LIB_NODE *node = gui_font_lib_find(font_bin_addr);
    if (node != NULL && node->font_type == GUI_FONT_SRC_BMP)
    {
        gui_font_lib_addref(node);
        return 0;
    }

    GUI_FONT_HEAD_BMP *font = (GUI_FONT_HEAD_BMP *)font_bin_addr;
    if (font->file_type != FONT_FILE_BMP_FLAG)
    {
        gui_log("this font file is not valid \n");
        return UINT8_MAX;
    }

    /* Register to font_lib_manager (no cached data for MEM mode) */
    node = gui_font_lib_register(font_bin_addr, font->font_size, FONT_SRC_MEMADDR,
                                 GUI_FONT_SRC_BMP, NULL, 0);
    if (node == NULL)
    {
        return UINT8_MAX;
    }

    return 0;
}

uint8_t gui_font_mem_delete(uint8_t *font_bin_addr)
{
    if (!font_bin_addr)
    {
        return UINT8_MAX;
    }

    /* Release from font_lib_manager */
    gui_font_lib_release(font_bin_addr);
    return 0;
}

/*============================================================================*
 *                V2 Typography Functions (Standard Typography Model)
 *============================================================================*/

bool gui_font_bmp_parse_typo_metrics(const GUI_FONT_HEAD_BMP *header,
                                     gui_font_typo_metrics_t *out_metrics)
{
    if (header == NULL || out_metrics == NULL)
    {
        return false;
    }

    /* V1 header - no typography extension */
    if (header->version[0] < 3)
    {
        return false;
    }

    /*
     * V2 extension fields are appended after font_name.
     * Calculate the byte offset where extension starts:
     *   head_length(1) + file_type(1) + version(4) + font_size(1) +
     *   render_mode(1) + bitfield(1) + index_area_size(4) +
     *   font_name_length(1) + font_name(N) = 14 + font_name_length
     */
    uint8_t v1_end = 14 + header->font_name_length;

    /* Verify head_length can hold the 8-byte extension */
    if (header->head_length < v1_end + 8)
    {
        return false;
    }

    const uint8_t *base = (const uint8_t *)header;
    const uint8_t *ext = base + v1_end;

    out_metrics->ascender     = (int16_t)(ext[0] | (ext[1] << 8));
    out_metrics->descender    = (int16_t)(ext[2] | (ext[3] << 8));
    out_metrics->line_gap     = (int16_t)(ext[4] | (ext[5] << 8));
    out_metrics->units_per_em = (uint16_t)(ext[6] | (ext[7] << 8));

    if (out_metrics->units_per_em == 0)
    {
        gui_log("Warning: V2 font header has units_per_em == 0\n");
        return false;
    }

    return true;
}

gui_font_typo_layout_t gui_font_typo_calc_layout(const gui_font_typo_metrics_t *metrics,
                                                 uint8_t font_size)
{
    gui_font_typo_layout_t layout = {0};

    if (metrics == NULL || metrics->units_per_em == 0 || font_size == 0)
    {
        return layout;
    }

    int16_t asc  = metrics->ascender;
    int16_t desc = metrics->descender;   /* negative */
    int16_t gap  = metrics->line_gap;
    uint16_t upm = metrics->units_per_em;

    /* Integer arithmetic with truncation rounding for deterministic embedded results */
    layout.ascent_px   = (int16_t)((int32_t)asc * font_size / upm);
    layout.descent_px  = (int16_t)((int32_t)(-desc) * font_size / upm);
    layout.baseline    = layout.ascent_px;
    layout.line_height = (int16_t)((int32_t)(asc - desc + gap) * font_size / upm);

    return layout;
}

gui_font_typo_context_t gui_font_bmp_get_typo_context(const GUI_FONT_HEAD_BMP *header,
                                                      uint8_t font_size)
{
    gui_font_typo_context_t ctx;
    memset(&ctx, 0, sizeof(ctx));

    if (gui_font_bmp_parse_typo_metrics(header, &ctx.metrics))
    {
        ctx.is_v2 = true;
        gui_font_typo_layout_t layout = gui_font_typo_calc_layout(&ctx.metrics, font_size);
        ctx.baseline_px = layout.baseline;
        ctx.default_line_height = layout.line_height;
    }
    else
    {
        ctx.is_v2 = false;
        ctx.baseline_px = 0;
        ctx.default_line_height = (int16_t)font_size;
    }

    return ctx;
}

