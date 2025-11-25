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
#include "app_main.h"

/*============================================================================*
 *                           Types
 *============================================================================*/
struct FUNCTION_STATUS
{
    bool equalizer              ;
    bool ambient_sound          ;
    bool smart_talk             ;
    bool spatial_sound          ;
    bool hr_audio               ;
    bool voice_aware            ;
    bool auto_play_pause        ;
    bool auracast               ;
    bool password               ;
    bool notification           ;
    bool message_preview        ;
    bool auto_dim_off_screen    ;
    bool quick_wake_up_screen   ;
    bool unlock_slider          ;

    uint32_t bt                 : 2;
    uint32_t ble                : 1;

    uint32_t factory_reset_done : 1;

    uint32_t tx_charging        : 1;
    uint32_t headband_charging  : 1;

    uint32_t headband_connect   : 1;
    uint32_t playback           : 1;
    uint32_t flashlight         : 1;
    uint32_t music_input        : 1;
    uint32_t notification_new   : 1;
    uint32_t call               : 2;
    uint32_t mute               : 1;
    uint32_t timer              : 1;
    uint32_t ota                : 2;
    uint32_t silentnow          : 1;
    uint32_t clock_settings     : 2; // 0:off, 1:hour, 2:minute

    uint32_t infor_center_func_0        : 1;
    uint32_t infor_center_func_1        : 1;
    uint32_t infor_center_func_2        : 1;
    uint32_t infor_center_func_3        : 1;
    uint32_t infor_center_func_4        : 1;
    uint32_t infor_center_func_5        : 1;


};

struct AUDIO_SOURCE
{
    char connected_name[2][30];
    char paired_name[4][30];
    char auracast_name[2][30];
    char auracast_sub_channel_name[2][30];
    bool auracast_hq[2];
    bool connected_auracast_hq[2];
    bool connected_source_is_cast[2];
    bool auracast_receiver;

    uint16_t connected_num               : 2; //[0, 2]
    uint16_t paired_num                  : 3; //[0, 4]
    uint16_t streaming_index             : 2; //[0, 2]  0: none
    uint16_t auracast_streaming_index    : 2; //[0, 2]  0: none
    uint16_t auracast_num                : 2; //[0, 2]
    uint16_t auracast_sub_channel_num    : 2; //[0, 2]
    uint16_t wait                        : 1;
};

struct TX_MANAGEMENT
{
    char connected_name[30];
    char paired_name[4][30];
    char discovered_name[4][30];

    bool connected;
    uint8_t paired_num                  : 3; //[0, 4]
    uint8_t discovered_num              : 3; //[0, 4]
    uint8_t wait                        : 1;
    uint8_t pairing                     : 1;
};

typedef struct gui_call
{
    void (*call_start)(void);
    void (*call_end)(void);
    void (*call_set_speaker)(bool); // True: on, false: off
} gui_call_t;

struct NOTIFICATION_CONTENT
{
    char source[10];
    char content[100];
};

typedef enum
{
    PLAYBACK = 0,
    VOLUME,
    AMBIENT_SOUND,
    EQUALIZER,
    SMART_TALK,
    SPATIAL_SOUND,
    HR_AUDIO,
    VOICE_AWARE,
    AUTO_PLAY_PAUSE,
    AUDIO_SOURCE,
    AURACAST_BROADCAST,

    TIMER,
    FLASHLIGHT,
    SILENTNOW,
    VOLUME_UNIT_METER,
    JBL_HEADPHONES_APP,

    SMART_TX_MANAGEMENT,
    SCREEN_BRIGHTNESS,
    DARK_LIGHT_MODE,
    LOCK_SCREEN,
    AUTO_DIM_OFF_SCREEN,
    QUICK_WAKE_UP_SCREEN,
    CASE_BUTTON_CUSTOMIZE,
    INFORMATION_CENTER_CUSTOMIZE,
    UNLOCK_SLIDER,
    NOTIFICATION,
    CLOCK_SETTINGS,
    TIME_FORMAT,
    LANGUAGE,
    REORDER_QUICK_ACCESS,
    SUPPORT,
    PRODUCT_TIPS,
    FACTORY_RESET,

    PASSWORD,
    MESSAGE_PREVIEW
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
    SPATIAL_SOUND_OFF = 0,
    SPATIAL_SOUND_ON,
    SPATIAL_SOUND_HEAD_TRACKING
} SPATIAL_SOUND_STATUS;

