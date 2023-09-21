#include "root_image_hongkong/hongkong.h"
#include <gui_magic_img.h>

void page_tb_activity(void *parent)
{
    gui_magic_img_create_from_mem(parent, "page1", ACTIVITY_BIN, 0, 0, 0, 0);
}