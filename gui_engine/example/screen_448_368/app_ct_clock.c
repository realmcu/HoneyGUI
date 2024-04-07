#include "root_image_hongkong/ui_resource.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_watch_gradient_spot.h"
#include "gui_text.h"
#include "gui_watchface_gradient.h"
#include "gui_cardview.h"
#include "gui_card.h"
#include "gui_tab.h"
#include "gui_app.h"

gui_win_t *win_watch;
gui_img_t *img;
gui_watch_gradient_spot_t *watch;
gui_text_t *rate;
static T_GUI_WATCHFACE_GRADIENT *canvas;
gui_tabview_t *tablist_tab;

// static void canvas_cb(gui_canvas_t *canvas)
// {
//     nvgRect(canvas->vg, 0, 0, 368, 448);
//     nvgFillColor(canvas->vg, nvgRGBA(30, 30, 30, 255));
//     nvgFill(canvas->vg);
// }

static void callback_time()
{
    int idx = tablist_tab->cur_id.x;
    gui_log("idx:%d\n", idx);
    gui_app_t *app = (gui_app_t *)get_app_hongkong();
    gui_obj_t *screen = &(app->screen);
    gui_tree_free(screen);
    app->ui_design(app);
    switch (idx)
    {
    case 0:
        {
            GET_BASE(img)->not_show = false;
            GET_BASE(watch)->not_show = true;
            GET_BASE(canvas)->not_show = true;
        }
        break;
    case 1:
        {
            GET_BASE(img)->not_show = true;
            GET_BASE(watch)->not_show = false;
            GET_BASE(canvas)->not_show = true;
        }
        break;
    case 2:
        {
            GET_BASE(img)->not_show = true;
            GET_BASE(watch)->not_show = true;
            GET_BASE(canvas)->not_show = false;
        }
        break;
    default:
        break;
    }

}

static void callback_touch_long(void *obj, gui_event_t e)
{
    gui_log("win widget long touch enter cb\n");
    gui_app_t *app = (gui_app_t *)get_app_hongkong();
    gui_obj_t *screen = &(app->screen);
    int idx = 0;
    if (!GET_BASE(img)->not_show)
    {
        idx = 0;
    }
    if (!GET_BASE(watch)->not_show)
    {
        idx = 1;
    }
    if (!GET_BASE(canvas)->not_show)
    {
        idx = 2;
    }

    gui_tree_free(screen);
    gui_win_t *win = gui_win_create(screen, "win", 0, 0, 320, 320);
    gui_obj_add_event_cb(win, (gui_event_cb_t)callback_time, GUI_EVENT_TOUCH_CLICKED, NULL);

    tablist_tab = gui_tabview_create(win, "tabview", 59, 84, 250, 300);
    gui_tabview_set_style(tablist_tab, CLASSIC);
    gui_tab_t *tb_watchface = gui_tab_create(tablist_tab, "tb_watchface",   0, 0, 250, 0, 2, 0);
    gui_tab_t *tb_watch = gui_tab_create(tablist_tab, "tb_watch",           0, 0, 250, 0, 1, 0);

    gui_tab_t *tb_clock = gui_tab_create(tablist_tab, "tb_clock",           0, 0, 250, 0, 0, 0);
    gui_tabview_jump_tab(tablist_tab, idx, 0);
    extern void tablist_clock(void *parent);
    tablist_clock(tb_clock);
    extern void tablist_watch(void *parent);
    tablist_watch(tb_watch);
    extern void tablist_watchface(void *parent);
    tablist_watchface(tb_watchface);

}

static void callback_watchface()
{
    static char clock;
    if (clock == 1)
    {
        GET_BASE(img)->not_show = false;
        GET_BASE(watch)->not_show = true;
        GET_BASE(canvas)->not_show = true;
        clock = 2;
    }
    else if (clock == 2)
    {
        GET_BASE(img)->not_show = true;
        GET_BASE(watch)->not_show = false;
        GET_BASE(canvas)->not_show = true;
        clock = 0;
    }
    else if (clock == 0)
    {
        GET_BASE(img)->not_show = true;
        GET_BASE(watch)->not_show = true;
        GET_BASE(canvas)->not_show = false;
        clock = 1;
    }
}



void page_ct_clock(void *parent)
{
    win_watch = gui_win_create(parent, "win", 0, 0, 368, 448);

    gui_obj_add_event_cb(win_watch, (gui_event_cb_t)callback_touch_long, GUI_EVENT_TOUCH_LONG, NULL);

    img = gui_img_create_from_mem(parent, "page0", CLOCKN_BIN, 0, 0, 0, 0);
    gui_img_set_mode(img, IMG_SRC_OVER_MODE);
    watch = gui_watch_gradient_spot_create(win_watch, "watchface", 0, 0, 0, 0);
    gui_watch_gradient_spot_set_center(watch, 368 / 2, 448 / 2);
    canvas = gui_watchface_gradient_create(parent, "watchface_gradient", (368 - 368) / 2,
                                           (448 - 448) / 2, 368, 448);
    GET_BASE(watch)->not_show = true;
    GET_BASE(canvas)->not_show = true;

}

