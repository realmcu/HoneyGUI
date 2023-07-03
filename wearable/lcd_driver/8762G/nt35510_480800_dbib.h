#ifndef _LCD_RM69330_454_QSPI_H_
#define _LCD_RM69330_454_QSPI_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "stdint.h"


#define NT35510_LCD_WIDTH                           800
#define NT35510_LCD_HIGHT                           480

#define NT35510_DRV_PIXEL_BITS                      32
#define NT35510_DRV_PIXEL_BYTES                     4

void rtk_lcd_hal_set_window(uint16_t xStart, uint16_t yStart, uint16_t w, uint16_t h);
void rtk_lcd_hal_init(void);
void rtk_lcd_init(void);
void rtk_lcd_clear(void);
void rtk_lcd_hal_update_framebuffer(uint8_t *buf, uint32_t len);
void rtk_lcd_hal_rect_fill(uint16_t xStart, uint16_t yStart, uint16_t w, uint16_t h,
                           uint32_t color);
void rtk_lcd_hal_start_transfer(uint8_t *buf, uint32_t len);
void rtk_lcd_hal_transfer_done(void);

uint32_t rtk_lcd_hal_get_width(void);
uint32_t rtk_lcd_hal_get_height(void);
uint32_t rtk_lcd_hal_get_pixel_bits(void);

void rtl_lcd_hal_power_on(void);
void rtl_lcd_hal_power_off(void);

#ifdef __cplusplus
}
#endif

#endif /* _LCD_RM69330_454_QSPI_H_ */
