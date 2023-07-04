#ifndef _LCD_NV3047_480272_RGB_
#define _LCD_NV3047_480272_RGB_

#ifdef __cplusplus
extern "C" {
#endif
#include "stdint.h"


#define NV3047_480272_LCD_WIDTH                   480
#define NV3047_480272_LCD_HEIGHT                  272


#define NV3047_DRV_PIXEL_BITS                  16



void rtk_lcd_hal_init(void);
void rtk_lcd_hal_update_framebuffer(uint8_t *p_buf, uint32_t size);
void rtk_lcd_hal_set_window(uint16_t xStart, uint16_t yStart, uint16_t w, uint16_t h);
void rtk_lcd_hal_start_transfer(uint8_t *buf, uint32_t len);
void rtk_lcd_hal_transfer_done(void);
uint32_t rtk_lcd_hal_power_off(void);
uint32_t rtk_lcd_hal_power_on(void);
uint32_t rtk_lcd_hal_dlps_restore(void);



#ifdef __cplusplus
}
#endif

#endif /* _LCD_NV3047_480272_RGB_ */
