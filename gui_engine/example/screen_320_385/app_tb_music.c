#include <gui_img.h>
#include "gui_win.h"
#include "gui_obj.h"
#include <gui_button.h>
#include "gui_text.h"
#include "gui_switch.h"
#include "app_chargebox.h"
#include "gui_tabview.h"

gui_switch_t *switch_music_play_pause;

void page_tb_music(void *parent)
{
    gui_button_t *button_music_backward = gui_button_create(parent, 50, 150, 80, 80, MUSIC_BACKWARD_BIN,
                                                            MUSIC_BACKWARD_HIGHLIGHT_BIN, NULL, BUTTON_BG_ICON, 0);
    gui_button_click(button_music_backward, (gui_event_cb_t)app_gui_ble_action_cb,
                     (void *)GUI_BLE_SUBEVENT_MUSIC_BACKWARD);

    switch_music_play_pause = gui_switch_create(parent, 150, 150, 80, 80, MUSIC_PLAY_BIN,
                                                MUSIC_PAUSE_BIN);
    gui_obj_add_event_cb(switch_music_play_pause, (gui_event_cb_t)app_gui_ble_action_cb, GUI_EVENT_1,
                         (void *)GUI_BLE_SUBEVENT_MUSIC_PLAY);
    gui_obj_add_event_cb(switch_music_play_pause, (gui_event_cb_t)app_gui_ble_action_cb, GUI_EVENT_2,
                         (void *)GUI_BLE_SUBEVENT_MUSIC_PAUSE);

    gui_button_t *button_music_forward = gui_button_create(parent, 250, 150, 80, 80, MUSIC_FORWARD_BIN,
                                                           MUSIC_FORWARD_HIGHLIGHT_BIN, NULL, BUTTON_BG_ICON, 0);
    gui_button_click(button_music_forward, (gui_event_cb_t)app_gui_ble_action_cb,
                     (void *)GUI_BLE_SUBEVENT_MUSIC_FORWARD);
}
