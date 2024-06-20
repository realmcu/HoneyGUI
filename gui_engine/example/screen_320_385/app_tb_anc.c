#include <gui_img.h>
#include "gui_win.h"
#include "gui_obj.h"
#include <gui_button.h>
#include "gui_text.h"
#include "gui_switch.h"
#include "app_chargebox.h"
#include "gui_tabview.h"

gui_switch_t *switch_listen_off;
gui_switch_t *switch_listen_ANC;
gui_switch_t *switch_listen_APT;

static void switch_listen_off_on_cb(void)
{
    gui_switch_is_off(switch_listen_ANC);
    gui_switch_is_off(switch_listen_APT);
}

static void switch_listen_ANC_on_cb(void)
{
    gui_switch_is_off(switch_listen_off);
    gui_switch_is_off(switch_listen_APT);
}

static void switch_listen_APT_on_cb(void)
{
    gui_switch_is_off(switch_listen_off);
    gui_switch_is_off(switch_listen_ANC);
}


void page_tb_anc(void *parent)
{
    char *func = "降噪";
    gui_text_t *text_func = gui_text_create(parent, "text_func", 20, 60, 200, 28);
    gui_text_set(text_func, func, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX, UINT8_MAX, UINT8_MAX),
                 strlen(func), 28);

    switch_listen_off = gui_switch_create(parent, 20, 150, 108, 70, LISTEN_OFF_BIN,
                                          LISTEN_OFF_HIGHLIGHT_BIN);
    gui_obj_add_event_cb(switch_listen_off, (gui_event_cb_t)switch_listen_off_on_cb, GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_listen_off, (gui_event_cb_t)app_gui_ble_action_cb, GUI_EVENT_1,
                         (void *)GUI_BLE_SUBEVENT_LISTEN_OFF);

    switch_listen_ANC = gui_switch_create(parent, 140, 150, 108, 70, LISTEN_ANC_BIN,
                                          LISTEN_ANC_HIGHLIGHT_BIN);
    gui_obj_add_event_cb(switch_listen_ANC, (gui_event_cb_t)switch_listen_ANC_on_cb, GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_listen_ANC, (gui_event_cb_t)app_gui_ble_action_cb, GUI_EVENT_1,
                         (void *)GUI_BLE_SUBEVENT_LISTEN_ANC);

    switch_listen_APT = gui_switch_create(parent, 260, 150, 108, 70, LISTEN_APT_BIN,
                                          LISTEN_APT_HIGHLIGHT_BIN);
    gui_obj_add_event_cb(switch_listen_APT, (gui_event_cb_t)switch_listen_APT_on_cb, GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_listen_APT, (gui_event_cb_t)app_gui_ble_action_cb, GUI_EVENT_1,
                         (void *)GUI_BLE_SUBEVENT_LISTEN_APT);

}
