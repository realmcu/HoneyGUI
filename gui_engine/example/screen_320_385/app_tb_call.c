#include <gui_img.h>
#include "gui_win.h"
#include "gui_obj.h"
#include <gui_button.h>
#include "gui_text.h"
#include "gui_switch.h"
#include "app_chargebox.h"
#include "gui_tabview.h"

extern gui_tabview_t *tabview_main;
gui_tabview_t *tabview_call_mgr;
gui_button_t *button_call_answer;
gui_button_t *button_call_reject;
gui_switch_t *switch_mic_unmute;
gui_switch_t *switch_mic_mute;

static void call_reject_cb(void)
{
    if (tabview_call_mgr != NULL)
    {
        gui_obj_tree_free(tabview_call_mgr);
    }
    gui_tabview_jump_tab(tabview_main, tabview_main->cur_id.x, 0);
}

static void switch_mic_unmute_on_cb(void)
{
    gui_switch_is_off(switch_mic_mute);
}

static void switch_mic_mute_on_cb(void)
{
    gui_switch_is_off(switch_mic_unmute);
}

static void call_answer_cb(void)
{
    button_call_answer->base.not_show = true;
    button_call_reject->base.not_show = true;

    gui_button_t *button_call_end = gui_button_create(tabview_call_mgr, 80, 150, 80, 80,
                                                      CALL_REJECT_BIN,
                                                      CALL_REJECT_HIGHLIGHT_BIN, NULL, BUTTON_BG_ICON, 0);
    gui_button_click(button_call_end, (gui_event_cb_t)call_reject_cb, NULL);
    gui_button_click(button_call_end, (gui_event_cb_t)app_gui_ble_action_cb,
                     (void *)GUI_BLE_SUBEVENT_CALL_REJECT);

    switch_mic_unmute = gui_switch_create(tabview_call_mgr, 200, 160, 60, 60, MIC_UNMUTE_BIN,
                                          MIC_UNMUTE_HIGHLIGHT_BIN);
    gui_obj_add_event_cb(switch_mic_unmute, (gui_event_cb_t)switch_mic_unmute_on_cb, GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_mic_unmute, (gui_event_cb_t)app_gui_ble_action_cb, GUI_EVENT_1,
                         (void *)GUI_BLE_SUBEVENT_MIC_UNMUTE);

    switch_mic_mute = gui_switch_create(tabview_call_mgr, 280, 160, 60, 60, MIC_MUTE_BIN,
                                        MIC_MUTE_HIGHLIGHT_BIN);
    gui_obj_add_event_cb(switch_mic_mute, (gui_event_cb_t)switch_mic_mute_on_cb, GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_mic_mute, (gui_event_cb_t)app_gui_ble_action_cb, GUI_EVENT_1,
                         (void *)GUI_BLE_SUBEVENT_MIC_MUTE);

}

void page_tb_call(void *parent)
{
    tabview_call_mgr = gui_tabview_create(parent, "tabview call manager", 0, 0, 0, 0);
    button_call_answer = gui_button_create(tabview_call_mgr, 60, 150, 80, 80, CALL_ANSWER_BIN,
                                           CALL_ANSWER_HIGHLIGHT_BIN, NULL, BUTTON_BG_ICON, 0);
    gui_button_click(button_call_answer, (gui_event_cb_t)call_answer_cb, NULL);
    gui_button_click(button_call_answer, (gui_event_cb_t)app_gui_ble_action_cb,
                     (void *)GUI_BLE_SUBEVENT_CALL_ANSWER);

    button_call_reject = gui_button_create(tabview_call_mgr, 240, 150, 80, 80, CALL_REJECT_BIN,
                                           CALL_REJECT_HIGHLIGHT_BIN, NULL, BUTTON_BG_ICON, 0);
    gui_button_click(button_call_reject, (gui_event_cb_t)call_reject_cb, NULL);
    gui_button_click(button_call_reject, (gui_event_cb_t)app_gui_ble_action_cb,
                     (void *)GUI_BLE_SUBEVENT_CALL_REJECT);
}
