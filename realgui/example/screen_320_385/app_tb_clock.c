#include <gui_img.h>
#include "gui_win.h"
#include "gui_obj.h"
#include <gui_button.h>
#include "gui_text.h"
#include "gui_switch.h"
#include "app_chargebox.h"
#include "gui_tabview.h"

#define ALRAM_CLOCK_LENGTH 10
gui_text_t *text_set_time;
static uint8_t set_time = 5;
static char *set_time_uint = "分钟";
static char set_alarm_clock[ALRAM_CLOCK_LENGTH];

void time_dec_cb(void)
{
    if (set_time >= 5)
    {
        set_time = set_time - 5;
        snprintf(set_alarm_clock, ALRAM_CLOCK_LENGTH, "%d%s", set_time, set_time_uint);
        gui_text_set(text_set_time, set_alarm_clock, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX, UINT8_MAX,
                                                                               UINT8_MAX),
                     strlen(set_alarm_clock), 28);
    }
}

void time_inc_cb(void)
{
    if (set_time <= 55)
    {
        set_time = set_time + 5;
        snprintf(set_alarm_clock, ALRAM_CLOCK_LENGTH, "%d%s", set_time, set_time_uint);
        gui_text_set(text_set_time, set_alarm_clock, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX, UINT8_MAX,
                                                                               UINT8_MAX),
                     strlen(set_alarm_clock), 28);
    }
}

void page_tb_clock(void *parent)
{
    char *func = "闹钟设置";
    gui_text_t *text_func = gui_text_create(parent, "text_func", 20, 60, 200, 28);
    gui_text_set(text_func, func, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX, UINT8_MAX, UINT8_MAX),
                 strlen(func), 28);

    text_set_time = gui_text_create(parent, "text_set_time", 160, 180, 100, 28);
    snprintf(set_alarm_clock, ALRAM_CLOCK_LENGTH, "%d%s", set_time, set_time_uint);
    gui_text_set(text_set_time, set_alarm_clock, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX, UINT8_MAX,
                                                                           UINT8_MAX),
                 strlen(set_alarm_clock), 28);

    gui_button_t *button_time_dec = gui_button_create(parent, 60, 150, 80, 80, OPTION_LEFT_BIN,
                                                      OPTION_LEFT_HIGHLIGHT_BIN, NULL, BUTTON_BG_ICON, 0);
    gui_button_click(button_time_dec, (gui_event_cb_t)time_dec_cb, NULL);


    gui_button_t *button_time_inc = gui_button_create(parent, 240, 150, 80, 80, OPTION_RIGHT_BIN,
                                                      OPTION_RIGHT_HIGHLIGHT_BIN, NULL, BUTTON_BG_ICON, 0);
    gui_button_click(button_time_inc, (gui_event_cb_t)time_inc_cb, NULL);

    gui_button_t *button_clock_set = gui_button_create(parent, 110, 250, 162, 64, CLOCK_SET_BIN,
                                                       CLOCK_SET_HIGHLIGHT_BIN, NULL, BUTTON_BG_ICON, 0);


}
