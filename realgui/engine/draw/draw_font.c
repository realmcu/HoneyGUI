/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                           Header Files
 *============================================================================*/

#include "guidef.h"
#include "draw_font.h"
#include <string.h>

/*============================================================================*
 *                           Types
 *============================================================================*/

/*============================================================================*
 *                           Constants
 *============================================================================*/

/*============================================================================*
 *                           Macros
 *============================================================================*/

/*Arabic and Persian*/
#define UNDEF_ARABIC_PERSIAN_CHARS     (UINT32_MAX)
#define AP_ALPHABET_BASE_CODE          0x0622
#define AP_ALPHABET_END_CODE           0x06F9
#define AP_END_CHARS_LIST              {0,0,0,0,0,{0,0}}

/*Thai*/
#define THAI_ALPHABET_BASE_CODE        0x0E01
#define THAI_ALPHABET_END_CODE         0x0E5B

/*Hebrew*/
#define HEBREW_ALPHABET_BASE_CODE      0x0590
#define HEBREW_ALPHABET_END_CODE       0x05FF

/*============================================================================*
 *                           Variables
 *============================================================================*/

const ap_chars_map_t ap_chars_map[] =
{
    /*{Key Offset, End, Beginning, Middle, Isolated, {conjunction}}*/
    {0, 0xFE81, 0, 0, 0,  {0, 0}},      // أ
    {1, 0xFE84, -1, 0, -1,  {1, 0}},    // أ
    {2, 0xFE86, -1, 0, -1,  {1, 0}},    // ؤ
    {3, 0xFE88, -1, 0, -1,  {1, 0}},    // ﺇ
    {4, 0xFE8A, 1, 2, -1,  {1, 1}},    // ئ
    {5, 0xFE8E, -1, 0, -1,  {1, 0}},    // آ
    {6, 0xFE90, 1, 2, -1,  {1, 1}},    // ب
    {92, 0xFB57, 1, 2, -1,  {1, 1}},   // پ
    {8, 0xFE96, 1, 2, -1,  {1, 1}},    // ت
    {9, 0xFE9A, 1, 2, -1,  {1, 1}},    // ث
    {10, 0xFE9E, 1, 2, -1,  {1, 1}},   // ج
    {100, 0xFB7B, 1, 2, -1,  {1, 1}},  // چ
    {11, 0xFEA2, 1, 2, -1,  {1, 1}},   // ح
    {12, 0xFEA6, 1, 2, -1,  {1, 1}},   // خ
    {13, 0xFEAA, -1, 0, -1,  {1, 0}},   // د
    {14, 0xFEAC, -1, 0, -1,  {1, 0}},   // ذ
    {15, 0xFEAE, -1, 0, -1,  {1, 0}},   // ر
    {16, 0xFEB0, -1, 0, -1,  {1, 0}},   // ز
    {118, 0xFB8B, -1, 0, -1,  {1, 0}},  // ژ
    {17, 0xFEB2, 1, 2, -1,  {1, 1}},   // س
    {18, 0xFEB6, 1, 2, -1,  {1, 1}},   // ش
    {19, 0xFEBA, 1, 2, -1,  {1, 1}},   // ص
    {20, 0xFEBE, 1, 2, -1,  {1, 1}},   // ض
    {21, 0xFEC2, 1, 2, -1,  {1, 1}},   // ط
    {22, 0xFEC6, 1, 2, -1,  {1, 1}},   // ظ
    {23, 0xFECA, 1, 2, -1,  {1, 1}},   // ع
    {24, 0xFECE, 1, 2, -1,  {1, 1}},   // غ
    {30, 0x0640, 0, 0, 0,  {1, 1}},   // - (mad, hyphen)
    {31, 0xFED2, 1, 2, -1,  {1, 1}},   // ف
    {32, 0xFED6, 1, 2, -1,  {1, 1}},   // ق
    {135, 0xFB8F, 1, 2, -1,  {1, 1}},  // ک
    {33, 0xFEDA, 1, 2, -1,  {1, 1}},  // ﻙ
    {141, 0xFB93, 1, 2, -1,  {1, 1}},  // گ
    {34, 0xFEDE, 1, 2, -1,  {1, 1}},   // ل
    {35, 0xFEE2, 1, 2, -1,  {1, 1}},   // م
    {36, 0xFEE6, 1, 2, -1,  {1, 1}},   // ن
    {38, 0xFEEE, -1, 0, -1,  {1, 0}},   // و
    {37, 0xFEEA, 1, 2, -1,  {1, 1}},   // ه
    {39, 0xFEF0, 0, 0, -1, {1, 0}},   // ى
    {40, 0xFEF2, 1, 2, -1,  {1, 1}},   // ي
    {170, 0xFBFD, 1, 2, -1,  {1, 1}},   // ی
    {7, 0xFE94, -1, 2, -1,  {1, 0}},   // ة
    {206, 0x06F0, -1, 2, 0,  {0, 0}},  // ۰
    {207, 0x06F1, 0, 0, 0,  {0, 0}},  // ۱
    {208, 0x06F2, 0, 0, 0,  {0, 0}},  // ۲
    {209, 0x06F3, 0, 0, 0,  {0, 0}},  // ۳
    {210, 0x06F4, 0, 0, 0,  {0, 0}},  // ۴
    {211, 0x06F5, 0, 0, 0,  {0, 0}},  // ۵
    {212, 0x06F6, 0, 0, 0,  {0, 0}},  // ۶
    {213, 0x06F7, 0, 0, 0,  {0, 0}},  // ۷
    {214, 0x06F8, 0, 0, 0,  {0, 0}},  // ۸
    {215, 0x06F9, 0, 0, 0,  {0, 0}},  // ۹
    AP_END_CHARS_LIST
};

