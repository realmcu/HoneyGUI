#include "root_image_hongkong/ui_resource.h"
#include <gui_magic_img.h>
#include "gui_perspective.h"
#include "draw_font.h"

void page_array_flash(void *parent)
{
    gui_magic_img_t *img = gui_magic_img_create_from_mem(parent, "test", C1_BIN, 0, 0, 454, 454);
    gui_img_scale(img, 0.2f, 0.2f);
    gui_img_translate(img, 200, 200);
    gui_img_rotation(img, 45, 128 + (454 - 256) / 2, 128 + (454 - 256) / 2);
    uint8_t *array_flash[] = {C1_BIN, C2_BIN, C3_BIN, C4_BIN, C5_BIN, C6_BIN};
    gui_perspective_t *img_test = gui_perspective_create(parent, "test", array_flash, 0, 0, 454, 454);
}