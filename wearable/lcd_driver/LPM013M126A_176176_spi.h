#ifndef _LPM013M126A_176176_SPI_H_
#define _LPM013M126A_176176_SPI_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "stdint.h"


#define LPM013M126A_LCD_WIDTH                  176
#define LPM013M126A_LCD_HIGHT                  176
#define INPUT_PIXEL_BYTES                   2
#define OUTPUT_PIXEL_BYTES                  1
#define LPM013M126A_DRV_PIXEL_BITS             3








void rtk_lcd_hal_set_window(uint16_t xStart, uint16_t yStart, uint16_t w, uint16_t h);
void rtk_lcd_hal_init(void);
void rtk_lcd_hal_update_framebuffer(uint8_t *buf, uint32_t len);
void rtk_lcd_hal_rect_fill(uint16_t xStart, uint16_t yStart, uint16_t w, uint16_t h,
                           uint32_t color);
void rtk_lcd_hal_start_transfer(uint8_t *buf, uint32_t len);
void rtk_lcd_hal_transfer_done(void);
#ifdef __cplusplus
}
#endif

#endif /* _ICNA3311_280X456_QSPI_H_ */
