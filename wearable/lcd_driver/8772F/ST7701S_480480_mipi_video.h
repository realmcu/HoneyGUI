/**
*********************************************************************************************************
*               Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file
* @brief
* @details
* @author   howie wang
* @date     2022-05-17
* @version  v0.1
*********************************************************************************************************
*/
#ifndef __ST7701S_480480_MIPI_VIDEO_H
#define __ST7701S_480480_MIPI_VIDEO_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>



#if defined ( __GNUC__ ) || (defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)) /* GNU and ARM Compiler 6 compilers */
#ifndef __weak
#define __weak __attribute__((weak))
#endif /* __weak */
#endif /* __GNUC__ || (__ARMCC_VERSION && (__ARMCC_VERSION >= 6010050)) */

#define  ST7701S_LCD_WIDTH    (480U)
#define  ST7701S_LCD_HEIGHT   (480U)
#define  ST7701S_LCD_HSYNC    (8U)
#define  ST7701S_LCD_HBP      (50U)
#define  ST7701S_LCD_HFP      (10U)
#define  ST7701S_LCD_VSYNC    (4U)
#define  ST7701S_LCD_VBP      (20U)
#define  ST7701S_LCD_VFP      (10U)




#define INPUT_PIXEL_BYTES                   4
#define OUTPUT_PIXEL_BYTES                  3


uint8_t ST7701S_Init();
void lcd_st7701s_mipi_video_init_framebuffer(uint8_t *buf, uint32_t len);
void lcd_st7701s_mipi_video_update_framebuffer(uint8_t *buf, uint32_t len);



#ifdef __cplusplus
}
#endif

#endif /* __ST7701S_480480_MIPI_VIDEO_H */


