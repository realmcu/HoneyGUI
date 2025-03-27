#include "guidef.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_img.h"
#include "gui_gray.h"
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
#include "test-gray16.txt"


void test_cb(void *obj, gui_event_t e, void *param)
{
    gui_img_t *img = (gui_img_t *)obj;
    gui_log("obj name = %s, e = 0x%x\n", img->base.name, e);
}




static int app_init(void)
{
    gui_win_t *win = gui_win_create(gui_obj_get_root(), "win_test", 0, 0, 0, 0);

    // gui_obj_add_event_cb(win, (gui_event_cb_t)test_cb, GUI_EVENT_TOUCH_CLICKED, NULL);


    gui_img_t *img = gui_img_create_from_mem(win,  "img_1_test", (void *)_actiger_blue_compressed, 0, 0,
                                             0, 0);

    // gui_gray_t *gray = gui_gray_create(gui_obj_get_root(), "gray", (void *)_test_gray16, 240, 240, 480, 480);


    // gui_obj_add_event_cb(img, (gui_event_cb_t)test_cb, GUI_EVENT_TOUCH_MOVE_LEFT, NULL);
    // gui_obj_add_event_cb(img, (gui_event_cb_t)test_cb, GUI_EVENT_TOUCH_MOVE_RIGHT, NULL);

    // gui_obj_create_timer(&(img->base), 1000, true, test_cb);
    // gui_obj_start_timer(&(img->base));

    return 0;
}

GUI_INIT_APP_EXPORT(app_init);


