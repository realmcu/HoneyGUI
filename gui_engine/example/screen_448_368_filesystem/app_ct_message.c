#include "root_image_hongkong/ui_resource.h"
#include <gui_img.h>
#include "gui_perspective.h"
void page_ct_message(void *parent)
{
    //gui_img_create_from_mem(parent, "page_down", MESSAGE_BIN, 0, 0, 0, 0);
    gui_perspective_imgfile_t imgfile =
    {
        .src_mode[0] = IMG_SRC_MEMADDR, .src_mode[1] = IMG_SRC_MEMADDR, .src_mode[2] = IMG_SRC_MEMADDR,
        .src_mode[3] = IMG_SRC_MEMADDR, .src_mode[4] = IMG_SRC_MEMADDR, .src_mode[5] = IMG_SRC_MEMADDR,
        .data_addr[0] = CLOCKN_BIN,
        .data_addr[1] = WEATHER_BIN,
        .data_addr[2] = MUSIC_BIN,
        .data_addr[3] = QUICKCARD_BIN,
        .data_addr[4] = HEARTRATE_BIN,
        .data_addr[5] = ACTIVITY_BIN
    };
    gui_perspective_t *img_test = gui_perspective_create(parent, "test", &imgfile, 0, 0);
}