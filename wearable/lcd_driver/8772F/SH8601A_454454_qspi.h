#ifndef _LCD_SH8601A_454_QSPI_H_
#define _LCD_SH8601A_454_QSPI_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "stdint.h"

#define SH8601A_MAX_PARA_COUNT                          (300)

#define DMA_LINKLIST                        1
#define TE_VALID                            1
#define SH8601A_LCD_WIDTH                   454
#define SH8601A_LCD_HIGHT                   454
#define INPUT_PIXEL_BYTES                   4
#define OUTPUT_PIXEL_BYTES                  3
#define RED                                 0xf800



typedef struct _SH8601A_CMD_DESC
{
    uint8_t instruction;
    uint8_t index;
    uint16_t delay;
    uint16_t wordcount;
    uint8_t  payload[SH8601A_MAX_PARA_COUNT];
} SH8601A_CMD_DESC;


void rtk_lcd_hal_set_window(uint16_t xStart, uint16_t yStart, uint16_t w, uint16_t h);
void rtk_lcd_hal_init(void);
void rtk_lcd_hal_update_framebuffer(uint8_t *buf, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif /* _LCD_SH8601A_454_QSPI_H_ */
