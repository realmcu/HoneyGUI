#ifndef _APP_MAIN_H
#define _APP_MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"
#include "ui_func.h"
#include "lv_img_dsc_list.h"
#include "lv_image_dsc.h"

#define M_PI    ((float)3.14159265358979323846)

#define LVGL_USE_CJSON 0

#define WATCH_DEMO_USE_TILESLIDE    0

#if WATCH_DEMO_USE_TILESLIDE
#define WATCH_DEMO_USE_SNAPSHOT     1
#if WATCH_DEMO_USE_SNAPSHOT
#if !LV_USE_DRAW_PPE_RTL8773E && \
            !LV_USE_DRAW_PPE_RTL872xG && \
            LV_MEM_SIZE < 3 * 1024 * 1024
#warning "It's recommended to have at least 3MB RAM for the snapshot tileview watch demo on SW"
#endif
#else
#if !LV_USE_DRAW_PPE_RTL8773E && \
            !LV_USE_DRAW_PPE_RTL872xG && \
            LV_MEM_SIZE < 1024 * 1024
#warning "It's recommended to have at least 1MB RAM for the tileview watch demo on SW"
#endif
#endif
#endif

#ifndef WATCH_DEMO_USE_SNAPSHOT
#define WATCH_DEMO_USE_SNAPSHOT     0
#endif

uint32_t event_snapshot_creat;
uint32_t event_snapshot_delete;

extern lv_obj_t *tileview;
extern lv_obj_t *scr_watchface;
extern lv_obj_t *scr_up_curtain;
extern lv_obj_t *scr_down_curtain;
extern lv_obj_t *scr_left_curtain;
extern lv_obj_t *scr_right_curtain;
extern lv_obj_t *scr_app_menu;



void lv_watchface_init(void);
void lv_up_curtain_init(void);
void lv_down_curtain_init(void);
void lv_left_curtain_init(void);
void lv_right_curtain_init(void);
void lv_app_menu_init(void);


// FONTS
LV_FONT_DECLARE(SourceHanSansSC_size12_bits1_font);
LV_FONT_DECLARE(SourceHanSansSC_size24_bits1_font);
LV_FONT_DECLARE(SourceHanSansSC_size32_bits1_font);
LV_FONT_DECLARE(SourceHanSansSC_size48_bits1_font);


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif