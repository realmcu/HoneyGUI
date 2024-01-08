
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
                //TODO
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

