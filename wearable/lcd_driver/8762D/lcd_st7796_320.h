#ifndef _LCD_ST7796_320_H_
#define _LCD_ST7796_320_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "stdint.h"


#define ST7796_LCD_WIDTH                  320
#define ST7796_LCD_HIGHT                  320
#define INPUT_PIXEL_BYTES                   2
#define OUTPUT_PIXEL_BYTES                  2
#define ST7796_DRV_PIXEL_BITS               16


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
uint32_t rtk_lcd_hal_power_on(void);
uint32_t rtk_lcd_hal_power_off(void);

#ifdef __cplusplus
}
#endif

#endif /* _LCD_ST7796_320_H_ */
