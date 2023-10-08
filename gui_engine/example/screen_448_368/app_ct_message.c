#include "root_image_hongkong/ui_resource.h"
#include <gui_magic_img.h>
#include "gui_perspective.h"
void page_ct_message(void *parent)
{
    //gui_magic_img_create_from_mem(parent, "page_down", MESSAGE_BIN, 0, 0, 0, 0);
    uint8_t *array_flash[] = {ACTIVITY_BIN, BLOODOXYGEN_BIN, CLOCKN_BIN, MUSIC_BIN, QUICKCARD_BIN};
    gui_perspective_t *img_test = gui_perspective_create(parent, "test", array_flash, 0, 0, 454, 454);
}