/*============================================================================*
 *                           Private Functions
 *============================================================================*/

/*unicode function*/

static uint8_t get_utf8_byte_num(uint8_t first_byte)
{
    if (first_byte < 0x80)
    {
        return 1;   // 1-byte (ASCII character)
    }
    else if ((first_byte >> 5) == 0x6)
    {
        return 2;   // 2-byte sequence
    }
    else if ((first_byte >> 4) == 0xE)
    {
        return 3;   // 3-byte sequence
    }
    else if ((first_byte >> 3) == 0x1E)
    {
        return 4;   // 4-byte sequence
    }
    else
    {
        return 0;   // Invalid first byte
    }
}
static uint16_t utf8_to_unicode_length(uint8_t *utf8, uint32_t utf8_len)
{
    uint16_t unicode_len = 0;
    uint16_t i = 0;

    while (i < utf8_len)
    {
        uint8_t char_len = get_utf8_byte_num(utf8[i]);

        if (char_len == 0 || i + char_len > utf8_len)
        {
            // Invalid UTF-8 character or out of bounds
            break;
        }

        i += char_len;
        unicode_len++;
    }

    return unicode_len;
}
static uint16_t utf8_to_unicode(uint8_t *utf8, uint16_t len, uint32_t *unicode_array,
                                uint16_t unicode_buf_len)
{
    uint16_t k = 0; // Number of Unicode characters converted

    for (uint16_t i = 0; i < len; i++)
    {
        if (k >= unicode_buf_len)
        {
            return k;
        }

        uint8_t utf8_bytes = get_utf8_byte_num(utf8[i]);
        switch (utf8_bytes)
        {
        case 1:
            unicode_array[k++] = utf8[i];
            break;

        case 2:
            if (i + 1 < len)
            {
                unicode_array[k++] = ((utf8[i] & 0x1F) << 6) | (utf8[i + 1] & 0x3F);
                i += 1;
            }
            break;

        case 3:
            if (i + 2 < len)
            {
                unicode_array[k++] = ((utf8[i] & 0x0F) << 12) |
                                     ((utf8[i + 1] & 0x3F) << 6) |
                                     (utf8[i + 2] & 0x3F);
                i += 2;
            }
            break;

        case 4:
            if (i + 3 < len)
            {
                uint32_t code_point = ((utf8[i] & 0x07) << 18) |
                                      ((utf8[i + 1] & 0x3F) << 12) |
                                      ((utf8[i + 2] & 0x3F) << 6) |
                                      (utf8[i + 3] & 0x3F);
                if (code_point >= 0x10000 && code_point <= 0x10FFFF)
                {
                    unicode_array[k++] = code_point; // Valid 4-byte code point
                }
                else
                {
                    unicode_array[k++] = 0xFFFD; // Replacement character for invalid code points
                }
                i += 3;
            }
            break;

        default:
            unicode_array[k++] = 0xFFFD; // Replacement character for invalid UTF-8 sequences
            break;
        }
    }

    return k;
}
#if 0
static void utf16_be_to_le(const uint8_t *input, uint8_t *output, size_t length)
{
    for (size_t i = 0; i < length; i += 2)
    {
        // Swap the positions of the two bytes
        output[i] = input[i + 1];
        output[i + 1] = input[i];
    }
}
static uint16_t utf16_to_unicode_length(uint16_t *utf16, uint32_t len)
{
    uint16_t i = 0;
    uint16_t unicode_length = 0;

    while (i < len / 2)
    {
        uint16_t current = utf16[i];
        if (current >= 0xD800 && current <= 0xDBFF)
        {
            // High surrogate
            if (i + 1 < len / 2 && utf16[i + 1] >= 0xDC00 && utf16[i + 1] <= 0xDFFF)
            {
                // Valid surrogate pair
                unicode_length++;
                i += 2;
            }
            else
            {
                // Invalid surrogate pair, treat as individual
                unicode_length++;
                i++;
            }
        }
        else
        {
            // Non-surrogate BMP character
            unicode_length++;
            i++;
        }
    }

    return unicode_length;
}
#endif
static uint16_t utf16le_to_unicode(uint8_t *utf16le, uint16_t len, uint32_t *unicode_array,
                                   uint16_t unicode_buf_len)
{
    if (len % 2 != 0)
    {
        gui_log("Error! UTF-16LE string length should be even!\n");
        return 0;
    }

    uint16_t i = 0;
    uint16_t unicode_index = 0;

    while (i < len && unicode_index < unicode_buf_len)
    {
        uint16_t unit1 = utf16le[i] | (utf16le[i + 1] << 8);
        if (unit1 >= 0xD800 && unit1 <= 0xDBFF)
        {
            // High surrogate
            if (i + 3 < len)
            {
                uint16_t unit2 = utf16le[i + 2] | (utf16le[i + 3] << 8);
                if (unit2 >= 0xDC00 && unit2 <= 0xDFFF)
                {
                    // Valid surrogate pair
                    uint32_t unicode_char = ((unit1 - 0xD800) << 10) + (unit2 - 0xDC00) + 0x10000;
                    unicode_array[unicode_index++] = unicode_char;
                    i += 4; // Move past both units
                    continue;
                }
            }
            // Invalid surrogate pair, treat as individual
            unicode_array[unicode_index++] = unit1;
            i += 2;
        }
        else if (unit1 >= 0xDC00 && unit1 <= 0xDFFF)
        {
            // Unmatched low surrogate, treat as invalid and increment count
            unicode_array[unicode_index++] = unit1;
            i += 2;
        }
        else
        {
            // Non-surrogate BMP character
            unicode_array[unicode_index++] = unit1;
            i += 2;
        }
    }

    return unicode_index;
}
static uint16_t utf16be_to_unicode(uint8_t *utf16be, uint16_t len, uint32_t *unicode_array,
                                   uint16_t unicode_buf_len)
{
    if (len % 2 != 0)
    {
        gui_log("Error! UTF-16BE string length should be even!\n");
        return 0;
    }

    uint16_t i = 0;
    uint16_t unicode_index = 0;

    while (i < len && unicode_index < unicode_buf_len)
    {
        uint16_t unit1 = (utf16be[i] << 8) | utf16be[i + 1];
        if (unit1 >= 0xD800 && unit1 <= 0xDBFF)
        {
            // High surrogate
            if (i + 3 < len)
            {
                uint16_t unit2 = (utf16be[i + 2] << 8) | utf16be[i + 3];
                if (unit2 >= 0xDC00 && unit2 <= 0xDFFF)
                {
                    // Valid surrogate pair
                    uint32_t unicode_char = ((unit1 - 0xD800) << 10) + (unit2 - 0xDC00) + 0x10000;
                    unicode_array[unicode_index++] = unicode_char;
                    i += 4; // Move past both units
                    continue;
                }
            }
            // Invalid surrogate pair, treat as individual
            unicode_array[unicode_index++] = unit1;
            i += 2;
        }
        else if (unit1 >= 0xDC00 && unit1 <= 0xDFFF)
        {
            // Unmatched low surrogate, treat as invalid and increment count
            unicode_array[unicode_index++] = unit1;
            i += 2;
        }
        else
        {
            // Non-surrogate BMP character
            unicode_array[unicode_index++] = unit1;
            i += 2;
        }
    }

    return unicode_index;
}

