#include "guidef.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_img.h"
#include "gui_gray.h"
#include "gui_obj.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "gui_server.h"
#include "gui_components_init.h"
#include "../../assets/tiger_blue_compressed.txt"



void test_event_cb(void *obj, gui_event_t e, void *param)
{
    gui_obj_t *this = (gui_obj_t *)obj;
    gui_log("Event test obj name = %s, e = 0x%x !\n", this->name, e);
}

void test_timer_cb(void *param)
{
    gui_log("timer cb test!\n");
}




static int app_init(void)
{
    void *addr = (void *)_actiger_blue_compressed;

    gui_img_t *img = gui_img_create_from_mem(gui_obj_get_root(),  "img_1_test", addr, 0, 0, 0, 0);
    // gui_img_set_location(img, 50, 50);
    gui_img_rotation(img, 45.0f, 0, 0);

    gui_obj_add_event_cb(img, (gui_event_cb_t)test_event_cb, GUI_EVENT_TOUCH_CLICKED, NULL);


    gui_obj_create_timer(&(img->base), 1000, true, test_timer_cb);
    gui_obj_start_timer(&(img->base));

    return 0;
}

GUI_INIT_APP_EXPORT(app_init);


