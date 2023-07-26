
#include <gui_obj.h>
#include <gui_watch_gradient_spot.h>
#include <gui_magic_img.h>
#include "rtk_gui_resource.h"



void page_tb_watch(void *parent)
{

    gui_watch_gradient_spot_t *watch = gui_watch_gradient_spot_create(parent,  "tb_00", 0, 0,
                                                                      454, 454);
    gui_watch_gradient_spot_set_center(watch, 454 / 2, 454 / 2);



}
