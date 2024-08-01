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
#include "gui_core.h"
#include "gui_bmp.h"
#include "gui_psram.h"
#include "wristband_gui.h"




#define GUI_GLOBAL_WIDGET_MAX       29
#define GUI_GLOBAL_BMP_MAX       50


GUI_ConfigDef rtl_gui_config = {false, false};
uint8_t __attribute__((aligned(4))) rtl_gui_core_buff[LCD_WIDTH * LCD_SECTION_HEIGHT * PIXEL_BYTES *
                                                                3];
uint8_t *disp_write_buff1 = rtl_gui_core_buff;
uint8_t *disp_write_buff2 = NULL;
uint8_t *disp_read_buff = NULL;
uint8_t *dma_link_list_buff = rtl_gui_core_buff;

bool lcd_screen_updating_flag = false;
GUI_FRAMEBUFFER_MODE FrameBufferMode = FRAMEBUFFER_MODE_NONE_PSRAM;

static UI_WidgetTypeDef  g_cur_widget[GUI_GLOBAL_WIDGET_MAX];
static UI_WidgetTypeDef  g_sub_widget[GUI_GLOBAL_WIDGET_MAX];
static UI_WidgetTypeDef  g_par_widget[GUI_GLOBAL_WIDGET_MAX];

static UI_BMPTypeDef  g_cur_bmp[GUI_GLOBAL_BMP_MAX];
static UI_BMPTypeDef  g_sub_bmp[GUI_GLOBAL_BMP_MAX];
static UI_BMPTypeDef  g_par_bmp[GUI_GLOBAL_BMP_MAX];

UI_MenuTypeDef g_cur_menu =
{
    /* change Here for UI */
    .name = (const char *)"g_cur_menu",
    .MenuParent = NULL,
    .MenuSub = NULL,
    .button_func = NULL,
    .touch_func = NULL,
    .pWidgetList = g_cur_widget,
    .pBMPList = g_cur_bmp,
    .current_max_widget = 0,
};
UI_MenuTypeDef g_sub_menu =
{
    /* change Here for UI */
    .name = (const char *)"g_sub_menu",
    .MenuParent = NULL,
    .MenuSub = NULL,
    .button_func = NULL,
    .touch_func = NULL,
    .pWidgetList = g_sub_widget,
    .pBMPList = g_sub_bmp,
    .current_max_widget = 0,
};
UI_MenuTypeDef g_par_menu =
{
    /* change Here for UI */
    .name = (const char *)"g_par_menu",
    .MenuParent = NULL,
    .MenuSub = NULL,
    .button_func = NULL,
    .touch_func = NULL,
    .pWidgetList = g_par_widget,
    .pBMPList = g_par_bmp,
    .current_max_widget = 0,
};



void gui_core_init(void)
{
    disp_write_buff1 = rtl_gui_core_buff;
    disp_write_buff2 = disp_write_buff1 + LCD_WIDTH * LCD_SECTION_HEIGHT * PIXEL_BYTES;
    disp_read_buff = disp_write_buff1 + LCD_WIDTH * LCD_SECTION_HEIGHT * PIXEL_BYTES * 2;
#if FEATURE_PSRAM
    psram_frame_buffer_init();
#endif
}

void gui_set_te_function(bool flag)
{
    rtl_gui_config.te_enable = flag;
}



void rtl_gui_refresh_by_dma_internal(uint8_t *readbuf, uint32_t count_for_section)
{
    // GUI_ASSERT(NULL != NULL);
    gui_dispdev_t *dc = gui_get_dc();

    if (count_for_section == TOTAL_SECTION_COUNT - 1)
    {
        uint32_t length = ((LCD_HIGHT - count_for_section * LCD_SECTION_HEIGHT) % LCD_SECTION_HEIGHT) *
                          LCD_WIDTH * PIXEL_BYTES;
        memcpy(dc->lcd_gram + count_for_section * LCD_SECTION_BYTE_LEN, readbuf, length);
    }
    else
    {
        memcpy(dc->lcd_gram + count_for_section * LCD_SECTION_BYTE_LEN, readbuf, LCD_SECTION_BYTE_LEN);
    }

}



