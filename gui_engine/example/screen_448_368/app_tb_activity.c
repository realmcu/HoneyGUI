#include "root_image_hongkong/ui_resource.h"
#include <gui_magic_img.h>
#include "gui_win.h"

extern void callback_prism(void *obj, gui_event_t e);
void page_tb_activity(void *parent)
{
    gui_magic_img_create_from_mem(parent, "page1", ACTIVITY_BIN, 0, 0, 0, 0);
    gui_win_t *win_function = gui_win_create(parent, "win_function", 0, 0, 368, 448);
    gui_obj_add_event_cb(win_function, (gui_event_cb_t)callback_prism, GUI_EVENT_TOUCH_LONG, NULL);
}