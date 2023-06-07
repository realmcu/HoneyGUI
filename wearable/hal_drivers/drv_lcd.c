/*
 * Copyright (c) 2022
 *
 *
 *
 *
 *
 *
 */
#include "drv_lcd.h"
#include "drv_dlps.h"
#include "trace.h"
#include "utils.h"
#include "app_section.h"
#include "os_mem.h"



void drv_lcd_power_on(void)
{

}

void drv_lcd_power_off(void)
{

}

void drv_lcd_update(uint8_t *framebuffer, uint16_t xStart, uint16_t yStart, uint16_t w,
                    uint16_t h)
{
    rtk_lcd_hal_set_window(xStart, yStart, w, h);
    rtk_lcd_hal_update_framebuffer(framebuffer, w * h);
}

void drv_lcd_set_window(uint16_t xStart, uint16_t yStart, uint16_t w, uint16_t h)
{
    rtk_lcd_hal_set_window(xStart, yStart, w, h);
}
void drv_lcd_start_transfer(uint8_t *buf, uint32_t len)
{
    rtk_lcd_hal_start_transfer(buf, len);
}
void drv_lcd_transfer_done(void)
{
    rtk_lcd_hal_transfer_done();
}

uint32_t drv_lcd_get_screen_width(void)
{
    return rtk_lcd_hal_get_width();
}
uint32_t drv_lcd_get_screen_height(void)
{
    return rtk_lcd_hal_get_height();
}

uint32_t drv_lcd_get_fb_width(void)
{
    return ((rtk_lcd_hal_get_width() + 15) >> 4) << 4;
}
uint32_t drv_lcd_get_fb_height(void)
{
    return rtk_lcd_hal_get_height();
}

uint32_t drv_lcd_get_pixel_bits(void)
{
    return rtk_lcd_hal_get_pixel_bits();
}


void Display_Handler(void)
{
    DBG_DIRECT("Display_Handler1");
}

static void lcd_enter_dlps(void *drv_io)
{
    drv_lcd_power_off();
}

static void lcd_exit_dlps(void *drv_io)
{
    rtk_lcd_hal_init();
    DBG_DIRECT("Drv lcd init");
}

static bool lcd_allowed_enter_dlps_check(void *drv_io)
{
    return false;
}

static bool lcd_system_wakeup_dlps_check(void *drv_io)
{
    return false;
}


void hw_lcd_init(void)
{
    rtk_lcd_hal_init();
    DBG_DIRECT("Drv lcd init");

    drv_dlps_exit_cbacks_register("lcd", lcd_exit_dlps);
    DBG_DIRECT("Drv lcd Register Exit DLPS CB");
}

/************** end of file ********************/
