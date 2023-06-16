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
#ifndef __OTM8009A_480800_MIPI_CMD_H
#define __OTM8009A_480800_MIPI_CMD_H

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

#define  OTM8009A_LCD_WIDTH    (800)
#define  OTM8009A_LCD_HEIGHT   (480)

#define  OTM8009A_LCD_HSYNC    (5U)
#define  OTM8009A_LCD_HBP      (15U)
#define  OTM8009A_LCD_HFP      (16U)
#define  OTM8009A_LCD_VSYNC    (2U)
#define  OTM8009A_LCD_VBP      (34U)
#define  OTM8009A_LCD_VFP      (34U)

/**
 *  @brief LCD_OrientationTypeDef
 *  Possible values of Display Orientation
 */
#define OTM8009A_ORIENTATION_PORTRAIT    ((uint32_t)0x00) /* Portrait orientation choice of LCD screen  */
#define OTM8009A_ORIENTATION_LANDSCAPE   ((uint32_t)0x01) /* Landscape orientation choice of LCD screen */

/**
 *  @brief  Possible values of
 *  pixel data format (ie color coding) transmitted on DSI Data lane in DSI packets
 */
#define OTM8009A_FORMAT_RGB888    ((uint32_t)0x00) /* Pixel format chosen is RGB888 : 24 bpp */
#define OTM8009A_FORMAT_RBG565    ((uint32_t)0x02) /* Pixel format chosen is RGB565 : 16 bpp */



/* List of OTM8009A used commands                                  */
/* Detailed in OTM8009A Data Sheet 'DATA_SHEET_OTM8009A_V0 92.pdf' */
/* Version of 14 June 2012                                         */
#define  OTM8009A_CMD_NOP                   0x00  /* NOP command      */
#define  OTM8009A_CMD_SWRESET               0x01  /* Sw reset command */
#define  OTM8009A_CMD_RDDMADCTL             0x0B  /* Read Display MADCTR command : read memory display access ctrl */
#define  OTM8009A_CMD_RDDCOLMOD             0x0C  /* Read Display pixel format */
#define  OTM8009A_CMD_SLPIN                 0x10  /* Sleep In command */
#define  OTM8009A_CMD_SLPOUT                0x11  /* Sleep Out command */
#define  OTM8009A_CMD_PTLON                 0x12  /* Partial mode On command */

#define  OTM8009A_CMD_DISPOFF               0x28  /* Display Off command */
#define  OTM8009A_CMD_DISPON                0x29  /* Display On command */

#define  OTM8009A_CMD_CASET                 0x2A  /* Column address set command */
#define  OTM8009A_CMD_PASET                 0x2B  /* Page address set command */

#define  OTM8009A_CMD_RAMWR                 0x2C  /* Memory (GRAM) write command */
#define  OTM8009A_CMD_RAMRD                 0x2E  /* Memory (GRAM) read command  */

#define  OTM8009A_CMD_PLTAR                 0x30  /* Partial area command (4 parameters) */

#define  OTM8009A_CMD_TEOFF                 0x34  /* Tearing Effect Line Off command : command with no parameter */

#define  OTM8009A_CMD_TEEON                 0x35  /* Tearing Effect Line On command : command with 1 parameter 'TELOM' */

/* Parameter TELOM : Tearing Effect Line Output Mode : possible values */
#define OTM8009A_TEEON_TELOM_VBLANKING_INFO_ONLY            0x00
#define OTM8009A_TEEON_TELOM_VBLANKING_AND_HBLANKING_INFO   0x01

#define  OTM8009A_CMD_MADCTR                0x36  /* Memory Access write control command  */

/* Possible used values of MADCTR */
#define OTM8009A_MADCTR_MODE_PORTRAIT       0x00
#define OTM8009A_MADCTR_MODE_LANDSCAPE      0x60  /* MY = 0, MX = 1, MV = 1, ML = 0, RGB = 0 */

#define  OTM8009A_CMD_IDMOFF                0x38  /* Idle mode Off command */
#define  OTM8009A_CMD_IDMON                 0x39  /* Idle mode On command  */

#define  OTM8009A_CMD_COLMOD                0x3A  /* Interface Pixel format command */

/* Possible values of COLMOD parameter corresponding to used pixel formats */
#define  OTM8009A_COLMOD_RGB565             0x55
#define  OTM8009A_COLMOD_RGB888             0x77

#define  OTM8009A_CMD_RAMWRC                0x3C  /* Memory write continue command */
#define  OTM8009A_CMD_RAMRDC                0x3E  /* Memory read continue command  */

#define  OTM8009A_CMD_WRTESCN               0x44  /* Write Tearing Effect Scan line command */
#define  OTM8009A_CMD_RDSCNL                0x45  /* Read  Tearing Effect Scan line command */

/* CABC Management : ie : Content Adaptive Back light Control in IC OTM8009a */
#define  OTM8009A_CMD_WRDISBV               0x51  /* Write Display Brightness command          */
#define  OTM8009A_CMD_WRCTRLD               0x53  /* Write CTRL Display command                */
#define  OTM8009A_CMD_WRCABC                0x55  /* Write Content Adaptive Brightness command */
#define  OTM8009A_CMD_WRCABCMB              0x5E  /* Write CABC Minimum Brightness command     */

/**
  * @brief  OTM8009A_480X800 frequency divider
  */
#define OTM8009A_480X800_FREQUENCY_DIVIDER  2   /* LCD Frequency divider      */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/

/** @defgroup OTM8009A_Exported_Macros OTM8009A Exported Macros
  * @{
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/

/** @addtogroup OTM8009A_Exported_Functions
  * @{
  */
void DSI_IO_WriteCmd(uint32_t NbrParams, uint8_t *pParams);
uint8_t OTM8009A_Init(uint32_t ColorCoding, uint32_t orientation);
void lcd_ota8009a_mipi_cmd_set_window(uint16_t xStart, uint16_t yStart, uint16_t xEnd,
                                      uint16_t yEnd);
void lcd_ota8009a_update_framebuffer(uint8_t *buf, uint32_t len);
/**
  * @}
  */
#ifdef __cplusplus
}
#endif

#endif /* __OTM8009A_480800_MIPI_CMD_H */
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
