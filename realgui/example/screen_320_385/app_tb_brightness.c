#include <gui_img.h>
#include "gui_win.h"
#include "gui_obj.h"
#include <gui_button.h>
#include "gui_text.h"
#include "gui_switch.h"
#include "app_chargebox.h"
#include "gui_progressbar.h"

#define BAR_SIZE 5
gui_progressbar_t *progressbar_brightness;
gui_text_t *text_brightness;
static char brightness[20];

const static void *scrollbar_array[BAR_SIZE] =
{
    BAR0_BIN, BAR1_BIN, BAR2_BIN, BAR3_BIN, BAR4_BIN
};
uint8_t current_brightness = 2;

void brightness_dec_cb(void)
{
    if (current_brightness > 0)
    {
        current_brightness--;
    }

    snprintf(brightness, sizeof(brightness), "屏幕亮度: %d", current_brightness);
    gui_text_set(text_brightness, brightness, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX, UINT8_MAX,
                                                                        UINT8_MAX),
                 strlen(brightness), 28);

    float progress = (float)current_brightness / (BAR_SIZE - 1);
    if (progress >= 0 && progress <= 1)
    {
        gui_progressbar_set_percentage(progressbar_brightness, progress) ;
    }

}

void brightness_inc_cb(void)
{
    if (current_brightness < 4)
    {
        current_brightness++;
    }

    snprintf(brightness, sizeof(brightness), "屏幕亮度: %d", current_brightness);
    gui_text_set(text_brightness, brightness, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX, UINT8_MAX,
                                                                        UINT8_MAX),
                 strlen(brightness), 28);

    float progress = (float)current_brightness / (BAR_SIZE - 1);
    if (progress >= 0 && progress <= 1)
    {
        gui_progressbar_set_percentage(progressbar_brightness, progress) ;
    }

}

void page_tb_brightness(void *parent)
{
    snprintf(brightness, sizeof(brightness), "屏幕亮度: %d", current_brightness);
    text_brightness = gui_text_create(parent, "brightness", 20, 60, 200, 28);
    gui_text_set(text_brightness, brightness, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX, UINT8_MAX,
                                                                        UINT8_MAX),
                 strlen(brightness), 28);

    gui_button_t *button_brightness_dec = gui_button_create(parent, 60, 200, 80, 80, OPTION_LEFT_BIN,
                                                            OPTION_LEFT_HIGHLIGHT_BIN, NULL, BUTTON_BG_ICON, 0);
    gui_button_click(button_brightness_dec, (gui_event_cb_t)brightness_dec_cb, NULL);

    progressbar_brightness = gui_progressbar_movie_create(parent, (void *)scrollbar_array, BAR_SIZE, 94,
                                                          150);
    progressbar_brightness->max = sizeof(scrollbar_array) / sizeof(void *);
    gui_progressbar_set_percentage(progressbar_brightness, (float)current_brightness / (BAR_SIZE - 1));

    gui_button_t *button_brightness_inc = gui_button_create(parent, 240, 200, 80, 80, OPTION_RIGHT_BIN,
                                                            OPTION_RIGHT_HIGHLIGHT_BIN, NULL, BUTTON_BG_ICON, 0);
    gui_button_click(button_brightness_inc, (gui_event_cb_t)brightness_inc_cb, NULL);
}
