#include "root_image_hongkong/ui_resource.h"
#include <gui_magic_img.h>
#include "gui_win.h"
#include "gui_watch_gradient_spot.h"
gui_win_t *win_watch;
gui_magic_img_t *img;
gui_watch_gradient_spot_t *watch;
static void callback_watchface()
{
    static bool clock;
    if (clock)
    {

        GET_BASE(img)->not_show = false;
        GET_BASE(watch)->not_show = true;
    }
    else
    {
        GET_BASE(img)->not_show = true;
        GET_BASE(watch)->not_show = false;
    }
    clock = !clock;


}
void page_ct_clock(void *parent)
{
    win_watch = gui_win_create(parent, "win", 0, 0, 368, 448);
    gui_obj_add_event_cb(win_watch, (gui_event_cb_t)callback_watchface, GUI_EVENT_TOUCH_CLICKED, NULL);
    img = gui_magic_img_create_from_mem(parent, "page0", CLOCKN_BIN, 0, 0, 0, 0);
    watch = gui_watch_gradient_spot_create(win_watch, "watchface", 0, 0, 0, 0);
    gui_watch_gradient_spot_set_center(watch, 368 / 2, 448 / 2);
    GET_BASE(watch)->not_show = true;
}
