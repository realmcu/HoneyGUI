/**
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file font_mem.c
  * @brief realui memory font engine
  * @details realui memory font engine
  * @author luke_sun@realsil.com.cn
  * @date 2025/12/31
  * @version v3.0 - Refactored to use font_lib_manager
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2025 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

#include <string.h>
#include "draw_font.h"
#include "font_mem.h"
#include "font_lib_manager.h"
#include "font_rendering_utils.h"
#include "gui_vfs.h"

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

    gui_vfs_file_t *file = gui_vfs_open((const char *)node->font_file, GUI_VFS_READ);
    if (file == NULL)
    {
        return -1;
    }

    if (gui_vfs_seek(file, offset, GUI_VFS_SEEK_SET) < 0)
    {
        gui_vfs_close(file);
        return -1;
    }

    int read_size = gui_vfs_read(file, buf, size);
    gui_vfs_close(file);

    return read_size;
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
    }
    text->data = chr;
    text->content_refresh = false;

    int32_t all_char_w = 0;
    int32_t all_char_h = 0;
    uint32_t line_flag = 0;
    int32_t line_byte = 0;

    uint32_t uni_i = 0;
    uint32_t chr_i = 0;

    if (crop)
    {
        switch (index_method)
        {
        case 0: //address
            for (uni_i = 0; uni_i < unicode_len; uni_i++)
            {
                chr[chr_i].unicode = unicode_buf[uni_i];
                chr[chr_i].w = 0;
                chr[chr_i].h = text->font_height;
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
                    if (text->emoji_path)
                    {
                        char file_path[100];
                        memset(file_path, 0, 100);
                        snprintf(file_path, sizeof(file_path), "%s", text->emoji_path);
                        uint32_t multi_unicode_len = generate_emoji_file_path_from_unicode(&unicode_buf[uni_i],
                                                                                           unicode_len - uni_i, file_path);
                        // gui_log("emoji len %d, path %s\n", multi_unicode_len,file_path);
                        if (multi_unicode_len != 0)
                        {
                            chr[chr_i].dot_addr = (void *)gui_vfs_get_file_address(file_path);
                            if (chr[chr_i].dot_addr != NULL)
                            {
                                chr[chr_i].char_w = text->font_height;
                                chr[chr_i].char_h = text->font_height;
                            }
                            else
                            {
                                chr[chr_i].char_w = 0;
                                chr[chr_i].char_h = 0;
                            }
                            uni_i += multi_unicode_len - 1;
                        }
                        else
                        {
                            gui_log("No valid emoji\n");
                        }
                    }
                }
                else
                {
                    uint32_t *offset_addr = (uint32_t *)((uint8_t *)(uintptr_t)table_offset + chr[chr_i].unicode *
                                                         index_unit_length);
                    offset = *offset_addr;
                    if (offset == 0xFFFFFFFF) { continue; }
                    if (text->font_mode == FONT_SRC_MEMADDR)
                    {
                        chr[chr_i].dot_addr = (uint8_t *)text->path + offset + 4;
                        chr[chr_i].char_w = (uint8_t)(*(chr[chr_i].dot_addr - 2));
                        chr[chr_i].char_y = (uint8_t)(*(chr[chr_i].dot_addr - 4));
                        chr[chr_i].char_h = (uint8_t)(*(chr[chr_i].dot_addr - 1)) - chr[chr_i].char_y;
                        line_byte = (chr[chr_i].char_w * render_mode + 8 - 1) / 8;
                        chr[chr_i].w = line_byte * 8 / render_mode;
                    }
                    else if (text->font_mode == FONT_SRC_FTL)
                    {
                        chr[chr_i].dot_addr = (uint8_t *)text->path + offset + 4;
                        uint8_t header[4];
                        gui_ftl_read((uintptr_t)chr[chr_i].dot_addr - 4, header, 4);

                        chr[chr_i].char_y = header[0];
                        chr[chr_i].char_w = header[2];
                        chr[chr_i].char_h = header[3];

                        chr[chr_i].char_h = chr[chr_i].char_h - chr[chr_i].char_y;
                        line_byte = (chr[chr_i].char_w * render_mode + 8 - 1) / 8;
                        chr[chr_i].w = line_byte * 8 / render_mode;

                        uint32_t dot_size = line_byte * chr[chr_i].char_h;
                        uint8_t *dot_buf = gui_malloc(dot_size);
                        gui_ftl_read((uintptr_t)chr[chr_i].dot_addr, dot_buf, dot_size);

                        chr[chr_i].dot_addr = dot_buf;
                    }
                    else if (text->font_mode == FONT_SRC_FILESYS)
                    {
                        /* For crop mode, offset points directly to char data in file */
                        uint32_t file_offset = offset;
                        uint8_t header[4];
                        int ret = font_fs_read(node, file_offset, header, 4);
                        if (ret < 0)
                        {
                            continue;
                        }

                        chr[chr_i].char_y = header[0];
                        chr[chr_i].char_w = header[2];
                        chr[chr_i].char_h = header[3] - chr[chr_i].char_y;
                        line_byte = (chr[chr_i].char_w * render_mode + 8 - 1) / 8;
                        chr[chr_i].w = line_byte * 8 / render_mode;

                        uint32_t dot_size = line_byte * chr[chr_i].char_h;
                        uint8_t *dot_buf = gui_malloc(dot_size);
                        if (dot_buf == NULL)
                        {
                            continue;
                        }
                        ret = font_fs_read(node, file_offset + 4, dot_buf, dot_size);
                        if (ret < 0)
                        {
                            gui_free(dot_buf);
                            continue;
                        }
                        chr[chr_i].dot_addr = dot_buf;
                    }
                }
                all_char_w += chr[chr_i].char_w;
                chr_i ++;
            }
            break;
        case 1: //offset
            {
                //todo by luke
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
                    if (text->emoji_path)
                    {
                        char file_path[100];
                        memset(file_path, 0, 100);
                        memcpy(file_path, text->emoji_path, strlen((const char *)text->emoji_path));
                        uint32_t multi_unicode_len = generate_emoji_file_path_from_unicode(&unicode_buf[uni_i],
                                                                                           unicode_len - uni_i, file_path);
                        // gui_log("emoji len %d, path %s\n", multi_unicode_len,file_path);
                        if (multi_unicode_len != 0)
                        {
                            chr[chr_i].dot_addr = (void *)gui_vfs_get_file_address(file_path);
                            if (chr[chr_i].dot_addr != NULL)
                            {
                                chr[chr_i].char_w = text->font_height;
                                chr[chr_i].char_h = text->font_height;
                            }
                            else
                            {
                                chr[chr_i].char_w = 0;
                                chr[chr_i].char_h = 0;
                            }
                            uni_i += multi_unicode_len - 1;
                        }
                        else
                        {
                            gui_log("No valid emoji\n");
                        }
                    }
                }
                else
                {
                    offset = *(uint16_t *)(uintptr_t)(chr[chr_i].unicode * 2 + table_offset);
                    if (offset == 0xFFFF) { continue; }
                    if (text->font_mode == FONT_SRC_MEMADDR)
                    {
                        chr[chr_i].dot_addr = (uint8_t *)(uintptr_t)((uintptr_t)offset * font_area + dot_offset + 4);
                        chr[chr_i].char_w = (int16_t)(*(chr[chr_i].dot_addr - 2));
                        chr[chr_i].char_h = (int16_t)(*(chr[chr_i].dot_addr - 1));
                    }
                    else if (text->font_mode == FONT_SRC_FTL)
                    {
                        chr[chr_i].dot_addr = (uint8_t *)(uintptr_t)((uintptr_t)offset * font_area + dot_offset + 4);
                        uint8_t header[2];
                        gui_ftl_read((uintptr_t)chr[chr_i].dot_addr - 2, header, 2);
                        chr[chr_i].char_w = (int16_t)header[0];
                        chr[chr_i].char_h = (int16_t)header[1];
                        uint32_t dot_size = aliened_font_size * text->font_height / 8 * render_mode;
                        uint8_t *dot_buf = gui_malloc(dot_size);
                        gui_ftl_read((uintptr_t)chr[chr_i].dot_addr, dot_buf, dot_size);
                        chr[chr_i].dot_addr = dot_buf;
                    }
                    else if (text->font_mode == FONT_SRC_FILESYS)
                    {
                        uint32_t file_offset = (uint32_t)offset * font_area + dot_offset;
                        uint8_t header[2];
                        int ret = font_fs_read(node, file_offset + 2, header, 2);
                        if (ret < 0)
                        {
                            continue;
                        }
                        chr[chr_i].char_w = (int16_t)header[0];
                        chr[chr_i].char_h = (int16_t)header[1];
                        uint32_t dot_size = aliened_font_size * text->font_height / 8 * render_mode;
                        uint8_t *dot_buf = gui_malloc(dot_size);
                        if (dot_buf == NULL)
                        {
                            continue;
                        }
                        ret = font_fs_read(node, file_offset + 4, dot_buf, dot_size);
                        if (ret < 0)
                        {
                            gui_free(dot_buf);
                            continue;
                        }
                        chr[chr_i].dot_addr = dot_buf;
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
                        if (text->emoji_path)
                        {
                            char file_path[100];
                            memset(file_path, 0, 100);
                            memcpy(file_path, text->emoji_path, strlen((const char *)text->emoji_path));
                            uint32_t multi_unicode_len = generate_emoji_file_path_from_unicode(&unicode_buf[uni_i],
                                                                                               unicode_len - uni_i, file_path);
                            // gui_log("emoji len %d, path %s\n", multi_unicode_len,file_path);
                            if (multi_unicode_len != 0)
                            {
                                chr[chr_i].dot_addr = (void *)gui_vfs_get_file_address(file_path);
                                if (chr[chr_i].dot_addr != NULL)
                                {
                                    chr[chr_i].char_w = text->font_height;
                                    chr[chr_i].char_h = text->font_height;
                                }
                                else
                                {
                                    chr[chr_i].char_w = 0;
                                    chr[chr_i].char_h = 0;
                                }
                                uni_i += multi_unicode_len - 1;
                            }
                            else
                            {
                                gui_log("No valid emoji\n");
                            }
                        }
                    }
                    else
                    {
                        int32_t index = font_index_bsearch_bmp(table_offset, index_area_size, chr[chr_i].unicode);
                        if (index < 0)
                        {
                            gui_log("Character %x not found in BMP-BIN file \n", chr[chr_i].unicode);
                            continue;
                        }
                        if (text->font_mode == FONT_SRC_MEMADDR)
                        {
                            chr[chr_i].dot_addr = (uint8_t *)(uintptr_t)((uintptr_t)index * font_area + dot_offset + 4);
                            chr[chr_i].char_w = (int16_t)(*(chr[chr_i].dot_addr - 2));
                            chr[chr_i].char_h = (int16_t)(*(chr[chr_i].dot_addr - 1));
                        }
                        else if (text->font_mode == FONT_SRC_FTL)
                        {
                            chr[chr_i].dot_addr = (uint8_t *)(uintptr_t)((uintptr_t)index * font_area + dot_offset + 4);
                            uint8_t header[2];
                            gui_ftl_read((uintptr_t)chr[chr_i].dot_addr - 2, header, 2);
                            chr[chr_i].char_w = (int16_t)header[0];
                            chr[chr_i].char_h = (int16_t)header[1];
                            uint32_t dot_size = aliened_font_size * text->font_height / 8 * render_mode;
                            uint8_t *dot_buf = gui_malloc(dot_size);
                            gui_ftl_read((uintptr_t)chr[chr_i].dot_addr, dot_buf, dot_size);
                            chr[chr_i].dot_addr = dot_buf;
                        }
                        else if (text->font_mode == FONT_SRC_FILESYS)
                        {
                            uint32_t file_offset = (uint32_t)index * font_area + dot_offset;
                            uint8_t header[2];
                            font_fs_read(node, file_offset + 2, header, 2);
                            chr[chr_i].char_w = (int16_t)header[0];
                            chr[chr_i].char_h = (int16_t)header[1];
                            uint32_t dot_size = aliened_font_size * text->font_height / 8 * render_mode;
                            uint8_t *dot_buf = gui_malloc(dot_size);
                            font_fs_read(node, file_offset + 4, dot_buf, dot_size);
                            chr[chr_i].dot_addr = dot_buf;
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
            int32_t line_height = chr[0].h + line_spacing;
            int32_t align_factor = (text_mode <= MULTI_RIGHT) ?
                                   (text_mode - MULTI_LEFT) : (text_mode - MULTI_MID_LEFT);
            bool vertical_center = (text_mode >= MULTI_MID_LEFT);

            int32_t line_count = rect_h / line_height + 1;
            line_buf = gui_malloc(line_count * sizeof(gui_text_line_t));
            memset(line_buf, 0, line_count * sizeof(gui_text_line_t));

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
                    if (wordwrap && i != font_len - 1)
                    {
                        i = last_space_index + 1;
                    }
                    line_buf[line].index = i - 1;
                    line_buf[line].offset = (rect_w - chr[i].x + rect->x1 + letter_spacing) / 2 * align_factor;
                    line++;
                    chr[i].x = rect->x1;
                }
                else if (chr[i - 1].unicode == 0x0A)
                {
                    line_buf[line].index = i - 1;
                    line_buf[line].offset = (rect_w - chr[i].x + rect->x1 + letter_spacing * 2) / 2 * align_factor;
                    line++;
                    chr[i].x = rect->x1;
                }

                chr[i].y = rect->y1 + line * line_height;
                if (chr[i].y + chr[i].h - 1 > rect->y2)
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
            line_buf[line].offset = (rect_w - chr[active_font_len - 1].x + rect->x1 - chr[active_font_len -
                                                                                          1].char_w) / 2 * align_factor;
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
            break;
        }
    case SCROLL_Y:
        {
            uint32_t line = 0;
            int32_t last_space_index = 0;
            int32_t line_height = chr[0].h + line_spacing;
            active_font_len = font_len;

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
                    if (wordwrap && i != font_len - 1)
                    {
                        i = last_space_index + 1;
                    }
                    line++;
                    chr[i].x = rect->x1;
                }
                else if (chr[i - 1].unicode == 0x0A)
                {
                    line++;
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
                gui_free(chr[i].dot_addr);
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
                gui_free(chr[i].dot_addr);
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
            rtk_draw_unicode(chr + i, outcolor, render_mode, rect, font->crop);
        }
    }
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

