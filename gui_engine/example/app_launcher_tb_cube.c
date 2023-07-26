
#include <gui_obj.h>
#include "gui_cube.h"
#include "rtk_gui_resource.h"



void page_tb_cube(void *parent)
{
    void *array[] = {C1_BIN, C2_BIN, C3_BIN, C4_BIN, C5_BIN, C6_BIN};
    gui_cube_t *cube = gui_cube_create(parent, "cube", array, 0, 0, 454, 454);
    gui_cube_set_size(cube, 50);
    gui_cube_set_center(cube, 50, 50);

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
