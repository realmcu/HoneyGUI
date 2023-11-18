#ifndef __APP_GUI_MAIN_H__
#define __APP_GUI_MAIN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <gui_app.h>
#include "gui_tabview.h"
#include "root_image/ui_resource.h"

#define LCD_W                   454
#define LCD_H                   454
#define FONT_CHINESE_BYTE       3
#define FONT_CHINESE_W          32
#define FONT_NUM_ALPHA_W        16
#define FONT_H_32               32
#define FONT_BIN                SIMKAI_SIZE32_BITS1_FONT_BIN


void *get_app_watch_ui(void);
extern void design_tab_watchface(void *parent);
extern void design_tab_player(void *parent);
extern void design_tab_call(void *parent);
extern void design_tab_contacts(void *parent);
extern void design_tab_record(void *parent);

extern gui_tabview_t *tabview_main;

#ifdef __cplusplus
}
#endif
#endif


