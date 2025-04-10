#include "ezxml.h"
#include <gui_tabview.h>
#include <gui_img.h>
#include <gui_win.h>
#include <gui_text.h>
#include <gui_obj.h>
#include <gui_curtain.h>
#include "gui_button.h"
#include <gui_seekbar.h>
#include <gui_page.h>
#include "gui_gallery.h"
#include "gui_keyboard.h"
#include <string.h>
#include <time.h>
#include <stdio.h>
#include "gui_server.h"
#include <gui_img.h>
#include <gui_app.h>
#include <stdlib.h>
#include "gui_grid.h"
#include "gui_scroll_text.h"
#include "gui_switch.h"
#include "gui_tab.h"
#include "string.h"
#include "stdio.h"
#include <stdlib.h>

#include <string.h>
#include <ctype.h>
#include "ezxml.h"

#include "gui_slider.h"
#include "gui_scroll_wheel_new.h"
#include "gui_calendar.h"
#include "gui_combo.h"
#include "gui_wave.h"
#include "gui_api.h"
#include "cJSON.h"
#include "gui_multi_level.h"
#include "gui_pagelist_new.h"
#include "gui_canvas_rect.h"
//char *GUI_ROOT_FOLDER = GUI_ROOT_FOLDER;
struct widget_create
{
    char *name;
    int type;
};

#define BUTTON_HIGHLIGHT_ARRAY INT8_MAX
#define XML_DOM_KEY_COUINT (UINT8_MAX + 1)
#define XML_DOM_WIFI_COUINT (30)
typedef struct xml_dom_wifi
{
    const char *name;
} xml_dom_wifi_t;
static int active_wifi_index;
#define ACTIVE_WIFI_PASSWORD_MAX (30)
static char active_wifi_password_input[ACTIVE_WIFI_PASSWORD_MAX];

#define ACTIVE_WIFI_STATUS_DISCONNECTED (4)
#define ACTIVE_WIFI_STATUS_CONNECTED (1)
#define ACTIVE_WIFI_STATUS_CONNECTING (2)
#define ACTIVE_WIFI_STATUS_DISABLECONNECT (0)
static char active_wifi_status = ACTIVE_WIFI_STATUS_DISABLECONNECT;
static xml_dom_wifi_t wifi_array[XML_DOM_WIFI_COUINT] =
{
    {"WiFi1"}, {"WiFi2"}, {"WiFi3"}, {"WiFi4"}, {"WiFi5"}, {"WiFi6"},  {"WiFi7"}, {"WiFi8"}, {"WiFi9"}, {"WiFi10"},
    {"WiFi11"}, {"WiFi12"}, {"WiFi13"}, {"WiFi14"}, {"WiFi15"}, {"WiFi16"}, {"WiFi17"}, {"WiFi18"}, {"WiFi19"}, {"WiFi20"},
    {"WiFi21"}, {"WiFi22"}, {"WiFi23"}, {"WiFi24"}, {"WiFi25"}, {"WiFi26"}, {"WiFi27"},
};
#define MACRO_CONNECTED 9001
#define MACRO_DISCONNECTED 9002
#define MACRO_CONNECTING 9003
#define MACRO_WIFI 9004

static struct widget_create widget[] =
{
    { "win", WINDOW }, //
    { "textbox", TEXTBOX },//
    { "img", IMAGE },//
    { "progressbar", PROGRESSBAR },
    { "seekbar", SEEKBAR },
    { "tabview", TABVIEW },//
    { "tab", TAB },
    { "curtainview", CURTAINVIEW },//
    { "curtain", CURTAIN },
    //{ "iconlist", ICONLIST },
    { "icon", ICON },//
    { "script", JAVASCRIPT },
    { "switch", CLICKSWITCH },
    { "page", PAGE },//
    //{ "pagebar", PAGEBAR },
    { "screen", SCREEN },
    { "grid",  GRID},
    {"gallery", GALLERY},
    {"radio", RADIO},
    {"appendTexts", RADIOSWITCH},
    {"arc", MACRO_CANVAS_ARC},
    {"movie", MOVIE},
    {"animateTransform", MACRO_ANIMATETRANSFORM},
    {"motorizedCurtain", MACRO_MOTORIZED_CURTAIN},
    {"keyboard", KEYBOARD},
    {"multiLevel", MULTI_LEVEL},
    {"onClick", MACRO_ONCLICK},
    {"backIcon", MACRO_BACKICON},
    {"slider", SLIDER},
    {"onChange", MACRO_ONCHANGE},
    {"onOn", MACRO_ONON},
    {"onOff", MACRO_ONOFF},
    {"onLoad", MACRO_ONLOAD},
    {"roller", TYPE_SCROLL_WHEEL_NEW},
    {"calendar", MACRO_CALENDAR},
    {"onTime", MACRO_ONTIME},
    {"combo", MACRO_COMBO},
    {"onPeripheral", MACRO_ON_PERIPHERAL},
    {"chart", MACRO_CHART},
    {"listview", MACRO_PAGE_LIST_NEW},
    {"onComplete", MACRO_ONCOMPLETE},
    {"key", MACRO_KEY},
    {"onSelect", MACRO_ONSELECT},
    {"rect", RECTANGLE},
    {"wifi", MACRO_WIFI},
    {"onConnected", MACRO_CONNECTED},
    {"onDisconnected", MACRO_DISCONNECTED},
    {"onConnecting", MACRO_CONNECTING},
};

typedef struct
{
    int x;
    int y;
    int translate_from_x;
    int translate_from_y;
    int translate_to_x;
    int translate_to_y;
    int rotate_x;
    int rotate_y;
    int rotate_from;
    int rotate_to;
    float textContent_from;
    float textContent_to;
    float scale_x;
    float scale_y;
    float scale_x_from;
    float scale_y_from;
    int scale_x_center;
    int scale_y_center;
    uint8_t opacity;
    uint8_t opacity_from;
    void **image_arr;
    int image_count;
    char *img_name;
    char *animate_type;
    gui_img_t *img;
} image_animate_params_t;
typedef struct
{
    float values[10];
    float values_2[10];
    float key_times[10];
    int length;
    char *img_name;
    char *animate_type;
    gui_img_t *img;
    uint8_t calc_mode;
} image_animate_key_times_params_t;
typedef struct arc_animation_param
{
    gui_color_t stroke;
    float sd;
    float ed;
    int image_data_length;
    int image_width;
    int image_height;
    uint8_t *target_buffer;
    int16_t cx;
    int16_t cy;
    int16_t r;
    int16_t stroke_width;
    uint8_t cap;
    uint8_t aniamtion_type;
    uint8_t dir;
    uint8_t calc_mode;

} arc_animation_param_t;
typedef struct chart_animation_param
{
    gui_color_t color;
    int image_data_length;
    int image_width;
    int image_height;
    uint8_t *target_buffer;

    int *data_array;
    int max, min;
    uint8_t aniamtion_type;
    uint8_t chart_type;
    int16_t stroke_width;
} chart_animation_param_t;
typedef struct
{
    int num1;
    int num2;
} two_integers;
struct on_click_jump_cb_param
{
    const char *to_widget_name;
    int id1;
    int id2;
    int id;
};

/**
 *  \brief The keyboard scancode representation.
 *
 *  Values of this type are used to represent keyboard keys, among other places
 *  in the \link Keysym::scancode key.keysym.scancode \endlink field of the
 *  Event structure.
 *
 *  The values in this enumeration are based on the USB usage page standard:
 *  https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf
 */
typedef enum
{
    SCANCODE_UNKNOWN = 0,

    /**
     *  \name Usage page 0x07
     *
     *  These values are from usage page 0x07 (USB keyboard page).
     */
    /* @{ */

    SCANCODE_A = 4,
    SCANCODE_B = 5,
    SCANCODE_C = 6,
    SCANCODE_D = 7,
    SCANCODE_E = 8,
    SCANCODE_F = 9,
    SCANCODE_G = 10,
    SCANCODE_H = 11,
    SCANCODE_I = 12,
    SCANCODE_J = 13,
    SCANCODE_K = 14,
    SCANCODE_L = 15,
    SCANCODE_M = 16,
    SCANCODE_N = 17,
    SCANCODE_O = 18,
    SCANCODE_P = 19,
    SCANCODE_Q = 20,
    SCANCODE_R = 21,
    SCANCODE_S = 22,
    SCANCODE_T = 23,
    SCANCODE_U = 24,
    SCANCODE_V = 25,
    SCANCODE_W = 26,
    SCANCODE_X = 27,
    SCANCODE_Y = 28,
    SCANCODE_Z = 29,

    SCANCODE_1 = 30,
    SCANCODE_2 = 31,
    SCANCODE_3 = 32,
    SCANCODE_4 = 33,
    SCANCODE_5 = 34,
    SCANCODE_6 = 35,
    SCANCODE_7 = 36,
    SCANCODE_8 = 37,
    SCANCODE_9 = 38,
    SCANCODE_0 = 39,

    SCANCODE_RETURN = 40,
    SCANCODE_ESCAPE = 41,
    SCANCODE_BACKSPACE = 42,
    SCANCODE_TAB = 43,
    SCANCODE_SPACE = 44,

    SCANCODE_MINUS = 45,
    SCANCODE_EQUALS = 46,
    SCANCODE_LEFTBRACKET = 47,
    SCANCODE_RIGHTBRACKET = 48,
    SCANCODE_BACKSLASH = 49, /**< Located at the lower left of the return
                                  *   key on ISO keyboards and at the right end
                                  *   of the QWERTY row on ANSI keyboards.
                                  *   Produces REVERSE SOLIDUS (backslash) and
                                  *   VERTICAL LINE in a US layout, REVERSE
                                  *   SOLIDUS and VERTICAL LINE in a UK Mac
                                  *   layout, NUMBER SIGN and TILDE in a UK
                                  *   Windows layout, DOLLAR SIGN and POUND SIGN
                                  *   in a Swiss German layout, NUMBER SIGN and
                                  *   APOSTROPHE in a German layout, GRAVE
                                  *   ACCENT and POUND SIGN in a French Mac
                                  *   layout, and ASTERISK and MICRO SIGN in a
                                  *   French Windows layout.
                                  */
    SCANCODE_NONUSHASH = 50, /**< ISO USB keyboards actually use this code
                                  *   instead of 49 for the same key, but all
                                  *   OSes I've seen treat the two codes
                                  *   identically. So, as an implementor, unless
                                  *   your keyboard generates both of those
                                  *   codes and your OS treats them differently,
                                  *   you should generate SCANCODE_BACKSLASH
                                  *   instead of this code. As a user, you
                                  *   should not rely on this code because SDL
                                  *   will never generate it with most (all?)
                                  *   keyboards.
                                  */
    SCANCODE_SEMICOLON = 51,
    SCANCODE_APOSTROPHE = 52,
    SCANCODE_GRAVE = 53, /**< Located in the top left corner (on both ANSI
                              *   and ISO keyboards). Produces GRAVE ACCENT and
                              *   TILDE in a US Windows layout and in US and UK
                              *   Mac layouts on ANSI keyboards, GRAVE ACCENT
                              *   and NOT SIGN in a UK Windows layout, SECTION
                              *   SIGN and PLUS-MINUS SIGN in US and UK Mac
                              *   layouts on ISO keyboards, SECTION SIGN and
                              *   DEGREE SIGN in a Swiss German layout (Mac:
                              *   only on ISO keyboards), CIRCUMFLEX ACCENT and
                              *   DEGREE SIGN in a German layout (Mac: only on
                              *   ISO keyboards), SUPERSCRIPT TWO and TILDE in a
                              *   French Windows layout, COMMERCIAL AT and
                              *   NUMBER SIGN in a French Mac layout on ISO
                              *   keyboards, and LESS-THAN SIGN and GREATER-THAN
                              *   SIGN in a Swiss German, German, or French Mac
                              *   layout on ANSI keyboards.
                              */
    SCANCODE_COMMA = 54,
    SCANCODE_PERIOD = 55,
    SCANCODE_SLASH = 56,

    SCANCODE_CAPSLOCK = 57,

    SCANCODE_F1 = 58,
    SCANCODE_F2 = 59,
    SCANCODE_F3 = 60,
    SCANCODE_F4 = 61,
    SCANCODE_F5 = 62,
    SCANCODE_F6 = 63,
    SCANCODE_F7 = 64,
    SCANCODE_F8 = 65,
    SCANCODE_F9 = 66,
    SCANCODE_F10 = 67,
    SCANCODE_F11 = 68,
    SCANCODE_F12 = 69,

    SCANCODE_PRINTSCREEN = 70,
    SCANCODE_SCROLLLOCK = 71,
    SCANCODE_PAUSE = 72,
    SCANCODE_INSERT = 73, /**< insert on PC, help on some Mac keyboards (but
                                   does send code 73, not 117) */
    SCANCODE_HOME = 74,
    SCANCODE_PAGEUP = 75,
    SCANCODE_DELETE = 76,
    SCANCODE_END = 77,
    SCANCODE_PAGEDOWN = 78,
    SCANCODE_RIGHT = 79,
    SCANCODE_LEFT = 80,
    SCANCODE_DOWN = 81,
    SCANCODE_UP = 82,

    SCANCODE_NUMLOCKCLEAR = 83, /**< num lock on PC, clear on Mac keyboards
                                     */
    SCANCODE_KP_DIVIDE = 84,
    SCANCODE_KP_MULTIPLY = 85,
    SCANCODE_KP_MINUS = 86,
    SCANCODE_KP_PLUS = 87,
    SCANCODE_KP_ENTER = 88,
    SCANCODE_KP_1 = 89,
    SCANCODE_KP_2 = 90,
    SCANCODE_KP_3 = 91,
    SCANCODE_KP_4 = 92,
    SCANCODE_KP_5 = 93,
    SCANCODE_KP_6 = 94,
    SCANCODE_KP_7 = 95,
    SCANCODE_KP_8 = 96,
    SCANCODE_KP_9 = 97,
    SCANCODE_KP_0 = 98,
    SCANCODE_KP_PERIOD = 99,

    SCANCODE_NONUSBACKSLASH = 100, /**< This is the additional key that ISO
                                        *   keyboards have over ANSI ones,
                                        *   located between left shift and Y.
                                        *   Produces GRAVE ACCENT and TILDE in a
                                        *   US or UK Mac layout, REVERSE SOLIDUS
                                        *   (backslash) and VERTICAL LINE in a
                                        *   US or UK Windows layout, and
                                        *   LESS-THAN SIGN and GREATER-THAN SIGN
                                        *   in a Swiss German, German, or French
                                        *   layout. */
    SCANCODE_APPLICATION = 101, /**< windows contextual menu, compose */
    SCANCODE_POWER = 102, /**< The USB document says this is a status flag,
                               *   not a physical key - but some Mac keyboards
                               *   do have a power key. */
    SCANCODE_KP_EQUALS = 103,
    SCANCODE_F13 = 104,
    SCANCODE_F14 = 105,
    SCANCODE_F15 = 106,
    SCANCODE_F16 = 107,
    SCANCODE_F17 = 108,
    SCANCODE_F18 = 109,
    SCANCODE_F19 = 110,
    SCANCODE_F20 = 111,
    SCANCODE_F21 = 112,
    SCANCODE_F22 = 113,
    SCANCODE_F23 = 114,
    SCANCODE_F24 = 115,
    SCANCODE_EXECUTE = 116,
    SCANCODE_HELP = 117,    /**< AL Integrated Help Center */
    SCANCODE_MENU = 118,    /**< Menu (show menu) */
    SCANCODE_SELECT = 119,
    SCANCODE_STOP = 120,    /**< AC Stop */
    SCANCODE_AGAIN = 121,   /**< AC Redo/Repeat */
    SCANCODE_UNDO = 122,    /**< AC Undo */
    SCANCODE_CUT = 123,     /**< AC Cut */
    SCANCODE_COPY = 124,    /**< AC Copy */
    SCANCODE_PASTE = 125,   /**< AC Paste */
    SCANCODE_FIND = 126,    /**< AC Find */
    SCANCODE_MUTE = 127,
    SCANCODE_VOLUMEUP = 128,
    SCANCODE_VOLUMEDOWN = 129,
    /* not sure whether there's a reason to enable these */
    /*     SCANCODE_LOCKINGCAPSLOCK = 130,  */
    /*     SCANCODE_LOCKINGNUMLOCK = 131, */
    /*     SCANCODE_LOCKINGSCROLLLOCK = 132, */
    SCANCODE_KP_COMMA = 133,
    SCANCODE_KP_EQUALSAS400 = 134,

    SCANCODE_INTERNATIONAL1 = 135, /**< used on Asian keyboards, see
                                            footnotes in USB doc */
    SCANCODE_INTERNATIONAL2 = 136,
    SCANCODE_INTERNATIONAL3 = 137, /**< Yen */
    SCANCODE_INTERNATIONAL4 = 138,
    SCANCODE_INTERNATIONAL5 = 139,
    SCANCODE_INTERNATIONAL6 = 140,
    SCANCODE_INTERNATIONAL7 = 141,
    SCANCODE_INTERNATIONAL8 = 142,
    SCANCODE_INTERNATIONAL9 = 143,
    SCANCODE_LANG1 = 144, /**< Hangul/English toggle */
    SCANCODE_LANG2 = 145, /**< Hanja conversion */
    SCANCODE_LANG3 = 146, /**< Katakana */
    SCANCODE_LANG4 = 147, /**< Hiragana */
    SCANCODE_LANG5 = 148, /**< Zenkaku/Hankaku */
    SCANCODE_LANG6 = 149, /**< reserved */
    SCANCODE_LANG7 = 150, /**< reserved */
    SCANCODE_LANG8 = 151, /**< reserved */
    SCANCODE_LANG9 = 152, /**< reserved */

    SCANCODE_ALTERASE = 153,    /**< Erase-Eaze */
    SCANCODE_SYSREQ = 154,
    SCANCODE_CANCEL = 155,      /**< AC Cancel */
    SCANCODE_CLEAR = 156,
    SCANCODE_PRIOR = 157,
    SCANCODE_RETURN2 = 158,
    SCANCODE_SEPARATOR = 159,
    SCANCODE_OUT = 160,
    SCANCODE_OPER = 161,
    SCANCODE_CLEARAGAIN = 162,
    SCANCODE_CRSEL = 163,
    SCANCODE_EXSEL = 164,

    SCANCODE_KP_00 = 176,
    SCANCODE_KP_000 = 177,
    SCANCODE_THOUSANDSSEPARATOR = 178,
    SCANCODE_DECIMALSEPARATOR = 179,
    SCANCODE_CURRENCYUNIT = 180,
    SCANCODE_CURRENCYSUBUNIT = 181,
    SCANCODE_KP_LEFTPAREN = 182,
    SCANCODE_KP_RIGHTPAREN = 183,
    SCANCODE_KP_LEFTBRACE = 184,
    SCANCODE_KP_RIGHTBRACE = 185,
    SCANCODE_KP_TAB = 186,
    SCANCODE_KP_BACKSPACE = 187,
    SCANCODE_KP_A = 188,
    SCANCODE_KP_B = 189,
    SCANCODE_KP_C = 190,
    SCANCODE_KP_D = 191,
    SCANCODE_KP_E = 192,
    SCANCODE_KP_F = 193,
    SCANCODE_KP_XOR = 194,
    SCANCODE_KP_POWER = 195,
    SCANCODE_KP_PERCENT = 196,
    SCANCODE_KP_LESS = 197,
    SCANCODE_KP_GREATER = 198,
    SCANCODE_KP_AMPERSAND = 199,
    SCANCODE_KP_DBLAMPERSAND = 200,
    SCANCODE_KP_VERTICALBAR = 201,
    SCANCODE_KP_DBLVERTICALBAR = 202,
    SCANCODE_KP_COLON = 203,
    SCANCODE_KP_HASH = 204,
    SCANCODE_KP_SPACE = 205,
    SCANCODE_KP_AT = 206,
    SCANCODE_KP_EXCLAM = 207,
    SCANCODE_KP_MEMSTORE = 208,
    SCANCODE_KP_MEMRECALL = 209,
    SCANCODE_KP_MEMCLEAR = 210,
    SCANCODE_KP_MEMADD = 211,
    SCANCODE_KP_MEMSUBTRACT = 212,
    SCANCODE_KP_MEMMULTIPLY = 213,
    SCANCODE_KP_MEMDIVIDE = 214,
    SCANCODE_KP_PLUSMINUS = 215,
    SCANCODE_KP_CLEAR = 216,
    SCANCODE_KP_CLEARENTRY = 217,
    SCANCODE_KP_BINARY = 218,
    SCANCODE_KP_OCTAL = 219,
    SCANCODE_KP_DECIMAL = 220,
    SCANCODE_KP_HEXADECIMAL = 221,

    SCANCODE_LCTRL = 224,
    SCANCODE_LSHIFT = 225,
    SCANCODE_LALT = 226, /**< alt, option */
    SCANCODE_LGUI = 227, /**< windows, command (apple), meta */
    SCANCODE_RCTRL = 228,
    SCANCODE_RSHIFT = 229,
    SCANCODE_RALT = 230, /**< alt gr, option */
    SCANCODE_RGUI = 231, /**< windows, command (apple), meta */

    SCANCODE_MODE = 257,    /**< I'm not sure if this is really not covered
                                 *   by any of the above, but since there's a
                                 *   special KMOD_MODE for it I'm adding it here
                                 */

    /* @} *//* Usage page 0x07 */

    /**
     *  \name Usage page 0x0C
     *
     *  These values are mapped from usage page 0x0C (USB consumer page).
     *  See https://usb.org/sites/default/files/hut1_2.pdf
     *
     *  There are way more keys in the spec than we can represent in the
     *  current scancode range, so pick the ones that commonly come up in
     *  real world usage.
     */
    /* @{ */

    SCANCODE_AUDIONEXT = 258,
    SCANCODE_AUDIOPREV = 259,
    SCANCODE_AUDIOSTOP = 260,
    SCANCODE_AUDIOPLAY = 261,
    SCANCODE_AUDIOMUTE = 262,
    SCANCODE_MEDIASELECT = 263,
    SCANCODE_WWW = 264,             /**< AL Internet Browser */
    SCANCODE_MAIL = 265,
    SCANCODE_CALCULATOR = 266,      /**< AL Calculator */
    SCANCODE_COMPUTER = 267,
    SCANCODE_AC_SEARCH = 268,       /**< AC Search */
    SCANCODE_AC_HOME = 269,         /**< AC Home */
    SCANCODE_AC_BACK = 270,         /**< AC Back */
    SCANCODE_AC_FORWARD = 271,      /**< AC Forward */
    SCANCODE_AC_STOP = 272,         /**< AC Stop */
    SCANCODE_AC_REFRESH = 273,      /**< AC Refresh */
    SCANCODE_AC_BOOKMARKS = 274,    /**< AC Bookmarks */

    /* @} *//* Usage page 0x0C */

    /**
     *  \name Walther keys
     *
     *  These are values that Christian Walther added (for mac keyboard?).
     */
    /* @{ */

    SCANCODE_BRIGHTNESSDOWN = 275,
    SCANCODE_BRIGHTNESSUP = 276,
    SCANCODE_DISPLAYSWITCH = 277, /**< display mirroring/dual display
                                           switch, video mode switch */
    SCANCODE_KBDILLUMTOGGLE = 278,
    SCANCODE_KBDILLUMDOWN = 279,
    SCANCODE_KBDILLUMUP = 280,
    SCANCODE_EJECT = 281,
    SCANCODE_SLEEP = 282,           /**< SC System Sleep */

    SCANCODE_APP1 = 283,
    SCANCODE_APP2 = 284,

    /* @} *//* Walther keys */

    /**
     *  \name Usage page 0x0C (additional media keys)
     *
     *  These values are mapped from usage page 0x0C (USB consumer page).
     */
    /* @{ */

    SCANCODE_AUDIOREWIND = 285,
    SCANCODE_AUDIOFASTFORWARD = 286,

    /* @} *//* Usage page 0x0C (additional media keys) */

    /**
     *  \name Mobile keys
     *
     *  These are values that are often used on mobile phones.
     */
    /* @{ */

    SCANCODE_SOFTLEFT = 287, /**< Usually situated below the display on phones and
                                      used as a multi-function feature key for selecting
                                      a software defined function shown on the bottom left
                                      of the display. */
    SCANCODE_SOFTRIGHT = 288, /**< Usually situated below the display on phones and
                                       used as a multi-function feature key for selecting
                                       a software defined function shown on the bottom right
                                       of the display. */
    SCANCODE_CALL = 289, /**< Used for accepting phone calls. */
    SCANCODE_ENDCALL = 290, /**< Used for rejecting phone calls. */

    /* @} *//* Mobile keys */

    /* Add any other keys here. */

    NUM_SCANCODES = 512 /**< not a key, just marks the number of scancodes
                                 for array bounds */
} scan_code;
struct on_click_keyboard_cb_param
{
    gui_text_t *to_text;
    scan_code key_board;
};
#define TEXT_WEATHER_CUR_ANIMATION 1
#define ARC_ACTIVITY_EX_ANIMATION 2
#define CHART_HEART_RATE_DATA_ANIMATION 3
#define TEXT_HEART_RATE_CUR_ANIMATION 4
#define TEXT_ACTIVITY_EX_ANIMATION 5
#define TEXT_BATTERY_CAPACITY_ANIMATION 6
#define TEXT_WIFI_NAME_ANIMATION 7
#define TEXT_WIFI_PASSWORD_ANIMATION 8
#define ANIMATION_CALC_MODE_DISCRETE 0
#define ANIMATION_CALC_MODE_LINEAR 1
#define ANIMATION_CALC_MODE_PACED 3
#define ANIMATION_CALC_MODE_SPLINE 4
static void pause_animation_cb(gui_obj_t *this, void *null, char *to_name[]);
static void start_animation_cb(gui_obj_t *this, void *null, char *to_name[]);
static void foreach_create_animate(ezxml_t p, gui_obj_t *parent, const char *animate_name);

static const uint8_t *gui_get_image_file_address(const char *image_file_path);

static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(text_animate_watchface_callback);
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(img_animate_watchface_callback);
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(arc_animate_activity_callback);
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(text_animate_weather_callback);
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(chart_animate_heartrate_data_callback);
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(text_animation_hr_callback);
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(text_animate_activity_callback);
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(text_animate_battery_callback);
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(img_animate_watchface_callback_hour);
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(img_animate_watchface_callback_minute);
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(img_animate_watchface_callback_second);
static void get_2_int_from_string(const char *input, int *int1, int *int2);
static bool ends_with_xml(const char *str);
static ezxml_t f1;
static void gui_canvas_img_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type);
static void img_ontime_render(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type);
static void img_ontime_canvas_buffer_render(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type);
static void img_render(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type);
static void button_render(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type);
static void parse_id_string(const char *id, struct on_click_jump_cb_param *param);
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(page_list_animation_wifi_name);
extern gui_multi_level_t *gui_multi_level_create_group(void *parent, const char *widget_name,
                                                       void (*ui_design)(gui_obj_t *), const char *group_name);
scan_code get_scan_code_by_keyboard_string(const char *keyboard);
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(text_animate_wifi_callback);
static void on_click_keyboard_cb_switch_turn_on(void *obj, gui_event_t e,
                                                struct on_click_keyboard_cb_param *param);
static bool shift;
static void gui_xml_dom_text_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type);
/**
 * @brief Writes a key value in a array at the specified ID.
 *
 * @param id The ID of the key array to write.
 * @param up Whether the key is up or down.
 * @param down Whether the key is down or up.
 *
 * @return A gui_error_t indicating the result of the operation.
 */
gui_error_t gui_xml_dom_write_key_array(int id, bool up, bool down);
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(win_wifi_ani);
static void img_rotate_cb(image_animate_params_t *animate_params, void *null,
                          gui_animate_t *animate)
{


    gui_img_translate(animate_params->img, animate_params->rotate_x,
                      animate_params->rotate_y);
    // gui_log("img_rotate_cb%f\n", animate_params->rotate_from + (animate_params->rotate_to -
    //                                                             animate_params->rotate_from)*
    //         animate->progress_percent);
    gui_img_rotation(animate_params->img,
                     animate_params->rotate_from + (animate_params->rotate_to - animate_params->rotate_from)*
                     animate->progress_percent,
                     animate_params->rotate_x,
                     animate_params->rotate_y);
}

static void img_translate_cb(image_animate_params_t *animate_params, void *null,
                             gui_animate_t *animate)
{
    float t_x = animate_params->translate_from_x + animate->progress_percent *
                (animate_params->translate_to_x -
                 animate_params->translate_from_x);
    float t_y = animate_params->translate_from_y + animate->progress_percent *
                (animate_params->translate_to_y -
                 animate_params->translate_from_y);

    gui_img_translate(animate_params->img, t_x, t_y);
}

static void img_scale_cb(image_animate_params_t *animate_params, void *null, gui_animate_t *animate)
{
    float scale_x = animate_params->scale_x_from - animate->progress_percent *
                    (animate_params->scale_x_from - animate_params->scale_x);
    float scale_y = animate_params->scale_y_from - animate->progress_percent *
                    (animate_params->scale_y_from - animate_params->scale_y);

    gui_img_scale(animate_params->img, scale_x, scale_y);
    gui_img_translate(animate_params->img, animate_params->scale_x_center,
                      animate_params->scale_y_center);

    gui_img_rotation(animate_params->img, 0, animate_params->scale_x_center,
                     animate_params->scale_y_center);
}


static void text_content_cb(image_animate_params_t *animate_params, void *null,
                            gui_animate_t *animate)
{
    uint8_t opacity = animate_params->textContent_from - animate->progress_percent *
                      (animate_params->textContent_from - animate_params->textContent_to);
    //gui_log("img_opacity_cb%d\n", opacity);
    int num = opacity;
    static  char str[20];


    sprintf(str, "%d", num);

    gui_text_content_set((void *)animate_params->img, str, strlen(str));

}
static void progress_content_cb(image_animate_params_t *animate_params, void *null,
                                gui_animate_t *animate)
{
    float opacity = animate_params->textContent_from - animate->progress_percent *
                    (animate_params->textContent_from - animate_params->textContent_to);
    gui_progressbar_set_percentage((void *)animate_params->img, opacity);
}
static void img_opacity_cb(image_animate_params_t *animate_params, void *null,
                           gui_animate_t *animate)
{
    uint8_t opacity = animate_params->opacity_from - animate->progress_percent *
                      (animate_params->opacity_from - animate_params->opacity);
    //gui_log("img_opacity_cb%d\n", opacity);
    gui_img_set_opacity(animate_params->img, opacity);
}
static void img_opacity_key_times_cb(image_animate_key_times_params_t *animate_params, void *null,
                                     gui_animate_t *animate)
{
    float per = animate->progress_percent;
    int opacity = 255; // Default opacity if no interpolation is possible

    switch (animate_params->calc_mode)
    {
    case ANIMATION_CALC_MODE_DISCRETE:
        {
            // Find the position in the key_times array
            for (int i = 0; i < animate_params->length - 1; i++)
            {
                if (per >= animate_params->key_times[i] && per < animate_params->key_times[i + 1])
                {
                    // Linear interpolation between key times
                    //float segment_progress = (per - animate_params->key_times[i]) / (animate_params->key_times[i + 1] - animate_params->key_times[i]);
                    opacity = animate_params->values[i] /*+ segment_progress * (animate_params->values[i + 1] - animate_params->values[i])*/;
                    break;
                }
            }

        }
        break;
    case ANIMATION_CALC_MODE_LINEAR:
    case ANIMATION_CALC_MODE_PACED:
    case ANIMATION_CALC_MODE_SPLINE:
        {
            // Find the position in the key_times array
            for (int i = 0; i < animate_params->length - 1; i++)
            {
                if (per >= animate_params->key_times[i] && per < animate_params->key_times[i + 1])
                {
                    // Linear interpolation between key times
                    float segment_progress = (per - animate_params->key_times[i]) / (animate_params->key_times[i + 1] -
                                                                                     animate_params->key_times[i]);
                    opacity = animate_params->values[i] + segment_progress * (animate_params->values[i + 1] -
                                                                              animate_params->values[i]);
                    break;
                }
            }

        }
        break;
    default:
        break;
    }


    // If 'per' is exactly at the last key time, use the last value
    if (per >= animate_params->key_times[animate_params->length - 1] || animate->end_frame)
    {
        opacity = animate_params->values[animate_params->length - 1];
    }

    // Set the image opacity
    gui_img_set_opacity(animate_params->img,
                        (int)(opacity)); // Assuming gui_img_set_opacity expects an integer between 0 and 255

    // Optional: debug logging
    // gui_log("img_opacity_cb: opacity set to %d\n", (int)(opacity * 255));
}
static void img_translate_key_times_cb(image_animate_key_times_params_t *animate_params, void *null,
                                       gui_animate_t *animate)
{
    float per = animate->progress_percent;

    float t_x = 0;


    float t_y = 0;


    switch (animate_params->calc_mode)
    {
    case ANIMATION_CALC_MODE_DISCRETE:
        {
// Find the position in the key_times array
            for (int i = 0; i < animate_params->length - 1; i++)
            {
                if (per >= animate_params->key_times[i] && per < animate_params->key_times[i + 1])
                {
                    // Linear interpolation between key times
                    //float segment_progress = (per - animate_params->key_times[i]) / (animate_params->key_times[i + 1] - animate_params->key_times[i]);
                    t_x = animate_params->values[i] /*+ segment_progress * (animate_params->values[i + 1] - animate_params->values[i])*/;
                    t_y = animate_params->values_2[i];
                    break;
                }
            }
        }
        break;
    case ANIMATION_CALC_MODE_LINEAR:
    case ANIMATION_CALC_MODE_PACED:
    case ANIMATION_CALC_MODE_SPLINE:
        {
// Find the position in the key_times array
            for (int i = 0; i < animate_params->length - 1; i++)
            {
                if (per >= animate_params->key_times[i] && per < animate_params->key_times[i + 1])
                {
                    // Linear interpolation between key times
                    float segment_progress = (per - animate_params->key_times[i]) / (animate_params->key_times[i + 1] -
                                                                                     animate_params->key_times[i]);
                    t_x = animate_params->values[i] + segment_progress * (animate_params->values[i + 1] -
                                                                          animate_params->values[i]);
                    t_y = animate_params->values_2[i] + segment_progress * (animate_params->values_2[i + 1] -
                                                                            animate_params->values_2[i]);

                    break;
                }
            }
        }
        break;
    default:
        break;
    }


    // If 'per' is exactly at the last key time, use the last value
    if (per >= animate_params->key_times[animate_params->length - 1] || animate->end_frame)
    {
        t_x = animate_params->values[animate_params->length - 1];
        t_y = animate_params->values_2[animate_params->length - 1];

    }



    gui_img_translate(animate_params->img, t_x, t_y);
    // Optional: debug logging
//gui_log("img_opacity_cb: opacity set to %d\n", (int)(opacity * 255));
}
static void image_animate_callback(void *params, void *null, gui_animate_t *animate);
static void multi_animate_callback(void *params, void *null, gui_animate_t *animate)
{
    image_animate_params_t *animate_params = (image_animate_params_t *)params;
    //gui_log("multi_animate_callback:%x,%s\n", animate_params, animate_params->animate_type);
    if (!strcmp(animate_params->animate_type, "rotate"))
    {
        img_rotate_cb(params, null, animate);
    }
    else if (!strcmp(animate_params->animate_type, "translate"))
    {
        img_translate_cb(params, null, animate);
    }
    else if (!strcmp(animate_params->animate_type, "scale"))
    {
        img_scale_cb(params, null, animate);
    }
    else if (!strcmp(animate_params->animate_type, "opacity"))
    {
        img_opacity_cb(params, null, animate);
    }
    else if (!strcmp(animate_params->animate_type, "textContent"))
    {
        text_content_cb(params, null, animate);
    }
    else if (!strcmp(animate_params->animate_type, "progress"))
    {
        progress_content_cb(params, null, animate);
    }
    else if (!strcmp(animate_params->animate_type, "rotate_translate"))
    {
        img_rotate_cb(params, null, animate);
        img_translate_cb(params, null, animate);
    }
    else if (!strcmp(animate_params->animate_type, "rotate_scale"))
    {
        img_rotate_cb(params, null, animate);
        img_scale_cb(params, null, animate);
    }
    else if (!strcmp(animate_params->animate_type, "rotate_opacity"))
    {
        img_rotate_cb(params, null, animate);
        img_opacity_cb(params, null, animate);
    }
    else if (!strcmp(animate_params->animate_type, "translate_scale"))
    {
        img_translate_cb(params, null, animate);
        img_scale_cb(params, null, animate);
    }
    else if (!strcmp(animate_params->animate_type, "translate_opacity"))
    {
        img_translate_cb(params, null, animate);
        img_opacity_cb(params, null, animate);
    }
    else if (!strcmp(animate_params->animate_type, "scale_opacity"))
    {
        img_scale_cb(params, null, animate);
        img_opacity_cb(params, null, animate);
    }
    else if (!strcmp(animate_params->animate_type, "rotate_translate_scale"))
    {
        img_rotate_cb(params, null, animate);
        img_translate_cb(params, null, animate);
        img_scale_cb(params, null, animate);
    }
    else if (!strcmp(animate_params->animate_type, "rotate_translate_opacity"))
    {
        img_rotate_cb(params, null, animate);
        img_translate_cb(params, null, animate);
        img_opacity_cb(params, null, animate);
    }
    else if (!strcmp(animate_params->animate_type, "translate_scale_opacity"))
    {
        img_translate_cb(params, null, animate);
        img_scale_cb(params, null, animate);
        img_opacity_cb(params, null, animate);
    }
    else if (!strcmp(animate_params->animate_type, "rotate_translate_scale_opacity"))
    {
        img_rotate_cb(params, null, animate);
        img_translate_cb(params, null, animate);
        img_scale_cb(params, null, animate);
        img_opacity_cb(params, null, animate);
    }
    else if (!strcmp(animate_params->animate_type, "image frame"))
    {
        image_animate_callback(params, null, animate);
    }
}
static void multi_animate_key_times_callback(void *params, void *null, gui_animate_t *animate)
{
    image_animate_key_times_params_t *animate_params = (image_animate_key_times_params_t *)params;
    //gui_log("multi_animate_callback:%x,%s\n", animate_params, animate_params->animate_type);
    if (!strcmp(animate_params->animate_type, "rotate"))
    {
        //img_rotate_cb(params, null, animate);
    }
    else if (!strcmp(animate_params->animate_type, "translate"))
    {
        img_translate_key_times_cb(params, null, animate);
    }
    else if (!strcmp(animate_params->animate_type, "scale"))
    {
        //img_scale_cb(params, null, animate);
    }
    else if (!strcmp(animate_params->animate_type, "opacity"))
    {
        img_opacity_key_times_cb(params, null, animate);
    }
    else if (!strcmp(animate_params->animate_type, "image frame"))
    {
        //image_animate_callback(params, null, animate);
    }
}
static void image_animate_callback(void *params, void *null, gui_animate_t *animate)
{
    int current_image_index = 0;
    image_animate_params_t *animate_params = (image_animate_params_t *)params;

    if (animate_params->image_count > 0)
    {
        current_image_index = animate->progress_percent * (animate_params->image_count - 1);
        gui_img_set_attribute(animate_params->img, GET_BASE(animate_params->img)->name,
                              animate_params->image_arr[current_image_index], GET_BASE(animate_params->img)->x,
                              GET_BASE(animate_params->img)->y);
    }

}

#define LIGHTS_NUM 20
char light_value_text[30];
typedef struct
{
    int id;
    bool state;
    bool has_brightness;
    bool has_colorTemperature;
    int brightness;
    int colorTemperature;
    char *prefix;
    char *postfix;
    gui_text_t *text;
} light_param_t;

light_param_t lights[LIGHTS_NUM];

static char *read_file(const char *path)
{
    FILE *file = fopen(path, "r");
    if (!file)
    {
        perror("fopen");
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    if (length <= 0)
    {
        fclose(file);
        return 0;
    }
    char *content = (char *)gui_malloc(length + 1);
    //if (content)
    {
        size_t size = fread(content, 1, length, file);
        GUI_UNUSED(size);
        if (size == 0)
        {
            //gui_log("%d\n", size);
            gui_free(content);
            fclose(file);
            return 0;
        }


        content[length] = '\0';
    }
    fclose(file);
    return content;
}

static void parse_json(const char *json_str)
{
    const char *ptr = json_str;
    int index = 0;

    while ((ptr = strstr(ptr, "\"id\"")) != NULL && index < LIGHTS_NUM)
    {
        char state_buffer[10];
        bool has_brightness = false;
        bool has_colorTemperature = false;

        sscanf(ptr, "\"id\": %d", &lights[index].id);

        ptr = strstr(ptr, "\"state\"");
        if (!ptr)
        {
            return;
        }

        sscanf(ptr, "\"state\": \"%9[^\"]\"", state_buffer);
        lights[index].state = (strcmp(state_buffer, "on") == 0);

        const char *br_ptr = strstr(ptr, "\"brightness\"");
        if (br_ptr != NULL && (br_ptr - ptr) < (strchr(ptr, '}') - ptr))
        {
            sscanf(br_ptr, "\"brightness\": %d", &lights[index].brightness);
            has_brightness = true;
        }
        lights[index].has_brightness = has_brightness;

        const char *ct_ptr = strstr(ptr, "\"colorTemperature\"");
        if (ct_ptr != NULL && (ct_ptr - ptr) < (strchr(ptr, '}') - ptr))
        {
            sscanf(ct_ptr, "\"colorTemperature\": %d", &lights[index].colorTemperature);
            has_colorTemperature = true;
        }
        lights[index].has_colorTemperature = has_colorTemperature;

        index++;
    }
}

static void write_file(const char *filename, const char *content)
{
    FILE *file = fopen(filename, "w");
    if (file != NULL)
    {
        fwrite(content, 1, strlen(content), file);
        fclose(file);
    }
}

static void update_light_config(int index, const char *attribute, int value)
{
    char *config_file_path = "./realgui/example/web/peripheral_simulation/json/light_config.json";
    char *json_str = read_file(config_file_path);
    if (json_str)
    {
        parse_json(json_str);
        gui_free(json_str);
    }

    for (int i = 0; i < LIGHTS_NUM; i++)
    {
        if (lights[i].id == index)
        {
            if (strcmp(attribute, "state") == 0)
            {
                lights[i].state = value;
            }
            else if (strcmp(attribute, "brightness") == 0)
            {
                lights[i].brightness = value;
            }
            else if (strcmp(attribute, "colorTemperature") == 0)
            {
                lights[i].colorTemperature = value;
            }
            break;
        }
    }

    char config_str[2048] = "{\n    \"lights\": [\n";
    for (int i = 0; i < LIGHTS_NUM; i++)
    {
        char light_str[256];
        snprintf(light_str, sizeof(light_str),
                 "        {\"id\": %d, \"state\": \"%s\"",
                 lights[i].id, lights[i].state ? "on" : "off");
        strcat(config_str, light_str);

        if (lights[i].has_brightness)
        {
            char brightness_str[50];
            snprintf(brightness_str, sizeof(brightness_str), ", \"brightness\": %d", lights[i].brightness);
            strcat(config_str, brightness_str);
        }

        if (lights[i].has_colorTemperature)
        {
            char color_temp_str[50];
            snprintf(color_temp_str, sizeof(color_temp_str), ", \"colorTemperature\": %d",
                     lights[i].colorTemperature);
            strcat(config_str, color_temp_str);
        }
        strcat(config_str, (i < LIGHTS_NUM - 1) ? "},\n" : "}\n");
    }
    strcat(config_str, "    ]\n}");

    write_file(config_file_path, config_str);
}
static void xml_dom_wifi_entry(void *parameter)
{
    gui_log("try to connect %s %d\n", parameter, active_wifi_status);
    active_wifi_status = ACTIVE_WIFI_STATUS_CONNECTING;
    gui_thread_mdelay(3000);
    static int rand;
    uint8_t wifi_status[] = {ACTIVE_WIFI_STATUS_DISCONNECTED, ACTIVE_WIFI_STATUS_CONNECTED};
    active_wifi_status = wifi_status[(rand++) % (sizeof(wifi_status) / sizeof(wifi_status[0]))];
    if (active_wifi_status == ACTIVE_WIFI_STATUS_DISCONNECTED)
    {
        gui_log("DISCONNECT %s \n", (char *)parameter);
    }
    else
    {
        gui_log(" CONNECTED  %s \n", (char *)parameter);
    }


}
void gui_xml_dom_wifi_connect(uint8_t *status, const char *wifi_name)
{

    gui_thread_create("xml_dom_wifi",
                      xml_dom_wifi_entry, (void *)wifi_name,
                      1024 * 10,
                      15);


}
static void wifi_connect(void *obj, gui_event_t e, void *p)
{
    gui_xml_dom_wifi_connect(&active_wifi_status, wifi_array[active_wifi_index].name);
}
#define GET_WIDGET_BY_NAME(NAME, obj)\
    gui_obj_t *obj = 0;\
    gui_obj_tree_get_widget_by_name((gui_obj_t *)gui_current_app(), NAME, (gui_obj_t **)&obj);\
    if (!obj|| obj->magic!=GUI_MAGIC_NUMBER)\
    {\
        gui_log("GET_WIDGET_BY_NAME %s failed\n", (char *)NAME);\
    }
static char input_text_string[100];
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(text_input_win_ani)
{
    char *name = p;
    GET_WIDGET_BY_NAME(p, widget);
    if (!widget)
    {
        return;
    }
    gui_text_t *text = (void *)widget;


    if (animate->Beginning_frame)
    {
        if (animate->current_repeat_count == 0)
        {
            if ((!text->inputable) && (!text->ispasswd))
            {
                strncpy(input_text_string, text->content, sizeof(input_text_string) - 1);
                input_text_string[sizeof(input_text_string) - 1] = '\0';
            }
            text->inputable = 1;
        }
        else if (!text->inputable)
        {
            animate->animate = 0;
            return;
        }

        char end = '|';
        if ((animate->current_repeat_count % 2))
        {
            end = ' ';
        }
        gui_free(text->content);
        text->content = gui_malloc(strlen(input_text_string) + 2);
        if (!text->ispasswd)
        {
            sprintf(text->content, "%s%c", input_text_string, end);
        }
        else
        {
            char head[strlen(input_text_string) + 1];
            memset(head, '*', strlen(input_text_string));
            head[strlen(input_text_string)] = '\0';
            sprintf(text->content, "%s%c", head, end);
        }


        gui_text_content_set(text, text->content, strlen(text->content));
        //gui_log("%d,%d,%c,%s\n", animate->current_repeat_count, animate->current_repeat_count%2,end, text->content);
    }




}
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(text_no_input_win_ani)
{
    char *name = p;
    GET_WIDGET_BY_NAME(p, widget);
    if (!widget)
    {
        return;
    }
    gui_text_t *text = (void *)widget;

    if (text->inputable)
    {
        text->inputable = 0;
        gui_free(text->content);
        text->content = gui_strdup((input_text_string));
        gui_text_content_set(text, text->content, strlen(text->content));
        animate->animate = 0;
    }





}
static GUI_EVENT_CALLBACK_FUNCTION_DEFINE(text_input)
{
    if (GUI_BASE(obj)->type == WINDOW)
    {
        gui_win_set_animate(obj, 300, -1, text_input_win_ani, param);
    }
    else if (GUI_BASE(obj)->type == ICON)
    {
        GUI_API(gui_button_t).animate(obj, 300, -1, text_input_win_ani, param);
    }



}
static GUI_EVENT_CALLBACK_FUNCTION_DEFINE(text_no_input)
{
    gui_obj_t *objj = obj    ;
    if (GUI_BASE(obj)->type == WINDOW)
    {
        gui_win_set_animate(obj, 300, -1, text_no_input_win_ani, param);
    }
    else if (GUI_BASE(obj)->type == BUTTON)
    {
        GUI_API(gui_button_t).animate(obj, 300, -1, text_no_input_win_ani, param);
    }



}
static GUI_EVENT_CALLBACK_FUNCTION_DEFINE(on_on_text_password)
{
    char *name = param;
    GET_WIDGET_BY_NAME(name, widget);
    if (!widget)
    {
        return;
    }
    gui_text_t *text = (void *)widget;
    text->ispasswd = true;
    if (!text->inputable)
    {
        strncpy(input_text_string, text->content, sizeof(input_text_string) - 1);
        input_text_string[sizeof(input_text_string) - 1] = '\0';
        char head[strlen(input_text_string) + 1];
        memset(head, '*', strlen(input_text_string));
        head[strlen(input_text_string)] = '\0';
        sprintf(text->content, "%s", head);
        gui_text_content_set(text, text->content, strlen(text->content));
    }




}
static GUI_EVENT_CALLBACK_FUNCTION_DEFINE(on_off_text_text)
{
    char *name = param;
    GET_WIDGET_BY_NAME(name, widget);
    if (!widget)
    {
        return;
    }
    gui_text_t *text = (void *)widget;

    if (!text->inputable && text->ispasswd)
    {


        sprintf(text->content, "%s", input_text_string);
        gui_text_content_set(text, text->content, strlen(text->content));
    }
    text->ispasswd = false;

}
static void light_control_cb(void *obj, gui_event_t e, light_param_t *light)
{
    gui_log("light->id: %d, light->state: %d\n", light->id, light->state);
    update_light_config(light->id, "state", light->state);
}

static void light_switch_on_cb(void *obj, gui_event_t e, light_param_t *light)
{
    light->state = true;
    gui_log("light->id: %d, light->state: %d\n", light->id, light->state);
    update_light_config(light->id, "state", light->state);
}

static void light_switch_off_cb(void *obj, gui_event_t e, light_param_t *light)
{
    light->state = false;
    gui_log("light->id: %d, light->state: %d\n", light->id, light->state);
    update_light_config(light->id, "state", light->state);
}

static void light_brightness_cb(void *obj, gui_event_t e, light_param_t *light)
{
    gui_seekbar_t *this = (gui_seekbar_t *)obj;
    light->brightness = 100 * GUI_API(gui_seekbar_t).get_progress(this);
    gui_log("light->id: %d  light->brightness: %d\n", light->id, light->brightness);
    update_light_config(light->id, "brightness", light->brightness);
}

static void light_colorTemp_cb(void *obj, gui_event_t e, light_param_t *light)
{
    gui_slider_t *this = (gui_slider_t *)obj;
    light->colorTemperature = GUI_API(gui_slider_t).get_currentValue(this);
    update_light_config(light->id,  "colorTemperature", light->colorTemperature);
}
static void slider_write_text_cb(void *obj, gui_event_t e, char *text_name)
{
    gui_slider_t *this = (gui_slider_t *)obj;
    gui_text_t *text = 0;
    gui_obj_tree_get_widget_by_type((gui_obj_t *)gui_current_app(), TEXTBOX, (gui_obj_t **)&text);

    snprintf(light_value_text, sizeof(light_value_text), "%d",
             GUI_API(gui_slider_t).get_currentValue(this));
    gui_text_content_set(text, light_value_text, strlen(light_value_text));
}
static void seekbar_write_text_cb(void *obj, gui_event_t e, char *text_name)
{
    gui_seekbar_t *this = (gui_seekbar_t *)obj;
    gui_text_t *text = 0;
    gui_obj_tree_get_widget_by_name((gui_obj_t *)gui_current_app(), text_name, (gui_obj_t **)&text);
    if (!text)
    {
        return;
    }

    gui_free(text->content);
    text->content = gui_malloc(5);
    memset(text->content, 0, 5);
    snprintf(text->content, 5, "%d",
             (int)(GUI_API(gui_seekbar_t).get_progress(this) * 100 + 0.5f));
    gui_text_content_set(text, text->content, strlen(text->content));
    gui_log("text->content:%s\n", (char *)(text->content));
}
static char *open_switch_name;
static char *pause_switch_name;
static char *close_switch_name;
static void create_tree_in_multi_level(gui_app_t *app, gui_multi_level_t *parent);
static void setting_return_cb(void *obj, gui_event_t e, void *param);
static void **image_array;
static void *return_image;
static void *return_image_hl;
static int file_count;


static void seekbar_ani_cb(int args, gui_seekbar_t *this)
{
    switch (args)
    {
    case 0:
        {
            GUI_API(gui_seekbar_t).set_progress(this, this->animate->progress_percent);
        }
        break;
    case 1:
        {
            GUI_API(gui_seekbar_t).set_progress(this, 1.0f - this->animate->progress_percent);
        }
        break;
    default:
        break;
    }
}
static void switch_cb_for_mororized_curtain(gui_switch_t *sw, gui_event_t event_code)
{
    size_t args = 0;
    gui_event_dsc_t *event = GUI_BASE(sw)->event_dsc;

    // same event only handle the first register one
    for (size_t i = 0; i < GUI_BASE(sw)->event_dsc_cnt; i++)
    {
        if (event[i].filter == event_code)
        {
            args = (size_t)event[i].user_data;
        }
    }
    gui_seekbar_t *seekbar = 0;
    gui_obj_tree_get_widget_by_type(GUI_BASE(sw)->parent, SEEKBAR, (gui_obj_t **)&seekbar);
    switch (args)
    {
    case 3:
        {
            if (seekbar->animate)
            {
                seekbar->animate->animate = 1;
            }
        }
        break;
    case 2:
        {
            if (seekbar->animate)
            {
                seekbar->animate->animate = 0;
            }


        }
        break;
    default:
        {
            GUI_API(gui_seekbar_t).animate(seekbar, 10000, 0, seekbar_ani_cb, (void *)args);
        }
        break;
    }
}
static gui_img_t *xml_gui_img_create_from_mem(void *parent,  const char *name, void *addr,
                                              int16_t x,
                                              int16_t y)
{
    if (addr == NULL)
    {
        addr = gui_get_file_address("app/system/resource/icMenuBird.bin");
    }

    return gui_img_create_from_mem(parent, name, addr,
                                   x,
                                   y, 0, 0);
}

static char *get_space_string_head(const char *string)
{
    char *s = gui_strdup(string);
    char *rst = 0;
    if (s == 0)
    {
        return rst;
    }
    size_t length = strlen(s);
    for (int i = 0; i <= length - 1; i++)
    {
        if (!((s[i] >= 32) && (s[i] <= 126)))
        {
            s[i] = '\0';
            break;
        };
    }
    if (strlen(s) > 0)
    {
        rst = gui_strdup(s);
    }
    gui_free(s);
    return rst;
}
static char *js;



static gui_color_t string_rgb888(const char *color)
{
    return gui_color_css(color);
}
static void sport_button_press_ani_cb(gui_button_t *button)
{
    GUI_UNUSED(js);
    float per = (button->animate->progress_percent);
    float alpha_to = 100;
    static float alpha_from = 0;
    static float from = 0;
    if (per == 0.0f)
    {
        from = gui_img_get_transform_scale_x((gui_img_t *)(button->img));
        alpha_from = button->img->draw_img->opacity_value;
    }
    from = 1.0f;
    float scale = (-0.5f * from) * per + from;
    //circle_color_tab[1] = change_opacity_of_rgba(circle_color_tab[1],
    //                                             (uint8_t)(int)((alpha_to - alpha_from) * per + alpha_from));
    if (button->style == WIDGET_FADE || button->style == WIDGET_SCALE_FADE)
    {
        gui_img_set_opacity((void *)button->img,
                            (uint8_t)(int)((alpha_to - alpha_from) * per + alpha_from));
    }
    if (button->style == WIDGET_SCALE || button->style == WIDGET_SCALE_FADE)
    {
        gui_img_scale((void *)button->img, scale,  scale);
        //gui_img_translate((void *)button->img, 25.0f * (from - scale),  25.0f * (from - scale));
    }



    //gui_log("o:%f\n", 25.0f * (from - scale));
}
static void reverse_array(void *arr[], int n)
{
    void *temp;
    for (int i = 0; i < n / 2; i++)
    {
        temp = arr[i];
        arr[i] = arr[n - i - 1];
        arr[n - i - 1] = temp;
    }
}

static void button_press_ani_cb_array(int p, gui_button_t *button)
{

    if (p == -1)
    {
        button->img->data = button->off_pic_addr;
    }
    else if (button->style == BUTTON_HIGHLIGHT_ARRAY)
    {
        float per = button->animate->progress_percent;
        void **image_array = (void **)button->data;
        button->img->data = image_array[(int)((p - 1) * per)];
    }
}
static void switch_press_ani_cb_array(int p, gui_switch_t *button)
{

    if (p == -1)
    {
        button->switch_picture->data = button->off_pic_addr;
    }
    else if (button->style == SWITCH_HIGHLIGHT_ARRAY)
    {
        float per = button->animate->progress_percent;
        void **image_array = (void **)button->data;
        button->switch_picture->data = image_array[(int)((p - 1) * per)];
    }
}
static void sport_button_release_ani_cb(gui_button_t *button)
{
    float per = (button->animate->progress_percent);
    float alpha_to = UINT8_MAX;
    static float alpha_from = 0;
    static float from = 0;
    if (per == 0.0f)
    {
        from = gui_img_get_transform_scale_x((gui_img_t *)(button->img));
        alpha_from = button->img->draw_img->opacity_value;
    }
    //from = 1.25f;
    float scale = (1.0f - from) * per + from;
    //circle_color_tab[1] = change_opacity_of_rgba(circle_color_tab[1],
    //                                             (uint8_t)(int)((alpha_to - alpha_from) * per + alpha_from));


    //gui_img_translate((void *)button->img, 25.0f * (1.0f - scale),  25.0f * (1.0f - scale));
    if (button->style == WIDGET_FADE || button->style == WIDGET_SCALE_FADE)
    {
        gui_img_set_opacity((void *)button->img,
                            (uint8_t)(int)((alpha_to - alpha_from) * per + alpha_from));
    }
    if (button->style == WIDGET_SCALE || button->style == WIDGET_SCALE_FADE)
    {
        gui_img_scale((void *)button->img, scale,  scale);
    }
}
static void sport_button_press(gui_button_t *b)
{
    gui_button_set_animate(b, 200, 0, sport_button_press_ani_cb, b);
}
static void button_press_array(gui_button_t *b, void *code, int p)
{
    GUI_API(gui_button_t).animate(b, 200, 0, button_press_ani_cb_array, (void *)(uintptr_t)p);
}
static void switch_press_array(gui_switch_t *b, void *code, int p)
{
    if (!b->ifon)
    {
        GUI_API(gui_switch_t).animate(b, b->animate->dur, 0, switch_press_ani_cb_array,
                                      (void *)(uintptr_t)p);
    }


}

static void sport_button_release(gui_button_t *b)
{
    gui_button_set_animate(b, 200, 0, sport_button_release_ani_cb, b);
}
static void multi_level_ui_design(gui_multi_level_t *obj)
{
    create_tree_in_multi_level(gui_current_app(), obj);
}


static void on_click_jump_cb(void *obj, gui_event_t e, struct on_click_jump_cb_param *param)
{
    if (param->to_widget_name)
    {
        gui_multi_level_t *ml = gui_multi_level_get_group(param->to_widget_name);
        if (!ml)
        {
            return;
        }

        if (param->id1 < 0)
        {
            setting_return_cb(obj, e, (void *)param);
        }
        else
        {
            GUI_API(gui_multi_level_t).jump((void *)ml, param->id1, param->id2);
        }
    }
    else
    {
        GUI_WIDGET_POINTER_BY_TYPE(ml, MULTI_LEVEL, &(gui_current_app()->screen));
        // if (!ml)
        // {
        //     return;
        // }
        if (param->id1 < 0)
        {
            setting_return_cb(obj, e, (void *)param);
        }
        else
        {
            GUI_API(gui_multi_level_t).jump((void *)ml, param->id1, param->id2);
        }
    }
}
static void on_connect_wifi_listview_cb(void *obj, gui_event_t e, const char *listview_name)
{
    if (listview_name)
    {
        gui_obj_t *to = 0;
        gui_obj_tree_get_widget_by_name((void *)gui_current_app(), listview_name, &to);
        if (!to)
        {
            return;
        }


    }
    else
    {
        GUI_WIDGET_POINTER_BY_TYPE(ml, MACRO_PAGE_LIST_NEW, &(gui_current_app()->screen));





    }
}
static void on_click_jump_cb_wifi(void *obj, gui_event_t e, struct on_click_jump_cb_param *param)
{
    active_wifi_index = param->id;
    active_wifi_status = ACTIVE_WIFI_STATUS_DISABLECONNECT;
    if (param->to_widget_name)
    {
        gui_multi_level_t *ml = gui_multi_level_get_group(param->to_widget_name);
        if (param->id1 < 0)
        {
            setting_return_cb(obj, e, (void *)param);
        }
        else
        {
            GUI_API(gui_multi_level_t).jump((void *)ml, param->id1, param->id2);
        }
    }
    else
    {
        GUI_WIDGET_POINTER_BY_TYPE(ml, MULTI_LEVEL, &(gui_current_app()->screen));
        if (param->id1 < 0)
        {
            setting_return_cb(obj, e, (void *)param);
        }
        else
        {
            GUI_API(gui_multi_level_t).jump((void *)ml, param->id1, param->id2);
        }
    }
}
static void on_click_keyboard_cb(void *obj, gui_event_t e, struct on_click_keyboard_cb_param *param)
{
    if (param->to_text && param->to_text->base.type == TEXTBOX &&
        param->to_text->base.magic == GUI_MAGIC_NUMBER)
    {
        if (param->to_text->inputable || param->to_text->ispasswd)
        {
            gui_free(param->to_text->content);
            param->to_text->content = gui_strdup(input_text_string);
            gui_text_content_set(param->to_text, param->to_text->content, strlen(param->to_text->content));
        }
        char *content = param->to_text->content;

        if (!content)
        {
            content = "";
        }
        if (param->key_board >= SCANCODE_1 && param->key_board < SCANCODE_0)
        {
            char string[strlen(content) + 2];
            sprintf(string, "%s%c", content, '1' + param->key_board - SCANCODE_1);
            gui_log("%s\n", string);
            gui_free(content);
            content  = gui_strdup(string);
        }
        else if (param->key_board == SCANCODE_0)
        {
            char string[strlen(content) + 2];
            sprintf(string, "%s%c", content, '0');
            gui_log("%s\n", string);
            gui_free(content);
            content  = gui_strdup(string);
        }
        else if (param->key_board >= SCANCODE_A && param->key_board <= SCANCODE_Z)
        {
            char string[strlen(content) + 2];
            char c = 'a' + param->key_board - SCANCODE_A;
            if (shift)
            {
                c = toupper(c);
            }
            sprintf(string, "%s%c", content, c);
            gui_log("%s\n", string);
            gui_free(content);
            content  = gui_strdup(string);
        }
        else if (param->key_board == SCANCODE_BACKSPACE)
        {
            size_t new_length = strlen(content) - 1;
            if (new_length > 0)
            {
                content[new_length] = '\0'; // Remove the last character
            }
            else
            {
                content[0] = '\0'; // If length is zero, just set it to an empty string
            }
        }
        else if (param->key_board == SCANCODE_SPACE)
        {
            char string[strlen(content) + 2];
            sprintf(string, "%s ", content);
            gui_log("%s\n", string);
            gui_free(content);
            content  = gui_strdup(string);
        }
        else if (param->key_board == SCANCODE_LSHIFT || param->key_board == SCANCODE_RSHIFT)
        {
            // toggle shift
            if (shift)
            {
                shift = false;
            }
            else
            {
                shift = true;
            }
        }

        if (param->to_text->inputable)
        {
            strcpy(input_text_string, content);
        }
        if (param->to_text->ispasswd)
        {
            strcpy(input_text_string, content);
            size_t len = strlen(content);
            char *stars = gui_malloc(len + 1);
            memset(stars, '*', len);
            stars[len] = '\0';
            gui_free(content);
            content = stars;
        }

        gui_text_content_set(param->to_text, content, strlen(content));
    }

}
static void on_click_keyboard_cb_switch_turn_on(void *obj, gui_event_t e,
                                                struct on_click_keyboard_cb_param *param)
{
    if (param->to_text && param->to_text->base.type == TEXTBOX &&
        param->to_text->base.magic == GUI_MAGIC_NUMBER)
    {

        char *content = param->to_text->content;
        if (!content)
        {
            content = "";
        }
        if (param->key_board >= SCANCODE_1 && param->key_board < SCANCODE_0)
        {
            char string[strlen(content) + 2];
            sprintf(string, "%s%c", content, '1' + param->key_board - SCANCODE_1);
            gui_log("%s\n", string);
            gui_free(content);
            content  = gui_strdup(string);
        }
        else if (param->key_board == SCANCODE_0)
        {
            char string[strlen(content) + 2];
            sprintf(string, "%s%c", content, '0');
            gui_log("%s\n", string);
            gui_free(content);
            content  = gui_strdup(string);
        }
        else if (param->key_board >= SCANCODE_A && param->key_board <= SCANCODE_Z)
        {
            char string[strlen(content) + 2];
            char c = 'a' + param->key_board - SCANCODE_A;
            if (shift)
            {
                c = toupper(c);
            }
            sprintf(string, "%s%c", content, c);
            gui_log("%s\n", string);
            gui_free(content);
            content  = gui_strdup(string);
        }
        else if (param->key_board == SCANCODE_BACKSPACE)
        {
            size_t new_length = strlen(content) - 1;
            if (new_length > 0)
            {
                content[new_length] = '\0'; // Remove the last character
            }
            else
            {
                content[0] = '\0'; // If length is zero, just set it to an empty string
            }
        }
        else if (param->key_board == SCANCODE_SPACE)
        {
            char string[strlen(content) + 2];
            sprintf(string, "%s ", content);
            gui_log("%s\n", string);
            gui_free(content);
            content  = gui_strdup(string);
        }
        else if (param->key_board == SCANCODE_LSHIFT || param->key_board == SCANCODE_RSHIFT)
        {
            // toggle shift
            // if (shift)
            // {
            //     shift = false;
            // }
            // else
            {
                shift = true;
            }
        }


        gui_text_content_set(param->to_text, content, strlen(content));
    }

}
static void on_click_keyboard_cb_switch_turn_off(void *obj, gui_event_t e,
                                                 struct on_click_keyboard_cb_param *param)
{
    if (param->to_text && param->to_text->base.type == TEXTBOX &&
        param->to_text->base.magic == GUI_MAGIC_NUMBER)
    {

        char *content = param->to_text->content;
        if (!content)
        {
            content = "";
        }
        if (param->key_board >= SCANCODE_1 && param->key_board < SCANCODE_0)
        {
            char string[strlen(content) + 2];
            sprintf(string, "%s%c", content, '1' + param->key_board - SCANCODE_1);
            gui_log("%s\n", string);
            gui_free(content);
            content  = gui_strdup(string);
        }
        else if (param->key_board == SCANCODE_0)
        {
            char string[strlen(content) + 2];
            sprintf(string, "%s%c", content, '0');
            gui_log("%s\n", string);
            gui_free(content);
            content  = gui_strdup(string);
        }
        else if (param->key_board >= SCANCODE_A && param->key_board <= SCANCODE_Z)
        {
            char string[strlen(content) + 2];
            char c = 'a' + param->key_board - SCANCODE_A;
            if (shift)
            {
                c = toupper(c);
            }
            sprintf(string, "%s%c", content, c);
            gui_log("%s\n", string);
            gui_free(content);
            content  = gui_strdup(string);
        }
        else if (param->key_board == SCANCODE_BACKSPACE)
        {
            size_t new_length = strlen(content) - 1;
            if (new_length > 0)
            {
                content[new_length] = '\0'; // Remove the last character
            }
            else
            {
                content[0] = '\0'; // If length is zero, just set it to an empty string
            }
        }
        else if (param->key_board == SCANCODE_SPACE)
        {
            char string[strlen(content) + 2];
            sprintf(string, "%s ", content);
            gui_log("%s\n", string);
            gui_free(content);
            content  = gui_strdup(string);
        }
        else if (param->key_board == SCANCODE_LSHIFT || param->key_board == SCANCODE_RSHIFT)
        {
            // toggle shift
            // if (shift)
            {
                shift = false;
            }
            // else
            // {
            //     shift = true;
            // }
        }


        gui_text_content_set(param->to_text, content, strlen(content));
    }

}

static void on_click_jump_cb_tabview(void *obj, gui_event_t e, struct on_click_jump_cb_param *param)
{
    if (param->to_widget_name)
    {
        GUI_WIDGET_POINTER_BY_NAME(tabview, param->to_widget_name);
        gui_tabview_jump_tab((void *)tabview, param->id1, param->id2);
    }
    else
    {
        GUI_WIDGET_POINTER_BY_TYPE(tabview, TABVIEW, &(gui_current_app()->screen));
        gui_tabview_jump_tab((void *)tabview, param->id1, param->id2);
    }



}
static void on_click_jump_to_app_cb(void *obj, gui_event_t e, const char *new_xml)
{
    if (new_xml)
    {
        extern void *get_app_xml(void);
        gui_app_t *app2 = (gui_app_t *)get_app_xml();
        app2->xml = new_xml;
        gui_app_switch(gui_current_app(), app2);
    }
}
static void on_click_jump_to_capp_cb(void *obj, gui_event_t e, gui_app_t *handle)
{
    if (handle)
    {
        gui_app_switch(gui_current_app(), handle);
    }
}
static gui_obj_t *widget_create_textbox(ezxml_t p, gui_obj_t *parent, T_OBJ_TYPE widget_type)
{
    {
        size_t i = 0;
        int16_t x = 0;
        int16_t y = 0;
        int16_t w = gui_get_screen_width();
        int16_t h = 50;
        const char *text = "text";

        char *font = 0;

        gui_color_t color = APP_COLOR_WHITE;
        int fontSize = 0;
        TEXT_MODE style = (TEXT_MODE)0;
        uint8_t inputable = false;
        while (true)
        {
            if (!(p->attr[i]))
            {
                break;
            }
            ////gui_log("p->attr[i]:%s\n", p->attr[i]);
            if (!strcmp(p->attr[i], "x"))
            {
                x = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "y"))
            {
                y = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "w"))
            {
                uint32_t w_temp = atoi(p->attr[++i]);
                if (w_temp)
                {
                    w = w_temp;
                }
            }
            else if (!strcmp(p->attr[i], "h"))
            {
                uint32_t w_temp = atoi(p->attr[++i]);
                if (w_temp)
                {
                    h = w_temp;
                }
            }
            else if (!strcmp(p->attr[i], "text"))
            {
                text = p->attr[++i];
            }
            else if (!strcmp(p->attr[i], "font"))
            {
                font = p->attr[++i];
            }
            else if (!strcmp(p->attr[i], "fontSize"))
            {
                fontSize = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "color"))
            {
                color = string_rgb888(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "mode"))
            {
                char *s = p->attr[++i];
                if (!strcmp(p->attr[i], "truncate"))
                {
                    style = (TEXT_MODE)0;
                }
                else if (!strcmp(p->attr[i], "verticalscroll"))
                {
                    style = SCROLL_Y;
                }
                else if (!strcmp(p->attr[i], "left"))
                {
                    style = LEFT;
                }
                else if (!strcmp(p->attr[i], "center"))
                {
                    style = CENTER;
                }
                else if (!strcmp(p->attr[i], "right"))
                {
                    style = RIGHT;
                }
            }
            else if (!strcmp(p->attr[i], "inputable"))
            {
                inputable = atoi(p->attr[++i]);
            }
            i++;
        }
        //gui_log("x:%d,y:%d,w:%dh:%d,font:%s,text:%s,color:%x\n", x, y, w, h, font, text, color);
        char *ptxt = get_space_string_head(p->txt);
        //gui_log("p->txt2 = %s,\n", ptxt);

        if (text)
        {
            gui_text_t *t = 0;
            if (font == 0 ||
                fontSize == 0 ||
                gui_get_file_address(font) == 0)
            {
                text = "Font Export failed! Try again.";
                fontSize = 16;
                w = gui_get_screen_width();
                font = "app/system/resource/font/arialbd_size16_bits4_font.bin";
            }
            if (style == 0 || style == CENTER || style == RIGHT || style == LEFT)
            {
                t = gui_text_create(parent, ptxt, x, y, w, h);
                GUI_BASE(t)->obj_cb = gui_xml_dom_text_cb;
                gui_color_t color_temporary;
                color_temporary = color;
                gui_text_set(t, gui_strdup(text), GUI_FONT_SRC_BMP, color_temporary, strlen(text), 32);
                if (inputable)
                {
                    gui_text_input_set(t, inputable);
                    gui_text_click(t, gui_keyboard_launch_by_widget, t);
                }


                {
                    FONT_SRC_TYPE font_type2; GUI_UNUSED(font_type2);
                    char *font_type = font;
                    if (strstr(font_type, ".bin") != NULL)
                    {
                        font_type2 = GUI_FONT_SRC_BMP;
                        void *addr1 = gui_get_file_address(font_type);
                        //gui_font_mem_init(addr1);
                        t->font_height = fontSize;
                        t->path = 0;
                        t->font_type = GUI_FONT_SRC_BMP;
                        gui_text_type_set(t, addr1, FONT_SRC_MEMADDR);
                        gui_text_mode_set(t, style);
                        // t->font_height = fontSize;
                        //t->path = 0;
                    }
                    else if ((strstr(font_type, ".ttf") != NULL) || (strstr(font_type, ".TTF") != NULL))
                    {
#ifdef __WIN32
                        font_type2 = GUI_FONT_SRC_TTF;
                        t->path = gui_get_file_address(font);
                        t->font_height = fontSize;
                        t->font_type = GUI_FONT_SRC_TTF;
#else
                        font_type =
                            "app/system/resource/font/gbk_32_32_dot.bin;app/system/resource/font/gbk_unicode_table.bin";
                        {
                            font_type2 = GUI_FONT_SRC_BMP;
                            char b[100] = {0};
                            strncpy(b, font_type, strstr(font_type, ".bin;") - font_type + strlen(".bin"));
                            void *addr1 = gui_get_file_address(b);
                            memset(b, 0, sizeof(b));
                            char *a = font_type;
                            strncpy(b, strstr(a, ".bin;") + strlen(".bin;"), strlen(a) - (strstr(a,
                                                                                                 ".bin;") - a + strlen(".bin;")));
                            void *addr2 = gui_get_file_address(b);
                            // gui_set_font_mem_resourse(fontSize, addr1,  addr2);
                            t->path = 0;
                            t->font_type = GUI_FONT_SRC_BMP;
                        }
#endif
                    }

                }
            }
            else
            {
                gui_scroll_text_t *scroll_text = gui_scroll_text_create(parent,  ptxt, x, y,
                                                                        strlen(text) * (fontSize / 4), h);
                gui_scroll_text_scroll_set(scroll_text, style, 100, 0, 3000, 1500000);
                gui_color_t color_temporary;
                color_temporary = color;
                gui_scroll_text_set(scroll_text, gui_strdup(text), GUI_FONT_SRC_TTF, color_temporary,
                                    strlen(text),
                                    fontSize);
                t = (void *)scroll_text;
            }
            parent = (void *)t;
        }
    }
    return parent;
}
static gui_obj_t *widget_create_window(ezxml_t p, gui_obj_t *parent, T_OBJ_TYPE widget_type)
{

    {
        size_t i = 0;
        int16_t x = 0;
        int16_t y = 0;
        int16_t w = 0;
        int16_t h = 0;
        bool hide = false;
        while (true)
        {
            if (!(p->attr[i]))
            {
                break;
            }
            ////gui_log("p->attr[i]:%s\n", p->attr[i]);
            if (!strcmp(p->attr[i], "x"))
            {
                x = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "y"))
            {
                y = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "w"))
            {
                w = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "h"))
            {
                h = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "hidden"))
            {
                hide = true;
            }
            i++;
        }
        char *ptxt = get_space_string_head(p->txt);
        //gui_log("x:%d,y:%d,w:%dh:%d\n", x, y, w, h);
        {
            parent = (void *)gui_win_create(parent, ptxt, x, y, w, h);
            if (hide)
            {
                parent->not_show = 1;
            }


        }
    }

    return parent;
}
static gui_obj_t *widget_create_rectangle(ezxml_t p, gui_obj_t *parent, T_OBJ_TYPE widget_type)
{

    {
        size_t i = 0;
        int16_t x = 0;
        int16_t y = 0;
        int16_t w = 0;
        int16_t h = 0;
        gui_color_t color = APP_COLOR_WHITE;
        uint8_t opacity = 255;
        while (true)
        {
            if (!(p->attr[i]))
            {
                break;
            }
            ////gui_log("p->attr[i]:%s\n", p->attr[i]);
            if (!strcmp(p->attr[i], "x"))
            {
                x = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "y"))
            {
                y = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "w"))
            {
                w = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "h"))
            {
                h = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "color"))
            {
                color = string_rgb888(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "opacity"))
            {
                opacity = atof(p->attr[++i]);
            }
            i++;
        }
        char *ptxt = get_space_string_head(p->txt);
        //gui_log("x:%d,y:%d,w:%dh:%d\n", x, y, w, h);
        {
            if (opacity != 255)
            {
                color.color.rgba.a = opacity;
            }

            parent = (void *)gui_canvas_rect_create(parent, ptxt, x, y, w, h, color);
        }
    }
    return parent;
}
static gui_obj_t *widget_create_tabview(ezxml_t p, gui_obj_t *parent, T_OBJ_TYPE widget_type)
{

    {
        size_t i = 0;
        int16_t x = 0;
        int16_t y = 0;
        int16_t w = 0;
        int16_t h = 0;
        int style = 0; GUI_UNUSED(style);
        bool gestures = true;
        while (true)
        {
            if (!(p->attr[i]))
            {
                break;
            }
            ////gui_log("p->attr[i]:%s\n", p->attr[i]);
            if (!strcmp(p->attr[i], "x"))
            {
                x = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "y"))
            {
                y = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "w"))
            {
                w = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "h"))
            {
                h = atoi(p->attr[++i]);
            }
            // else if (!strcmp(p->attr[i], "transition"))
            // {
            //     char *s = p->attr[++i];
            //     if (!strcmp(p->attr[i], "normal"))
            //     {
            //         style = CLASSIC;
            //     }
            //     else if (!strcmp(p->attr[i], "fade"))
            //     {
            //         style = FADE;
            //     }
            //     else if (!strcmp(p->attr[i], "scale"))
            //     {
            //         style = REDUCTION;
            //     }
            //     else if (!strcmp(p->attr[i], "fadeScale"))
            //     {
            //         style = REDUCTION_FADE;
            //     }

            // }
            else if (!strcmp(p->attr[i], "gestures"))
            {
                ++i;
                if (!strcmp(p->attr[i], "true"))
                {
                    gestures = true;
                }
                else if (!strcmp(p->attr[i], "false"))
                {
                    gestures = false;
                }

            }
            i++;
        }
        char *ptxt = get_space_string_head(p->txt);
        parent = (void *)gui_tabview_create(parent, ptxt, x, y, w, h);
        gui_tabview_loop_x((void *)parent, true);
        GUI_TYPE(gui_tabview_t, parent)->tp_disable = !gestures;
    }

    return parent;
}
static gui_obj_t *widget_create_macro_canvas_arc(ezxml_t p, gui_obj_t *parent,
                                                 T_OBJ_TYPE widget_type)
{

    {
        size_t i = 0;
        int16_t cx = 100; GUI_UNUSED(cx);
        int16_t cy = 100; GUI_UNUSED(cy);
        int16_t r = 100; GUI_UNUSED(r);
        int16_t stroke_width = 10; GUI_UNUSED(stroke_width);
        int cap = 0; GUI_UNUSED(cap);
        gui_color_t stroke = APP_COLOR_RED;
        float sd = 0; GUI_UNUSED(sd);
        float ed = 100; GUI_UNUSED(ed);
        int dir = NVG_CW;
        while (true)
        {
            if (!(p->attr[i]))
            {
                break;
            }
            ////gui_log("p->attr[i]:%s\n", p->attr[i]);
            if (!strcmp(p->attr[i], "cx") || !strcmp(p->attr[i], "centralX") || !strcmp(p->attr[i], "centerX"))
            {
                cx = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "cy") || !strcmp(p->attr[i], "centerY") ||
                     !strcmp(p->attr[i], "centralY"))
            {
                cy = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "r") || !strcmp(p->attr[i], "radius"))
            {
                r = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "startDegree") || !strcmp(p->attr[i], "startAngle"))
            {
                sd = atof(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "endDegree") || !strcmp(p->attr[i], "endAngle"))
            {
                ed = atof(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "strokeWidth") || !strcmp(p->attr[i], "stroke-width"))
            {
                stroke_width = atoi(p->attr[++i]);
            }
            // else if (!strcmp(p->attr[i], "stroke-linecap") || !strcmp(p->attr[i], "capMode") ||
            //          !strcmp(p->attr[i], "cap"))
            // {
            //     char *s = p->attr[++i];
            //     if (!strcmp(p->attr[i], "butt"))
            //     {
            //         cap = NVG_BUTT;
            //     }
            //     else if (!strcmp(p->attr[i], "round"))
            //     {
            //         cap = NVG_ROUND;
            //     }
            //     else if (!strcmp(p->attr[i], "square"))
            //     {
            //         cap = NVG_SQUARE;
            //     }
            // }
            else if (!strcmp(p->attr[i], "strokeColor") || !strcmp(p->attr[i], "color"))
            {
                stroke = string_rgb888(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "direction") || !strcmp(p->attr[i], "clockwise"))
            {
                if (!strcmp(p->attr[++i], "CCW") || !strcmp(p->attr[i], "count clockwise"))
                {
                    dir = NVG_CCW;
                }
                else if (!strcmp(p->attr[i], "CW") || !strcmp(p->attr[i], "clockwise"))
                {
                    dir = NVG_CW;
                }

            }
            i++;
        }
        char *ptxt = get_space_string_head(p->txt);
        //arc animation by user's buffer
        {
            int image_h = r * 2 + stroke_width * 2,
                image_w = image_h,
                pixel_bytes = 4,
                buffer_size = image_h * image_w * pixel_bytes + sizeof(gui_rgb_data_head_t);
            uint8_t *imgdata = gui_lower_malloc(buffer_size);
            memset(imgdata, 0, buffer_size);
            int format = GUI_CANVAS_OUTPUT_RGBA;  int image_width = image_w;
            int image_height = image_h;  uint8_t *target_buffer = imgdata;
            {
                int pixel_length = 4;
                int data_length = 0;
                uint8_t *buffer = 0;
                uint8_t *output_data = 0;
                switch (format)
                {
                case GUI_CANVAS_OUTPUT_PNG:
                case GUI_CANVAS_OUTPUT_JPG:
                    {
                        data_length = image_width * image_height * pixel_length;
                        buffer = gui_lower_malloc(data_length);
                        memset(buffer, 0, data_length);
                    }
                    break;
                case GUI_CANVAS_OUTPUT_RGBA:
                    {
                        output_data = target_buffer;
                        buffer = output_data + sizeof(gui_rgb_data_head_t);
                        memset(output_data, 0, sizeof(gui_rgb_data_head_t));
                        gui_rgb_data_head_t *head = (void *)output_data;
                        head->type = ARGB8888;
                        head->w = image_width;
                        head->h = image_height;
                    }
                    break;
                case GUI_CANVAS_OUTPUT_RGB565:
                    {
                        pixel_length = 2;
                        output_data = target_buffer;
                        memset(output_data, 0, sizeof(gui_rgb_data_head_t));
                        buffer = output_data + sizeof(gui_rgb_data_head_t);
                        gui_rgb_data_head_t *head = (void *)output_data;
                        head->type = RGB565;
                        head->w = image_width;
                        head->h = image_height;
                    }
                    break;
                default:
                    break;
                }

                {
                    NVGcontext *vg = 0;
                    extern NVGcontext *nvgCreateAGGE(uint32_t w,
                                                     uint32_t h,
                                                     uint32_t stride,
                                                     enum     NVGtexture format,
                                                     uint8_t *data);
                    extern void nvgDeleteAGGE(NVGcontext * ctx);
                    vg = nvgCreateAGGE(image_width, image_height, image_width * (pixel_length),
                                       (pixel_length) == 2 ? NVG_TEXTURE_BGR565 : NVG_TEXTURE_BGRA, buffer);
                    nvgBeginFrame(vg, image_width, image_height, 1);

                    nvgResetTransform(vg);
                    float a0,  a1;
                    a0 = sd;
                    a1 = ed;
                    if (a0 != a1)
                    {
                        nvgArc(vg, r + stroke_width, r + stroke_width, r, sd, ed, dir);
                        nvgStrokeWidth(vg, stroke_width);
                        nvgStrokeColor(vg, nvgRGBA(stroke.color.rgba.r, stroke.color.rgba.g, stroke.color.rgba.b,
                                                   stroke.color.rgba.a));
                        nvgStroke(vg);
                    }


                    nvgEndFrame(vg);
                    nvgDeleteAGGE(vg);
                }
            }
            gui_img_t *img = gui_img_create_from_mem(parent, ptxt, imgdata, cx - image_w / 2, cy - image_h / 2,
                                                     0, 0);
            gui_img_set_mode(img, IMG_SRC_OVER_MODE);
            parent = (void *)img;
            parent->obj_cb = gui_canvas_img_cb;
        }


    }

    return parent;
}
/**
 * @brief Create a progress bar widget.
 *
 * The created widget can be a canvas type or image type.
 *
 * For canvas type, the color of the progress bar can be set.
 * For image type, the image of the progress bar can be set.
 *
 * The orientation of the progress bar can be set to vertical or horizontal.
 *
 * The created widget can be used as a parent of other widgets.
 *
 * @param p the XML node of the widget.
 * @param parent the parent widget of the created widget.
 * @param widget_type the type of the created widget.
 * @return the created widget.
 */
static gui_obj_t *widget_create_progressbar(ezxml_t p, gui_obj_t *parent, T_OBJ_TYPE widget_type)
{

    {
        size_t i = 0;
        int16_t x = 0;
        int16_t y = 0;
        int16_t w = 0; GUI_UNUSED(w);
        int16_t h = 0; GUI_UNUSED(h);
        gui_color_t color = APP_COLOR_WHITE_OPACITY;
        gui_color_t highlightColor = APP_COLOR_WHITE;
        bool vh = false;
        bool canvas = false;
        char *picture = "app/system/resource/Progress bar_full.bin";
        while (true)
        {
            if (!(p->attr[i]))
            {
                break;
            }
            ////gui_log("p->attr[i]:%s\n", p->attr[i]);
            if (!strcmp(p->attr[i], "x"))
            {
                x = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "y"))
            {
                y = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "w"))
            {
                w = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "h"))
            {
                h = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "canvasOrPicture"))
            {
                if (!strcmp(p->attr[++i], "canvas"))
                {
                    canvas = true;
                }
                else
                {
                    canvas = false;
                }

            }
            else if (!strcmp(p->attr[i], "picture"))
            {
                picture = gui_strdup(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "color"))
            {
                color = string_rgb888(p->attr[++i]);
                //gui_log("color %s,%x\n", p->attr[i], color);
            }
            else if (!strcmp(p->attr[i], "highlightColor"))
            {
                highlightColor = string_rgb888(p->attr[++i]);
                //gui_log("color %s,%x\n", p->attr[i], highlightColor);
            }
            else if (!strcmp(p->attr[i], "orientation"))
            {
                char *s = p->attr[++i];
                if (!strcmp(p->attr[i], "vertical"))
                {
                    vh = true;
                }
                else if (!strcmp(p->attr[i], "V"))
                {
                    vh = true;
                }
                else if (!strcmp(p->attr[i], "horizontal"))
                {
                    vh = false;
                }
                else if (!strcmp(p->attr[i], "H"))
                {
                    vh = false;
                }

            }
            i++;
        }
        char *ptxt = get_space_string_head(p->txt);
        if (canvas == true)
        {
#if 0
            if (vh)
            {
                //parent = (void *)gui_progressbar_v_create(parent, ptxt, x, y, w, h);
            }
            else
            {
                //parent = (void *)gui_progressbar_create(parent, ptxt, x, y, w, h);
            }

            GUI_TYPE(gui_progressbar_t, parent)->color = color;
            GUI_TYPE(gui_progressbar_t, parent)->color_hl = highlightColor;
#endif
        }
        else
        {
            if (vh)
            {
                parent = (void *)gui_progressbar_img_v_create(parent, (void *)gui_get_image_file_address(picture),
                                                              x, y);
            }
            else
            {
                parent = (void *)gui_progressbar_img_h_create(parent, (void *)gui_get_image_file_address(picture),
                                                              x, y);
            }
        }
        parent->name = ptxt;




    }

    return parent;
}
static gui_obj_t *widget_create_seekbar(ezxml_t p, gui_obj_t *parent, T_OBJ_TYPE widget_type)
{

    {
        size_t i = 0;
        int16_t x = 0;
        int16_t y = 0;
        int16_t w = 0; GUI_UNUSED(w);
        int16_t h = 0; GUI_UNUSED(h);
        gui_color_t color = APP_COLOR_WHITE_OPACITY;
        gui_color_t highlightColor = APP_COLOR_WHITE;
        bool canvas = false;
        bool arc = false;
        char *picture = "app/system/resource/Progress bar_full.bin";
        char *folder = NULL;
        char *picture_thumb = 0;
        char *picture_thumb_hl = 0;
        bool vh = false;
        int16_t cx = 0;
        int16_t cy = 0;

        float sd = 0;
        float ed = 0;
        // default image blend_mode
        BLEND_MODE_TYPE blendMode = IMG_FILTER_BLACK;
        uint8_t opacity = 255;
        bool reverse = 0;
        while (true)
        {
            if (!(p->attr[i]))
            {
                break;
            }
            ////gui_log("p->attr[i]:%s\n", p->attr[i]);
            if (!strcmp(p->attr[i], "x"))
            {
                x = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "y"))
            {
                y = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "w"))
            {
                w = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "h"))
            {
                h = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "canvasOrPicture"))
            {
                if (!strcmp(p->attr[++i], "canvas"))
                {
                    canvas = true;
                }
                else
                {
                    canvas = false;
                }

            }
            else if (!strcmp(p->attr[i], "folder"))
            {
                folder = gui_strdup(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "picture"))
            {
                picture = (p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "pictureThumb"))
            {
                picture_thumb = (p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "pictureThumbHighlight"))
            {
                picture_thumb_hl = (p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "color"))
            {
                color = string_rgb888(p->attr[++i]);
                //gui_log("color %s,%x\n", p->attr[i], color);
            }
            else if (!strcmp(p->attr[i], "highlightColor"))
            {
                highlightColor = string_rgb888(p->attr[++i]);
                //gui_log("color %s,%x\n", p->attr[i], highlightColor);
            }
            else if (!strcmp(p->attr[i], "orientation"))
            {
                char *s = p->attr[++i];
                if (!strcmp(p->attr[i], "vertical"))
                {
                    vh = true;
                }
                else if (!strcmp(p->attr[i], "V"))
                {
                    vh = true;
                }
                else if (!strcmp(p->attr[i], "horizontal"))
                {
                    vh = false;
                }
                else if (!strcmp(p->attr[i], "H"))
                {
                    vh = false;
                }
                else if (!strcmp(p->attr[i], "arc"))
                {
                    arc = true;
                }

            }
            else if (!strcmp(p->attr[i], "cx") || !strcmp(p->attr[i], "centralX"))
            {
                cx = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "cy") || !strcmp(p->attr[i], "centralY"))
            {
                cy = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "startDegree"))
            {
                sd = atof(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "endDegree"))
            {
                ed = atof(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "reverse"))
            {
                reverse = 1;
            }
            else if (!strcmp(p->attr[i], "blendMode"))
            {
                i++;
                if (!strcmp(p->attr[i], "imgBypassMode"))
                {
                    blendMode = IMG_BYPASS_MODE;
                }
                else if (!strcmp(p->attr[i], "imgFilterBlack"))
                {
                    blendMode = IMG_FILTER_BLACK;
                }
                else if (!strcmp(p->attr[i], "imgSrcOverMode"))
                {
                    blendMode = IMG_SRC_OVER_MODE;
                }
                else if (!strcmp(p->attr[i], "imgCoverMode"))
                {
                    blendMode = IMG_COVER_MODE;
                }
                GUI_UNUSED(blendMode);
            }
            else if (!strcmp(p->attr[i], "opacity"))
            {
                opacity = atof(p->attr[++i]);
            }
            i++;
        }
        char *ptxt = get_space_string_head(p->txt);
        if (canvas)
        {
#if 0
            /* code */


            if (vh)
            {
                //parent = (void *)gui_seekbar_create(parent, ptxt, x, y, w, h);
            }
            else
            {
                //parent = (void *)gui_seekbar_h_create(parent, ptxt, x, y, w, h);
            }

            GUI_TYPE(gui_progressbar_t, parent)->color = color;
            GUI_TYPE(gui_progressbar_t, parent)->color_hl = highlightColor;
#endif
        }
        else if (!folder)
        {
            if (picture_thumb)
            {
                if (!picture_thumb_hl)
                {
                    picture_thumb_hl = picture_thumb;
                }


                parent = (void *)gui_seekbar_create_thumb_h(parent,
                                                            (void *)gui_get_image_file_address(picture_thumb),
                                                            (void *)gui_get_image_file_address(picture_thumb_hl), x,
                                                            y, w);
            }
            else
            {
                /* code */


                if (vh)
                {
                    parent = (void *)gui_seekbar_create_img_v(parent, (void *)gui_get_image_file_address(picture), x,
                                                              y);
                }
                else
                {
                    parent = (void *)gui_seekbar_create_img_h(parent, (void *)gui_get_image_file_address(picture), x,
                                                              y);
                }
            }
            gui_img_set_mode(GUI_TYPE(gui_img_t, GUI_TYPE(gui_seekbar_t, parent)->base.c), blendMode);
            gui_img_set_opacity(GUI_TYPE(gui_img_t, GUI_TYPE(gui_seekbar_t, parent)->base.c), opacity);
        }
        else if (folder)
        {
            int file_count = 0;

            {
                gui_fs_dir *dir = 0;
                struct gui_fs_dirent *entry;
                char *path = gui_malloc(strlen(folder) + strlen(GUI_ROOT_FOLDER) + 1);
                sprintf(path, "%s%s", GUI_ROOT_FOLDER, folder);
                if ((dir = gui_fs_opendir(path)) == NULL)
                {
                    gui_free(path);
                    return 0;
                }
                gui_free(path);
                while ((entry = gui_fs_readdir(dir)) != NULL)
                {
                    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                    {
                        file_count++;
                    }


                }
                gui_fs_closedir(dir);
            }
            void **image_array = gui_malloc(file_count * sizeof(void *));
            {
                gui_fs_dir *dir = 0;
                struct gui_fs_dirent *entry;
                char *path = gui_malloc(strlen(folder) + strlen(GUI_ROOT_FOLDER) + 1);
                sprintf(path, "%s%s", GUI_ROOT_FOLDER, folder);
                if ((dir = gui_fs_opendir(path)) == NULL)
                {
                    gui_free(path);
                    return 0;
                }

                int count = 0;
                while ((entry = gui_fs_readdir(dir)) != NULL)
                {
                    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                    {
                        char *path2 = gui_malloc(strlen(entry->d_name) + strlen(folder) + 2);
                        sprintf(path2, "%s/%s", folder, entry->d_name);
                        image_array[count++] = (void *)gui_get_image_file_address(path2);
                    }

                }
                gui_free(path);
                gui_fs_closedir(dir);
                if (reverse)
                {
                    reverse_array(image_array, count);
                }


            }
            bool if_widget = 0;
            if (p->parent)
            {
                if (!strcmp(widget[21].name, p->parent->name))
                {
                    if_widget = 1;
                }
            }
            if (if_widget)
            {
                parent = (void *)gui_seekbar_create_movie_h_double(parent, image_array, file_count, x, y);
                gui_img_set_mode(GUI_TYPE(gui_img_t, GUI_TYPE(gui_seekbar_t, parent)->base.c), blendMode);
                gui_img_set_opacity(GUI_TYPE(gui_img_t, GUI_TYPE(gui_seekbar_t, parent)->base.c), opacity);
            }
            else
            {
                if (arc)
                {
                    parent = (void *)gui_seekbar_create_movie_arc(parent, image_array, file_count, x, y,
                                                                  cx, cy, 100, 100, sd, ed);
                    gui_img_set_mode(GUI_TYPE(gui_img_t, GUI_TYPE(gui_seekbar_t, parent)->base.c), blendMode);
                    gui_img_set_opacity(GUI_TYPE(gui_img_t, GUI_TYPE(gui_seekbar_t, parent)->base.c), opacity);
                }
                else
                {
                    if (vh)
                    {
                        parent = (void *)gui_seekbar_create_movie_v(parent, image_array, file_count, x, y);
                    }
                    else
                    {
                        parent = (void *)gui_seekbar_create_movie_h(parent, image_array, file_count, x, y);
                    }
                    gui_img_set_mode(GUI_TYPE(gui_img_t, GUI_TYPE(gui_seekbar_t, parent)->base.c), blendMode);
                    gui_img_set_opacity(GUI_TYPE(gui_img_t, GUI_TYPE(gui_seekbar_t, parent)->base.c), opacity);
                }
            }





        }

        parent->name = ptxt;



    }

    return parent;
}
static gui_obj_t *widget_create_tab(ezxml_t p, gui_obj_t *parent, T_OBJ_TYPE widget_type)
{

    {
        size_t i = 0;
        int16_t x = 0;
        int16_t y = 0;
        int16_t w = 0;
        int16_t h = 0;
        int16_t idx = 0;
        int16_t idy = 0;

        while (true)
        {
            if (!(p->attr[i]))
            {
                break;
            }
            ////gui_log("p->attr[i]:%s\n", p->attr[i]);
            if (!strcmp(p->attr[i], "x"))
            {
                x = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "y"))
            {
                y = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "w"))
            {
                w = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "h"))
            {
                h = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "idx"))
            {
                idx = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "idy"))
            {
                idy = atoi(p->attr[++i]);
            }

            i++;
        }
        char *ptxt = get_space_string_head(p->txt);
        //gui_log("x:%d,y:%d,w:%dh:%d,idx:%d,idy:%d\n", x, y, w, h, idx, idy);
        parent = (void *)gui_tab_create(parent, ptxt, x, y, w, h, idx, idy);
    }

    return parent;
}
static gui_obj_t *widget_create_curtainview(ezxml_t p, gui_obj_t *parent, T_OBJ_TYPE widget_type)
{

    {
        size_t i = 0;
        int16_t x = 0;
        int16_t y = 0;
        int16_t w = 0;
        int16_t h = 0;
        int style = 0;
        while (true)
        {
            if (!(p->attr[i]))
            {
                break;
            }
            ////gui_log("p->attr[i]:%s\n", p->attr[i]);
            if (!strcmp(p->attr[i], "x"))
            {
                x = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "y"))
            {
                y = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "w"))
            {
                w = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "h"))
            {
                h = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "transition"))
            {
                char *s = p->attr[++i];
                if (!strcmp(p->attr[i], "normal"))
                {
                    style = WIDGET_CLASSIC;
                }
                else if (!strcmp(p->attr[i], "fade"))
                {
                    style = WIDGET_FADE;
                }
                else if (!strcmp(p->attr[i], "scale"))
                {
                    style = WIDGET_SCALE;
                }
                else if (!strcmp(p->attr[i], "fadeScale"))
                {
                    style = WIDGET_SCALE_FADE;
                }
                GUI_UNUSED(style);
            }
            i++;
        }
        char *ptxt = get_space_string_head(p->txt);
        //gui_log("x:%d,y:%d,w:%dh:%d,scope:%f\n", x, y, w, h);
        parent = (void *)gui_curtainview_create(parent, ptxt, x, y, w, h);
        GUI_TYPE(gui_curtainview_t, parent)->style = style;
    }

    return parent;
}
static gui_obj_t *widget_create_page(ezxml_t p, gui_obj_t *parent, T_OBJ_TYPE widget_type)
{

    {
        size_t i = 0;
        int16_t x = 0;
        int16_t y = 0;
        int16_t w = 0;
        int16_t h = 0;
        while (true)
        {
            if (!(p->attr[i]))
            {
                break;
            }
            ////gui_log("p->attr[i]:%s\n", p->attr[i]);
            if (!strcmp(p->attr[i], "x"))
            {
                x = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "y"))
            {
                y = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "w"))
            {
                w = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "h"))
            {
                h = atoi(p->attr[++i]);
            }
            i++;
        }
        char *ptxt = get_space_string_head(p->txt);
        //gui_log("x:%d,y:%d,w:%dh:%d,scope:%f\n", x, y, w, h);
        parent = (void *)gui_win_create(parent, "_page_scope", x, y, w, h);
        gui_win_set_scope((void *)parent, 1);
        parent = (void *)gui_page_create(parent, ptxt, 0, 0, w, h);
    }

    return parent;
}
static gui_obj_t *widget_create_grid(ezxml_t p, gui_obj_t *parent, T_OBJ_TYPE widget_type)
{

    {
        size_t i = 0;
        int16_t x = 0;
        int16_t y = 0;
        int16_t rowNumber = 4;
        int16_t colNumber = 4;
        int16_t rowGap = 100;
        int16_t colGap = 100;
        while (true)
        {
            if (!(p->attr[i]))
            {
                break;
            }
            ////gui_log("p->attr[i]:%s\n", p->attr[i]);
            if (!strcmp(p->attr[i], "x"))
            {
                x = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "y"))
            {
                y = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "rowNumber"))
            {
                rowNumber = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "colNumber"))
            {
                colNumber = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "rowGap"))
            {
                rowGap = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "colGap"))
            {
                colGap = atoi(p->attr[++i]);
            }
            i++;
        }
        char *ptxt = get_space_string_head(p->txt);
        //gui_log("x:%d,y:%d,w:%dh:%d,scope:%f\n", x, y, w, h);
        parent = (void *)gui_grid_create(parent, x, y, rowNumber, colNumber, colGap, rowGap);
        parent->name = ptxt;
    }

    return parent;
}
static gui_obj_t *widget_create_screen(ezxml_t p, gui_obj_t *parent, T_OBJ_TYPE widget_type)
{

    {
        size_t i = 0;
        int16_t w = 0; GUI_UNUSED(w);
        int16_t h = 0; GUI_UNUSED(h);
        while (true)
        {
            if (!(p->attr[i]))
            {
                break;
            }
            ////gui_log("p->attr[i]:%s\n", p->attr[i]);
            else if (!strcmp(p->attr[i], "w"))
            {
                w = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "h"))
            {
                h = atoi(p->attr[++i]);
            }
            i++;
        }
    }

    return parent;
}
static gui_obj_t *widget_create_curtain(ezxml_t p, gui_obj_t *parent, T_OBJ_TYPE widget_type)
{

    {
        size_t i = 0;
        int16_t x = 0;
        int16_t y = 0;
        int16_t w = 0;
        int16_t h = 0;
        float scope = 1.0f;
        T_GUI_CURTAIN_ENUM orientation = CURTAIN_MIDDLE;
        // int style = 0; GUI_UNUSED(style);
        while (true)
        {
            if (!(p->attr[i]))
            {
                break;
            }
            ////gui_log("p->attr[i]:%s\n", p->attr[i]);
            if (!strcmp(p->attr[i], "x"))
            {
                x = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "y"))
            {
                y = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "w"))
            {
                w = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "h"))
            {
                h = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "scope"))
            {
                scope = atof(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "orientation"))
            {
                const char *o = p->attr[++i];
                if (!strcmp(o, "middle"))
                {
                    orientation = CURTAIN_MIDDLE;
                }
                else if (!strcmp(o, "up"))
                {
                    orientation = CURTAIN_UP;
                }
                else if (!strcmp(o, "down"))
                {
                    orientation = CURTAIN_DOWN;
                }
                else if (!strcmp(o, "left"))
                {
                    orientation = CURTAIN_LEFT;
                }
                else if (!strcmp(o, "right"))
                {
                    orientation = CURTAIN_RIGHT;
                }
            }
            // else if (!strcmp(p->attr[i], "transition"))
            // {
            //     char *s = p->attr[++i];
            //     if (!strcmp(p->attr[i], "normal"))
            //     {
            //         style = WIDGET_CLASSIC;
            //     }
            //     else if (!strcmp(p->attr[i], "fade"))
            //     {
            //         style = WIDGET_FADE;
            //     }
            //     else if (!strcmp(p->attr[i], "scale"))
            //     {
            //         style = WIDGET_SCALE;
            //     }
            //     else if (!strcmp(p->attr[i], "fadeScale"))
            //     {
            //         style = WIDGET_SCALE_FADE;
            //     }

            // }
            i++;
        }
        char *ptxt = get_space_string_head(p->txt);
        //gui_log("x:%d,y:%d,w:%dh:%d,orientation:%d\n", x, y, w, h, orientation);
        parent = (void *)gui_curtain_create(parent, ptxt, x, y, w, h, orientation, scope);
        if (orientation == CURTAIN_UP)
        {
            parent = (void *)gui_win_create(parent, 0,  0, (1 - scope) * gui_get_screen_height(), w, h * scope);
        }

        else if (orientation == CURTAIN_LEFT)
        {
            parent = (void *)gui_win_create(parent, 0, (1 - scope) * gui_get_screen_width(), 0, w * scope, h);
        }


    }

    return parent;
}
static gui_obj_t *widget_create_icon(ezxml_t p, gui_obj_t *parent, T_OBJ_TYPE widget_type)
{

    {
        size_t i = 0;
        int16_t x = 0;
        int16_t y = 0;
        int16_t w = 0;
        int16_t h = 0;


        char *font_type = 0;
        char *text = NULL;
        int text_x = 0;
        int text_y = 0;
        gui_color_t font_color = APP_COLOR_RED;
        uint32_t font_size = 0;
        int picture_x = 0;
        int picture_y = 0;
        int transition = 0; GUI_UNUSED(transition);
        char *picture = NULL;
        char *hl_picture = NULL;
        int style = 0;
        // default image blend_mode
        BLEND_MODE_TYPE blendMode = IMG_FILTER_BLACK;
        uint8_t opacity = 255;
        float scalex = 1;
        float scaley = 1;
        float angle = 0;
        while (true)
        {
            if (!(p->attr[i]))
            {
                break;
            }
            //gui_log("p->attr[i]:%s,\n", p->attr[i]);
            if (!strcmp(p->attr[i], "x"))
            {
                x = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "y"))
            {
                y = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "w"))
            {
                w = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "h"))
            {
                h = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "font"))
            {
                font_type = gui_strdup(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "picture"))
            {
                picture = gui_strdup(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "highlightPicture"))
            {
                hl_picture = gui_strdup(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "fontColor"))
            {
                //font_color = string_rgb888_to_rgb565(p->attr[++i]);
                font_color = string_rgb888(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "fontSize"))
            {
                font_size = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "text"))
            {
                text = gui_strdup(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "textX"))
            {
                text_x = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "textY"))
            {
                text_y = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "pictureX"))
            {
                picture_x = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "pictureY"))
            {
                picture_y = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "mode"))
            {
                char *s = p->attr[++i];
                if (!strcmp(p->attr[i], "normal"))
                {
                    style = WIDGET_CLASSIC;
                }
                else if (!strcmp(p->attr[i], "fade"))
                {
                    style = WIDGET_FADE;
                }
                else if (!strcmp(p->attr[i], "scale"))
                {
                    style = WIDGET_SCALE;
                }
                else if (!strcmp(p->attr[i], "fadeScale"))
                {
                    style = WIDGET_SCALE_FADE;
                }
                else if (!strcmp(p->attr[i], "array"))
                {
                    style = BUTTON_HIGHLIGHT_ARRAY;
                }
                GUI_UNUSED(style);
            }
            else if (!strcmp(p->attr[i], "blendMode"))
            {
                i++;
                if (!strcmp(p->attr[i], "imgBypassMode"))
                {
                    blendMode = IMG_BYPASS_MODE;
                }
                else if (!strcmp(p->attr[i], "imgFilterBlack"))
                {
                    blendMode = IMG_FILTER_BLACK;
                }
                else if (!strcmp(p->attr[i], "imgSrcOverMode"))
                {
                    blendMode = IMG_SRC_OVER_MODE;
                }
                else if (!strcmp(p->attr[i], "imgCoverMode"))
                {
                    blendMode = IMG_COVER_MODE;
                }
                GUI_UNUSED(blendMode);
            }
            else if (!strcmp(p->attr[i], "opacity"))
            {
                opacity = atof(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "scaleX"))
            {
                scalex = atof(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "scaleY"))
            {
                scaley = atof(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "rotationAngle"))
            {
                angle = atof(p->attr[++i]);
            }
            i++;
        }
        void *img1;
        void *img2;
        {
            img1 = (void *)gui_get_image_file_address(picture);
            gui_free(picture);
            img2 = img1;
        }
        if (style != BUTTON_HIGHLIGHT_ARRAY)
        {
            img2 = (void *)gui_get_image_file_address(hl_picture);
            gui_free(hl_picture);
        }
        char *ptxt = get_space_string_head(p->txt);
        if (text)
        {
            if (strlen(text) == 0)
            {
                text = 0;
            }
            else if (font_type == 0 ||
                     font_size == 0 ||
                     gui_get_file_address(font_type) == 0)
            {
                text = "Font Export failed! Try again.";
                font_size = 16;
                w = gui_get_screen_width();
                font_type = "app/system/resource/font/arialbd_size16_bits4_font.bin";
            }
        }


        //font_size = 32;
        parent = (void *)gui_button_create(parent, x, y, w, h, img1, img2, text, BUTTON_BG_ICON, 0);
        GUI_TYPE(gui_button_t, parent)->style = style;
        parent->name = ptxt;
        gui_button_img_move((void *)parent, picture_x, picture_y);
        gui_button_text_move((void *)parent, text_x, text_y);
        gui_img_set_mode(GUI_TYPE(gui_button_t, parent)->img, blendMode);
        gui_img_set_opacity(GUI_TYPE(gui_button_t, parent)->img, opacity);
        if (scalex != 1 || scaley != 1)
        {
            gui_img_scale((void *)GUI_TYPE(gui_button_t, parent)->img, scalex, scaley);
        }
        if (angle != 0)
        {
            gui_img_get_height((void *)GUI_TYPE(gui_button_t, parent)->img);
            gui_img_rotation((void *)GUI_TYPE(gui_button_t, parent)->img, angle, GUI_BASE(GUI_TYPE(gui_button_t,
                                                                                          parent)->img)->w / 2, GUI_BASE(GUI_TYPE(gui_button_t, parent)->img)->h / 2);
        }
        if (text && GUI_TYPE(gui_button_t, parent)->text)
        {
            gui_color_t color_temporary;
            color_temporary = font_color;
            GUI_TYPE(gui_button_t, parent)->text->color = color_temporary;
        }
        parent->obj_cb = button_render;
        parent->has_destroy_cb = true;

        if (style)
        {
            if (style == BUTTON_HIGHLIGHT_ARRAY)
            {
                int file_count = 0;
                char *folder = hl_picture;
                {
                    gui_fs_dir *dir = 0;
                    struct gui_fs_dirent *entry;
                    char *path = gui_malloc(strlen(folder) + strlen(GUI_ROOT_FOLDER) + 1);
                    sprintf(path, "%s%s", GUI_ROOT_FOLDER, folder);
                    if ((dir = gui_fs_opendir(path)) == NULL)
                    {
                        GUI_ASSERT(0);
                    }
                    gui_free(path);
                    while ((entry = gui_fs_readdir(dir)) != NULL)
                    {
                        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                        {
                            file_count++;
                        }


                    }
                    gui_fs_closedir(dir);
                }
                void **image_array = gui_malloc(file_count * sizeof(void *));
                {
                    gui_fs_dir *dir = 0;
                    struct gui_fs_dirent *entry;
                    char *path = gui_malloc(strlen(folder) + strlen(GUI_ROOT_FOLDER) + 1);
                    sprintf(path, "%s%s", GUI_ROOT_FOLDER, folder);
                    if ((dir = gui_fs_opendir(path)) == NULL)
                    {
                        gui_free(path);
                        return 0;
                    }

                    int count = 0;
                    while ((entry = gui_fs_readdir(dir)) != NULL)
                    {
                        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                        {
                            char *path2 = gui_malloc(strlen(entry->d_name) + strlen(folder) + 2);
                            sprintf(path2, "%s/%s", folder, entry->d_name);
                            image_array[count++] = (void *)gui_get_image_file_address(path2);
                        }

                    }
                    gui_free(path);
                    gui_fs_closedir(dir);
                    GUI_API(gui_button_t).on_press((void *)parent, (gui_event_cb_t)button_press_array,
                                                   (void *)(uintptr_t)file_count);
                    GUI_TYPE(gui_button_t, parent)->data = image_array;
                    GUI_API(gui_button_t).on_release((void *)parent, (gui_event_cb_t)button_press_array, (void *) - 1);
                }
            }
            else
            {
                gui_button_press((void *)parent, (gui_event_cb_t)sport_button_press, parent);
                gui_button_release((void *)parent, (gui_event_cb_t)sport_button_release, parent);
            }

        }
        if (text && GUI_TYPE(gui_button_t, parent)->text)
        {
            FONT_SRC_TYPE font_type2; GUI_UNUSED(font_type2);
            if (strstr(font_type, ".bin") != NULL)
            {
                font_type2 = GUI_FONT_SRC_BMP;
                void *addr1 = gui_get_file_address(font_type);
//gui_font_mem_init(addr1);
                GUI_TYPE(gui_button_t, parent)->text->font_height = font_size;
                GUI_TYPE(gui_button_t, parent)->text->path = 0;
                gui_text_type_set(GUI_TYPE(gui_button_t, parent)->text, addr1, FONT_SRC_MEMADDR);
                gui_text_mode_set(GUI_TYPE(gui_button_t, parent)->text, LEFT);
// t->font_height = fontSize;
//t->path = 0;
            }
            else if ((strstr(font_type, ".ttf") != NULL) || (strstr(font_type, ".TTF") != NULL))
            {
#ifdef __WIN32
                font_type2 = GUI_FONT_SRC_TTF;
                GUI_TYPE(gui_button_t, parent)->text->path = gui_get_file_address(font_type);
                GUI_TYPE(gui_button_t, parent)->text->font_type = GUI_FONT_SRC_TTF;

#else
                font_type =
                    "app/system/resource/font/gbk_32_32_dot.bin;app/system/resource/font/gbk_unicode_table.bin";
                {
                    font_type2 = GUI_FONT_SRC_BMP;
                    char b[100] = {0};
                    strncpy(b, font_type, strstr(font_type, ".bin;") - font_type + strlen(".bin"));
                    void *addr1 = gui_get_file_address(b);
                    memset(b, 0, sizeof(b));
                    char *a = font_type;
                    strncpy(b, strstr(a, ".bin;") + strlen(".bin;"), strlen(a) - (strstr(a,
                                                                                         ".bin;") - a + strlen(".bin;")));
                    void *addr2 = gui_get_file_address(b);
// gui_set_font_mem_resourse(32, addr1,  addr2);
                    GUI_TYPE(gui_button_t, parent)->text->path = 0;
                    GUI_TYPE(gui_button_t, parent)->text->font_type = GUI_FONT_SRC_BMP;
                }
#endif
            }
        }
        {
            bool if_widget = 0;
            if (p->parent)
            {
                if (!strcmp(widget[21].name, p->parent->name))
                {
                    if_widget = 1;
                }
            }
            if (if_widget)
            {
                if (open_switch_name && !strcmp(parent->name, open_switch_name))
                {
                    gui_free(open_switch_name);
                    open_switch_name = 0;
                    GUI_API(gui_button_t).on_click((gui_button_t *)parent,
                                                   (gui_event_cb_t)switch_cb_for_mororized_curtain, (void *)0);
                }
                else if (close_switch_name && !strcmp(parent->name, close_switch_name))
                {
                    gui_free(close_switch_name);
                    close_switch_name = 0;
                    GUI_API(gui_button_t).on_click((gui_button_t *)parent,
                                                   (gui_event_cb_t)switch_cb_for_mororized_curtain, (void *)1);
                }
                else if (pause_switch_name && !strcmp(parent->name, pause_switch_name))
                {
                    gui_free(pause_switch_name);
                    pause_switch_name = 0;
                    GUI_API(gui_button_t).on_click((gui_button_t *)parent,
                                                   (gui_event_cb_t)switch_cb_for_mororized_curtain, (void *)2);
                }
            }
        }


    }

    return parent;
}
typedef struct radio
{
    const uint8_t *pic;
    const uint8_t *pic_hl;
    char *radio_name;
    bool check;
    gui_img_t *img;
} radio_t;
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(win_radio_cb)
{
    radio_t *param = p;
    if (param->check)
    {
        gui_obj_enable_event(this_widget, GUI_EVENT_6);
        param->check = 0;
    }
    else
    {
        gui_obj_enable_event(this_widget, GUI_EVENT_INVALIDE);
    }


}

static void radio_win_update_att(gui_obj_t *obj)
{
    gui_win_t *this = (void *)obj;
    animate_frame_update(this->animate, obj);
    {
        for (size_t i = 0; i < this->animate_array_length; i++)
        {
            animate_frame_update(this->animate_array[i], obj);
        }

    }

}
static void radio_win_input_prepare(gui_obj_t *obj)
{
}
#include "kb_algo.h"
static void radio_win_prepare(gui_obj_t *obj)
{
    extern touch_info_t *tp_get_info(void);
    touch_info_t *tp = tp_get_info();
    kb_info_t *kb = kb_get_info();

    gui_win_t *this = (void *)obj;

    radio_win_update_att(obj);
    gui_event_t event_code = GUI_EVENT_INVALIDE;
    for (uint8_t i = 0; i < obj->event_dsc_cnt; i++)
    {
        gui_event_dsc_t *event_dsc = obj->event_dsc + i;
        event_code = event_dsc->event_code;
    }
    if ((kb->type == KB_SHORT) && (obj->event_dsc_cnt > 0) && !(obj->event_dsc->event_code))
    {
        if (event_code != GUI_EVENT_6)
        {
            gui_obj_enable_event(obj, GUI_EVENT_KB_SHORT_CLICKED);
        }
    }

    if (gui_obj_point_in_obj_rect(obj, tp->x, tp->y) == true)
    {
        if (tp->pressing)
        {
            if (event_code != GUI_EVENT_6)
            {
                gui_obj_enable_event(obj, GUI_EVENT_TOUCH_PRESSING);
            }
        }
        switch (tp->type)
        {
        case TOUCH_SHORT:
            {
                if (event_code != GUI_EVENT_6)
                {
                    gui_obj_enable_event(obj, GUI_EVENT_TOUCH_CLICKED);
                }
            }
            break;

        case TOUCH_UP_SLIDE:
            {
                if (event_code != GUI_EVENT_6)
                {
                    gui_obj_enable_event(obj, GUI_EVENT_3);
                }
            }
            break;

        case TOUCH_DOWN_SLIDE:
            {
                if (event_code != GUI_EVENT_6)
                {
                    gui_obj_enable_event(obj, GUI_EVENT_4);
                }
            }
            break;

        case TOUCH_LEFT_SLIDE:
            {
                if (event_code != GUI_EVENT_6)
                {
                    gui_obj_enable_event(obj, GUI_EVENT_1);
                }
            }
            break;

        case TOUCH_RIGHT_SLIDE:
            {
                if (event_code != GUI_EVENT_6)
                {
                    gui_obj_enable_event(obj, GUI_EVENT_2);
                }
            }
            break;

        case TOUCH_LONG:
            {
                if (this->long_flag == false)
                {
                    this->long_flag = true;
                    if (event_code != GUI_EVENT_6)
                    {
                        gui_obj_enable_event(obj, GUI_EVENT_TOUCH_LONG);
                    }
                }
            }
            break;

        default:
            break;
        }

        if (tp->pressed)
        {
            if (event_code != GUI_EVENT_6)
            {
                gui_obj_enable_event(obj, GUI_EVENT_TOUCH_PRESSED);
            }
            this->long_flag = false;
            this->press_flag = true;
        }
    }

    if (this->release_flag)
    {
        this->press_flag = false;
        this->release_flag = false;
        if (event_code != GUI_EVENT_6)
        {
            gui_obj_enable_event(obj, GUI_EVENT_TOUCH_RELEASED);
        }
        this->long_flag = false;
    }

    if (tp->released && this->press_flag)
    {
        this->release_flag = true;
    }

    if (this->scale != 0 || this->scale_y != 0)
    {
        matrix_translate(GET_BASE(obj)->w / 2, GET_BASE(obj)->h / 2, GET_BASE(obj)->matrix);
        if (this->scale == 0)
        {
            matrix_scale(1, this->scale_y, GET_BASE(obj)->matrix);
        }
        else if (this->scale_y == 0)
        {
            matrix_scale(this->scale, 1, GET_BASE(obj)->matrix);
        }
        else
        {
            matrix_scale(this->scale, this->scale_y, GET_BASE(obj)->matrix);
        }
        matrix_translate(GET_BASE(obj)->w / -2, GET_BASE(obj)->h / -2, GET_BASE(obj)->matrix);
    }
    if (this->hold_tp)
    {
        gui_obj_skip_other_up_hold(obj);
        gui_obj_skip_other_down_hold(obj);
        gui_obj_skip_other_left_hold(obj);
        gui_obj_skip_other_right_hold(obj);
    }
    if (this->event5_flag)
    {
        gui_obj_enable_event(obj, GUI_EVENT_5);

    }
    if (gui_obj_in_rect(obj, 0, 0, gui_get_screen_width(), gui_get_screen_height()))
    {
        if (this->load == 0)
        {
            gui_obj_enable_event(obj, GUI_EVENT_WIN_LOAD);
            this->load = 1;
        }
    }

}

static void radio_win_destory(gui_obj_t *obj)
{
    gui_win_t *this = (void *)obj;
    if (this->animate)
    {
        gui_free(this->animate);
        this->animate = NULL;
    }
    if (this->animate_array)
    {
        for (size_t i = 0; i < this->animate_array_length; i++)
        {
            gui_free(this->animate_array[i]);
            this->animate_array[i] = NULL;
        }
        gui_free(this->animate_array);
        this->animate_array = 0;
        this->animate_array_length = 0;
    }
}

static void radio_win_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_INPUT_PREPARE:
            radio_win_input_prepare(obj);
            break;
        case OBJ_PREPARE:
            radio_win_prepare(obj);
            break;

        case OBJ_DESTROY:
            radio_win_destory(obj);
            break;

        default:
            break;
        }
    }
}



static GUI_EVENT_CALLBACK_FUNCTION_DEFINE(win_radio_press)
{
    gui_obj_t **widgets = NULL;
    int count = 0;


    gui_obj_tree_get_widget_array_by_type(
        (gui_obj_t *)gui_current_app(), RADIO, &widgets, &count);

    for (int i = 0; i < count; i++)
    {

        gui_win_t *win = GUI_TYPE(gui_win_t, widgets[i]);
        radio_t *param = win->animate->p;
        param->img->data = (void *)param->pic;
        param->check = 0;
    }
    gui_free(widgets);
    {
        gui_win_t *win = GUI_TYPE(gui_win_t, obj);
        radio_t *param = win->animate->p;
        param->img->data = (void *)param->pic_hl;
        param->check = 1;
    }

}
static gui_obj_t *widget_create_radio(ezxml_t p, gui_obj_t *parent, T_OBJ_TYPE widget_type)
{
    size_t i = 0;
    int16_t x = 0;
    int16_t y = 0;
    int16_t w = 0;
    int16_t h = 0;

    int picture_x = 0; GUI_UNUSED(picture_x);
    int picture_y = 0; GUI_UNUSED(picture_y);
    char *picture = NULL;
    char *hl_picture = NULL;
    char *radio_name = NULL;
    // default image blend_mode
    BLEND_MODE_TYPE blendMode = IMG_FILTER_BLACK;
    uint8_t opacity = 255; GUI_UNUSED(opacity);
    while (true)
    {
        if (!(p->attr[i]))
        {
            break;
        }
        //gui_log("p->attr[i]:%s,\n", p->attr[i]);
        if (!strcmp(p->attr[i], "x"))
        {
            x = atoi(p->attr[++i]);
        }
        else if (!strcmp(p->attr[i], "y"))
        {
            y = atoi(p->attr[++i]);
        }
        else if (!strcmp(p->attr[i], "w"))
        {
            w = atoi(p->attr[++i]);
        }
        else if (!strcmp(p->attr[i], "h"))
        {
            h = atoi(p->attr[++i]);
        }
        else if (!strcmp(p->attr[i], "name"))
        {
            radio_name = gui_strdup(p->attr[++i]);
        }
        else if (!strcmp(p->attr[i], "picture"))
        {
            picture = gui_strdup(p->attr[++i]);
        }
        else if (!strcmp(p->attr[i], "highlightPicture"))
        {
            hl_picture = gui_strdup(p->attr[++i]);
        }

        else if (!strcmp(p->attr[i], "pictureX"))
        {
            picture_x = atoi(p->attr[++i]);
        }
        else if (!strcmp(p->attr[i], "pictureY"))
        {
            picture_y = atoi(p->attr[++i]);
        }

        else if (!strcmp(p->attr[i], "blendMode"))
        {
            i++;
            if (!strcmp(p->attr[i], "imgBypassMode"))
            {
                blendMode = IMG_BYPASS_MODE;
            }
            else if (!strcmp(p->attr[i], "imgFilterBlack"))
            {
                blendMode = IMG_FILTER_BLACK;
            }
            else if (!strcmp(p->attr[i], "imgSrcOverMode"))
            {
                blendMode = IMG_SRC_OVER_MODE;
            }
            else if (!strcmp(p->attr[i], "imgCoverMode"))
            {
                blendMode = IMG_COVER_MODE;
            }
            GUI_UNUSED(blendMode);
        }
        else if (!strcmp(p->attr[i], "opacity"))
        {
            opacity = atof(p->attr[++i]);
        }
        i++;
    }
    char *ptxt = get_space_string_head(p->txt);
    //gui_log("x:%d,y:%d,w:%dh:%d\n", x, y, w, h);
    radio_t *param = gui_malloc(sizeof(radio_t));
    memset(param, 0, sizeof(radio_t));
    parent = (void *)gui_win_create(parent, ptxt, x, y, w, h);
    parent->obj_cb = radio_win_cb;
    GUI_TYPE(gui_obj_t, parent)->type = RADIO;
    gui_win_press((void *)parent, win_radio_press, param);

    gui_win_set_animate((void *)parent, 10000, -1, win_radio_cb, param);
    gui_img_t *img = gui_img_create_from_mem(parent, 0, (void *)gui_get_image_file_address(picture), 0,
                                             0, 0, 0);
    param->img = img;
    param->pic_hl = gui_get_image_file_address(hl_picture);
    param->pic = gui_get_image_file_address(picture);
    param->radio_name = radio_name;
    img->blend_mode = blendMode;





    return parent;
}
static gui_obj_t *widget_create_clickswitch(ezxml_t p, gui_obj_t *parent, T_OBJ_TYPE widget_type)
{

    {
        size_t i = 0;
        int16_t x = 0;
        int16_t y = 0;
        int16_t w = 0;
        int16_t h = 0;
        char *picture = NULL;
        char *hl_picture = NULL;
        char *pictureHl = NULL;
        char *hl_pictureHl = NULL;
        int picture_x = 0;
        int picture_y = 0;
        int dur = 1000;
        // default image blend_mode
        BLEND_MODE_TYPE blendMode = IMG_FILTER_BLACK;
        uint8_t opacity = 255;
        int style = 0;
        while (true)
        {
            if (!(p->attr[i]))
            {
                break;
            }
            //gui_log("p->attr[i]:%s,\n", p->attr[i]);
            if (!strcmp(p->attr[i], "x"))
            {
                x = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "y"))
            {
                y = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "w"))
            {
                w = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "h"))
            {
                h = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "picture"))
            {
                picture = gui_strdup(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "highlightPicture"))
            {
                hl_picture = gui_strdup(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "clickedPicture"))
            {
                pictureHl = gui_strdup(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "clickedHighlightPicture"))
            {
                hl_pictureHl = gui_strdup(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "pictureX"))
            {
                picture_x = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "pictureY"))
            {
                picture_y = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "blendMode"))
            {
                i++;
                if (!strcmp(p->attr[i], "imgBypassMode"))
                {
                    blendMode = IMG_BYPASS_MODE;
                }
                else if (!strcmp(p->attr[i], "imgFilterBlack"))
                {
                    blendMode = IMG_FILTER_BLACK;
                }
                else if (!strcmp(p->attr[i], "imgSrcOverMode"))
                {
                    blendMode = IMG_SRC_OVER_MODE;
                }
                else if (!strcmp(p->attr[i], "imgCoverMode"))
                {
                    blendMode = IMG_COVER_MODE;
                }
                GUI_UNUSED(blendMode);
            }
            else if (!strcmp(p->attr[i], "opacity"))
            {
                opacity = atof(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "mode"))
            {
                char *s = p->attr[++i];
                if (!strcmp(p->attr[i], "array"))
                {
                    style = SWITCH_HIGHLIGHT_ARRAY;
                }
            }
            else if (!strcmp(p->attr[i], "duration"))
            {
                dur = atoi(p->attr[++i]);
            }
            i++;
        }
        void *img1;
        void *img2;
        {
            img1 = gui_get_file_address(picture);
            img2 = img1;
        }
        if (style != SWITCH_HIGHLIGHT_ARRAY)
        {
            img2 = gui_get_file_address(hl_picture);
        }
        if (!img1)
        {
            img1 = img2;
        }
        if (!img2)
        {
            img2 = img1;
        }
        if (!img2 && !img1)
        {
            img1 = (void *)gui_get_image_file_address("app/system/resource/switchOff.bin");
            img2 = (void *)gui_get_image_file_address("app/system/resource/switchOn.bin");
            int16_t *scale = img1;
            scale++;
            w = *scale;
            scale++;
            h = *scale;
        }


        parent = (void *)gui_switch_create(parent, x, y, w, h, img1, img2);
        parent->name = get_space_string_head(p->txt);
        if (hl_pictureHl)
        {
            GUI_TYPE(gui_switch_t, parent)->on_hl_pic_addr = (void *)gui_get_image_file_address(hl_pictureHl);
        }
        if (pictureHl)
        {
            GUI_TYPE(gui_switch_t, parent)->off_hl_pic_addr = (void *)gui_get_image_file_address(pictureHl);
        }
        GUI_TYPE(gui_switch_t, parent)->switch_picture->base.x = picture_x;
        GUI_TYPE(gui_switch_t, parent)->switch_picture->base.y = picture_y;
        gui_img_set_mode(GUI_TYPE(gui_switch_t, parent)->switch_picture, blendMode);
        gui_img_set_opacity(GUI_TYPE(gui_switch_t, parent)->switch_picture, opacity);
        if (style)
        {
            if (style == SWITCH_HIGHLIGHT_ARRAY)
            {
                GUI_TYPE(gui_switch_t, parent)->style = style;
                int file_count = 0;
                char *folder = hl_picture;
                {
                    gui_fs_dir *dir = 0;
                    struct gui_fs_dirent *entry;
                    char *path = gui_malloc(strlen(folder) + strlen(GUI_ROOT_FOLDER) + 1);
                    sprintf(path, "%s%s", GUI_ROOT_FOLDER, folder);
                    if ((dir = gui_fs_opendir(path)) == NULL)
                    {
                        GUI_ASSERT(0);
                    }
                    gui_free(path);
                    while ((entry = gui_fs_readdir(dir)) != NULL)
                    {
                        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                        {
                            file_count++;
                        }


                    }
                    gui_fs_closedir(dir);
                }
                void **image_array = gui_malloc(file_count * sizeof(void *));
                {
                    gui_fs_dir *dir = 0;
                    struct gui_fs_dirent *entry;
                    char *path = gui_malloc(strlen(folder) + strlen(GUI_ROOT_FOLDER) + 1);
                    sprintf(path, "%s%s", GUI_ROOT_FOLDER, folder);
                    if ((dir = gui_fs_opendir(path)) == NULL)
                    {
                        gui_free(path);
                        return 0;
                    }

                    int count = 0;
                    while ((entry = gui_fs_readdir(dir)) != NULL)
                    {
                        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                        {
                            char *path2 = gui_malloc(strlen(entry->d_name) + strlen(folder) + 2);
                            sprintf(path2, "%s/%s", folder, entry->d_name);
                            image_array[count++] = (void *)gui_get_image_file_address(path2);
                        }

                    }
                    gui_free(path);
                    gui_fs_closedir(dir);
                    GUI_API(gui_switch_t).on_press((void *)parent, (gui_event_cb_t)switch_press_array,
                                                   (void *)(uintptr_t)file_count);
                    GUI_API(gui_switch_t).animate((void *)parent, dur, 0, switch_press_ani_cb_array, 0);
                    GUI_TYPE(gui_switch_t, parent)->animate->animate = 0;
                    GUI_TYPE(gui_switch_t, parent)->data = image_array;
                    GUI_TYPE(gui_switch_t, parent)->on_pic_addr = image_array[file_count - 1];

                }
            }


        }
        bool if_widget = 0;
        if (p->parent)
        {
            if (!strcmp(widget[21].name, p->parent->name))
            {
                if_widget = 1;
            }
        }
        if (if_widget)
        {
            if (open_switch_name && !strcmp(parent->name, open_switch_name))
            {
                gui_free(open_switch_name);
                open_switch_name = 0;
                GUI_API(gui_switch_t).on_turn_on((gui_switch_t *)parent,
                                                 (gui_event_cb_t)switch_cb_for_mororized_curtain, (void *)0);
                GUI_API(gui_switch_t).on_turn_off((gui_switch_t *)parent,
                                                  (gui_event_cb_t)switch_cb_for_mororized_curtain, (void *)0);
            }
            else if (close_switch_name && !strcmp(parent->name, close_switch_name))
            {
                gui_free(close_switch_name);
                close_switch_name = 0;
                GUI_API(gui_switch_t).on_turn_on((gui_switch_t *)parent,
                                                 (gui_event_cb_t)switch_cb_for_mororized_curtain, (void *)1);
                GUI_API(gui_switch_t).on_turn_off((gui_switch_t *)parent,
                                                  (gui_event_cb_t)switch_cb_for_mororized_curtain, (void *)1);
            }
            else if (pause_switch_name && !strcmp(parent->name, pause_switch_name))
            {
                gui_free(pause_switch_name);
                pause_switch_name = 0;
                GUI_API(gui_switch_t).on_turn_on((gui_switch_t *)parent,
                                                 (gui_event_cb_t)switch_cb_for_mororized_curtain, (void *)2);
                GUI_API(gui_switch_t).on_turn_off((gui_switch_t *)parent,
                                                  (gui_event_cb_t)switch_cb_for_mororized_curtain, (void *)3);
            }
        }

    }

    return parent;
}
static gui_obj_t *widget_create_gallery(ezxml_t p, gui_obj_t *parent, T_OBJ_TYPE widget_type)
{

    {
        size_t i = 0;
        int16_t x = 0;
        int16_t y = 0;
        int16_t w = 0;
        int16_t h = 0;

        gui_gallery_config_t config = {0};
        char *picture = "app/system/resource/Progress bar_full.bin"; GUI_UNUSED(picture);
        char *folder = NULL;


        uint8_t opacity = 255; GUI_UNUSED(opacity);

        memset(&config, 0, sizeof(config));
        while (true)
        {
            // #define IS_STR(str) (!strcmp(p->attr[i], str))
            if (!(p->attr[i]))
            {
                break;
            }
            if (!strcmp(p->attr[i], "x"))
            {
                x = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "y"))
            {
                y = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "w"))
            {
                w = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "h"))
            {
                h = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "folder"))
            {
                folder = gui_strdup(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "mainBg"))
            {
                config.data_main_bg = gui_strdup(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "centerBg"))
            {
                config.data_center_bg = gui_strdup(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "centerPercent"))
            {
                config.center_percent = atof(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "sideScale"))
            {
                config.side_scale = atof(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "sidePosPercent"))
            {
                config.side_pos_percent = atof(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "opacity"))
            {
                opacity = atof(p->attr[++i]);
            }
            i++;
        }

        char *ptxt = get_space_string_head(p->txt);
        if (!folder)
        {
            gui_log("gallery folder is not found!\n");
        }
        else if (folder)
        {
            int file_count = 0;
            gui_fs_dir *dir = 0;
            struct gui_fs_dirent *entry;
            char *dir_path = gui_malloc(strlen(folder) + strlen(GUI_ROOT_FOLDER) + 1);

            // init image array
            sprintf(dir_path, "%s%s", GUI_ROOT_FOLDER, folder);
            if ((dir = gui_fs_opendir(dir_path)) == NULL)
            {
                gui_free(dir_path);
                return 0;
            }
            gui_free(dir_path);

            while ((entry = gui_fs_readdir(dir)) != NULL)
            {
                if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                {
                    char *file_path = gui_malloc(strlen(entry->d_name) + strlen(folder) + 2);
                    void *addr = NULL;
                    void *data = NULL;

                    file_count++;
                    sprintf(file_path, "%s/%s", folder, entry->d_name);
                    addr = (void *)gui_get_image_file_address(file_path);
                    if (!config.img_array)
                    {
                        data = gui_malloc(sizeof(uint8_t *));
                    }
                    else
                    {
                        data = gui_realloc(config.img_array, file_count * (sizeof(void *)));
                    }
                    config.img_array = data;
                    config.img_array[file_count - 1] = addr;

                    gui_free(file_path);
                }
            }
            gui_fs_closedir(dir);

            config.num_pic = file_count;
        }

        if (config.data_main_bg)
        {
            config.data_main_bg = (void *)gui_get_image_file_address(config.data_main_bg);
        }
        if (config.data_center_bg)
        {
            config.data_center_bg = (void *)gui_get_image_file_address(config.data_center_bg);
        }

        gui_gallery_create(parent, ptxt, &config,
                           x, y, w, h);
    }

    return parent;
}
static gui_obj_t *widget_create_keyboard(ezxml_t p, gui_obj_t *parent, T_OBJ_TYPE widget_type)
{

    {
        size_t i = 0;
        gui_kb_config_t config = {0};
        char *picture = "app/system/resource/Progress bar_full.bin"; GUI_UNUSED(picture);
        // char *folder = NULL;

        // default image blend_mode
        memset(&config, 0, sizeof(config));

        while (true)
        {
            if (!(p->attr[i]))
            {
                break;
            }
            if (!strcmp(p->attr[i], "ime"))
            {
                i++;
                if (!strcmp(p->attr[i], "pinyin"))
                {
                    config.ime = KB_METHOD_PINYIN;
                }
            }
            i++;
        }

        {
#define PATH_PRE "app/system/resource/"
            char *floder_kb = PATH_PRE"keyboard"; GUI_UNUSED(floder_kb);
            char *floder_letter = PATH_PRE"keyboard/0_letter/";
            char *floder_letter_upper = PATH_PRE"keyboard/1_letter_upper/";
            char *floder_num = PATH_PRE"keyboard/2_number/";
            char *floder_symbol = PATH_PRE"keyboard/3_symbol/";
            char *floder_func = PATH_PRE"keyboard/4_func/";
            char *floder_other = PATH_PRE"keyboard/5_other/";

            config.layout = KB_LAYOUT_BASIC;
            config.mode = KB_MODE_BASIC_ENG_LOWWER;

            uint16_t file_count = 0;
            gui_fs_dir *dir = NULL;
            struct gui_fs_dirent *entry;
            char *folder_array[] = {floder_letter, floder_letter_upper, floder_num, floder_symbol, floder_func, floder_other};
            for (int i = 0; i < sizeof(folder_array) / sizeof(folder_array[0]); i++)
            {
                char *folder = folder_array[i];
                char *dir_path = gui_malloc(strlen(folder) + strlen(GUI_ROOT_FOLDER) + 1);

                // init image array
                sprintf(dir_path, "%s%s", GUI_ROOT_FOLDER, folder);
                if ((dir = gui_fs_opendir(dir_path)) == NULL)
                {
                    gui_free(dir_path);
                    continue;
                    //perror("gui_fs_opendir() failed"); return;
                }
                // gui_log("folder: %d %s\n", dir, folder);
                gui_free(dir_path);


                while ((entry = gui_fs_readdir(dir)) != NULL)
                {
                    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                    {
                        char *file_path = gui_malloc(strlen(entry->d_name) + strlen(folder) + 2);
                        void *addr = NULL;
                        void *data = NULL;

                        file_count++;
                        sprintf(file_path, "%s/%s", folder, entry->d_name);
                        // gui_log("file_count %d, file: %s\n", file_count, entry->d_name);
                        addr = (void *)gui_get_image_file_address(file_path);
                        if (!config.img_array)
                        {
                            data = gui_malloc(sizeof(uint8_t *));
                        }
                        else
                        {
                            data = gui_realloc(config.img_array, file_count * (sizeof(void *)));
                        }
                        config.img_array = data;
                        config.img_array[file_count - 1] = addr;

                        gui_free(file_path);
                    }

                }
                gui_fs_closedir(dir);
                // gui_log("file_count %d, folder: %s\n", file_count, folder);
                if (0 == strcmp(folder, floder_symbol))
                {
                    config.file_mark[0] = file_count;
                }
                else if (0 == strcmp(folder, floder_func))
                {
                    config.file_mark[1] = file_count;
                }
            }
            config.num_pic = file_count;
        }

        gui_keyboard_create(parent, "test", &config,
                            0, 0, gui_get_screen_width(), gui_get_screen_height());
    }

    return parent;
}
static gui_obj_t *widget_create_javascript(ezxml_t p, gui_obj_t *parent, T_OBJ_TYPE widget_type)
{
    {
        if (!strcmp(p->attr[0], "file"))
        {
            js = gui_strdup(p->attr[1]);
        }

    }
    return parent;
}
static gui_obj_t *widget_create_macro_motorized_curtain(ezxml_t p, gui_obj_t *parent,
                                                        T_OBJ_TYPE widget_type)
{

    {
        size_t i = 0;
        int16_t x = 0;
        int16_t y = 0;
        int16_t w = 0;
        int16_t h = 0;
        while (true)
        {
            if (!(p->attr[i]))
            {
                break;
            }
            ////gui_log("p->attr[i]:%s\n", p->attr[i]);
            if (!strcmp(p->attr[i], "x"))
            {
                x = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "y"))
            {
                y = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "w"))
            {
                w = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "h"))
            {
                h = atoi(p->attr[++i]);
            }
            if (!strcmp(p->attr[i], "switchOpen"))
            {
                open_switch_name = gui_strdup(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "switchClose"))
            {
                close_switch_name = gui_strdup(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "switchPause"))
            {
                pause_switch_name = gui_strdup(p->attr[++i]);
            }
            i++;
        }
        char *ptxt = get_space_string_head(p->txt);
        //gui_log("x:%d,y:%d,w:%dh:%d,orientation:%d\n", x, y, w, h, orientation);
        parent = (void *)gui_win_create(parent, ptxt, x, y, w, h);
    }

    return parent;
}
static gui_obj_t *widget_create_multi_level(ezxml_t p, gui_obj_t *parent, T_OBJ_TYPE widget_type)
{
    {
        char *ptxt = get_space_string_head(p->txt);
        static unsigned char ml_count;
        char *group_name = 0;
        //gui_log("x:%d,y:%d,w:%dh:%d,orientation:%d\n", x, y, w, h, orientation);
        size_t i = 0;
        while (true)
        {
            if (!(p->attr[i]))
            {
                break;
            }
            ////gui_log("p->attr[i]:%s\n", p->attr[i]);
            if (!strcmp(p->attr[i], "name"))
            {
                group_name = (p->attr[++i]);
            }
            // else if (!strcmp(p->attr[i], "switchPause"))
            // {
            //     pause_switch_name = gui_strdup(p->attr[++i]);
            // }
            i++;
        }
        if (group_name && strlen(group_name) == 0)
        {
            group_name = 0;
        }
        else if (group_name)
        {
            group_name = gui_strdup(group_name);
        }

        if (ptxt && ptxt[0] == 0)
        {
            gui_free(ptxt);
            ptxt = 0;
        }
        if (!ptxt)
        {

            char buffer[20];
            sprintf(buffer, "__ml%d_", ml_count++);
            ptxt = gui_strdup(buffer);
        }

        parent = (void *)gui_multi_level_create_group(parent, ptxt,
                                                      (void(*)(gui_obj_t *))multi_level_ui_design, group_name);


    }
    return parent;
}
/**
 * Extract the substring after the second comma, trimming leading and trailing spaces
 * @param id Input string (e.g., "1,0,    ml0 ")
 * @param param Structure containing to_widget_name to store the result
 */
static void parse_id_string(const char *id, struct on_click_jump_cb_param *param)
{
    if (id == NULL || param == NULL)
    {
        gui_log("Invalid input: id or param is NULL\n");
        return;
    }

    // Find the first comma in the string
    const char *first_comma = strchr(id, ',');
    if (first_comma == NULL)
    {
        gui_log("The first comma is not found in '%s'\n", id);
        param->to_widget_name = 0;
        return;
    }

    // Find the second comma starting from the character after the first comma
    const char *second_comma = strchr(first_comma + 1, ',');
    if (second_comma == NULL || *(second_comma + 1) == '\0')
    {
        gui_log("The second comma does not exist or is at the end in '%s'\n", id);
        param->to_widget_name = 0;
        return;
    }

    // Start after the second comma
    const char *value_start = second_comma + 1;

    // Skip leading spaces
    while (isspace((unsigned char)*value_start))
    {
        value_start++;
    }
    if (*value_start == '\0')
    {
        gui_log("No non-space content after second comma in '%s'\n", id);
        param->to_widget_name = 0;
        return;
    }

    // Find the end of the value (string end)
    const char *value_end = value_start + strlen(value_start);

    // Trim trailing spaces
    while (value_end > value_start && isspace((unsigned char) * (value_end - 1)))
    {
        value_end--;
    }

    // Calculate length of the trimmed string
    size_t value_length = value_end - value_start;
    if (value_length == 0)
    {
        gui_log("Empty value after trimming in '%s'\n", id);
        param->to_widget_name = 0;
        return;
    }

    // Duplicate the trimmed string
    char trimmed_value[value_length + 1] ; // Assume gui_strdup allocates new memory

    strncpy(trimmed_value, value_start, value_length);
    trimmed_value[value_length] = '\0';

    param->to_widget_name = gui_strdup(trimmed_value);
}
static gui_obj_t *widget_create_macro_onclick(ezxml_t p, gui_obj_t *parent, T_OBJ_TYPE widget_type)
{

    {
        char *type = 0;
        char *to = 0;
        char *id = 0;
        int x = 0;
        int y = 0;
        size_t i = 0;
        while (true)
        {
            //gui_log("p->attr[i]:%x\n",p->attr[i]);
            if (!(p->attr[i]))
            {
                break;
            }
            //gui_log("p->attr[i]:%s,\n", p->attr[i]);
            if (!strcmp(p->attr[i], "type"))
            {
                type = (p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "to"))
            {
                to = (p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "id1"))
            {
                x = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "id"))
            {
                id = p->attr[++i];
            }
            else if (!strcmp(p->attr[i], "id2"))
            {
                y = atoi(p->attr[++i]);
            }
            i++;
        }
        int to_widget = 0; GUI_UNUSED(to_widget);
        if (type && to)
        {
            if (!strcmp(type, "jump"))
            {
                {


                    //to
                    if (!strcmp(to, "multiLevel"))
                    {
                        //GUI_API(gui_multi_level_t).jump(parent, x, y);
                        struct on_click_jump_cb_param *param;
                        param = gui_malloc(sizeof(struct on_click_jump_cb_param));
                        if (id)
                        {
                            get_2_int_from_string(id, &x, &y);
                        }
                        parse_id_string(id, param);

                        param->id1 = x;
                        param->id2 = y;
                        if (parent->type == BUTTON)
                        {
                            GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)on_click_jump_cb, param);
                        }
                        else if (parent->type == WINDOW)
                        {
                            gui_win_click((gui_win_t *)parent, (gui_event_cb_t)on_click_jump_cb, param);
                        }
                        else if (parent->type == MACRO_KEY)
                        {
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_cb, GUI_EVENT_6, param);
                        }
                        else if (parent->type == MACRO_PAGE_LIST_NEW)
                        {
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_cb_wifi, GUI_EVENT_6, param);

                            gui_pagelist_new_t *pl = (void *)parent;
                            pl->click_param = gui_malloc(sizeof(void *)*pl->item_count);
                            for (size_t i = 0; i < pl->item_count; i++)
                            {
                                pl->click_function_array[i] = (gui_event_cb_t)on_click_jump_cb_wifi;
                                pl->click_param[i] = param;
                            }
                        }
                    }

                    else if (!strcmp(to, "app"))
                    {
                        if (ends_with_xml(id))
                        {
                            if (parent->type == BUTTON)
                            {
                                GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)on_click_jump_to_app_cb,
                                                               (void *)gui_get_path_by_relative(id));
                            }
                            else if (parent->type == WINDOW)
                            {
                                gui_win_click((gui_win_t *)parent, (gui_event_cb_t)on_click_jump_to_app_cb, gui_strdup(id));
                            }
                            else if (parent->type == MACRO_KEY)
                            {
                                gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_to_app_cb, GUI_EVENT_6, gui_strdup(id));
                            }
                        }
                        else
                        {
                            gui_log("[SaaA] error app jump format\n");
                        }



                    }
                    else if (!strcmp(to, "C-APP"))
                    {
                        if (parent->type == BUTTON)
                        {
                            GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)on_click_jump_to_capp_cb,
                                                           gui_app_get_by_name(id));
                        }
                        else if (parent->type == WINDOW)
                        {
                            gui_win_click((gui_win_t *)parent, (gui_event_cb_t)on_click_jump_to_capp_cb, 0);
                        }
                        else if (parent->type == MACRO_KEY)
                        {
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_to_capp_cb, GUI_EVENT_6,
                                                 gui_app_get_by_name(id));
                        }
                    }
                    else if (!strcmp(to, "tabview") || !strcmp(to, "tab"))
                    {
                        //GUI_API(gui_multi_level_t).jump(parent, x, y);
                        struct on_click_jump_cb_param *param;
                        param = gui_malloc(sizeof(struct on_click_jump_cb_param));
                        char *tabview_name = 0;
                        if (id)
                        {
                            get_2_int_from_string(id, &x, &y);

                            {
                                // Find the first comma in the string
                                const char *first_comma = strchr(id, ',');

                                if (first_comma != NULL)
                                {
                                    // Find the second comma starting from the character after the first comma
                                    const char *second_comma = strchr(first_comma + 1, ',');

                                    // If the second comma was found and it's not at the end of the string
                                    if (second_comma != NULL && *(second_comma + 1) != '\0')
                                    {
                                        tabview_name = gui_strdup(second_comma + 1);
                                    }
                                    else
                                    {
                                        // Handle the case where the second comma is not found or is at the end
                                        gui_log("The second comma does not exist or is at the end.\n");
                                    }
                                }
                                else
                                {
                                    // Handle the case where the first comma is not found
                                    gui_log("The first comma is not found.\n");
                                }
                            }
                        }

                        param->id1 = x;
                        param->id2 = y;
                        param->to_widget_name = tabview_name;
                        if (parent->type == BUTTON)
                        {
                            GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)on_click_jump_cb_tabview,
                                                           param);
                        }
                        else if (parent->type == WINDOW)
                        {
                            gui_win_click((gui_win_t *)parent, (gui_event_cb_t)on_click_jump_cb_tabview, param);
                        }
                        else if (parent->type == MACRO_KEY)
                        {
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_cb_tabview, GUI_EVENT_6, param);
                        }
                    }

                }

            }
            else if (!strcmp(type, "control"))
            {
                if (!strcmp(to, "light"))
                {
                    light_param_t *light;
                    light = gui_malloc(sizeof(light_param_t));
                    light->id = x;
                    light->state = (bool)y;
                    if (parent->type == BUTTON)
                    {
                        GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)light_control_cb, light);
                    }
                    else if (parent->type == WINDOW)
                    {
                        gui_win_click((gui_win_t *)parent, (gui_event_cb_t)light_control_cb, light);
                    }
                    else if (parent->type == CLICKSWITCH)
                    {
                        GUI_API(gui_switch_t).on_turn_on((gui_switch_t *)parent,
                                                         (gui_event_cb_t)light_switch_on_cb, light);
                        GUI_API(gui_switch_t).on_turn_off((gui_switch_t *)parent,
                                                          (gui_event_cb_t)light_switch_off_cb, light);
                    }
                }

                //gui_log("p->attr[i]:%x\n", (size_t)(p->attr[i]));

            }
            else if ((!strcmp(type, "animatePause")) || (!strcmp(type, "animate")))
            {
                char **param = gui_malloc(sizeof(char *) * 3);
                param[0] = gui_strdup(to);
                param[2] = (void *)parent;
                if (id)
                {
                    param[1] = gui_strdup(id);
                }
                if (!strcmp(type, "animatePause"))
                {
                    if (parent->type == BUTTON)
                    {
                        GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)pause_animation_cb, param);
                    }
                    else if (parent->type == WINDOW)
                    {
                        gui_win_click((gui_win_t *)parent, (gui_event_cb_t)pause_animation_cb, param);
                    }
                    else if (parent->type == CLICKSWITCH)
                    {
                        GUI_API(gui_switch_t).on_turn_on(GUI_TYPE(gui_switch_t, parent), (gui_event_cb_t)pause_animation_cb,
                                                         (param));
                        GUI_API(gui_switch_t).on_turn_off(GUI_TYPE(gui_switch_t, parent),
                                                          (gui_event_cb_t)pause_animation_cb,
                                                          (param));
                    }

                }
                else if (!strcmp(type, "animate"))
                {
                    if (parent->type == BUTTON)
                    {
                        GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)start_animation_cb, param);
                    }
                    else if (parent->type == WINDOW)
                    {
                        gui_win_click((gui_win_t *)parent, (gui_event_cb_t)start_animation_cb, param);
                    }
                    else if (parent->type == CLICKSWITCH)
                    {
                        GUI_API(gui_switch_t).on_turn_on(GUI_TYPE(gui_switch_t, parent), (gui_event_cb_t)start_animation_cb,
                                                         (param));
                        GUI_API(gui_switch_t).on_turn_off(GUI_TYPE(gui_switch_t, parent),
                                                          (gui_event_cb_t)start_animation_cb,
                                                          (param));
                    }

                }
            }
            else if (!strcmp(type, "keyboard"))
            {
                if (to && strlen(to) > 0)
                {
                    GUI_WIDGET_POINTER_BY_NAME(to_text, to);
                    if (to_text->type == TEXTBOX)
                    {
                        gui_text_t *widget_to_text = GUI_TYPE(gui_text_t, to_text);
                        char *content = widget_to_text->content;
                        struct on_click_keyboard_cb_param *param;
                        param = gui_malloc(sizeof(struct on_click_keyboard_cb_param));

                        if (parent->type == BUTTON)
                        {
                            param->to_text = (void *)to_text;
                            param->key_board = get_scan_code_by_keyboard_string(id);
                            if (param->key_board != SCANCODE_UNKNOWN)
                            {
                                GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)on_click_keyboard_cb, param);
                            }
                        }

                    }


                }

                //gui_log("p->attr[i]:%x\n", (size_t)(p->attr[i]));

            }
            else if (!strcmp(type, "connect"))
            {
                if (!strcmp(to, "wifi"))
                {
                    if (parent->type == BUTTON)
                    {
                        bool multi_click = false;
                        for (uint8_t i = 0; i < parent->event_dsc_cnt; i++)
                        {
                            gui_event_dsc_t *event_dsc = parent->event_dsc + i;
                            if (event_dsc->filter == GUI_EVENT_TOUCH_CLICKED)
                            {
                                multi_click = true;
                                break;
                            }
                        }
                        if (multi_click)
                        {
                            gui_win_t *win = gui_win_create(parent, "_win_for_multiclick", 0, 0, parent->w, parent->h);
                            gui_win_click(win, wifi_connect, 0);
                        }
                        else


                        {
                            GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)wifi_connect, 0);
                        }
                    }


                }

                //gui_log("p->attr[i]:%x\n", (size_t)(p->attr[i]));

            }
            else if (!strcmp(type, "input"))
            {
                if (!strcmp(to, "text") && id)
                {
                    if (parent->type == BUTTON)
                    {
                        bool multi_click = false;
                        for (uint8_t i = 0; i < parent->event_dsc_cnt; i++)
                        {
                            gui_event_dsc_t *event_dsc = parent->event_dsc + i;
                            if (event_dsc->filter == GUI_EVENT_TOUCH_CLICKED)
                            {
                                multi_click = true;
                                break;
                            }
                        }
                        if (multi_click)
                        {
                            gui_win_t *win = gui_win_create(parent, "_win_for_multiclick", 0, 0, parent->w, parent->h);
                            gui_win_click(win, text_input, gui_strdup(id));
                        }
                        else


                        {
                            GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)text_input, gui_strdup(id));
                        }
                    }
                    else if (parent->type == WINDOW)
                    {
                        bool multi_click = false;
                        for (uint8_t i = 0; i < parent->event_dsc_cnt; i++)
                        {
                            gui_event_dsc_t *event_dsc = parent->event_dsc + i;
                            if (event_dsc->filter == GUI_EVENT_TOUCH_CLICKED)
                            {
                                multi_click = true;
                                break;
                            }
                        }
                        if (multi_click)
                        {
                            gui_win_t *win = gui_win_create(parent, "_win_for_multiclick", 0, 0, parent->w, parent->h);
                            gui_win_click(win, text_input, gui_strdup(id));
                        }
                        else


                        {
                            gui_win_click((gui_win_t *)parent, text_input, gui_strdup(id));

                        }
                    }

                }

                //gui_log("p->attr[i]:%x\n", (size_t)(p->attr[i]));

            }
            else if (!strcmp(type, "no input"))
            {
                if (!strcmp(to, "text"))
                {
                    if (parent->type == BUTTON)
                    {
                        bool multi_click = false;
                        for (uint8_t i = 0; i < parent->event_dsc_cnt; i++)
                        {
                            gui_event_dsc_t *event_dsc = parent->event_dsc + i;
                            if (event_dsc->filter == GUI_EVENT_TOUCH_CLICKED)
                            {
                                multi_click = true;
                                break;
                            }
                        }
                        if (multi_click)
                        {
                            gui_win_t *win = gui_win_create(parent, "_win_for_multiclick", 0, 0, parent->w, parent->h);
                            gui_win_click(win, text_no_input, gui_strdup(id));
                        }
                        else


                        {
                            GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)text_no_input,
                                                           gui_strdup(id));
                        }
                    }
                    else if (parent->type == WINDOW)
                    {
                        bool multi_click = false;
                        for (uint8_t i = 0; i < parent->event_dsc_cnt; i++)
                        {
                            gui_event_dsc_t *event_dsc = parent->event_dsc + i;
                            if (event_dsc->filter == GUI_EVENT_TOUCH_CLICKED)
                            {
                                multi_click = true;
                                break;
                            }
                        }
                        if (multi_click)
                        {
                            gui_win_t *win = gui_win_create(parent, "_win_for_multiclick", 0, 0, parent->w, parent->h);
                            gui_win_click(win, text_no_input, gui_strdup(id));
                        }
                        else


                        {
                            gui_win_click((gui_win_t *)parent, text_no_input, gui_strdup(id));

                        }
                    }

                }

                //gui_log("p->attr[i]:%x\n", (size_t)(p->attr[i]));

            }
        }
    }

    return parent;
}
static gui_obj_t *widget_create_macro_on_connected(ezxml_t p, gui_obj_t *parent,
                                                   T_OBJ_TYPE widget_type)
{

    {
        char *type = 0;
        char *to = 0;
        char *id = 0;
        int x = 0;
        int y = 0;
        size_t i = 0;
        while (true)
        {
            //gui_log("p->attr[i]:%x\n",p->attr[i]);
            if (!(p->attr[i]))
            {
                break;
            }
            //gui_log("p->attr[i]:%s,\n", p->attr[i]);
            if (!strcmp(p->attr[i], "type"))
            {
                type = (p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "to"))
            {
                to = (p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "id1"))
            {
                x = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "id"))
            {
                id = p->attr[++i];
            }
            else if (!strcmp(p->attr[i], "id2"))
            {
                y = atoi(p->attr[++i]);
            }
            i++;
        }
        int to_widget = 0; GUI_UNUSED(to_widget);
        if (type && to)
        {
            if (!strcmp(type, "jump"))
            {
                {


                    //to
                    if (!strcmp(to, "multiLevel"))
                    {
                        //GUI_API(gui_multi_level_t).jump(parent, x, y);
                        struct on_click_jump_cb_param *param;
                        param = gui_malloc(sizeof(struct on_click_jump_cb_param));
                        if (id)
                        {
                            get_2_int_from_string(id, &x, &y);
                        }
                        parse_id_string(id, param);

                        param->id1 = x;
                        param->id2 = y;
                        if (parent->type == BUTTON)
                        {
                            GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)on_click_jump_cb, param);
                        }
                        else if (parent->type == WINDOW)
                        {
                            gui_win_click((gui_win_t *)parent, (gui_event_cb_t)on_click_jump_cb, param);
                        }
                        else if (parent->type == MACRO_KEY)
                        {
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_cb, GUI_EVENT_6, param);
                        }
                        else if (parent->type == MACRO_WIFI)
                        {
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_cb, GUI_EVENT_6, param);
                        }
                        else if (parent->type == MACRO_PAGE_LIST_NEW)
                        {
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_cb_wifi, GUI_EVENT_6, param);

                            gui_pagelist_new_t *pl = (void *)parent;
                            pl->click_param = gui_malloc(sizeof(void *)*pl->item_count);
                            for (size_t i = 0; i < pl->item_count; i++)
                            {
                                pl->click_function_array[i] = (gui_event_cb_t)on_click_jump_cb_wifi;
                                pl->click_param[i] = param;
                            }
                        }
                    }

                    else if (!strcmp(to, "app"))
                    {
                        if (ends_with_xml(id))
                        {
                            if (parent->type == BUTTON)
                            {
                                GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)on_click_jump_to_app_cb,
                                                               (void *)gui_get_path_by_relative(id));
                            }
                            else if (parent->type == WINDOW)
                            {
                                gui_win_click((gui_win_t *)parent, (gui_event_cb_t)on_click_jump_to_app_cb, gui_strdup(id));
                            }
                            else if (parent->type == MACRO_KEY)
                            {
                                gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_to_app_cb, GUI_EVENT_6, gui_strdup(id));
                            }
                            else if (parent->type == MACRO_WIFI)
                            {
                                gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_to_app_cb, GUI_EVENT_6, gui_strdup(id));
                            }
                        }
                        else
                        {
                            gui_log("[SaaA] error app jump format\n");
                        }



                    }
                    else if (!strcmp(to, "C-APP"))
                    {
                        if (parent->type == BUTTON)
                        {
                            GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)on_click_jump_to_capp_cb,
                                                           gui_app_get_by_name(id));
                        }
                        else if (parent->type == WINDOW)
                        {
                            gui_win_click((gui_win_t *)parent, (gui_event_cb_t)on_click_jump_to_capp_cb, 0);
                        }
                        else if (parent->type == MACRO_KEY)
                        {
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_to_capp_cb, GUI_EVENT_6,
                                                 gui_app_get_by_name(id));
                        }
                        else if (parent->type == MACRO_WIFI)
                        {
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_to_capp_cb, GUI_EVENT_6,
                                                 gui_app_get_by_name(id));
                        }
                    }
                    else if (!strcmp(to, "tabview") || !strcmp(to, "tab"))
                    {
                        //GUI_API(gui_multi_level_t).jump(parent, x, y);
                        struct on_click_jump_cb_param *param;
                        param = gui_malloc(sizeof(struct on_click_jump_cb_param));
                        char *tabview_name = 0;
                        if (id)
                        {
                            get_2_int_from_string(id, &x, &y);

                            {
                                // Find the first comma in the string
                                const char *first_comma = strchr(id, ',');

                                if (first_comma != NULL)
                                {
                                    // Find the second comma starting from the character after the first comma
                                    const char *second_comma = strchr(first_comma + 1, ',');

                                    // If the second comma was found and it's not at the end of the string
                                    if (second_comma != NULL && *(second_comma + 1) != '\0')
                                    {
                                        tabview_name = gui_strdup(second_comma + 1);
                                    }
                                    else
                                    {
                                        // Handle the case where the second comma is not found or is at the end
                                        gui_log("The second comma does not exist or is at the end.\n");
                                    }
                                }
                                else
                                {
                                    // Handle the case where the first comma is not found
                                    gui_log("The first comma is not found.\n");
                                }
                            }
                        }

                        param->id1 = x;
                        param->id2 = y;
                        param->to_widget_name = tabview_name;
                        if (parent->type == BUTTON)
                        {
                            GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)on_click_jump_cb_tabview,
                                                           param);
                        }
                        else if (parent->type == WINDOW)
                        {
                            gui_win_click((gui_win_t *)parent, (gui_event_cb_t)on_click_jump_cb_tabview, param);
                        }
                        else if (parent->type == MACRO_KEY)
                        {
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_cb_tabview, GUI_EVENT_6, param);
                        }
                        else if (parent->type == MACRO_WIFI)
                        {
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_cb_tabview, GUI_EVENT_6, param);
                        }
                    }

                }

            }
            else if (!strcmp(type, "control"))
            {
                if (!strcmp(to, "light"))
                {
                    light_param_t *light;
                    light = gui_malloc(sizeof(light_param_t));
                    light->id = x;
                    light->state = (bool)y;
                    if (parent->type == BUTTON)
                    {
                        GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)light_control_cb, light);
                    }
                    else if (parent->type == WINDOW)
                    {
                        gui_win_click((gui_win_t *)parent, (gui_event_cb_t)light_control_cb, light);
                    }
                    else if (parent->type == CLICKSWITCH)
                    {
                        GUI_API(gui_switch_t).on_turn_on((gui_switch_t *)parent,
                                                         (gui_event_cb_t)light_switch_on_cb, light);
                        GUI_API(gui_switch_t).on_turn_off((gui_switch_t *)parent,
                                                          (gui_event_cb_t)light_switch_off_cb, light);
                    }
                }

                //gui_log("p->attr[i]:%x\n", (size_t)(p->attr[i]));

            }
            else if ((!strcmp(type, "animatePause")) || (!strcmp(type, "animate")))
            {
                char **param = gui_malloc(sizeof(char *) * 3);
                param[0] = gui_strdup(to);
                param[2] = (void *)parent;
                if (id)
                {
                    param[1] = gui_strdup(id);
                }
                if (!strcmp(type, "animatePause"))
                {
                    if (parent->type == BUTTON)
                    {
                        GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)pause_animation_cb, param);
                    }
                    else if (parent->type == WINDOW)
                    {
                        gui_win_click((gui_win_t *)parent, (gui_event_cb_t)pause_animation_cb, param);
                    }
                    else if (parent->type == CLICKSWITCH)
                    {
                        GUI_API(gui_switch_t).on_turn_on(GUI_TYPE(gui_switch_t, parent), (gui_event_cb_t)pause_animation_cb,
                                                         (param));
                        GUI_API(gui_switch_t).on_turn_off(GUI_TYPE(gui_switch_t, parent),
                                                          (gui_event_cb_t)pause_animation_cb,
                                                          (param));
                    }

                }
                else if (!strcmp(type, "animate"))
                {
                    if (parent->type == BUTTON)
                    {
                        GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)start_animation_cb, param);
                    }
                    else if (parent->type == WINDOW)
                    {
                        gui_win_click((gui_win_t *)parent, (gui_event_cb_t)start_animation_cb, param);
                    }
                    else if (parent->type == CLICKSWITCH)
                    {
                        GUI_API(gui_switch_t).on_turn_on(GUI_TYPE(gui_switch_t, parent), (gui_event_cb_t)start_animation_cb,
                                                         (param));
                        GUI_API(gui_switch_t).on_turn_off(GUI_TYPE(gui_switch_t, parent),
                                                          (gui_event_cb_t)start_animation_cb,
                                                          (param));
                    }

                }
            }
            else if (!strcmp(type, "keyboard"))
            {
                if (to && strlen(to) > 0 && id)
                {
                    GUI_WIDGET_POINTER_BY_NAME(to_text, to);
                    if (to_text->type == TEXTBOX)
                    {
                        gui_text_t *widget_to_text = GUI_TYPE(gui_text_t, to_text);
                        char *content = widget_to_text->content;
                        struct on_click_keyboard_cb_param *param;
                        param = gui_malloc(sizeof(struct on_click_keyboard_cb_param));

                        if (parent->type == BUTTON)
                        {
                            param->to_text = (void *)to_text;
                            param->key_board = get_scan_code_by_keyboard_string(id);
                            if (param->key_board != SCANCODE_UNKNOWN)
                            {
                                GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)on_click_keyboard_cb, param);
                            }
                        }

                    }


                }

                //gui_log("p->attr[i]:%x\n", (size_t)(p->attr[i]));

            }
            else if (!strcmp(type, "connect"))
            {
                if (!strcmp(to, "wifi"))
                {
                    if (parent->type == BUTTON)
                    {
                        bool multi_click = false;
                        for (uint8_t i = 0; i < parent->event_dsc_cnt; i++)
                        {
                            gui_event_dsc_t *event_dsc = parent->event_dsc + i;
                            if (event_dsc->filter == GUI_EVENT_TOUCH_CLICKED)
                            {
                                multi_click = true;
                                break;
                            }
                        }
                        if (multi_click)
                        {
                            gui_win_t *win = gui_win_create(parent, "_win_for_multiclick", 0, 0, parent->w, parent->h);
                            gui_win_click(win, wifi_connect, 0);
                        }
                        else
                        {
                            GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)wifi_connect, 0);
                        }
                    }
                }
            }
            else if (!strcmp(type, "highlight"))
            {
                if (!strcmp(to, "listview"))
                {
                    if (parent->type == MACRO_WIFI)
                    {
                        bool multi_click = false;
                        for (uint8_t i = 0; i < parent->event_dsc_cnt; i++)
                        {
                            gui_event_dsc_t *event_dsc = parent->event_dsc + i;
                            if (event_dsc->filter == GUI_EVENT_6)
                            {
                                multi_click = true;
                                break;
                            }
                        }
                        if (multi_click)
                        {

                            gui_win_t *win = gui_win_create(parent, "_win_for_multiclick", 0, 0, parent->w, parent->h);
                            gui_win_set_animate(win, 10000, -1, win_wifi_ani, 0);

                            GUI_BASE(win)->type = MACRO_WIFI;
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)on_connect_wifi_listview_cb, GUI_EVENT_6,
                                                 gui_strdup(id));
                        }
                        else
                        {
                            GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)wifi_connect, 0);
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)on_connect_wifi_listview_cb, GUI_EVENT_6,
                                                 gui_strdup(id));
                        }
                    }
                }
            }
        }
    }

    return parent;
}
static gui_obj_t *widget_create_macro_on_connecting(ezxml_t p, gui_obj_t *parent,
                                                    T_OBJ_TYPE widget_type)
{

    {
        char *type = 0;
        char *to = 0;
        char *id = 0;
        int x = 0;
        int y = 0;
        size_t i = 0;
        while (true)
        {
            //gui_log("p->attr[i]:%x\n",p->attr[i]);
            if (!(p->attr[i]))
            {
                break;
            }
            //gui_log("p->attr[i]:%s,\n", p->attr[i]);
            if (!strcmp(p->attr[i], "type"))
            {
                type = (p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "to"))
            {
                to = (p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "id1"))
            {
                x = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "id"))
            {
                id = p->attr[++i];
            }
            else if (!strcmp(p->attr[i], "id2"))
            {
                y = atoi(p->attr[++i]);
            }
            i++;
        }
        int to_widget = 0; GUI_UNUSED(to_widget);
        if (type && to)
        {
            if (!strcmp(type, "jump"))
            {
                {


                    //to
                    if (!strcmp(to, "multiLevel"))
                    {
                        //GUI_API(gui_multi_level_t).jump(parent, x, y);
                        struct on_click_jump_cb_param *param;
                        param = gui_malloc(sizeof(struct on_click_jump_cb_param));
                        if (id)
                        {
                            get_2_int_from_string(id, &x, &y);
                        }
                        parse_id_string(id, param);

                        param->id1 = x;
                        param->id2 = y;
                        if (parent->type == BUTTON)
                        {
                            GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)on_click_jump_cb, param);
                        }
                        else if (parent->type == WINDOW)
                        {
                            gui_win_click((gui_win_t *)parent, (gui_event_cb_t)on_click_jump_cb, param);
                        }
                        else if (parent->type == MACRO_KEY)
                        {
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_cb, GUI_EVENT_6, param);
                        }
                        else if (parent->type == MACRO_WIFI)
                        {
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_cb, GUI_EVENT_7, param);
                        }
                        else if (parent->type == MACRO_PAGE_LIST_NEW)
                        {
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_cb_wifi, GUI_EVENT_6, param);

                            gui_pagelist_new_t *pl = (void *)parent;
                            pl->click_param = gui_malloc(sizeof(void *)*pl->item_count);
                            for (size_t i = 0; i < pl->item_count; i++)
                            {
                                pl->click_function_array[i] = (gui_event_cb_t)on_click_jump_cb_wifi;
                                pl->click_param[i] = param;
                            }
                        }
                    }

                    else if (!strcmp(to, "app"))
                    {
                        if (ends_with_xml(id))
                        {
                            if (parent->type == BUTTON)
                            {
                                GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)on_click_jump_to_app_cb,
                                                               (void *)gui_get_path_by_relative(id));
                            }
                            else if (parent->type == WINDOW)
                            {
                                gui_win_click((gui_win_t *)parent, (gui_event_cb_t)on_click_jump_to_app_cb, gui_strdup(id));
                            }
                            else if (parent->type == MACRO_KEY)
                            {
                                gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_to_app_cb, GUI_EVENT_6, gui_strdup(id));
                            }
                            else if (parent->type == MACRO_WIFI)
                            {
                                gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_to_app_cb, GUI_EVENT_7, gui_strdup(id));
                            }
                        }
                        else
                        {
                            gui_log("[SaaA] error app jump format\n");
                        }



                    }
                    else if (!strcmp(to, "C-APP"))
                    {
                        if (parent->type == BUTTON)
                        {
                            GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)on_click_jump_to_capp_cb,
                                                           gui_app_get_by_name(id));
                        }
                        else if (parent->type == WINDOW)
                        {
                            gui_win_click((gui_win_t *)parent, (gui_event_cb_t)on_click_jump_to_capp_cb, 0);
                        }
                        else if (parent->type == MACRO_KEY)
                        {
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_to_capp_cb, GUI_EVENT_6,
                                                 gui_app_get_by_name(id));
                        }
                        else if (parent->type == MACRO_WIFI)
                        {
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_to_capp_cb, GUI_EVENT_7,
                                                 gui_app_get_by_name(id));
                        }
                    }
                    else if (!strcmp(to, "tabview") || !strcmp(to, "tab"))
                    {
                        //GUI_API(gui_multi_level_t).jump(parent, x, y);
                        struct on_click_jump_cb_param *param;
                        param = gui_malloc(sizeof(struct on_click_jump_cb_param));
                        char *tabview_name = 0;
                        if (id)
                        {
                            get_2_int_from_string(id, &x, &y);

                            {
                                // Find the first comma in the string
                                const char *first_comma = strchr(id, ',');

                                if (first_comma != NULL)
                                {
                                    // Find the second comma starting from the character after the first comma
                                    const char *second_comma = strchr(first_comma + 1, ',');

                                    // If the second comma was found and it's not at the end of the string
                                    if (second_comma != NULL && *(second_comma + 1) != '\0')
                                    {
                                        tabview_name = gui_strdup(second_comma + 1);
                                    }
                                    else
                                    {
                                        // Handle the case where the second comma is not found or is at the end
                                        gui_log("The second comma does not exist or is at the end.\n");
                                    }
                                }
                                else
                                {
                                    // Handle the case where the first comma is not found
                                    gui_log("The first comma is not found.\n");
                                }
                            }
                        }

                        param->id1 = x;
                        param->id2 = y;
                        param->to_widget_name = tabview_name;
                        if (parent->type == BUTTON)
                        {
                            GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)on_click_jump_cb_tabview,
                                                           param);
                        }
                        else if (parent->type == WINDOW)
                        {
                            gui_win_click((gui_win_t *)parent, (gui_event_cb_t)on_click_jump_cb_tabview, param);
                        }
                        else if (parent->type == MACRO_KEY)
                        {
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_cb_tabview, GUI_EVENT_6, param);
                        }
                        else if (parent->type == MACRO_WIFI)
                        {
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_cb_tabview, GUI_EVENT_7, param);
                        }
                    }

                }

            }
            else if (!strcmp(type, "control"))
            {
                if (!strcmp(to, "light"))
                {
                    light_param_t *light;
                    light = gui_malloc(sizeof(light_param_t));
                    light->id = x;
                    light->state = (bool)y;
                    if (parent->type == BUTTON)
                    {
                        GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)light_control_cb, light);
                    }
                    else if (parent->type == WINDOW)
                    {
                        gui_win_click((gui_win_t *)parent, (gui_event_cb_t)light_control_cb, light);
                    }
                    else if (parent->type == CLICKSWITCH)
                    {
                        GUI_API(gui_switch_t).on_turn_on((gui_switch_t *)parent,
                                                         (gui_event_cb_t)light_switch_on_cb, light);
                        GUI_API(gui_switch_t).on_turn_off((gui_switch_t *)parent,
                                                          (gui_event_cb_t)light_switch_off_cb, light);
                    }
                }

                //gui_log("p->attr[i]:%x\n", (size_t)(p->attr[i]));

            }
            else if ((!strcmp(type, "animatePause")) || (!strcmp(type, "animate")))
            {
                char **param = gui_malloc(sizeof(char *) * 3);
                param[0] = gui_strdup(to);
                param[2] = (void *)parent;
                if (id)
                {
                    param[1] = gui_strdup(id);
                }
                if (!strcmp(type, "animatePause"))
                {
                    if (parent->type == BUTTON)
                    {
                        GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)pause_animation_cb, param);
                    }
                    else if (parent->type == WINDOW)
                    {
                        gui_win_click((gui_win_t *)parent, (gui_event_cb_t)pause_animation_cb, param);
                    }
                    else if (parent->type == CLICKSWITCH)
                    {
                        GUI_API(gui_switch_t).on_turn_on(GUI_TYPE(gui_switch_t, parent), (gui_event_cb_t)pause_animation_cb,
                                                         (param));
                        GUI_API(gui_switch_t).on_turn_off(GUI_TYPE(gui_switch_t, parent),
                                                          (gui_event_cb_t)pause_animation_cb,
                                                          (param));
                    }

                }
                else if (!strcmp(type, "animate"))
                {
                    if (parent->type == BUTTON)
                    {
                        GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)start_animation_cb, param);
                    }
                    else if (parent->type == WINDOW)
                    {
                        gui_win_click((gui_win_t *)parent, (gui_event_cb_t)start_animation_cb, param);
                    }
                    else if (parent->type == CLICKSWITCH)
                    {
                        GUI_API(gui_switch_t).on_turn_on(GUI_TYPE(gui_switch_t, parent), (gui_event_cb_t)start_animation_cb,
                                                         (param));
                        GUI_API(gui_switch_t).on_turn_off(GUI_TYPE(gui_switch_t, parent),
                                                          (gui_event_cb_t)start_animation_cb,
                                                          (param));
                    }

                }
            }
            else if (!strcmp(type, "keyboard"))
            {
                if (to && strlen(to) > 0 && id)
                {
                    GUI_WIDGET_POINTER_BY_NAME(to_text, to);
                    if (to_text->type == TEXTBOX)
                    {
                        gui_text_t *widget_to_text = GUI_TYPE(gui_text_t, to_text);
                        char *content = widget_to_text->content;
                        struct on_click_keyboard_cb_param *param;
                        param = gui_malloc(sizeof(struct on_click_keyboard_cb_param));

                        if (parent->type == BUTTON)
                        {
                            param->to_text = (void *)to_text;
                            param->key_board = get_scan_code_by_keyboard_string(id);
                            if (param->key_board != SCANCODE_UNKNOWN)
                            {
                                GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)on_click_keyboard_cb, param);
                            }
                        }

                    }


                }

                //gui_log("p->attr[i]:%x\n", (size_t)(p->attr[i]));

            }
            else if (!strcmp(type, "connect"))
            {
                if (!strcmp(to, "wifi"))
                {
                    if (parent->type == BUTTON)
                    {
                        bool multi_click = false;
                        for (uint8_t i = 0; i < parent->event_dsc_cnt; i++)
                        {
                            gui_event_dsc_t *event_dsc = parent->event_dsc + i;
                            if (event_dsc->filter == GUI_EVENT_TOUCH_CLICKED)
                            {
                                multi_click = true;
                                break;
                            }
                        }
                        if (multi_click)
                        {
                            gui_win_t *win = gui_win_create(parent, "_win_for_multiclick", 0, 0, parent->w, parent->h);
                            gui_win_click(win, wifi_connect, 0);
                        }
                        else


                        {
                            GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)wifi_connect, 0);
                        }
                    }


                }

                //gui_log("p->attr[i]:%x\n", (size_t)(p->attr[i]));

            }
        }
    }

    return parent;
}
static gui_obj_t *widget_create_macro_on_disconnected(ezxml_t p, gui_obj_t *parent,
                                                      T_OBJ_TYPE widget_type)
{

    {
        char *type = 0;
        char *to = 0;
        char *id = 0;
        int x = 0;
        int y = 0;
        size_t i = 0;
        while (true)
        {
            //gui_log("p->attr[i]:%x\n",p->attr[i]);
            if (!(p->attr[i]))
            {
                break;
            }
            //gui_log("p->attr[i]:%s,\n", p->attr[i]);
            if (!strcmp(p->attr[i], "type"))
            {
                type = (p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "to"))
            {
                to = (p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "id1"))
            {
                x = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "id"))
            {
                id = p->attr[++i];
            }
            else if (!strcmp(p->attr[i], "id2"))
            {
                y = atoi(p->attr[++i]);
            }
            i++;
        }
        int to_widget = 0; GUI_UNUSED(to_widget);
        if (type && to)
        {
            if (!strcmp(type, "jump"))
            {
                {


                    //to
                    if (!strcmp(to, "multiLevel"))
                    {
                        //GUI_API(gui_multi_level_t).jump(parent, x, y);
                        struct on_click_jump_cb_param *param;
                        param = gui_malloc(sizeof(struct on_click_jump_cb_param));
                        if (id)
                        {
                            get_2_int_from_string(id, &x, &y);
                        }
                        parse_id_string(id, param);

                        param->id1 = x;
                        param->id2 = y;
                        if (parent->type == BUTTON)
                        {
                            GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)on_click_jump_cb, param);
                        }
                        else if (parent->type == WINDOW)
                        {
                            gui_win_click((gui_win_t *)parent, (gui_event_cb_t)on_click_jump_cb, param);
                        }
                        else if (parent->type == MACRO_KEY)
                        {
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_cb, GUI_EVENT_6, param);
                        }
                        else if (parent->type == MACRO_WIFI)
                        {
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_cb, GUI_EVENT_8, param);
                        }
                        else if (parent->type == MACRO_PAGE_LIST_NEW)
                        {
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_cb_wifi, GUI_EVENT_6, param);

                            gui_pagelist_new_t *pl = (void *)parent;
                            pl->click_param = gui_malloc(sizeof(void *)*pl->item_count);
                            for (size_t i = 0; i < pl->item_count; i++)
                            {
                                pl->click_function_array[i] = (gui_event_cb_t)on_click_jump_cb_wifi;
                                pl->click_param[i] = param;
                            }
                        }
                    }

                    else if (!strcmp(to, "app"))
                    {
                        if (ends_with_xml(id))
                        {
                            if (parent->type == BUTTON)
                            {
                                GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)on_click_jump_to_app_cb,
                                                               (void *)gui_get_path_by_relative(id));
                            }
                            else if (parent->type == WINDOW)
                            {
                                gui_win_click((gui_win_t *)parent, (gui_event_cb_t)on_click_jump_to_app_cb, gui_strdup(id));
                            }
                            else if (parent->type == MACRO_KEY)
                            {
                                gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_to_app_cb, GUI_EVENT_6, gui_strdup(id));
                            }
                            else if (parent->type == MACRO_WIFI)
                            {
                                gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_to_app_cb, GUI_EVENT_8, gui_strdup(id));
                            }
                        }
                        else
                        {
                            gui_log("[SaaA] error app jump format\n");
                        }



                    }
                    else if (!strcmp(to, "C-APP"))
                    {
                        if (parent->type == BUTTON)
                        {
                            GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)on_click_jump_to_capp_cb,
                                                           gui_app_get_by_name(id));
                        }
                        else if (parent->type == WINDOW)
                        {
                            gui_win_click((gui_win_t *)parent, (gui_event_cb_t)on_click_jump_to_capp_cb, 0);
                        }
                        else if (parent->type == MACRO_KEY)
                        {
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_to_capp_cb, GUI_EVENT_6,
                                                 gui_app_get_by_name(id));
                        }
                        else if (parent->type == MACRO_WIFI)
                        {
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_to_capp_cb, GUI_EVENT_8,
                                                 gui_app_get_by_name(id));
                        }
                    }
                    else if (!strcmp(to, "tabview") || !strcmp(to, "tab"))
                    {
                        //GUI_API(gui_multi_level_t).jump(parent, x, y);
                        struct on_click_jump_cb_param *param;
                        param = gui_malloc(sizeof(struct on_click_jump_cb_param));
                        char *tabview_name = 0;
                        if (id)
                        {
                            get_2_int_from_string(id, &x, &y);

                            {
                                // Find the first comma in the string
                                const char *first_comma = strchr(id, ',');

                                if (first_comma != NULL)
                                {
                                    // Find the second comma starting from the character after the first comma
                                    const char *second_comma = strchr(first_comma + 1, ',');

                                    // If the second comma was found and it's not at the end of the string
                                    if (second_comma != NULL && *(second_comma + 1) != '\0')
                                    {
                                        tabview_name = gui_strdup(second_comma + 1);
                                    }
                                    else
                                    {
                                        // Handle the case where the second comma is not found or is at the end
                                        gui_log("The second comma does not exist or is at the end.\n");
                                    }
                                }
                                else
                                {
                                    // Handle the case where the first comma is not found
                                    gui_log("The first comma is not found.\n");
                                }
                            }
                        }

                        param->id1 = x;
                        param->id2 = y;
                        param->to_widget_name = tabview_name;
                        if (parent->type == BUTTON)
                        {
                            GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)on_click_jump_cb_tabview,
                                                           param);
                        }
                        else if (parent->type == WINDOW)
                        {
                            gui_win_click((gui_win_t *)parent, (gui_event_cb_t)on_click_jump_cb_tabview, param);
                        }
                        else if (parent->type == MACRO_KEY)
                        {
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_cb_tabview, GUI_EVENT_6, param);
                        }
                        else if (parent->type == MACRO_WIFI)
                        {
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_cb_tabview, GUI_EVENT_8, param);
                        }
                    }

                }

            }
            else if (!strcmp(type, "control"))
            {
                if (!strcmp(to, "light"))
                {
                    light_param_t *light;
                    light = gui_malloc(sizeof(light_param_t));
                    light->id = x;
                    light->state = (bool)y;
                    if (parent->type == BUTTON)
                    {
                        GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)light_control_cb, light);
                    }
                    else if (parent->type == WINDOW)
                    {
                        gui_win_click((gui_win_t *)parent, (gui_event_cb_t)light_control_cb, light);
                    }
                    else if (parent->type == CLICKSWITCH)
                    {
                        GUI_API(gui_switch_t).on_turn_on((gui_switch_t *)parent,
                                                         (gui_event_cb_t)light_switch_on_cb, light);
                        GUI_API(gui_switch_t).on_turn_off((gui_switch_t *)parent,
                                                          (gui_event_cb_t)light_switch_off_cb, light);
                    }
                }

                //gui_log("p->attr[i]:%x\n", (size_t)(p->attr[i]));

            }
            else if ((!strcmp(type, "animatePause")) || (!strcmp(type, "animate")))
            {
                char **param = gui_malloc(sizeof(char *) * 3);
                param[0] = gui_strdup(to);
                param[2] = (void *)parent;
                if (id)
                {
                    param[1] = gui_strdup(id);
                }
                if (!strcmp(type, "animatePause"))
                {
                    if (parent->type == BUTTON)
                    {
                        GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)pause_animation_cb, param);
                    }
                    else if (parent->type == WINDOW)
                    {
                        gui_win_click((gui_win_t *)parent, (gui_event_cb_t)pause_animation_cb, param);
                    }
                    else if (parent->type == CLICKSWITCH)
                    {
                        GUI_API(gui_switch_t).on_turn_on(GUI_TYPE(gui_switch_t, parent), (gui_event_cb_t)pause_animation_cb,
                                                         (param));
                        GUI_API(gui_switch_t).on_turn_off(GUI_TYPE(gui_switch_t, parent),
                                                          (gui_event_cb_t)pause_animation_cb,
                                                          (param));
                    }

                }
                else if (!strcmp(type, "animate"))
                {
                    if (parent->type == BUTTON)
                    {
                        GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)start_animation_cb, param);
                    }
                    else if (parent->type == WINDOW)
                    {
                        gui_win_click((gui_win_t *)parent, (gui_event_cb_t)start_animation_cb, param);
                    }
                    else if (parent->type == CLICKSWITCH)
                    {
                        GUI_API(gui_switch_t).on_turn_on(GUI_TYPE(gui_switch_t, parent), (gui_event_cb_t)start_animation_cb,
                                                         (param));
                        GUI_API(gui_switch_t).on_turn_off(GUI_TYPE(gui_switch_t, parent),
                                                          (gui_event_cb_t)start_animation_cb,
                                                          (param));
                    }

                }
            }
            else if (!strcmp(type, "keyboard"))
            {
                if (to && strlen(to) > 0 && id)
                {
                    GUI_WIDGET_POINTER_BY_NAME(to_text, to);
                    if (to_text->type == TEXTBOX)
                    {
                        gui_text_t *widget_to_text = GUI_TYPE(gui_text_t, to_text);
                        char *content = widget_to_text->content;
                        struct on_click_keyboard_cb_param *param;
                        param = gui_malloc(sizeof(struct on_click_keyboard_cb_param));

                        if (parent->type == BUTTON)
                        {
                            param->to_text = (void *)to_text;
                            param->key_board = get_scan_code_by_keyboard_string(id);
                            if (param->key_board != SCANCODE_UNKNOWN)
                            {
                                GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)on_click_keyboard_cb, param);
                            }
                        }

                    }


                }

                //gui_log("p->attr[i]:%x\n", (size_t)(p->attr[i]));

            }
            else if (!strcmp(type, "connect"))
            {
                if (!strcmp(to, "wifi"))
                {
                    if (parent->type == BUTTON)
                    {
                        bool multi_click = false;
                        for (uint8_t i = 0; i < parent->event_dsc_cnt; i++)
                        {
                            gui_event_dsc_t *event_dsc = parent->event_dsc + i;
                            if (event_dsc->filter == GUI_EVENT_TOUCH_CLICKED)
                            {
                                multi_click = true;
                                break;
                            }
                        }
                        if (multi_click)
                        {
                            gui_win_t *win = gui_win_create(parent, "_win_for_multiclick", 0, 0, parent->w, parent->h);
                            gui_win_click(win, wifi_connect, 0);
                        }
                        else


                        {
                            GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)wifi_connect, 0);
                        }
                    }


                }

                //gui_log("p->attr[i]:%x\n", (size_t)(p->attr[i]));

            }
        }
    }

    return parent;
}

static gui_obj_t *widget_create_macro_on_select(ezxml_t p, gui_obj_t *parent,
                                                T_OBJ_TYPE widget_type)
{

    {
        char *type = 0;
        char *to = 0;
        char *id = 0;
        int x = 0;
        int y = 0;
        size_t i = 0;
        while (true)
        {
            //gui_log("p->attr[i]:%x\n",p->attr[i]);
            if (!(p->attr[i]))
            {
                break;
            }
            //gui_log("p->attr[i]:%s,\n", p->attr[i]);
            if (!strcmp(p->attr[i], "type"))
            {
                type = (p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "to"))
            {
                to = (p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "id"))
            {
                id = p->attr[++i];
            }
            i++;
        }
        int to_widget = 0; GUI_UNUSED(to_widget);
        if (type && to)
        {
            if (!strcmp(type, "jump"))
            {
                {
                    //to
                    if (!strcmp(to, "multiLevel"))
                    {
                        //GUI_API(gui_multi_level_t).jump(parent, x, y);
                        struct on_click_jump_cb_param *param;
                        param = gui_malloc(sizeof(struct on_click_jump_cb_param));
                        if (id)
                        {
                            get_2_int_from_string(id, &x, &y);
                        }

                        parse_id_string(id, param);
                        param->id1 = x;
                        param->id2 = y;
                        if (parent->type == BUTTON)
                        {
                            //GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)on_click_jump_cb, param);
                        }
                        else if (parent->type == WINDOW)
                        {
                            //gui_win_click((gui_win_t *)parent, (gui_event_cb_t)on_click_jump_cb, param);
                        }
                        else if (parent->type == SEEKBAR)
                        {
                            //gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_cb, GUI_EVENT_4, param);
                        }
                        else if (parent->type == RADIO)
                        {
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_cb, GUI_EVENT_6, param);
                        }
                    }
                    else if (!strcmp(to, "app"))
                    {
                        if (ends_with_xml(id))
                        {
                            if (parent->type == BUTTON)
                            {
                                // GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)on_click_jump_to_app_cb,
                                //                                (void *)gui_get_path_by_relative(id));
                            }
                            else if (parent->type == WINDOW)
                            {
                                //gui_win_click((gui_win_t *)parent, (gui_event_cb_t)on_click_jump_to_app_cb, gui_strdup(id));
                            }
                            else if (parent->type == SEEKBAR)
                            {
                                //gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_to_app_cb, GUI_EVENT_4, gui_strdup(id));
                            }
                            else if (parent->type == RADIO)
                            {
                                gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_to_app_cb, GUI_EVENT_6, gui_strdup(id));
                            }
                        }
                        else
                        {
                            gui_log("[SaaA] error app jump format\n");
                        }



                    }
                    else if (!strcmp(to, "C-APP"))
                    {
                        if (parent->type == BUTTON)
                        {
                            // GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)on_click_jump_to_capp_cb,
                            //                                gui_app_get_by_name(id));
                        }
                        else if (parent->type == WINDOW)
                        {
                            //gui_win_click((gui_win_t *)parent, (gui_event_cb_t)on_click_jump_to_capp_cb, 0);
                        }
                        else if (parent->type == SEEKBAR)
                        {
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_to_capp_cb, GUI_EVENT_4,
                                                 gui_app_get_by_name(id));
                        }
                        else if (parent->type == RADIO)
                        {
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_to_capp_cb, GUI_EVENT_6,
                                                 gui_app_get_by_name(id));
                        }
                    }
                    else if (!strcmp(to, "tabview") || !strcmp(to, "tab"))
                    {
                        //GUI_API(gui_multi_level_t).jump(parent, x, y);
                        struct on_click_jump_cb_param *param;
                        param = gui_malloc(sizeof(struct on_click_jump_cb_param));
                        char *tabview_name = 0;
                        if (id)
                        {
                            get_2_int_from_string(id, &x, &y);

                            {
                                // Find the first comma in the string
                                const char *first_comma = strchr(id, ',');

                                if (first_comma != NULL)
                                {
                                    // Find the second comma starting from the character after the first comma
                                    const char *second_comma = strchr(first_comma + 1, ',');

                                    // If the second comma was found and it's not at the end of the string
                                    if (second_comma != NULL && *(second_comma + 1) != '\0')
                                    {
                                        tabview_name = gui_strdup(second_comma + 1);
                                    }
                                    else
                                    {
                                        // Handle the case where the second comma is not found or is at the end
                                        gui_log("The second comma does not exist or is at the end.\n");
                                    }
                                }
                                else
                                {
                                    // Handle the case where the first comma is not found
                                    gui_log("The first comma is not found.\n");
                                }
                            }
                        }

                        param->id1 = x;
                        param->id2 = y;
                        param->to_widget_name = tabview_name;
                        if (parent->type == BUTTON)
                        {
                            // GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)on_click_jump_cb_tabview,
                            //                                param);
                        }
                        else if (parent->type == WINDOW)
                        {
                            //gui_win_click((gui_win_t *)parent, (gui_event_cb_t)on_click_jump_cb_tabview, param);
                        }
                        else if (parent->type == SEEKBAR)
                        {
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_cb_tabview, GUI_EVENT_4, param);
                        }
                        else if (parent->type == RADIO)
                        {
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_cb_tabview, GUI_EVENT_6, param);
                        }
                    }

                }

            }
            //     else if (!strcmp(type, "control"))
            //     {
            //         if (!strcmp(to, "light"))
            //         {
            //             light_param_t *light;
            //             light = gui_malloc(sizeof(light_param_t));
            //             light->id = x;
            //             light->state = (bool)y;
            //             if (parent->type == BUTTON)
            //             {
            //                 GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)light_control_cb, light);
            //             }
            //             else if (parent->type == WINDOW)
            //             {
            //                 gui_win_click((gui_win_t *)parent, (gui_event_cb_t)light_control_cb, light);
            //             }
            //             else if (parent->type == CLICKSWITCH)
            //             {
            //                 GUI_API(gui_switch_t).on_turn_on((gui_switch_t *)parent,
            //                                                  (gui_event_cb_t)light_switch_on_cb, light);
            //                 GUI_API(gui_switch_t).on_turn_off((gui_switch_t *)parent,
            //                                                   (gui_event_cb_t)light_switch_off_cb, light);
            //             }
            //         }

            //         //gui_log("p->attr[i]:%x\n", (size_t)(p->attr[i]));

            //     }
            //     else if ((!strcmp(type, "animatePause")) || (!strcmp(type, "animate")))
            //     {
            //         char **param = gui_malloc(sizeof(char *) * 3);
            //         param[0] = gui_strdup(to);
            //         param[2] = (void *)parent;
            //         if (id)
            //         {
            //             param[1] = gui_strdup(id);
            //         }
            //         if (!strcmp(type, "animatePause"))
            //         {
            //             if (parent->type == BUTTON)
            //             {
            //                 GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)pause_animation_cb, param);
            //             }
            //             else if (parent->type == WINDOW)
            //             {
            //                 gui_win_click((gui_win_t *)parent, (gui_event_cb_t)pause_animation_cb, param);
            //             }
            //             else if (parent->type == CLICKSWITCH)
            //             {
            //                 GUI_API(gui_switch_t).on_turn_on(GUI_TYPE(gui_switch_t, parent), (gui_event_cb_t)pause_animation_cb,
            //                                                  (param));
            //                 GUI_API(gui_switch_t).on_turn_off(GUI_TYPE(gui_switch_t, parent),
            //                                                   (gui_event_cb_t)pause_animation_cb,
            //                                                   (param));
            //             }

            //         }
            //         else if (!strcmp(type, "animate"))
            //         {
            //             if (parent->type == BUTTON)
            //             {
            //                 GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)start_animation_cb, param);
            //             }
            //             else if (parent->type == WINDOW)
            //             {
            //                 gui_win_click((gui_win_t *)parent, (gui_event_cb_t)start_animation_cb, param);
            //             }
            //             else if (parent->type == CLICKSWITCH)
            //             {
            //                 GUI_API(gui_switch_t).on_turn_on(GUI_TYPE(gui_switch_t, parent), (gui_event_cb_t)start_animation_cb,
            //                                                  (param));
            //                 GUI_API(gui_switch_t).on_turn_off(GUI_TYPE(gui_switch_t, parent),
            //                                                   (gui_event_cb_t)start_animation_cb,
            //                                                   (param));
            //             }

            //         }
            //     }
            //
        }
    }

    return parent;
}
static gui_obj_t *widget_create_macro_oncomplete(ezxml_t p, gui_obj_t *parent,
                                                 T_OBJ_TYPE widget_type)
{

    {
        char *type = 0;
        char *to = 0;
        char *id = 0;
        int x = 0;
        int y = 0;
        size_t i = 0;
        while (true)
        {
            //gui_log("p->attr[i]:%x\n",p->attr[i]);
            if (!(p->attr[i]))
            {
                break;
            }
            //gui_log("p->attr[i]:%s,\n", p->attr[i]);
            if (!strcmp(p->attr[i], "type"))
            {
                type = (p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "to"))
            {
                to = (p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "id"))
            {
                id = p->attr[++i];
            }
            i++;
        }
        int to_widget = 0; GUI_UNUSED(to_widget);
        if (type && to)
        {
            if (!strcmp(type, "jump"))
            {
                {
                    //to
                    if (!strcmp(to, "multiLevel"))
                    {
                        //GUI_API(gui_multi_level_t).jump(parent, x, y);
                        struct on_click_jump_cb_param *param;
                        param = gui_malloc(sizeof(struct on_click_jump_cb_param));
                        if (id)
                        {
                            get_2_int_from_string(id, &x, &y);
                        }

                        parse_id_string(id, param);
                        param->id1 = x;
                        param->id2 = y;
                        if (parent->type == BUTTON)
                        {
                            //GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)on_click_jump_cb, param);
                        }
                        else if (parent->type == WINDOW)
                        {
                            //gui_win_click((gui_win_t *)parent, (gui_event_cb_t)on_click_jump_cb, param);
                        }
                        else if (parent->type == SEEKBAR)
                        {
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_cb, GUI_EVENT_4, param);
                        }

                    }
                    else if (!strcmp(to, "app"))
                    {
                        if (ends_with_xml(id))
                        {
                            if (parent->type == BUTTON)
                            {
                                // GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)on_click_jump_to_app_cb,
                                //                                (void *)gui_get_path_by_relative(id));
                            }
                            else if (parent->type == WINDOW)
                            {
                                //gui_win_click((gui_win_t *)parent, (gui_event_cb_t)on_click_jump_to_app_cb, gui_strdup(id));
                            }
                            else if (parent->type == SEEKBAR)
                            {
                                gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_to_app_cb, GUI_EVENT_4, gui_strdup(id));
                            }
                        }
                        else
                        {
                            gui_log("[SaaA] error app jump format\n");
                        }



                    }
                    else if (!strcmp(to, "C-APP"))
                    {
                        if (parent->type == BUTTON)
                        {
                            // GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)on_click_jump_to_capp_cb,
                            //                                gui_app_get_by_name(id));
                        }
                        else if (parent->type == WINDOW)
                        {
                            //gui_win_click((gui_win_t *)parent, (gui_event_cb_t)on_click_jump_to_capp_cb, 0);
                        }
                        else if (parent->type == SEEKBAR)
                        {
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_to_capp_cb, GUI_EVENT_4,
                                                 gui_app_get_by_name(id));
                        }
                    }
                    else if (!strcmp(to, "tabview") || !strcmp(to, "tab"))
                    {
                        //GUI_API(gui_multi_level_t).jump(parent, x, y);
                        struct on_click_jump_cb_param *param;
                        param = gui_malloc(sizeof(struct on_click_jump_cb_param));
                        char *tabview_name = 0;
                        if (id)
                        {
                            get_2_int_from_string(id, &x, &y);

                            {
                                // Find the first comma in the string
                                const char *first_comma = strchr(id, ',');

                                if (first_comma != NULL)
                                {
                                    // Find the second comma starting from the character after the first comma
                                    const char *second_comma = strchr(first_comma + 1, ',');

                                    // If the second comma was found and it's not at the end of the string
                                    if (second_comma != NULL && *(second_comma + 1) != '\0')
                                    {
                                        tabview_name = gui_strdup(second_comma + 1);
                                    }
                                    else
                                    {
                                        // Handle the case where the second comma is not found or is at the end
                                        gui_log("The second comma does not exist or is at the end.\n");
                                    }
                                }
                                else
                                {
                                    // Handle the case where the first comma is not found
                                    gui_log("The first comma is not found.\n");
                                }
                            }
                        }

                        param->id1 = x;
                        param->id2 = y;
                        param->to_widget_name = tabview_name;
                        if (parent->type == BUTTON)
                        {
                            // GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)on_click_jump_cb_tabview,
                            //                                param);
                        }
                        else if (parent->type == WINDOW)
                        {
                            //gui_win_click((gui_win_t *)parent, (gui_event_cb_t)on_click_jump_cb_tabview, param);
                        }
                        else if (parent->type == SEEKBAR)
                        {
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_cb_tabview, GUI_EVENT_4, param);
                        }
                    }

                }

            }
            //     else if (!strcmp(type, "control"))
            //     {
            //         if (!strcmp(to, "light"))
            //         {
            //             light_param_t *light;
            //             light = gui_malloc(sizeof(light_param_t));
            //             light->id = x;
            //             light->state = (bool)y;
            //             if (parent->type == BUTTON)
            //             {
            //                 GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)light_control_cb, light);
            //             }
            //             else if (parent->type == WINDOW)
            //             {
            //                 gui_win_click((gui_win_t *)parent, (gui_event_cb_t)light_control_cb, light);
            //             }
            //             else if (parent->type == CLICKSWITCH)
            //             {
            //                 GUI_API(gui_switch_t).on_turn_on((gui_switch_t *)parent,
            //                                                  (gui_event_cb_t)light_switch_on_cb, light);
            //                 GUI_API(gui_switch_t).on_turn_off((gui_switch_t *)parent,
            //                                                   (gui_event_cb_t)light_switch_off_cb, light);
            //             }
            //         }

            //         //gui_log("p->attr[i]:%x\n", (size_t)(p->attr[i]));

            //     }
            //     else if ((!strcmp(type, "animatePause")) || (!strcmp(type, "animate")))
            //     {
            //         char **param = gui_malloc(sizeof(char *) * 3);
            //         param[0] = gui_strdup(to);
            //         param[2] = (void *)parent;
            //         if (id)
            //         {
            //             param[1] = gui_strdup(id);
            //         }
            //         if (!strcmp(type, "animatePause"))
            //         {
            //             if (parent->type == BUTTON)
            //             {
            //                 GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)pause_animation_cb, param);
            //             }
            //             else if (parent->type == WINDOW)
            //             {
            //                 gui_win_click((gui_win_t *)parent, (gui_event_cb_t)pause_animation_cb, param);
            //             }
            //             else if (parent->type == CLICKSWITCH)
            //             {
            //                 GUI_API(gui_switch_t).on_turn_on(GUI_TYPE(gui_switch_t, parent), (gui_event_cb_t)pause_animation_cb,
            //                                                  (param));
            //                 GUI_API(gui_switch_t).on_turn_off(GUI_TYPE(gui_switch_t, parent),
            //                                                   (gui_event_cb_t)pause_animation_cb,
            //                                                   (param));
            //             }

            //         }
            //         else if (!strcmp(type, "animate"))
            //         {
            //             if (parent->type == BUTTON)
            //             {
            //                 GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)start_animation_cb, param);
            //             }
            //             else if (parent->type == WINDOW)
            //             {
            //                 gui_win_click((gui_win_t *)parent, (gui_event_cb_t)start_animation_cb, param);
            //             }
            //             else if (parent->type == CLICKSWITCH)
            //             {
            //                 GUI_API(gui_switch_t).on_turn_on(GUI_TYPE(gui_switch_t, parent), (gui_event_cb_t)start_animation_cb,
            //                                                  (param));
            //                 GUI_API(gui_switch_t).on_turn_off(GUI_TYPE(gui_switch_t, parent),
            //                                                   (gui_event_cb_t)start_animation_cb,
            //                                                   (param));
            //             }

            //         }
            //     }
            //
        }
    }

    return parent;
}
static gui_obj_t *widget_create_macro_backicon(ezxml_t p, gui_obj_t *parent, T_OBJ_TYPE widget_type)
{

    {
        size_t i = 0;
        int16_t x = 0;
        int16_t y = 0;
        int16_t w = 0;
        int16_t h = 0;
        char *picture = NULL;
        char *hl_picture = NULL;
        while (true)
        {
            if (!(p->attr[i]))
            {
                break;
            }
            //gui_log("p->attr[i]:%s,\n", p->attr[i]);
            if (!strcmp(p->attr[i], "x"))
            {
                x = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "y"))
            {
                y = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "w"))
            {
                w = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "h"))
            {
                h = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "picture"))
            {
                picture = gui_strdup(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "highlightPicture"))
            {
                hl_picture = gui_strdup(p->attr[++i]);
            }
            i++;
        }
        void *img1 = return_image;
        void *img2 = return_image_hl;
        if (picture)
        {
            img1 = (void *)gui_get_image_file_address(picture);
            img2 = img1;
        }
        if (hl_picture)
        {
            img2 = (void *)gui_get_image_file_address(hl_picture);
        }
        char *ptxt = get_space_string_head(p->txt);
        //font_size = 32;
        parent = (void *)gui_button_create(parent, x, y, w, h, img1, img2, 0, (T_BUTTON_BG_TYPE)0, 0);
        parent->name = ptxt;
        gui_button_t *button = (void *)parent;
        button->img->blend_mode = IMG_SRC_OVER_MODE;
        GUI_API(gui_button_t).on_click(button, (gui_event_cb_t)setting_return_cb, 0);



    }

    return parent;
}

#define BOARD_SDL2
//#include "board_xml.h"
typedef struct xml_dom_key
{
    bool down;
    bool up;
} xml_dom_key_t;

static xml_dom_key_t xml_dom_key_array[XML_DOM_KEY_COUINT];
gui_error_t gui_xml_dom_write_key_array(int id, bool up, bool down)
{
    if (id >= 0 && id <= XML_DOM_KEY_COUINT - 1)
    {
        xml_dom_key_array[id].up = up;
        xml_dom_key_array[id].down = down;
        return GUI_SUCCESS;
    }
    return GUI_ERROR_NULL;
}
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(win_key_ani)
{
    intptr_t id = (size_t)p;
    if (id >= 0 && id <= XML_DOM_KEY_COUINT - 1)
    {
        if (xml_dom_key_array[id].up)
        {
            gui_obj_enable_event(this_widget, GUI_EVENT_6);
            xml_dom_key_array[id].up = 0;
        }
    }


}
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(win_wifi_ani)
{
    static bool connecting;
    switch (active_wifi_status)
    {
    case ACTIVE_WIFI_STATUS_CONNECTED:
        {
            gui_obj_enable_event(this_widget, GUI_EVENT_6);
            connecting = false;
        }
        break;
    case ACTIVE_WIFI_STATUS_CONNECTING:
        {
            if (!connecting)
            {
                gui_obj_enable_event(this_widget, GUI_EVENT_7);
                connecting = true;
            }
        }
        break;
    case ACTIVE_WIFI_STATUS_DISCONNECTED:
        {
            gui_obj_enable_event(this_widget, GUI_EVENT_8);
            connecting = false;
        }
        break;
    default:
        break;
    }


}
static GUI_EVENT_CALLBACK_FUNCTION_DEFINE(win_key_cb)
{
//gui_log("win_key_cb%d\n",param);
    GUI_UNUSED(win_key_cb);
}
static gui_obj_t *widget_create_macro_key(ezxml_t p, gui_obj_t *parent, T_OBJ_TYPE widget_type)
{

    {
        size_t i = 0;
        int id = 0;
        while (true)
        {
            if (!(p->attr[i]))
            {
                break;
            }
            //gui_log("p->attr[i]:%s,\n", p->attr[i]);
            if (!strcmp(p->attr[i], "id"))
            {
                id = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "name"))
            {

            }
            i++;
        }
        if (id >= 0 && id < sizeof(xml_dom_key_array) / sizeof(xml_dom_key_t))
        {


            char *ptxt = get_space_string_head(p->txt);
            gui_win_t *win = gui_win_create(parent, ptxt, 0,  0, 0, 0);
            gui_win_set_animate(win, 10000, -1, win_key_ani, (void *)(size_t)id);

            GUI_BASE(win)->type = MACRO_KEY;
            parent = (void *)win;
        }
        else
        {
            gui_log("Key id >=256, create failed\n");
        }


    }

    return parent;
}
static gui_obj_t *widget_create_macro_wifi(ezxml_t p, gui_obj_t *parent, T_OBJ_TYPE widget_type)
{

    {
        size_t i = 0;
        int id = 0;
        while (true)
        {
            if (!(p->attr[i]))
            {
                break;
            }
            //gui_log("p->attr[i]:%s,\n", p->attr[i]);
            if (!strcmp(p->attr[i], "id"))
            {
                id = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "name"))
            {

            }
            i++;
        }
        char *ptxt = get_space_string_head(p->txt);
        gui_win_t *win = gui_win_create(parent, ptxt, 0,  0, 0, 0);
        gui_win_set_animate(win, 10000, -1, win_wifi_ani, 0);

        GUI_BASE(win)->type = MACRO_WIFI;
        parent = (void *)win;
    }
    return parent;
}
static gui_obj_t *widget_create_slider(ezxml_t p, gui_obj_t *parent, T_OBJ_TYPE widget_type)
{

    {
        size_t i = 0;
        int16_t x = 0;
        int16_t y = 0;
        int16_t w = 0;
        int16_t h = 0;
        char *slider_picture = NULL;
        char *bg_picture = NULL;
        int16_t minValue = 0;
        int16_t maxValue = 0;
        int16_t currentValue = 0;
        int16_t slider_size = 0;
        while (true)
        {
            if (!(p->attr[i]))
            {
                break;
            }
            //gui_log("p->attr[i]:%s,\n", p->attr[i]);
            if (!strcmp(p->attr[i], "x"))
            {
                x = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "y"))
            {
                y = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "w"))
            {
                w = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "h"))
            {
                h = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "bgPicture"))
            {
                bg_picture = gui_strdup(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "sliderPicture"))
            {
                slider_picture = gui_strdup(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "minValue"))
            {
                minValue = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "maxValue"))
            {
                maxValue = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "currentValue"))
            {
                currentValue = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "sliderSize"))
            {
                slider_size = atoi(p->attr[++i]);
            }
            i++;
        }
        void *bg_buf = 0;
        void *slider_buf = 0;
        if (bg_picture)
        {
            bg_buf = (void *)gui_get_image_file_address(bg_picture);
        }
        if (slider_picture)
        {
            slider_buf = (void *)gui_get_image_file_address(slider_picture);
        }

        parent = (void *)gui_slider_create(parent, bg_buf, x, y, w, h, minValue, maxValue, slider_buf,
                                           currentValue, slider_size);
    }

    return parent;
}
static gui_obj_t *widget_create_macro_onchange(ezxml_t p, gui_obj_t *parent, T_OBJ_TYPE widget_type)
{

    {
        char *type = 0;
        char *to = "null";
        int id = 0;
        size_t i = 0;
        while (true)
        {
            if (!(p->attr[i]))
            {
                break;
            }
            if (!strcmp(p->attr[i], "type"))
            {
                type = (p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "to"))
            {
                to = (p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "id"))
            {
                id = atoi(p->attr[++i]);
            }
            i++;
        }
        if (type)
        {

            if (!strcmp(type, "control"))
            {
                light_param_t *light;
                light = gui_malloc(sizeof(light_param_t));
                light->id = (id);
                if (!strcmp(to, "controlLightBT"))
                {
                    if (parent->type == SEEKBAR)
                    {
                        GUI_API(gui_seekbar_t).on_change((gui_seekbar_t *)parent, (gui_event_cb_t)light_brightness_cb,
                                                         light);
                    }
                }
                else if (!strcmp(to, "controlLightCT"))
                {
                    light->text = gui_malloc(sizeof(gui_text_t));
                    if (parent->type == SLIDER)
                    {
                        gui_slider_t *slider = (gui_slider_t *)parent;
                        GUI_API(gui_slider_t).on_change(slider, (gui_event_cb_t)light_colorTemp_cb, light);
                    }

                }
            }
            else if (!strcmp(type, "write"))
            {
                if (parent->type == SLIDER)
                {
                    gui_slider_t *slider = (gui_slider_t *)parent;
                    GUI_API(gui_slider_t).on_change(slider, (gui_event_cb_t)slider_write_text_cb, gui_strdup(to));
                }
                else if (parent->type == SEEKBAR)
                {
                    GUI_API(gui_seekbar_t).on_change((gui_seekbar_t *)parent, (gui_event_cb_t)seekbar_write_text_cb,
                                                     gui_strdup(to));
                }
            }
        }
    }

    return parent;
}
static gui_obj_t *widget_create_macro_onoff(ezxml_t p, gui_obj_t *parent, T_OBJ_TYPE widget_type)
{

    {
        char *type = 0;
        char *to = 0;
        char *id = 0;
        int x = 0;
        int y = 0;
        size_t i = 0;
        while (true)
        {
            if (!(p->attr[i]))
            {
                break;
            }
            if (!strcmp(p->attr[i], "type"))
            {
                type = (p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "to"))
            {
                to = (p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "id1"))
            {
                x = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "id"))
            {
                id = p->attr[++i];
            }
            else if (!strcmp(p->attr[i], "id2"))
            {
                y = atoi(p->attr[++i]);
            }
            i++;
        }
        int to_widget = 0; GUI_UNUSED(to_widget);
        if (type && to && id)
        {
            char **param = gui_malloc(sizeof(char *) * 2);
            param[0] = gui_strdup(to);
            param[1] = gui_strdup(id);
            if (!strcmp(type, "animatePause"))
            {

                GUI_API(gui_switch_t).on_turn_off(GUI_TYPE(gui_switch_t, parent),
                                                  (gui_event_cb_t)pause_animation_cb,
                                                  (param));
            }
            else if (!strcmp(type, "animate"))
            {
                GUI_API(gui_switch_t).on_turn_off(GUI_TYPE(gui_switch_t, parent),
                                                  (gui_event_cb_t)start_animation_cb,
                                                  param);
            }
        }
        if (type && to)
        {
            if (!strcmp(type, "jump"))
            {
                {


                    //to
                    if (!strcmp(to, "multiLevel"))
                    {
                        //GUI_API(gui_multi_level_t).jump(parent, x, y);
                        struct on_click_jump_cb_param *param;
                        param = gui_malloc(sizeof(struct on_click_jump_cb_param));
                        if (id)
                        {
                            get_2_int_from_string(id, &x, &y);
                        }
                        parse_id_string(id, param);

                        param->id1 = x;
                        param->id2 = y;
                        if (parent->type == CLICKSWITCH)
                        {
                            GUI_API(gui_switch_t).on_turn_off(GUI_TYPE(gui_switch_t, parent), (gui_event_cb_t)on_click_jump_cb,
                                                              (param));
                        }
                    }

                    else if (!strcmp(to, "app"))
                    {
                        if (ends_with_xml(id))
                        {

                            if (parent->type == CLICKSWITCH)
                            {
                                GUI_API(gui_switch_t).on_turn_off(GUI_TYPE(gui_switch_t, parent),
                                                                  (gui_event_cb_t)on_click_jump_to_app_cb, (void *)gui_get_path_by_relative(id));
                            }
                        }
                        else
                        {
                            gui_log("[SaaA] error app jump format\n");
                        }



                    }
                    else if (!strcmp(to, "C-APP"))
                    {
                        if (parent->type == CLICKSWITCH)
                        {
                            GUI_API(gui_switch_t).on_turn_off((void *)parent, (gui_event_cb_t)on_click_jump_to_capp_cb,
                                                              (void *)gui_app_get_by_name(id));
                        }

                    }
                    else if (!strcmp(to, "tabview") || !strcmp(to, "tab"))
                    {
                        //GUI_API(gui_multi_level_t).jump(parent, x, y);
                        struct on_click_jump_cb_param *param;
                        param = gui_malloc(sizeof(struct on_click_jump_cb_param));
                        char *tabview_name = 0;
                        if (id)
                        {
                            get_2_int_from_string(id, &x, &y);

                            {
                                // Find the first comma in the string
                                const char *first_comma = strchr(id, ',');

                                if (first_comma != NULL)
                                {
                                    // Find the second comma starting from the character after the first comma
                                    const char *second_comma = strchr(first_comma + 1, ',');

                                    // If the second comma was found and it's not at the end of the string
                                    if (second_comma != NULL && *(second_comma + 1) != '\0')
                                    {
                                        tabview_name = gui_strdup(second_comma + 1);
                                    }
                                    else
                                    {
                                        // Handle the case where the second comma is not found or is at the end
                                        gui_log("The second comma does not exist or is at the end.\n");
                                    }
                                }
                                else
                                {
                                    // Handle the case where the first comma is not found
                                    gui_log("The first comma is not found.\n");
                                }
                            }
                        }

                        param->id1 = x;
                        param->id2 = y;
                        param->to_widget_name = tabview_name;
                        if (parent->type == CLICKSWITCH)
                        {
                            GUI_API(gui_switch_t).on_turn_off((void *)parent, (gui_event_cb_t)on_click_jump_cb_tabview,
                                                              (void *)param);
                        }

                    }

                }

            }
            else if (!strcmp(type, "control"))
            {
                if (!strcmp(to, "light"))
                {
                    light_param_t *light;
                    light = gui_malloc(sizeof(light_param_t));
                    light->id = x;
                    light->state = (bool)y;
                    if (parent->type == CLICKSWITCH)
                    {
                        GUI_API(gui_switch_t).on_turn_off((void *)parent, (gui_event_cb_t)light_control_cb, light);
                    }

                }

                //gui_log("p->attr[i]:%x\n", (size_t)(p->attr[i]));

            }
            // else if ((!strcmp(type, "animatePause")) || (!strcmp(type, "animate")))
            // {
            //     char **param = gui_malloc(sizeof(char *) * 3);
            //     param[0] = gui_strdup(to);
            //     param[2] = (void *)parent;
            //     if (id)
            //     {
            //         param[1] = gui_strdup(id);
            //     }
            //     if (!strcmp(type, "animatePause"))
            //     {
            //         if (parent->type == BUTTON)
            //         {
            //             GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)pause_animation_cb, param);
            //         }
            //         else if (parent->type == WINDOW)
            //         {
            //             gui_win_click((gui_win_t *)parent, (gui_event_cb_t)pause_animation_cb, param);
            //         }
            //         else if (parent->type == CLICKSWITCH)
            //         {
            //             GUI_API(gui_switch_t).on_turn_on(GUI_TYPE(gui_switch_t, parent), (gui_event_cb_t)pause_animation_cb,
            //                                              (param));
            //             GUI_API(gui_switch_t).on_turn_off(GUI_TYPE(gui_switch_t, parent),
            //                                               (gui_event_cb_t)pause_animation_cb,
            //                                               (param));
            //         }

            //     }
            //     else if (!strcmp(type, "animate"))
            //     {
            //         if (parent->type == BUTTON)
            //         {
            //             GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)start_animation_cb, param);
            //         }
            //         else if (parent->type == WINDOW)
            //         {
            //             gui_win_click((gui_win_t *)parent, (gui_event_cb_t)start_animation_cb, param);
            //         }
            //         else if (parent->type == CLICKSWITCH)
            //         {
            //             GUI_API(gui_switch_t).on_turn_on(GUI_TYPE(gui_switch_t, parent), (gui_event_cb_t)start_animation_cb,
            //                                              (param));
            //             GUI_API(gui_switch_t).on_turn_off(GUI_TYPE(gui_switch_t, parent),
            //                                               (gui_event_cb_t)start_animation_cb,
            //                                               (param));
            //         }

            //     }
            // }
            else if (!strcmp(type, "keyboard"))
            {
                if (to && strlen(to) > 0)
                {
                    GUI_WIDGET_POINTER_BY_NAME(to_text, to);
                    if (to_text->type == TEXTBOX)
                    {
                        gui_text_t *widget_to_text = GUI_TYPE(gui_text_t, to_text);
                        char *content = widget_to_text->content;
                        struct on_click_keyboard_cb_param *param;
                        param = gui_malloc(sizeof(struct on_click_keyboard_cb_param));

                        if (parent->type == CLICKSWITCH)
                        {
                            param->to_text = (void *)to_text;
                            param->key_board = get_scan_code_by_keyboard_string(id);
                            if (param->key_board != SCANCODE_UNKNOWN)
                            {

                                {
                                    GUI_API(gui_switch_t).on_turn_off((void *)parent,
                                                                      (gui_event_cb_t)on_click_keyboard_cb_switch_turn_off, param);
                                }
                            }

                        }


                    }

                    //gui_log("p->attr[i]:%x\n", (size_t)(p->attr[i]));

                }
            }
            else if (!strcmp(type, "password"))
            {
                if (to && strlen(to) > 0 && GUI_STRINGS_EQUAL(to, "text"))
                {

                    GUI_API(gui_switch_t).on_turn_off((gui_switch_t *)parent,
                                                      (gui_event_cb_t)on_on_text_password, gui_strdup(id));
                    //gui_log("p->attr[i]:%x\n", (size_t)(p->attr[i]));

                }
            }
            else if (!strcmp(type, "text"))
            {
                if (to && strlen(to) > 0 && GUI_STRINGS_EQUAL(to, "text"))
                {

                    GUI_API(gui_switch_t).on_turn_off((gui_switch_t *)parent,
                                                      (gui_event_cb_t)on_off_text_text, gui_strdup(id));
                    //gui_log("p->attr[i]:%x\n", (size_t)(p->attr[i]));

                }
            }
        }
    }

    return parent;
}
static gui_obj_t *widget_create_macro_onon(ezxml_t p, gui_obj_t *parent, T_OBJ_TYPE widget_type)
{

    {
        char *type = 0;
        char *to = 0;
        char *id = 0;
        int x = 0;
        int y = 0;
        size_t i = 0;
        while (true)
        {
            if (!(p->attr[i]))
            {
                break;
            }
            if (!strcmp(p->attr[i], "type"))
            {
                type = (p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "to"))
            {
                to = (p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "id1"))
            {
                x = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "id"))
            {
                id = p->attr[++i];
            }
            else if (!strcmp(p->attr[i], "id2"))
            {
                y = atoi(p->attr[++i]);
            }
            i++;
        }
        int to_widget = 0; GUI_UNUSED(to_widget);
        if (type && to && id)
        {
            char **param = gui_malloc(sizeof(char *) * 2);
            param[0] = gui_strdup(to);
            param[1] = gui_strdup(id);
            if (!strcmp(type, "animatePause"))
            {

                GUI_API(gui_switch_t).on_turn_on(GUI_TYPE(gui_switch_t, parent), (gui_event_cb_t)pause_animation_cb,
                                                 (param));
            }
            else if (!strcmp(type, "animate"))
            {
                GUI_API(gui_switch_t).on_turn_on(GUI_TYPE(gui_switch_t, parent), (gui_event_cb_t)start_animation_cb,
                                                 param);
            }
        }
        if (type && to)
        {
            if (!strcmp(type, "jump"))
            {
                {


                    //to
                    if (!strcmp(to, "multiLevel"))
                    {
                        //GUI_API(gui_multi_level_t).jump(parent, x, y);
                        struct on_click_jump_cb_param *param;
                        param = gui_malloc(sizeof(struct on_click_jump_cb_param));
                        if (id)
                        {
                            get_2_int_from_string(id, &x, &y);
                        }
                        parse_id_string(id, param);

                        param->id1 = x;
                        param->id2 = y;
                        if (parent->type == CLICKSWITCH)
                        {
                            GUI_API(gui_switch_t).on_turn_on(GUI_TYPE(gui_switch_t, parent), (gui_event_cb_t)on_click_jump_cb,
                                                             (param));
                        }
                    }

                    else if (!strcmp(to, "app"))
                    {
                        if (ends_with_xml(id))
                        {

                            if (parent->type == CLICKSWITCH)
                            {
                                GUI_API(gui_switch_t).on_turn_on(GUI_TYPE(gui_switch_t, parent),
                                                                 (gui_event_cb_t)on_click_jump_to_app_cb, (void *)gui_get_path_by_relative(id));
                            }
                        }
                        else
                        {
                            gui_log("[SaaA] error app jump format\n");
                        }



                    }
                    else if (!strcmp(to, "C-APP"))
                    {
                        if (parent->type == CLICKSWITCH)
                        {
                            GUI_API(gui_switch_t).on_turn_on((void *)parent, (gui_event_cb_t)on_click_jump_to_capp_cb,
                                                             gui_app_get_by_name(id));
                        }

                    }
                    else if (!strcmp(to, "tabview") || !strcmp(to, "tab"))
                    {
                        //GUI_API(gui_multi_level_t).jump(parent, x, y);
                        struct on_click_jump_cb_param *param;
                        param = gui_malloc(sizeof(struct on_click_jump_cb_param));
                        char *tabview_name = 0;
                        if (id)
                        {
                            get_2_int_from_string(id, &x, &y);

                            {
                                // Find the first comma in the string
                                const char *first_comma = strchr(id, ',');

                                if (first_comma != NULL)
                                {
                                    // Find the second comma starting from the character after the first comma
                                    const char *second_comma = strchr(first_comma + 1, ',');

                                    // If the second comma was found and it's not at the end of the string
                                    if (second_comma != NULL && *(second_comma + 1) != '\0')
                                    {
                                        tabview_name = gui_strdup(second_comma + 1);
                                    }
                                    else
                                    {
                                        // Handle the case where the second comma is not found or is at the end
                                        gui_log("The second comma does not exist or is at the end.\n");
                                    }
                                }
                                else
                                {
                                    // Handle the case where the first comma is not found
                                    gui_log("The first comma is not found.\n");
                                }
                            }
                        }

                        param->id1 = x;
                        param->id2 = y;
                        param->to_widget_name = tabview_name;
                        if (parent->type == CLICKSWITCH)
                        {
                            GUI_API(gui_switch_t).on_turn_on((void *)parent, (gui_event_cb_t)on_click_jump_cb_tabview,
                                                             param);
                        }

                    }

                }

            }
            else if (!strcmp(type, "control"))
            {
                if (!strcmp(to, "light"))
                {
                    light_param_t *light;
                    light = gui_malloc(sizeof(light_param_t));
                    light->id = x;
                    light->state = (bool)y;
                    if (parent->type == CLICKSWITCH)
                    {
                        GUI_API(gui_switch_t).on_turn_on((void *)parent, (gui_event_cb_t)light_control_cb, light);
                    }

                }

                //gui_log("p->attr[i]:%x\n", (size_t)(p->attr[i]));

            }
            // else if ((!strcmp(type, "animatePause")) || (!strcmp(type, "animate")))
            // {
            //     char **param = gui_malloc(sizeof(char *) * 3);
            //     param[0] = gui_strdup(to);
            //     param[2] = (void *)parent;
            //     if (id)
            //     {
            //         param[1] = gui_strdup(id);
            //     }
            //     if (!strcmp(type, "animatePause"))
            //     {
            //         if (parent->type == BUTTON)
            //         {
            //             GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)pause_animation_cb, param);
            //         }
            //         else if (parent->type == WINDOW)
            //         {
            //             gui_win_click((gui_win_t *)parent, (gui_event_cb_t)pause_animation_cb, param);
            //         }
            //         else if (parent->type == CLICKSWITCH)
            //         {
            //             GUI_API(gui_switch_t).on_turn_on(GUI_TYPE(gui_switch_t, parent), (gui_event_cb_t)pause_animation_cb,
            //                                              (param));
            //             GUI_API(gui_switch_t).on_turn_off(GUI_TYPE(gui_switch_t, parent),
            //                                               (gui_event_cb_t)pause_animation_cb,
            //                                               (param));
            //         }

            //     }
            //     else if (!strcmp(type, "animate"))
            //     {
            //         if (parent->type == BUTTON)
            //         {
            //             GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)start_animation_cb, param);
            //         }
            //         else if (parent->type == WINDOW)
            //         {
            //             gui_win_click((gui_win_t *)parent, (gui_event_cb_t)start_animation_cb, param);
            //         }
            //         else if (parent->type == CLICKSWITCH)
            //         {
            //             GUI_API(gui_switch_t).on_turn_on(GUI_TYPE(gui_switch_t, parent), (gui_event_cb_t)start_animation_cb,
            //                                              (param));
            //             GUI_API(gui_switch_t).on_turn_off(GUI_TYPE(gui_switch_t, parent),
            //                                               (gui_event_cb_t)start_animation_cb,
            //                                               (param));
            //         }

            //     }
            // }
            else if (!strcmp(type, "keyboard"))
            {
                if (to && strlen(to) > 0)
                {
                    GUI_WIDGET_POINTER_BY_NAME(to_text, to);
                    if (to_text->type == TEXTBOX)
                    {
                        gui_text_t *widget_to_text = GUI_TYPE(gui_text_t, to_text);
                        char *content = widget_to_text->content;
                        struct on_click_keyboard_cb_param *param;
                        param = gui_malloc(sizeof(struct on_click_keyboard_cb_param));

                        if (parent->type == CLICKSWITCH)
                        {
                            param->to_text = (void *)to_text;
                            param->key_board = get_scan_code_by_keyboard_string(id);
                            if (param->key_board != SCANCODE_UNKNOWN)
                            {

                                {
                                    GUI_API(gui_switch_t).on_turn_on((void *)parent,
                                                                     (gui_event_cb_t)on_click_keyboard_cb_switch_turn_on, param);
                                }
                            }

                        }


                    }

                    //gui_log("p->attr[i]:%x\n", (size_t)(p->attr[i]));

                }
            }
            else if (!strcmp(type, "password"))
            {
                if (to && strlen(to) > 0 && GUI_STRINGS_EQUAL(to, "text"))
                {

                    GUI_API(gui_switch_t).on_turn_on((gui_switch_t *)parent,
                                                     (gui_event_cb_t)on_on_text_password, gui_strdup(id));
                    //gui_log("p->attr[i]:%x\n", (size_t)(p->attr[i]));

                }
            }
        }
    }

    return parent;
}
static gui_obj_t *widget_create_macro_onload(ezxml_t p, gui_obj_t *parent, T_OBJ_TYPE widget_type)
{

    {
        char *type = 0;
        char *to = 0;
        char *id = 0;
        int x = 0;
        int y = 0;
        size_t i = 0;
        while (true)
        {
            //gui_log("p->attr[i]:%x\n",p->attr[i]);
            if (!(p->attr[i]))
            {
                break;
            }
            //gui_log("p->attr[i]:%s,\n", p->attr[i]);
            if (!strcmp(p->attr[i], "type"))
            {
                type = (p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "to"))
            {
                to = (p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "id1"))
            {
                x = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "id"))
            {
                id = p->attr[++i];
            }
            else if (!strcmp(p->attr[i], "id2"))
            {
                y = atoi(p->attr[++i]);
            }
            i++;
        }
        int to_widget = 0; GUI_UNUSED(to_widget);
        if (type && to)
        {
            if (!strcmp(type, "jump"))
            {
                {


                    //to
                    if (!strcmp(to, "multiLevel"))
                    {
                        //GUI_API(gui_multi_level_t).jump(parent, x, y);
                        struct on_click_jump_cb_param *param;
                        param = gui_malloc(sizeof(struct on_click_jump_cb_param));
                        if (id)
                        {
                            get_2_int_from_string(id, &x, &y);
                        }
                        parse_id_string(id, param);

                        param->id1 = x;
                        param->id2 = y;
                        if (parent->type == BUTTON)
                        {
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_cb, GUI_EVENT_2, param);
                        }
                        else if (parent->type == WINDOW)
                        {
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_cb, GUI_EVENT_WIN_LOAD, param);
                        }
                        else if (parent->type == MACRO_KEY)
                        {
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_cb, GUI_EVENT_WIN_LOAD, param);
                        }
                        else if (parent->type == MACRO_PAGE_LIST_NEW)
                        {
                            // gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_cb_wifi, GUI_EVENT_6, param);

                            // gui_pagelist_new_t *pl = (void *)parent;
                            // pl->click_param = gui_malloc(sizeof(void *)*pl->item_count);
                            // for (size_t i = 0; i < pl->item_count; i++)
                            // {
                            //     pl->click_function_array[i] = (gui_event_cb_t)on_click_jump_cb_wifi;
                            //     pl->click_param[i] = param;
                            // }
                        }
                        else if (parent->type == RADIO)
                        {
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_cb, GUI_EVENT_WIN_LOAD, param);
                        }
                    }

                    else if (!strcmp(to, "app"))
                    {
                        if (ends_with_xml(id))
                        {
                            if (parent->type == BUTTON)
                            {
                                gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_to_app_cb, GUI_EVENT_2,
                                                     (void *)gui_get_path_by_relative(id));
                            }
                            else if (parent->type == WINDOW)
                            {
                                gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_to_app_cb, GUI_EVENT_WIN_LOAD,
                                                     gui_strdup(id));
                            }
                            else if (parent->type == MACRO_KEY)
                            {
                                gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_to_app_cb, GUI_EVENT_WIN_LOAD,
                                                     gui_strdup(id));
                            }
                            else if (parent->type == RADIO)
                            {
                                gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_to_app_cb, GUI_EVENT_WIN_LOAD,
                                                     gui_strdup(id));
                            }
                        }
                        else
                        {
                            gui_log("[SaaA] error app jump format\n");
                        }



                    }
                    else if (!strcmp(to, "C-APP"))
                    {
                        if (parent->type == BUTTON)
                        {
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_to_capp_cb, GUI_EVENT_2,
                                                 (void *)gui_app_get_by_name(id));
                        }
                        else if (parent->type == WINDOW)
                        {
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_to_capp_cb, GUI_EVENT_WIN_LOAD,
                                                 (void *)gui_app_get_by_name(id));
                        }
                        else if (parent->type == MACRO_KEY)
                        {
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_to_capp_cb, GUI_EVENT_WIN_LOAD,
                                                 gui_app_get_by_name(id));
                        }
                        else if (parent->type == RADIO)
                        {
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_to_capp_cb, GUI_EVENT_WIN_LOAD,
                                                 gui_app_get_by_name(id));
                        }
                    }
                    else if (!strcmp(to, "tabview") || !strcmp(to, "tab"))
                    {
                        //GUI_API(gui_multi_level_t).jump(parent, x, y);
                        struct on_click_jump_cb_param *param;
                        param = gui_malloc(sizeof(struct on_click_jump_cb_param));
                        char *tabview_name = 0;
                        if (id)
                        {
                            get_2_int_from_string(id, &x, &y);

                            {
                                // Find the first comma in the string
                                const char *first_comma = strchr(id, ',');

                                if (first_comma != NULL)
                                {
                                    // Find the second comma starting from the character after the first comma
                                    const char *second_comma = strchr(first_comma + 1, ',');

                                    // If the second comma was found and it's not at the end of the string
                                    if (second_comma != NULL && *(second_comma + 1) != '\0')
                                    {
                                        tabview_name = gui_strdup(second_comma + 1);
                                    }
                                    else
                                    {
                                        // Handle the case where the second comma is not found or is at the end
                                        gui_log("The second comma does not exist or is at the end.\n");
                                    }
                                }
                                else
                                {
                                    // Handle the case where the first comma is not found
                                    gui_log("The first comma is not found.\n");
                                }
                            }
                        }

                        param->id1 = x;
                        param->id2 = y;
                        param->to_widget_name = tabview_name;
                        if (parent->type == BUTTON)
                        {
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_cb_tabview, GUI_EVENT_2, param);
                        }
                        else if (parent->type == WINDOW)
                        {
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_cb_tabview, GUI_EVENT_WIN_LOAD, param);
                        }
                        else if (parent->type == MACRO_KEY)
                        {
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_cb_tabview, GUI_EVENT_WIN_LOAD, param);
                        }
                        else if (parent->type == RADIO)
                        {
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_jump_cb_tabview, GUI_EVENT_WIN_LOAD, param);
                        }
                    }

                }

            }
            else if (!strcmp(type, "control"))
            {
                if (!strcmp(to, "light"))
                {
                    light_param_t *light;
                    light = gui_malloc(sizeof(light_param_t));
                    light->id = x;
                    light->state = (bool)y;
                    if (parent->type == BUTTON)
                    {
                        GUI_API(gui_button_t).on_click((gui_button_t *)parent, (gui_event_cb_t)light_control_cb, light);
                    }
                    else if (parent->type == WINDOW)
                    {
                        gui_win_click((gui_win_t *)parent, (gui_event_cb_t)light_control_cb, light);
                    }
                    else if (parent->type == CLICKSWITCH)
                    {
                        GUI_API(gui_switch_t).on_turn_on((gui_switch_t *)parent,
                                                         (gui_event_cb_t)light_switch_on_cb, light);
                        GUI_API(gui_switch_t).on_turn_off((gui_switch_t *)parent,
                                                          (gui_event_cb_t)light_switch_off_cb, light);
                    }


                    if (parent->type == BUTTON)
                    {
                        gui_obj_add_event_cb(parent, (gui_event_cb_t)light_control_cb, GUI_EVENT_2, light);
                    }
                    else if (parent->type == WINDOW)
                    {
                        gui_obj_add_event_cb(parent, (gui_event_cb_t)light_control_cb, GUI_EVENT_WIN_LOAD, light);
                    }
                    else if (parent->type == MACRO_KEY)
                    {
                        gui_obj_add_event_cb(parent, (gui_event_cb_t)light_control_cb, GUI_EVENT_WIN_LOAD, light);
                    }
                    else if (parent->type == RADIO)
                    {
                        gui_obj_add_event_cb(parent, (gui_event_cb_t)light_control_cb, GUI_EVENT_WIN_LOAD, light);
                    }
                    else if (parent->type == CLICKSWITCH)
                    {
                        gui_obj_add_event_cb(parent, (gui_event_cb_t)light_control_cb, GUI_EVENT_SWITCH_LOAD, light);
                    }
                }

                //gui_log("p->attr[i]:%x\n", (size_t)(p->attr[i]));

            }
            else if ((!strcmp(type, "animatePause")) || (!strcmp(type, "animate")))
            {
                {
                    if (!strcmp(type, "animatePause"))
                    {

                        {
                            char **param = gui_malloc(sizeof(char *) * 3);
                            param[0] = gui_strdup(to);
                            param[2] = (void *)parent;
                            if (id)
                            {
                                param[1] = gui_strdup(id);
                            }
                            if (parent->type == BUTTON)
                            {
                                gui_obj_add_event_cb(parent, (gui_event_cb_t)pause_animation_cb, GUI_EVENT_2, param);
                            }
                            else if (parent->type == WINDOW)
                            {
                                gui_obj_add_event_cb(parent, (gui_event_cb_t)pause_animation_cb, GUI_EVENT_WIN_LOAD, param);
                            }
                            else if (parent->type == CLICKSWITCH)
                            {
                                gui_obj_add_event_cb(parent, (gui_event_cb_t)pause_animation_cb, GUI_EVENT_SWITCH_LOAD, param);
                            }
                            else if (parent->type == IMAGE)
                            {
                                gui_obj_add_event_cb(parent, (gui_event_cb_t)pause_animation_cb, GUI_EVENT_2, param);
                            }
                            else if (parent->type == RADIO)
                            {
                                gui_obj_add_event_cb(parent, (gui_event_cb_t)pause_animation_cb, GUI_EVENT_WIN_LOAD, param);
                            }
                        }
                    }
                    else if (!strcmp(type, "animate"))
                    {

                        if (GUI_STRINGS_EQUAL(to, parent->name))
                        {
                            ezxml_t f = 0;
                            if (f1 != 0)
                            {
                                f = f1;
                            }
                            else
                            {
                                f = ezxml_parse_file(((gui_app_t *)gui_current_app())->xml);
                            }
                            if (id)
                            {
                                foreach_create_animate(f, parent, gui_strdup(id));
                            }
                            //gui_log(" ");
                            if (f1 == 0)
                            {
                                ezxml_free(f);
                            }
                        }
                        else
                        {
                            char **param = gui_malloc(sizeof(char *) * 3);
                            param[0] = gui_strdup(to);
                            param[2] = (void *)parent;
                            if (id)
                            {
                                param[1] = gui_strdup(id);
                            }
                            if (parent->type == BUTTON)
                            {
                                gui_obj_add_event_cb(parent, (gui_event_cb_t)start_animation_cb, GUI_EVENT_2, param);
                            }
                            else if (parent->type == WINDOW)
                            {
                                gui_obj_add_event_cb(parent, (gui_event_cb_t)start_animation_cb, GUI_EVENT_WIN_LOAD, param);
                            }
                            else if (parent->type == CLICKSWITCH)
                            {
                                gui_obj_add_event_cb(parent, (gui_event_cb_t)start_animation_cb, GUI_EVENT_SWITCH_LOAD, param);
                            }
                            else if (parent->type == IMAGE_FROM_MEM)
                            {
                                gui_obj_add_event_cb(parent, (gui_event_cb_t)start_animation_cb, GUI_EVENT_2, param);
                            }
                            else if (parent->type == RADIO)
                            {
                                gui_obj_add_event_cb(parent, (gui_event_cb_t)start_animation_cb, GUI_EVENT_WIN_LOAD, param);
                            }
                        }
                    }
                }
            }
            else if (!strcmp(type, "keyboard"))
            {
                if (to && strlen(to) > 0)
                {
                    GUI_WIDGET_POINTER_BY_NAME(to_text, to);
                    if (to_text->type == TEXTBOX)
                    {
                        gui_text_t *widget_to_text = GUI_TYPE(gui_text_t, to_text);
                        char *content = widget_to_text->content;
                        struct on_click_keyboard_cb_param *param;
                        param = gui_malloc(sizeof(struct on_click_keyboard_cb_param));

                        if (parent->type == BUTTON)
                        {
                            param->to_text = (void *)to_text;
                            param->key_board = get_scan_code_by_keyboard_string(id);
                            if (param->key_board != SCANCODE_UNKNOWN)
                            {
                                gui_obj_add_event_cb(parent, (gui_event_cb_t)on_click_keyboard_cb, GUI_EVENT_WIN_LOAD, param);
                            }
                        }
                    }
                }

                //gui_log("p->attr[i]:%x\n", (size_t)(p->attr[i]));

            }
            else if (!strcmp(type, "connect"))
            {
                if (!strcmp(to, "wifi"))
                {
                    if (parent->type == BUTTON)
                    {
                        bool multi_click = false;
                        for (uint8_t i = 0; i < parent->event_dsc_cnt; i++)
                        {
                            gui_event_dsc_t *event_dsc = parent->event_dsc + i;
                            if (event_dsc->filter == GUI_EVENT_TOUCH_CLICKED)
                            {
                                multi_click = true;
                                break;
                            }
                        }
                        if (multi_click)
                        {
                            gui_win_t *win = gui_win_create(parent, "_win_for_multiclick", 0, 0, parent->w, parent->h);
                            gui_obj_add_event_cb(win, (gui_event_cb_t)wifi_connect, GUI_EVENT_WIN_LOAD, 0);
                        }
                        else
                        {
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)wifi_connect, GUI_EVENT_2, 0);
                        }
                    }


                }

                //gui_log("p->attr[i]:%x\n", (size_t)(p->attr[i]));

            }
            else if (!strcmp(type, "input"))
            {
                if (!strcmp(to, "text"))
                {
                    if (parent->type == BUTTON)
                    {
                        bool multi_click = false;
                        for (uint8_t i = 0; i < parent->event_dsc_cnt; i++)
                        {
                            gui_event_dsc_t *event_dsc = parent->event_dsc + i;
                            if (event_dsc->filter == GUI_EVENT_TOUCH_CLICKED)
                            {
                                multi_click = true;
                                break;
                            }
                        }
                        if (multi_click)
                        {
                            gui_win_t *win = gui_win_create(parent, "_win_for_multiclick", 0, 0, parent->w, parent->h);
                            gui_obj_add_event_cb(win, (gui_event_cb_t)text_input, GUI_EVENT_WIN_LOAD, gui_strdup(id));
                        }
                        else
                        {
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)text_input, GUI_EVENT_2, gui_strdup(id));
                        }
                    }
                    else if (parent->type == WINDOW)
                    {
                        bool multi_click = false;
                        for (uint8_t i = 0; i < parent->event_dsc_cnt; i++)
                        {
                            gui_event_dsc_t *event_dsc = parent->event_dsc + i;
                            if (event_dsc->filter == GUI_EVENT_TOUCH_CLICKED)
                            {
                                multi_click = true;
                                break;
                            }
                        }
                        if (multi_click)
                        {
                            gui_win_t *win = gui_win_create(parent, "_win_for_multiclick", 0, 0, parent->w, parent->h);
                            gui_obj_add_event_cb(win, (gui_event_cb_t)text_input, GUI_EVENT_WIN_LOAD, gui_strdup(id));
                        }
                        else
                        {
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)text_input, GUI_EVENT_WIN_LOAD, gui_strdup(id));
                        }
                    }

                }

                //gui_log("p->attr[i]:%x\n", (size_t)(p->attr[i]));

            }
            else if (!strcmp(type, "no input"))
            {
                if (!strcmp(to, "text"))
                {
                    if (parent->type == BUTTON)
                    {
                        bool multi_click = false;
                        for (uint8_t i = 0; i < parent->event_dsc_cnt; i++)
                        {
                            gui_event_dsc_t *event_dsc = parent->event_dsc + i;
                            if (event_dsc->filter == GUI_EVENT_TOUCH_CLICKED)
                            {
                                multi_click = true;
                                break;
                            }
                        }
                        if (multi_click)
                        {
                            gui_win_t *win = gui_win_create(parent, "_win_for_multiclick", 0, 0, parent->w, parent->h);
                            gui_obj_add_event_cb(win, (gui_event_cb_t)text_no_input, GUI_EVENT_WIN_LOAD, gui_strdup(id));
                        }
                        else
                        {
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)text_no_input, GUI_EVENT_2, gui_strdup(id));
                        }
                    }
                    else if (parent->type == WINDOW)
                    {
                        bool multi_click = false;
                        for (uint8_t i = 0; i < parent->event_dsc_cnt; i++)
                        {
                            gui_event_dsc_t *event_dsc = parent->event_dsc + i;
                            if (event_dsc->filter == GUI_EVENT_TOUCH_CLICKED)
                            {
                                multi_click = true;
                                break;
                            }
                        }
                        if (multi_click)
                        {
                            gui_win_t *win = gui_win_create(parent, "_win_for_multiclick", 0, 0, parent->w, parent->h);
                            gui_obj_add_event_cb(win, (gui_event_cb_t)text_no_input, GUI_EVENT_WIN_LOAD, gui_strdup(id));
                        }
                        else
                        {
                            gui_obj_add_event_cb(parent, (gui_event_cb_t)text_no_input, GUI_EVENT_WIN_LOAD, gui_strdup(id));
                        }
                    }

                }

                //gui_log("p->attr[i]:%x\n", (size_t)(p->attr[i]));

            }
        }

    }

    return parent;
}
static gui_obj_t *widget_create_type_scroll_wheel_new(ezxml_t p, gui_obj_t *parent,
                                                      T_OBJ_TYPE widget_type)
{

    {
        /*<roller
            id="scrollWheel1"
            x="50"               <!-- X coordinate -->
            y="100"              <!-- Y coordinate -->
            w="200"              <!-- Width -->
            h="300"              <!-- Height -->
            items="Item1,Item2,Item3,Item4,Item5,Item6,Item7,Item8,Item9,Item10,Item11 "  <!-- Items displayed in each row of the scroll wheel -->
            itemCount="11"        <!-- Total number of items -->
            rowCount="3"         <!-- Number of visible rows in the scroll wheel -->
            rowSpacing="20"      <!-- Spacing between each row -->
        ></roller>*/

        size_t i = 0;
        int16_t x = 0;
        int16_t y = 0;
        int16_t w = 0;
        int16_t h = 0; GUI_UNUSED(h);
        int16_t row_count = 0;
        int16_t row_spacing = 0;
        int16_t item_count = 0;
        const char *items = "NULL";
        int column_offset = 0; GUI_UNUSED(column_offset);
        const char *font = 0;
        int16_t font_size = 0;
        gui_color_t  font_color = {0};
        gui_color_t  font_color_highlight = {0};
        gui_color_t  item_color = {0}; GUI_UNUSED(item_color);
        gui_color_t  item_color_highlight = {0}; GUI_UNUSED(item_color_highlight);
        bool loop = 1;
        TEXT_MODE style = LEFT;
        while (true)
        {
            if (!(p->attr[i]))
            {
                break;
            }

            //gui_log("p->attr[i]:%s\n", p->attr[i]);

            if (!strcmp(p->attr[i], "x"))
            {
                x = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "y"))
            {
                y = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "w"))
            {
                w = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "h"))
            {
                h = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "rowCount"))
            {
                row_count = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "rowSpacing"))
            {
                row_spacing = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "items"))
            {
                items = p->attr[++i];
            }
            else if (!strcmp(p->attr[i], "font"))
            {
                font = p->attr[++i];
            }
            else if (!strcmp(p->attr[i], "fontSize"))
            {
                font_size = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "fontColor"))
            {
                //font_color = string_rgb888_to_rgb565(p->attr[++i]);
                font_color = string_rgb888(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "fontColorHighlight"))
            {
                //font_color = string_rgb888_to_rgb565(p->attr[++i]);
                font_color_highlight = string_rgb888(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "itemColor"))
            {
                //font_color = string_rgb888_to_rgb565(p->attr[++i]);
                item_color = string_rgb888(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "itemColorHighlight"))
            {
                //font_color = string_rgb888_to_rgb565(p->attr[++i]);
                item_color_highlight = string_rgb888(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "loop"))
            {
                if (!strcmp(p->attr[++i], "loop"))
                {
                    loop = 1;
                }
                else if (!strcmp(p->attr[i], "true"))
                {
                    loop = 1;
                }
                else if (!strcmp(p->attr[i], "false"))
                {
                    loop = 0;
                }
                else if (!strcmp(p->attr[i], "disable"))
                {
                    loop = 0;
                }
                else if (!strcmp(p->attr[i], "not loop"))
                {
                    loop = 0;
                }
            }
            else if (!strcmp(p->attr[i], "fontMode"))
            {
                char *s = p->attr[++i];
                if (!strcmp(p->attr[i], "truncate"))
                {
                    style = (TEXT_MODE)0;
                }
                else if (!strcmp(p->attr[i], "verticalscroll"))
                {
                    style = SCROLL_Y;
                }
                else if (!strcmp(p->attr[i], "left"))
                {
                    style = LEFT;
                }
                else if (!strcmp(p->attr[i], "center"))
                {
                    style = CENTER;
                }
                else if (!strcmp(p->attr[i], "right"))
                {
                    style = RIGHT;
                }
            }
            i++;
        }
        {
            char *items_copy = gui_strdup(items); // Make a copy of items to use with strtok
            char *token = strtok(items_copy, ",");
            int j = 0;
            while (token != NULL)
            {
                j++;
                token = strtok(NULL, ",");
            }
            item_count = j;
            gui_free(items_copy);
        }
        if (item_count == 0)
        {
            item_count = 1;
            items = "NULL";
        }
        // Split items into an array of strings
        char *items_copy = gui_strdup(items); // Make a copy of items to use with strtok
        const char **string_array = gui_malloc(item_count * sizeof(char *));
        char *token = strtok(items_copy, ",");
        int j = 0;
        while (token != NULL && j < item_count)
        {
            string_array[j++] = token;
            token = strtok(NULL, ",");
        }
        //gui_free(items_copy);
        int blank_count = 0;
        if (loop)
        {
            if (item_count - 2 < row_count)
            {
                int item_count_temp = item_count;
                if (item_count > row_count)
                {
                    item_count_temp = item_count * 2;
                }
                else
                {
                    item_count_temp = (row_count / item_count + 2) * item_count;
                }
                const char **string_array_temp = gui_malloc(item_count_temp * sizeof(char *));
                for (size_t i = 0; i < item_count_temp; i++)
                {
                    string_array_temp[i] = string_array[i % item_count];
                }
                gui_free(string_array);
                string_array = string_array_temp;
                item_count = item_count_temp;
            }
        }
        else
        {

            {
                int item_count_temp = _UI_MAX(row_count + 2 + row_count / 2, item_count + 2 + item_count / 2) ;
                const char **string_array_temp = gui_malloc(item_count_temp * sizeof(char *));
                for (size_t i = 0; i < item_count_temp; i++)
                {
                    if (i < item_count)
                    {
                        string_array_temp[i] = string_array[i];
                    }
                    else
                    {
                        string_array_temp[i] = " ";
                    }


                }
                blank_count = item_count_temp - item_count;
                gui_free(string_array);
                string_array = string_array_temp;
                item_count = item_count_temp;
            }
        }



        // Create the scroll wheel widget
        gui_scroll_wheel_new_t *scroll_wheel = gui_scroll_wheel_new_create(
                                                   parent, x, y, w, row_spacing, row_count, string_array, item_count
                                               );
        if (scroll_wheel == NULL)
        {
            gui_log("Failed to create scroll wheel widget.\n");
            return 0;;
        }
        scroll_wheel->font_color = font_color;
        scroll_wheel->font_color_highlight = font_color_highlight;
        extern void gui_scroll_wheel_new_render_text_alien(gui_scroll_wheel_new_t *widget,
                                                           const void *font_file_pointer,
                                                           int font_size, TEXT_MODE mode);
        gui_scroll_wheel_new_render_text_alien(scroll_wheel, gui_get_file_address(font), font_size, style);
        scroll_wheel->loop = loop;
        scroll_wheel->blank_count = blank_count;
        parent = GUI_BASE(scroll_wheel);

    }

    return parent;
}
#include "gui_pagelist_new.h"
static void ticket(void *obj, gui_event_t e, void *param)
{
    static int i;
    gui_log("ticket%d\n", i++);
}
extern void gui_page_list_new_win_press(void *obj, gui_event_t e, void *param);
extern void gui_page_list_new_win_release(void *obj, gui_event_t e, void *param);
extern void gui_page_list_new_override(void *p, void *this_widget, gui_animate_t *animate);
extern void gui_page_list_new_override_horizontal(void *p, void *this_widget,
                                                  gui_animate_t *animate);
static gui_error_t xml_dom_page_list_new_render(gui_pagelist_new_t *pagelist_new,
                                                const uint16_t item_count,
                                                const gui_event_cb_t *item_click_function_array,
                                                const char **item_text_array
                                               )
{
    if (!(item_count  && item_text_array && pagelist_new))
    {
        return GUI_ERROR_NULL;
    }
    gui_obj_child_free((void *)pagelist_new);
    {
        if (pagelist_new->click_function_array)
        {
            if (pagelist_new->append)
            {
                gui_free((void *)pagelist_new->click_function_array);
            }
        }
        if (pagelist_new->item_text_array)
        {
            if (pagelist_new->append)
            {
                gui_free(pagelist_new->item_text_array);
            }
        }
        pagelist_new->append = false;
        pagelist_new->item_count = item_count;
        pagelist_new->click_function_array = (void *)item_click_function_array;
        pagelist_new->item_text_array = item_text_array;
    }
    gui_win_t *win = gui_win_create(pagelist_new, 0, 0, 0, GUI_BASE(pagelist_new)->w,
                                    GUI_BASE(pagelist_new)->h);
    gui_win_t *timer1 = gui_win_create(win, 0, 0, 0, 0, 0);

    pagelist_new->timer = timer1;
    int count = pagelist_new->row_count;
    if (item_count < count)
    {
        count = item_count;
    }

    if (pagelist_new->horizontal)
    {
        gui_win_set_animate(win, 1000, -1, (gui_animate_callback_t)gui_page_list_new_override_horizontal,
                            pagelist_new);
        for (size_t i = 0; i < count; i++)
        {
            gui_win_t *win = gui_win_create(timer1, "_page_list_new_click_win", i * pagelist_new->row_space, 0,
                                            pagelist_new->row_space,
                                            GUI_BASE(pagelist_new)->h);
            gui_img_t *img = gui_img_create_from_mem(win, 0, (void *)pagelist_new->item_image, 0, 0, 0, 0);
            gui_img_set_mode(img, pagelist_new->blending);
            const char *text = pagelist_new->item_text_array[i];
            gui_text_t *t = gui_text_create(win, 0, 0, 0, pagelist_new->row_space, GUI_BASE(win)->h);
            //extern void gui_xml_dom_text_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type);
            GUI_BASE(t)->obj_cb = gui_xml_dom_text_cb;
            gui_text_set(t, (void *)text, GUI_FONT_SRC_BMP, pagelist_new->font_color, strlen(text),
                         pagelist_new->font_size);
            gui_text_type_set(t, (void *)pagelist_new->font, FONT_SRC_MEMADDR);
            gui_text_mode_set(t, pagelist_new->text_mode);
            if (item_click_function_array && pagelist_new->click_function_array[i])
            {
                gui_win_click(win, (gui_event_cb_t)pagelist_new->click_function_array[i], (gui_event_cb_t)i);
            }

            gui_win_press(win, gui_page_list_new_win_press, (void *)i);
            gui_win_release(win, gui_page_list_new_win_release, (void *)i);
        }
    }
    else
    {
        gui_win_set_animate(win, 1000, -1, (gui_animate_callback_t)gui_page_list_new_override,
                            pagelist_new);
        for (size_t i = 0; i < count; i++)
        {
            gui_win_t *win = gui_win_create(timer1, "_page_list_new_click_win", 0, i * pagelist_new->row_space,
                                            GUI_BASE(pagelist_new)->w, pagelist_new->row_space);
            gui_img_t *img = gui_img_create_from_mem(win, 0, (void *)pagelist_new->item_image, 0, 0, 0, 0);
            gui_img_set_mode(img, pagelist_new->blending);
            const char *text = pagelist_new->item_text_array[i];
            gui_text_t *t = gui_text_create(win, 0, 0, 0, GUI_BASE(win)->w, pagelist_new->row_space);
            //extern void gui_xml_dom_text_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type);
            GUI_BASE(t)->obj_cb = gui_xml_dom_text_cb;
            gui_text_set(t, (void *)text, GUI_FONT_SRC_BMP, pagelist_new->font_color, strlen(text),
                         pagelist_new->font_size);
            gui_text_type_set(t, (void *)pagelist_new->font, FONT_SRC_MEMADDR);
            gui_text_mode_set(t, pagelist_new->text_mode);

            if (item_click_function_array && pagelist_new->click_function_array[i])
            {
                gui_win_click(win, (gui_event_cb_t)pagelist_new->click_function_array[i], (gui_event_cb_t)i);
            }
            gui_win_press(win, gui_page_list_new_win_press, (void *)i);
            gui_win_release(win, gui_page_list_new_win_release, (void *)i);
        }
    }
    return GUI_SUCCESS;

}

static gui_error_t xml_dom_page_list_new_render_param(gui_pagelist_new_t *pagelist_new,
                                                      const uint16_t item_count,
                                                      const gui_event_cb_t *item_click_function_array,
                                                      const char **item_text_array,
                                                      void  **item_click_function_array_param
                                                     )
{
    if (!(item_count  && item_text_array && pagelist_new))
    {
        return GUI_ERROR_NULL;
    }
    gui_obj_child_free((void *)pagelist_new);
    {
        if (pagelist_new->click_function_array)
        {
            if (pagelist_new->append)
            {
                gui_free((void *)pagelist_new->click_function_array);
            }
        }
        if (pagelist_new->item_text_array)
        {
            if (pagelist_new->append)
            {
                gui_free(pagelist_new->item_text_array);
            }
        }
        pagelist_new->append = false;
        pagelist_new->item_count = item_count;
        pagelist_new->click_function_array = (void *)item_click_function_array;
        pagelist_new->item_text_array = item_text_array;
    }
    gui_win_t *win = gui_win_create(pagelist_new, 0, 0, 0, GUI_BASE(pagelist_new)->w,
                                    GUI_BASE(pagelist_new)->h);
    gui_win_t *timer1 = gui_win_create(win, 0, 0, 0, 0, 0);

    pagelist_new->timer = timer1;
    int count = pagelist_new->row_count;
    if (item_count < count)
    {
        count = item_count;
    }

    if (pagelist_new->horizontal)
    {
        gui_win_set_animate(win, 1000, -1, (gui_animate_callback_t)gui_page_list_new_override_horizontal,
                            pagelist_new);
        for (size_t i = 0; i < count; i++)
        {
            gui_win_t *win = gui_win_create(timer1, "_page_list_new_click_win", i * pagelist_new->row_space, 0,
                                            pagelist_new->row_space,
                                            GUI_BASE(pagelist_new)->h);
            gui_img_t *img = gui_img_create_from_mem(win, 0, (void *)pagelist_new->item_image, 0, 0, 0, 0);
            gui_img_set_mode(img, pagelist_new->blending);
            const char *text = pagelist_new->item_text_array[i];
            gui_text_t *t = gui_text_create(win, 0, 0, 0, pagelist_new->row_space, GUI_BASE(win)->h);
            //extern void gui_xml_dom_text_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type);
            GUI_BASE(t)->obj_cb = gui_xml_dom_text_cb;
            gui_text_set(t, (void *)text, GUI_FONT_SRC_BMP, pagelist_new->font_color, strlen(text),
                         pagelist_new->font_size);
            gui_text_type_set(t, (void *)pagelist_new->font, FONT_SRC_MEMADDR);
            gui_text_mode_set(t, pagelist_new->text_mode);
            if (item_click_function_array && pagelist_new->click_function_array[i])
            {
                struct on_click_jump_cb_param *p =  item_click_function_array_param[i];
                gui_win_click(win, (gui_event_cb_t)pagelist_new->click_function_array[i],
                              item_click_function_array_param[i]);
            }

            gui_win_press(win, gui_page_list_new_win_press, (void *)i);
            gui_win_release(win, gui_page_list_new_win_release, (void *)i);
        }
    }
    else
    {
        gui_win_set_animate(win, 1000, -1, (gui_animate_callback_t)gui_page_list_new_override,
                            pagelist_new);
        for (size_t i = 0; i < count; i++)
        {
            gui_win_t *win = gui_win_create(timer1, "_page_list_new_click_win", 0, i * pagelist_new->row_space,
                                            GUI_BASE(pagelist_new)->w, pagelist_new->row_space);
            gui_img_t *img = gui_img_create_from_mem(win, 0, (void *)pagelist_new->item_image, 0, 0, 0, 0);
            gui_img_set_mode(img, pagelist_new->blending);
            const char *text = pagelist_new->item_text_array[i];
            gui_text_t *t = gui_text_create(win, 0, 0, 0, GUI_BASE(win)->w, pagelist_new->row_space);
            //extern void gui_xml_dom_text_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type);
            GUI_BASE(t)->obj_cb = gui_xml_dom_text_cb;
            gui_text_set(t, (void *)text, GUI_FONT_SRC_BMP, pagelist_new->font_color, strlen(text),
                         pagelist_new->font_size);
            gui_text_type_set(t, (void *)pagelist_new->font, FONT_SRC_MEMADDR);
            gui_text_mode_set(t, pagelist_new->text_mode);

            if (item_click_function_array && pagelist_new->click_function_array[i])
            {
                struct on_click_jump_cb_param *p =  item_click_function_array_param[i];
                gui_win_click(win, (gui_event_cb_t)pagelist_new->click_function_array[i],
                              item_click_function_array_param[i]);
            }
            gui_win_press(win, gui_page_list_new_win_press, (void *)i);
            gui_win_release(win, gui_page_list_new_win_release, (void *)i);
        }
    }
    return GUI_SUCCESS;

}
static gui_obj_t *widget_create_macro_page_list_new(ezxml_t p, gui_obj_t *parent,
                                                    T_OBJ_TYPE widget_type)
{
    {

        size_t i = 0;
        int16_t x = 0;
        int16_t y = 0;
        int16_t w = 0;
        int16_t h = 0; GUI_UNUSED(h);
        int16_t row_spacing = 0;
        int16_t item_count = 0;
        const char *items = "NULL";
        const char *font = 0;
        int16_t font_size = 0;
        gui_color_t  font_color = {0};
        bool horizontal = 1;
        TEXT_MODE style = CENTER;
        BLEND_MODE_TYPE blendMode = IMG_BYPASS_MODE;
        char *picture = NULL;
        char *hl_picture = NULL;
        char *st_picture = NULL;
        while (true)
        {
            if (!(p->attr[i]))
            {
                break;
            }

            //gui_log("p->attr[i]:%s\n", p->attr[i]);

            if (!strcmp(p->attr[i], "x"))
            {
                x = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "y"))
            {
                y = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "w"))
            {
                w = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "h"))
            {
                h = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "rowSpacing"))
            {
                row_spacing = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "items"))
            {
                items = p->attr[++i];
            }
            else if (!strcmp(p->attr[i], "font"))
            {
                font = p->attr[++i];
            }
            else if (!strcmp(p->attr[i], "fontSize"))
            {
                font_size = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "fontColor"))
            {
                //font_color = string_rgb888_to_rgb565(p->attr[++i]);
                font_color = string_rgb888(p->attr[++i]);
            }

            else if (!strcmp(p->attr[i], "horizontal") || !strcmp(p->attr[i], "orientation"))
            {
                if (!strcmp(p->attr[++i], "horizontal"))
                {
                    horizontal = 1;
                }
                else if (!strcmp(p->attr[i], "true"))
                {
                    horizontal = 1;
                }
                else if (!strcmp(p->attr[i], "false"))
                {
                    horizontal = 0;
                }
                else if (!strcmp(p->attr[i], "vertical"))
                {
                    horizontal = 0;
                }
                else if (!strcmp(p->attr[i], "not horizontal"))
                {
                    horizontal = 0;
                }
            }
            else if (!strcmp(p->attr[i], "fontMode"))
            {
                char *s = p->attr[++i];
                if (!strcmp(p->attr[i], "truncate"))
                {
                    style = (TEXT_MODE)0;
                }
                else if (!strcmp(p->attr[i], "verticalscroll"))
                {
                    style = SCROLL_Y;
                }
                else if (!strcmp(p->attr[i], "left"))
                {
                    style = LEFT;
                }
                else if (!strcmp(p->attr[i], "center"))
                {
                    style = CENTER;
                }
                else if (!strcmp(p->attr[i], "right"))
                {
                    style = RIGHT;
                }
            }
            else if (!strcmp(p->attr[i], "picture"))
            {
                picture = (p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "highlightPicture"))
            {
                hl_picture = (p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "selectedPicture"))
            {
                st_picture = (p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "blendMode"))
            {
                i++;
                if (!strcmp(p->attr[i], "imgBypassMode"))
                {
                    blendMode = IMG_BYPASS_MODE;
                }
                else if (!strcmp(p->attr[i], "imgFilterBlack"))
                {
                    blendMode = IMG_FILTER_BLACK;
                }
                else if (!strcmp(p->attr[i], "imgSrcOverMode"))
                {
                    blendMode = IMG_SRC_OVER_MODE;
                }
                else if (!strcmp(p->attr[i], "imgCoverMode"))
                {
                    blendMode = IMG_COVER_MODE;
                }
                GUI_UNUSED(blendMode);
            }
            i++;
        }
        {
            char *items_copy = gui_strdup(items); // Make a copy of items to use with strtok
            char *token = strtok(items_copy, ",");
            int j = 0;
            while (token != NULL)
            {
                j++;
                token = strtok(NULL, ",");
            }
            item_count = j;
            gui_free(items_copy);
        }
        if (item_count == 0)
        {
            item_count = 1;
            items = "NULL";
        }
        // Split items into an array of strings
        char *items_copy = gui_strdup(items); // Make a copy of items to use with strtok
        const char **string_array = gui_malloc(item_count * sizeof(char *));
        char *token = strtok(items_copy, ",");
        int j = 0;
        while (token != NULL && j < item_count)
        {
            string_array[j++] = token;
            token = strtok(NULL, ",");
        }
        if (w == 0)
        {
            w = gui_get_screen_width();
        }
        if (w == 0)
        {
            w = 400;
        }
        parent = (void *)gui_win_create(parent, "_pagelist_scope", x, y, w, h);
        gui_win_set_scope((void *)parent, 1);
        if (!horizontal)
        {
            parent = (void *)gui_pagelist_new_create(parent,
                                                     0,
                                                     0,
                                                     w,
                                                     row_spacing,
                                                     gui_get_file_address(picture),
                                                     gui_get_file_address(hl_picture),
                                                     blendMode,
                                                     gui_get_file_address(font),
                                                     font_size,
                                                     font_color);
        }
        else
        {
            parent = (void *)gui_pagelist_new_create_horizontal(parent,
                                                                0,
                                                                0,
                                                                w,
                                                                row_spacing,
                                                                gui_get_file_address(picture),
                                                                gui_get_file_address(hl_picture),
                                                                blendMode,
                                                                gui_get_file_address(font),
                                                                font_size,
                                                                font_color);
        }

        GUI_TYPE(gui_pagelist_new_t, parent)->text_mode = style;
        gui_event_cb_t *item_click_function_array = gui_malloc(sizeof(gui_event_cb_t) * item_count);
        for (size_t i = 0; i < item_count; i++)
        {
            item_click_function_array[i] = ticket;
        }
        xml_dom_page_list_new_render((void *)parent, item_count, item_click_function_array, string_array);
        parent->h = h;
        parent->name = get_space_string_head(p->txt);
        GUI_TYPE(gui_pagelist_new_t, parent)->item_image_selected = gui_get_file_address(st_picture);
    }
    return parent;
}
static gui_obj_t *widget_create_macro_calendar(ezxml_t p, gui_obj_t *parent, T_OBJ_TYPE widget_type)
{

    {

        /*    <calendar
        x="0"
        y="360"
        w="800"
        h="400"
        fontColor="#ff000000"
        fontSize="16"
        font="app/box/resource/font/arialbd_size16_bits4_font.bin">calendar
        </calendar>*/

        size_t i = 0;
        int16_t x = 0;
        int16_t y = 0;
        int16_t w = 0;
        int16_t h = 0;
        gui_color_t color_p = gui_color_css("DarkSlateGray");
        gui_color_t font_color_highlight = gui_color_css("#FFFFFFFF");;
        gui_color_t item_color = gui_color_css("#FFFFFF");;
        gui_color_t item_color_highlight = gui_color_css("rgba(135, 206, 250, 1.0)");;
        gui_color_t title_color = gui_color_css("rgb(255, 255, 255)");;
        gui_color_t title_background_color = gui_color_css("hsl(203, 92%, 75%)");;
        const char *font = 0;
        int16_t font_size = 0;
        while (true)
        {
            if (!(p->attr[i]))
            {
                break;
            }

            //gui_log("p->attr[i]:%s\n", p->attr[i]);

            if (!strcmp(p->attr[i], "x"))
            {
                x = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "y"))
            {
                y = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "w"))
            {
                w = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "h"))
            {
                h = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "fontColor"))
            {
                //font_color = string_rgb888_to_rgb565(p->attr[++i]);
                color_p = string_rgb888(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "fontColorHighlight"))
            {
                //font_color = string_rgb888_to_rgb565(p->attr[++i]);
                font_color_highlight = string_rgb888(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "itemColor"))
            {
                //font_color = string_rgb888_to_rgb565(p->attr[++i]);
                item_color = string_rgb888(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "itemColorHighlight"))
            {
                //font_color = string_rgb888_to_rgb565(p->attr[++i]);
                item_color_highlight = string_rgb888(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "titleColor"))
            {
                //font_color = string_rgb888_to_rgb565(p->attr[++i]);
                title_color = string_rgb888(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "titleBackgroundColor"))
            {
                //font_color = string_rgb888_to_rgb565(p->attr[++i]);
                title_background_color = string_rgb888(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "font"))
            {
                font = p->attr[++i];
            }
            else if (!strcmp(p->attr[i], "fontSize"))
            {
                font_size = atoi(p->attr[++i]);
            }
            i++;
        }
        gui_calender_create(parent, x, y, w, h, gui_get_file_address(font), font_size, color_p,
                            font_color_highlight, item_color, item_color_highlight, title_color, title_background_color);

    }

    return parent;
}
static gui_obj_t *widget_create_macro_ontime(ezxml_t p, gui_obj_t *parent, T_OBJ_TYPE widget_type)
{

    {
        char *type = 0;
        char *to = 0; GUI_UNUSED(to);
        char *id = 0; GUI_UNUSED(id);
        size_t i = 0;
        while (true)
        {
            if (!(p->attr[i]))
            {
                break;
            }
            if (!strcmp(p->attr[i], "type"))
            {
                type = (p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "to"))
            {
                to = (p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "id"))
            {
                id = (p->attr[++i]);
            }
            i++;
        }
        switch (parent->type)
        {
        case IMAGE_FROM_MEM:
            {
                if (type)
                {
                    if (id)
                    {
                        char *token;
                        int numbers[2];
                        int index = 0;
                        token = strtok(id, ",");
                        while (token != NULL && index < 2)
                        {
                            numbers[index] = atoi(token);
                            index++;
                            token = strtok(NULL, ",");
                        }

                        two_integers *data = (two_integers *)gui_malloc(sizeof(two_integers));
                        if (data)
                        {


                            data->num1 = numbers[0];
                            data->num2 = numbers[1];

                            if (!strcmp(type, "hour"))
                            {

                                gui_img_append_animate((void *)parent, 1000, -1, img_animate_watchface_callback_hour,
                                                       (void *)data, "hour");
                            }
                            else if (!strcmp(type, "minute"))
                            {
                                gui_img_append_animate((void *)parent, 1000, -1, img_animate_watchface_callback_minute,
                                                       (void *)data, "minute");
                            }
                            else if (!strcmp(type, "second"))
                            {
                                gui_img_append_animate((void *)parent, 1000, -1, img_animate_watchface_callback_second,
                                                       (void *)data, "second");
                            }
                            parent->obj_cb = img_ontime_render;
                        }
                    }
                    else
                    {
                        if (!strcmp(type, "hour"))
                        {
#define HOUR_ANIMATION 1
#define MINUTE_ANIMATION 2
#define SECOND_ANIMATION 3
                            gui_img_append_animate((void *)parent, 1000, -1, img_animate_watchface_callback,
                                                   (void *)HOUR_ANIMATION, "hour");
                        }
                        else if (!strcmp(type, "minute"))
                        {
                            gui_img_append_animate((void *)parent, 1000, -1, img_animate_watchface_callback,
                                                   (void *)MINUTE_ANIMATION, "minute");
                        }
                        else if (!strcmp(type, "second"))
                        {
                            gui_img_append_animate((void *)parent, 1000, -1, img_animate_watchface_callback,
                                                   (void *)SECOND_ANIMATION, "second");
                        }
                    }
                }

            }
            break;
        case TEXTBOX:
            {
                if (type)
                {
                    gui_text_set_animate(parent, 1000, -1, text_animate_watchface_callback, gui_strdup(type));
                }


            }
            break;
        default:
            break;
        }



    }

    return parent;
}
static gui_obj_t *widget_create_macro_combo(ezxml_t p, gui_obj_t *parent, T_OBJ_TYPE widget_type)
{

    {
        /*<combo
            x="450"
            y="400"
            w="200"
            items="Item1,Item2,Item3,Item4,Item5"
            rowSpacing="32"
            fontSize="16"
            font="app/box/resource/font/arialbd_size16_bits4_font.bin"
            fontColor="#000000"
            picture="app\\box\\resource\\combo\\rect20.bin"
            highlightPicture="app\\box\\resource\\combo\\rect20hl.bin"
            blendMode="imgBypassMode">combo1
        </combo>*/

        size_t i = 0;
        int16_t x = 0;
        int16_t y = 0;
        int16_t w = 0;
        int16_t row_spacing = 0;
        int16_t item_count = 0;
        const char *items = "null,null";
        const char *font = 0;
        int16_t font_size = 0;
        gui_color_t  font_color = {0};
        char *picture = NULL;
        char *hl_picture = NULL;
        char *item_selection_picture = NULL;
        char *selector_picture = NULL;
        char *selector_picture_collapsed = NULL;
        char *selector_picture_hl = NULL;
        char *selector_picture_collapsed_hl = NULL;
        const char *font_selection = 0;
        int16_t font_size_selection = 0;
        gui_color_t  font_color_selection = {0};
        gui_color_t  font_color_highlight = {0};
        gui_color_t  background_color = {0};
        // default image blend_mode
        BLEND_MODE_TYPE blendMode = IMG_FILTER_BLACK;
        while (true)
        {
            if (!(p->attr[i]))
            {
                break;
            }

            //gui_log("p->attr[i]:%s\n", p->attr[i]);

            if (!strcmp(p->attr[i], "x"))
            {
                x = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "y"))
            {
                y = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "w"))
            {
                w = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "rowSpacing"))
            {
                row_spacing = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "items"))
            {
                items = p->attr[++i];
            }
            else if (!strcmp(p->attr[i], "font"))
            {
                font = p->attr[++i];
            }
            else if (!strcmp(p->attr[i], "fontSize"))
            {
                font_size = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "fontColor"))
            {
                //font_color = string_rgb888_to_rgb565(p->attr[++i]);
                font_color = string_rgb888(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "fontSelection"))
            {
                font_selection = p->attr[++i];
            }
            else if (!strcmp(p->attr[i], "fontSizeSelection"))
            {
                font_size_selection = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "fontColorSelection"))
            {
                //font_color = string_rgb888_to_rgb565(p->attr[++i]);
                font_color_selection = string_rgb888(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "fontColorHighlight"))
            {
                //font_color = string_rgb888_to_rgb565(p->attr[++i]);
                font_color_highlight = string_rgb888(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "backgroundColor"))
            {
                //font_color = string_rgb888_to_rgb565(p->attr[++i]);
                background_color = string_rgb888(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "itemPicture"))
            {
                picture = gui_strdup(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "itemPictureHighlight"))
            {
                hl_picture = gui_strdup(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "itemPictureSelection"))
            {
                item_selection_picture = gui_strdup(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "selectionPicture"))
            {
                selector_picture = gui_strdup(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "selectionPictureCollapsed"))
            {
                selector_picture_collapsed = gui_strdup(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "selectionPictureHighlight"))
            {
                selector_picture_hl = gui_strdup(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "selectionPictureCollapsedHighlight"))
            {
                selector_picture_collapsed_hl = gui_strdup(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "blendMode"))
            {
                i++;
                if (!strcmp(p->attr[i], "imgBypassMode"))
                {
                    blendMode = IMG_BYPASS_MODE;
                }
                else if (!strcmp(p->attr[i], "imgFilterBlack"))
                {
                    blendMode = IMG_FILTER_BLACK;
                }
                else if (!strcmp(p->attr[i], "imgSrcOverMode"))
                {
                    blendMode = IMG_SRC_OVER_MODE;
                }
                else if (!strcmp(p->attr[i], "imgCoverMode"))
                {
                    blendMode = IMG_COVER_MODE;
                }
                GUI_UNUSED(blendMode);
            }
            i++;
        }
        {
            char *items_copy = gui_strdup(items); // Make a copy of items to use with strtok
            char *token = strtok(items_copy, ",");
            int j = 0;
            while (token != NULL)
            {
                j++;
                token = strtok(NULL, ",");
            }
            item_count = j;
            gui_free(items_copy);
        }
        if (item_count == 0)
        {
            item_count = 1;
            items = "NULL";
        }
        // Split items into an array of strings
        char *items_copy = gui_strdup(items); // Make a copy of items to use with strtok
        const char **string_array = gui_malloc(item_count * sizeof(char *));
        char *token = strtok(items_copy, ",");
        int j = 0;
        while (token != NULL && j < item_count)
        {
            string_array[j++] = token;
            token = strtok(NULL, ",");
        }
        gui_combo_t *combo = gui_combo_create(parent, x, y, w, item_count,
                                              row_spacing,
                                              string_array, gui_get_image_file_address(picture),
                                              gui_get_image_file_address(hl_picture),
                                              gui_get_image_file_address(item_selection_picture),
                                              gui_get_image_file_address(selector_picture),
                                              gui_get_image_file_address(selector_picture_hl),
                                              gui_get_image_file_address(selector_picture_collapsed),
                                              gui_get_image_file_address(selector_picture_collapsed_hl),
                                              background_color,
                                              font_size,
                                              font_color,
                                              gui_get_file_address(font),
                                              font_size_selection,
                                              font_color_selection, gui_get_file_address(font_selection), font_color_highlight, blendMode);


        // parent = GUI_BASE(combo);

    }

    return parent;
}
static gui_obj_t *widget_create_macro_on_peripheral(ezxml_t p, gui_obj_t *parent,
                                                    T_OBJ_TYPE widget_type)
{

    {
        char *type = 0;
        char *to = 0; GUI_UNUSED(to);
        char *id = 0;
        size_t i = 0;
        while (true)
        {
            if (!(p->attr[i]))
            {
                break;
            }
            if (!strcmp(p->attr[i], "type"))
            {
                type = (p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "to"))
            {
                to = (p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "id"))
            {
                id = (p->attr[++i]);
            }
            i++;
        }
        if (!id)
        {
            return parent;
        }
        switch (parent->type)
        {
        case IMAGE_FROM_MEM:
            {
                if (type)
                {
                    if (!strcmp(type, "activity"))
                    {
                        if (!strcmp(id, "exercise"))
                        {
                            arc_animation_param_t *param = 0;
                            ezxml_t pt = p->parent;
                            if (!strcmp(p->parent->name, "arc"))
                            {
                                ezxml_t p = pt;
                                size_t i = 0;
                                int16_t cx = 100; GUI_UNUSED(cx);
                                int16_t cy = 100; GUI_UNUSED(cy);
                                int16_t r = 100; GUI_UNUSED(r);
                                int16_t stroke_width = 10; GUI_UNUSED(stroke_width);
                                int cap = 0; GUI_UNUSED(cap);
                                gui_color_t stroke = APP_COLOR_RED;
                                float sd = 0; GUI_UNUSED(sd);
                                float ed = 100; GUI_UNUSED(ed);
                                int dir = 0;
                                while (true)
                                {
                                    if (!(p->attr[i]))
                                    {
                                        break;
                                    }
                                    ////gui_log("p->attr[i]:%s\n", p->attr[i]);
                                    if (!strcmp(p->attr[i], "cx") || !strcmp(p->attr[i], "centralX") || !strcmp(p->attr[i], "centerX"))
                                    {
                                        cx = atoi(p->attr[++i]);
                                    }
                                    else if (!strcmp(p->attr[i], "cy") || !strcmp(p->attr[i], "centerY") ||
                                             !strcmp(p->attr[i], "centralY"))
                                    {
                                        cy = atoi(p->attr[++i]);
                                    }
                                    else if (!strcmp(p->attr[i], "r") || !strcmp(p->attr[i], "radius"))
                                    {
                                        r = atoi(p->attr[++i]);
                                    }
                                    else if (!strcmp(p->attr[i], "startDegree") || !strcmp(p->attr[i], "startAngle"))
                                    {
                                        sd = atof(p->attr[++i]);
                                    }
                                    else if (!strcmp(p->attr[i], "endDegree") || !strcmp(p->attr[i], "endAngle"))
                                    {
                                        ed = atof(p->attr[++i]);
                                    }
                                    else if (!strcmp(p->attr[i], "strokeWidth") || !strcmp(p->attr[i], "stroke-width"))
                                    {
                                        stroke_width = atoi(p->attr[++i]);
                                    }
                                    else if (!strcmp(p->attr[i], "stroke-linecap") || !strcmp(p->attr[i], "capMode") ||
                                             !strcmp(p->attr[i], "cap"))
                                    {
                                        char *s = p->attr[++i];
                                        if (!strcmp(p->attr[i], "butt"))
                                        {
                                            cap = NVG_BUTT;
                                        }
                                        else if (!strcmp(p->attr[i], "round"))
                                        {
                                            cap = NVG_ROUND;
                                        }
                                        else if (!strcmp(p->attr[i], "square"))
                                        {
                                            cap = NVG_SQUARE;
                                        }
                                        GUI_UNUSED(cap);
                                    }
                                    else if (!strcmp(p->attr[i], "strokeColor") || !strcmp(p->attr[i], "color"))
                                    {
                                        stroke = string_rgb888(p->attr[++i]);
                                    }
                                    else if (!strcmp(p->attr[i], "direction") || !strcmp(p->attr[i], "clockwise"))
                                    {
                                        if (!strcmp(p->attr[++i], "CCW") || !strcmp(p->attr[i], "count clockwise"))
                                        {
                                            dir = NVG_CCW;
                                        }
                                        else if (!strcmp(p->attr[i], "CW") || !strcmp(p->attr[i], "clockwise"))
                                        {
                                            dir = NVG_CW;
                                        }

                                    }
                                    i++;
                                }
                                int image_h = r * 2 + stroke_width * 2,
                                    image_w = image_h,
                                    pixel_bytes = 4,
                                    buffer_size = image_h * image_w * pixel_bytes + sizeof(gui_rgb_data_head_t);
                                param = gui_malloc(sizeof(arc_animation_param_t));
                                memset(param, 0, sizeof(arc_animation_param_t));
                                param->aniamtion_type = ARC_ACTIVITY_EX_ANIMATION;
                                param->cap = cap;
                                param->cx = cx;
                                param->cy = cy;
                                param->dir = dir;
                                param->ed = ed;
                                param->image_data_length = buffer_size;
                                param->r = r;
                                param->sd = sd;
                                param->stroke = stroke;
                                param->stroke_width = stroke_width;
                                param->image_width = image_w;
                                param->image_height = image_h;
                                param->target_buffer  = (void *)gui_img_get_image_data((void *)parent);

                            }
                            gui_img_append_animate((void *)parent, 1000, -1, arc_animate_activity_callback, (void *)param, 0);
                            parent->obj_cb = img_ontime_render;
                        }
                    }
                    else if (!strcmp(type, "heart_rate"))
                    {
                        if (!strcmp(id, "array"))
                        {
                            chart_animation_param_t *param = 0;
                            ezxml_t pt = p->parent;
                            if (!strcmp(p->parent->name, "chart"))
                            {
                                ezxml_t p = pt;
                                size_t i = 0;
                                int16_t x = 0; GUI_UNUSED(x);
                                int16_t y = 0; GUI_UNUSED(y);
                                int16_t w = 0;
                                int16_t h = 0;
                                int16_t item_count = 0; GUI_UNUSED(item_count);
                                const char *items = NULL; GUI_UNUSED(items);
                                gui_color_t  color = {0};
                                int16_t max = 0;
                                int16_t min = 0;
                                const char *style = "waveform";
                                int16_t stroke_width = 4;
                                while (true)
                                {
                                    if (!(p->attr[i]))
                                    {
                                        break;
                                    }

                                    //gui_log("p->attr[i]:%s\n", p->attr[i]);

                                    if (!strcmp(p->attr[i], "x"))
                                    {
                                        x = atoi(p->attr[++i]);
                                    }
                                    else if (!strcmp(p->attr[i], "y"))
                                    {
                                        y = atoi(p->attr[++i]);
                                    }
                                    else if (!strcmp(p->attr[i], "w"))
                                    {
                                        w = atoi(p->attr[++i]);
                                    }
                                    else if (!strcmp(p->attr[i], "h"))
                                    {
                                        h = atoi(p->attr[++i]);
                                    }
                                    else if (!strcmp(p->attr[i], "items"))
                                    {
                                        items = p->attr[++i];
                                    }
                                    else if (!strcmp(p->attr[i], "color"))
                                    {
                                        color = string_rgb888(p->attr[++i]);
                                    }
                                    else if (!strcmp(p->attr[i], "max"))
                                    {
                                        max = atoi(p->attr[++i]);
                                    }
                                    else if (!strcmp(p->attr[i], "min"))
                                    {
                                        min = atoi(p->attr[++i]);
                                    }
                                    else if (!strcmp(p->attr[i], "style"))
                                    {
                                        style = (p->attr[++i]);
                                    }
                                    else if (!strcmp(p->attr[i], "strokeWidth"))
                                    {
                                        stroke_width = atoi(p->attr[++i]);
                                    }
                                    i++;
                                }
                                int image_h = h,
                                    image_w = w,
                                    pixel_bytes = 4,
                                    buffer_size = image_h * image_w * pixel_bytes + sizeof(gui_rgb_data_head_t);
                                param = gui_malloc(sizeof(chart_animation_param_t));
                                memset(param, 0, sizeof(chart_animation_param_t));
                                param->aniamtion_type = CHART_HEART_RATE_DATA_ANIMATION;
                                param->image_data_length = buffer_size;
                                param->image_width = image_w;
                                param->image_height = image_h;
                                param->target_buffer  = (void *)gui_img_get_image_data((void *)parent);
                                param->color = color;
                                param->max = max;
                                param->min = min;
                                param->stroke_width = stroke_width;
                                if (!strcmp(style, "waveform"))
                                {
                                    param->chart_type = 1;
                                }
                                else if (!strcmp(style, "bar"))
                                {
                                    param->chart_type = 2;
                                }
                                else if (!strcmp(style, "line"))
                                {
                                    param->chart_type = 3;
                                }
                                else
                                {
                                    param->chart_type = 1;
                                }

                            }
                            gui_img_append_animate((void *)parent, 1000, -1, chart_animate_heartrate_data_callback,
                                                   (void *)param, 0);
                            parent->obj_cb = img_ontime_canvas_buffer_render;
                        }
                    }
                }
            }
            break;
        case TEXTBOX:
            {
                if (type)
                {
                    if (!strcmp(type, "Weather"))
                    {
                        if (!strcmp(id, "current"))
                        {
                            gui_text_set_animate(parent, 1000, -1, text_animate_weather_callback,
                                                 (void *)TEXT_WEATHER_CUR_ANIMATION);
                        }
                    }
                    else if (!strcmp(type, "heart_rate"))
                    {
                        if (!strcmp(id, "current"))
                        {
                            gui_text_set_animate(parent, 1000, -1, text_animation_hr_callback,
                                                 (void *)TEXT_HEART_RATE_CUR_ANIMATION);
                        }
                    }
                    else if (!strcmp(type, "activity"))
                    {
                        if (!strcmp(id, "exercise"))
                        {
                            gui_text_set_animate(parent, 1000, -1, text_animate_activity_callback,
                                                 (void *)TEXT_ACTIVITY_EX_ANIMATION);
                        }
                    }
                    else if (!strcmp(type, "battery"))
                    {
                        if (!strcmp(id, "capacity"))
                        {
                            gui_text_set_animate(parent, 1000, -1, text_animate_battery_callback,
                                                 (void *)TEXT_BATTERY_CAPACITY_ANIMATION);
                        }
                    }
                    else if (!strcmp(type, "wifi"))
                    {
                        if (!strcmp(id, "name"))
                        {
                            gui_text_set_animate(parent, 1000, -1, text_animate_wifi_callback,
                                                 (void *)TEXT_WIFI_NAME_ANIMATION);
                        }
                        else if (!strcmp(id, "password"))
                        {
                            gui_text_set_animate(parent, 1000, -1, text_animate_wifi_callback,
                                                 (void *)TEXT_WIFI_PASSWORD_ANIMATION);
                        }
                    }
                }
            }
            break;
        case MACRO_PAGE_LIST_NEW:
            {
                if (type)
                {
                    if (!strcmp(type, "wifi"))
                    {
                        if (!strcmp(id, "name"))
                        {
                            gui_win_set_animate((void *)parent->parent, 10000, -1, page_list_animation_wifi_name, parent);
                        }
                    }

                }
            }
            break;
        default:
            break;
        }



    }

    return parent;
}
static gui_obj_t *widget_create_macro_chart(ezxml_t p, gui_obj_t *parent, T_OBJ_TYPE widget_type)
{

    {
        /*  <chart
                w="55"
                h="55"
                x="342"
                y="342"
                color="yellow"
                items="100,200,0,44,50, 1000, -200,20,-30,88"
                max="400"
                min="-300"
                >chart1
            </chart>  */
        size_t i = 0;
        int16_t x = 0;
        int16_t y = 0;
        int16_t w = 0;
        int16_t h = 0;
        int16_t item_count = 0;
        const char *items = "NULL";
        gui_color_t  color = {0};
        int16_t max = 0;
        int16_t min = 0;
        const char *style = "waveform";
        int16_t stroke_width = 4;
        while (true)
        {
            if (!(p->attr[i]))
            {
                break;
            }

            //gui_log("p->attr[i]:%s\n", p->attr[i]);

            if (!strcmp(p->attr[i], "x"))
            {
                x = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "y"))
            {
                y = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "w"))
            {
                w = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "h"))
            {
                h = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "items"))
            {
                items = p->attr[++i];
            }
            else if (!strcmp(p->attr[i], "color"))
            {
                color = string_rgb888(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "max"))
            {
                max = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "min"))
            {
                min = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "style"))
            {
                style = (p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "strokeWidth"))
            {
                stroke_width = atoi(p->attr[++i]);
            }
            i++;
        }
        {
            char *items_copy = gui_strdup(items); // Make a copy of items to use with strtok
            char *token = strtok(items_copy, ",");
            int j = 0;
            while (token != NULL)
            {
                j++;
                token = strtok(NULL, ",");
            }
            item_count = j;
            gui_free(items_copy);
        }
        if (item_count == 0)
        {
            item_count = 1;
            items = "0";
        }
        float numbers[item_count];
        {
            const char delim[2] = ",";
            char *token;
            int i = 0;
            char *str_copy = gui_strdup(items);
            if (str_copy == NULL)
            {
                gui_log("strdup failed");
                return 0;
            }
            token = strtok(str_copy, delim);
            while (token != NULL)
            {
                numbers[i++] = atof(token);
                token = strtok(NULL, delim);
            }
            // for (int j = 0; j < i; j++)
            // {
            //     gui_log("%d ", numbers[j]);
            // }
            // gui_log("\n");
            gui_free(str_copy);
        }
        int
        pixel_bytes = 4;
        size_t buffer_size = h * w * pixel_bytes + sizeof(gui_rgb_data_head_t);
        uint8_t *buffer = gui_lower_malloc(buffer_size);
        memset(buffer, 0, buffer_size);
        NVGcontext *vg = gui_canvas_output_buffer_blank(GUI_CANVAS_OUTPUT_RGBA, 0, w, h, buffer);
        if (!strcmp(style, "waveform"))
        {
            gui_wave_width_render(vg, 0, 0, w,
                                  h,
                                  item_count,
                                  numbers,
                                  color,
                                  max,
                                  min, stroke_width);
        }
        else if (!strcmp(style, "bar"))
        {
            gui_bar_render(vg, 0, 0, w,
                           h,
                           item_count,
                           numbers,
                           color,
                           max,
                           min, stroke_width);
        }
        else if (!strcmp(style, "line"))
        {
            gui_line_render(vg, 0, 0, w,
                            h,
                            item_count,
                            numbers,
                            color,
                            max,
                            min, stroke_width);
        }
        else
        {
            gui_wave_width_render(vg, 0, 0, w,
                                  h,
                                  item_count,
                                  numbers,
                                  color,
                                  max,
                                  min, stroke_width);
        }

        gui_canvas_output_buffer_blank_close(vg);
        char *ptxt = get_space_string_head(p->txt);
        gui_img_t *img = gui_img_create_from_mem(parent, ptxt, buffer, x, y,
                                                 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);

        parent = (void *)img;
        parent->obj_cb = gui_canvas_img_cb;
    }

    return parent;
}

static gui_obj_t *widget_create_image(ezxml_t p, gui_obj_t *parent, T_OBJ_TYPE widget_type)
{

    {
        size_t i = 0;
        int16_t x = 0;
        int16_t y = 0;
        int16_t w = 0; GUI_UNUSED(w);
        int16_t h = 0; GUI_UNUSED(h);
        char *file = NULL;
        char *folder = NULL;
        int count = 0;
        uint32_t duration = 1000;
        float scalex = 1;
        float scaley = 1;
        float angle = 0;
        // default image blend_mode
        BLEND_MODE_TYPE blendMode = IMG_FILTER_BLACK;
        uint8_t opacity = 255;
        while (true)
        {
            if (!(p->attr[i]))
            {
                break;
            }
            //gui_log("p->attr[i]:%s,\n", p->attr[i]);
            if (!strcmp(p->attr[i], "x"))
            {
                x = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "y"))
            {
                y = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "w"))
            {
                w = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "h"))
            {
                h = atoi(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "scaleX"))
            {
                scalex = atof(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "scaleY"))
            {
                scaley = atof(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "rotationAngle"))
            {
                angle = atof(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "blendMode"))
            {
                i++;
                if (!strcmp(p->attr[i], "imgBypassMode"))
                {
                    blendMode = IMG_BYPASS_MODE;
                }
                else if (!strcmp(p->attr[i], "imgFilterBlack"))
                {
                    blendMode = IMG_FILTER_BLACK;
                }
                else if (!strcmp(p->attr[i], "imgSrcOverMode"))
                {
                    blendMode = IMG_SRC_OVER_MODE;
                }
                else if (!strcmp(p->attr[i], "imgCoverMode"))
                {
                    blendMode = IMG_COVER_MODE;
                }
                GUI_UNUSED(blendMode);
            }
            else if (!strcmp(p->attr[i], "opacity"))
            {
                opacity = atof(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "file"))
            {
                char *filetemp = p->attr[++i];
                if (strlen(filetemp) == 0)
                {
                    file = NULL;
                }
                else
                {
                    file = gui_strdup(filetemp);
                }
            }
            else if (!strcmp(p->attr[i], "folder"))
            {
                folder = gui_strdup(p->attr[++i]);
            }
            else if (!strcmp(p->attr[i], "duration"))
            {
                duration = atoi(p->attr[++i]);
            }
            i++;
        }
        char *ptxt = get_space_string_head(p->txt);

        //gui_log("x:%d,y:%d,w:%dh:%d,file:%s\n", x, y, w, h, file);
        if (folder)
        {
            int file_count = 0;
            {
                gui_fs_dir *dir = NULL;
                struct gui_fs_dirent *entry;
                char *path = gui_malloc(strlen(folder) + strlen(GUI_ROOT_FOLDER) + 1);
                sprintf(path, "%s%s", GUI_ROOT_FOLDER, folder);
                if ((dir = gui_fs_opendir(path)) == NULL)
                {
                    gui_free(path);
                    return 0;
                }
                gui_free(path);
                while ((entry = gui_fs_readdir(dir)) != NULL)
                {
                    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                    {
                        file_count++;
                    }
                }
                gui_fs_closedir(dir);
            }

            void **image_array = gui_malloc(file_count * sizeof(void *));
            {
                gui_fs_dir *dir = NULL;
                struct gui_fs_dirent *entry;
                char *path = gui_malloc(strlen(folder) + strlen(GUI_ROOT_FOLDER) + 1);
                sprintf(path, "%s%s", GUI_ROOT_FOLDER, folder);
                if ((dir = gui_fs_opendir(path)) == NULL)
                {
                    gui_free(path);
                    return 0;
                }


                while ((entry = gui_fs_readdir(dir)) != NULL)
                {
                    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                    {
                        char *path2 = gui_malloc(strlen(entry->d_name) + strlen(folder) + 2);
                        sprintf(path2, "%s/%s", folder, entry->d_name);
                        image_array[count++] = (void *)gui_get_image_file_address(path2);
                    }
                }
                gui_free(path);
                gui_fs_closedir(dir);
            }
            parent = (void *)xml_gui_img_create_from_mem(parent, ptxt, image_array[0], x, y);

            image_animate_params_t *params = gui_malloc(sizeof(image_animate_params_t));
            params->image_arr = image_array;
            params->image_count = count;
            params->img = (gui_img_t *)parent;
            params->img_name = ptxt;

            gui_img_set_animate((gui_img_t *)parent, duration, -1, image_animate_callback, params);
            gui_img_set_mode((gui_img_t *)parent, blendMode);
            gui_img_set_opacity((gui_img_t *)parent, opacity);
            if (angle != 0)
            {
                gui_img_get_height((void *)parent);
                gui_img_rotation((void *)parent, angle, parent->w / 2, parent->h / 2);
            }
            parent->obj_cb = img_render;
        }
        else if (file)
        {
            void *imgbuf = (void *)gui_get_image_file_address(file);
            gui_free(file);
            parent = (void *)xml_gui_img_create_from_mem(parent, ptxt, imgbuf, x, y);
            if (scalex != 1 || scaley != 1)
            {
                gui_img_scale((void *)parent, scalex, scaley);
            }
            if (angle != 0)
            {
                gui_img_get_height((void *)parent);
                gui_img_rotation((void *)parent, angle, parent->w / 2, parent->h / 2);
            }
            gui_img_set_mode((gui_img_t *)parent, blendMode);
            gui_img_set_opacity((gui_img_t *)parent, opacity);
            parent->obj_cb = img_render;
        }
    }


    return parent;
}

typedef struct xml_widget_create
{
    T_OBJ_TYPE widget_type;
    gui_obj_t *(* widget_create)(ezxml_t p, gui_obj_t *parent, T_OBJ_TYPE widget_type);
} xml_widget_create_t;
static xml_widget_create_t xml_widget_create_array[] =
{
    {TEXTBOX, widget_create_textbox},
    {IMAGE, widget_create_image},
    {WINDOW, widget_create_window},
    {PROGRESSBAR, widget_create_progressbar},
    {SEEKBAR, widget_create_seekbar},
    {TABVIEW, widget_create_tabview},
    {TAB, widget_create_tab},
    {CURTAINVIEW, widget_create_curtainview},
    {CURTAIN, widget_create_curtain},
    {ICON, widget_create_icon},
    {JAVASCRIPT, widget_create_javascript},
    {CLICKSWITCH, widget_create_clickswitch},
    {PAGE, widget_create_page},
    {SCREEN, widget_create_screen},
    {GRID, widget_create_grid},
    {GALLERY, widget_create_gallery},
    {RADIO, widget_create_radio},
    {MACRO_CANVAS_ARC, widget_create_macro_canvas_arc},
    {MACRO_MOTORIZED_CURTAIN, widget_create_macro_motorized_curtain},
    {KEYBOARD, widget_create_keyboard},
    {MULTI_LEVEL, widget_create_multi_level},
    {MACRO_ONCLICK, widget_create_macro_onclick},
    {MACRO_BACKICON, widget_create_macro_backicon},
    {SLIDER, widget_create_slider},
    {MACRO_ONCHANGE, widget_create_macro_onchange},
    {MACRO_ONON, widget_create_macro_onon},
    {MACRO_ONOFF, widget_create_macro_onoff},
    {MACRO_ONLOAD, widget_create_macro_onload},
    {TYPE_SCROLL_WHEEL_NEW, widget_create_type_scroll_wheel_new},
    {MACRO_CALENDAR, widget_create_macro_calendar},
    {MACRO_ONTIME, widget_create_macro_ontime},
    {MACRO_COMBO, widget_create_macro_combo},
    {MACRO_ON_PERIPHERAL, widget_create_macro_on_peripheral},
    {MACRO_CHART, widget_create_macro_chart},
    {MACRO_PAGE_LIST_NEW, widget_create_macro_page_list_new},
    {MACRO_ONCOMPLETE, widget_create_macro_oncomplete},
    {MACRO_KEY, widget_create_macro_key},
    {MACRO_ONSELECT, widget_create_macro_on_select},
    {RECTANGLE, widget_create_rectangle},
    {MACRO_WIFI, widget_create_macro_wifi},
    {MACRO_CONNECTED, widget_create_macro_on_connected},
    {MACRO_DISCONNECTED, widget_create_macro_on_disconnected},
    {MACRO_CONNECTING, widget_create_macro_on_connecting},
};

static gui_obj_t *widget_create_interface(ezxml_t p, gui_obj_t *parent, T_OBJ_TYPE widget_type)
{
    int array_size = sizeof(xml_widget_create_array) / sizeof(xml_widget_create_array[0]);
    for (int i = 0; i < array_size; i++)
    {
        if (xml_widget_create_array[i].widget_type == widget_type)
        {
            // Call the creation function
            parent = xml_widget_create_array[i].widget_create(p, parent, widget_type);
            return parent;
        }
    }
    printf("Unknown widget type%d\n", widget_type);
    return parent;
}
gui_obj_t *widget_create_handle(ezxml_t p, gui_obj_t *parent)
{
    char *name = p->name;
    //gui_log("widget_create_handle:%s: %s", name, p->txt);
    for (size_t i = 0; i < sizeof(widget) / sizeof(widget[0]); i++)
    {
        if (!strcmp(widget[i].name, name))
        {
            parent = widget_create_interface(p, parent, widget[i].type);
        }
    }
    return parent;


}
// Define various formats
#define MON_FORMAT     0         // "MON"
#define Mon_FORMAT     1         // "Mon"
#define Mo_FORMAT      2         // "Mo"
#define FULL_DATE      3         // "Mon, January 1"
#define SHORT_DATE     4         // "Mon 1"
#define TIME_24HR      5         // "00:00"
#define PARTIAL_DATE   6         // "Mon, Jan 1"
#define NUM_DATE       7         // "1/JANUARY"
#define FULL_DAY       8         // "MONDAY"
#define JUST_HOUR       9         // "hour"
#define JUST_MINUTE       10      // "minute"
#define JUST_SECOND      11       // "second"
#define DEFAULT_FORMAT 100       // Default

static int is_format(const char *src)
{
    if (!src) { return DEFAULT_FORMAT; }
    if (strcmp(src, "MON") == 0) { return MON_FORMAT; }
    if (strcmp(src, "Mon") == 0) { return Mon_FORMAT; }
    if (strcmp(src, "Mo") == 0) { return Mo_FORMAT; }
    if (strcmp(src, "Mon, January 1") == 0) { return FULL_DATE; }
    if (strcmp(src, "Mon 1") == 0) { return SHORT_DATE; }
    if (strcmp(src, "00:00") == 0) { return TIME_24HR; }
    if (strcmp(src, "Mon, Jan 1") == 0) { return PARTIAL_DATE; }
    if (strcmp(src, "1/JANUARY") == 0) { return NUM_DATE; }
    if (strcmp(src, "MONDAY") == 0) { return FULL_DAY; }
    if (strcmp(src, "hour") == 0) { return JUST_HOUR; }
    if (strcmp(src, "minute") == 0) { return JUST_MINUTE; }
    if (strcmp(src, "second") == 0) { return JUST_SECOND; }
    return DEFAULT_FORMAT;
}

static size_t my_strftime(char *__restrict__ buf, size_t maxsize, const char *__restrict__ format,
                          const struct tm *__restrict__ tm)
{
    size_t pos = 0;

    const char *days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    const char *full_days[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

    const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    const char *full_months[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

    while (*format && pos < maxsize)
    {
        if (*format == '%')
        {
            format++;
            if (!*format) { break; }

            switch (*format)
            {
            case 'a': // Abbreviated weekday name
                pos += snprintf(buf + pos, maxsize - pos, "%s", days[tm->tm_wday]);
                break;
            case 'A': // Full weekday name
                pos += snprintf(buf + pos, maxsize - pos, "%s", full_days[tm->tm_wday]);
                break;
            case 'b': // Abbreviated month name
                pos += snprintf(buf + pos, maxsize - pos, "%s", months[tm->tm_mon]);
                break;
            case 'B': // Full month name
                pos += snprintf(buf + pos, maxsize - pos, "%s", full_months[tm->tm_mon]);
                break;
            case 'Y': // Year
                pos += snprintf(buf + pos, maxsize - pos, "%d", 1900 + tm->tm_year);
                break;
            case 'm': // Month (01-12)
                pos += snprintf(buf + pos, maxsize - pos, "%02d", tm->tm_mon + 1);
                break;
            case 'd': // Day (01-31)
                pos += snprintf(buf + pos, maxsize - pos, "%02d", tm->tm_mday);
                break;
            case 'H': // Hour (00-23)
                pos += snprintf(buf + pos, maxsize - pos, "%02d", tm->tm_hour);
                break;
            case 'M': // Minute (00-59)
                pos += snprintf(buf + pos, maxsize - pos, "%02d", tm->tm_min);
                break;
            case 'S': // Second (00-59)
                pos += snprintf(buf + pos, maxsize - pos, "%02d", tm->tm_sec);
                break;
            default: // Unknown specifier
                if (pos < maxsize) { buf[pos++] = '%'; }
                if (pos < maxsize) { buf[pos++] = *format; }
                break;
            }
        }
        else
        {
            buf[pos++] = *format;
        }
        format++;
    }

    if (pos >= maxsize)
    {
        if (maxsize > 0) { buf[maxsize - 1] = '\0'; }
        return 0;
    }

    buf[pos] = '\0';
    return pos;
}

static void text_animate_watchface_callback(void *p, void *this_widget, gui_animate_t *animate)
{
    const char *input_type = p;
    gui_text_t *date_txt = this_widget;
    if (date_txt)
    {
        size_t buffer_size = 100;
        char *buffer = date_txt->content;
        gui_free(buffer);
        buffer = (char *)gui_malloc(buffer_size);
        if (!buffer)
        {
            return;
        }

        int type = is_format(input_type);
        time_t rawtime; GUI_UNUSED(rawtime);
        struct tm *timeinfo; GUI_UNUSED(timeinfo);

#if __WIN32
        time(&rawtime);
        timeinfo = localtime(&rawtime);
#else
        // extern struct tm watch_clock_get(void);
        // struct tm watch_time = watch_clock_get();
        // timeinfo = &watch_time;
        // gui_log("mday: %d, month: %d\n", timeinfo->tm_mday, timeinfo->tm_mon);
#endif
        // Choose the formatting string based on the type
        switch (type)
        {
        case MON_FORMAT:
            my_strftime(buffer, buffer_size, "%a", timeinfo); // Display short week name (Mon)
            for (char *p = buffer; *p; ++p) { *p = toupper(*p); } // Convert to uppercase (MON)
            break;
        case Mon_FORMAT:
            my_strftime(buffer, buffer_size, "%a", timeinfo); // Display short week name (Mon)
            break;
        case Mo_FORMAT:
            my_strftime(buffer, buffer_size, "%a", timeinfo); // Display short week name (Mon)
            buffer[2] = '\0'; // Only display the first two letters (Mo)
            break;
        case FULL_DATE:
            my_strftime(buffer, buffer_size, "%a, %B %d", timeinfo); // Display full date (Mon, January 1)
            break;
        case SHORT_DATE:
            my_strftime(buffer, buffer_size, "%a %d", timeinfo); // Display short date (Mon 1)
            break;
        case TIME_24HR:
            my_strftime(buffer, buffer_size, "%H:%M", timeinfo); // Display 24-hour format time (00:00)
            break;
        case PARTIAL_DATE:
            my_strftime(buffer, buffer_size, "%a, %b %d", timeinfo); // Display partial date (Mon, Jan 1)
            break;
        case NUM_DATE:
            my_strftime(buffer, buffer_size, "%d/%B", timeinfo); // Display numeric date (1/JANUARY)
            // Convert the month part to uppercase
            for (char *p = strchr(buffer, '/') + 1; *p; ++p) { *p = toupper(*p); }
            break;
        case FULL_DAY:
            my_strftime(buffer, buffer_size, "%A", timeinfo); // Display full week name (Monday)
            for (char *p = buffer; *p; ++p) { *p = toupper(*p); } // Convert to uppercase (MONDAY)
            break;
        case JUST_HOUR:
            my_strftime(buffer, buffer_size, "%H", timeinfo); // Display 24-hour format time (00:00)
            break;
        case JUST_MINUTE:
            my_strftime(buffer, buffer_size, "%M", timeinfo); // Display 24-hour format time (00:00)
            break;
        case JUST_SECOND:
            my_strftime(buffer, buffer_size, "%S", timeinfo); // Display 24-hour format time (00:00)
            break;
        default:
            my_strftime(buffer, buffer_size, "%a, %b %d", timeinfo); // Default: (Mon, Jan 1)
            break;
        }
// #endif

        date_txt->content = buffer;
        gui_text_content_set(date_txt, buffer, strlen(buffer));
    }
}

#define GET_TIME \
    uint16_t seconds = 0;\
    uint16_t minute = 0;\
    uint16_t hour = 0;\
    int millisecond = 0;\
    get_time(&seconds, &minute, &hour, &millisecond);

static void get_time(uint16_t *seconds, uint16_t *minute, uint16_t *hour, int *millisecond)
{
#ifndef __WIN32
    // extern struct tm watch_clock_get(void);
    // extern uint16_t millisecond_cnt;
    // struct tm watch_time = watch_clock_get();
    // *millisecond = millisecond_cnt;
    // *seconds = watch_time.tm_sec;
    // *minute = watch_time.tm_min;
    // *hour = watch_time.tm_hour;
    *millisecond = 0;
    *seconds = 0;
    *minute = 0;
    *hour = 0;
#else
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    *seconds = timeinfo->tm_sec;
    *minute = timeinfo->tm_min;
    *hour = timeinfo->tm_hour;
    *millisecond = 0;
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    *millisecond = spec.tv_nsec / 1000000;
#endif
}
static void img_animate_watchface_callback(void *p, void *this_widget, gui_animate_t *animate)
{
    GET_TIME
    const float angle_per_second = 360.0f / 60.0f;
    gui_img_t *hand = this_widget;
    switch ((size_t)p)
    {
    case HOUR_ANIMATION:
        {
            float angle_hour = (hour % 12) * M_PI / 6 + minute * M_PI / 360;
            gui_img_translate(hand, hand->base.w / 2, gui_get_screen_height() / 2 - hand->base.y);
            gui_img_rotation(hand, angle_hour * 180 / M_PI,  hand->base.w / 2,
                             gui_get_screen_height() / 2 - hand->base.y);
        }
        break;
    case MINUTE_ANIMATION:
        {
            float angle_min  = minute * M_PI / 30 + seconds * M_PI / 1800;
            gui_img_translate(hand, hand->base.w / 2, gui_get_screen_height() / 2 - hand->base.y);
            gui_img_rotation(hand, angle_min * 180 / M_PI, hand->base.w / 2,
                             gui_get_screen_height() / 2 - hand->base.y);
        }
        break;
    case SECOND_ANIMATION:
        {
            float angle_sec = (seconds + millisecond / 1000.0f) * angle_per_second;
            gui_img_translate(hand, hand->base.w / 2, gui_get_screen_height() / 2 - hand->base.y);
            gui_img_rotation(hand, angle_sec,  hand->base.w / 2,
                             gui_get_screen_height() / 2 - hand->base.y);
        }
        break;
    default:
        break;
    }
}
static void img_animate_watchface_callback_hour(void *p, void *this_widget, gui_animate_t *animate)
{
    GET_TIME

    gui_img_t *hand = this_widget;
    {
        two_integers *cxcy = p;
        float angle_hour = (hour % 12) * M_PI / 6 + minute * M_PI / 360;
        gui_img_translate(hand, cxcy->num1, cxcy->num2);
        gui_img_rotation(hand, angle_hour * 180 / M_PI,  cxcy->num1, cxcy->num2);
    }
}
static void img_animate_watchface_callback_minute(void *p, void *this_widget,
                                                  gui_animate_t *animate)
{
    GET_TIME

    gui_img_t *hand = this_widget;
    {
        two_integers *cxcy = p;
        float angle_min  = minute * M_PI / 30 + seconds * M_PI / 1800;
        gui_img_translate(hand, cxcy->num1, cxcy->num2);
        gui_img_rotation(hand, angle_min * 180 / M_PI, cxcy->num1, cxcy->num2);
    }
}
static void img_animate_watchface_callback_second(void *p, void *this_widget,
                                                  gui_animate_t *animate)
{
    GET_TIME
    const float angle_per_second = 360.0f / 60.0f;
    gui_img_t *hand = this_widget;
    {
        two_integers *cxcy = p;
        float angle_sec = (seconds + millisecond / 1000.0f) * angle_per_second;
        gui_img_translate(hand, cxcy->num1, cxcy->num2);
        gui_img_rotation(hand, angle_sec,  cxcy->num1, cxcy->num2);
    }
}
gui_obj_t *animate_create_handle(ezxml_t p, gui_obj_t *parent, const char *aniamte_name)
{
    if (!parent)
    {
        return 0;
    }
    char *name = p->name;

    for (size_t i = 0; i < sizeof(widget) / sizeof(widget[0]); i++)
    {
        if (!strcmp(widget[i].name, name))
        {
            switch (widget[i].type)
            {
            case MACRO_ANIMATETRANSFORM:
                {
                    char *ptxt = get_space_string_head(p->txt);
                    if (strcmp(aniamte_name, ptxt))
                    {
                        gui_free(ptxt);
                        continue;
                    }
                    gui_free(ptxt);
                    //gui_log("animate_create_handle:%s: %s", name, p->txt);
                    char *type = 0;
                    char *from = 0;
                    char *to = 0;
                    char *dur = 0;
                    char *repeatCount = 0;
                    char *pause = 0; GUI_UNUSED(pause);
                    float scale_x = 1; GUI_UNUSED(scale_x);
                    float scale_y = 1; GUI_UNUSED(scale_y);
                    uint8_t opacity = 255; GUI_UNUSED(opacity);
                    size_t i = 0;
                    char *values = 0;
                    char *key_times = 0;
                    uint8_t calc_mode = ANIMATION_CALC_MODE_LINEAR;
                    while (true)
                    {
                        //gui_log("p->attr[i]:%x\n",p->attr[i]);
                        if (!(p->attr[i]))
                        {
                            break;
                        }
                        //gui_log("p->attr[i]:%s,\n", p->attr[i]);
                        if (!strcmp(p->attr[i], "type"))
                        {
                            type = (p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "from"))
                        {
                            from = (p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "to"))
                        {
                            to = (p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "dur"))
                        {
                            dur = (p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "repeatCount"))
                        {
                            repeatCount = (p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "pause"))
                        {
                            pause = (p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "values"))
                        {
                            values = (p->attr[++i]);
                        }
                        else if (!strcmp(p->attr[i], "keyTimes"))
                        {
                            key_times = (p->attr[++i]);
                        }

                        else if (!strcmp(p->attr[i], "calcMode"))
                        {
                            ++i;
                            if (!strcmp(p->attr[i], "discrete"))
                            {
                                calc_mode = ANIMATION_CALC_MODE_DISCRETE;
                            }
                            else if (!strcmp(p->attr[i], "linear"))
                            {
                                calc_mode = ANIMATION_CALC_MODE_LINEAR;
                            }
                            else if (!strcmp(p->attr[i], "paced"))
                            {
                                calc_mode = ANIMATION_CALC_MODE_PACED;
                            }
                            else if (!strcmp(p->attr[i], "spline"))
                            {
                                calc_mode = ANIMATION_CALC_MODE_SPLINE;
                            }


                        }
                        i++;
                    }

                    if (type && from && to && dur && repeatCount)
                    {
                        float from_num_f[4];
                        float to_num_f[4];
                        memset(from_num_f, 0, sizeof(from_num_f));
                        memset(to_num_f, 0, sizeof(to_num_f));
                        void **image_array = 0;
                        int count = 0;
                        if (!strcmp(type, "rotate") || !strcmp(type, "scale") || !strcmp(type, "opacity") ||
                            !strcmp(type, "translate") || !strcmp(type, "textContent") || !strcmp(type, "progress"))
                        {
                            int dur_num = 0; GUI_UNUSED(dur_num);
                            int repeat_num = 0; GUI_UNUSED(repeat_num);
                            {
                                //from
                                int from_length = strlen(from);
                                int index[4 + 1];
                                index[0] = 0;
                                int idcount = 1;
                                for (size_t i = 0; i < from_length + 1; i++)
                                {
                                    if (from[i] == ' ' || from[i] == '\0')
                                    {
                                        index[idcount] = i;
                                        int num_length = index[idcount] - index[idcount - 1] + 1;
                                        char num_char[num_length + 1];
                                        num_char[num_length] = '\0';
                                        memcpy(num_char, from + index[idcount - 1], num_length);
                                        from_num_f[idcount - 1] = (float)atof(num_char);
                                        idcount++;
                                    }
                                }
                                //gui_log("p->attr[i]:%x\n", (size_t)(p->attr[i]));
                            }
                            {
                                //to
                                int from_length = strlen(to);
                                int index[5];
                                index[0] = 0;

                                int idcount = 1;
                                for (size_t i = 0; i < from_length + 1; i++)
                                {
                                    if (to[i] == ' ' || to[i] == '\0')
                                    {
                                        index[idcount] = i;
                                        int num_length = index[idcount] - index[idcount - 1] + 1;
                                        char num_char[num_length + 1];
                                        num_char[num_length] = '\0';
                                        memcpy(num_char, to + index[idcount - 1], num_length);
                                        to_num_f[idcount - 1] = (float)atof(num_char);
                                        idcount++;
                                    }
                                }
                                //gui_log("p->attr[i]:%x\n", (size_t)(p->attr[i]));
                            }
                        }
                        if (!strcmp(type, "image frame") && from)
                        {
                            int file_count = 0;
                            {
                                gui_fs_dir *dir = NULL;
                                struct gui_fs_dirent *entry;
                                char *path = gui_malloc(strlen(from) + strlen(GUI_ROOT_FOLDER) + 1);
                                sprintf(path, "%s%s", GUI_ROOT_FOLDER, from);
                                if ((dir = gui_fs_opendir(path)) == NULL)
                                {
                                    gui_free(path);
                                    return 0;
                                }
                                gui_free(path);
                                while ((entry = gui_fs_readdir(dir)) != NULL)
                                {
                                    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                                    {
                                        file_count++;
                                    }
                                }
                                gui_fs_closedir(dir);
                            }

                            image_array = gui_malloc(file_count * sizeof(void *));
                            {
                                gui_fs_dir *dir = NULL;
                                struct gui_fs_dirent *entry;
                                char *path = gui_malloc(strlen(from) + strlen(GUI_ROOT_FOLDER) + 1);
                                sprintf(path, "%s%s", GUI_ROOT_FOLDER, from);
                                if ((dir = gui_fs_opendir(path)) == NULL)
                                {
                                    gui_free(path);
                                    return 0;
                                }


                                while ((entry = gui_fs_readdir(dir)) != NULL)
                                {
                                    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                                    {
                                        char *path2 = gui_malloc(strlen(entry->d_name) + strlen(from) + 2);
                                        sprintf(path2, "%s/%s", from, entry->d_name);
                                        image_array[count++] = (void *)gui_get_image_file_address(path2);
                                    }
                                }
                                gui_free(path);
                                gui_fs_closedir(dir);
                            }
                        }


                        int dur_num = 0;

                        int repeat_num = 0;


                        {
                            //dur
                            int from_length = strlen(dur);
                            int ms = 0;
                            for (size_t i = 0; i < from_length + 1; i++)
                            {
                                if (dur[i] == 'm')
                                {
                                    ms = 1;
                                    break;
                                }

                            }
                            dur_num = atoi(dur);
                            if (!ms)
                            {
                                dur_num = dur_num * 1000;
                            }

                            //gui_log("p->attr[i]:%x\n", (size_t)(p->attr[i]));
                        }
                        {
                            //repeatCount
                            int repeatCount_num = 0;
                            if (!strcmp(repeatCount, "indefinite"))
                            {
                                repeatCount_num = -1;
                            }
                            else
                            {
                                repeatCount_num = atoi(repeatCount);
                            }
                            repeat_num = repeatCount_num;

                            //gui_log("p->attr[i]:%x\n", (size_t)(p->attr[i]));
                        }


                        image_animate_params_t *params = gui_malloc(sizeof(image_animate_params_t));
                        params->img = (gui_img_t *)parent;
                        params->animate_type = gui_strdup(type);

                        params->x = (int)to_num_f[1];
                        params->y = (int)to_num_f[2];

                        params->scale_x = to_num_f[0];
                        params->scale_y = to_num_f[1];
                        params->opacity = (int)to_num_f[0];

                        params->scale_x_from = from_num_f[0];
                        params->scale_y_from = from_num_f[1];
                        params->scale_x_center = from_num_f[2];
                        params->scale_y_center = from_num_f[3];
                        params->opacity_from = (int)from_num_f[0];
                        params->rotate_from = (int)from_num_f[0];
                        params->rotate_to = (int)to_num_f[0];
                        params->rotate_x = (int)from_num_f[1];
                        params->rotate_y = (int)from_num_f[2];
                        params->translate_from_x = from_num_f[0];
                        params->translate_from_y = from_num_f[1];
                        params->translate_to_x = to_num_f[0];
                        params->translate_to_y = to_num_f[1];
                        params->image_arr = image_array;
                        params->image_count = count;
                        params->textContent_from = from_num_f[0];
                        params->textContent_to = to_num_f[0];
                        if (!strcmp(type, "textContent"))
                        {
                            if (parent->type == TEXTBOX)
                            {
                                gui_text_set_animate((gui_text_t *)parent, dur_num, repeat_num, multi_animate_callback, params);
                            }
                        }
                        else if (!strcmp(type, "progress"))
                        {
                            if (parent->type == SEEKBAR)
                            {
                                GUI_API(gui_seekbar_t).animate((void *)parent, dur_num, repeat_num, multi_animate_callback, params);
                            }
                        }
                        else
                        {
                            if (parent->type == IMAGE_FROM_MEM)
                            {
                                gui_img_append_animate((gui_img_t *)parent, dur_num, repeat_num, multi_animate_callback, params,
                                                       (aniamte_name));
                            }

                        }


                    }
                    else if (type && key_times && values && dur && repeatCount)
                    {
                        float values_num_f[10];
                        float values2_num_f[10];
                        float key_times_num_f[10];
                        memset(values_num_f, 0, sizeof(values_num_f));
                        memset(values2_num_f, 0, sizeof(values2_num_f));
                        memset(key_times_num_f, 0, sizeof(key_times_num_f));
                        void **image_array = 0;
                        int count = 0;
                        int values_length = 0;
                        if (!strcmp(type, "rotate") || !strcmp(type, "scale") || !strcmp(type, "opacity"))
                        {
                            int dur_num = 0; GUI_UNUSED(dur_num);
                            int repeat_num = 0; GUI_UNUSED(repeat_num);
                            {
                                //values
                                int from_length = strlen(values);
                                int index[10 + 1];
                                index[0] = -1;
                                int idcount = 1;
                                for (size_t i = 0; i < from_length + 1; i++)
                                {
                                    if (values[i] == ';' || values[i] == '\0')
                                    {
                                        index[idcount] = i;
                                        int num_length = index[idcount] - index[idcount - 1] ;
                                        char num_char[num_length + 1];
                                        num_char[num_length] = '\0';
                                        memcpy(num_char, values + index[idcount - 1] + 1, num_length);
                                        values_num_f[idcount - 1] = (float)atof(num_char);
                                        idcount++;
                                    }
                                }

                                values_length = idcount - 1; // Adjust for the fact that we count one extra for '\0'
                                //gui_log("p->attr[i]:%x\n", (size_t)(p->attr[i]));
                            }
                            {
                                //key_times
                                int from_length = strlen(key_times);
                                int index[10 + 1];
                                index[0] = -1;

                                int idcount = 1;
                                for (size_t i = 0; i < from_length + 1; i++)
                                {
                                    if (key_times[i] == ';' || key_times[i] == '\0')
                                    {
                                        index[idcount] = i;
                                        int num_length = index[idcount] - index[idcount - 1] ;
                                        char num_char[num_length + 1];
                                        num_char[num_length] = '\0';
                                        memcpy(num_char, key_times + index[idcount - 1] + 1, num_length);
                                        key_times_num_f[idcount - 1] = (float)atof(num_char);
                                        idcount++;
                                    }
                                }
                                //gui_log("p->attr[i]:%x\n", (size_t)(p->attr[i]));
                            }
                        }
                        else if (
                            !strcmp(type, "translate"))
                        {
                            int dur_num = 0; GUI_UNUSED(dur_num);
                            int repeat_num = 0; GUI_UNUSED(repeat_num);
                            {
                                //values
                                int from_length = strlen(values);
                                int index[10 + 1];
                                index[0] = -1;
                                int idcount = 1;

// Count the number of segments
                                for (size_t i = 0; i < from_length + 1; i++)
                                {
                                    if (values[i] == ';' || values[i] == '\0')
                                    {
                                        index[idcount] = i;
                                        idcount++;
                                    }
                                }
                                values_length = idcount - 1; // Adjust for the fact that we count one extra for '\0'


// Parse each segment
                                for (int i = 0; i < values_length; i++)
                                {
                                    char segment[20]; // Assuming no segment will be longer than 19 characters
                                    int segment_length;

                                    if (i == values_length - 1) // Last segment
                                    {
                                        segment_length = from_length - index[i] - 1;
                                    }
                                    else
                                    {
                                        segment_length = index[i + 1] - index[i] - 1;
                                    }

                                    // Copy segment to a temporary buffer
                                    strncpy(segment, values + index[i] + 1, segment_length);
                                    segment[segment_length] = '\0';

                                    // Parse x and y from the segment
                                    char *token = strtok(segment, ",");
                                    if (token != NULL)
                                    {
                                        values_num_f[i] = atof(token); // Store x value
                                        token = strtok(NULL, ",");
                                        if (token != NULL)
                                        {
                                            values2_num_f[i] = atof(token);    // Store y value
                                        }
                                    }
                                }

// Debug print to verify parsing
                                for (int i = 0; i < values_length; i++)
                                {
                                    printf("Point %d: (%f, %f)\n", i, values_num_f[i], values2_num_f[i]);
                                }
                                //gui_log("p->attr[i]:%x\n", (size_t)(p->attr[i]));
                            }
                            {
                                //key_times
                                int from_length = strlen(key_times);
                                int index[10 + 1];
                                index[0] = -1;

                                int idcount = 1;
                                for (size_t i = 0; i < from_length + 1; i++)
                                {
                                    if (key_times[i] == ';' || key_times[i] == '\0')
                                    {
                                        index[idcount] = i;
                                        int num_length = index[idcount] - index[idcount - 1] ;
                                        char num_char[num_length + 1];
                                        num_char[num_length] = '\0';
                                        memcpy(num_char, key_times + index[idcount - 1] + 1, num_length);
                                        key_times_num_f[idcount - 1] = (float)atof(num_char);
                                        idcount++;
                                    }
                                }
                                //gui_log("p->attr[i]:%x\n", (size_t)(p->attr[i]));
                            }
                        }
                        if (!strcmp(type, "image frame") && values)
                        {
                            int file_count = 0;
                            {
                                gui_fs_dir *dir = NULL;
                                struct gui_fs_dirent *entry;
                                char *path = gui_malloc(strlen(values) + strlen(GUI_ROOT_FOLDER) + 1);
                                sprintf(path, "%s%s", GUI_ROOT_FOLDER, values);
                                if ((dir = gui_fs_opendir(path)) == NULL)
                                {
                                    gui_free(path);
                                    return 0;
                                }
                                gui_free(path);
                                while ((entry = gui_fs_readdir(dir)) != NULL)
                                {
                                    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                                    {
                                        file_count++;
                                    }
                                }
                                gui_fs_closedir(dir);
                            }

                            image_array = gui_malloc(file_count * sizeof(void *));
                            {
                                gui_fs_dir *dir = NULL;
                                struct gui_fs_dirent *entry;
                                char *path = gui_malloc(strlen(values) + strlen(GUI_ROOT_FOLDER) + 1);
                                sprintf(path, "%s%s", GUI_ROOT_FOLDER, values);
                                if ((dir = gui_fs_opendir(path)) == NULL)
                                {
                                    gui_free(path);
                                    return 0;
                                }


                                while ((entry = gui_fs_readdir(dir)) != NULL)
                                {
                                    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                                    {
                                        char *path2 = gui_malloc(strlen(entry->d_name) + strlen(values) + 2);
                                        sprintf(path2, "%s/%s", values, entry->d_name);
                                        image_array[count++] = (void *)gui_get_image_file_address(path2);
                                    }
                                }
                                gui_free(path);
                                gui_fs_closedir(dir);
                            }
                        }


                        int dur_num = 0;

                        int repeat_num = 0;


                        {
                            //dur
                            int from_length = strlen(dur);
                            int ms = 0;
                            for (size_t i = 0; i < from_length + 1; i++)
                            {
                                if (dur[i] == 'm')
                                {
                                    ms = 1;
                                    break;
                                }

                            }
                            dur_num = atoi(dur);
                            if (!ms)
                            {
                                dur_num = dur_num * 1000;
                            }

                            //gui_log("p->attr[i]:%x\n", (size_t)(p->attr[i]));
                        }
                        {
                            //repeatCount
                            int repeatCount_num = 0;
                            if (!strcmp(repeatCount, "indefinite"))
                            {
                                repeatCount_num = -1;
                            }
                            else
                            {
                                repeatCount_num = atoi(repeatCount);
                            }
                            repeat_num = repeatCount_num;

                            //gui_log("p->attr[i]:%x\n", (size_t)(p->attr[i]));
                        }


                        image_animate_key_times_params_t *params = gui_malloc(sizeof(image_animate_key_times_params_t));
                        params->img = (gui_img_t *)parent;
                        params->animate_type = gui_strdup(type);
                        memcpy(params->key_times, key_times_num_f, sizeof(key_times_num_f));
                        memcpy(params->values, values_num_f, sizeof(values_num_f));
                        memcpy(params->values_2, values2_num_f, sizeof(values2_num_f));
                        params->length = values_length;
                        params->calc_mode = calc_mode;
                        if (parent->type == IMAGE_FROM_MEM)
                        {
                            gui_img_append_animate((gui_img_t *)parent, dur_num, repeat_num, multi_animate_key_times_callback,
                                                   params,
                                                   (aniamte_name));
                        }
                    }
                    return 0;
                }

            default:
                break;
            }
        }
    }
    return parent;
}
void foreach_create(ezxml_t p, gui_obj_t *parent)
{
    ezxml_t i;
    for (i = p; i != NULL; i = i->ordered)
    {
        bool ml = 0;
        if (parent->type == MULTI_LEVEL && strncmp(i->name, "multiLevel", strlen("multiLevel")) == 0)
        {
            char *group_name = 0;
            //gui_log("x:%d,y:%d,w:%dh:%d,orientation:%d\n", x, y, w, h, orientation);
            {
                size_t ii = 0;

                while (true)
                {
                    if (!(i->attr[ii]))
                    {
                        break;
                    }
                    ////gui_log("p->attr[i]:%s\n", p->attr[i]);
                    if (!strcmp(i->attr[ii], "name"))
                    {
                        group_name = (i->attr[++ii]);
                    }
                    // else if (!strcmp(p->attr[i], "switchPause"))
                    // {
                    //     pause_switch_name = gui_strdup(p->attr[++i]);
                    // }
                    ii++;
                }
            }
            if (group_name && strlen(group_name) == 0)
            {
                group_name = 0;
            }
            if (group_name && !(GUI_STRINGS_EQUAL(GUI_TYPE(gui_multi_level_t, parent)->group_name, group_name)))
            {
                //gui_log("%s,%s\n", i->name, i->txt);
                ml = 1;
            }


        }


        if (parent->type == MULTI_LEVEL && (strncmp(i->name, "multiLevel", strlen("multiLevel")) != 0 ||
                                            ml))
        {

            {
                //gui_log("%s,%s\n", i->name, i->txt);
                foreach_create(i->child, parent);
            }


        }
        else
        {
            foreach_create(i->child, widget_create_handle(i, parent));
        }
    }
}
static void foreach_create_animate(ezxml_t p, gui_obj_t *parent, const char *animate_name)
{
    ezxml_t i;
    for (i = p; i != NULL; i = i->ordered)
    {
        {
            foreach_create_animate(i->child, animate_create_handle(i, parent, animate_name), animate_name);
        }
    }
}
void foreach_scan(ezxml_t p, const char *element, ezxml_t *target)
{
    ezxml_t i;
    for (i = p; i != NULL; i = i->ordered)
    {
        if (strcmp(i->name, "screen") == 0)
        {

            *target = i;
            return;
        }
        foreach_scan(i->child, element, target);
    }
}
void foreach_scan_with_content(ezxml_t p, const char *element, ezxml_t *target, const char *content)
{
    ezxml_t i;
    for (i = p; i != NULL; i = i->ordered)
    {
        if (strncmp(i->name, element, strlen(element)) == 0 &&
            strncmp(i->txt, content, strlen(content)) == 0)
        {

            *target = i;
            return;
        }
        foreach_scan_with_content(i->child, element, target, content);
    }
}
static int level, order;
void foreach_create_in_multilevel(ezxml_t p, gui_obj_t *parent, const char *group_name_parent)
{
    GUI_UNUSED(level); GUI_UNUSED(order);
    ezxml_t i;
    for (i = p; i != NULL; i = i->ordered)
    {
        bool ml = 0; bool ml2 = 0;
        if (strncmp(i->name, "multiLevel", strlen("multiLevel")) == 0)
        {
            {
                char *group_name = 0;
                //gui_log("x:%d,y:%d,w:%dh:%d,orientation:%d\n", x, y, w, h, orientation);
                {
                    size_t ii = 0;

                    while (true)
                    {
                        if (!(i->attr[ii]))
                        {
                            break;
                        }
                        ////gui_log("p->attr[i]:%s\n", p->attr[i]);
                        if (!strcmp(i->attr[ii], "name"))
                        {
                            group_name = (i->attr[++ii]);
                        }
                        // else if (!strcmp(p->attr[i], "switchPause"))
                        // {
                        //     pause_switch_name = gui_strdup(p->attr[++i]);
                        // }
                        ii++;
                    }
                }
                if (group_name && strlen(group_name) == 0)
                {
                    group_name = 0;
                }


                if (group_name && !(GUI_STRINGS_EQUAL(group_name, group_name_parent)))
                {
                    //gui_log("%s,%s\n", i->name, i->txt);
                    ml = 1;
                }


            }
        }
        if (parent->type == MULTI_LEVEL &&
            (!(GUI_STRINGS_EQUAL(group_name_parent, GUI_TYPE(gui_multi_level_t, parent)->group_name))))
        {
            ml2 = 1;
        }
        if (ml2)
        {
            if ((strncmp(i->name, "multiLevel", strlen("multiLevel")) != 0))
            {
                foreach_create_in_multilevel(i->child,  parent, group_name_parent);
            }
            else { foreach_create_in_multilevel(i->child, widget_create_handle(i, parent), group_name_parent); }
        }
        else if ((strncmp(i->name, "multiLevel", strlen("multiLevel")) != 0) || ml)
        {
            //gui_log("%s,%s\n", i->name, i->txt);


            foreach_create_in_multilevel(i->child, widget_create_handle(i, parent), group_name_parent);
        }




    }
}
static void foreach_create_for_multilevel(ezxml_t p, gui_obj_t *parent)
{
    ezxml_t i;
    for (i = p; i != NULL; i = i->ordered)
    {
//gui_log("ml%s,%s,%s\n", i->name,i->txt,parent->name);
        if (strncmp(i->name, "multiLevel", strlen("multiLevel")) == 0 &&
            strncmp(i->txt, parent->name, strlen(parent->name)) == 0)
        {
            foreach_create_in_multilevel(i->child, parent, GUI_TYPE(gui_multi_level_t, parent)->group_name);
            return;
        }

        foreach_create_for_multilevel(i->child, parent);
    }
}
void level_scan(ezxml_t p, char **pic, char **text)
{
    ezxml_t i;
    for (i = p; i != NULL; i = i->ordered)
    {

        if (strcmp(i->name, "app") == 0)
        {

            *text = get_space_string_head(i->txt); //gui_log("level:%s\n", *text);
            size_t j = 0;
            while (true)
            {
                if (!(i->attr[j]))
                {
                    break;
                }
                ////gui_log("p->attr[i]:%s\n", p->attr[i]);
                if (!strcmp(i->attr[j], "iconPicture"))
                {
                    *pic = gui_strdup(i->attr[++j]);
                    break;
                }

                j++;
            }
            break;
        }
    }
}
void foreach_count(ezxml_t p, size_t *widget_count)
{
    ezxml_t i;
    for (i = p; i != NULL; i = i->ordered)
    {
        (*widget_count)++;
        foreach_count(i->child, widget_count);
    }
}
//#include "gui_server.h"
/*void app_init(void *p)
{
    gui_app_t *app = p;
    gui_list_init(&(app->screen->child_list));
    gui_list_init(&(app->screen->brother_list));

    ezxml_t f1 = ezxml_parse_file(app->xml);
    js = NULL;
    foreach_create(f1, app->screen);
    ezxml_free(f1);
    gui_return_create(app->screen);
    app->inited = true;
    js_run_file_on_server(js, p);

    gui_exec(app->screen);
    while (1)
    {
        gui_thread_mdelay(1000000);
        //handle_launcher_msg();
    }
}*/

static void load_return_array()
{
    const char *folder = "app/system/resource/return_array";
    {
        gui_fs_dir *dir = 0;
        struct gui_fs_dirent *entry;

        char *path = gui_malloc(strlen(folder) + strlen(GUI_ROOT_FOLDER) + 1);
        sprintf(path, "%s%s", GUI_ROOT_FOLDER, folder);
        if ((dir = gui_fs_opendir(path)) == NULL)
        {
            gui_free(path);
            //perror("gui_fs_opendir() failed");
            return;
        }
        gui_free(path);
        while ((entry = gui_fs_readdir(dir)) != NULL)
        {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
            {
                file_count++;
            }


        }
        gui_fs_closedir(dir);
    }
    image_array = gui_malloc(file_count * sizeof(void *));
    {
        gui_fs_dir *dir = 0;
        struct gui_fs_dirent *entry;
        char *path = gui_malloc(strlen(folder) + strlen(GUI_ROOT_FOLDER) + 1);
        sprintf(path, "%s%s", GUI_ROOT_FOLDER, folder);
        if ((dir = gui_fs_opendir(path)) == NULL)
        {
            gui_free(path);
            //perror("gui_fs_opendir() failed");
            return;
        }

        int count = 0;
        while ((entry = gui_fs_readdir(dir)) != NULL)
        {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
            {
                char *path2 = gui_malloc(strlen(entry->d_name) + strlen(folder) + 2);
                sprintf(path2, "%s/%s", folder, entry->d_name);
                image_array[count++] = (void *)gui_get_image_file_address(path2);
            }

        }
        gui_free(path);
        gui_fs_closedir(dir);


    }

}
static void load_return_image()
{
    return_image = (void *)gui_get_image_file_address("app/system/resource/return.bin");
    return_image_hl = (void *)gui_get_image_file_address("app/system/resource/returnhl.bin");
}
static void system_load()
{
    load_return_array();
    load_return_image();


}
void create_tree(gui_app_t *app)
{
    system_load();
    f1 = ezxml_parse_file(app->xml);
    js = NULL;
    foreach_create(f1, &app->screen);
    ezxml_free(f1);
    f1 = 0;
#ifdef ENABLE_RTK_GUI_SCRIPT_AS_A_APP
    extern void js_run_file(const char *file, gui_app_t  *app);
    extern void js_isReady(void);
    if (js)
    {
        char *path = gui_malloc(strlen(js) + strlen(GUI_ROOT_FOLDER) + 1);

        sprintf(path, "%s%s", GUI_ROOT_FOLDER, js); gui_log("js_path:%s", path);
        js_run_file(path, app);

        gui_free(path);
        js_isReady();
    }
#endif
}
static void create_tree_in_multi_level(gui_app_t *app, gui_multi_level_t *parent)
{
    //gui_obj_tree_print((gui_obj_t *)parent);
    ezxml_t title = 0; GUI_UNUSED(title);
    ezxml_t f = 0;
    if (f1 != 0)
    {
        f = f1;
    }
    else
    {
        f = ezxml_parse_file(app->xml);
    }
    foreach_create_for_multilevel(f, (gui_obj_t *)parent);
    //gui_log(" ");
    if (f1 == 0)
    {
        ezxml_free(f);
    }
    //gui_obj_tree_print((gui_obj_t *)parent);




}
const char *get_child_ele_attr(char *xml, char *ele, char *txt, char *chile_ele, char *attr)
{
    ezxml_t f1 = ezxml_parse_file(xml);
    ezxml_t t;
    foreach_scan_with_content(f1, ele, &t, txt);
    ezxml_t title = ezxml_get(t, chile_ele, -1);
    return ezxml_attr(title, attr);
}
ezxml_t get_child_ele(char *xml, char *ele, char *txt, char *chile_ele)
{
    ezxml_t f1 = ezxml_parse_file(xml);
    ezxml_t t;
    foreach_scan_with_content(f1, ele, &t, txt);
    ezxml_t title = ezxml_get(t, chile_ele, -1);
    return title;
}
const char *get_tag_by_widget_type(int type)
{
    if (type == BUTTON)
    {
        type = ICON;
    }

    for (size_t i = 0; i < sizeof(widget) / sizeof(struct widget_create); i++)
    {
        if (widget[i].type == type)
        {
            return widget[i].name;
        }
    }
    return NULL;
}
void get_app(gui_app_t *app, char *pic, char *text)
{
    ezxml_t f1 = ezxml_parse_file(app->xml);


    level_scan(f1, (void *)pic, (void *)text);
    ezxml_free(f1);
}
void level_scan_width_and_hight(ezxml_t p, int *width, int *hight)
{
    ezxml_t i;
    for (i = p; i != NULL; i = i->ordered)
    {
        //gui_log("%s\n",i->child->next->name);
        if (strcmp(i->name, "app") == 0)
        {
            ezxml_t title = ezxml_get(i, "screen", -1);
//gui_log("%s,%s,%s,%s\n",title->name, title->attr[0], title->attr[1],title->attr[2]);

            size_t j = 0;
            while (true)
            {
                if (!(title->attr[j]))
                {
                    break;
                }
                ////gui_log("p->attr[i]:%s\n", p->attr[i]);
                if (!strcmp(title->attr[j], "w"))
                {
                    *width = atoi(title->attr[++j]); gui_log("widgt:%d\n", *width);
                }
                else if (!strcmp(title->attr[j], "h"))
                {
                    *hight = atoi(title->attr[++j]); gui_log("hight:%d\n", *hight);
                }

                j++;
            }
            break;
        }
    }
}
void get_app_by_file(char *xml, char *pic, char *text)
{
    ezxml_t f1 = ezxml_parse_file(xml);


    level_scan(f1, (void *)pic, (void *)text);
    ezxml_free(f1);
}
void get_screen_size(char *xml, int *widgt, int *hight)
{
    ezxml_t f1 = ezxml_parse_file(xml);


    level_scan_width_and_hight(f1, widgt, hight);
    ezxml_free(f1);
}
void get_system_screen(int *w, int *h)
{
    char *ezxmlpath = "app/system/launcher.xml";
    char *path = gui_malloc(strlen(ezxmlpath) + strlen(GUI_ROOT_FOLDER) + 1);
    sprintf(path, "%s%s", GUI_ROOT_FOLDER, ezxmlpath);
    ezxml_t f1 =
        ezxml_parse_file(path);
    gui_free(path);
    ezxml_t t;
    foreach_scan(f1, "screen", &t);
    size_t i = 0;

    while (true)
    {
        if (!(t->attr[i]))
        {
            break;
        }
        ////gui_log("p->attr[i]:%s\n", p->attr[i]);
        else if (!strcmp(t->attr[i], "w"))
        {
            *w = atoi(t->attr[++i]);
        }
        else if (!strcmp(t->attr[i], "h"))
        {
            *h = atoi(t->attr[++i]);
        }
        i++;
    }
    ezxml_free(f1);
}
/*void album_startup(void *this)
{
    gui_app_startup_base(this);
}
static void album_close(void *this)
{
    gui_app_close_base(this);
}
void album_ctor(void *this)
{
    gui_app_ctor_base(this, "album", app_init, album_startup, album_close);
}*/

char *get_app_name(const char *xml)
{
    ezxml_t f1 = ezxml_parse_file(xml);
    GUI_ASSERT(f1);
    if (!strcmp("app", f1->name))
    {
        char *s = gui_strdup(f1->txt);
        ezxml_free(f1);
        return s;
    }
    return NULL;
}
// Function that recursively searches for the <preview> tag at any level
static void find_preview_tags(ezxml_t node, char **file_output)
{
    if (!node) { return; }
    // If the node's name is "preview", print the file attribute
    if (strcmp(node->name, "preview") == 0)
    {
        const char *file = gui_strdup(ezxml_attr(node, "file"));
        if (file)
        {
            //gui_log("File attribute value: %s\n", file);
            *file_output = (char *)file;
            return;
        }
    }

    // Recurse down to all children
    ezxml_t child = node->child;
    while (child)
    {
        find_preview_tags(child, file_output);
        child = child->sibling;
    }
}
char *gui_dom_get_preview_image_file(const char *xml_file)
{
    ezxml_t xml = ezxml_parse_file(xml_file);
    if (!xml)
    {
        gui_log("Failed to load XML file.\n");
    }
    char *file_output = 0;
    find_preview_tags(xml, &file_output);
    ezxml_free(xml);
    return file_output;
}
void create_tree_nest(const char *xml, void *obj)
{
    GUI_WIDGET_TRY_EXCEPT(obj);
    gui_app_t *app = gui_obj_tree_get_app(obj);
    app->xml = xml;
    ezxml_t f1 = ezxml_parse_file(xml);
    if (f1)
    {
        foreach_create(f1, obj);
    }
    ezxml_free(f1);
}
void gui_dom_create_tree_nest(const char *xml, gui_obj_t *parent_widget)
{
    create_tree_nest(xml, parent_widget);
}
static void setting_return_cb(void *obj, gui_event_t e, void *param)
{
    gui_obj_t *this = (gui_obj_t *)obj;
    if (this->parent && this->parent->type == MULTI_LEVEL)
    {
        if (this->parent->parent && this->parent->parent->type == MULTI_LEVEL)
        {
            uint8_t index = GUI_TYPE(gui_multi_level_t, this->parent->parent)->index;
            uint8_t level = GUI_TYPE(gui_multi_level_t, this->parent->parent)->level;
            GUI_API(gui_multi_level_t).jump((void *)this->parent, level, index);
        }
    }
}
static void pause_animation_cb(gui_obj_t *this, void *null, char *to_name[])
{
    if (to_name[0])
    {
        gui_obj_t *to = 0;
        gui_obj_tree_get_widget_by_name((void *)gui_current_app(), to_name[0], &to);

        GUI_WIDGET_TRY_EXCEPT(to)
        {
            /* pause */
            if (to->type == IMAGE_FROM_MEM)
            {
                if (GUI_TYPE(gui_img_t, to)->animate)
                {
                    //GUI_TYPE(gui_img_t, to)->animate->animate = 0;
                    for (size_t i = 0; i < GUI_TYPE(gui_img_t, to)->animate_array_length; i++)
                    {
                        ((gui_animate_t **)(GUI_TYPE(gui_img_t, to)->animate))[i]->animate = 0;
                    }

                }


            }
        }
    }
}
static void start_animation_cb(gui_obj_t *this, void *null, char *to_name[])
{
    if (to_name[0])
    {
        gui_obj_t *to = 0;
        gui_obj_tree_get_widget_by_name((void *)gui_current_app(), to_name[0], &to);
        GUI_WIDGET_TRY_EXCEPT(to)
        {
            /* pause */
            if (to->type == IMAGE_FROM_MEM)
            {
                gui_img_t *img = GUI_TYPE(gui_img_t, to);
                bool create = 1;
                bool new_trigger = 0;
                for (size_t i = 0; i < img->animate_array_length; i++)
                {
                    if (!(((gui_animate_t **)(img->animate))[i]->trigger_name == to_name[2]))
                    {
                        new_trigger = 1;
                    }
                }
                if (new_trigger)
                {
                    for (size_t i = 0; i < img->animate_array_length; i++)
                    {

                        for (size_t i = 0; i < img->animate_array_length; i++)
                        {
                            image_animate_params_t *params = ((((gui_animate_t **)(img->animate))[i])->p);
                            gui_free(params->animate_type);
                            gui_free((((gui_animate_t **)(img->animate))[i])->p);
                            gui_free(((gui_animate_t **)(img->animate))[i]);
                            ((gui_animate_t **)(img->animate))[i] = NULL;
                        }
                        gui_free(img->animate);
                        img->animate = 0;
                        img->animate_array_length = 0;


                    }
                }

                {
                    for (size_t i = 0; i < img->animate_array_length; i++)
                    {
                        if (GUI_STRINGS_EQUAL(((gui_animate_t **)(img->animate))[i]->name, to_name[1]))
                        {
                            create = 0;
                        }
                    }
                }
                if (create)
                {
                    ezxml_t f = 0;
                    if (f1 != 0)
                    {
                        f = f1;
                    }
                    else
                    {
                        f = ezxml_parse_file(((gui_app_t *)gui_current_app())->xml);
                    }
                    foreach_create_animate(f, to, to_name[1]);
                    for (size_t i = 0; i < img->animate_array_length; i++)
                    {
                        (((gui_animate_t **)(img->animate))[i]->trigger_name = to_name[2]);

                    }
                    //gui_log(" ");
                    if (f1 == 0)
                    {
                        ezxml_free(f);
                    }
                }
                else
                {
                    for (size_t i = 0; i < GUI_TYPE(gui_img_t, to)->animate_array_length; i++)
                    {
                        ((gui_animate_t **)(img->animate))[i]->animate = 1;
                    }
                }
            }
        }
    }
}
static const uint8_t *gui_get_image_file_address(const char *image_file_path)
{
    const uint8_t *addr = gui_get_file_address(image_file_path);
    if (addr == 0)
    {
        static const unsigned char image404[1571] =
        {
            0x10, 0x00, 0x32, 0x00, 0x32, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x32, 0x00, 0x00, 0x00,
            0x32, 0x00, 0x00, 0x00, 0xd8, 0x00, 0x00, 0x00, 0xdb, 0x00, 0x00, 0x00, 0xde, 0x00, 0x00, 0x00,
            0xe1, 0x00, 0x00, 0x00, 0xe4, 0x00, 0x00, 0x00, 0xe7, 0x00, 0x00, 0x00, 0xea, 0x00, 0x00, 0x00,
            0xed, 0x00, 0x00, 0x00, 0xf0, 0x00, 0x00, 0x00, 0xf9, 0x00, 0x00, 0x00, 0x47, 0x01, 0x00, 0x00,
            0x71, 0x01, 0x00, 0x00, 0x9e, 0x01, 0x00, 0x00, 0xd1, 0x01, 0x00, 0x00, 0xec, 0x01, 0x00, 0x00,
            0x07, 0x02, 0x00, 0x00, 0x28, 0x02, 0x00, 0x00, 0x4c, 0x02, 0x00, 0x00, 0x73, 0x02, 0x00, 0x00,
            0x97, 0x02, 0x00, 0x00, 0xc4, 0x02, 0x00, 0x00, 0xf4, 0x02, 0x00, 0x00, 0x21, 0x03, 0x00, 0x00,
            0x51, 0x03, 0x00, 0x00, 0x75, 0x03, 0x00, 0x00, 0x96, 0x03, 0x00, 0x00, 0xc0, 0x03, 0x00, 0x00,
            0xe7, 0x03, 0x00, 0x00, 0x0e, 0x04, 0x00, 0x00, 0x35, 0x04, 0x00, 0x00, 0x5c, 0x04, 0x00, 0x00,
            0x80, 0x04, 0x00, 0x00, 0xa7, 0x04, 0x00, 0x00, 0xce, 0x04, 0x00, 0x00, 0xf2, 0x04, 0x00, 0x00,
            0x13, 0x05, 0x00, 0x00, 0x2e, 0x05, 0x00, 0x00, 0x49, 0x05, 0x00, 0x00, 0x67, 0x05, 0x00, 0x00,
            0x82, 0x05, 0x00, 0x00, 0xb2, 0x05, 0x00, 0x00, 0xfa, 0x05, 0x00, 0x00, 0x03, 0x06, 0x00, 0x00,
            0x06, 0x06, 0x00, 0x00, 0x09, 0x06, 0x00, 0x00, 0x0c, 0x06, 0x00, 0x00, 0x0f, 0x06, 0x00, 0x00,
            0x12, 0x06, 0x00, 0x00, 0x15, 0x06, 0x00, 0x00, 0x18, 0x06, 0x00, 0x00, 0x1b, 0x06, 0x00, 0x00,
            0x32, 0xff, 0xff, 0x32, 0xff, 0xff, 0x32, 0xff, 0xff, 0x32, 0xff, 0xff, 0x32, 0xff, 0xff, 0x32,
            0xff, 0xff, 0x32, 0xff, 0xff, 0x32, 0xff, 0xff, 0x1e, 0xff, 0xff, 0x01, 0xdf, 0xff, 0x13, 0xff,
            0xff, 0x07, 0xff, 0xff, 0x01, 0xbe, 0xf7, 0x01, 0xdb, 0xde, 0x01, 0xba, 0xd6, 0x01, 0x79, 0xce,
            0x01, 0x38, 0xc6, 0x01, 0xf7, 0xbd, 0x01, 0xb6, 0xb5, 0x01, 0x75, 0xad, 0x01, 0x34, 0xa5, 0x01,
            0xf3, 0x9c, 0x01, 0x92, 0x94, 0x01, 0x51, 0x8c, 0x01, 0x6d, 0x6b, 0x01, 0x18, 0xc6, 0x08, 0xff,
            0xff, 0x01, 0xaa, 0x52, 0x02, 0x00, 0x00, 0x01, 0x82, 0x10, 0x01, 0xc7, 0x39, 0x01, 0x2c, 0x63,
            0x01, 0x71, 0x8c, 0x01, 0xf7, 0xbd, 0x01, 0x1c, 0xe7, 0x01, 0xdf, 0xff, 0x0b, 0xff, 0xff, 0x05,
            0xff, 0xff, 0x01, 0xbe, 0xf7, 0x01, 0xe7, 0x39, 0x0c, 0x00, 0x00, 0x01, 0x65, 0x29, 0x07, 0xff,
            0xff, 0x01, 0x5d, 0xef, 0x01, 0x61, 0x08, 0x0a, 0x00, 0x00, 0x01, 0x61, 0x08, 0x01, 0xa6, 0x31,
            0x01, 0x0c, 0x63, 0x01, 0xd3, 0x9c, 0x07, 0xff, 0xff, 0x04, 0xff, 0xff, 0x01, 0xbe, 0xf7, 0x0d,
            0x00, 0x00, 0x01, 0xe7, 0x39, 0x0a, 0xff, 0xff, 0x01, 0x5d, 0xef, 0x01, 0x38, 0xc6, 0x01, 0xd3,
            0x9c, 0x01, 0x6d, 0x6b, 0x01, 0x08, 0x42, 0x01, 0x24, 0x21, 0x01, 0x20, 0x00, 0x08, 0x00, 0x00,
            0x01, 0xd3, 0x9c, 0x05, 0xff, 0xff, 0x04, 0xff, 0xff, 0x01, 0xe7, 0x39, 0x02, 0x00, 0x00, 0x01,
            0x71, 0x8c, 0x01, 0x3c, 0xe7, 0x01, 0x7d, 0xef, 0x01, 0xbe, 0xf7, 0x19, 0xff, 0xff, 0x01, 0xdf,
            0xff, 0x01, 0x9a, 0xd6, 0x01, 0x14, 0xa5, 0x01, 0xae, 0x73, 0x01, 0x49, 0x4a, 0x01, 0xe3, 0x18,
            0x03, 0x00, 0x00, 0x01, 0xb6, 0xb5, 0x04, 0xff, 0xff, 0x04, 0xff, 0xff, 0x02, 0x00, 0x00, 0x01,
            0x71, 0x8c, 0x23, 0xff, 0xff, 0x01, 0xbe, 0xf7, 0x01, 0x61, 0x08, 0x01, 0x00, 0x00, 0x01, 0x82,
            0x10, 0x04, 0xff, 0xff, 0x03, 0xff, 0xff, 0x01, 0x1c, 0xe7, 0x02, 0x00, 0x00, 0x01, 0xfb, 0xde,
            0x24, 0xff, 0xff, 0x01, 0xf7, 0xbd, 0x02, 0x00, 0x00, 0x01, 0xbe, 0xf7, 0x03, 0xff, 0xff, 0x03,
            0xff, 0xff, 0x01, 0x3c, 0xe7, 0x02, 0x00, 0x00, 0x01, 0xba, 0xd6, 0x1a, 0xff, 0xff, 0x02, 0x34,
            0xa5, 0x08, 0xff, 0xff, 0x01, 0xdb, 0xde, 0x02, 0x00, 0x00, 0x01, 0x3c, 0xe7, 0x03, 0xff, 0xff,
            0x03, 0xff, 0xff, 0x01, 0x7d, 0xef, 0x02, 0x00, 0x00, 0x01, 0x9a, 0xd6, 0x18, 0xff, 0xff, 0x01,
            0x34, 0xa5, 0x04, 0x00, 0x00, 0x01, 0x14, 0xa5, 0x06, 0xff, 0xff, 0x01, 0x96, 0xb5, 0x02, 0x00,
            0x00, 0x04, 0xff, 0xff, 0x03, 0xff, 0xff, 0x01, 0xbe, 0xf7, 0x02, 0x00, 0x00, 0x01, 0x59, 0xce,
            0x17, 0xff, 0xff, 0x01, 0x3c, 0xe7, 0x06, 0x00, 0x00, 0x01, 0x3c, 0xe7, 0x05, 0xff, 0xff, 0x01,
            0x10, 0x84, 0x01, 0x00, 0x00, 0x01, 0x41, 0x08, 0x04, 0xff, 0xff, 0x04, 0xff, 0xff, 0x02, 0x00,
            0x00, 0x01, 0x18, 0xc6, 0x17, 0xff, 0xff, 0x01, 0xae, 0x73, 0x06, 0x00, 0x00, 0x01, 0x8e, 0x73,
            0x05, 0xff, 0xff, 0x01, 0xaa, 0x52, 0x01, 0x00, 0x00, 0x01, 0x45, 0x29, 0x04, 0xff, 0xff, 0x04,
            0xff, 0xff, 0x02, 0x00, 0x00, 0x01, 0xd7, 0xbd, 0x05, 0xff, 0xff, 0x01, 0xc7, 0x39, 0x01, 0x18,
            0xc6, 0x10, 0xff, 0xff, 0x01, 0x4d, 0x6b, 0x06, 0x00, 0x00, 0x01, 0x2c, 0x63, 0x05, 0xff, 0xff,
            0x01, 0x65, 0x29, 0x01, 0x00, 0x00, 0x01, 0x69, 0x4a, 0x04, 0xff, 0xff, 0x04, 0xff, 0xff, 0x02,
            0x00, 0x00, 0x01, 0x75, 0xad, 0x04, 0xff, 0xff, 0x01, 0xc7, 0x39, 0x02, 0x00, 0x00, 0x01, 0xf7,
            0xbd, 0x0f, 0xff, 0xff, 0x01, 0xf7, 0xbd, 0x06, 0x00, 0x00, 0x01, 0xf7, 0xbd, 0x05, 0xff, 0xff,
            0x01, 0x20, 0x00, 0x01, 0x00, 0x00, 0x01, 0xef, 0x7b, 0x04, 0xff, 0xff, 0x04, 0xff, 0xff, 0x02,
            0x00, 0x00, 0x01, 0x34, 0xa5, 0x03, 0xff, 0xff, 0x01, 0x28, 0x42, 0x04, 0x00, 0x00, 0x01, 0x55,
            0xad, 0x0f, 0xff, 0xff, 0x01, 0x8a, 0x52, 0x04, 0x00, 0x00, 0x01, 0x69, 0x4a, 0x06, 0xff, 0xff,
            0x02, 0x00, 0x00, 0x01, 0x55, 0xad, 0x04, 0xff, 0xff, 0x04, 0xff, 0xff, 0x02, 0x00, 0x00, 0x01,
            0xf3, 0x9c, 0x02, 0xff, 0xff, 0x01, 0x69, 0x4a, 0x06, 0x00, 0x00, 0x01, 0x34, 0xa5, 0x0f, 0xff,
            0xff, 0x01, 0x34, 0xa5, 0x02, 0x28, 0x42, 0x01, 0x34, 0xa5, 0x06, 0xff, 0xff, 0x01, 0x3c, 0xe7,
            0x02, 0x00, 0x00, 0x01, 0x79, 0xce, 0x04, 0xff, 0xff, 0x04, 0xff, 0xff, 0x02, 0x00, 0x00, 0x01,
            0xd3, 0x9c, 0x01, 0xff, 0xff, 0x01, 0x0c, 0x63, 0x08, 0x00, 0x00, 0x01, 0x51, 0x8c, 0x18, 0xff,
            0xff, 0x01, 0xb6, 0xb5, 0x02, 0x00, 0x00, 0x01, 0x9e, 0xf7, 0x04, 0xff, 0xff, 0x04, 0xff, 0xff,
            0x01, 0x20, 0x00, 0x01, 0x00, 0x00, 0x01, 0x55, 0xad, 0x01, 0xef, 0x7b, 0x0a, 0x00, 0x00, 0x01,
            0x30, 0x84, 0x17, 0xff, 0xff, 0x01, 0x71, 0x8c, 0x02, 0x00, 0x00, 0x05, 0xff, 0xff, 0x04, 0xff,
            0xff, 0x01, 0x61, 0x08, 0x01, 0x00, 0x00, 0x01, 0xe3, 0x18, 0x0c, 0x00, 0x00, 0x01, 0x6d, 0x6b,
            0x0c, 0xff, 0xff, 0x01, 0xb6, 0xb5, 0x01, 0xf7, 0xbd, 0x08, 0xff, 0xff, 0x01, 0xeb, 0x5a, 0x01,
            0x00, 0x00, 0x01, 0xc3, 0x18, 0x05, 0xff, 0xff, 0x04, 0xff, 0xff, 0x01, 0x82, 0x10, 0x0f, 0x00,
            0x00, 0x01, 0x0c, 0x63, 0x09, 0xff, 0xff, 0x01, 0x18, 0xc6, 0x03, 0x00, 0x00, 0x01, 0xc3, 0x18,
            0x07, 0xff, 0xff, 0x01, 0x86, 0x31, 0x01, 0x00, 0x00, 0x01, 0x49, 0x4a, 0x05, 0xff, 0xff, 0x04,
            0xff, 0xff, 0x01, 0xc3, 0x18, 0x10, 0x00, 0x00, 0x01, 0x69, 0x4a, 0x07, 0xff, 0xff, 0x01, 0x8e,
            0x73, 0x05, 0x00, 0x00, 0x01, 0x24, 0x21, 0x06, 0xff, 0xff, 0x01, 0x82, 0x10, 0x01, 0x00, 0x00,
            0x01, 0x8e, 0x73, 0x05, 0xff, 0xff, 0x04, 0xff, 0xff, 0x01, 0x04, 0x21, 0x11, 0x00, 0x00, 0x01,
            0x08, 0x42, 0x06, 0xff, 0xff, 0x01, 0xae, 0x73, 0x06, 0x00, 0x00, 0x01, 0xae, 0x73, 0x04, 0xff,
            0xff, 0x01, 0xdf, 0xff, 0x02, 0x00, 0x00, 0x01, 0x14, 0xa5, 0x05, 0xff, 0xff, 0x04, 0xff, 0xff,
            0x01, 0x24, 0x21, 0x12, 0x00, 0x00, 0x01, 0xc7, 0x39, 0x06, 0xff, 0xff, 0x01, 0xa6, 0x31, 0x06,
            0x00, 0x00, 0x01, 0x18, 0xc6, 0x03, 0xff, 0xff, 0x01, 0x3c, 0xe7, 0x02, 0x00, 0x00, 0x01, 0x79,
            0xce, 0x05, 0xff, 0xff, 0x04, 0xff, 0xff, 0x01, 0x65, 0x29, 0x13, 0x00, 0x00, 0x01, 0x04, 0x21,
            0x06, 0xff, 0xff, 0x07, 0x00, 0x00, 0x01, 0xbe, 0xf7, 0x02, 0xff, 0xff, 0x01, 0xf7, 0xbd, 0x02,
            0x00, 0x00, 0x01, 0x9e, 0xf7, 0x05, 0xff, 0xff, 0x04, 0xff, 0xff, 0x01, 0xa6, 0x31, 0x14, 0x00,
            0x00, 0x01, 0xc3, 0x18, 0x01, 0xbe, 0xf7, 0x04, 0xff, 0xff, 0x01, 0x3c, 0xe7, 0x06, 0x00, 0x00,
            0x01, 0x04, 0x21, 0x02, 0xff, 0xff, 0x01, 0x92, 0x94, 0x02, 0x00, 0x00, 0x06, 0xff, 0xff, 0x04,
            0xff, 0xff, 0x01, 0xc7, 0x39, 0x15, 0x00, 0x00, 0x01, 0x5d, 0xef, 0x04, 0xff, 0xff, 0x01, 0xf3,
            0x9c, 0x07, 0x00, 0x00, 0x01, 0x0c, 0x63, 0x01, 0xff, 0xff, 0x01, 0x2c, 0x63, 0x01, 0x00, 0x00,
            0x01, 0xe3, 0x18, 0x06, 0xff, 0xff, 0x04, 0xff, 0xff, 0x01, 0x08, 0x42, 0x14, 0x00, 0x00, 0x01,
            0x9e, 0xf7, 0x04, 0xff, 0xff, 0x01, 0x28, 0x42, 0x09, 0x00, 0x00, 0x01, 0x79, 0xce, 0x01, 0x28,
            0x42, 0x01, 0x00, 0x00, 0x01, 0xe7, 0x39, 0x06, 0xff, 0xff, 0x04, 0xff, 0xff, 0x01, 0x49, 0x4a,
            0x13, 0x00, 0x00, 0x01, 0x3c, 0xe7, 0x03, 0xff, 0xff, 0x01, 0xfb, 0xde, 0x0c, 0x00, 0x00, 0x01,
            0xa2, 0x10, 0x01, 0x00, 0x00, 0x01, 0x6d, 0x6b, 0x06, 0xff, 0xff, 0x04, 0xff, 0xff, 0x01, 0x8a,
            0x52, 0x12, 0x00, 0x00, 0x01, 0xdb, 0xde, 0x03, 0xff, 0xff, 0x01, 0xd3, 0x9c, 0x0f, 0x00, 0x00,
            0x01, 0xd3, 0x9c, 0x06, 0xff, 0xff, 0x04, 0xff, 0xff, 0x01, 0x2c, 0x63, 0x11, 0x00, 0x00, 0x01,
            0x79, 0xce, 0x03, 0xff, 0xff, 0x01, 0x8a, 0x52, 0x10, 0x00, 0x00, 0x01, 0x18, 0xc6, 0x06, 0xff,
            0xff, 0x04, 0xff, 0xff, 0x01, 0x75, 0xad, 0x10, 0x00, 0x00, 0x01, 0xf7, 0xbd, 0x02, 0xff, 0xff,
            0x01, 0x7d, 0xef, 0x01, 0x20, 0x00, 0x11, 0x00, 0x00, 0x01, 0xdf, 0xff, 0x06, 0xff, 0xff, 0x05,
            0xff, 0xff, 0x01, 0x61, 0x08, 0x0e, 0x00, 0x00, 0x01, 0x96, 0xb5, 0x02, 0xff, 0xff, 0x01, 0x30,
            0x84, 0x12, 0x00, 0x00, 0x01, 0x65, 0x29, 0x07, 0xff, 0xff, 0x05, 0xff, 0xff, 0x01, 0x9e, 0xf7,
            0x01, 0x82, 0x10, 0x0c, 0x00, 0x00, 0x01, 0x14, 0xa5, 0x03, 0xff, 0xff, 0x01, 0x5d, 0xef, 0x01,
            0x59, 0xce, 0x01, 0xf3, 0x9c, 0x01, 0x8e, 0x73, 0x01, 0x28, 0x42, 0x01, 0xc3, 0x18, 0x01, 0x20,
            0x00, 0x0c, 0x00, 0x00, 0x01, 0x9e, 0xf7, 0x07, 0xff, 0xff, 0x07, 0xff, 0xff, 0x01, 0xd3, 0x9c,
            0x01, 0x49, 0x4a, 0x01, 0xc7, 0x39, 0x01, 0x08, 0x42, 0x01, 0x69, 0x4a, 0x01, 0x8a, 0x52, 0x01,
            0xeb, 0x5a, 0x01, 0x2c, 0x63, 0x01, 0x6d, 0x6b, 0x01, 0xae, 0x73, 0x01, 0xcf, 0x7b, 0x01, 0x75,
            0xad, 0x0b, 0xff, 0xff, 0x01, 0x7d, 0xef, 0x01, 0x9a, 0xd6, 0x01, 0x34, 0xa5, 0x01, 0xcf, 0x7b,
            0x01, 0x69, 0x4a, 0x01, 0x04, 0x21, 0x04, 0x00, 0x00, 0x01, 0xe7, 0x39, 0x01, 0xbe, 0xf7, 0x08,
            0xff, 0xff, 0x26, 0xff, 0xff, 0x01, 0x7d, 0xef, 0x0b, 0xff, 0xff, 0x32, 0xff, 0xff, 0x32, 0xff,
            0xff, 0x32, 0xff, 0xff, 0x32, 0xff, 0xff, 0x32, 0xff, 0xff, 0x32, 0xff, 0xff, 0x32, 0xff, 0xff,
            0x32, 0xff, 0xff,
        };
        addr = image404;
    }
    return addr;
}
/**
 * @brief Retrieves the address of an image file.
 *
 * This function takes the file path of an image and returns the memory address
 * where the image data is stored. It is used to access image files in the GUI domain.
 *
 * @param image_file_path The path to the image file whose address is to be retrieved.
 *                        This should be a null-terminated string representing the file path.
 *
 * @return A pointer to a uint8_t representing the address of the image file data.
 *         If the file cannot be found or accessed, the function may return a error image data.
 */
const uint8_t *gui_dom_get_image_file_address(const char *image_file_path)
{
    return gui_get_image_file_address(image_file_path);
}
void gui_get_json_value(const char *path, const char *parent_key, const char *key, void *value)
{
    cJSON *root;
    if (!path)
    {
        gui_log("path can't be NULL!\r\n");
        return;
    }
    else
    {
        if (!parent_key || !key)
        {
            gui_log("parent_key and key can't be NULL!\r\n");
            return;
        }
        root = cJSON_Parse(path);
        if (!root)
        {
            gui_log("Error parsing JSON!\r\n");
            return;
        }
    }
    // parse array
    cJSON *json_array = cJSON_GetObjectItemCaseSensitive(root, parent_key);
    if (!cJSON_IsArray(json_array))
    {
        //gui_log("get %s_Array unsuccessful\n", parent_key);
        cJSON_Delete(root);
        return;
    }
    else
    {
        cJSON *obj = cJSON_GetArrayItem(json_array, 0);
        if (!obj)
        {
            //gui_log("get %s_ArrayItem unsuccessful\n", parent_key);
            cJSON_Delete(root);
            return;
        }
        else
        {
            cJSON *json_key = cJSON_GetObjectItemCaseSensitive(obj, key);
            if (!json_key)
            {
                //gui_log("get %s_Key unsuccessful\n", key);
                cJSON_Delete(root);
                return;
            }
            else
            {
                if (strcmp(key, "array") == 0)
                {
                    int array_size = cJSON_GetArraySize(json_key);
                    for (int i = 0; i < array_size; i++)
                    {
                        cJSON *array_item = cJSON_GetArrayItem(json_key, i);
                        if (cJSON_IsNumber(array_item))
                        {
                            ((int *)value)[i] = array_item->valueint;
                            //printf("array[%d] = %d\n", i, array_item->valueint);
                        }
                    }
                }
                else if (strstr(key, "condition") != NULL)
                {
                    sprintf((char *)value, "%s", json_key->valuestring);
                }
                else
                {
                    *((int *)value) = json_key->valueint;
                }
            }
        }
    }
    // clear
    cJSON_Delete(root);
}
float *gui_get_json_array(const char *path, const char *parent_key, const char *key,
                          int *array_length)
{
    cJSON *root;
    float *array = 0;
    *array_length = 0;
    if (!path)
    {
        gui_log("path can't be NULL!\r\n");
        return 0;
    }
    else
    {
        if (!parent_key || !key)
        {
            gui_log("parent_key and key can't be NULL!\r\n");
            return 0;
        }
        root = cJSON_Parse(path);
        if (!root)
        {
            gui_log("Error parsing JSON!\r\n");
            return 0;
        }
    }
    // parse array
    cJSON *json_array = cJSON_GetObjectItemCaseSensitive(root, parent_key);
    if (!cJSON_IsArray(json_array))
    {
        //gui_log("get %s_Array unsuccessful\n", parent_key);
        cJSON_Delete(root);
        return 0;
    }
    else
    {
        cJSON *obj = cJSON_GetArrayItem(json_array, 0);
        if (!obj)
        {
            //gui_log("get %s_ArrayItem unsuccessful\n", parent_key);
            cJSON_Delete(root);
            return 0;
        }
        else
        {
            cJSON *json_key = cJSON_GetObjectItemCaseSensitive(obj, key);
            if (!json_key)
            {
                //gui_log("get %s_Key unsuccessful\n", key);
                cJSON_Delete(root);
                return 0;
            }
            else
            {
                if (strcmp(key, "array") == 0)
                {
                    int array_size = cJSON_GetArraySize(json_key);
                    array = gui_malloc((array_size) * sizeof(float));
                    memset(array, 0, (array_size)*sizeof(float));
                    for (int i = 0; i < array_size; i++)
                    {
                        cJSON *array_item = cJSON_GetArrayItem(json_key, i);
                        if (cJSON_IsNumber(array_item))
                        {
                            array[i] = array_item->valuedouble;
                            //printf("array[%d] = %d,%f\n", i, array_item->valueint, array_item->valuedouble);
                        }
                    }
                    *array_length = array_size;


                }

            }
        }
    }
    // clear
    cJSON_Delete(root);
    return array;
}
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(text_animate_weather_callback)
{
    if (!animate->Beginning_frame)
    {
        return;
    }

    switch ((size_t)p)
    {
    case TEXT_WEATHER_CUR_ANIMATION:
        {
            const char *path =
                gui_get_path_by_relative("app/system/peripheral_simulation/json/simulation_data.json");
            int fd = gui_fs_open(path, 0);
            gui_free((void *)path);
            if (fd < 1)
            {
                return;
            }
            int size = gui_fs_lseek(fd, 0, SEEK_END);
            gui_fs_lseek(fd, 0, SEEK_SET);
            char *json_string = gui_malloc(size + 1);
            gui_fs_read(fd, json_string, size);
            gui_fs_close(fd);
            json_string[size] = '\0';
            int move = 0;
            //gui_log("%s\n",json_string);
            gui_get_json_value(json_string, "weather", "current", &move);
            gui_free(json_string);
            static char move_string[4];
            memset(move_string, 0, GUI_ARRAY_SIZE(move_string));
            sprintf(move_string, "%d", move);
            gui_text_content_set(this_widget, move_string, strlen(move_string));
        }
        break;

    default:
        break;
    }

}
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(text_animation_hr_callback)
{
    if (!animate->Beginning_frame)
    {
        return;
    }

    switch ((size_t)p)
    {
    case TEXT_HEART_RATE_CUR_ANIMATION:
        {
            const char *path =
                gui_get_path_by_relative("app/system/peripheral_simulation/json/simulation_data.json");
            int fd = gui_fs_open(path, 0);
            if (fd < 1)
            {
                return;
            }
            gui_free((void *)path);
            int size = gui_fs_lseek(fd, 0, SEEK_END);
            gui_fs_lseek(fd, 0, SEEK_SET);
            char *json_string = gui_malloc(size + 1);
            gui_fs_read(fd, json_string, size);
            gui_fs_close(fd);
            json_string[size] = '\0';
            int array_length = 0;
            //gui_log("%s\n",json_string);
            float *array = gui_get_json_array(json_string, "heart_rate", "array", &array_length);
            gui_free(json_string);
            if (array_length == 0 || array == 0)
            {
                return;
            }
            static char move_string[4];
            memset(move_string, 0, GUI_ARRAY_SIZE(move_string));
            sprintf(move_string, "%d", (int)array[array_length - 1]);
            gui_text_content_set(this_widget, move_string, strlen(move_string));
            gui_free(array);
        }
        break;

    default:
        break;
    }

}
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(text_animate_activity_callback)
{
    if (!animate->Beginning_frame)
    {
        return;
    }

    switch ((size_t)p)
    {
    case TEXT_ACTIVITY_EX_ANIMATION:
        {
            const char *path =
                gui_get_path_by_relative("app/system/peripheral_simulation/json/simulation_data.json");
            int fd = gui_fs_open(path, 0);
            if (fd < 1)
            {
                return;
            }
            gui_free((void *)path);
            int size = gui_fs_lseek(fd, 0, SEEK_END);
            gui_fs_lseek(fd, 0, SEEK_SET);
            char *json_string = gui_malloc(size + 1);
            gui_fs_read(fd, json_string, size);
            gui_fs_close(fd);
            json_string[size] = '\0';
            int move = 0;
            //gui_log("%s\n",json_string);
            gui_get_json_value(json_string, "activity", "exercise", &move);
            gui_free(json_string);

            static char move_string[4];
            memset(move_string, 0, GUI_ARRAY_SIZE(move_string));
            sprintf(move_string, "%d", (int)move);
            gui_text_content_set(this_widget, move_string, strlen(move_string));
        }
        break;

    default:
        break;
    }

}
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(text_animate_battery_callback)
{
    if (!animate->Beginning_frame)
    {
        return;
    }

    switch ((size_t)p)
    {
    case TEXT_BATTERY_CAPACITY_ANIMATION:
        {
            const char *path =
                gui_get_path_by_relative("app/system/peripheral_simulation/json/simulation_data.json");
            int fd = gui_fs_open(path, 0);
            if (fd < 1)
            {
                return;
            }
            gui_free((void *)path);
            int size = gui_fs_lseek(fd, 0, SEEK_END);
            gui_fs_lseek(fd, 0, SEEK_SET);
            char *json_string = gui_malloc(size + 1);
            gui_fs_read(fd, json_string, size);
            gui_fs_close(fd);
            json_string[size] = '\0';
            int move = 0;
            //gui_log("%s\n",json_string);
            gui_get_json_value(json_string, "battery", "capacity", &move);
            gui_free(json_string);

            static char move_string[4];
            memset(move_string, 0, GUI_ARRAY_SIZE(move_string));
            sprintf(move_string, "%d", (int)move);
            gui_text_content_set(this_widget, move_string, strlen(move_string));
        }
        break;

    default:
        break;
    }

}
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(text_animate_wifi_callback)
{
    if (!animate->Beginning_frame)
    {
        return;
    }

    switch ((size_t)p)
    {
    case TEXT_WIFI_NAME_ANIMATION:
        {
            if (active_wifi_index >= 0 && active_wifi_index < XML_DOM_WIFI_COUINT &&
                wifi_array[active_wifi_index].name)
            {
                gui_text_content_set(this_widget, (void *)wifi_array[active_wifi_index].name,
                                     strlen(wifi_array[active_wifi_index].name));
            }
        }
        break;
    case TEXT_WIFI_PASSWORD_ANIMATION:
        {
            if (active_wifi_index >= 0 && active_wifi_index < XML_DOM_WIFI_COUINT &&
                wifi_array[active_wifi_index].name)
            {
                char *password = GUI_TYPE(gui_text_t, this_widget)->content;
                if (password && strlen(password) > 0)
                {
                    {
                        strncpy(active_wifi_password_input, password, sizeof(active_wifi_password_input) - 1);
                        active_wifi_password_input[sizeof(active_wifi_password_input) - 1] =
                            '\0'; // Null-terminate the array

                    }

                }


            }
        }
        break;
    default:
        break;
    }

}
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(arc_animate_activity_callback)
{
    if (!animate->Beginning_frame)
    {
        return;
    }
    arc_animation_param_t *param = p;
    switch (param->aniamtion_type)
    {
    case ARC_ACTIVITY_EX_ANIMATION:
        {
            const char *path =
                gui_get_path_by_relative("app/system/peripheral_simulation/json/simulation_data.json");
            int fd = gui_fs_open(path, 0);
            if (fd < 1)
            {
                return;
            }
            gui_free((void *)path);
            int size = gui_fs_lseek(fd, 0, SEEK_END);
            gui_fs_lseek(fd, 0, SEEK_SET);
            char *json_string = gui_malloc(size + 1);
            gui_fs_read(fd, json_string, size);
            gui_fs_close(fd);
            json_string[size] = '\0';
            int move = 0;
            //gui_log("%s\n",json_string);
            gui_get_json_value(json_string, "activity", "exercise", &move);
            gui_free(json_string);
            int max = 60;
            int min = 0;
            float per = (float)(move - min) / (float)(max - min);
            //gui_log("%d,%f\n", move, per);
            arc_animation_param_t *param = p;
            {
                memset(param->target_buffer, 0, param->image_data_length);
                //int format = GUI_CANVAS_OUTPUT_RGBA;
                bool compression = 0; GUI_UNUSED(compression);
                int image_width = param->image_width;
                int image_height = param->image_height;  uint8_t *target_buffer = param->target_buffer;
                {
                    int pixel_length = 4;
                    //int data_length = 0;
                    uint8_t *buffer = 0;
                    uint8_t *output_data = 0;
                    // switch (format)
                    // {
                    // case GUI_CANVAS_OUTPUT_PNG:
                    // case GUI_CANVAS_OUTPUT_JPG:
                    //     {
                    //         data_length = image_width * image_height * pixel_length;
                    //         buffer = gui_lower_malloc(data_length);
                    //         memset(buffer, 0, data_length);
                    //     }
                    //     break;
                    // case GUI_CANVAS_OUTPUT_RGBA:
                    //     {
                    //         output_data = target_buffer;
                    //         buffer = output_data + sizeof(gui_rgb_data_head_t);
                    //         memset(output_data, 0, sizeof(gui_rgb_data_head_t));
                    //         gui_rgb_data_head_t *head = (void *)output_data;
                    //         head->type = ARGB8888;
                    //         head->w = image_width;
                    //         head->h = image_height;
                    //     }
                    //     break;
                    // case GUI_CANVAS_OUTPUT_RGB565:
                    //     {
                    //         pixel_length = 2;
                    //         output_data = target_buffer;
                    //         memset(output_data, 0, sizeof(gui_rgb_data_head_t));
                    //         buffer = output_data + sizeof(gui_rgb_data_head_t);
                    //         gui_rgb_data_head_t *head = (void *)output_data;
                    //         head->type = RGB565;
                    //         head->w = image_width;
                    //         head->h = image_height;
                    //     }
                    //     break;
                    // default:
                    //     break;
                    // }
                    output_data = target_buffer;
                    buffer = output_data + sizeof(gui_rgb_data_head_t);
                    memset(output_data, 0, sizeof(gui_rgb_data_head_t));
                    gui_rgb_data_head_t *head = (void *)output_data;
                    head->type = ARGB8888;
                    head->w = image_width;
                    head->h = image_height;
                    {
                        NVGcontext *vg = 0;
                        extern NVGcontext *nvgCreateAGGE(uint32_t w,
                                                         uint32_t h,
                                                         uint32_t stride,
                                                         enum     NVGtexture format,
                                                         uint8_t *data);
                        extern void nvgDeleteAGGE(NVGcontext * ctx);
                        // vg = nvgCreateAGGE(image_width, image_height, image_width * (pixel_length),
                        //                    (pixel_length) == 2 ? NVG_TEXTURE_BGR565 : NVG_TEXTURE_BGRA, buffer);
                        vg = nvgCreateAGGE(image_width, image_height, image_width * (pixel_length),
                                           NVG_TEXTURE_BGRA, buffer);
                        nvgBeginFrame(vg, image_width, image_height, 1);

                        nvgResetTransform(vg);
                        float a0 = 0,  a1 = 0;
                        a0 = param->sd;
                        if (param->dir == NVG_CW)
                        {
                            a1 = (param->ed - param->sd) * per + param->sd;
                        }
                        else if (param->dir == NVG_CCW)
                        {
                            a1 = ((2 * M_PI - (param->ed - param->sd)) * (1 - per) + param->ed);
                            if (per == 0)
                            {
                                a1 = a0;
                            }
                        }


                        if (a0 != a1)
                        {
                            nvgArc(vg, param->r + param->stroke_width, param->r + param->stroke_width, param->r, a0, a1,
                                   param->dir);
                            nvgStrokeWidth(vg, param->stroke_width);
                            nvgStrokeColor(vg, nvgRGBA(param->stroke.color.rgba.r, param->stroke.color.rgba.g,
                                                       param->stroke.color.rgba.b, param->stroke.color.rgba.a));
                            nvgStroke(vg);
                        }



                        nvgEndFrame(vg);
                        nvgDeleteAGGE(vg);
                    }
                }
            }
        }
        break;

    default:
        break;
    }
}
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(chart_animate_heartrate_data_callback)
{
    if (!animate->Beginning_frame)
    {
        return;
    }
    chart_animation_param_t *param = p;
    switch (param->aniamtion_type)
    {
    case CHART_HEART_RATE_DATA_ANIMATION:
        {
            const char *path =
                gui_get_path_by_relative("app/system/peripheral_simulation/json/simulation_data.json");
            int fd = gui_fs_open(path, 0);
            if (fd < 1)
            {
                return;
            }
            gui_free((void *)path);
            int size = gui_fs_lseek(fd, 0, SEEK_END);
            gui_fs_lseek(fd, 0, SEEK_SET);
            char *json_string = gui_malloc(size + 1);
            gui_fs_read(fd, json_string, size);
            gui_fs_close(fd);
            json_string[size] = '\0';
            int array_length = 0;
            //gui_log("%s\n",json_string);
            float *array = gui_get_json_array(json_string, "heart_rate", "array", &array_length);
            int max = 0, min = 0;
            gui_get_json_value(json_string, "heart_rate", "range_min", &min);
            gui_get_json_value(json_string, "heart_rate", "range_max", &max);
            if (max != 0)
            {
                param->max = max;
            }
            if (min != 0)
            {
                param->min = min;
            }
            gui_free(json_string);
            if (array_length == 0 || array == 0)
            {
                return;
            }

            int w = param->image_width;
            int h = param->image_height;


            uint8_t *buffer = param->target_buffer;
            memset(buffer, 0, param->image_data_length);
            NVGcontext *vg = gui_canvas_output_buffer_blank(GUI_CANVAS_OUTPUT_RGBA, 0, w, h, buffer);
            if (param->chart_type == 1)
            {
                gui_wave_width_render(vg, 0, 0, w,
                                      h,
                                      array_length,
                                      array,
                                      param->color,
                                      param->max,
                                      param->min, param->stroke_width);
            }
            else if (param->chart_type == 2)
            {
                gui_bar_render(vg, 0, 0, w,
                               h,
                               array_length,
                               array,
                               param->color,
                               param->max,
                               param->min, param->stroke_width);
            }
            else if (param->chart_type == 3)
            {
                gui_line_render(vg, 0, 0, w,
                                h,
                                array_length,
                                array,
                                param->color,
                                param->max,
                                param->min, param->stroke_width);
            }
            else
            {
                gui_wave_width_render(vg, 0, 0, w,
                                      h,
                                      array_length,
                                      array,
                                      param->color,
                                      param->max,
                                      param->min, param->stroke_width);
            }
            gui_free(array);
            gui_canvas_output_buffer_blank_close(vg);
            gui_img_set_image_data(this_widget, buffer);
        }
    }

}
static void get_2_int_from_string(const char *input, int *int1, int *int2)
{
    char *token;
    int numbers[2];
    numbers[0] = 0;
    numbers[1] = 0;
    int index = 0;
    char *buffer = gui_strdup(input);
    token = strtok(buffer, ",");
    while (token != NULL && index < 2)
    {
        numbers[index] = atoi(token);
        index++;
        token = strtok(NULL, ",");
    }


    *int1 = numbers[0];
    *int2 = numbers[1];
    gui_free(buffer);
}
static bool ends_with_xml(const char *str)
{
    if (str == NULL)
    {
        return false; // Handle null pointer input gracefully
    }

    // Define the suffix we are looking for
    const char *suffix = ".xml";
    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);

    // If the string is shorter than the suffix, it cannot end with the suffix
    if (str_len < suffix_len)
    {
        return false;
    }

    // Compare the end of the string with the suffix
    return strcmp(str + str_len - suffix_len, suffix) == 0;
}
static void app_xml_ui_design(gui_app_t *app)
{
    extern void create_tree(gui_app_t *app);
    create_tree(app);
    gui_fb_change();
}
static gui_app_t app_xml =
{
    .screen =
    {
        .name = "app_xml",
        .x    = 0,
        .y    = 0,
    },
    /*
    * Default no sleep, no active time limitation.
    * Please set active time by api(gui_set_app_active_time) if power control is needed.
    */
    .active_ms = (uint32_t) - 1,
    .ui_design = app_xml_ui_design,
};

void *get_app_xml(void)
{
    return &app_xml;
}
extern void gui_img_destroy(gui_obj_t *obj);
extern void gui_img_input_prepare(gui_obj_t *obj);
extern void gui_img_prepare(gui_obj_t *obj);
extern void gui_img_draw_cb(gui_obj_t *obj);
extern void gui_img_end(gui_obj_t *obj);
static void canvas_img_destroy(gui_obj_t *obj)
{
    gui_img_destroy(obj);
    gui_free(GUI_TYPE(gui_img_t, obj)->data);
}

static void gui_canvas_img_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{

    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_INPUT_PREPARE:
            {
                gui_img_input_prepare(obj);
            }
            break;
        case OBJ_PREPARE:
            {
                gui_img_prepare(obj);
            }
            break;

        case OBJ_DRAW:
            {
                gui_img_draw_cb(obj);
            }
            break;

        case OBJ_END:
            {
                gui_img_end(obj);
            }
            break;

        case OBJ_DESTROY:
            {
                canvas_img_destroy(obj);
            }
            break;

        default:
            break;
        }
    }
}
static void img_ontime_destroy(gui_obj_t *obj)
{
    gui_img_t *img = GUI_TYPE(gui_img_t, obj);
    for (size_t i = 0; i < img->animate_array_length; i++)
    {
        gui_animate_t *animate = ((gui_animate_t **)(img->animate))[i];
        gui_free(animate->p);
    }
    gui_img_destroy(obj);
}
static void img_ontime_canvas_buffer_destroy(gui_obj_t *obj)
{
    gui_img_t *img = GUI_TYPE(gui_img_t, obj);
    for (size_t i = 0; i < img->animate_array_length; i++)
    {
        gui_animate_t *animate = ((gui_animate_t **)(img->animate))[i];
        gui_free(animate->p);
    }
    gui_img_destroy(obj);
    gui_free(GUI_TYPE(gui_img_t, obj)->data);
}
static void img_ontime_canvas_buffer_render(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{

    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_INPUT_PREPARE:
            {
                gui_img_input_prepare(obj);
            }
            break;
        case OBJ_PREPARE:
            {
                gui_img_prepare(obj);
            }
            break;

        case OBJ_DRAW:
            {
                gui_img_draw_cb(obj);
            }
            break;

        case OBJ_END:
            {
                gui_img_end(obj);
            }
            break;

        case OBJ_DESTROY:
            {
                img_ontime_canvas_buffer_destroy(obj);
            }
            break;

        default:
            break;
        }
    }
}

static void img_ontime_render(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{

    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_INPUT_PREPARE:
            {
                gui_img_input_prepare(obj);
            }
            break;
        case OBJ_PREPARE:
            {
                gui_img_prepare(obj);
            }
            break;

        case OBJ_DRAW:
            {
                gui_img_draw_cb(obj);
            }
            break;

        case OBJ_END:
            {
                gui_img_end(obj);
            }
            break;

        case OBJ_DESTROY:
            {
                img_ontime_destroy(obj);
            }
            break;

        default:
            break;
        }
    }
}

static void img_destroy(gui_obj_t *obj)
{
    gui_free((void *)obj->name);
    gui_img_destroy(obj);
}
static void img_render(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{

    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_INPUT_PREPARE:
            {
                gui_img_input_prepare(obj);
            }
            break;
        case OBJ_PREPARE:
            {
                gui_img_prepare(obj);
            }
            break;

        case OBJ_DRAW:
            {
                gui_img_draw_cb(obj);
            }
            break;

        case OBJ_END:
            {
                gui_img_end(obj);
            }
            break;

        case OBJ_DESTROY:
            {
                img_destroy(obj);
            }
            break;

        default:
            break;
        }
    }
}
static void gui_button_destroy(gui_obj_t *obj)
{
    gui_button_t *button = GUI_TYPE(gui_button_t, obj);
    if (button && button->text && button->text->content)
    {
        gui_free(button->text->content);
    }
    if (obj)
    {
        gui_free((void *)obj->name);
    }



}
extern void gui_button_prepare(gui_obj_t *obj);
extern void gui_button_input_prepare(gui_obj_t *obj);
static void button_render(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_INPUT_PREPARE:
            gui_button_input_prepare(obj);
            break;
        case OBJ_PREPARE:
            gui_button_prepare(obj);
            break;
        case OBJ_DESTROY:
            {
                gui_button_destroy(obj);
            }
            break;
        default:
            break;
        }
    }
}
#include "gui_text.h"
#include "gui_matrix.h"
#include "font_mem.h"
#include "font_ttf.h"



#include "tp_algo.h"
#include "gui_fb.h"
static void gui_scroll_text_read_scope(gui_text_t *text, gui_text_rect_t *rect)
{
    text->scope = 0;
    gui_obj_t *o = (gui_obj_t *)text;
    gui_win_t *win_scope = NULL;
    while (o->parent != NULL)
    {
        o = o->parent;
        if (o->type == WINDOW && GUI_TYPE(gui_win_t, o)->scope)
        {
            text->scope = 1;
            win_scope = (void *)o;
            break;
        }
    }
    if (text->scope && win_scope != NULL)
    {
        int scopex = win_scope->base.matrix->m[0][2];
        int scopey = win_scope->base.matrix->m[1][2];
        int scopew = win_scope->base.matrix->m[0][0] * win_scope->base.w;
        int scopeh = win_scope->base.matrix->m[1][1] * win_scope->base.h;

        rect->xboundleft = _UI_MAX(scopex, rect->xboundleft);
        rect->xboundright = _UI_MIN(scopex + scopew - 1, rect->xboundright);
        rect->yboundtop = _UI_MAX(scopey, rect->yboundtop);
        rect->yboundbottom = _UI_MIN(scopey + scopeh - 1, rect->yboundbottom);
    }
}





static void gui_text_font_load(gui_text_t *text, gui_text_rect_t *rect)
{
    extern void gui_text_font_load_global(gui_text_t *text, gui_text_rect_t *rect);
    gui_text_font_load_global(text, rect);
}

static void gui_text_font_draw(gui_text_t *text, gui_text_rect_t *rect)
{
    extern void gui_text_font_draw_global(gui_text_t *text, gui_text_rect_t *rect);
    gui_text_font_draw_global(text, rect);
}

static void gui_text_font_unload(gui_text_t *text)
{
    extern void gui_text_font_unload_global(gui_text_t *text);
    gui_text_font_unload_global(text);
}
static void gui_text_draw(gui_obj_t *obj)
{
    gui_text_t *text = (gui_text_t *)obj;
    struct gui_dispdev *dc;
    gui_text_rect_t draw_rect = {0};
    uint32_t total_section_count = UINT32_MAX;

    if (text->len == 0)
    {
        text->content_refresh = false;
        text->layout_refresh = false;
        return;
    }

    dc = gui_get_dc();
    draw_rect.x1 = text->offset_x;
    draw_rect.y1 = text->offset_y;
    draw_rect.x2 = draw_rect.x1 + obj->w - 1;
    draw_rect.y2 = draw_rect.y1 + obj->h - 1;
    draw_rect.xboundleft = draw_rect.x1;
    draw_rect.xboundright = draw_rect.x2;
    draw_rect.yboundtop = draw_rect.y1;
    draw_rect.yboundbottom = draw_rect.y2;
    gui_scroll_text_read_scope((gui_text_t *)text, &draw_rect);

    if (dc->section_count == 0)
    {
        gui_text_font_load(text, &draw_rect);
    }
    if (text->font_type == GUI_FONT_SRC_TTF)
    {
        gui_font_ttf_adapt_rect(text, &draw_rect);
    }
    if (text->font_type == GUI_FONT_SRC_MAT && text->use_img_blit)
    {
        gui_font_ttf_adapt_rect(text, &draw_rect);
    }

    if (dc->pfb_type == PFB_X_DIRECTION)
    {
        total_section_count = dc->screen_width / dc->fb_width -
                              ((dc->screen_width % dc->fb_width) ? 0 : 1);
        if (!(draw_rect.x1 >= (int)(dc->section_count + 1)*dc->fb_width || \
              draw_rect.x2 < (int)(dc->section_count)*dc->fb_width))
        {
            gui_text_font_draw(text, &draw_rect);
        }
    }
    else if (dc->pfb_type == PFB_Y_DIRECTION)
    {
        total_section_count = dc->screen_height / dc->fb_height -
                              ((dc->screen_height % dc->fb_height) ? 0 : 1);
        if (!(draw_rect.y1 >= (int)(dc->section_count + 1)*dc->fb_height || \
              draw_rect.y2 < (int)(dc->section_count)*dc->fb_height))
        {
            gui_text_font_draw(text, &draw_rect);
        }
    }

    if (dc->section_count == total_section_count)
    {
        gui_text_font_unload(text);
    }
}


static void gui_xml_dom_text_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        extern void gui_text_input_prepare_global(gui_obj_t *obj);

        extern void gui_text_prepare_global(gui_obj_t *obj);

        extern void gui_text_end_global(gui_obj_t *obj);

        extern void gui_text_destroy_global(gui_obj_t *obj);

        switch (cb_type)
        {
        case OBJ_INPUT_PREPARE:
            gui_text_input_prepare_global(obj);
            break;
        case OBJ_PREPARE:
            gui_text_prepare_global(obj);
            break;

        case OBJ_DRAW:
            gui_text_draw(obj);
            break;

        case OBJ_END:
            gui_text_end_global(obj);
            break;

        case OBJ_DESTROY:
            gui_text_destroy_global(obj);
            break;

        default:
            break;
        }
    }
}

static GUI_EVENT_CALLBACK_FUNCTION_DEFINE(page_list_wifi_cb)
{

}
static GUI_ANIMATION_CALLBACK_FUNCTION_DEFINE(page_list_animation_wifi_name)
{
    if (!animate->Beginning_frame)
    {
        return;
    }
    int count = 0;
    for (size_t i = 0; i < XML_DOM_WIFI_COUINT; i++)
    {
        if (wifi_array[i].name)
        {
            count++;
        }

    }
    gui_event_cb_t *func_array = gui_malloc(sizeof(gui_event_cb_t) * (count + 1));
    const char **item_text_array = gui_malloc(sizeof(char *) * (count + 1));
    struct on_click_jump_cb_param **click_param = gui_malloc(sizeof(void *) * (count + 1));
    gui_pagelist_new_t *pl = p;
    for (size_t i = 0; i < count; i++)
    {
        item_text_array[i] = wifi_array[i].name;
        func_array[i] = (void *)on_click_jump_cb_wifi;

        if (pl->click_param)
        {
            click_param[i] = gui_malloc(sizeof(struct on_click_jump_cb_param));
            memcpy(click_param[i], pl->click_param[0], sizeof(struct on_click_jump_cb_param));
            click_param[i]->id = i;
        }


    }
    pl->click_param = (void *)click_param;



    xml_dom_page_list_new_render_param(p, count, func_array, item_text_array, (void *)click_param);



}

typedef struct keyboard_code_map
{
    const char *keyboard;
    scan_code code;
} keyboard_code_map_t;
keyboard_code_map_t keyboard_map[] =
{
    {"K_a", SCANCODE_A},
    {"K_b", SCANCODE_B},
    {"K_c", SCANCODE_C},
    {"K_d", SCANCODE_D},
    {"K_e", SCANCODE_E},
    {"K_f", SCANCODE_F},
    {"K_g", SCANCODE_G},
    {"K_h", SCANCODE_H},
    {"K_i", SCANCODE_I},
    {"K_j", SCANCODE_J},
    {"K_k", SCANCODE_K},
    {"K_l", SCANCODE_L},
    {"K_m", SCANCODE_M},
    {"K_n", SCANCODE_N},
    {"K_o", SCANCODE_O},
    {"K_p", SCANCODE_P},
    {"K_q", SCANCODE_Q},
    {"K_r", SCANCODE_R},
    {"K_s", SCANCODE_S},
    {"K_t", SCANCODE_T},
    {"K_u", SCANCODE_U},
    {"K_v", SCANCODE_V},
    {"K_w", SCANCODE_W},
    {"K_x", SCANCODE_X},
    {"K_y", SCANCODE_Y},
    {"K_z", SCANCODE_Z},
    {"K_0", SCANCODE_0},
    {"K_1", SCANCODE_1},
    {"K_2", SCANCODE_2},
    {"K_3", SCANCODE_3},
    {"K_4", SCANCODE_4},
    {"K_5", SCANCODE_5},
    {"K_6", SCANCODE_6},
    {"K_7", SCANCODE_7},
    {"K_8", SCANCODE_8},
    {"K_9", SCANCODE_9},
    //{"K_RETURN", SCANCODE_RETURN},
    //{"K_ESCAPE", SCANCODE_ESCAPE},
    {"K_BACKSPACE", SCANCODE_BACKSPACE},
    {"K_TAB", SCANCODE_TAB},
    {"K_SPACE", SCANCODE_SPACE},
    {"K_MINUS", SCANCODE_MINUS},
    {"K_EQUALS", SCANCODE_EQUALS},
    {"K_LEFTBRACKET", SCANCODE_LEFTBRACKET},
    {"K_RIGHTBRACKET", SCANCODE_RIGHTBRACKET},
    {"K_BACKSLASH", SCANCODE_BACKSLASH},
    {"K_SEMICOLON", SCANCODE_SEMICOLON},
    {"K_APOSTROPHE", SCANCODE_APOSTROPHE},
    {"K_GRAVE", SCANCODE_GRAVE},
    {"K_COMMA", SCANCODE_COMMA},
    {"K_PERIOD", SCANCODE_PERIOD},
    {"K_SLASH", SCANCODE_SLASH},
    {"K_CAPSLOCK", SCANCODE_CAPSLOCK},
    {"K_F1", SCANCODE_F1},
    {"K_F2", SCANCODE_F2},
    {"K_F3", SCANCODE_F3},
    {"K_F4", SCANCODE_F4},
    {"K_F5", SCANCODE_F5},
    {"K_F6", SCANCODE_F6},
    {"K_F7", SCANCODE_F7},
    {"K_F8", SCANCODE_F8},
    {"K_F9", SCANCODE_F9},
    {"K_F10", SCANCODE_F10},
    {"K_F11", SCANCODE_F11},
    {"K_F12", SCANCODE_F12},
    //{"K_PRINTSCREEN", SCANCODE_PRINTSCREEN},
    //{"K_SCROLLLOCK", SCANCODE_SCROLLLOCK},
    //{"K_PAUSE", SCANCODE_PAUSE},
    //{"K_INSERT", SCANCODE_INSERT},
    //{"K_HOME", SCANCODE_HOME},
    // {"K_PAGEUP", SCANCODE_PAGEUP},
    // {"K_DELETE", SCANCODE_DELETE},
    // {"K_END", SCANCODE_END},
    // {"K_PAGEDOWN", SCANCODE_PAGEDOWN},
    {"K_RIGHT", SCANCODE_RIGHT},
    {"K_LEFT", SCANCODE_LEFT},
    {"K_DOWN", SCANCODE_DOWN},
    {"K_UP", SCANCODE_UP},
    // {"K_NUMLOCKCLEAR", SCANCODE_NUMLOCKCLEAR},
    // {"K_KP_DIVIDE", SCANCODE_KP_DIVIDE},
    // {"K_KP_MULTIPLY", SCANCODE_KP_MULTIPLY},
    // {"K_KP_MINUS", SCANCODE_KP_MINUS},
    // {"K_KP_PLUS", SCANCODE_KP_PLUS},
    // {"K_KP_ENTER", SCANCODE_KP_ENTER},
    // {"K_KP_1", SCANCODE_KP_1},
    // {"K_KP_2", SCANCODE_KP_2},
    // {"K_KP_3", SCANCODE_KP_3},
    // {"K_KP_4", SCANCODE_KP_4},
    // {"K_KP_5", SCANCODE_KP_5},
    // {"K_KP_6", SCANCODE_KP_6},
    // {"K_KP_7", SCANCODE_KP_7},
    // {"K_KP_8", SCANCODE_KP_8},
    // {"K_KP_9", SCANCODE_KP_9},
    // {"K_KP_0", SCANCODE_KP_0},
    // {"K_KP_PERIOD", SCANCODE_KP_PERIOD},
    // {"K_APPLICATION", SCANCODE_APPLICATION},
    // {"K_POWER", SCANCODE_POWER},
    // {"K_KP_EQUALS", SCANCODE_KP_EQUALS},
    // {"K_F13", SCANCODE_F13},
    // {"K_F14", SCANCODE_F14},
    // {"K_F15", SCANCODE_F15},
    // {"K_F16", SCANCODE_F16},
    // {"K_F17", SCANCODE_F17},
    // {"K_F18", SCANCODE_F18},
    // {"K_F19", SCANCODE_F19},
    // {"K_F20", SCANCODE_F20},
    // {"K_F21", SCANCODE_F21},
    // {"K_F22", SCANCODE_F22},
    // {"K_F23", SCANCODE_F23},
    // {"K_F24", SCANCODE_F24},
    // {"K_EXECUTE", SCANCODE_EXECUTE},
    // {"K_HELP", SCANCODE_HELP},
    // {"K_MENU", SCANCODE_MENU},
    // {"K_SELECT", SCANCODE_SELECT},
    // {"K_STOP", SCANCODE_STOP},
    // {"K_AGAIN", SCANCODE_AGAIN},
    // {"K_UNDO", SCANCODE_UNDO},
    // {"K_CUT", SCANCODE_CUT},
    // {"K_COPY", SCANCODE_COPY},
    // {"K_PASTE", SCANCODE_PASTE},
    // {"K_FIND", SCANCODE_FIND},
    // {"K_MUTE", SCANCODE_MUTE},
    // {"K_VOLUMEUP", SCANCODE_VOLUMEUP},
    // {"K_VOLUMEDOWN", SCANCODE_VOLUMEDOWN},
    // {"K_KP_COMMA", SCANCODE_KP_COMMA},
    // {"K_KP_EQUALSAS400", SCANCODE_KP_EQUALSAS400},
    // {"K_INTERNATIONAL1", SCANCODE_INTERNATIONAL1},
    // {"K_INTERNATIONAL2", SCANCODE_INTERNATIONAL2},
    // {"K_INTERNATIONAL3", SCANCODE_INTERNATIONAL3},
    // {"K_INTERNATIONAL4", SCANCODE_INTERNATIONAL4},
    // {"K_INTERNATIONAL5", SCANCODE_INTERNATIONAL5},
    // {"K_INTERNATIONAL6", SCANCODE_INTERNATIONAL6},
    // {"K_INTERNATIONAL7", SCANCODE_INTERNATIONAL7},
    // {"K_INTERNATIONAL8", SCANCODE_INTERNATIONAL8},
    // {"K_INTERNATIONAL9", SCANCODE_INTERNATIONAL9},
    // {"K_LANG1", SCANCODE_LANG1},
    // {"K_LANG2", SCANCODE_LANG2},
    // {"K_LANG3", SCANCODE_LANG3},
    // {"K_LANG4", SCANCODE_LANG4},
    // {"K_LANG5", SCANCODE_LANG5},
    // {"K_LANG6", SCANCODE_LANG6},
    // {"K_LANG7", SCANCODE_LANG7},
    // {"K_LANG8", SCANCODE_LANG8},
    // {"K_LANG9", SCANCODE_LANG9},
    // {"K_ALTERASE", SCANCODE_ALTERASE},
    // {"K_SYSREQ", SCANCODE_SYSREQ},
    // {"K_CANCEL", SCANCODE_CANCEL},
    // {"K_CLEAR", SCANCODE_CLEAR},
    // {"K_PRIOR", SCANCODE_PRIOR},
    // {"K_RETURN2", SCANCODE_RETURN2},
    // {"K_SEPARATOR", SCANCODE_SEPARATOR},
    // {"K_OUT", SCANCODE_OUT},
    // {"K_OPER", SCANCODE_OPER},
    // {"K_CLEARAGAIN", SCANCODE_CLEARAGAIN},
    // {"K_CRSEL", SCANCODE_CRSEL},
    // {"K_EXSEL", SCANCODE_EXSEL},
    // {"K_KP_00", SCANCODE_KP_00},
    // {"K_KP_000", SCANCODE_KP_000},
    // {"K_THOUSANDSSEPARATOR", SCANCODE_THOUSANDSSEPARATOR},
    // {"K_DECIMALSEPARATOR", SCANCODE_DECIMALSEPARATOR},
    // {"K_CURRENCYUNIT", SCANCODE_CURRENCYUNIT},
    // {"K_CURRENCYSUBUNIT", SCANCODE_CURRENCYSUBUNIT},
    // {"K_KP_LEFTPAREN", SCANCODE_KP_LEFTPAREN},
    // {"K_KP_RIGHTPAREN", SCANCODE_KP_RIGHTPAREN},
    // {"K_KP_LEFTBRACE", SCANCODE_KP_LEFTBRACE},
    // {"K_KP_RIGHTBRACE", SCANCODE_KP_RIGHTBRACE},
    // {"K_KP_TAB", SCANCODE_KP_TAB},
    // {"K_KP_BACKSPACE", SCANCODE_KP_BACKSPACE},
    // {"K_KP_A", SCANCODE_KP_A},
    // {"K_KP_B", SCANCODE_KP_B},
    // {"K_KP_C", SCANCODE_KP_C},
    // {"K_KP_D", SCANCODE_KP_D},
    // {"K_KP_E", SCANCODE_KP_E},
    // {"K_KP_F", SCANCODE_KP_F},
    // {"K_KP_XOR", SCANCODE_KP_XOR},
    // {"K_KP_POWER", SCANCODE_KP_POWER},
    // {"K_KP_PERCENT", SCANCODE_KP_PERCENT},
    // {"K_KP_LESS", SCANCODE_KP_LESS},
    // {"K_KP_GREATER", SCANCODE_KP_GREATER},
    // {"K_KP_AMPERSAND", SCANCODE_KP_AMPERSAND},
    // {"K_KP_DBLAMPERSAND", SCANCODE_KP_DBLAMPERSAND},
    // {"K_KP_VERTICALBAR", SCANCODE_KP_VERTICALBAR},
    // {"K_KP_DBLVERTICALBAR", SCANCODE_KP_DBLVERTICALBAR},
    // {"K_KP_COLON", SCANCODE_KP_COLON},
    // {"K_KP_HASH", SCANCODE_KP_HASH},
    // {"K_KP_SPACE", SCANCODE_KP_SPACE},
    // {"K_KP_AT", SCANCODE_KP_AT},
    // {"K_KP_EXCLAM", SCANCODE_KP_EXCLAM},
    // {"K_KP_MEMSTORE", SCANCODE_KP_MEMSTORE},
    // {"K_KP_MEMRECALL", SCANCODE_KP_MEMRECALL},
    // {"K_KP_MEMCLEAR", SCANCODE_KP_MEMCLEAR},
    // {"K_KP_MEMADD", SCANCODE_KP_MEMADD},
    // {"K_KP_MEMSUBTRACT", SCANCODE_KP_MEMSUBTRACT},
    // {"K_KP_MEMMULTIPLY", SCANCODE_KP_MEMMULTIPLY},
    // {"K_KP_MEMDIVIDE", SCANCODE_KP_MEMDIVIDE},
    // {"K_KP_PLUSMINUS", SCANCODE_KP_PLUSMINUS},
    // {"K_KP_CLEAR", SCANCODE_KP_CLEAR},
    // {"K_KP_CLEARENTRY", SCANCODE_KP_CLEARENTRY},
    // {"K_KP_BINARY", SCANCODE_KP_BINARY},
    // {"K_KP_OCTAL", SCANCODE_KP_OCTAL},
    // {"K_KP_DECIMAL", SCANCODE_KP_DECIMAL},
    // {"K_KP_HEXADECIMAL", SCANCODE_KP_HEXADECIMAL},
    // {"K_LCTRL", SCANCODE_LCTRL},
    {"K_LSHIFT", SCANCODE_LSHIFT},
    // {"K_LALT", SCANCODE_LALT},
    // {"K_LGUI", SCANCODE_LGUI},
    // {"K_RCTRL", SCANCODE_RCTRL},
    {"K_RSHIFT", SCANCODE_RSHIFT},
    // {"K_RALT", SCANCODE_RALT},
    // {"K_RGUI", SCANCODE_RGUI},
    // {"K_MODE", SCANCODE_MODE},
    // {"K_AUDIONEXT", SCANCODE_AUDIONEXT},
    // {"K_AUDIOPREV", SCANCODE_AUDIOPREV},
    // {"K_AUDIOSTOP", SCANCODE_AUDIOSTOP},
    // {"K_AUDIOPLAY", SCANCODE_AUDIOPLAY},
    // {"K_AUDIOMUTE", SCANCODE_AUDIOMUTE},
    // {"K_MEDIASELECT", SCANCODE_MEDIASELECT},
    // {"K_WWW", SCANCODE_WWW},
    // {"K_MAIL", SCANCODE_MAIL},
    // {"K_CALCULATOR", SCANCODE_CALCULATOR},
    // {"K_COMPUTER", SCANCODE_COMPUTER},
    // {"K_AC_SEARCH", SCANCODE_AC_SEARCH},
    // {"K_AC_HOME", SCANCODE_AC_HOME},
    // {"K_AC_BACK", SCANCODE_AC_BACK},
    // {"K_AC_FORWARD", SCANCODE_AC_FORWARD},
    // {"K_AC_STOP", SCANCODE_AC_STOP},
    // {"K_AC_REFRESH", SCANCODE_AC_REFRESH},
    // {"K_AC_BOOKMARKS", SCANCODE_AC_BOOKMARKS},
    // {"K_BRIGHTNESSDOWN", SCANCODE_BRIGHTNESSDOWN},
    // {"K_BRIGHTNESSUP", SCANCODE_BRIGHTNESSUP},
    // {"K_DISPLAYSWITCH", SCANCODE_DISPLAYSWITCH},
    // {"K_KBDILLUMTOGGLE", SCANCODE_KBDILLUMTOGGLE},
    // {"K_KBDILLUMDOWN", SCANCODE_KBDILLUMDOWN},
    // {"K_KBDILLUMUP", SCANCODE_KBDILLUMUP},
    // {"K_EJECT", SCANCODE_EJECT},
    // {"K_SLEEP", SCANCODE_SLEEP},
    // {"K_APP1", SCANCODE_APP1},
    // {"K_APP2", SCANCODE_APP2},
    // {"K_AUDIOREWIND", SCANCODE_AUDIOREWIND},
    // {"K_AUDIOFASTFORWARD", SCANCODE_AUDIOFASTFORWARD},
    // {"K_SOFTLEFT", SCANCODE_SOFTLEFT},
    // {"K_SOFTRIGHT", SCANCODE_SOFTRIGHT},
    // {"K_CALL", SCANCODE_CALL},
    // {"K_ENDCALL", SCANCODE_ENDCALL}


}
;
scan_code get_scan_code_by_keyboard_string(const char *keyboard)
{
    for (size_t i = 0; i < GUI_ARRAY_SIZE(keyboard_map); i++)
    {
        if (GUI_STRINGS_EQUAL(keyboard_map[i].keyboard, keyboard))
        {
            return keyboard_map[i].code;
        }
    }
    return SCANCODE_UNKNOWN;
}













