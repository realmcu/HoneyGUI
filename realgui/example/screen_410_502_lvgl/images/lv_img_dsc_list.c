#include "lvgl.h"
#include "lv_img_dsc_list.h"
#include "lv_image_dsc.h"
#include "lv_color.h"
#include "root_image_lvgl/ui_resource.h"

const lv_image_dsc_t control_lte_off =
{
    .header.w = 182,
    .header.h = 121,
    .data_size = 88088,
    .header.cf = 0x10,
    .data = CONTROL_LTE_OFF_BIN + 8
};

const lv_image_dsc_t control_lte_on =
{
    .header.w = 182,
    .header.h = 121,
    .data_size = 88088,
    .header.cf = 0x10,
    .data = CONTROL_LTE_ON_BIN + 8
};

const lv_image_dsc_t control_mute_off =
{
    .header.w = 182,
    .header.h = 121,
    .data_size = 88088,
    .header.cf = 0x10,
    .data = CONTROL_MUTE_OFF_BIN + 8
};

const lv_image_dsc_t control_mute_on =
{
    .header.w = 182,
    .header.h = 121,
    .data_size = 88088,
    .header.cf = 0x10,
    .data = CONTROL_MUTE_ON_BIN + 8
};

const lv_image_dsc_t control_phone_off =
{
    .header.w = 182,
    .header.h = 121,
    .data_size = 88088,
    .header.cf = 0x10,
    .data = CONTROL_PHONE_OFF_BIN + 8
};

const lv_image_dsc_t control_phone_on =
{
    .header.w = 182,
    .header.h = 121,
    .data_size = 88088,
    .header.cf = 0x10,
    .data = CONTROL_PHONE_ON_BIN + 8
};

const lv_image_dsc_t fruit_ninja_banana =
{
    .header.w = 126,
    .header.h = 50,
    .data_size = 25200,
    .header.cf = 0x10,
    .data = FRUIT_NINJA_BANANA_BIN + 8
};

const lv_image_dsc_t fruit_ninja_banana_half_1 =
{
    .header.w = 126,
    .header.h = 50,
    .data_size = 25200,
    .header.cf = 0x10,
    .data = FRUIT_NINJA_BANANA_HALF_1_BIN + 8
};

const lv_image_dsc_t fruit_ninja_banana_half_2 =
{
    .header.w = 126,
    .header.h = 50,
    .data_size = 25200,
    .header.cf = 0x10,
    .data = FRUIT_NINJA_BANANA_HALF_2_BIN + 8
};

const lv_image_dsc_t fruit_ninja_bg =
{
    .header.w = 410,
    .header.h = 502,
    .data_size = 823280,
    .header.cf = 0x10,
    .data = FRUIT_NINJA_BG_BIN + 8
};

const lv_image_dsc_t fruit_ninja_bomb =
{
    .header.w = 97,
    .header.h = 106,
    .data_size = 41128,
    .header.cf = 0x10,
    .data = FRUIT_NINJA_BOMB_BIN + 8
};

const lv_image_dsc_t fruit_ninja_peach =
{
    .header.w = 62,
    .header.h = 59,
    .data_size = 14632,
    .header.cf = 0x10,
    .data = FRUIT_NINJA_PEACH_BIN + 8
};

const lv_image_dsc_t fruit_ninja_peach_half_1 =
{
    .header.w = 62,
    .header.h = 59,
    .data_size = 14632,
    .header.cf = 0x10,
    .data = FRUIT_NINJA_PEACH_HALF_1_BIN + 8
};

const lv_image_dsc_t fruit_ninja_peach_half_2 =
{
    .header.w = 62,
    .header.h = 59,
    .data_size = 14632,
    .header.cf = 0x10,
    .data = FRUIT_NINJA_PEACH_HALF_2_BIN + 8
};

const lv_image_dsc_t fruit_ninja_strawberry =
{
    .header.w = 68,
    .header.h = 72,
    .data_size = 19584,
    .header.cf = 0x10,
    .data = FRUIT_NINJA_STRAWBERRY_BIN + 8
};

const lv_image_dsc_t fruit_ninja_strawberry_half_1 =
{
    .header.w = 68,
    .header.h = 72,
    .data_size = 19584,
    .header.cf = 0x10,
    .data = FRUIT_NINJA_STRAWBERRY_HALF_1_BIN + 8
};

const lv_image_dsc_t fruit_ninja_strawberry_half_2 =
{
    .header.w = 68,
    .header.h = 72,
    .data_size = 19584,
    .header.cf = 0x10,
    .data = FRUIT_NINJA_STRAWBERRY_HALF_2_BIN + 8
};

