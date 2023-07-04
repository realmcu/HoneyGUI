/**
*********************************************************************************************************
*               Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file
* @brief
* @details
* @author   howie wang
* @date     2022-02-16
* @version  v0.1
*********************************************************************************************************
*/
#ifndef __SH8601A_454454_MIPI_CMD_H
#define __SH8601A_454454_MIPI_CMD_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
/** @addtogroup BSP
  * @{
  */

/** @addtogroup Components
  * @{
  */

/** @addtogroup otm8009a
  * @{
  */

/** @addtogroup OTM8009A_Exported_Variables
  * @{
  */

#if defined ( __GNUC__ ) || (defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)) /* GNU and ARM Compiler 6 compilers */
#ifndef __weak
#define __weak __attribute__((weak))
#endif /* __weak */
#endif /* __GNUC__ || (__ARMCC_VERSION && (__ARMCC_VERSION >= 6010050)) */

#define  SH8601A_LCD_WIDTH    (454)
#define  SH8601A_LCD_HEIGHT   (454)
#define  SH8601A_LCD_HSYNC    (5)
#define  SH8601A_LCD_HBP      (15)
#define  SH8601A_LCD_HFP      (16)
#define  SH8601A_LCD_VSYNC    (2)
#define  SH8601A_LCD_VBP      (34)
#define  SH8601A_LCD_VFP      (34)


#define SH8601A_LCD_BITS_PIXEL              32
#define INPUT_PIXEL_BYTES                   4
#define OUTPUT_PIXEL_BYTES                  3




void rtk_lcd_hal_init(void);
void rtk_lcd_hal_set_window(uint16_t xStart, uint16_t yStart, uint16_t w, uint16_t h);
void rtk_lcd_hal_update_framebuffer(uint8_t *buf, uint32_t len);
uint32_t rtk_lcd_hal_get_width(void);
uint32_t rtk_lcd_hal_get_height(void);
uint32_t rtk_lcd_hal_get_pixel_bits(void);
uint32_t rtk_lcd_hal_power_on(void);
uint32_t rtk_lcd_hal_power_off(void);
uint32_t rtk_lcd_hal_dlps_restore(void);

//for ramless
void rtk_lcd_hal_start_transfer(uint8_t *buf, uint32_t len);
void rtk_lcd_hal_transfer_done(void);


#ifdef __cplusplus
}
#endif

#endif /* __SH8601A_454454_MIPI_CMD_H */
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
