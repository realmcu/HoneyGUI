#include <gui_img.h>
#include "gui_win.h"
#include "gui_obj.h"
#include <gui_button.h>
#include "gui_text.h"
#include "gui_switch.h"
#include "app_chargebox.h"
#include "gui_tabview.h"


void page_tb_flashlight(void *parent)
{
    char *func = "手电筒";
    gui_text_t *text_func = gui_text_create(parent, "text_func", 20, 60, 200, 28);
    gui_text_set(text_func, func, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX, UINT8_MAX, UINT8_MAX),
                 strlen(func), 28);

    gui_switch_t *switch_flashlight = gui_switch_create(parent, 140, 150, 104, 104, FLASHLIGHT_BIN,
                                                        FLASHLIGHT_HIGHLIGHT_BIN);

}
