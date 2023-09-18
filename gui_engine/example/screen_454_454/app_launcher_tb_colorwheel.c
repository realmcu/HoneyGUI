
#include <gui_obj.h>
#include "gui_colorwheel.h"



void page_tb_colorwheel(void *parent)
{
    gui_dispdev_t *dc = gui_get_dc();

    gui_colorwheel_t *colorwheel = gui_colorwheel_create(parent,  "tb_10", NULL, 0, 0,
                                                         454, 454);
    gui_colorwheel_set(colorwheel, 0, 0, dc->screen_width, dc->screen_height, 0);
}
