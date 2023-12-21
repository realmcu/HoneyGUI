
#include <gui_obj.h>
#include "gui_cube.h"
#include "root_image_hongkong/ui_resource.h"
#include "gui_win.h"

extern void callback_prism(void *obj, gui_event_t e);


void page_tb_cube(void *parent)
{
#ifdef __WIN32
    gui_cube_imgfile_t imgfile =
    {
        .flg_fs = true,
        .img_path.img_path_front = "Activity.bin",
        .img_path.img_path_back = "Weather.bin",
        .img_path.img_path_up = "HeartRate.bin",
        .img_path.img_path_down = "Clockn.bin",
        .img_path.img_path_left = "Music.bin",
        .img_path.img_path_right = "QuickCard.bin"
    };
    // gui_cube_imgfile_t imgfile =
    // {
    //     .flg_fs = false,
    //     .data_addr.data_addr_front = ACTIVITY_BIN,
    //     .data_addr.data_addr_back = WEATHER_BIN,
    //     .data_addr.data_addr_up =  HEARTRATE_BIN,
    //     .data_addr.data_addr_down = CLOCKN_BIN,
    //     .data_addr.data_addr_left = MUSIC_BIN,
    //     .data_addr.data_addr_right = QUICKCARD_BIN
    // };
#else
    gui_cube_imgfile_t imgfile =
    {
        .flg_fs = false,
        .data_addr.data_addr_front = ACTIVITY_BIN,
        .data_addr.data_addr_back = WEATHER_BIN,
        .data_addr.data_addr_up =  HEARTRATE_BIN,
        .data_addr.data_addr_down = CLOCKN_BIN,
        .data_addr.data_addr_left = MUSIC_BIN,
        .data_addr.data_addr_right = QUICKCARD_BIN
    };
#endif
    gui_cube_t *cube4 = gui_cube_create(parent, "cube", &imgfile, 0, 0);
    // "Clockn.bin"
    gui_cube_set_mode(cube4, CUBE_SIDE_DOWN, IMG_SRC_OVER_MODE);
    gui_cube_set_size(cube4, 100);
    gui_cube_set_center(cube4, 200, 200);
    gui_win_t *win_function = gui_win_create(parent, "win_function", 0, 0, 368, 448);
    gui_obj_add_event_cb(win_function, (gui_event_cb_t)callback_prism, GUI_EVENT_TOUCH_LONG, NULL);
}
