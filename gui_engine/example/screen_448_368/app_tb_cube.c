
#include <gui_obj.h>
#include "gui_cube.h"
#include "root_image_hongkong/ui_resource.h"
#include "gui_win.h"

extern void callback_prism(void *obj, gui_event_t e);


void page_tb_cube(void *parent)
{
    uint8_t *array[6] = {ACTIVITY_BIN, WEATHER_BIN, HEARTRATE_BIN, CLOCKN_BIN, MUSIC_BIN, QUICKCARD_BIN};


    gui_cube_t *cube4 = gui_cube_create(parent, "cube", array, 0, 0, 454, 454);
    gui_cube_set_size(cube4, 100);
    gui_cube_set_center(cube4, 200, 200);
    gui_win_t *win_function = gui_win_create(parent, "win_function", 0, 0, 368, 448);
    gui_obj_add_event_cb(win_function, (gui_event_cb_t)callback_prism, GUI_EVENT_TOUCH_LONG, NULL);
}
