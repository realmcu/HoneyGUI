#ifndef _LCD_ST99703_400_RLSPI_H_
#define _LCD_ST99703_400_RLSPI_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "stdint.h"

#define     DMA_MULTIBLOCK          1
#define     INFINITE_MODE           1

#define PIXEL_BYTES                 4

#define ST77903_WIDTH                   (400U)
#define ST77903_HEIGHT                  (400U)

#define ST77903_VSYNC                   (1U)// for now
#define ST77903_VBP                     (8U)
#define ST77903_VFP                     (8U)

#define ST77903_VSYNC_CMD               0xD8
#define ST77903_VBP_CMD                 0xD8
#define ST77903_VACTIVE_CMD_QSPI        0xDE
#define ST77903_VACTIVE_CMD_DSPI        0xDB
#define ST77903_VACTIVE_CMD_SPI         0xDE
#define ST77903_VFP_CMD                 0xD8
#define ST77903_DUMMY_CMD               0x00

#define ST77903_VSYNC_ADDR              0x006100
#define ST77903_VBP_ADDR                0x006000
#define ST77903_VACTIVE_ADDR            0x006000
#define ST77903_VFP_ADDR                0x006000

#define LCDC_DMA_CHANNEL_NUM              0
#define LCDC_DMA_CHANNEL_INDEX            LCDC_DMA_Channel0

void st77903_screen_init(void);
void init_cmds(void);
void print_SPIC_reg(void);
void st77903_framebuffer_init(uint8_t *buf, uint32_t len);
void st77903_update_framebuffer(uint8_t *buf, uint32_t line_bytes);

#endif
