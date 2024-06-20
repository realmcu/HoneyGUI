#include <gui_img.h>
#include "gui_win.h"
#include "gui_obj.h"
#include <gui_button.h>
#include "gui_text.h"
#include "gui_switch.h"
#include "app_chargebox.h"
#include "gui_tabview.h"


gui_text_t *text_set_language;
static char *set_language = "简体中文";


void language_ch_cb(void)
{
    set_language = "简体中文";
    gui_text_set(text_set_language, set_language, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX, UINT8_MAX,
                                                                            UINT8_MAX),
                 strlen(set_language), 28);
}

void language_en_cb(void)
{
    set_language = "English";
    gui_text_set(text_set_language, set_language, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX, UINT8_MAX,
                                                                            UINT8_MAX),
                 strlen(set_language), 28);
}

void page_tb_language(void *parent)
{
    char *func = "语言切换";
    gui_text_t *text_func = gui_text_create(parent, "text_func", 20, 60, 200, 28);
    gui_text_set(text_func, func, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX, UINT8_MAX, UINT8_MAX),
                 strlen(func), 28);

    text_set_language = gui_text_create(parent, "text_set_time", 150, 180, 100, 28);
    gui_text_set(text_set_language, set_language, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX, UINT8_MAX,
                                                                            UINT8_MAX),
                 strlen(set_language), 28);

    gui_button_t *button_language_ch = gui_button_create(parent, 60, 150, 80, 80, OPTION_LEFT_BIN,
                                                         OPTION_LEFT_HIGHLIGHT_BIN, NULL, BUTTON_BG_ICON, 0);
    gui_button_click(button_language_ch, (gui_event_cb_t)language_ch_cb, NULL);

    gui_button_t *button_language_en = gui_button_create(parent, 240, 150, 80, 80, OPTION_RIGHT_BIN,
                                                         OPTION_RIGHT_HIGHLIGHT_BIN, NULL, BUTTON_BG_ICON, 0);
    gui_button_click(button_language_en, (gui_event_cb_t)language_en_cb, NULL);

}
