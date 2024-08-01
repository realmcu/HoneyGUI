/*********************************************************************************************************
*               Copyright(c) 2018, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file
* @brief
* @details
* @author
* @date
* @version
*********************************************************************************************************
*/

#include "string.h"
#include <string.h>
#include "gui_bmp.h"


uint8_t rtl_gui_show_bmp_sector(UI_WidgetTypeDef *widget, int16_t detal_x, int16_t detal_y, int Zs,
                                int Ze, uint8_t *buf)
{
    int xs = widget->x + detal_x;
    int ys = widget->y + detal_y;
    int width = widget->width;
    int height = widget->hight;
    unsigned char *BMP = (unsigned char *)widget->addr;
    int y_start = widget->active_ys;
    int y_end = widget->active_ye;
    if (BMP == 0)
    {
        return 0;
    }
    if ((y_start == 0) && (y_end == 0))
    {
        y_start = 0;
        y_end = LCD_HIGHT;
    }

    if (y_start > (Zs + LCD_SECTION_HEIGHT))
    {
        return 0;
    }
    if (y_end < Ze)
    {
        return 0;
    }

    uint16_t loopk;
    uint16_t loopj;
    uint16_t loopy;
    uint16_t addr_hs;
    uint16_t addr_hh;
    uint16_t read_off;
    uint16_t write_off;
    uint8_t *bmp_data;

#if PIXEL_FORMAT == RGB16BIT_565
    uint16_t *writebuf = (uint16_t *)buf;
    uint16_t tempcolour;
#elif PIXEL_FORMAT == RGB24BIT_888
    uint8_t *writebuf = buf;
#endif

    if ((ys >= Ze) || (xs >= LCD_WIDTH) || ((ys + height) <= Zs) || ((xs + width) <= 0) ||
        (width > LCD_WIDTH))
    {
        return 0;
    }

    if (ys > Zs)
    {
        addr_hs = 0;
        addr_hh = Ze - ys;
        addr_hh = (addr_hh > height) ? height : addr_hh;
        ys = ys - Zs;
    }
    else
    {
        if ((ys + height) < Ze)
        {
            addr_hs = Zs - ys;
            addr_hh = (ys + height) - Zs;
        }
        else
        {
            addr_hs = Zs - ys;
            addr_hh = Ze - Zs;
        }
        ys = 0;
    }
    loopy = ys;
#if 0
    if ((uint32_t)BMP  + width * PIXEL_BYTES * addr_hs >= 0x1000000)
    {
        extern uint16_t *disp_read_buff;
        extern bool flash_read_quick_locked(uint32_t start_addr, uint32_t data_len, uint8_t *data);
        flash_ioctl(flash_ioctl_set_seq_trans_enable, 0, 0);
        flash_read_quick_locked((uint32_t)BMP  + width * PIXEL_BYTES * addr_hs, \
                                LCD_WIDTH * LCD_SECTION_HEIGHT * PIXEL_BYTES, (uint8_t *)disp_read_buff);
        flash_ioctl(flash_ioctl_set_seq_trans_enable, 1, 0);
        bmp_data = disp_read_buff;
    }
#else
    bmp_data = (uint8_t *)(BMP + width * PIXEL_BYTES * addr_hs);
#endif

    if ((xs < 0) && ((xs + width) > 0))
    {
        int width_cache;
        width_cache = width + xs;
        for (loopk = 0; loopk < addr_hh; loopk ++, loopy ++)
        {
            read_off = loopk * width;
            write_off = loopy * LCD_WIDTH;
            for (loopj = 0; loopj < width_cache; loopj ++)
            {
#if PIXEL_FORMAT == RGB16BIT_565
                tempcolour = *((uint16_t *)bmp_data + read_off + (loopj - xs));
                if (tempcolour  != 0)
                {
                    writebuf[write_off + loopj] = tempcolour;
                }
#elif PIXEL_FORMAT == RGB24BIT_888
                uint8_t *pixel = bmp_data + (read_off + loopj - xs) * PIXEL_BYTES;
                if ((*pixel) || (*(pixel + 1)) || (*(pixel + 2)))
                {
                    writebuf[(write_off + loopj) * PIXEL_BYTES] = *pixel;
                    writebuf[(write_off + loopj) * PIXEL_BYTES + 1] = *(pixel + 1);
                    writebuf[(write_off + loopj) * PIXEL_BYTES + 2] = *(pixel + 2);
                }
#endif
            }
        }
    }
    else if (((xs + width) > LCD_WIDTH) && (xs <= LCD_WIDTH))
    {
        int width_cache;
        width_cache = LCD_WIDTH - xs;
        for (loopk = 0; loopk < addr_hh; loopk ++, loopy ++)
        {
            read_off = loopk * width;
            write_off = loopy * LCD_WIDTH;
            for (loopj = 0; loopj < width_cache; loopj ++)
            {
#if PIXEL_FORMAT == RGB16BIT_565
                tempcolour = *((uint16_t *)bmp_data + read_off + loopj);
                if (tempcolour  != 0)
                {
                    writebuf[write_off + xs + loopj] = tempcolour;
                }
#elif PIXEL_FORMAT == RGB24BIT_888
                uint8_t *pixel = bmp_data + (read_off + loopj) * PIXEL_BYTES;
                if ((*pixel) || (*(pixel + 1)) || (*(pixel + 2)))
                {
                    writebuf[(write_off + xs + loopj) * PIXEL_BYTES] = *pixel;
                    writebuf[(write_off + xs + loopj) * PIXEL_BYTES + 1] = *(pixel + 1);
                    writebuf[(write_off + xs + loopj) * PIXEL_BYTES + 2] = *(pixel + 2);
                }
#endif

            }
        }
    }
    else if ((xs >= 0) && ((xs + width) <= LCD_WIDTH))
    {
        for (loopk = 0; loopk < addr_hh; loopk ++, loopy ++)
        {
            read_off = loopk * width;
            write_off = loopy * LCD_WIDTH;
            for (loopj = 0; loopj < width; loopj ++)
            {
#if PIXEL_FORMAT == RGB16BIT_565
                tempcolour = *((uint16_t *)bmp_data + read_off + loopj);
                if (tempcolour  != 0)
                {
                    writebuf[write_off + xs + loopj] = tempcolour;
                }
#elif PIXEL_FORMAT == RGB24BIT_888
                uint8_t *pixel = bmp_data + (read_off + loopj) * PIXEL_BYTES;
                if ((*pixel) || (*(pixel + 1)) || (*(pixel + 2)))
                {
                    writebuf[(write_off + xs + loopj) * PIXEL_BYTES] = *pixel;
                    writebuf[(write_off + xs + loopj) * PIXEL_BYTES + 1] = *(pixel + 1);
                    writebuf[(write_off + xs + loopj) * PIXEL_BYTES + 2] = *(pixel + 2);
                }
#endif
            }
        }
    }

    return xs + width;
}


