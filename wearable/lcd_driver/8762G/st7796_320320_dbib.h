#ifndef _LCD_RM69330_454_QSPI_H_
#define _LCD_RM69330_454_QSPI_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "stdint.h"


#define ST7796_LCD_WIDTH                           320
#define ST7796_LCD_HIGHT                           320

#define ST7796_DRV_PIXEL_BITS                      16

#define LCD_8080_RST                     P2_0
#define LCD_8080_BL                      P1_2 //need R15
/*because most 8080 lcd pins ara fixed*/
#define LCD_8080_D0                      P0_4
#define LCD_8080_D1                      P0_5
#define LCD_8080_D2                      P0_6
#define LCD_8080_D3                      P0_7
#define LCD_8080_D4                      P4_0
#define LCD_8080_D5                      P4_1
#define LCD_8080_D6                      P4_2
#define LCD_8080_D7                      P4_3

#define LCD_8080_CS                      P0_0
#define LCD_8080_DCX                     P1_5
#define LCD_8080_RD                      P1_6
#define LCD_8080_WR                      P0_2
#define LCD_TE_SYNC                      P0_1


void rtk_lcd_hal_set_window(uint16_t xStart, uint16_t yStart, uint16_t w, uint16_t h);
void rtk_lcd_hal_init(void);
void rtk_lcd_hal_update_framebuffer(uint8_t *buf, uint32_t len);


#ifdef __cplusplus
}
#endif

#endif /* _LCD_RM69330_454_QSPI_H_ */
