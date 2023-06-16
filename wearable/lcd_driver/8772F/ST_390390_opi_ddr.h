#ifndef _ST_390390_OPI_DDR_H_
#define _ST_390390_OPI_DDR_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "stdint.h"


#define DMA_LINKLIST                        1
#define TE_VALID                            0
#define STOPI_LCD_WIDTH                   390
#define STOPI_LCD_HIGHT                   390
#define INPUT_PIXEL_BYTES                   4
#define OUTPUT_PIXEL_BYTES                  3

static void spic3_spi_write(uint8_t *buf, uint32_t len);
void lcd_st_opi_update_framebuffer(uint8_t *buf, uint32_t len);
void lcd_st_opi_390_set_window(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd);
void lcd_st_opi_390_init(void);






#ifdef __cplusplus
}
#endif

#endif /* _ST_390390_OPI_DDR_H_ */
