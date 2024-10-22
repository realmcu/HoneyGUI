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
static gui_curtainview_t *ct;
static gui_curtain_t *ct_clock;
static gui_curtain_t *ct_control0;
static gui_curtain_t *ct_left;
static gui_curtain_t *ct_card;
static void curtain_ctr_cb()
{
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

void page_tb_clock(void *parent)
{
    ct = gui_curtainview_create(parent, "ct_clock", 0, 0, 368, 448);
    ct_clock = gui_curtain_create(ct, "1", 0, 0, 368, 448, CURTAIN_MIDDLE, 1);
    ct_control0 = gui_curtain_create(ct, "2", 0, 0, 368, 448, CURTAIN_UP, 1);
    ct_left = gui_curtain_create(ct, "3", 0, 0, 368, 448, CURTAIN_LEFT, 0.65f);

    ct_card = gui_curtain_create(ct, "card", 0, 0, 368, 448, CURTAIN_DOWN, 1);

    gui_win_t *win = gui_win_create(parent, "", 0, 0, 0, 0);
    gui_win_set_animate(win, 1000, -1, curtain_ctr_cb, NULL);
    extern void page_ct_clock(void *parent);
    extern void page_ct_sidebar(void *parent);
    extern void tabview_up_design(void *parent_widget);
    extern void curtain_down_design(void *parent_widget);
    page_ct_clock(ct_clock);
    page_ct_sidebar(ct_left);
    tabview_up_design(ct_control0);
    curtain_down_design(ct_card);
}
/* curtain example end*/

