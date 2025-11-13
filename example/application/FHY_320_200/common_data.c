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
const char *page_name_array[35] =
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
    "Clock Settings",
    "Time Format",
    "language",
    "Reorder Quick Access",
    "Support",
    "Product Tips",
    "Factory Reset",

    "Password",
    "Message Preview"
};

/* Lock screen */
int8_t wallpaper_index = 0; // [0, 4]
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

/* Playback */
int8_t music_index = 0;
char music_array[MUSIC_MAX_NUM][20] =
{
    "Watermelon Sugar",
    "Save Your Tears",
    "Feels",
    "Stay",
    "Blinding Lights",
    "Levitating",
    "Peaches",
    "Montero",
    "Good 4 U",
    "Kiss Me More",
    "Industry Baby",
    "Heat Waves",
};
char lyrics_str[200] =
{
    "Never gonna give you up \
 Never gonna let you down \
 Never gonna run around and desert you \
 Never gonna make you cry \
 Never gonna say goodbye \
 Never gonna tell a lie and hurt you"
};

/* Volume */
int8_t volume_val = 8; // [0, 16]

/* Battery */
uint8_t battery_tx_val = 30; //[0,100]
uint8_t battery_headband_val = 10;

/* Message */
uint8_t message_num_val = 9;

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

/* Volume unit meter */
uint8_t vu_strength_l = 6; // [0, 13]
uint8_t vu_strength_r = 13; // [0, 13]

/* Silentnow */
uint16_t silent_time_val = 900; // seconds

/* Smart tx management */
struct TX_MANAGEMENT tx_management =
{
    .connected = true,
    .connected_name = "Headphones 1",
    .paired_num = 3,
    .paired_name = {"Headphones 2", "Headphones 3", "Headphones 4"},
};

/* Clock settings */
uint8_t hour_val = 0; //[0, 23]
uint8_t min_val = 0; //[0, 59]
char hour_str[3] = {0};
char min_str[3] = {0};

/* Notification */
struct NOTIFICATION_CONTENT notification_content[NOTIFICATION_MAX_NUM] =
{
    {"Wechat", "Avocado: Hi how's everything going? I'm planing to visit our grandpa next week."},
    {"Line", "Orange: Yo sup bro how's everything going? I'm planing to visit our grandpa next week."},
    {"QQ", "Banana: Yo sup bro how's everything going? I'm planing to visit our grandpa next week."},
    {"Wechat", "Avocado: Hi how's everything going? I'm planing to visit our grandpa next week."},
    {"Line", "Orange: Yo sup bro how's everything going? I'm planing to visit our grandpa next week."},
    {"QQ", "Banana: Yo sup bro how's everything going? I'm planing to visit our grandpa next week."},
    {"Wechat", "Avocado: Hi how's everything going? I'm planing to visit our grandpa next week."},
    {"Line", "Orange: Yo sup bro how's everything going? I'm planing to visit our grandpa next week."},
    {"QQ", "Banana: Yo sup bro how's everything going? I'm planing to visit our grandpa next week."},
};

