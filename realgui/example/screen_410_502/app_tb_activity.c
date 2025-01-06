#include "root_image_hongkong/ui_resource.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_text.h"
#include "gui_app.h"
#include "time.h"
#include "gui_curtainview.h"
#include <math.h>
#include "cJSON.h"
#include "gui_canvas_img.h"
#include "gui_curtain.h"
#include "gui_canvas_rect.h"
#include "guidef.h"
#include <gui_tabview.h>

#define SCREEN_WIDTH 410
#define SCREEN_HEIGHT 502

extern gui_tabview_t *tv;
static float progress = 0;
static bool tab_change = 0;
static bool draw_flag = 0;

static void win_cb(gui_obj_t *obj)
{
    // gui_log("tv_name: %s, tv->cur_id.x = %d\n", GUI_BASE(tv)->name, tv->cur_id.x);
    if (!tab_change)
    {
        if (tv->cur_id.x == 2)
        {
            extern void activity_app(gui_obj_t *obj);
            activity_app(obj);
        }
    }
    if (tv->cur_id.x != 2)
    {
        tab_change = 0;
    }
    else
    {
        tab_change = 1;
    }
}

void page_tb_activity(void *parent)
{
    // tv = (gui_tabview_t *)(GUI_BASE(parent)->parent->parent);

    gui_win_t *win = gui_win_create(parent, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    gui_win_set_animate(win, 2000, -1, (gui_animate_callback_t)win_cb, GUI_BASE(win));
}