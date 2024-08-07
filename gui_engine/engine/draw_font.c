
#include <guidef.h>
#include <draw_font.h>
#include <string.h>

static int get_utf8_byte_num(uint8_t firstCh)
{
    uint8_t temp = 0x80;
    int num = 0;

    while (temp & firstCh)
    {
        num++;
        temp = (temp >> 1);
    }
    return num;
}

uint32_t get_len_by_char_num(uint8_t *utf8, uint32_t char_num)
{
    uint32_t num = 0;
    uint32_t len = 0;
    uint32_t temp = 0;
    while (num++ < char_num)
    {
        temp = get_utf8_byte_num(utf8[len]);
        if (temp == 0)
        {
            len ++;
        }
        else
        {
            len += temp;
        }
    }
    return len;
}

uint16_t utf8_to_unicode(uint8_t *utf8, uint16_t len, uint16_t *unicode_array,
                         uint16_t unicode_buf_len)
{
    uint16_t k = 0;

    for (uint32_t i = 0; i < len; i++)
    {
        if (k >= unicode_buf_len || k == 0xffff)
        {
            return k;
        }
        switch (get_utf8_byte_num((uint8_t)utf8[i]))
        {
        case 0:
            {
                unicode_array[k] = *(utf8 + i);
                k++;
                break;
            }
        case 1:
            {
                //TODO
                //can not enter here
                //k++;
                break;
            }
        case 2:
            {
                unicode_array[k] = (uint16_t)(((utf8[i] & 0x03) << 6 | utf8[i + 1] & 0x3F) | ((
                        utf8[i] & 0x38) >> 2 << 8));
                k++;
                i = i + 1;
                break;
            }
        case 3:
            {
                unicode_array[k] = ((utf8[i + 1] & 0x03) << 6) + (utf8[i + 2] & 0x3F);
                unicode_array[k] |= (uint16_t)(((utf8[i] & 0x0F) << 4) | ((utf8[i + 1] >> 2) & 0x0F)) << 8;
                k++;
                i = i + 2;
                break;
            }
        case 4:
            {
                //Because it is outside the standard unicode range, replace the character with a whitespace character.
                unicode_array[k] = 0x0;
                k++;
                i = i + 3;
                break;
            }
        case 5:
            {
                //TODO
                k++;
                i = i + 4;
                break;
            }
        case 6:
            {
                //TODO
                k++;
                i = i + 5;
                break;
            }
        default:
            //gui_log("the len is more than 6\n");
            break;
        }
    }
    return k;
}

// Function to convert UTF-16 Big-Endian (BE) to Little-Endian (LE)
void utf16_be_to_le(const uint8_t *input, uint8_t *output, size_t length)
{
    for (size_t i = 0; i < length; i += 2)
    {
        // Swap the positions of the two bytes
        output[i] = input[i + 1];
        output[i + 1] = input[i];
    }
}
