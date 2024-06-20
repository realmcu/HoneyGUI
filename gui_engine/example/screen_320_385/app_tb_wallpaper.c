#include <gui_img.h>
#include "gui_win.h"
#include "gui_obj.h"
#include <gui_button.h>
#include "gui_text.h"
#include "gui_switch.h"
#include "app_chargebox.h"
#include "gui_tabview.h"
#include "gui_img_scope.h"

extern uint8_t wallpaper_idx;
extern uint32_t *wallpaper_list[];
extern gui_img_t *screen_wallpaper;
gui_img_t *img_wallpaper;

void wallpaper_last_cb(void)
{
    if (wallpaper_idx > 0)
    {
        wallpaper_idx--;
        gui_img_set_attribute(img_wallpaper, "wallpaper", wallpaper_list[wallpaper_idx],
                              GET_BASE(img_wallpaper)->x, GET_BASE(img_wallpaper)->y);
        gui_img_scale(img_wallpaper, 0.4, 0.4);

        gui_img_set_attribute(screen_wallpaper, "screen lock", wallpaper_list[wallpaper_idx], 0, 0);
    }
}

void wallpaper_next_cb(void)
{
    gui_log("wallpaper_idx: %d\n", wallpaper_idx);
    if (wallpaper_idx < WALLPAPER_NUM - 1)
    {
        wallpaper_idx++;
        gui_img_set_attribute(img_wallpaper, "wallpaper", wallpaper_list[wallpaper_idx],
                              GET_BASE(img_wallpaper)->x, GET_BASE(img_wallpaper)->y);
        gui_img_scale(img_wallpaper, 0.4, 0.4);

        gui_img_set_attribute(screen_wallpaper, "screen lock", wallpaper_list[wallpaper_idx], 0, 0);
    }
}

void page_tb_wallpaper(void *parent)
{
    char *func = "锁屏壁纸";
    gui_text_t *text_func = gui_text_create(parent, "text_func", 20, 60, 200, 28);
    gui_text_set(text_func, func, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX, UINT8_MAX, UINT8_MAX),
                 strlen(func), 28);

    img_wallpaper = gui_img_create_from_mem(parent, "wallpaper", WALLPAPER1_BIN, 120, 120, 0, 0);
    gui_img_scale(img_wallpaper, 0.4, 0.4);
    gui_img_set_mode(img_wallpaper, IMG_BYPASS_MODE);

    gui_button_t *button_wallpaper_last = gui_button_create(parent, 20, 150, 80, 80, OPTION_LEFT_BIN,
                                                            OPTION_LEFT_HIGHLIGHT_BIN, NULL, BUTTON_BG_ICON, 0);
    gui_button_click(button_wallpaper_last, (gui_event_cb_t)wallpaper_last_cb, NULL);


    gui_button_t *button_wallpaper_next = gui_button_create(parent, 280, 150, 80, 80, OPTION_RIGHT_BIN,
                                                            OPTION_RIGHT_HIGHLIGHT_BIN, NULL, BUTTON_BG_ICON, 0);
    gui_button_click(button_wallpaper_next, (gui_event_cb_t)wallpaper_next_cb, NULL);

}
