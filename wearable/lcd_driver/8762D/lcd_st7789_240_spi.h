#ifndef _LCD_ST7789_240_SPI_H_
#define _LCD_ST7789_240_SPI_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "stdint.h"

void lcd_st7789_power_on(void);
void lcd_st7789_power_off(void);
void lcd_st7789_set_window(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd);
void lcd_st7789_init(void);
void lcd_st7789_frame_rate(uint8_t RTNA, uint8_t FPA, uint8_t BPA);

#ifdef __cplusplus
}
#endif

#endif /* _LCD_ST7789_240_SPI_H_ */
