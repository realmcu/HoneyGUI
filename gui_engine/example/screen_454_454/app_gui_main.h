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
#define GUI_FRAME               30


gui_app_t *get_app_watch_ui(void);
extern void design_watchface_mgr(void *parent);
extern void design_player_mgr(void *parent);
extern void design_call_mgr(void *parent);
extern void design_contacts_mgr(void *parent);
extern void design_record_mgr(void *parent);
extern void design_benchmark_mgr(void *parent);

extern gui_tabview_t *tabview_main;

#ifdef __cplusplus
}
#endif
#endif