uint8_t rtl_gui_show_bmp_simple(UI_BMPTypeDef *widget, int16_t detal_x, int16_t detal_y, int Zs,
                                int Ze, uint8_t *buf)
{
    int xs = widget->x + detal_x;
    int ys = widget->y + detal_y;
    int width = widget->width;
    int height = widget->hight;
    unsigned char *BMP = (unsigned char *)widget->addr;
    int y_start = widget->active_ys;
    int y_end = widget->active_ye;
    if (BMP == 0)
    {
        return 0;
    }
    if ((y_start == 0) && (y_end == 0))
    {
        y_start = 0;
        y_end = LCD_HIGHT;
    }

    if (y_start > (Zs + LCD_SECTION_HEIGHT))
    {
        return 0;
    }
    if (y_end < Ze)
    {
        return 0;
    }

    uint16_t loopk;
    uint16_t loopj;
    uint16_t loopy;
    uint16_t addr_hs;
    uint16_t addr_hh;
    uint16_t read_off;
    uint16_t write_off;
    uint8_t *bmp_data;

#if PIXEL_FORMAT == RGB16BIT_565
    uint16_t *writebuf = (uint16_t *)buf;
    uint16_t tempcolour;
#elif PIXEL_FORMAT == RGB24BIT_888
    uint8_t *writebuf = buf;
#endif

    if ((ys >= Ze) || (xs >= LCD_WIDTH) || ((ys + height) <= Zs) || ((xs + width) <= 0) ||
        (width > LCD_WIDTH))
    {
        return 0;
    }

    if (ys > Zs)
    {
        addr_hs = 0;
        addr_hh = Ze - ys;
        addr_hh = (addr_hh > height) ? height : addr_hh;
        ys = ys - Zs;
    }
    else
    {
        if ((ys + height) < Ze)
        {
            addr_hs = Zs - ys;
            addr_hh = (ys + height) - Zs;
        }
        else
        {
            addr_hs = Zs - ys;
            addr_hh = Ze - Zs;
        }
        ys = 0;
    }
    loopy = ys;
#if 0
    if ((uint32_t)BMP  + width * PIXEL_BYTES * addr_hs >= 0x1000000)
    {
        extern uint16_t *disp_read_buff;
        extern bool flash_read_quick_locked(uint32_t start_addr, uint32_t data_len, uint8_t *data);
        flash_ioctl(flash_ioctl_set_seq_trans_enable, 0, 0);
        flash_read_quick_locked((uint32_t)BMP  + width * PIXEL_BYTES * addr_hs, \
                                LCD_WIDTH * LCD_SECTION_HEIGHT * PIXEL_BYTES, (uint8_t *)disp_read_buff);
        flash_ioctl(flash_ioctl_set_seq_trans_enable, 1, 0);
        bmp_data = disp_read_buff;
    }
#else
    bmp_data = (uint8_t *)(BMP + width * PIXEL_BYTES * addr_hs);
#endif

    if ((xs < 0) && ((xs + width) > 0))
    {
        int width_cache;
        width_cache = width + xs;
        for (loopk = 0; loopk < addr_hh; loopk ++, loopy ++)
        {
            read_off = loopk * width;
            write_off = loopy * LCD_WIDTH;
            for (loopj = 0; loopj < width_cache; loopj ++)
            {
#if PIXEL_FORMAT == RGB16BIT_565
                tempcolour = *((uint16_t *)bmp_data + read_off + (loopj - xs));
                if (tempcolour  != 0)
                {
                    writebuf[write_off + loopj] = tempcolour;
                }
#elif PIXEL_FORMAT == RGB24BIT_888
                uint8_t *pixel = bmp_data + (read_off + loopj - xs) * PIXEL_BYTES;
                if ((*pixel) || (*(pixel + 1)) || (*(pixel + 2)))
                {
                    writebuf[(write_off + loopj) * PIXEL_BYTES] = *pixel;
                    writebuf[(write_off + loopj) * PIXEL_BYTES + 1] = *(pixel + 1);
                    writebuf[(write_off + loopj) * PIXEL_BYTES + 2] = *(pixel + 2);
                }
#endif
            }
        }
    }
    else if (((xs + width) > LCD_WIDTH) && (xs <= LCD_WIDTH))
    {
        int width_cache;
        width_cache = LCD_WIDTH - xs;
        for (loopk = 0; loopk < addr_hh; loopk ++, loopy ++)
        {
            read_off = loopk * width;
            write_off = loopy * LCD_WIDTH;
            for (loopj = 0; loopj < width_cache; loopj ++)
            {
#if PIXEL_FORMAT == RGB16BIT_565
                tempcolour = *((uint16_t *)bmp_data + read_off + loopj);
                if (tempcolour  != 0)
                {
                    writebuf[write_off + xs + loopj] = tempcolour;
                }
#elif PIXEL_FORMAT == RGB24BIT_888
                uint8_t *pixel = bmp_data + (read_off + loopj) * PIXEL_BYTES;
                if ((*pixel) || (*(pixel + 1)) || (*(pixel + 2)))
                {
                    writebuf[(write_off + xs + loopj) * PIXEL_BYTES] = *pixel;
                    writebuf[(write_off + xs + loopj) * PIXEL_BYTES + 1] = *(pixel + 1);
                    writebuf[(write_off + xs + loopj) * PIXEL_BYTES + 2] = *(pixel + 2);
                }
#endif

            }
        }
    }
    else if ((xs >= 0) && ((xs + width) <= LCD_WIDTH))
    {
        for (loopk = 0; loopk < addr_hh; loopk ++, loopy ++)
        {
            read_off = loopk * width;
            write_off = loopy * LCD_WIDTH;
            for (loopj = 0; loopj < width; loopj ++)
            {
#if PIXEL_FORMAT == RGB16BIT_565
                tempcolour = *((uint16_t *)bmp_data + read_off + loopj);
                if (tempcolour  != 0)
                {
                    writebuf[write_off + xs + loopj] = tempcolour;
                }
#elif PIXEL_FORMAT == RGB24BIT_888
                uint8_t *pixel = bmp_data + (read_off + loopj) * PIXEL_BYTES;
                if ((*pixel) || (*(pixel + 1)) || (*(pixel + 2)))
                {
                    writebuf[(write_off + xs + loopj) * PIXEL_BYTES] = *pixel;
                    writebuf[(write_off + xs + loopj) * PIXEL_BYTES + 1] = *(pixel + 1);
                    writebuf[(write_off + xs + loopj) * PIXEL_BYTES + 2] = *(pixel + 2);
                }
#endif
            }
        }
    }

    return xs + width;
}

