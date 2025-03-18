#include <gui_img.h>
#include "gui_win.h"
#include "gui_obj.h"
#include <gui_button.h>
#include "gui_text.h"
#include "gui_switch.h"
#include "app_chargebox.h"
#include "gui_tabview.h"

gui_text_t *text_equalizer_mode;
static char *equalizer_mode = "普通模式";

void eq_normal_cb(void)
{
    equalizer_mode = "普通模式";
    gui_text_set(text_equalizer_mode, equalizer_mode, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX, UINT8_MAX,
                                                                                UINT8_MAX),
                 strlen(equalizer_mode), 28);
}

void eq_gaming_cb(void)
{
    equalizer_mode = "游戏模式";
    gui_text_set(text_equalizer_mode, equalizer_mode, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX, UINT8_MAX,
                                                                                UINT8_MAX),
                 strlen(equalizer_mode), 28);
}

void page_tb_equalizer(void *parent)
{
    char *func = "均衡器";
    gui_text_t *text_func = gui_text_create(parent, "text_func", 20, 60, 200, 28);
    gui_text_set(text_func, func, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX, UINT8_MAX, UINT8_MAX),
                 strlen(func), 28);

    text_equalizer_mode = gui_text_create(parent, "equalizer_mode", 150, 180, 100, 28);
    gui_text_set(text_equalizer_mode, equalizer_mode, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX, UINT8_MAX,
                                                                                UINT8_MAX),
                 strlen(equalizer_mode), 28);

    gui_button_t *button_eq_normal = gui_button_create(parent, 60, 150, 80, 80, OPTION_LEFT_BIN,
                                                       OPTION_LEFT_HIGHLIGHT_BIN, NULL, BUTTON_BG_ICON, 0);
    gui_button_click(button_eq_normal, (gui_event_cb_t)eq_normal_cb, NULL);
    gui_button_click(button_eq_normal, (gui_event_cb_t)app_gui_ble_action_cb,
                     (void *)GUI_BLE_SUBEVENT_EQ_NORMAL);


    gui_button_t *button_eq_gaming = gui_button_create(parent, 240, 150, 80, 80, OPTION_RIGHT_BIN,
                                                       OPTION_RIGHT_HIGHLIGHT_BIN, NULL, BUTTON_BG_ICON, 0);
    gui_button_click(button_eq_gaming, (gui_event_cb_t)eq_gaming_cb, NULL);
    gui_button_click(button_eq_gaming, (gui_event_cb_t)app_gui_ble_action_cb,
                     (void *)GUI_BLE_SUBEVENT_EQ_GAMING);


}
