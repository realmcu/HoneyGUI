
#include <gui_obj.h>
#include "gui_cube.h"
#include "rtk_gui_resource.h"



void page_tb_cube(void *parent)
{
    uint8_t *array_flash[] = {A1_BIN, A2_BIN, A3_BIN, A4_BIN, A5_BIN, A6_BIN};

    uint8_t *array[6];

#define CUBE_IMG_LEN (64*64*2 + 8)

    for (uint8_t i = 0; i < 6; i++)
    {
        array[i] = gui_malloc(CUBE_IMG_LEN + 64 + 8);
        array[i] = array[i] + 64 - (int)array[i] % 64 - 8;
        memcpy(array[i], array_flash[i], CUBE_IMG_LEN);
    }

    gui_cube_t *cube0 = gui_cube_create(parent, "cube", array, 0, 0, 454, 454);
    gui_cube_set_size(cube0, 50);
    gui_cube_set_center(cube0, 50, 50);

    gui_cube_t *cube1 = gui_cube_create(parent, "cube", array, 0, 0, 454, 454);
    gui_cube_set_size(cube1, 50);
    gui_cube_set_center(cube1, 350, 350);

    gui_cube_t *cube2 = gui_cube_create(parent, "cube", array, 0, 0, 454, 454);
    gui_cube_set_size(cube2, 50);
    gui_cube_set_center(cube2, 50, 350);

    gui_cube_t *cube3 = gui_cube_create(parent, "cube", array, 0, 0, 454, 454);
    gui_cube_set_size(cube3, 50);
    gui_cube_set_center(cube3, 350, 50);

    gui_cube_t *cube4 = gui_cube_create(parent, "cube", array, 0, 0, 454, 454);
    gui_cube_set_size(cube4, 50);
    gui_cube_set_center(cube4, 200, 200);
}
