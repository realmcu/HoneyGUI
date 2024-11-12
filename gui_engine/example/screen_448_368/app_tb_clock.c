// /* curtain example start*/
// #include "root_image_hongkong/ui_resource.h"
// #include <gui_img.h>
// #include "gui_curtainview.h"
// #include "gui_curtain.h"

// void page_tb_clock(void *parent)
// {
//     gui_curtainview_t *ct = gui_curtainview_create(parent, "ct_clock", 0, 0, 368, 448);
//     gui_curtain_t *ct_clock = gui_curtain_create(ct, "1", 0, 0, 368, 448, CURTAIN_MIDDLE, 1);
//     gui_curtain_t *ct_control0 = gui_curtain_create(ct, "2", 0, 0, 368, 448, CURTAIN_UP, 1);
//     gui_curtain_t *ct_left = gui_curtain_create(ct, "3", 0, 0, 368, 448, CURTAIN_LEFT, 0.65f);

//     gui_curtain_t *ct_card = gui_curtain_create(ct, "card", 0, 0, 368, 448, CURTAIN_DOWN, 1);
//     extern void page_ct_clock(void *parent);
//     extern void page_ct_sidebar(void *parent);
//     extern void tabview_up_design(void *parent_widget);
//     extern void curtain_down_design(void *parent_widget);
//     page_ct_clock(ct_clock);
//     page_ct_sidebar(ct_left);
//     tabview_up_design(ct_control0);
//     curtain_down_design(ct_card);
// }
// /* curtain example end*/

/* curtain example start*/
#include "root_image_hongkong/ui_resource.h"
#include <gui_img.h>
#include "gui_curtainview.h"
#include "gui_curtain.h"
#include "gui_win.h"
#include "gui_perspective.h"
#include "gui_app.h"
#include "guidef.h"

#define SCREEN_WIDTH 368   //410
#define SCREEN_HEIGHT 448  //502
#define SCREEN_X_OFF 0     //21
#define SCREEN_Y_OFF 0     //18

static gui_curtainview_t *ct;
static gui_curtain_t *ct_clock;
static gui_curtain_t *ct_control0;
static gui_curtain_t *ct_left;
static gui_curtain_t *ct_card;
extern gui_win_t *win_market, *win_watch;
static void curtain_ctr_cb()
{
    // touch_info_t *tp = tp_get_info();
    // if (tp->pressing)
    // {
    //     gui_log("x = %d, y = %d\n", tp->x,  tp->y);
    // }

    if (ct->cur_curtain == CURTAIN_DOWN || ct->cur_curtain == CURTAIN_UP)
    {
        GUI_BASE(ct_clock)->active = 0;
        GUI_BASE(ct_clock)->gesture = 1;
        GUI_BASE(ct_clock)->not_show = 1;
    }
    else
    {
        GUI_BASE(ct_clock)->active = 1;
        GUI_BASE(ct_clock)->gesture = 0;
        GUI_BASE(ct_clock)->not_show = 0;
    }
}

gui_perspective_t *perspect;
static void callback_prism_touch_clicked()
{
    int angle = perspect->release_x;
    gui_log("angel1 = %d\n", angle);
    if (angle < 0)
    {
        angle += 360;
    }
    angle = (angle % 360) / (360 / 6);
    gui_log("angel2 = %d\n", angle);
    if (angle < 0 || angle > 5)
    {
        angle = 0;
    }

    gui_app_t *app = get_app_hongkong();
    gui_obj_t *screen = &(app->screen);
    gui_obj_tree_free(screen);
    app->ui_design(app);
    switch (angle % 2)
    {
    case 0:
        {
            GUI_BASE(win_watch)->not_show = false;
            GUI_BASE(win_market)->not_show = true;
        }
        break;
    case 1:
        {
            GUI_BASE(win_watch)->not_show = true;
            GUI_BASE(win_market)->not_show = false;
        }
        break;
    default:
        break;
    }
    gui_fb_change();
}

/* callback_touch_long start*/
static void callback_touch_long(void *obj, gui_event_t e)
{
    gui_log("win widget long touch enter cb\n");
    gui_app_t *app = (gui_app_t *)get_app_hongkong();
    gui_obj_t *screen = &(app->screen);

    gui_obj_tree_free(screen);
    gui_win_t *win = gui_win_create(screen, "win", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    gui_perspective_imgfile_t imgfile =
    {
        .src_mode[0] = IMG_SRC_MEMADDR, .src_mode[1] = IMG_SRC_MEMADDR, .src_mode[2] = IMG_SRC_MEMADDR,
        .src_mode[3] = IMG_SRC_MEMADDR, .src_mode[4] = IMG_SRC_MEMADDR, .src_mode[5] = IMG_SRC_MEMADDR,
        .data_addr[0] = UI_CLOCK_FACE_MAIN_BIN,
        .data_addr[1] = SLICE1_BIN,
        .data_addr[2] = UI_CLOCK_FACE_MAIN_BIN,
        .data_addr[3] = SLICE1_BIN,
        .data_addr[4] = UI_CLOCK_FACE_MAIN_BIN,
        .data_addr[5] = SLICE1_BIN
    };
    perspect = gui_perspective_create(win, "test", &imgfile, 0, 0);

    gui_obj_add_event_cb(win, (gui_event_cb_t)callback_prism_touch_clicked, GUI_EVENT_TOUCH_CLICKED,
                         NULL);
    gui_fb_change();
}

void page_tb_clock(void *parent)
{
    ct = gui_curtainview_create(parent, "ct_clock", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    ct_clock = gui_curtain_create(ct, "1", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, CURTAIN_MIDDLE, 1);
    ct_control0 = gui_curtain_create(ct, "2", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, CURTAIN_UP, 1);
    ct_left = gui_curtain_create(ct, "3", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, CURTAIN_LEFT, 0.65f);

    ct_card = gui_curtain_create(ct, "card", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, CURTAIN_DOWN, 1);

    gui_win_t *win = gui_win_create(parent, "", 0, 0, 0, 0);
    gui_win_set_animate(win, 1000, -1, curtain_ctr_cb, NULL);
    extern void page_ct_clock(void *parent);
    extern void app_watchface_market(void *parent);
    extern void page_ct_sidebar(void *parent);
    extern void tabview_up_design(void *parent_widget);
    extern void curtain_down_design(void *parent_widget);
    app_watchface_market(ct_clock);
    page_ct_clock(ct_clock);
    page_ct_sidebar(ct_left);
    tabview_up_design(ct_control0);
    curtain_down_design(ct_card);
    // GUI_BASE(win_watch)->not_show = 1;
    gui_win_t *win_touch = gui_win_create(ct_clock, "win_touch", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    gui_obj_add_event_cb(win_touch, (gui_event_cb_t)callback_touch_long, GUI_EVENT_TOUCH_LONG, NULL);
}
/* curtain example end*/

