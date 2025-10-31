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
struct FUNCTION_STATUS
{
    bool equalizer              ;
    bool ambient_sound          ;
    bool smart_talk             ;
    bool spatial_sound          ;
    bool voice_aware            ;
    bool auto_play_pause        ;
    bool notification           ;
    bool message_preview        ;
    bool auto_dim_off_screen    ;
    bool quick_wake_up_screen   ;
    bool unlock_slider          ;

    uint32_t bt                 : 2;
    uint32_t ble                : 1;

    uint32_t factory_reset_done : 1;

    // uint32_t equalizer          : 1;
    // uint32_t ambient_sound      : 1;
    // uint32_t smart_talk         : 1;
    // uint32_t spatial_sound      : 1;
    // uint32_t voice_aware        : 1;
    // uint32_t auto_play_pause    : 1;
    // uint32_t notification       : 1;
    // uint32_t message_preview    : 1;
    uint32_t playback           : 1;
    uint32_t flashlight         : 1;
    uint32_t music_input        : 1;
    uint32_t notification_new   : 1;
    uint32_t call               : 1;
    uint32_t timer              : 1;

    uint32_t earbuds_connect_l          : 1;
    uint32_t earbuds_connect_r          : 1;
    uint32_t earbuds_in_ear_l           : 1;
    uint32_t earbuds_in_ear_r           : 1;
    uint32_t earbuds_find_l             : 1;
    uint32_t earbuds_find_r             : 1;
    uint32_t earbuds_shock_l            : 1;
    uint32_t earbuds_shock_r            : 1;

    uint32_t infor_center_func_0        : 1;
    uint32_t infor_center_func_1        : 1;
    uint32_t infor_center_func_2        : 1;
    uint32_t infor_center_func_3        : 1;
    uint32_t infor_center_func_4        : 1;
    uint32_t infor_center_func_5        : 1;


};

typedef enum
{
    PLAYBACK = 0,
    VOLUME,
    AMBIENT_SOUND,
    EQUALIZER,
    SMART_TALK,
    SPATIAL_SOUND,
    VOICE_AWARE,
    AUTO_PLAY_PAUSE,
    BT_SOURCE,

    FIND_MY_BUDS,
    TIMER,
    FLASHLIGHT,
    JBL_HEADPHONES_APP,

    SCREEN_BRIGHTNESS,
    DARK_LIGHT_MODE,
    LOCK_SCREEN,
    AUTO_DIM_OFF_SCREEN,
    QUICK_WAKE_UP_SCREEN,
    CASE_BUTTON_CUSTOMIZE,
    INFORMATION_CENTER_CUSTOMIZE,
    UNLOCK_SLIDER,
    NOTIFICATION,
    TIME_FORMAT,
    LANGUAGE,
    REORDER_QUICK_ACCESS,
    SUPPORT,
    PRODUCT_TIPS,
    FACTORY_RESET
} FUNCTION_TYPE;
typedef enum
{
    ANC = 0,
    AA,
    TT
} AMBIENT_SOUND_TYPE;

typedef enum
{
    SPATIAL_SOUND_MOVIE = 0,
    SPATIAL_SOUND_MUSIC,
    SPATIAL_SOUND_GAME
} SPATIAL_SOUND_TYPE;

typedef enum
{
    BT_DISCONNECT = 0,
    BT_CONNECT,
    BT_UNKNOWN
} BT_STATUS;

typedef enum
{
    SBC = 0,
    AAC,
    LC3,
    LC3_PLUS,
    LDAC,
    PCM
} CODEC_TYPE;

typedef enum
{
    ENGLISH = 0,
    FRENCH,
    GERMAN,
    SPANISH,
    ITALIAN,
    DUTCH,
    PORTUGUESE,
    RUSSIAN,
    CHINESE,
    JAPANESE,
    KOREAN,
    THAI,
    VIETNAM
} LANGUAGE_TYPE;

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
#define FG_2_DARK                        gui_rgba(0xFF,0xFF,0xFF,0xFF*0.3f)
#define FG_3_DARK                        gui_rgba(0xFF,0xFF,0xFF,0xFF*0.5f)
#define FG_1_LIGHT                       gui_rgb(0x32,0x32,0x32)
#define FG_2_LIGHT                       gui_rgba(0x32,0x32,0x32,0xFF*0.3f)
#define FG_3_LIGHT                       gui_rgba(0x32,0x32,0x32,0xFF*0.5f)
#define FG_WHITE                         FG_1_DARK
#define FG_DARK                          gui_rgb(0,0,0)
#define FG_WARNING                       gui_rgb(0xDF,0x2D,0x00)
#define FG_NORMAL                        gui_rgb(0x00,0xB4,0x36)

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