const lv_image_dsc_t fruit_ninja_watermelon =
{
    .header.w = 98,
    .header.h = 85,
    .data_size = 33320,
    .header.cf = 0x10,
    .data = FRUIT_NINJA_WATERMELON_BIN + 8
};

const lv_image_dsc_t fruit_ninja_watermelon_half_1 =
{
    .header.w = 98,
    .header.h = 85,
    .data_size = 33320,
    .header.cf = 0x10,
    .data = FRUIT_NINJA_WATERMELON_HALF_1_BIN + 8
};

const lv_image_dsc_t fruit_ninja_watermelon_half_2 =
{
    .header.w = 98,
    .header.h = 85,
    .data_size = 33320,
    .header.cf = 0x10,
    .data = FRUIT_NINJA_WATERMELON_HALF_2_BIN + 8
};

const lv_image_dsc_t w1slice1 =
{
    .header.w = 410,
    .header.h = 502,
    .data_size = 823280,
    .header.cf = 0x10,
    .data = W1SLICE1_BIN + 8
};

const lv_image_dsc_t ui_arrow_down =
{
    .header.w = 54,
    .header.h = 19,
    .data_size = 4104,
    .header.cf = 0x10,
    .data = UI_ARROW_DOWN_BIN + 8
};

const lv_image_dsc_t ui_arrow_up =
{
    .header.w = 54,
    .header.h = 19,
    .data_size = 4104,
    .header.cf = 0x10,
    .data = UI_ARROW_UP_BIN + 8
};

const lv_image_dsc_t ui_bg_icon =
{
    .header.w = 100,
    .header.h = 100,
    .data_size = 40000,
    .header.cf = 0x10,
    .data = UI_BG_ICON_BIN + 8
};

const lv_image_dsc_t ui_card_activity =
{
    .header.w = 392,
    .header.h = 157,
    .data_size = 246176,
    .header.cf = 0x10,
    .data = UI_CARD_ACTIVITY_BIN + 8
};

const lv_image_dsc_t ui_card_app =
{
    .header.w = 352,
    .header.h = 157,
    .data_size = 221056,
    .header.cf = 0x10,
    .data = UI_CARD_APP_BIN + 8
};

const lv_image_dsc_t ui_card_appview =
{
    .header.w = 296,
    .header.h = 76,
    .data_size = 89984,
    .header.cf = 0x10,
    .data = UI_CARD_APPVIEW_BIN + 8
};

const lv_image_dsc_t ui_card_calendar =
{
    .header.w = 352,
    .header.h = 157,
    .data_size = 221056,
    .header.cf = 0x10,
    .data = UI_CARD_CALENDAR_BIN + 8
};

const lv_image_dsc_t ui_card_clockcircle =
{
    .header.w = 334,
    .header.h = 177,
    .data_size = 236472,
    .header.cf = 0x10,
    .data = UI_CARD_CLOCKCIRCLE_BIN + 8
};

const lv_image_dsc_t ui_card_weather =
{
    .header.w = 352,
    .header.h = 157,
    .data_size = 221056,
    .header.cf = 0x10,
    .data = UI_CARD_WEATHER_BIN + 8
};

const lv_image_dsc_t ui_clock_activity_icon =
{
    .header.w = 100,
    .header.h = 100,
    .data_size = 40000,
    .header.cf = 0x10,
    .data = UI_CLOCK_ACTIVITY_ICON_BIN + 8
};

const lv_image_dsc_t ui_clock_box2d_ring_icon =
{
    .header.w = 100,
    .header.h = 100,
    .data_size = 40000,
    .header.cf = 0x10,
    .data = UI_CLOCK_BOX2D_RING_ICON_BIN + 8
};

const lv_image_dsc_t ui_clock_card_alarm =
{
    .header.w = 331,
    .header.h = 148,
    .data_size = 195952,
    .header.cf = 0x10,
    .data = UI_CLOCK_CARD_ALARM_BIN + 8
};

const lv_image_dsc_t ui_clock_card_compass =
{
    .header.w = 328,
    .header.h = 148,
    .data_size = 194176,
    .header.cf = 0x10,
    .data = UI_CLOCK_CARD_COMPASS_BIN + 8
};

const lv_image_dsc_t ui_clock_card_music =
{
    .header.w = 329,
    .header.h = 148,
    .data_size = 194768,
    .header.cf = 0x10,
    .data = UI_CLOCK_CARD_MUSIC_BIN + 8
};

