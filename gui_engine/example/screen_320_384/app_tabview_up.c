#include "root_image_hongkong_bee4/ui_resource.h"
#include "gui_img.h"
#include "gui_curtainview.h"
#include "gui_curtain.h"
#include "gui_tabview.h"
#include "gui_tab.h"
#include "gui_canvas.h"

void tabview_up_design(void *parent_widget)
{
#ifdef RTL87x2G
    gui_img_t *bg = gui_img_create_from_mem(parent_widget, "bg_up", ZIP_RECT_320_384_BIN, 0,
                                            0,
                                            0, 0);
#else
    gui_img_t *bg = gui_img_create_from_mem(parent_widget, "bg_up", RECT_320_384_BIN, 0, 0,
                                            0, 0);
#endif // RTL87x2G
    gui_img_set_opacity(bg, 128);
    gui_tabview_t *tv_up = gui_tabview_create(parent_widget, "tabview_up", 0, 0, 0, 0);
    gui_tabview_set_style(tv_up, CLASSIC);
    gui_tab_t *tb_control0 = gui_tab_create(tv_up, "tb_control0",    0, 0, 0, 0, 0, 0);
    gui_tab_t *tb_control1 = gui_tab_create(tv_up, "tb_control1",    0, 0, 0, 0, 1, 0);
    extern void page_tb_up_control0(void *parent);
    extern void page_tb_up_control1(void *parent);
    page_tb_up_control0(tb_control0);
    page_tb_up_control1(tb_control1);
}
