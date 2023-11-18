#include "app_gui_main.h"
#include "root_image/ui_resource.h"
#include "gui_img.h"

gui_img_t *img_clock = NULL;
void design_tab_watchface(void *parent)
{
    img_clock = gui_img_create_from_mem(parent, "img_clock", CLOCK_BIN, 0, 0, LCD_H, LCD_W);
}