const lv_image_dsc_t ui_clock_card_weather =
{
    .header.w = 328,
    .header.h = 147,
    .data_size = 192864,
    .header.cf = 0x10,
    .data = UI_CLOCK_CARD_WEATHER_BIN + 8
};

const lv_image_dsc_t ui_clock_card_workout =
{
    .header.w = 328,
    .header.h = 148,
    .data_size = 194176,
    .header.cf = 0x10,
    .data = UI_CLOCK_CARD_WORKOUT_BIN + 8
};

const lv_image_dsc_t ui_clock_compass_dial_icon =
{
    .header.w = 100,
    .header.h = 100,
    .data_size = 40000,
    .header.cf = 0x10,
    .data = UI_CLOCK_COMPASS_DIAL_ICON_BIN + 8
};

const lv_image_dsc_t ui_clock_compass_pointer_icon =
{
    .header.w = 14,
    .header.h = 10,
    .data_size = 560,
    .header.cf = 0x10,
    .data = UI_CLOCK_COMPASS_POINTER_ICON_BIN + 8
};

const lv_image_dsc_t ui_clock_face_main =
{
    .header.w = 417,
    .header.h = 472,
    .data_size = 787296,
    .header.cf = 0x10,
    .data = UI_CLOCK_FACE_MAIN_BIN + 8
};

const lv_image_dsc_t ui_clock_fruit_ninja_icon =
{
    .header.w = 100,
    .header.h = 100,
    .data_size = 40000,
    .header.cf = 0x10,
    .data = UI_CLOCK_FRUIT_NINJA_ICON_BIN + 8
};

const lv_image_dsc_t ui_clock_heartrate_icon =
{
    .header.w = 100,
    .header.h = 100,
    .data_size = 40000,
    .header.cf = 0x10,
    .data = UI_CLOCK_HEARTRATE_ICON_BIN + 8
};

const lv_image_dsc_t ui_clock_music_icon =
{
    .header.w = 100,
    .header.h = 100,
    .data_size = 40000,
    .header.cf = 0x10,
    .data = UI_CLOCK_MUSIC_ICON_BIN + 8
};

const lv_image_dsc_t ui_iwatch_icon =
{
    .header.w = 28,
    .header.h = 35,
    .data_size = 3920,
    .header.cf = 0x10,
    .data = UI_IWATCH_ICON_BIN + 8
};

const lv_image_dsc_t ui_line_still =
{
    .header.w = 48,
    .header.h = 6,
    .data_size = 1152,
    .header.cf = 0x10,
    .data = UI_LINE_STILL_BIN + 8
};

const lv_image_dsc_t ui_message_icon =
{
    .header.w = 56,
    .header.h = 58,
    .data_size = 12992,
    .header.cf = 0x10,
    .data = UI_MESSAGE_ICON_BIN + 8
};

const lv_image_dsc_t ui_person_icon =
{
    .header.w = 20,
    .header.h = 20,
    .data_size = 1600,
    .header.cf = 0x10,
    .data = UI_PERSON_ICON_BIN + 8
};

const lv_image_dsc_t ui_text_0 =
{
    .header.w = 33,
    .header.h = 55,
    .data_size = 7260,
    .header.cf = 0x10,
    .data = UI_TEXT_0_BIN + 8
};

const lv_image_dsc_t ui_text_1 =
{
    .header.w = 29,
    .header.h = 53,
    .data_size = 6148,
    .header.cf = 0x10,
    .data = UI_TEXT_1_BIN + 8
};

const lv_image_dsc_t ui_text_2 =
{
    .header.w = 34,
    .header.h = 54,
    .data_size = 7344,
    .header.cf = 0x10,
    .data = UI_TEXT_2_BIN + 8
};

const lv_image_dsc_t ui_text_3 =
{
    .header.w = 34,
    .header.h = 55,
    .data_size = 7480,
    .header.cf = 0x10,
    .data = UI_TEXT_3_BIN + 8
};

const lv_image_dsc_t ui_text_4 =
{
    .header.w = 37,
    .header.h = 53,
    .data_size = 7844,
    .header.cf = 0x10,
    .data = UI_TEXT_4_BIN + 8
};

const lv_image_dsc_t ui_text_5 =
{
    .header.w = 35,
    .header.h = 54,
    .data_size = 7560,
    .header.cf = 0x10,
    .data = UI_TEXT_5_BIN + 8
};

const lv_image_dsc_t ui_text_6 =
{
    .header.w = 33,
    .header.h = 55,
    .data_size = 7260,
    .header.cf = 0x10,
    .data = UI_TEXT_6_BIN + 8
};

