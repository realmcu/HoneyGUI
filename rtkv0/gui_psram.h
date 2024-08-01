#ifndef _GUI_PSRAM_H_
#define _GUI_PSRAM_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "stdint.h"
#include "stdbool.h"
#include "gui_core.h"




extern UI_PSRAMBufferTypeDef FrameBufferOrigin;
extern UI_PSRAMBufferTypeDef FrameBufferLeft;
extern UI_PSRAMBufferTypeDef FrameBufferRight;
extern UI_PSRAMBufferTypeDef FrameBufferDown;
extern UI_PSRAMBufferTypeDef FrameBufferUp;

extern int16_t psram_detal_x;
extern int16_t psram_detal_y;


void rtl_gui_lcd_update_from_psram(void);
void rtl_gui_copy_psram_to_lcd_x(UI_PSRAMBufferTypeDef left, UI_PSRAMBufferTypeDef right,
                                 uint32_t offset);
void rtl_gui_copy_psram_to_lcd_y(UI_PSRAMBufferTypeDef up, UI_PSRAMBufferTypeDef down,
                                 uint32_t offset);
void rtl_gui_copy_ram_to_psram(uint8_t *readbuf, uint32_t count, UI_PSRAMBufferTypeDef *location,
                               uint32_t length);
void psram_frame_buffer_init(void);
void rtl_gui_update_to_psram_framebuffer(UI_MenuTypeDef *p_show_menu,
                                         UI_PSRAMBufferTypeDef *location);

#ifdef __cplusplus
}
#endif

#endif /* _GUI_PSRAM_H_ */