static uint16_t utf32le_to_unicode(uint8_t *utf32le, uint16_t len, uint32_t *unicode_array,
                                   uint16_t unicode_buf_len)
{
    if (len % 4 != 0)
    {
        gui_log("Error! UTF-32LE string length should be divisible by 4!\n");
        return 0;
    }

    uint16_t unicode_len = len / 4;
    if (unicode_len > unicode_buf_len)
    {
        unicode_len = unicode_buf_len;
    }

    memcpy(unicode_array, utf32le, unicode_len * sizeof(uint32_t));

    return unicode_len;
}
static uint16_t utf32be_to_unicode(uint8_t *utf32be, uint16_t len, uint32_t *unicode_array,
                                   uint16_t unicode_buf_len)
{
    if (len % 4 != 0)
    {
        gui_log("Error! UTF-32BE string length should be divisible by 4!\n");
        return 0;
    }

    uint16_t unicode_index = 0;
    for (uint16_t i = 0; i < len && unicode_index < unicode_buf_len; i += 4)
    {
        uint32_t unicode_char = (utf32be[i] << 24) |
                                (utf32be[i + 1] << 16) |
                                (utf32be[i + 2] << 8) |
                                utf32be[i + 3];
        unicode_array[unicode_index++] = unicode_char;
    }

    return unicode_index;
}

