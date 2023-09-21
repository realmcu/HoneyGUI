#include "root_image_hongkong/hongkong.h"
#include <gui_magic_img.h>

void page_ct_message(void *parent)
{
    gui_magic_img_create_from_mem(parent, "page_down", MESSAGE_BIN, 0, 0, 0, 0);
}