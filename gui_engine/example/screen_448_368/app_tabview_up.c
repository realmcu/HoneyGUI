#include "root_image_hongkong/ui_resource.h"
#include <gui_img.h>
#include "gui_curtainview.h"
#include "gui_curtain.h"
#include "gui_tabview.h"
#include "gui_tab.h"
#include "gui_canvas.h"
static void canvas_cb_black(gui_canvas_t *canvas)
{
    nvgRect(canvas->vg, 0, -448, 368, 448 * 2);
    nvgFillColor(canvas->vg, nvgRGBA(0, 0, 0, 150));
    nvgFill(canvas->vg);
}
void tabview_up_design(void *parent_widget)
{
    gui_canvas_t *canvas = gui_canvas_create(parent_widget, "canvas", 0, 0, 0, 368, 448);
    gui_canvas_set_canvas_cb(canvas, canvas_cb_black);
    gui_tabview_t *tv_up = gui_tabview_create(parent_widget, "tabview_up", 0, 0, 0, 0);
    gui_tabview_set_style(tv_up, CLASSIC);
    gui_tab_t *tb_control0 = gui_tab_create(tv_up, "tb_control0",    0, 0, 0, 0, 0, 0);
    gui_tab_t *tb_control1 = gui_tab_create(tv_up, "tb_control1",    0, 0, 0, 0, 1, 0);
    extern void page_tb_control0(void *parent);
    extern void page_tb_control1(void *parent);
    page_tb_control0(tb_control0);
    page_tb_control1(tb_control1);
}
