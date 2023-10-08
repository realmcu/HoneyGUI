#include "root_image_hongkong/ui_resource.h"
#include <gui_magic_img.h>
#include "gui_curtainview.h"
#include "gui_curtain.h"
#include "gui_canvas.h"

void curtainview_design(void *parent_widget)
{
    gui_curtainview_t *ct = gui_curtainview_create(parent_widget, "ct", 0, 0, 368, 448);
    GET_BASE(ct)->cover = true;
    gui_curtain_t *ct_clock = gui_curtain_create(ct, "1", 0, 0, 368, 448, CURTAIN_MIDDLE, 1);
    gui_curtain_t *ct_control0 = gui_curtain_create(ct, "2", 0, 0, 368, 448, CURTAIN_UP, 1);
    gui_curtain_t *ct_left = gui_curtain_create(ct, "3", 0, 0, 368, 448, CURTAIN_LEFT, 0.8f);
    extern void page_ct_clock(void *parent);
    extern void page_ct_sidebar(void *parent);
    page_ct_clock(ct_clock);
    page_ct_sidebar(ct_left);

    extern void tabview_up_design(void *parent_widget);
    tabview_up_design(ct_control0);
}
