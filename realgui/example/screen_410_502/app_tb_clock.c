/* curtain example start*/
#include "root_image_hongkong/ui_resource.h"
#include <gui_img.h>
#include "gui_curtainview.h"
#include "gui_curtain.h"
#include "gui_win.h"
#include "gui_perspective.h"
#include "gui_app.h"
#include "guidef.h"
#include <tp_algo.h>
#include "app_hongkong.h"
#include <time.h>
#include "gui_tab.h"

#define SCREEN_WIDTH 410
#define SCREEN_HEIGHT 502

static gui_curtainview_t *ct;
static gui_curtain_t *ct_clock;
static gui_curtain_t *ct_inform;
static gui_curtain_t *ct_left;
static gui_curtain_t *ct_card;
extern gui_win_t *win_watch; // *win_market;
static gui_win_t *win_touch;
bool sidebar_flag = 0;
uint8_t watchface_index = 0;
char watchface_path[100];

struct tm *timeinfo;
static struct tm watch_time;

static void curtain_ctr_cb(gui_win_t *win)
{
    if (ct->cur_curtain == CURTAIN_DOWN || ct->cur_curtain == CURTAIN_UP)
    {
        GUI_BASE(ct_clock)->active = 0;
        GUI_BASE(ct_clock)->gesture = 1;
        GUI_BASE(ct_clock)->not_show = 1;
        sidebar_flag = 0;
    }
    else if (ct->cur_curtain == CURTAIN_LEFT)
    {
        sidebar_flag = 1;
    }
    else
    {
        GUI_BASE(ct_clock)->active = 1;
        GUI_BASE(ct_clock)->gesture = 0;
        GUI_BASE(ct_clock)->not_show = 0;
        sidebar_flag = 0;
    }
    // update time
#if defined __WIN32
    time_t rawtime;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
#else
    // extern struct tm watch_clock_get(void);
    // watch_time = watch_clock_get();
    timeinfo = &watch_time;
#endif
}

// gui_perspective_t *perspect;
// static void callback_prism_touch_clicked()
// {
//     int angle = perspect->release_x;
//     gui_log("angel1 = %d\n", angle);
//     if (angle < 0)
//     {
//         angle += 360;
//     }
//     angle = (angle % 360) / (360 / 6);
//     gui_log("angel2 = %d\n", angle);
//     if (angle < 0 || angle > 5)
//     {
//         angle = 0;
//     }

//     gui_app_t *app = get_app_hongkong();
//     gui_obj_t *screen = &(app->screen);
//     gui_obj_tree_free(screen);
//     app->ui_design(app);
//     switch (angle % 2)
//     {
//     case 0:
//         {
//             // GUI_BASE(win_watch)->not_show = false;
//             // GUI_BASE(win_market)->not_show = true;
//             watchface_index = 0;
//         }
//         break;
//     case 1:
//         {
//             // GUI_BASE(win_watch)->not_show = true;
//             // GUI_BASE(win_market)->not_show = false;
//             watchface_index = 1;
//         }
//         break;
//     default:
//         break;
//     }
//     gui_fb_change();
// }

/* callback_touch_long start*/
static void callback_touch_long(void *obj, gui_event_t e)
{
    gui_log("win widget long touch enter cb\n");
    extern void close_box2d_ring(void);
    close_box2d_ring();

    // !!!if use this APP on EVB, need to add romfs.c, romfs.h and update gui_port_filesystem.c
    extern gui_app_t  *_get_app_APP_WATCHFACE_MARKET_handle(void);
    gui_app_layer_top();
    gui_switch_app(gui_current_app(), _get_app_APP_WATCHFACE_MARKET_handle());
    return;
}

void page_tb_clock(void *parent)
{
    ct = gui_curtainview_create(parent, "ct_clock", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    ct_clock = gui_curtain_create(ct, "_watchface_main_window_", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
                                  CURTAIN_MIDDLE, 1);
    ct_inform = gui_curtain_create(ct, "2", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, CURTAIN_UP, 1);
    ct_left = gui_curtain_create(ct, "3", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, CURTAIN_LEFT, 0.34f);

    ct_card = gui_curtain_create(ct, "card", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, CURTAIN_DOWN, 1);

    gui_win_t *win = gui_win_create(parent, "", 0, 0, 0, 0);
    gui_win_set_animate(win, 2000, -1, (gui_animate_callback_t)curtain_ctr_cb, (void *)win);
    extern void page_ct_clock(void *parent);
    extern void page_ct_sidebar(void *parent);
    extern void tabview_up_design(void *parent_widget);
    extern void curtain_down_design(void *parent_widget);

    // app_watchface_m
    page_ct_sidebar(ct_left);
    tabview_up_design(ct_inform);
    curtain_down_design(ct_card);
    switch (watchface_index)
    {
    case 0:
        {
            page_ct_clock(ct_clock);
        }
        break;
    case 1:
        {
            extern void create_watchface_ring(void *parent);
            create_watchface_ring(ct_clock);
        }
        break;
    case 2:
        {
            extern void create_watchface_bf(void *parent);
            create_watchface_bf(ct_clock);
        }
        break;
    case 3:
        {
            extern void create_tree_nest(const char *xml, void *obj);
            create_tree_nest((void *)watchface_path, ct_clock);
        }
    default:
        // page_ct_clock(ct_clock);
        break;
    }

    if (sidebar_flag)
    {
        ct->cur_curtain = CURTAIN_LEFT;
    }
    win_touch = gui_win_create(ct_clock, "win_touch", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    gui_obj_add_event_cb(win_touch, (gui_event_cb_t)callback_touch_long, GUI_EVENT_TOUCH_LONG, NULL);
}
/* curtain example end*/