static void rtl_gui_lcd_real_update_x(UI_MenuTypeDef *p_show_menu)
{
    /* cache buffer for dma source address */
    uint8_t *pWriteBuf = NULL;
    p_show_menu->MenuSub->detal_x = p_show_menu->detal_x + LCD_WIDTH;
    p_show_menu->MenuParent->detal_x = p_show_menu->detal_x - LCD_WIDTH;

    UI_MenuTypeDef *local_cur_menu = p_show_menu;
    UI_WidgetTypeDef *local_cur_widget_list = p_show_menu->pWidgetList;
    UI_BMPTypeDef *local_cur_bmp_list = p_show_menu->pBMPList;

    UI_MenuTypeDef *local_sub_menu = p_show_menu->MenuSub;
    UI_WidgetTypeDef *local_sub_widget_list = p_show_menu->MenuSub->pWidgetList;
    UI_BMPTypeDef *local_sub_bmp_list = p_show_menu->MenuSub->pBMPList;

    UI_MenuTypeDef *local_par_menu = p_show_menu->MenuParent;
    UI_WidgetTypeDef *local_par_widget_list = p_show_menu->MenuParent->pWidgetList;
    UI_BMPTypeDef *local_par_bmp_list = p_show_menu->MenuParent->pBMPList;

    for (uint8_t i = 0; i < TOTAL_SECTION_COUNT; i++)
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

        for (uint8_t j = 0; j < local_cur_menu->current_max_widget; j++)
        {
            if (((local_cur_widget_list + j)->y >= (i + 1)*LCD_SECTION_HEIGHT) ||
                (((local_cur_widget_list + j)->y + (local_cur_widget_list + j)->hight) <= i * LCD_SECTION_HEIGHT))
            {
                continue;
            }

            if ((local_cur_widget_list + j)->widget_id_type == ICON_WIDGET)
            {
                rtl_gui_show_bmp_sector(local_cur_widget_list + j, \
                                        local_cur_menu->detal_x, 0, \
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

        if (local_sub_menu->detal_x < LCD_WIDTH)
        {
            for (uint8_t j = 0; j < local_sub_menu->current_max_widget; j++)
            {
                if (((local_sub_widget_list + j)->y >= (i + 1)*LCD_SECTION_HEIGHT) ||
                    (((local_sub_widget_list + j)->y + (local_sub_widget_list + j)->hight) <= i * LCD_SECTION_HEIGHT))
                {
                    continue;
                }

                if ((local_sub_widget_list + j)->widget_id_type == ICON_WIDGET)
                {
                    rtl_gui_show_bmp_sector(local_sub_widget_list + j, \
                                            local_sub_menu->detal_x, 0, \
                                            i * LCD_SECTION_HEIGHT, (i + 1)*LCD_SECTION_HEIGHT, \
                                            pWriteBuf);
                }
                else if ((local_sub_widget_list + j)->widget_id_type == ICON_BACKGROUND)
                {
                    rtl_gui_show_background(local_sub_menu, \
                                            local_sub_widget_list + j, \
                                            i * LCD_SECTION_HEIGHT, (i + 1)*LCD_SECTION_HEIGHT, \
                                            pWriteBuf);
                }
            }
            for (uint8_t j = 0; j < local_sub_menu->current_max_bmp; j++)
            {
                if (((local_sub_bmp_list + j)->y >= (i + 1)*LCD_SECTION_HEIGHT) ||
                    (((local_sub_bmp_list + j)->y + (local_sub_bmp_list + j)->hight) <= i * LCD_SECTION_HEIGHT))
                {
                    continue;
                }
                rtl_gui_show_bmp_simple(local_sub_bmp_list + j, \
                                        local_sub_menu->detal_x, 0, \
                                        i * LCD_SECTION_HEIGHT, (i + 1)*LCD_SECTION_HEIGHT, \
                                        pWriteBuf);
            }
        }

        if (local_par_menu->detal_x > -LCD_WIDTH)
        {
            for (uint8_t j = 0; j < local_par_menu->current_max_widget; j++)
            {
                if (((local_par_widget_list + j)->y >= (i + 1)*LCD_SECTION_HEIGHT) ||
                    (((local_par_widget_list + j)->y + (local_par_widget_list + j)->hight) <= i * LCD_SECTION_HEIGHT))
                {
                    continue;
                }
                if ((local_par_widget_list + j)->widget_id_type == ICON_WIDGET)
                {
                    rtl_gui_show_bmp_sector(local_par_widget_list + j, \
                                            local_par_menu->detal_x, 0, \
                                            i * LCD_SECTION_HEIGHT, (i + 1)*LCD_SECTION_HEIGHT, \
                                            pWriteBuf);
                }
                else if ((local_par_widget_list + j)->widget_id_type == ICON_BACKGROUND)
                {
                    rtl_gui_show_background(local_par_menu, \
                                            local_par_widget_list + j, \
                                            i * LCD_SECTION_HEIGHT, (i + 1)*LCD_SECTION_HEIGHT, \
                                            pWriteBuf);
                }
            }
            for (uint8_t j = 0; j < local_par_menu->current_max_bmp; j++)
            {
                if (((local_par_bmp_list + j)->y >= (i + 1)*LCD_SECTION_HEIGHT) ||
                    (((local_par_bmp_list + j)->y + (local_par_bmp_list + j)->hight) <= i * LCD_SECTION_HEIGHT))
                {
                    continue;
                }
                rtl_gui_show_bmp_simple(local_par_bmp_list + j, \
                                        local_par_menu->detal_x, 0, \
                                        i * LCD_SECTION_HEIGHT, (i + 1)*LCD_SECTION_HEIGHT, \
                                        pWriteBuf);
            }
        }
        rtl_gui_refresh_by_dma_internal(pWriteBuf, i);
    }
}


static void rtl_gui_lcd_real_update_y(UI_MenuTypeDef *p_show_menu)
{
    /* cache buffer for dma source address */
    uint8_t *pWriteBuf = NULL;
    p_show_menu->MenuSub->detal_y = p_show_menu->detal_y + LCD_HIGHT;
    p_show_menu->MenuParent->detal_y = p_show_menu->detal_y - LCD_HIGHT;

    UI_MenuTypeDef *local_cur_menu = p_show_menu;
    UI_WidgetTypeDef *local_cur_widget_list = p_show_menu->pWidgetList;
    UI_BMPTypeDef *local_cur_bmp_list = p_show_menu->pBMPList;

    UI_MenuTypeDef *local_sub_menu = p_show_menu->MenuSub;
    UI_WidgetTypeDef *local_sub_widget_list = p_show_menu->MenuSub->pWidgetList;
    UI_BMPTypeDef *local_sub_bmp_list = p_show_menu->MenuSub->pBMPList;

    UI_MenuTypeDef *local_par_menu = p_show_menu->MenuParent;
    UI_WidgetTypeDef *local_par_widget_list = p_show_menu->MenuParent->pWidgetList;
    UI_BMPTypeDef *local_par_bmp_list = p_show_menu->MenuParent->pBMPList;

    for (uint8_t i = 0; i < TOTAL_SECTION_COUNT; i++)
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

        for (uint8_t j = 0; j < local_cur_menu->current_max_widget; j++)
        {
            if (((local_cur_widget_list + j)->y + local_cur_menu->detal_y >= (i + 1)*LCD_SECTION_HEIGHT) ||
                (((local_cur_widget_list + j)->y + local_cur_menu->detal_y + (local_cur_widget_list + j)->hight) <=
                 i *
                 LCD_SECTION_HEIGHT))
            {
                continue;
            }
            if ((local_cur_widget_list + j)->widget_id_type == ICON_WIDGET)
            {
                rtl_gui_show_bmp_sector(local_cur_widget_list + j, \
                                        0, local_cur_menu->detal_y, \
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
            if (((local_cur_bmp_list + j)->y + local_cur_menu->detal_y >= (i + 1)*LCD_SECTION_HEIGHT) ||
                (((local_cur_bmp_list + j)->y + local_cur_menu->detal_y + (local_cur_bmp_list + j)->hight) <= i *
                 LCD_SECTION_HEIGHT))
            {
                continue;
            }
            rtl_gui_show_bmp_simple(local_cur_bmp_list + j, \
                                    0, local_cur_menu->detal_y, \
                                    i * LCD_SECTION_HEIGHT, (i + 1)*LCD_SECTION_HEIGHT, \
                                    pWriteBuf);
        }

        if (local_sub_menu->detal_y < LCD_HIGHT)
        {
            for (uint8_t j = 0; j < local_sub_menu->current_max_widget; j++)
            {
                if (((local_sub_widget_list + j)->y + local_sub_menu->detal_y >= (i + 1)*LCD_SECTION_HEIGHT) ||
                    (((local_sub_widget_list + j)->y + local_sub_menu->detal_y + (local_sub_widget_list + j)->hight) <=
                     i *
                     LCD_SECTION_HEIGHT))
                {
                    continue;
                }
                if ((local_sub_widget_list + j)->widget_id_type == ICON_WIDGET)
                {
                    rtl_gui_show_bmp_sector(local_sub_widget_list + j, \
                                            0, local_sub_menu->detal_y, \
                                            i * LCD_SECTION_HEIGHT, (i + 1)*LCD_SECTION_HEIGHT, \
                                            pWriteBuf);
                }
                else if ((local_sub_widget_list + j)->widget_id_type == ICON_BACKGROUND)
                {
                    rtl_gui_show_background(local_sub_menu, \
                                            local_sub_widget_list + j, \
                                            i * LCD_SECTION_HEIGHT, (i + 1)*LCD_SECTION_HEIGHT, \
                                            pWriteBuf);
                }
            }
            for (uint8_t j = 0; j < local_sub_menu->current_max_bmp; j++)
            {
                if (((local_sub_bmp_list + j)->y + local_sub_menu->detal_y >= (i + 1)*LCD_SECTION_HEIGHT) ||
                    (((local_sub_bmp_list + j)->y + local_sub_menu->detal_y + (local_sub_bmp_list + j)->hight) <= i *
                     LCD_SECTION_HEIGHT))
                {
                    continue;
                }
                rtl_gui_show_bmp_simple(local_sub_bmp_list + j, \
                                        0, local_sub_menu->detal_y, \
                                        i * LCD_SECTION_HEIGHT, (i + 1)*LCD_SECTION_HEIGHT, \
                                        pWriteBuf);
            }
        }

        if (local_par_menu->detal_y > -LCD_HIGHT)
        {
            for (uint8_t j = 0; j < local_par_menu->current_max_widget; j++)
            {
                if (((local_par_widget_list + j)->y + local_par_menu->detal_y >= (i + 1)*LCD_SECTION_HEIGHT) ||
                    (((local_par_widget_list + j)->y + local_par_menu->detal_y + (local_par_widget_list + j)->hight) <=
                     i *
                     LCD_SECTION_HEIGHT))
                {
                    continue;
                }
                if ((local_par_widget_list + j)->widget_id_type == ICON_WIDGET)
                {
                    rtl_gui_show_bmp_sector(local_par_widget_list + j, \
                                            0, local_par_menu->detal_y, \
                                            i * LCD_SECTION_HEIGHT, (i + 1)*LCD_SECTION_HEIGHT, \
                                            pWriteBuf);
                }
                else if ((local_par_widget_list + j)->widget_id_type == ICON_BACKGROUND)
                {
                    rtl_gui_show_background(local_par_menu, \
                                            local_par_widget_list + j, \
                                            i * LCD_SECTION_HEIGHT, (i + 1)*LCD_SECTION_HEIGHT, \
                                            pWriteBuf);
                }
            }
            for (uint8_t j = 0; j < local_par_menu->current_max_bmp; j++)
            {
                if (((local_par_bmp_list + j)->y + local_par_menu->detal_y >= (i + 1)*LCD_SECTION_HEIGHT) ||
                    (((local_par_bmp_list + j)->y + local_par_menu->detal_y + (local_par_bmp_list + j)->hight) <= i *
                     LCD_SECTION_HEIGHT))
                {
                    continue;
                }
                rtl_gui_show_bmp_simple(local_par_bmp_list + j, \
                                        0, local_par_menu->detal_y, \
                                        i * LCD_SECTION_HEIGHT, (i + 1)*LCD_SECTION_HEIGHT, \
                                        pWriteBuf);
            }
        }
        rtl_gui_refresh_by_dma_internal(pWriteBuf, i);
    }
}

void rtl_gui_lcd_real_update_from_internal_ram(UI_MenuTypeDef *p_show_menu)
{
    if (p_show_menu == NULL)
    {
        return;
    }

    if (p_show_menu->detal_y == 0)
    {
        rtl_gui_lcd_real_update_x(p_show_menu);
    }
    else
    {
        rtl_gui_lcd_real_update_y(p_show_menu);
    }


}



void rtl_gui_lcd_update_internal(void)
{
    lcd_screen_updating_flag = true;


    if (FrameBufferMode == FRAMEBUFFER_MODE_NONE_PSRAM)
    {
        rtl_gui_lcd_real_update_from_internal_ram(&g_cur_menu);
    }
#if FEATURE_PSRAM
    else
    {
        rtl_gui_lcd_update_from_psram();
    }
#endif

    lcd_screen_updating_flag = false;

}


void rtl_gui_menu_update(UI_MenuTypeDef *ui_cur, void *p_ui_cur_argv, \
                         UI_MenuTypeDef *ui_sub, void *p_ui_sub_argv, \
                         UI_MenuTypeDef *ui_par, void *p_ui_par_argv, \
                         int16_t detal_x, int16_t detal_y)
{
    FrameBufferMode = FRAMEBUFFER_MODE_NONE_PSRAM;
    if (ui_cur != NULL)
    {
        g_cur_menu.current_max_widget = ui_cur->current_max_widget;
        g_cur_menu.current_max_bmp = ui_cur->current_max_bmp;
        memcpy(g_cur_menu.pWidgetList, ui_cur->pWidgetList,
               ui_cur->current_max_widget * sizeof(UI_WidgetTypeDef));
        memcpy(g_cur_menu.pBMPList, ui_cur->pBMPList,
               ui_cur->current_max_bmp * sizeof(UI_BMPTypeDef));
        g_cur_menu.name = ui_cur->name;
        ui_cur->cur_display_info(ui_cur, &g_cur_menu, p_ui_cur_argv);

    }
    else
    {
        g_cur_menu.current_max_widget = 0;
        g_cur_menu.current_max_bmp = 0;
    }
    if (ui_sub != NULL)
    {
        g_sub_menu.current_max_widget = ui_sub->current_max_widget;
        g_sub_menu.current_max_bmp = ui_sub->current_max_bmp;
        memcpy(g_sub_menu.pWidgetList, ui_sub->pWidgetList,
               ui_sub->current_max_widget * sizeof(UI_WidgetTypeDef));
        memcpy(g_sub_menu.pBMPList, ui_sub->pBMPList,
               ui_sub->current_max_bmp * sizeof(UI_BMPTypeDef));
        ui_sub->cur_display_info(ui_sub, &g_sub_menu, p_ui_sub_argv);
    }
    else
    {
        g_sub_menu.current_max_widget = 0;
        g_sub_menu.current_max_bmp = 0;
    }
    if (ui_par != NULL)
    {
        g_par_menu.current_max_widget = ui_par->current_max_widget;
        g_par_menu.current_max_bmp = ui_par->current_max_bmp;
        memcpy(g_par_menu.pWidgetList, ui_par->pWidgetList,
               ui_par->current_max_widget * sizeof(UI_WidgetTypeDef));
        memcpy(g_par_menu.pBMPList, ui_par->pBMPList,
               ui_par->current_max_bmp * sizeof(UI_BMPTypeDef));
        ui_par->cur_display_info(ui_par, &g_par_menu, p_ui_par_argv);
    }
    else
    {
        g_par_menu.current_max_widget = 0;
        g_par_menu.current_max_bmp = 0;
    }
    g_cur_menu.detal_x = detal_x;
    if (detal_x)
    {
        g_cur_menu.detal_y = 0;
    }
    else
    {
        g_cur_menu.detal_y = detal_y;
    }

    g_cur_menu.MenuSub = &g_sub_menu;
    g_cur_menu.MenuParent = &g_par_menu;


    rtl_gui_lcd_update_internal();
}


void rtl_gui_prepare_frame_buffer(UI_MenuTypeDef *pCurMenu, \
                                  UI_MenuTypeDef *pLoadMenu, \
                                  void *p_ui_cur_argv, \
                                  UI_PSRAMBufferTypeDef *pPSRAM)
{
    FrameBufferMode = FRAMEBUFFER_MODE_PSRAM;
    if (pLoadMenu == NULL)
    {
        return;
    }
    lcd_screen_updating_flag = true;
    if (pPSRAM == &FrameBufferOrigin)
    {
        if (pCurMenu->psram_has_center_cache == false)
        {
            g_cur_menu.current_max_widget = pLoadMenu->current_max_widget;
            g_cur_menu.current_max_bmp = pLoadMenu->current_max_bmp;
            memcpy(g_cur_menu.pWidgetList, pLoadMenu->pWidgetList,
                   pLoadMenu->current_max_widget * sizeof(UI_WidgetTypeDef));
            memcpy(g_cur_menu.pBMPList, pLoadMenu->pBMPList,
                   pLoadMenu->current_max_bmp * sizeof(UI_BMPTypeDef));
            pLoadMenu->cur_display_info(pLoadMenu, &g_cur_menu, p_ui_cur_argv);
            rtl_gui_update_to_psram_framebuffer(&g_cur_menu, pPSRAM);
            pCurMenu->psram_has_center_cache = true;
        }
    }
    else if (pPSRAM == &FrameBufferLeft)
    {
        if (pCurMenu->psram_has_left_cache == false)
        {
            g_cur_menu.current_max_widget = pLoadMenu->current_max_widget;
            g_cur_menu.current_max_bmp = pLoadMenu->current_max_bmp;
            memcpy(g_cur_menu.pWidgetList, pLoadMenu->pWidgetList,
                   pLoadMenu->current_max_widget * sizeof(UI_WidgetTypeDef));
            memcpy(g_cur_menu.pBMPList, pLoadMenu->pBMPList,
                   pLoadMenu->current_max_bmp * sizeof(UI_BMPTypeDef));
            pLoadMenu->cur_display_info(pLoadMenu, &g_cur_menu, p_ui_cur_argv);
            rtl_gui_update_to_psram_framebuffer(&g_cur_menu, pPSRAM);
            pCurMenu->psram_has_left_cache = true;
        }
    }
    else if (pPSRAM == &FrameBufferRight)
    {
        if (pCurMenu->psram_has_right_cache == false)
        {
            g_cur_menu.current_max_widget = pLoadMenu->current_max_widget;
            g_cur_menu.current_max_bmp = pLoadMenu->current_max_bmp;
            memcpy(g_cur_menu.pWidgetList, pLoadMenu->pWidgetList,
                   pLoadMenu->current_max_widget * sizeof(UI_WidgetTypeDef));
            memcpy(g_cur_menu.pBMPList, pLoadMenu->pBMPList,
                   pLoadMenu->current_max_bmp * sizeof(UI_BMPTypeDef));
            pLoadMenu->cur_display_info(pLoadMenu, &g_cur_menu, p_ui_cur_argv);
            rtl_gui_update_to_psram_framebuffer(&g_cur_menu, pPSRAM);
            pCurMenu->psram_has_right_cache = true;
        }
    }
    else if (pPSRAM == &FrameBufferDown)
    {
        if (pCurMenu->psram_has_down_cache == false)
        {
            g_cur_menu.current_max_widget = pLoadMenu->current_max_widget;
            g_cur_menu.current_max_bmp = pLoadMenu->current_max_bmp;
            memcpy(g_cur_menu.pWidgetList, pLoadMenu->pWidgetList,
                   pLoadMenu->current_max_widget * sizeof(UI_WidgetTypeDef));
            memcpy(g_cur_menu.pBMPList, pLoadMenu->pBMPList,
                   pLoadMenu->current_max_bmp * sizeof(UI_BMPTypeDef));
            pLoadMenu->cur_display_info(pLoadMenu, &g_cur_menu, p_ui_cur_argv);
            rtl_gui_update_to_psram_framebuffer(&g_cur_menu, pPSRAM);
            pCurMenu->psram_has_down_cache = true;
        }
    }
    else if (pPSRAM == &FrameBufferUp)
    {
        if (pCurMenu->psram_has_up_cache == false)
        {
            g_cur_menu.current_max_widget = pLoadMenu->current_max_widget;
            g_cur_menu.current_max_bmp = pLoadMenu->current_max_bmp;
            memcpy(g_cur_menu.pWidgetList, pLoadMenu->pWidgetList,
                   pLoadMenu->current_max_widget * sizeof(UI_WidgetTypeDef));
            memcpy(g_cur_menu.pBMPList, pLoadMenu->pBMPList,
                   pLoadMenu->current_max_bmp * sizeof(UI_BMPTypeDef));
            pLoadMenu->cur_display_info(pLoadMenu, &g_cur_menu, p_ui_cur_argv);
            rtl_gui_update_to_psram_framebuffer(&g_cur_menu, pPSRAM);
            pCurMenu->psram_has_up_cache = true;
        }
    }

    lcd_screen_updating_flag = false;
}



void rtl_gui_lcd_update(bool force_update)
{
    if (force_update)
    {
        rtl_gui_lcd_update_internal();
        gui_log("[GUI CORE] Be careful this case! \n");
    }
    else
    {
        rtl_gui_lcd_update_internal();
    }
}


void rtl_gui_update_detal(int16_t x, int16_t y) //this api only for psram mode
{
    psram_detal_x = x;
    psram_detal_y = y;

    rtl_gui_lcd_update_internal();
}