/*emoji function*/

// Helper function to check if a code point is a zero-width joiner (ZWJ)
static int is_zwj(uint32_t code)
{
    return code == 0x200D;
}

// Helper function to check if a code point is a skin tone modifier
static int is_skin_tone_modifier(uint32_t code)
{
    return (code >= 0x1F3FB && code <= 0x1F3FF);
}

// Helper function to check if a code point is a combining diacritical mark
static int is_combining_diacritical_mark(uint32_t code)
{
    return (code >= 0x0300 && code <= 0x036F) ||
           (code >= 0x1AB0 && code <= 0x1AFF) ||
           (code >= 0x1DC0 && code <= 0x1DFF) ||
           (code >= 0x20D0 && code <= 0x20FF) ||
           (code >= 0xFE20 && code <= 0xFE2F);
}

// Helper function to check if a code point is a regional indicator
static int is_regional_indicator(uint32_t code)
{
    return (code >= 0x1F1E6 && code <= 0x1F1FF);
}

// Helper function to check if a code point is a variation selector
static int is_variation_selector(uint32_t code)
{
    return (code >= 0xFE00 && code <= 0xFE0F) ||
           (code >= 0xE0100 && code <= 0xE01EF);
}

// Helper function to check if a code point is a tag character
static int is_tag_character(uint32_t code)
{
    return (code >= 0xE0000 && code <= 0xE007F);
}

// Check if a code point should be combined based on emoji composition rules
static int is_combination_rule(uint32_t code)
{
    return is_skin_tone_modifier(code) ||
           is_combining_diacritical_mark(code) ||
           is_regional_indicator(code) ||
           is_variation_selector(code) ||
           is_tag_character(code);
}

static int find_first_emoji(const uint32_t *unicode_buf, uint32_t len)
{
    if (len == 0)
    {
        return 0;
    }
    uint32_t i = 0;

    for (i = 1; i < len; i++)
    {
        if (is_zwj(unicode_buf[i]))
        {
            i++;
            continue;
        }
        if (is_combination_rule(unicode_buf[i]))
        {

        }
        else
        {
            break;
        }
    }

    return i; // Emoji found successfully.
}

/*Arabic/Persian function*/

static bool is_ap_unicode(uint32_t c)
{
    return (c >= AP_ALPHABET_BASE_CODE) && (c <= AP_ALPHABET_END_CODE);
}

static uint32_t ap_get_char_index(uint16_t c)
{
    for (uint8_t i = 0; ap_chars_map[i].char_end_form; i++)
    {
        if (c == (ap_chars_map[i].char_offset + AP_ALPHABET_BASE_CODE))
        {
            return i;
        }
        else if (c ==
                 ap_chars_map[i].char_end_form                                                     //is it an End form
                 || c == (ap_chars_map[i].char_end_form +
                          ap_chars_map[i].char_beginning_form_offset)    //is it a Beginning form
                 || c == (ap_chars_map[i].char_end_form +
                          ap_chars_map[i].char_middle_form_offset)       //is it a middle form
                 || c == (ap_chars_map[i].char_end_form +
                          ap_chars_map[i].char_isolated_form_offset))    //is it an isolated form
        {
            return i;
        }
    }
    return UNDEF_ARABIC_PERSIAN_CHARS;
}

