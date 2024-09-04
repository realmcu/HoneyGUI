
#include <guidef.h>
#include <draw_font.h>
#include <string.h>

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

uint16_t process_content_by_charset(TEXT_CHARSET charset_type, uint8_t *content, uint16_t len,
                                    uint32_t **unicode_buf_ptr)
{
    uint16_t unicode_len = 0;
    uint32_t *unicode_buf = NULL;

    switch (charset_type)
    {
    case UTF_8:
        unicode_len = utf8_to_unicode_length(content, len);
        unicode_buf = (uint32_t *)gui_malloc(unicode_len * sizeof(uint32_t));
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
        unicode_buf = (uint32_t *)gui_malloc((unicode_len) * sizeof(uint32_t));
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
        unicode_buf = (uint32_t *)gui_malloc((unicode_len) * sizeof(uint32_t));
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
        unicode_buf = (uint32_t *)gui_malloc((unicode_len) * sizeof(uint32_t));
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
        unicode_buf = (uint32_t *)gui_malloc((unicode_len) * sizeof(uint32_t));
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

    *unicode_buf_ptr = unicode_buf;
    return unicode_len;
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
        snprintf(buffer, sizeof(buffer), "%04x", unicode_buf[i]);
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


