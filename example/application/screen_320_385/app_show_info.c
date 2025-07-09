#include <gui_img.h>
#include "gui_win.h"
#include "gui_obj.h"
#include <gui_button.h>
#include "gui_text.h"
#include "gui_switch.h"
#include "app_chargebox.h"
#include "gui_tabview.h"
#include "gui_page.h"

extern gui_tabview_t *tabview_info;
extern gui_img_t *bluetooth;
extern gui_page_t *page_device;
extern gui_switch_t *switch_music_play_pause;

char case_battery_level[5];
char left_battery_level[5];
char right_battery_level[5];
gui_text_t *text_device_name;
static char device_name[40];
gui_button_t *button_dev;

void app_gui_show_device_name(uint8_t *data, uint8_t length)
{
    if (text_device_name)
    {
        snprintf(device_name, length < sizeof(device_name) ? length : sizeof(device_name), "Name:   %s",
                 data);
        gui_text_content_set(text_device_name, device_name, strlen(device_name));
    }
}

void app_gui_show_link_device(void)
{
    button_dev = gui_button_create(page_device, 10, 40, 367, 38, BUTTON_BIN, BUTTON_ON_BIN, NULL,
                                   BUTTON_BG_ICON, 0);

    char *text1 = "RTL8763ESE";
    gui_text_t *button_text = gui_text_create(button_dev, "button_text", 10, 0, 367, 38);
    gui_text_set(button_text, text1, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX, UINT8_MAX, UINT8_MAX),
                 strlen(text1), 28);
}

void app_gui_show_battery(uint8_t case_battery, uint8_t left_battery, uint8_t right_battery)
{
    gui_log("app_gui_show_battery: case %d, left %d, right %d",
            case_battery, left_battery, right_battery);

    if (case_battery != BATTERY_INVALID_VALUE)
    {
        snprintf(case_battery_level, sizeof(case_battery_level), "%d%%", case_battery);
        gui_text_set(case_battery_text, case_battery_level, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX, UINT8_MAX,
                                                                                      UINT8_MAX), strlen(case_battery_level), 24);
    }

    if (left_battery != BATTERY_INVALID_VALUE)
    {
        snprintf(left_battery_level, sizeof(left_battery_level), "%d%%", left_battery);
        gui_text_set(left_battery_text, left_battery_level, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX, UINT8_MAX,
                                                                                      UINT8_MAX), strlen(left_battery_level), 24);
    }

    if (right_battery != BATTERY_INVALID_VALUE)
    {
        snprintf(right_battery_level, sizeof(right_battery_level), "%d%%", right_battery);
        gui_text_set(right_battery_text, right_battery_level, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX,
                                                                                        UINT8_MAX,
                                                                                        UINT8_MAX), strlen(right_battery_level), 24);
    }
}

void app_gui_show_bt_link_status(T_APP_GUI_LE_LINK_STATUS link_status)
{
    if (link_status == BLE_LINK_CONNECT)
    {
        gui_img_set_attribute(bluetooth, "bluetooth_status", BLUETOOTH_ON_BIN, GET_BASE(bluetooth)->x,
                              GET_BASE(bluetooth)->y);
    }
    else
    {
        gui_img_set_attribute(bluetooth, "bluetooth_status", BLUETOOTH_OFF_BIN, GET_BASE(bluetooth)->x,
                              GET_BASE(bluetooth)->y);
    }
}

void app_gui_show_player_status(T_APP_GUI_PLAYER_STATUS player_status)
{
    if (player_status == MUSIC_PLAYING)
    {
        gui_log("MUSIC_PLAYING\n");
        gui_switch_is_on(switch_music_play_pause);
    }
    else if (player_status == MUSIC_PAUSED)
    {
        gui_log("MUSIC_Paused\n");
        gui_switch_is_off(switch_music_play_pause);
    }
}