static uint32_t text_lam_alef(uint32_t ch_curr, uint32_t ch_next)
{
    uint32_t ch_code = 0;
    if (ap_chars_map[ch_curr].char_offset != 34)
    {
        return 0;
    }
    if (ch_next == UNDEF_ARABIC_PERSIAN_CHARS)
    {
        return 0;
    }
    ch_code = ap_chars_map[ch_next].char_offset + AP_ALPHABET_BASE_CODE;
    if (ch_code == 0x0622)
    {
        return 0xFEF5;    // (lam-alef) mad
    }
    if (ch_code == 0x0623)
    {
        return 0xFEF7;    // (lam-alef) top hamza
    }
    if (ch_code == 0x0625)
    {
        return 0xFEF9;    // (lam-alef) bot hamza
    }
    if (ch_code == 0x0627)
    {
        return 0xFEFB;    // (lam-alef) alef
    }
    return 0;
}

static bool is_arabic_vowel(uint16_t c)
{
    return (c >= 0x064B) && (c <= 0x0652);
}

/*Thai function*/
static bool is_thai_unicode(uint32_t c)
{
    return (c >= THAI_ALPHABET_BASE_CODE) && (c <= THAI_ALPHABET_END_CODE);
}

// static bool is_thai_base(uint32_t ch)
// {
//     return (ch >= 0x0E01 && ch <= 0x0E2E) || (ch == 0E32) || (ch == 0x0E33);
// }

static bool is_thai_above_char(uint32_t ch)
{
    return (ch == 0x0E31) ||
           (ch >= 0x0E34 && ch <= 0x0E37) ||
           (ch >= 0x0E47 && ch <= 0x0E4E);
}
static bool is_thai_below_char(uint32_t ch)
{
    return (ch >= 0x0E38 && ch <= 0x0E3A);
}
// static bool is_thai_left_char(uint32_t ch)
// {
//     return (ch >= 0x0E40 && ch <= 0x0E44);
// }

THAI_MARK_POS get_thai_mark_pos(uint32_t ch)
{
    if (is_thai_above_char(ch))
    {
        return THAI_ABOVE;
    }
    if (is_thai_below_char(ch))
    {
        return THAI_BELOW;
    }
    return THAI_NONE;
}

static void update_thai_char_position(mem_char_t *base_chr, mem_char_t *mark_chr,
                                      uint32_t mark_count, THAI_MARK_INFO *marks)
{
    for (uint32_t i = 0; i < mark_count; i++)
    {
        uint32_t base_idx = marks[i].base_index - 1;
        mark_chr[i].x = base_chr[base_idx].x + base_chr[base_idx].char_w - mark_chr[i].char_w;
        mark_chr[i].y = base_chr[base_idx].y;
        if (marks[i].mark_pos == THAI_TOPMOST)
        {
            mark_chr[i].y -= mark_chr[i].char_h;
        }
    }
}

/*Hebrew*/
static bool is_hebrew_unicode(uint32_t c)
{
    return (c >= HEBREW_ALPHABET_BASE_CODE) && (c <= HEBREW_ALPHABET_END_CODE);
}


/*============================================================================*
 *                           Public Functions
 *============================================================================*/

