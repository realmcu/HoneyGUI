#include "guidef.h"
#include "gui_gray.h"
#include "gui_obj.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "gui_server.h"
#include "gui_components_init.h"
#include "../../assets/test-gray16.txt"
#include "../../assets/test_gray256.c"


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

    gui_gray_t *gray = gui_gray_create(gui_obj_get_root(), "gray", (void *)_test_gray16, 240, 240, 480,
                                       480);


    gui_obj_add_event_cb(gray, (gui_event_cb_t)test_event_cb, GUI_EVENT_TOUCH_CLICKED, NULL);

    gui_obj_create_timer(&(gray->base), 1000, true, test_timer_cb);
    gui_obj_start_timer(&(gray->base));

    return 0;
}

GUI_INIT_APP_EXPORT(app_init);