typedef enum
{
    BT_DISCONNECT = 0,
    BT_CONNECT,
    BT_UNKNOWN
} BT_STATUS;

typedef enum
{
    OTA_IDLE = 0,
    OTA_DOING,
    OTA_DONE,
    OTA_FAIL
} OTA_STATUS;

typedef enum
{
    CALL_COMING = 0,
    CALL_ING,
    CALL_OUTGOING,
    CALL_CONNECTING
} CALL_STATUS;

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
#define NOTIFICATION_MAX_NUM  10
#define MUSIC_MAX_NUM 20

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
#define FG_BLACK                         gui_rgb(0,0,0)
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

#define JBL_ORANGE                       gui_rgb(0xFF,0x59,0x01)
#define JBL_GREY_5                       gui_rgb(0x88,0x94,0xA7)

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
extern struct tm tx_time;
extern bool time_format_24; // default 24H format

/* Date */
extern const char *month[13][12];
extern const char *day[13][7];

/* Page name */
extern const char *page_name_array[13][35];

/* Lock screen */
extern int8_t wallpaper_index;
extern bool switch_from_lock_screen;

/* Theme color */
extern uint8_t theme_color_index;
extern uint32_t theme_color_array[5];

/* Theme bg */
extern bool theme_bg_white; // default black bg

/* Playback */
extern int8_t music_index;
extern char music_array[MUSIC_MAX_NUM][20];
extern char lyrics_str[200];

/* Volume */
extern int8_t volume_val; // [0, 16]

/* Battery */
extern uint8_t battery_tx_val;
extern uint8_t battery_headband_val;
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
#if SS_WITH_HEAD_TRACKING
extern SPATIAL_SOUND_STATUS spatial_sound_status;
#endif

/* Voice Aware */
extern int8_t voice_aware_val; // [1, 3]

/* Codec type index */
extern uint8_t codec_type_index; // [0, 5]

/* Timer */
extern int8_t timer_max_min_val; // minutes [1, 60]
extern uint16_t timer_val; // seconds
extern char timer_str[8];

/* Call */
extern uint16_t calltime_val; // seconds
extern char calltime_str[8];
extern char call_source_str[20];
extern gui_call_t *gui_call;

/* Auto dim off screen*/
extern int8_t auto_dim_time_val; // [5, 60], step 5

/* Quick wake up screen*/
extern int8_t quick_wake_up_click_num; // [0, 2]

/* Case button customize*/
extern uint8_t case_button_customize_index_array[3];
extern uint8_t case_button_customize_type_index;
extern const char *case_button_customize_type[3];

/* Language */
extern int8_t language_index;

/* Reorder quick access */
extern const char *quick_page_name[4];
extern uint8_t quick_page_name_index;

/* Information center customize */
extern int8_t info_center_func_cnt; //[0,3]
extern const char *info_center_func_name[6];

/* Audio source */
extern struct AUDIO_SOURCE audio_source;

/* OTA */
extern uint8_t ota_val; // [0, 100]
extern char ota_str[8];

/* Auracast */
extern char auracast_password[5];

/* Volume unit meter */
extern uint8_t vu_strength_l; // [0, 13]
extern uint8_t vu_strength_r; // [0, 13]

/* Silentnow */
extern uint16_t silent_time_val; // seconds

/* Smart tx management */
extern struct TX_MANAGEMENT tx_management;

/* Clock settings */
extern uint8_t hour_val; //[0, 23]
extern uint8_t minutes_val; //[0, 59]
extern char hour_str[8];
extern char minutes_str[8];

/* Notification */
extern struct NOTIFICATION_CONTENT notification_content[NOTIFICATION_MAX_NUM];

/*============================================================================*
 *                           Punblic Functions
 *============================================================================*/
void msg_2_regenerate_current_view(void);

void msg_2_switch_ota_view(void);
void msg_2_switch_charging_view(void);
void msg_2_switch_call_view(void);

void set_device_name(const char *name);

void gui_call_register(gui_call_t *call);

#ifdef __cplusplus
}
#endif
#endif

