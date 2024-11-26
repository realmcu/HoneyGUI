#include <gui_img.h>
#include "gui_win.h"
#include "gui_obj.h"
#include <gui_button.h>
#include "gui_text.h"
#include "gui_switch.h"
#include "app_chargebox.h"
#include "gui_tabview.h"


void page_tb_findtws(void *parent)
{
    char *func = "找到我的耳机";
    gui_text_t *text_func = gui_text_create(parent, "anc",  20, 60, 200, 28);
    gui_text_set(text_func, func, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX, UINT8_MAX, UINT8_MAX),
                 strlen(func), 28);

    gui_button_t *button_findtws_left = gui_button_create(parent, 20, 150, 162, 74, TWS_LEFT_BIN,
                                                          TWS_LEFT_HIGHLIGHT_BIN, NULL, BUTTON_BG_ICON, 0);
    gui_button_click(button_findtws_left, (gui_event_cb_t)app_gui_ble_action_cb,
                     (void *)GUI_BLE_SUBEVENT_FINDTWS_LEFT);

    gui_button_t *button_findtws_right = gui_button_create(parent, 200, 150, 162, 74, TWS_RIGHT_BIN,
                                                           TWS_RIGHT_HIGHLIGHT_BIN, NULL, BUTTON_BG_ICON, 0);
    gui_button_click(button_findtws_right, (gui_event_cb_t)app_gui_ble_action_cb,
                     (void *)GUI_BLE_SUBEVENT_FINDTWS_RIGHT);

}
