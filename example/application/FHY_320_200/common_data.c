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
    "Wwe",
    "Thu",
    "Fri",
    "Sat"
};

/* Clock style */
uint8_t clock_style = 3;

/* Theme color */
gui_color_t theme_color = {0};

/* Theme bg */
bool theme_bg_white = false; // default black bg

/* Volume */
int8_t volume_val = 5;

/* Screent bg */
gui_img_t *screen_bg = NULL;

/* Battery */
uint8_t battery_barn_val = 90;
uint8_t battery_earphone_l_val = 90;
uint8_t battery_earphone_r_val = 90;

/* Message */
uint8_t message_num_val = 10;

/* Function status */
bool status_asc = false;
bool status_flashlight = false;
bool status_spatial_sound = false;