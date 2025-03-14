#include "guidef.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_img.h"
#include "gui_obj.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "tiger_blue.txt"
// #include "tiger_grey.txt"
// #include "tiger_laven.txt"
// #include "tiger_lime.txt"
// #include "tiger_turk.txt"
// #include "tiger_white.txt"
// #include "tiger_yellow.txt"
#include "gui_server.h"
#include "gui_components_init.h"
// #include "gui_cube.h"
// #include "gui_win.h"
// #include "gui_canvas_arc.h"
// #include "gui_canvas_round_rect.h"
#include "tiger_blue_compressed.txt"
// #include "acc_sw_rle.h"


void test_cb(void *arg)
{
    gui_log("test_cb\n");
}




static int app_init(void)
{
    gui_win_t *win = gui_win_create(gui_obj_get_root(), "win_test", 0, 0, 0, 0);

    gui_img_t *img = gui_img_create_from_mem(win,  "img_1_test", (void *)_actiger_blue_compressed, 0, 0,
                                             0, 0);

    gui_obj_set_timer(&(img->base), 1000, true, test_cb);

    return 0;
}

GUI_INIT_APP_EXPORT(app_init);


