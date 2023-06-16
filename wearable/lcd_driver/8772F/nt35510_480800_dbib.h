#ifndef _LCD_RM69330_454_QSPI_H_
#define _LCD_RM69330_454_QSPI_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "stdint.h"


#define NT35510_LCD_WIDTH                           800
#define NT35510_LCD_HIGHT                           480

#define NT35510_DRV_PIXEL_BITS                      32

void nt35510_init(void);
void lcd_nt35510_power_on(void);
void lcd_nt35510_power_off(void);
void lcd_nt35510_set_window(uint16_t xStart, uint16_t yStart, uint16_t xEnd,
                            uint16_t yEnd);
void lcd_nt35510_update_framebuffer(uint8_t *buf, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif /* _LCD_RM69330_454_QSPI_H_ */