#define BG_1_DARK                        gui_rgba(0xFF,0xFF,0xFF,0xFF*0.1f)
#define BG_2_DARK                        gui_rgba(0xFF,0xFF,0xFF,0xFF*0.2f)
#define BG_1_LIGHT                       gui_rgba(0,0,0,0xFF*0.05f)
#define BG_2_LIGHT                       gui_rgba(0,0,0,0xFF*0.2f)
#define SCREEN_BG_LIGHT                  gui_rgb(0xF2,0xF2,0xF7)
#define SCREEN_BG_DARK                   gui_rgb(0,0,0)

// #define FG_THEME1_DARK                   GUI_COLOR_ARGB8888(255, 0xFF,0xB4,0x11)
// #define FG_THEME2_DARK                   GUI_COLOR_ARGB8888(255, 0x1F,0xE5,0x7F)
// #define FG_THEME3_DARK                   GUI_COLOR_ARGB8888(255, 0x11,0xC8,0xFF)
// #define FG_THEME1_LIGHT                  GUI_COLOR_ARGB8888(255, 0xDC,0x82,0x14)
// #define FG_THEME2_LIGHT                  GUI_COLOR_ARGB8888(255, 0x14,0x9A,0x43)
// #define FG_THEME3_LIGHT                  GUI_COLOR_ARGB8888(255, 0x00,0x81,0xEA)
// #define FG_WARNING                       GUI_COLOR_ARGB8888(255, 0xDF,0x2D,0x00)
// #define FG_NORMAL                        GUI_COLOR_ARGB8888(255, 0x00,0xB4,0x36)
// #define FG_1_DARK                        GUI_COLOR_ARGB8888(255, 0xFF,0xFF,0xFF)
// #define FG_2_DARK                        GUI_COLOR_ARGB8888(0xFF*0.3f, 0xFF,0xFF,0xFF)
// #define FG_3_DARK                        GUI_COLOR_ARGB8888(0xFF*0.5f, 0xFF,0xFF,0xFF)
// #define FG_1_LIGHT                       GUI_COLOR_ARGB8888(255, 0x32,0x32,0x32)
// #define FG_2_LIGHT                       GUI_COLOR_ARGB8888(0xFF*0.3f, 0x32,0x32,0x32)
// #define FG_3_LIGHT                       GUI_COLOR_ARGB8888(0xFF*0.5f, 0x32,0x32,0x32)
// #define FG_WHITE                         FG_1_DARK
// #define FG_WARNING                       GUI_COLOR_ARGB8888(255, 0xDF,0x2D,0x00)

// #define BG_THEME1_BRIGHT_DARK            GUI_COLOR_ARGB8888(255, 0xF6,0x87,0x1F)
// #define BG_THEME1_MID_DARK               GUI_COLOR_ARGB8888(255, 0x5A,0x30,0x1E)
// #define BG_THEME1_DARK_DARK              GUI_COLOR_ARGB8888(255, 0x2C,0x17,0x00)
// #define BG_THEME2_BRIGHT_DARK            GUI_COLOR_ARGB8888(255, 0x00,0xA1,0x4E)
// #define BG_THEME2_MID_DARK               GUI_COLOR_ARGB8888(255, 0x05,0x44,0x2F)
// #define BG_THEME2_DARK_DARK              GUI_COLOR_ARGB8888(255, 0x00,0x28,0x21)
// #define BG_THEME3_BRIGHT_DARK            GUI_COLOR_ARGB8888(255, 0x00,0x85,0xFF)
// #define BG_THEME3_MID_DARK               GUI_COLOR_ARGB8888(255, 0x1A,0x1D,0x6E)
// #define BG_THEME3_DARK_DARK              GUI_COLOR_ARGB8888(255, 0x0F,0x11,0x3D)

