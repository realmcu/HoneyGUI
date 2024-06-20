#ifndef __APP_CHARGEBOX_H__
#define __APP_CHARGEBOX_H__

#ifdef __cplusplus
extern "C" {
#endif
#include <gui_app.h>
#include "root_image_chargebox/ui_resource.h"

#define WALLPAPER_NUM 3

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

#ifdef __cplusplus
}
#endif
#endif