uint16_t process_content_by_charset(TEXT_CHARSET charset_type, uint8_t *content, uint16_t len,
                                    uint32_t **unicode_buf_ptr)
{
    uint16_t unicode_len = 0;
    uint32_t *unicode_buf = NULL;

    if (len == 0)
    {
        gui_log("Warning! process_content_by_charset: len is 0\n");
        return 0;
    }

    switch (charset_type)
    {
    case UTF_8:
        unicode_len = utf8_to_unicode_length(content, len);
        if (unicode_len == 0)
        {
            gui_log("Warning! utf8_to_unicode_length failed\n");
            return 0;
        }
        unicode_buf = (uint32_t *)gui_malloc((unicode_len + 1) * sizeof(uint32_t));
        if (unicode_buf == NULL)
        {
            GUI_ASSERT(NULL != NULL);
            return 0;
        }
        else
        {
            unicode_len = utf8_to_unicode(content, len, unicode_buf, unicode_len);
        }
        break;

    case UTF_16LE:
        unicode_len = len / 2;
        unicode_buf = (uint32_t *)gui_malloc((unicode_len + 1) * sizeof(uint32_t));
        if (unicode_buf == NULL)
        {
            GUI_ASSERT(NULL != NULL);
            return 0;
        }
        else
        {
            unicode_len = utf16le_to_unicode(content, len, unicode_buf, unicode_len);
        }
        break;

    case UTF_16BE:
        unicode_len = len / 2;
        unicode_buf = (uint32_t *)gui_malloc((unicode_len + 1) * sizeof(uint32_t));
        if (unicode_buf == NULL)
        {
            GUI_ASSERT(NULL != NULL);
            return 0;
        }
        else
        {
            unicode_len = utf16be_to_unicode(content, len, unicode_buf, unicode_len);
        }
        break;

    case UTF_32LE:
        unicode_len = len / 4;
        unicode_buf = (uint32_t *)gui_malloc((unicode_len + 1) * sizeof(uint32_t));
        if (unicode_buf == NULL)
        {
            GUI_ASSERT(NULL != NULL);
            return 0;
        }
        else
        {
            unicode_len = utf32le_to_unicode(content, len, unicode_buf, unicode_len);
        }
        break;

    case UTF_32BE:
        unicode_len = len / 4;
        unicode_buf = (uint32_t *)gui_malloc((unicode_len + 1) * sizeof(uint32_t));
        if (unicode_buf == NULL)
        {
            GUI_ASSERT(NULL != NULL);
            return 0;
        }
        else
        {
            unicode_len = utf32be_to_unicode(content, len, unicode_buf, unicode_len);
        }
        break;

    default:
        GUI_ASSERT(0);
        return 0;
    }

    unicode_buf[unicode_len] = 0;
    *unicode_buf_ptr = unicode_buf;
    return unicode_len;
}

uint32_t generate_emoji_file_path_from_unicode(const uint32_t *unicode_buf, uint32_t len,
                                               char *file_path)
{
    uint32_t emoji_len = find_first_emoji(unicode_buf, len);
    // gui_log("first unicode %x ,emoji_len is %d \n",unicode_buf[0],emoji_len);

    if (emoji_len == 0)
    {
        return 0; // No emoji found.
    }

    for (uint32_t i = 0; i < emoji_len; ++i)
    {
        char buffer[12];
        if (unicode_buf[i] == 0xfe0f)
        {
            continue;
        }
        snprintf(buffer, sizeof(buffer), "%04x", (unsigned int)unicode_buf[i]);
        strcat(file_path, buffer);
        if (i < emoji_len - 1)
        {
            strcat(file_path, "_");
        }
    }

    uint32_t path_len = strlen(file_path);
    if (file_path[path_len - 1] == 95)
    {
        file_path[path_len - 1] = '\0';
    }

    strcat(file_path, ".bin");

    return emoji_len; // Success.
}

bool content_has_ap_unicode(uint32_t *unicode_buf, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++)
    {
        if (is_ap_unicode(unicode_buf[i]))
        {
            return true;
        }
    }
    return false;
}

bool content_has_ap(TEXT_CHARSET charset_type, uint8_t *content, uint16_t len)
{
    uint32_t *unicode_buf = NULL;
    uint16_t unicode_len = 0;
    bool has_ap = false;
    unicode_len = process_content_by_charset(charset_type, content, len, &unicode_buf);
    if (content_has_ap_unicode(unicode_buf, unicode_len))
    {
        has_ap = true;
    }
    gui_free(unicode_buf);
    return has_ap;
}

