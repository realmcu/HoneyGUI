#ifndef _WRISTBAND_GUI_H_
#define _WRISTBAND_GUI_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "stdint.h"
#include "string.h"
#include "stdbool.h"
#include "gui_api.h"
#include "gui_core.h"



#define LCD_WIDTH                           480
#define LCD_HIGHT                           480
#define PAGE_SWITCH_TIMER_INTERVAL          40
#define LCD_SECTION_HEIGHT                  20
#define FEATURE_PSRAM                       0

#define RGB16BIT_565                        16
#define RGB24BIT_888                        24
#define PIXEL_FORMAT                        RGB16BIT_565
#define PIXEL_BYTES                         (PIXEL_FORMAT/8)
#define LCD_SECTION_BYTE_LEN                (LCD_WIDTH * LCD_SECTION_HEIGHT * PIXEL_BYTES)
#define MAX_SECTION_COUNT                   (0x1FFFF/LCD_SECTION_BYTE_LEN)
#define TOTAL_SECTION_COUNT                 (LCD_HIGHT / LCD_SECTION_HEIGHT + ((LCD_HIGHT % LCD_SECTION_HEIGHT)?1:0))
#define FONT_BACK_64M                       1
#if (LCD_WIDTH*LCD_HIGHT*PIXEL_BYTES < 256 * 1024)
#define DIVIDE_INTO_CNT   3
#else
#define DIVIDE_INTO_CNT   3
#endif



#ifdef __cplusplus
}
#endif

#endif /* _WRISTBAND_GUI_H_ */
