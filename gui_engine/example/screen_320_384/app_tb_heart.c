#include "root_image_hongkong_bee4/ui_resource.h"
#include <gui_magic_img.h>

void page_tb_heart(void *parent)
{
    gui_magic_img_create_from_mem(parent, "page2", HEARTRATE_BIN, 0, 0, 0, 0);
}