uint32_t process_ap_unicode(uint32_t *unicode_buf, uint32_t unicode_len)
{
    if (!unicode_buf || unicode_len == 0)
    {
        return 0;
    }

    uint32_t i = 0, j = 0;
    uint32_t idx_prev = UNDEF_ARABIC_PERSIAN_CHARS;
    uint32_t idx_cur, idx_next;

    uint32_t *temp_buf = unicode_buf;

    while (i < unicode_len)
    {
        idx_cur = ap_get_char_index(temp_buf[i]);

        // 1. Copy vowel marks directly
        if (is_arabic_vowel(temp_buf[i]))
        {
            unicode_buf[j++] = temp_buf[i++];
            idx_prev = UNDEF_ARABIC_PERSIAN_CHARS;
            continue;
        }

        // 2. Lookahead: skip vowel marks after the current one
        if (is_arabic_vowel(temp_buf[i + 1]))
        {
            idx_next = ap_get_char_index(temp_buf[i + 2]);
        }
        else
        {
            idx_next = ap_get_char_index(temp_buf[i + 1]);
        }

        // 3. Non-Arabic/Persian character, copy directly
        if (idx_cur == UNDEF_ARABIC_PERSIAN_CHARS)
        {
            unicode_buf[j++] = temp_buf[i++];
            idx_prev = UNDEF_ARABIC_PERSIAN_CHARS;
            continue;
        }

        uint8_t conj_prev = (j == 0 || idx_prev == UNDEF_ARABIC_PERSIAN_CHARS) ?
                            0 : ap_chars_map[idx_prev].ap_chars_conjunction.conj_to_next;
        uint8_t conj_next = ((i == unicode_len - 1) || idx_next == UNDEF_ARABIC_PERSIAN_CHARS) ?
                            0 : ap_chars_map[idx_next].ap_chars_conjunction.conj_to_previous;

        // 4. Lam-Alef ligature handling
        uint32_t lam_alef = text_lam_alef(idx_cur, idx_next);
        if (lam_alef)
        {
            if (conj_prev)
            {
                lam_alef++;
            }
            unicode_buf[j++] = lam_alef;
            idx_prev = UNDEF_ARABIC_PERSIAN_CHARS;
            i += 2;
            continue;
        }

        // 5. Arabic/Persian character shaping
        if (conj_prev && conj_next)
        {
            unicode_buf[j++] = ap_chars_map[idx_cur].char_end_form +
                               ap_chars_map[idx_cur].char_middle_form_offset;
        }
        else if (!conj_prev && conj_next)
        {
            unicode_buf[j++] = ap_chars_map[idx_cur].char_end_form +
                               ap_chars_map[idx_cur].char_beginning_form_offset;
        }
        else if (conj_prev && !conj_next)
        {
            unicode_buf[j++] = ap_chars_map[idx_cur].char_end_form;
        }
        else
        {
            unicode_buf[j++] = ap_chars_map[idx_cur].char_end_form +
                               ap_chars_map[idx_cur].char_isolated_form_offset;
        }

        idx_prev = idx_cur;
        i++;
    }

    if (j < unicode_len)
    {
        unicode_buf[j] = 0;
    }

    return j;
}

bool content_has_thai_unicode(uint32_t *unicode_buf, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++)
    {
        if (is_thai_unicode(unicode_buf[i]))
        {
            return true;
        }
    }
    return false;
}

bool content_has_thai(TEXT_CHARSET charset_type, uint8_t *content, uint16_t len)
{
    uint32_t *unicode_buf = NULL;
    uint16_t unicode_len = 0;
    bool has_thai = false;
    unicode_len = process_content_by_charset(charset_type, content, len, &unicode_buf);
    if (content_has_thai_unicode(unicode_buf, unicode_len))
    {
        has_thai = true;
    }
    gui_free(unicode_buf);
    return has_thai;
}

uint32_t get_thai_mark_char_width(mem_char_t *chr, uint32_t char_count)
{
    uint32_t updated_width = 0;
    for (uint16_t i = 0; i < char_count; i++)
    {
        if (get_thai_mark_pos(chr[i].unicode) != THAI_NONE)
        {
            updated_width += chr[i].char_w;
        }
    }
    return updated_width;
}

