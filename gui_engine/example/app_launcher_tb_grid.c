
#include <gui_obj.h>
#include <gui_grid.h>
#include "gui_magic_img.h"
#include "rtk_gui_resource.h"

void page_tb_grid(void *parent)
{
    void *array[] = {CLOCK_BIN, EMAIL_BIN, SPORT_BIN, SLEEP_BIN, MAP_BIN, CLOCK_BIN};
    gui_grid_t *grid = gui_grid_create(parent, 100, 200, 1
                                       , 7, 30, 30);
    gui_grid_style(grid, GRID_3D);
    for (size_t i = 0; i < 6; i++)
    {
        gui_magic_img_create_from_mem(grid, "1", array[i], 0, 0, 0, 0);
    }
}