uint8_t rtl_gui_show_bmp_sector_legacy(int xs, int ys, int width, int height,
                                       const unsigned char BMP[],
                                       int Zs,
                                       int Ze, uint8_t *buf, int y_start, int y_end)
{
    if (BMP == 0)
    {
        return 0;
    }
    if ((y_start == 0) && (y_end == 0))
    {
        y_start = 0;
        y_end = LCD_HIGHT;
    }

    if (y_start > (Zs + LCD_SECTION_HEIGHT))
    {
        return 0;
    }
    if (y_end < Ze)
    {
        return 0;
    }

    uint16_t loopk;
    uint16_t loopj;
    uint16_t loopy;
    uint16_t addr_hs;
    uint16_t addr_hh;
    uint16_t read_off;
    uint16_t write_off;
    uint8_t *bmp_data;

#if PIXEL_FORMAT == RGB16BIT_565
    uint16_t *writebuf = (uint16_t *)buf;
    uint16_t tempcolour;
#elif PIXEL_FORMAT == RGB24BIT_888
    uint8_t *writebuf = buf;
#endif

    if ((ys >= Ze) || (xs >= LCD_WIDTH) || ((ys + height) <= Zs) || ((xs + width) <= 0) ||
        (width > LCD_WIDTH))
    {
        return 0;
    }

    if (ys > Zs)
    {
        addr_hs = 0;
        addr_hh = Ze - ys;
        addr_hh = (addr_hh > height) ? height : addr_hh;
        ys = ys - Zs;
    }
    else
    {
        if ((ys + height) < Ze)
        {
            addr_hs = Zs - ys;
            addr_hh = (ys + height) - Zs;
        }
        else
        {
            addr_hs = Zs - ys;
            addr_hh = Ze - Zs;
        }
        ys = 0;
    }
    loopy = ys;
#if 0
    if ((uint32_t)BMP  + width * PIXEL_BYTES * addr_hs >= 0x1000000)
    {
        extern uint16_t *disp_read_buff;
        extern bool flash_read_quick_locked(uint32_t start_addr, uint32_t data_len, uint8_t *data);
        flash_ioctl(flash_ioctl_set_seq_trans_enable, 0, 0);
        flash_read_quick_locked((uint32_t)BMP  + width * PIXEL_BYTES * addr_hs, \
                                LCD_WIDTH * LCD_SECTION_HEIGHT * PIXEL_BYTES, (uint8_t *)disp_read_buff);
        flash_ioctl(flash_ioctl_set_seq_trans_enable, 1, 0);
        bmp_data = disp_read_buff;
    }
#else
    bmp_data = (uint8_t *)(BMP + width * PIXEL_BYTES * addr_hs);
#endif

    if ((xs < 0) && ((xs + width) > 0))
    {
        int width_cache;
        width_cache = width + xs;
        for (loopk = 0; loopk < addr_hh; loopk ++, loopy ++)
        {
            read_off = loopk * width;
            write_off = loopy * LCD_WIDTH;
            for (loopj = 0; loopj < width_cache; loopj ++)
            {
#if PIXEL_FORMAT == RGB16BIT_565
                tempcolour = *((uint16_t *)bmp_data + read_off + (loopj - xs));
                if (tempcolour  != 0)
                {
                    writebuf[write_off + loopj] = tempcolour;
                }
#elif PIXEL_FORMAT == RGB24BIT_888
                uint8_t *pixel = bmp_data + (read_off + loopj - xs) * PIXEL_BYTES;
                if ((*pixel) || (*(pixel + 1)) || (*(pixel + 2)))
                {
                    writebuf[(write_off + loopj) * PIXEL_BYTES] = *pixel;
                    writebuf[(write_off + loopj) * PIXEL_BYTES + 1] = *(pixel + 1);
                    writebuf[(write_off + loopj) * PIXEL_BYTES + 2] = *(pixel + 2);
                }
#endif
            }
        }
    }
    else if (((xs + width) > LCD_WIDTH) && (xs <= LCD_WIDTH))
    {
        int width_cache;
        width_cache = LCD_WIDTH - xs;
        for (loopk = 0; loopk < addr_hh; loopk ++, loopy ++)
        {
            read_off = loopk * width;
            write_off = loopy * LCD_WIDTH;
            for (loopj = 0; loopj < width_cache; loopj ++)
            {
#if PIXEL_FORMAT == RGB16BIT_565
                tempcolour = *((uint16_t *)bmp_data + read_off + loopj);
                if (tempcolour  != 0)
                {
                    writebuf[write_off + xs + loopj] = tempcolour;
                }
#elif PIXEL_FORMAT == RGB24BIT_888
                uint8_t *pixel = bmp_data + (read_off + loopj) * PIXEL_BYTES;
                if ((*pixel) || (*(pixel + 1)) || (*(pixel + 2)))
                {
                    writebuf[(write_off + xs + loopj) * PIXEL_BYTES] = *pixel;
                    writebuf[(write_off + xs + loopj) * PIXEL_BYTES + 1] = *(pixel + 1);
                    writebuf[(write_off + xs + loopj) * PIXEL_BYTES + 2] = *(pixel + 2);
                }
#endif

            }
        }
    }
    else if ((xs >= 0) && ((xs + width) <= LCD_WIDTH))
    {
        for (loopk = 0; loopk < addr_hh; loopk ++, loopy ++)
        {
            read_off = loopk * width;
            write_off = loopy * LCD_WIDTH;
            for (loopj = 0; loopj < width; loopj ++)
            {
#if PIXEL_FORMAT == RGB16BIT_565
                tempcolour = *((uint16_t *)bmp_data + read_off + loopj);
                if (tempcolour  != 0)
                {
                    writebuf[write_off + xs + loopj] = tempcolour;
                }
#elif PIXEL_FORMAT == RGB24BIT_888
                uint8_t *pixel = bmp_data + (read_off + loopj) * PIXEL_BYTES;
                if ((*pixel) || (*(pixel + 1)) || (*(pixel + 2)))
                {
                    writebuf[(write_off + xs + loopj) * PIXEL_BYTES] = *pixel;
                    writebuf[(write_off + xs + loopj) * PIXEL_BYTES + 1] = *(pixel + 1);
                    writebuf[(write_off + xs + loopj) * PIXEL_BYTES + 2] = *(pixel + 2);
                }
#endif
            }
        }
    }

    return xs + width;
}


uint8_t rtl_gui_show_background(UI_WidgetTypeDef *widget, int16_t detal_x, int16_t detal_y, int Zs,
                                int Ze, uint8_t *buf)
{
    int xs = widget->x + detal_x;
    int ys = widget->y + detal_y;
    int width = widget->width;
    int height = widget->hight;
    unsigned char *BMP = (unsigned char *)widget->addr;
    int y_start = widget->active_ys;
    int y_end = widget->active_ye;
    if (BMP == 0)
    {
        return 0;
    }
    if ((y_start == 0) && (y_end == 0))
    {
        y_start = 0;
        y_end = LCD_HIGHT;
    }

    if (y_start > (Zs + LCD_SECTION_HEIGHT))
    {
        return 0;
    }
    if (y_end < Ze)
    {
        return 0;
    }

    uint16_t loopk;
    uint16_t loopy;
    uint16_t addr_hs;
    uint16_t addr_hh;
    uint16_t read_off;
    uint16_t write_off;
    uint8_t *bmp_data;

#if PIXEL_FORMAT == RGB16BIT_565
    uint16_t *writebuf = (uint16_t *)buf;
#elif PIXEL_FORMAT == RGB24BIT_888
    uint8_t *writebuf = buf;
#endif

    if ((ys >= Ze) || (xs >= LCD_WIDTH) || ((ys + height) <= Zs) || ((xs + width) <= 0) ||
        (width > LCD_WIDTH))
    {
        return 0;
    }

    if (ys > Zs)
    {
        addr_hs = 0;
        addr_hh = Ze - ys;
        addr_hh = (addr_hh > height) ? height : addr_hh;
        ys = ys - Zs;
    }
    else
    {
        if ((ys + height) < Ze)
        {
            addr_hs = Zs - ys;
            addr_hh = (ys + height) - Zs;
        }
        else
        {
            addr_hs = Zs - ys;
            addr_hh = Ze - Zs;
        }
        ys = 0;
    }
    loopy = ys;
#if 0
    if ((uint32_t)BMP  + width * PIXEL_BYTES * addr_hs >= 0x1000000)
    {
        extern uint16_t *disp_read_buff;
        extern bool flash_read_quick_locked(uint32_t start_addr, uint32_t data_len, uint8_t *data);
        flash_ioctl(flash_ioctl_set_seq_trans_enable, 0, 0);
        flash_read_quick_locked((uint32_t)BMP  + width * PIXEL_BYTES * addr_hs, \
                                LCD_WIDTH * LCD_SECTION_HEIGHT * PIXEL_BYTES, (uint8_t *)disp_read_buff);
        flash_ioctl(flash_ioctl_set_seq_trans_enable, 1, 0);
        bmp_data = disp_read_buff;
    }
#else
    bmp_data = (uint8_t *)(BMP + width * PIXEL_BYTES * addr_hs);
#endif

    if ((xs < 0) && ((xs + width) > 0))
    {
        int width_cache;
        width_cache = width + xs;
        for (loopk = 0; loopk < addr_hh; loopk ++, loopy ++)
        {
            read_off = loopk * width;
            write_off = loopy * LCD_WIDTH;
            memcpy(writebuf + write_off, (uint16_t *)bmp_data + read_off - xs, width_cache * PIXEL_BYTES);
        }
    }
    else if (((xs + width) > LCD_WIDTH) && (xs <= LCD_WIDTH))
    {
        int width_cache;
        width_cache = LCD_WIDTH - xs;
        for (loopk = 0; loopk < addr_hh; loopk ++, loopy ++)
        {
            read_off = loopk * width;
            write_off = loopy * LCD_WIDTH;
            memcpy(writebuf + write_off + xs, (uint16_t *)bmp_data + read_off, width_cache * PIXEL_BYTES);
        }
    }
    else if ((xs >= 0) && ((xs + width) <= LCD_WIDTH))
    {
        for (loopk = 0; loopk < addr_hh; loopk ++, loopy ++)
        {
            read_off = loopk * width;
            write_off = loopy * LCD_WIDTH;

            memcpy(writebuf + write_off + xs, (uint16_t *)bmp_data + read_off, width * PIXEL_BYTES);
        }
    }

    return xs + width;
}