// #define BG_THEME1_BRIGHT_LIGHT           GUI_COLOR_ARGB8888(255, 0xFF,0x94,0x20)
// #define BG_THEME1_MID_LIGHT              GUI_COLOR_ARGB8888(255, 0xEF,0xBD,0x8A)
// #define BG_THEME1_DARK_LIGHT             GUI_COLOR_ARGB8888(255, 0xF5,0xE4,0xD3)
// #define BG_THEME2_BRIGHT_LIGHT           GUI_COLOR_ARGB8888(255, 0x14,0xCD,0x6E)
// #define BG_THEME2_MID_LIGHT              GUI_COLOR_ARGB8888(255, 0xA8,0xE5,0xC3)
// #define BG_THEME2_DARK_LIGHT             GUI_COLOR_ARGB8888(255, 0xD1,0xEB,0xDF)
// #define BG_THEME3_BRIGHT_LIGHT           GUI_COLOR_ARGB8888(255, 0x41,0xAA,0xFF)
// #define BG_THEME3_MID_LIGHT              GUI_COLOR_ARGB8888(255, 0xB1,0xD9,0xFF)
// #define BG_THEME3_DARK_LIGHT             GUI_COLOR_ARGB8888(255, 0xD3,0xE8,0xFF)

// #define BG_1_DARK                        GUI_COLOR_ARGB8888(0xFF*0.1f, 0xFF,0xFF,0xFF)
// #define BG_2_DARK                        GUI_COLOR_ARGB8888(0xFF*0.2f, 0xFF,0xFF,0xFF)
// #define BG_1_LIGHT                       GUI_COLOR_ARGB8888(0xFF*0.05f, 0,0,0)
// #define BG_2_LIGHT                       GUI_COLOR_ARGB8888(0xFF*0.2f, 0,0,0)
// #define SCREEN_BG_LIGHT                  GUI_COLOR_ARGB8888(255, 0xF2,0xF2,0xF7)
// #define SCREEN_BG_DARK                   GUI_COLOR_ARGB8888(255, 0,0,0)


/*============================================================================*
 *                            Variables
 *============================================================================*/
/* Time */
extern struct tm *timeinfo;
extern struct tm barn_time;
extern bool time_format_24; // default 24H format

/* Date */
extern const char *month[12];
extern const char *day[7];

/* Page name */
extern const char *page_name_array[28];

/* Clock style */
extern int8_t clock_style;
extern bool switch_from_lock_screen;

/* Theme color */
extern uint8_t theme_color_index;
extern uint32_t theme_color_array[5];

/* Theme bg */
extern bool theme_bg_white; // default black bg

/* Volume */
extern int8_t volume_val; // [0, 10]

/* Battery */
extern uint8_t battery_barn_val;
extern uint8_t battery_earbuds_connect_l_val;
extern uint8_t battery_earbuds_connect_r_val;

/* Message */
extern uint8_t message_num_val;

/* Function status */
extern struct FUNCTION_STATUS f_status;

/* Screen brightness */
extern int8_t screen_brightness_val; // [1, 3]

/* Smart talk time */
extern uint8_t smart_talk_time_type; // 0: 5s, 1: 15s, 2: 20s

/* Ambient sound type */
extern AMBIENT_SOUND_TYPE ambient_sound_type;

/* Spatial sound type */
extern SPATIAL_SOUND_TYPE spatial_sound_type;

/* Voice Aware */
extern int8_t voice_aware_val; // [1, 3]

/* Codec type index */
extern uint8_t codec_type_index; // [0, 5]

/* Timer */
extern int8_t timer_max_min_val; // minutes [1, 60]
extern uint16_t timer_val; // seconds
extern char timer_str[6];

/* Auto dim off screen*/
extern int8_t auto_dim_time_val; // [5, 60], step 5

/* Quick wake up screen*/
extern int8_t quick_wake_up_click_num; // [0, 2]

/* Case button customize*/
extern uint8_t case_button_customize_index_array[3];
extern uint8_t case_button_customize_type_index;
extern const char *case_button_customize_type[3];

/* Language */
extern int8_t language_type_index;

/* Reorder quick access */
extern const char *quick_page_name[4];
extern uint8_t quick_page_name_index;

/* Information center customize */
extern int8_t info_center_func_cnt; //[0,3]
extern const char *info_center_func_name[6];
/*============================================================================*
 *                           Punblic Functions
 *============================================================================*/


#ifdef __cplusplus
}
#endif
#endif

