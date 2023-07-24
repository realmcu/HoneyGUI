#ifndef _ICNA3311_280X456_QSPI_H_
#define _ICNA3311_280X456_QSPI_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "stdint.h"


#define DMA_LINKLIST                        0
#define TE_VALID                            1
#define ICNA3311_LCD_WIDTH                  280
#define ICNA3311_LCD_HIGHT                  456
#define INPUT_PIXEL_BYTES                   2
#define OUTPUT_PIXEL_BYTES                  2
#define ICNA3311_DRV_PIXEL_BITS             16

#if 1 //1 QFN68 -  0 QFN88
#define LCD_QSPI_RST                     P3_6
// #define LCD_QSPI_BL                      P1_4
#define LCD_QSPI_D0                      P4_2
#define LCD_QSPI_D1                      P4_1
#define LCD_QSPI_D2                      P3_2
#define LCD_QSPI_D3                      P3_3
#define LCD_QSPI_CS                      P4_3
#define LCD_QSPI_CLK                     P4_0
#define LCD_QSPI_TE                      P0_5

//#define LCD_QSPI_IM0                      P0_1
//#define LCD_QSPI_IM1                      P0_1
#else
#define LCD_QSPI_RST                     P3_6
//#define LCD_QSPI_BL                      P1_2
#define LCD_QSPI_D0                      P4_2
#define LCD_QSPI_D1                      P4_1
#define LCD_QSPI_D2                      P0_6
#define LCD_QSPI_D3                      P0_7
#define LCD_QSPI_CS                      P4_3
#define LCD_QSPI_CLK                     P4_0
#define LCD_QSPI_TE                      P0_1
//#define LCD_QSPI_IM0                      P0_1
//#define LCD_QSPI_IM1                      P0_1

#endif




void rtk_lcd_hal_set_window(uint16_t xStart, uint16_t yStart, uint16_t w, uint16_t h);
void rtk_lcd_hal_init(void);
void rtk_lcd_hal_update_framebuffer(uint8_t *buf, uint32_t len);
void rtk_lcd_hal_rect_fill(uint16_t xStart, uint16_t yStart, uint16_t w, uint16_t h,
                           uint32_t color);
uint32_t rtk_lcd_hal_get_width(void);
uint32_t rtk_lcd_hal_get_height(void);
uint32_t rtk_lcd_hal_get_pixel_bits(void);
uint32_t rtk_lcd_hal_power_off(void);
uint32_t rtk_lcd_hal_power_on(void);
uint32_t rtk_lcd_hal_dlps_restore(void);
#ifdef __cplusplus
}
#endif

#endif /* _ICNA3311_280X456_QSPI_H_ */
