/**
 * @file app_music.h
 *
 */

#ifndef APP_MUSIC_H
#define APP_MUSIC_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
// #include "../apps.h"
#include "lvgl.h"
#include "app_main.h"


/*********************
 *      DEFINES
 *********************/
#define APP_MUSIC_SQUARE    0
#define APP_MUSIC_LANDSCAPE 0
#define APP_MUSIC_ROUND     0
#define APP_MUSIC_LARGE     0
#define APP_MUSIC_AUTO_PLAY 0
#define APP_MUSIC_HANDLE_SIZE  60
#else
#define APP_MUSIC_HANDLE_SIZE  20
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void app_music(lv_obj_t *parent);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

