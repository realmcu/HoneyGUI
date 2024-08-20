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
#include "draw_img.h"
#include "acc_init.h"


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



static void set_w_and_h(void *addr, uint16_t w, uint16_t h)
{
    gui_rgb_data_head_t *head = (gui_rgb_data_head_t *)addr;
    head->scan = 0;
    head->align = 0;
    head->resize = 0;//0-no resize;1-50%(x&y);2-70%;3-80%
    head->compress = 0;
    head->rsvd = 0;
    head->type = RGB565;
    head->version = 0;
    head->rsvd2 = 0;
    head->w = w;
    head->h = h;
}

void psram_frame_buffer_init(void)
{
    FrameBufferOrigin.name = "origin";
    FrameBufferOrigin.pLCDBuffer = PSRAM_LCD_FRAME_BUFFER_ORIGIN + 8;
    FrameBufferOrigin.Menu = NULL;
    FrameBufferOrigin.BufferReadFlag = false;
    set_w_and_h(PSRAM_LCD_FRAME_BUFFER_ORIGIN, 480, 480);

    FrameBufferLeft.name = "left";
    FrameBufferLeft.pLCDBuffer = PSRAM_LCD_FRAME_BUFFER_LEFT + 8;
    FrameBufferLeft.Menu = NULL;
    FrameBufferLeft.BufferReadFlag = false;
    set_w_and_h(PSRAM_LCD_FRAME_BUFFER_LEFT, 480, 480);

    FrameBufferRight.name = "right";
    FrameBufferRight.pLCDBuffer = PSRAM_LCD_FRAME_BUFFER_RIGHT + 8;
    FrameBufferRight.Menu = NULL;
    FrameBufferRight.BufferReadFlag = false;
    set_w_and_h(PSRAM_LCD_FRAME_BUFFER_RIGHT, 480, 480);

    FrameBufferDown.name = "down";
    FrameBufferDown.pLCDBuffer = PSRAM_LCD_FRAME_BUFFER_DOWN + 8;
    FrameBufferDown.Menu = NULL;
    FrameBufferDown.BufferReadFlag = false;
    set_w_and_h(PSRAM_LCD_FRAME_BUFFER_DOWN, 480, 480);

    FrameBufferUp.name = "up";
    FrameBufferUp.pLCDBuffer = PSRAM_LCD_FRAME_BUFFER_UP + 8;
    FrameBufferUp.Menu = NULL;
    FrameBufferUp.BufferReadFlag = false;
    set_w_and_h(PSRAM_LCD_FRAME_BUFFER_UP, 480, 480);
}



static void rtl_gui_cutscene_to_lcd(uint8_t *left, uint8_t *right, float depend_touch,
                                    uint8_t style)
{

    draw_img_t image_left;
    draw_img_t image_right;
    gui_dispdev_t dc;

    memcpy(&dc, gui_get_dc(), sizeof(gui_dispdev_t));

    image_left.img_w = dc.screen_width;
    image_left.img_h = dc.screen_height;
    image_left.data = left - 8 ;
    image_left.opacity_value = 255;
    image_left.blend_mode = IMG_FILTER_BLACK;

    image_right.img_w = dc.screen_width;
    image_right.img_h = dc.screen_height;
    image_right.data = right - 8;
    image_right.opacity_value = 255;
    image_right.blend_mode = IMG_FILTER_BLACK;

#if 0
    matrix_identity(&image_left.matrix);
    matrix_translate(depend_touch, 0, &image_left.matrix);

    matrix_identity(&image_right.matrix);
    matrix_translate(depend_touch + dc.screen_width, 0, &image_right.matrix);
#else
    extern void gui_v0_rotate(gui_matrix_t *matrix, int32_t x);
    gui_v0_rotate(&image_left.matrix, depend_touch);
    gui_v0_rotate(&image_right.matrix, depend_touch + dc.screen_width);

#endif

    memcpy(&image_left.inverse, &image_left.matrix, sizeof(gui_matrix_t));
    matrix_inverse(&image_left.inverse);
    draw_img_new_area(&image_left, NULL);

    memcpy(&image_right.inverse, &image_right.matrix, sizeof(gui_matrix_t));
    matrix_inverse(&image_right.inverse);
    draw_img_new_area(&image_right, NULL);

    for (uint32_t i = 0; i < dc.section_total; i++)
    {
        if (i % 2 == 0)
        {
            dc.frame_buf = dc.disp_buf_1;
        }
        else
        {
            dc.frame_buf = dc.disp_buf_2;
        }
        memset(dc.frame_buf, 0x00, dc.fb_height * dc.fb_width * (dc.bit_depth >> 3));

        dc.section_count = i;
        dc.section.x1 = 0;
        dc.section.x2 = LCD_WIDTH - 1;
        dc.section.y1 = dc.section_count * dc.fb_height;
        dc.section.y2 = (dc.section_count + 1) * dc.fb_height;
        gui_acc_blit_to_dc(&image_left, &dc, NULL);
        gui_acc_blit_to_dc(&image_right, &dc, NULL);
        extern void rtl_gui_refresh_by_dma_internal(uint8_t *readbuf, uint32_t count_for_section);
        rtl_gui_refresh_by_dma_internal(dc.frame_buf, i);

    }
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
        if (psram_detal_x > 0)
        {
            // rtl_gui_copy_psram_to_lcd_x(FrameBufferLeft, FrameBufferOrigin, LCD_WIDTH - psram_detal_x);
            rtl_gui_cutscene_to_lcd(FrameBufferLeft.pLCDBuffer, FrameBufferOrigin.pLCDBuffer, psram_detal_x, 0);
        }
        else
        {
            // rtl_gui_copy_psram_to_lcd_x(FrameBufferOrigin, FrameBufferRight, -psram_detal_x);
            rtl_gui_cutscene_to_lcd(FrameBufferOrigin.pLCDBuffer, FrameBufferRight.pLCDBuffer, psram_detal_x,
                                    0);
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
        for (uint8_t j = 0; j < local_cur_menu->current_max_3d; j++)
        {
            rtl_gui_show_3d(local_cur_menu, \
                            p_show_menu->p3DList + j, \
                            i * LCD_SECTION_HEIGHT, (i + 1)*LCD_SECTION_HEIGHT, \
                            pWriteBuf);
        }
        rtl_gui_copy_ram_to_psram(pWriteBuf, i, location, LCD_WIDTH * LCD_SECTION_HEIGHT * PIXEL_BYTES);
    }
}





