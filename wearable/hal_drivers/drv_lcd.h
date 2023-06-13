/*
 * Copyright (c) 2022
 *
 *
 *
 *
 *
 *
 */

#ifndef __DRV_LCD_H__
#define __DRV_LCD_H__

#include "rtl_hal_peripheral.h"
#include "drv_dlps.h"


void rtk_lcd_hal_init(void);
void rtk_lcd_hal_set_window(uint16_t xStart, uint16_t yStart, uint16_t w, uint16_t h);

void rtk_lcd_hal_update_framebuffer(uint8_t *buf, uint32_t len);
void rtk_lcd_hal_rect_fill(uint16_t xStart, uint16_t yStart, uint16_t w, uint16_t h,
                           uint32_t color);
void rtk_lcd_hal_start_transfer(uint8_t *buf, uint32_t len);
void rtk_lcd_hal_transfer_done(void);
uint32_t rtk_lcd_hal_get_width(void);
uint32_t rtk_lcd_hal_get_height(void);
uint32_t rtk_lcd_hal_get_pixel_bits(void);




void drv_lcd_update(uint8_t *framebuffer, uint16_t xStart, uint16_t yStart, uint16_t w, uint16_t h);
void drv_lcd_set_window(uint16_t xStart, uint16_t yStart, uint16_t w, uint16_t h);
void drv_lcd_start_transfer(uint8_t *buf, uint32_t len);
void drv_lcd_transfer_done(void);

uint32_t drv_lcd_get_screen_width(void);
uint32_t drv_lcd_get_screen_height(void);
uint32_t drv_lcd_get_fb_width(void);
uint32_t drv_lcd_get_fb_height(void);
uint32_t drv_lcd_get_pixel_bits(void);

#endif  /* __DRV_LCD_H__ */

