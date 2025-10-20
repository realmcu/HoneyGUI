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
struct tm barn_time =
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

/* Clock style */
int8_t clock_style = 0;
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
int8_t volume_val = 0; // [0, 10]

/* Battery */
uint8_t battery_barn_val = 90;
uint8_t battery_earphone_l_val = 90;
uint8_t battery_earphone_r_val = 90;

/* Message */
uint8_t message_num_val = 10;

/* Function status */
struct FUNCTION_STATUS f_status = {0};

/* Screen Brightness */
int8_t screen_brightness_val = 1; // [1, 3]