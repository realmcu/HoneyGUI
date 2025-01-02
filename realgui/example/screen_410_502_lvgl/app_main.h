#ifndef _APP_MAIN_H
#define _APP_MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"
#include "ui_func.h"
#include "lv_img_dsc_list.h"
#include "lv_image_dsc.h"

extern lv_obj_t *scr_watchface;
extern lv_obj_t *scr_up_curtain;
extern lv_obj_t *scr_down_curtain;
extern lv_obj_t *scr_left_curtain;
extern lv_obj_t *scr_right_curtain;



void lv_watchface_init(void);
void lv_up_curtain_init(void);
void lv_down_curtain_init(void);
void lv_left_curtain_init(void);
void lv_right_curtain_init(void);


// UI INIT


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif