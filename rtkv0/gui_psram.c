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
#include "wristband_gui.h"
#include "gui_bmp.h"
#include "gui_psram.h"


uint8_t PSRAM_LCD_FRAME_BUFFER_ORIGIN[0x100000];
uint8_t PSRAM_LCD_FRAME_BUFFER_LEFT[0x100000];
uint8_t PSRAM_LCD_FRAME_BUFFER_RIGHT[0x100000];
uint8_t PSRAM_LCD_FRAME_BUFFER_DOWN[0x100000];
uint8_t PSRAM_LCD_FRAME_BUFFER_UP[0x100000];


UI_PSRAMBufferTypeDef FrameBufferOrigin;
UI_PSRAMBufferTypeDef FrameBufferLeft;
UI_PSRAMBufferTypeDef FrameBufferRight;
UI_PSRAMBufferTypeDef FrameBufferDown;
UI_PSRAMBufferTypeDef FrameBufferUp;



void psram_frame_buffer_init(void)
{
    FrameBufferOrigin.name = "origin";
    FrameBufferOrigin.pLCDBuffer = PSRAM_LCD_FRAME_BUFFER_ORIGIN;
    FrameBufferOrigin.Menu = NULL;
    FrameBufferOrigin.BufferReadFlag = false;

    FrameBufferLeft.name = "left";
    FrameBufferLeft.pLCDBuffer = PSRAM_LCD_FRAME_BUFFER_LEFT;
    FrameBufferLeft.Menu = NULL;
    FrameBufferLeft.BufferReadFlag = false;

    FrameBufferRight.name = "right";
    FrameBufferRight.pLCDBuffer = PSRAM_LCD_FRAME_BUFFER_RIGHT;
    FrameBufferRight.Menu = NULL;
    FrameBufferRight.BufferReadFlag = false;

    FrameBufferDown.name = "down";
    FrameBufferDown.pLCDBuffer = PSRAM_LCD_FRAME_BUFFER_DOWN;
    FrameBufferDown.Menu = NULL;
    FrameBufferDown.BufferReadFlag = false;

    FrameBufferUp.name = "up";
    FrameBufferUp.pLCDBuffer = PSRAM_LCD_FRAME_BUFFER_UP;
    FrameBufferUp.Menu = NULL;
    FrameBufferUp.BufferReadFlag = false;
}

int16_t psram_detal_x = 0;
int16_t psram_detal_y = 0;



void rtl_gui_lcd_update_from_psram(void)
{
    if (psram_detal_x)
    {
        psram_detal_y = 0;
    }

    if (psram_detal_y == 0)
    {
        if (psram_detal_x >= 0)
        {
            rtl_gui_copy_psram_to_lcd_x(FrameBufferLeft, FrameBufferOrigin, LCD_WIDTH - psram_detal_x);
        }
        else
        {
            rtl_gui_copy_psram_to_lcd_x(FrameBufferOrigin, FrameBufferRight, -psram_detal_x);
        }
    }
    else
    {
        if (psram_detal_y > 0)
        {
            rtl_gui_copy_psram_to_lcd_y(FrameBufferUp, FrameBufferOrigin, LCD_HIGHT - psram_detal_y);
        }
        else
        {
            rtl_gui_copy_psram_to_lcd_y(FrameBufferOrigin, FrameBufferDown, -psram_detal_y);
        }
    }
}


void rtl_gui_copy_psram_to_lcd_x(UI_PSRAMBufferTypeDef left, UI_PSRAMBufferTypeDef right,
                                 uint32_t offset)//offset mean left menu not display in current menu
{
    if (offset > LCD_WIDTH)
    {
        return;
    }
    gui_dispdev_t *dc = gui_get_dc();
    for (uint16_t i = 0; i < dc->screen_height; i++)
    {
        uint16_t left_len = (dc->screen_width - offset) * 2;
        uint16_t right_len = offset * 2;

        memcpy(dc->lcd_gram + i * (dc->screen_width * 2), \
               left.pLCDBuffer + offset * 2 + i * (dc->screen_width * 2), \
               left_len);

        memcpy(dc->lcd_gram + i * (dc->screen_width * 2) + left_len, \
               right.pLCDBuffer + i * (dc->screen_width * 2),
               right_len);

    }

}


void rtl_gui_copy_psram_to_lcd_y(UI_PSRAMBufferTypeDef up, UI_PSRAMBufferTypeDef down,
                                 uint32_t offset) //offset mean up menu not display in current menu
{
    if (offset > LCD_HIGHT)
    {
        return;
    }

    GUI_ASSERT(NULL != NULL);

}



void rtl_gui_copy_ram_to_psram(uint8_t *readbuf, uint32_t count, UI_PSRAMBufferTypeDef *location,
                               uint32_t length)
{
    memcpy((uint8_t *)(location->pLCDBuffer + count * LCD_SECTION_BYTE_LEN), readbuf,
           LCD_WIDTH * LCD_SECTION_HEIGHT * PIXEL_BYTES);
}

void rtl_gui_update_to_psram_framebuffer(UI_MenuTypeDef *p_show_menu,
                                         UI_PSRAMBufferTypeDef *location)
{
    if (p_show_menu == NULL)
    {
        return;
    }
    /* cache buffer for dma source address */
    uint8_t *pWriteBuf = NULL;
    UI_MenuTypeDef *local_cur_menu = p_show_menu;
    UI_WidgetTypeDef *local_cur_widget_list = p_show_menu->pWidgetList;
    UI_BMPTypeDef *local_cur_bmp_list = p_show_menu->pBMPList;


    for (uint16_t i = 0; i < TOTAL_SECTION_COUNT; i++)
    {
        if (i % 2)
        {
            pWriteBuf = disp_write_buff2;
        }
        else
        {
            pWriteBuf = disp_write_buff1;
        }

        memset(pWriteBuf, 0x00, LCD_SECTION_HEIGHT * LCD_WIDTH * PIXEL_BYTES);

        for (uint32_t j = 0; j < local_cur_menu->current_max_widget; j++)
        {
            if ((local_cur_widget_list + j)->widget_id_type == ICON_WIDGET)
            {
                rtl_gui_show_bmp_sector(local_cur_widget_list + j, \
                                        0, 0, \
                                        i * LCD_SECTION_HEIGHT, (i + 1)*LCD_SECTION_HEIGHT, \
                                        pWriteBuf);
            }
            else if ((local_cur_widget_list + j)->widget_id_type == ICON_BACKGROUND)
            {
                rtl_gui_show_background(local_cur_menu, \
                                        local_cur_widget_list + j, \
                                        i * LCD_SECTION_HEIGHT, (i + 1)*LCD_SECTION_HEIGHT, \
                                        pWriteBuf);
            }
        }
        for (uint8_t j = 0; j < local_cur_menu->current_max_bmp; j++)
        {
            if (((local_cur_bmp_list + j)->y >= (i + 1)*LCD_SECTION_HEIGHT) ||
                (((local_cur_bmp_list + j)->y + (local_cur_bmp_list + j)->hight) <= i * LCD_SECTION_HEIGHT))
            {
                continue;
            }
            rtl_gui_show_bmp_simple(local_cur_bmp_list + j, \
                                    local_cur_menu->detal_x, 0, \
                                    i * LCD_SECTION_HEIGHT, (i + 1)*LCD_SECTION_HEIGHT, \
                                    pWriteBuf);
        }

        rtl_gui_copy_ram_to_psram(pWriteBuf, i, location, LCD_WIDTH * LCD_SECTION_HEIGHT * PIXEL_BYTES);
    }
}