uint32_t gui_get_mem_utf8_char_width(void *content, void *font_bin_addr)
{
    return gui_get_mem_char_width(content, font_bin_addr, UTF_8);
}

uint32_t gui_get_mem_char_width(void *content, void *font_bin_addr, TEXT_CHARSET charset)
{
    GUI_FONT_HEAD_BMP *font = (GUI_FONT_HEAD_BMP *)font_bin_addr;
    uint32_t string_len = strlen(content);
    uintptr_t table_offset = (uintptr_t)((uint8_t *)font_bin_addr + font->head_length);
    uintptr_t dot_offset = table_offset + font->index_area_size;

    uint8_t aliened_font_size = font->font_size;
    if (font->font_size % 8 != 0)
    {
        aliened_font_size = 8 - font->font_size % 8 + font->font_size;
    }
    uint32_t font_area = aliened_font_size * font->font_size / 8 * font->render_mode + 4;
    uint8_t index_unit_length = 4; //now set to 4 , todo
    uint32_t *unicode_buffer = NULL;
    uint16_t unicode_len = 0;
    unicode_len = process_content_by_charset(charset, content, string_len, &unicode_buffer);
    if (content_has_ap_unicode(unicode_buffer, unicode_len))
    {
        unicode_len = process_ap_unicode(unicode_buffer, unicode_len);
    }

    uint32_t all_char_w = 0;
    uint32_t line_flag = 0;

    if (font->crop)
    {
        switch (font->index_method)
        {
        case 0: //address
            for (uint32_t i = 0; i < unicode_len; i++)
            {
                uint32_t offset = 0;
                uint16_t char_w = 0;
                if (unicode_buffer[i] == 0x20 || unicode_buffer[i] == 0x0D)
                {
                    char_w = font->font_size / 4;
                }
                else if (unicode_buffer[i] == 0x0A)
                {
                    line_flag ++;
                    char_w = 0;
                }
                else
                {
                    uint32_t *offset_addr = (uint32_t *)((uint8_t *)(uintptr_t)table_offset + unicode_buffer[i] *
                                                         index_unit_length);
                    offset = *offset_addr;
                    if (offset == 0xFFFFFFFF) { continue; }
                    uint8_t *dot_addr = (uint8_t *)font_bin_addr + offset + 4;
                    char_w = (uint8_t)(*(dot_addr - 2));
                }
                all_char_w += char_w;
            }
            break;
        case 1: //offset
            for (uint32_t i = 0; i < unicode_len; i++)
            {
                //todo by luke
            }
            break;
        default:
            break;
        }
    }
    else
    {
        switch (font->index_method)
        {
        case 0: //address
            for (uint32_t i = 0; i < unicode_len; i++)
            {
                uint16_t offset = 0;
                uint16_t char_w = 0;
                if (unicode_buffer[i] == 0x20 || unicode_buffer[i] == 0x0D)
                {
                    char_w = font->font_size / 4;
                }
                else if (unicode_buffer[i] == 0x0A)
                {
                    line_flag ++;
                    char_w = 0;
                }
                else
                {
                    offset = *(uint16_t *)(uintptr_t)(unicode_buffer[i] * 2 + table_offset);
                    if (offset == 0xFFFF) { continue; }
                    uint8_t *dot_addr = (uint8_t *)(uintptr_t)((uintptr_t)offset * font_area + dot_offset + 4);
                    char_w = (int16_t)(*(dot_addr - 2));
                }
                all_char_w += char_w;
            }
            break;
        case 1: //offset
            for (uint32_t i = 0; i < unicode_len; i++)
            {
                uint16_t char_w = 0;
                if (unicode_buffer[i] == 0x0A)
                {
                    line_flag ++;
                    char_w = 0;
                }
                else if (unicode_buffer[i] == 0x20)
                {
                    char_w = font->font_size / 4;
                }
                else
                {
                    int32_t index = font_index_bsearch_bmp(table_offset, font->index_area_size, unicode_buffer[i]);
                    if (index >= 0)
                    {
                        uint8_t *dot_addr = (uint8_t *)(uintptr_t)((uintptr_t)index * font_area + dot_offset + 4);
                        char_w = (int16_t)(*(dot_addr - 2));
                    }
                }
                all_char_w += char_w;
            }
            break;
        default:
            break;
        }
    }
    gui_free(unicode_buffer);
    return all_char_w;
}
