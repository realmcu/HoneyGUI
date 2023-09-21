#include "root_image_hongkong/ui_resource.h"
#include <gui_magic_img.h>

void page_tb_control1(void *parent)
{
    gui_magic_img_create_from_mem(parent, "tb_control1", CONTROLMENU_1_BIN, 0, 0, 0, 0);
}
