#ifndef _GUI_BMP_H_
#define _GUI_BMP_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "stdint.h"
#include "stdbool.h"
#include "wristband_gui.h"

uint8_t rtl_gui_show_bmp_sector_legacy(int xs, int ys, int width, int height,
                                       const unsigned char BMP[],
                                       int Zs,
                                       int Ze, uint8_t *buf, int y_start, int y_end);
uint8_t rtl_gui_show_bmp_sector(UI_WidgetTypeDef *widget, int16_t detal_x, int16_t detal_y, int Zs,
                                int Ze, uint8_t *buf);
uint8_t rtl_gui_show_bmp_simple(UI_BMPTypeDef *widget, int16_t detal_x, int16_t detal_y, int Zs,
                                int Ze, uint8_t *buf);
uint8_t rtl_gui_show_background(UI_WidgetTypeDef *widget, int16_t detal_x, int16_t detal_y, int Zs,
                                int Ze, uint8_t *buf);
#ifdef __cplusplus
}
#endif

#endif /* _GUI_BMP_H_ */
