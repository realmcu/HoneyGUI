#include "root_image_hongkong_bee4/ui_resource.h"
#include "gui_img.h"
#include "gui_curtainview.h"
#include "gui_curtain.h"
#include "gui_canvas.h"

extern void design_tab_home(void *parent);
extern void page_left_figure(void *parent);
extern void page_down_message_design(void *parent);
extern void tabview_up_design(void *parent_widget);
void design_tv_home(void *parent)
{
    gui_curtainview_t *ct = gui_curtainview_create(parent, "ct", 0, 0, 320, 384);
    GET_BASE(ct)->cover = true;

    gui_curtain_t *ct_mid = gui_curtain_create(ct, "ct_mid", 0, 0, 320, 384, CURTAIN_MIDDLE, 1);
    gui_curtain_t *ct_up = gui_curtain_create(ct, "ct_up", 0, 0, 320, 384, CURTAIN_UP, 1);
    gui_curtain_t *ct_down = gui_curtain_create(ct, "ct_down", 0, 0, 320, 384, CURTAIN_DOWN, 1);
    gui_curtain_t *ct_left = gui_curtain_create(ct, "ct_left", 0, 0, 320, 384, CURTAIN_LEFT, 0.775f);

    design_tab_home(ct_mid);
    tabview_up_design(ct_up);
    page_down_message_design(ct_down);
    page_left_figure(ct_left);

}
