#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include "def_color.h"
#include "gui_img.h"
#include "root_image/ui_resource.h"

/*============================================================================*
 *                           Types
 *============================================================================*/

/*============================================================================*
 *                            Macros
 *============================================================================*/
// Color
#define FG_THEME1_DARK                   gui_rgb(0xFF,0xB4,0x11)
#define FG_THEME2_DARK                   gui_rgb(0x1F,0xE5,0x7F)
#define FG_THEME3_DARK                   gui_rgb(0x11,0xC8,0xFF)
#define FG_THEME1_LIGHT                  gui_rgb(0xDC,0x82,0x14)
#define FG_THEME2_LIGHT                  gui_rgb(0x14,0x9A,0x43)
#define FG_THEME3_LIGHT                  gui_rgb(0x00,0x81,0xEA)
#define FG_WARNING                       gui_rgb(0xDF,0x2D,0x00)
#define FG_NORMAL                        gui_rgb(0x00,0xB4,0x36)
#define FG_1_DARK                        gui_rgb(0xFF,0xFF,0xFF)
#define FG_2_DARK                        gui_rgb(0xFF*0.3f,0xFF*0.3f,0xFF*0.3f)
#define FG_3_DARK                        gui_rgb(0xFF*0.5f,0xFF*0.5f,0xFF*0.5f)
#define FG_1_LIGHT                       gui_rgb(0x32,0x32,0x32)
#define FG_2_LIGHT                       gui_rgb(0x32*0.3f,0x32*0.3f,0x32*0.3f)
#define FG_3_LIGHT                       gui_rgb(0x32*0.5f,0x32*0.5f,0x32*0.5f)
#define FG_WHITE                         FG_1_DARK
#define FG_WARNING                       gui_rgb(0xDF,0x2D,0x00)

#define BG_THEME1_BRIGHT_DARK            gui_rgb(0xF6,0x87,0x1F)
#define BG_THEME1_MID_DARK               gui_rgb(0x5A,0x30,0x1E)
#define BG_THEME1_DARK_DARK              gui_rgb(0x2C,0x17,0x00)
#define BG_THEME2_BRIGHT_DARK            gui_rgb(0x00,0xA1,0x4E)
#define BG_THEME2_MID_DARK               gui_rgb(0x05,0x44,0x2F)
#define BG_THEME2_DARK_DARK              gui_rgb(0x00,0x28,0x21)
#define BG_THEME3_BRIGHT_DARK            gui_rgb(0x00,0x85,0xFF)
#define BG_THEME3_MID_DARK               gui_rgb(0x1A,0x1D,0x6E)
#define BG_THEME3_DARK_DARK              gui_rgb(0x0F,0x11,0x3D)

#define BG_THEME1_BRIGHT_LIGHT           gui_rgb(0xFF,0x94,0x20)
#define BG_THEME1_MID_LIGHT              gui_rgb(0xEF,0xBD,0x8A)
#define BG_THEME1_DARK_LIGHT             gui_rgb(0xF5,0xE4,0xD3)
#define BG_THEME2_BRIGHT_LIGHT           gui_rgb(0x14,0xCD,0x6E)
#define BG_THEME2_MID_LIGHT              gui_rgb(0xA8,0xE5,0xC3)
#define BG_THEME2_DARK_LIGHT             gui_rgb(0xD1,0xEB,0xDF)
#define BG_THEME3_BRIGHT_LIGHT           gui_rgb(0x41,0xAA,0xFF)
#define BG_THEME3_MID_LIGHT              gui_rgb(0xB1,0xD9,0xFF)
#define BG_THEME3_DARK_LIGHT             gui_rgb(0xD3,0xE8,0xFF)

#define BG_1_DARK                        gui_rgb(0xFF*0.1f,0xFF*0.1f,0xFF*0.1f)
#define BG_2_DARK                        gui_rgb(0xFF*0.1f,0xFF*0.1f,0xFF*0.1f)
#define BG_1_LIGHT                       gui_rgb(0,0,0)
#define BG_2_LIGHT                       gui_rgb(0,0,0)
#define SCREEN_BG_LIGHT                  gui_rgb(0xF2,0xF2,0xF7)

/*============================================================================*
 *                            Variables
 *============================================================================*/
/* Time */
extern struct tm *timeinfo;

/* Date */
extern const char *month[12];
extern const char *day[7];

/* Clock style */
extern uint8_t clock_style;

/* Theme color */
extern gui_color_t theme_color;

/* Theme bg */
extern bool theme_bg_white; // default black bg

/* Volume */
extern int8_t volume_val;

/* Battery */
extern uint8_t battery_barn_val;
extern uint8_t battery_earphone_l_val;
extern uint8_t battery_earphone_r_val;

/* Message */
extern uint8_t message_num_val;

/* Function status */
extern bool status_asc;
extern bool status_flashlight;
extern bool status_spatial_sound;

/*============================================================================*
 *                           Punblic Functions
 *============================================================================*/


#ifdef __cplusplus
}
#endif
#endif

