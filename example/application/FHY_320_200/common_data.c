/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "common_data.h"


/*============================================================================*
 *                           Types
 *============================================================================*/


/*============================================================================*
 *                            Macros
 *============================================================================*/


/*============================================================================*
 *                            Variables
 *============================================================================*/
/* Time */
struct tm *timeinfo = NULL;
struct tm tx_time =
{
    .tm_hour = 11,
    .tm_min = 20,
    .tm_sec = 0,
    .tm_mday = 15,
    .tm_mon = 10,
    .tm_year = 2025,
    .tm_wday = 3,
};
bool time_format_24 = false; // default 24H format

/* Date */
const char *month[12] =
{
    "Jan",
    "Feb",
    "Mar",
    "Apr",
    "May",
    "Jun",
    "Jul",
    "Aug",
    "Sep",
    "Oct",
    "Nov",
    "Dec"
};
const char *day[7] =
{
    "Sun",
    "Mon",
    "Tue",
    "Wen",
    "Thu",
    "Fri",
    "Sat"
};

/* Page name */
const char *page_name_array[34] =
{
    "playback",
    "Volume",
    "Ambient Sound",
    "Equalizer",
    "Smart Talk",
    "Spatial Sound",
    "High-Resolution Audio (LDAC)",
    "Voice Aware",
    "Auto Play/Pause",
    "Audio Source",
    "Auracast Broadcast",

    "Timer",
    "Flashlight",
    "SilentNow",
    "Volume Unit Meter",
    "JBL Headphones App",

    "Smart Tx Management",
    "Screen Brightness",
    "Dark/Light Mode",
    "Lock Screen",
    "Auto Dim Off Screen",
    "Quick Wake Up Screen",
    "Case Button Customize",
    "Information Center Customize",
    "Unlock Slider",
    "Notification",
    "Time Format",
    "language",
    "Reorder Quick Access",
    "Support",
    "Product Tips",
    "Factory Reset",

    "Password",
    "Message Preview"
};

/* Clock style */
int8_t clock_style = 0; // [0, 4]
bool switch_from_lock_screen = false;

/* Theme color */
uint8_t theme_color_index = 0;
uint32_t theme_color_array[5] =
{
    0xFFFF5901,
    0xFFFF9420,
    0xFF8FBF80,
    0xFF7C9EFF,
    0xFFB490FF,
};

/* Theme bg */
bool theme_bg_white = false; // default black bg

/* Volume */
int8_t volume_val = 8; // [0, 16]

/* Battery */
uint8_t battery_tx_val = 30; //[0,100]
uint8_t battery_headband_val = 10;

/* Message */
uint8_t message_num_val = 10;

/* Function status */
struct FUNCTION_STATUS f_status = {0};

/* Screen Brightness */
int8_t screen_brightness_val = 1; // [1, 3]

/* Smart talk time */
uint8_t smart_talk_time_type = 0; // 0: 5s, 1: 15s, 2: 20s

/* Ambient sound type */
AMBIENT_SOUND_TYPE ambient_sound_type = ANC;

/* Spatial sound type */
SPATIAL_SOUND_TYPE spatial_sound_type = SPATIAL_SOUND_MOVIE;
#if SS_WITH_HEAD_TRACKING
SPATIAL_SOUND_STATUS spatial_sound_status = SPATIAL_SOUND_OFF;
#endif

/* Voice aware */
int8_t voice_aware_val = 2; // [1, 3]

/* Codec type index */
uint8_t codec_type_index = AAC; // [0, 5]

/* Timer */
int8_t timer_max_min_val = 5; // minutes [1, 60]
uint16_t timer_val = 300; // seconds
char timer_str[6] = {0};

/* Call */
uint16_t calltime_val = 0; // seconds
char calltime_str[6] = {0};
char call_source_str[20] = "+8618888888888";

/* Auto dim off screen*/
int8_t auto_dim_time_val = 30; // [5, 60], step 5

/* Quick wake up screen*/
int8_t quick_wake_up_click_num = 0; // [0, 2]

/* Case button customize*/
uint8_t case_button_customize_index_array[3] = {0};
uint8_t case_button_customize_type_index = 0; // [0, 2] single, double, long
const char *case_button_customize_type[3] =
{
    "Single press",
    "Double press",
    "Long press 5s"
};

/* Language */
int8_t language_type_index = ENGLISH;

/* Reorder quick access */
const char *quick_page_name[4] = {0};
uint8_t quick_page_name_index = 0;

/* Information center customize */
int8_t info_center_func_cnt = 3;
const char *info_center_func_name[6] =
{
    "Play/Pause",
    "Ambient\nSound",
    "EQ",
    "Spatial\nSound",
    "Flash light",
    "Dark/Light\nMode"
};

/* Audio source */
struct AUDIO_SOURCE audio_source =
{
    .auracast_num = 0,
    .connected_num = 1,
    .streaming_index = 1,
    .connected_source_is_cast = {0, 0},
    .auracast_hq = {0, 0},
    .paired_num = 3,
    .auracast_receiver = 0,
    .connected_name = {"device 1"},
    .paired_name = {"device 2", "device 3", "device 4"},
};

/* OTA */
uint8_t ota_val = 0; // [0, 100]
char ota_str[4] = {0};

/* Auracast */
char auracast_password[5] = "1234";