const lv_image_dsc_t ui_text_7 =
{
    .header.w = 34,
    .header.h = 53,
    .data_size = 7208,
    .header.cf = 0x10,
    .data = UI_TEXT_7_BIN + 8
};

const lv_image_dsc_t ui_text_8 =
{
    .header.w = 34,
    .header.h = 55,
    .data_size = 7480,
    .header.cf = 0x10,
    .data = UI_TEXT_8_BIN + 8
};

const lv_image_dsc_t ui_text_9 =
{
    .header.w = 33,
    .header.h = 55,
    .data_size = 7260,
    .header.cf = 0x10,
    .data = UI_TEXT_9_BIN + 8
};

const lv_image_dsc_t ui_text_colon =
{
    .header.w = 10,
    .header.h = 40,
    .data_size = 1600,
    .header.cf = 0x10,
    .data = UI_TEXT_COLON_BIN + 8
};

const lv_image_dsc_t ui_text_percent =
{
    .header.w = 61,
    .header.h = 55,
    .data_size = 13420,
    .header.cf = 0x10,
    .data = UI_TEXT_PERCENT_BIN + 8
};

const lv_image_dsc_t ui_weather_cloudy =
{
    .header.w = 39,
    .header.h = 28,
    .data_size = 4368,
    .header.cf = 0x10,
    .data = UI_WEATHER_CLOUDY_BIN + 8
};

const lv_image_dsc_t ui_weather_rain_l =
{
    .header.w = 31,
    .header.h = 31,
    .data_size = 3844,
    .header.cf = 0x10,
    .data = UI_WEATHER_RAIN_L_BIN + 8
};

const lv_image_dsc_t ui_weather_rain_m =
{
    .header.w = 31,
    .header.h = 31,
    .data_size = 3844,
    .header.cf = 0x10,
    .data = UI_WEATHER_RAIN_M_BIN + 8
};

const lv_image_dsc_t ui_weather_rain_s =
{
    .header.w = 31,
    .header.h = 31,
    .data_size = 3844,
    .header.cf = 0x10,
    .data = UI_WEATHER_RAIN_S_BIN + 8
};

const lv_image_dsc_t ui_weather_sunny =
{
    .header.w = 30,
    .header.h = 30,
    .data_size = 3600,
    .header.cf = 0x10,
    .data = UI_WEATHER_SUNNY_BIN + 8
};

const lv_image_dsc_t w2w =
{
    .header.w = 410,
    .header.h = 502,
    .data_size = 823280,
    .header.cf = 0x10,
    .data = W2W_BIN + 8
};

const lv_image_dsc_t w3w =
{
    .header.w = 410,
    .header.h = 502,
    .data_size = 823280,
    .header.cf = 0x10,
    .data = W3W_BIN + 8
};

const lv_image_dsc_t w5battery =
{
    .header.w = 45,
    .header.h = 53,
    .data_size = 9540,
    .header.cf = 0x10,
    .data = W5BATTERY_BIN + 8
};

const lv_image_dsc_t w5w =
{
    .header.w = 410,
    .header.h = 502,
    .data_size = 823280,
    .header.cf = 0x10,
    .data = W5W_BIN + 8
};

const lv_image_dsc_t watchface_butterfly =
{
    .header.w = 203,
    .header.h = 250,
    .data_size = 203000,
    .header.cf = 0x10,
    .data = WATCHFACE_BUTTERFLY_BIN + 8
};

const lv_image_dsc_t watchface_ring =
{
    .header.w = 205,
    .header.h = 250,
    .data_size = 205000,
    .header.cf = 0x10,
    .data = WATCHFACE_RING_BIN + 8
};

const lv_image_dsc_t watch_hour_hand =
{
    .header.w = 12,
    .header.h = 77,
    .data_size = 3696,
    .header.cf = 0x10,
    .data = WATCH_HOUR_HAND_BIN + 8
};

const lv_image_dsc_t watch_mask =
{
    .header.w = 12,
    .header.h = 12,
    .data_size = 576,
    .header.cf = 0x10,
    .data = WATCH_MASK_BIN + 8
};

const lv_image_dsc_t watch_minute_hand =
{
    .header.w = 12,
    .header.h = 153,
    .data_size = 7344,
    .header.cf = 0x10,
    .data = WATCH_MINUTE_HAND_BIN + 8
};

const lv_image_dsc_t watch_second_hand =
{
    .header.w = 1,
    .header.h = 140,
    .data_size = 560,
    .header.cf = 0x10,
    .data = WATCH_SECOND_HAND_BIN + 8
};