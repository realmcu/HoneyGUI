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
#ifndef __SH8601A_454454_MIPI_VIDEO_H
#define __SH8601A_454454_MIPI_VIDEO_H

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

#define  SH8601A_LCD_WIDTH    (454U)
#define  SH8601A_LCD_HEIGHT   (454U)
//#define  SH8601A_LCD_WIDTH    (480)
//#define  SH8601A_LCD_HEIGHT   (480)
#define  SH8601A_LCD_HSYNC    (20U)
#define  SH8601A_LCD_HBP      (10U)
#define  SH8601A_LCD_HFP      (10U)
#define  SH8601A_LCD_VSYNC    (4U)
#define  SH8601A_LCD_VBP      (8U)
#define  SH8601A_LCD_VFP      (8U)


///*size from command mode*/
//#define  SH8601A_LCD_WIDTH    (454U)
//#define  SH8601A_LCD_HEIGHT   (454U)

//#define  SH8601A_LCD_HSYNC    (5U)
//#define  SH8601A_LCD_HBP      (15U)
//#define  SH8601A_LCD_HFP      (16U)

//#define  SH8601A_LCD_VSYNC    (2U)
//#define  SH8601A_LCD_VBP      (34U)
//#define  SH8601A_LCD_VFP      (34U)


#define INPUT_PIXEL_BYTES                   4
#define OUTPUT_PIXEL_BYTES                  3


uint8_t SH8601A_Init();
void lcd_sh8601a_mipi_video_init_framebuffer(uint8_t *buf, uint32_t len);
void lcd_sh8601a_mipi_video_update_framebuffer(uint8_t *buf, uint32_t len);



#ifdef __cplusplus
}
#endif

#endif /* __SH8601A_454454_MIPI_VIDEO_H */
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
