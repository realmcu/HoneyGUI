#ifndef __APP_CHARGEBOX_H__
#define __APP_CHARGEBOX_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <gui_app.h>
#include "gui_img.h"
#include "gui_text.h"
#include "root_image_chargebox/ui_resource.h"

#define WALLPAPER_NUM 3
#define MUSIC_TITLE_MAX_LENGTH            512
#define MUSIC_ARTIST_MAX_LENGTH           128
#define MUSIC_ALBUM_MAX_LENGTH            32
#define MUSIC_SCROLL_TEXT_DURATION        3*1000
#define APP_GUI_DEVICE_NAME_MAX_LENGTH    40
#define BATTERY_INVALID_VALUE             0x7F

typedef enum
{
    GUI_LISTENING_OFF,
    GUI_LISTENING_NORMAL_APT,
    GUI_LISTENING_ANC,
    GUI_LISTENING_LLAPT,
} T_APP_GUI_LISTENING_MODE;

typedef enum
{
    MUSIC_STOP,
    MUSIC_PLAYING,
    MUSIC_PAUSED,
} T_APP_GUI_PLAYER_STATUS;

typedef enum
{
    BLE_LINK_DISCONNECT,
    BLE_LINK_CONNECT,
} T_APP_GUI_LE_LINK_STATUS;

typedef enum
{
    AVRCP_TITLE = 1,
    AVRCP_NAME_OF_ARTIST,
    AVRCP_NAME_OF_ALBUM,
    AVRCP_NUMBER_OF_MEDIA,
    AVRCP_TOTAL_NUMBER_OF_MEDIA,
    AVRCP_GENRE,
    AVRCP_PLAYING_TIME_IN_MILLISECOND,
} APP_AVRCP_ATTRIBUTE_TYPE;

typedef enum
{
    GUI_BLE_SUBEVENT_SCAN,

    GUI_BLE_SUBEVENT_MUSIC_PLAY,
    GUI_BLE_SUBEVENT_MUSIC_PAUSE,
    GUI_BLE_SUBEVENT_MUSIC_STOP,
    GUI_BLE_SUBEVENT_MUSIC_BACKWARD,
    GUI_BLE_SUBEVENT_MUSIC_FORWARD,

    GUI_BLE_SUBEVENT_VOLUME_UP,
    GUI_BLE_SUBEVENT_VOLUME_DOWN,

    GUI_BLE_SUBEVENT_CALL_ANSWER,
    GUI_BLE_SUBEVENT_CALL_REJECT,
    GUI_BLE_SUBEVENT_CALL_END,

    GUI_BLE_SUBEVENT_MIC_MUTE,
    GUI_BLE_SUBEVENT_MIC_UNMUTE,

    GUI_BLE_SUBEVENT_LISTEN_OFF,
    GUI_BLE_SUBEVENT_LISTEN_APT,
    GUI_BLE_SUBEVENT_LISTEN_ANC,
    GUI_BLE_SUBEVENT_LISTEN_LLAPT,

    GUI_BLE_SUBEVENT_EQ_NORMAL,
    GUI_BLE_SUBEVENT_EQ_GAMING,

    GUI_BLE_SUBEVENT_FINDTWS_LEFT,
    GUI_BLE_SUBEVENT_FINDTWS_RIGHT,
} T_APP_GUI_SUBEVENT_TYPE;

static void app_chargebox_ui_design(gui_app_t *app);
void app_gui_ble_action_cb(void *obj, gui_event_t e, void *param);

extern void *get_app_chargebox(void);
extern void page_tb_conn(void *parent);
extern void page_tb_music(void *parent);
extern void page_tb_volume(void *parent);
extern void page_tb_call(void *parent);
extern void page_tb_anc(void *parent);
extern void page_tb_equalizer(void *parent);
extern void page_tb_clock(void *parent);
extern void page_tb_brightness(void *parent);
extern void page_tb_language(void *parent);
extern void page_tb_wallpaper(void *parent);
extern void page_tb_findtws(void *parent);
extern void page_tb_flashlight(void *parent);

extern gui_img_t *bat_left;
extern gui_img_t *bat_right;
extern gui_img_t *bat_case;
extern gui_img_t *bluetooth;
extern gui_text_t *left_battery_text;
extern gui_text_t *right_battery_text;
extern gui_text_t *case_battery_text;

void app_gui_show_link_device(void);
void app_gui_show_title(uint8_t *title, uint16_t length);
void app_gui_show_artist(uint8_t *artist, uint16_t length);
void app_gui_show_album(uint8_t *album, uint16_t length);
void app_gui_show_volume(uint8_t current_phone_volume);
void app_gui_show_device_name(uint8_t *data, uint8_t length);
void app_gui_show_battery(uint8_t case_battery, uint8_t left_battery, uint8_t right_battery);
void app_gui_show_bt_link_status(T_APP_GUI_LE_LINK_STATUS link_status);
void app_gui_show_player_status(T_APP_GUI_PLAYER_STATUS player_status);


#ifdef __cplusplus
}
#endif
#endif