uint32_t process_thai_char_struct(mem_char_t *chr, uint32_t unicode_len,
                                  THAI_MARK_INFO **mark_array_out, uint32_t *mark_count_out)
{
    if (unicode_len == 0 || chr == NULL)
    {
        if (mark_array_out)
        {
            *mark_array_out = NULL;
        }
        if (mark_count_out)
        {
            *mark_count_out = 0;
        }
        return 0;
    }

    uint32_t base_count = 0;
    uint32_t mark_count = 0;
    int32_t last_base_idx = -1;
    uint32_t above_count_for_curr_base = 0;

    mem_char_t *processed_chr = gui_malloc(sizeof(mem_char_t) * unicode_len);
    memset(processed_chr, 0, sizeof(mem_char_t) * unicode_len);

    for (uint32_t i = 0; i < unicode_len; ++i)
    {
        THAI_MARK_POS pos = get_thai_mark_pos(chr[i].unicode);
        if (pos == THAI_NONE)
        {
            last_base_idx = base_count;
            base_count++;
        }
        else if (pos != THAI_NONE && last_base_idx >= 0)
        {
            mark_count++;
        }
    }

    if (base_count + mark_count != unicode_len)
    {
        gui_log("Warning! process_thai_char_struct: base_count + mark_count != unicode_len");
    }

    THAI_MARK_INFO *marks = NULL;
    if (mark_count > 0)
    {
        marks = (THAI_MARK_INFO *)gui_malloc(sizeof(THAI_MARK_INFO) * mark_count);
        memset(marks, 0, sizeof(THAI_MARK_INFO) * mark_count);
    }
    else
    {
        *mark_array_out = NULL;
        mark_count_out = 0;
        gui_free(processed_chr);
        return base_count;
    }

    uint32_t base_idx = 0;
    uint32_t mark_idx = 0;
    last_base_idx = -1;
    above_count_for_curr_base = 0;

    for (uint32_t i = 0; i < unicode_len; ++i)
    {
        uint32_t ch = chr[i].unicode;
        THAI_MARK_POS pos = get_thai_mark_pos(ch);
        if (pos == THAI_NONE)
        {
            last_base_idx = base_idx;
            above_count_for_curr_base = 0;
            memcpy(&processed_chr[base_idx], &chr[i], sizeof(mem_char_t));
            base_idx++;
        }
        else if (last_base_idx >= 0 && marks && mark_idx < mark_count)
        {
            if (pos == THAI_ABOVE)
            {
                above_count_for_curr_base++;
                if (above_count_for_curr_base > 1)
                {
                    pos = THAI_TOPMOST;
                }
            }
            memcpy(&processed_chr[base_count + mark_idx], &chr[i], sizeof(mem_char_t));
            marks[mark_idx].base_index   = base_idx;
            marks[mark_idx].mark_unicode = ch;
            marks[mark_idx].mark_pos     = pos;
            mark_idx++;
        }
    }
    *mark_array_out = marks;
    *mark_count_out = mark_idx;

    memcpy(chr, processed_chr, sizeof(mem_char_t) * unicode_len);
    gui_free(processed_chr);

    return base_count;
}

uint32_t post_process_thai_char_struct(mem_char_t *chr, uint32_t base_count, uint32_t active_base,
                                       uint32_t mark_count, THAI_MARK_INFO *marks)
{
    mem_char_t *base_chr = chr;
    mem_char_t *mark_chr = chr + base_count;

    update_thai_char_position(base_chr, mark_chr, mark_count, marks);

    mem_char_t *original_chr = gui_malloc(sizeof(mem_char_t) * (base_count + mark_count));
    // memset(original_chr, 0, sizeof(mem_char_t) * (base_count + mark_count));

    uint32_t active_len = 0;
    uint32_t original_idx = 0;
    for (uint32_t i = 0; i < mark_count; i++)
    {
        uint32_t part_base_count = marks[i].base_index - (original_idx - i);
        memcpy(&original_chr[original_idx], &base_chr[original_idx - i],
               sizeof(mem_char_t) * part_base_count);
        original_idx += part_base_count;
        memcpy(&original_chr[original_idx++], &base_chr[base_count + i], sizeof(mem_char_t));
        if (marks[i].base_index <= active_base)
        {
            active_len = active_base + i;
        }
    }
    int32_t tail_count = base_count + mark_count - original_idx;
    if (tail_count > 0)
    {
        memcpy(&original_chr[original_idx], &base_chr[base_count - tail_count],
               sizeof(mem_char_t) * tail_count);
    }
    memcpy(chr, original_chr, sizeof(mem_char_t) * (base_count + mark_count));
    gui_free(original_chr);
    return active_len;
}

/*Hebrew*/
bool content_has_hebrew_unicode(uint32_t *unicode_buf, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++)
    {
        if (is_hebrew_unicode(unicode_buf[i]))
        {
            return true;
        }
    }
    return false;
}

bool content_has_hebrew(TEXT_CHARSET charset_type, uint8_t *content, uint16_t len)
{
    uint32_t *unicode_buf = NULL;
    uint16_t unicode_len = 0;
    bool has_hebrew = false;
    unicode_len = process_content_by_charset(charset_type, content, len, &unicode_buf);
    if (content_has_hebrew_unicode(unicode_buf, unicode_len))
    {
        has_hebrew = true;
    }
    gui_free(unicode_buf);
    return has_hebrew;
}


