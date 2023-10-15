
#include <gui_obj.h>
#include "gui_cube.h"
#include "root_image_hongkong/ui_resource.h"



void page_tb_cube(void *parent)
{
    uint8_t *array[6] = {ACTIVITY_BIN, WEATHER_BIN, HEARTRATE_BIN, CLOCKN_BIN, MUSIC_BIN, QUICKCARD_BIN};


    gui_cube_t *cube4 = gui_cube_create(parent, "cube", array, 0, 0, 454, 454);
    gui_cube_set_size(cube4, 100);
    gui_cube_set_center(cube4, 200, 200);
}
