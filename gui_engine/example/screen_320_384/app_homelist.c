#include <gui_tabview.h>
#include "gui_cardview.h"
#include "gui_card.h"
#include <gui_obj.h>
#include <gui_win.h>
#include <gui_text.h>
#include <gui_curtain.h>
#include "root_image_hongkong_bee4/ui_resource.h"
#include <gui_app.h>
#include "gui_tab.h"
#include "gui_perspective.h"
#include "draw_font.h"
#include <gui_img.h>
#include "gui_switch.h"
#include "gui_canvas.h"
#include "gui_img.h"

static void app_homelist_ui_design(gui_app_t *app);
gui_tabview_t *homelist_tab;


extern void design_homelist_0(void *parent);
extern void design_homelist_1(void *parent);
extern void design_homelist_2(void *parent);
extern void design_homelist_3(void *parent);
extern void design_homelist_4(void *parent);
extern void design_homelist_5(void *parent);
extern void design_homelist_6(void *parent);
extern void design_homelist_7(void *parent);

static void callback_tab();
static gui_app_t app_homelist =
{
    .screen =
    {
        .name = "app_homelist",
        .x    = 0,
        .y    = 0,
    },
    .ui_design = app_homelist_ui_design,
    .thread_entry = NULL,
    .active_ms = 1000000,
};

void *get_app_homelist(void)
{
    return &app_homelist;
}

static void app_homelist_ui_design(gui_app_t *app)
{
    gui_log("app_hk_bee4_ui_design\n");

    homelist_tab = gui_tabview_create(&(app->screen), "tabview", 55, 72, 210, 240);
    gui_tabview_set_style(homelist_tab, CLASSIC);

    gui_tab_t *tab0 = gui_tab_create(homelist_tab, "tab0", 0, 0, 210, 0, 0, 0);
    gui_tab_t *tab1 = gui_tab_create(homelist_tab, "tab1", 0, 0, 210, 0, 1, 0);
    gui_tab_t *tab2 = gui_tab_create(homelist_tab, "tab2", 0, 0, 210, 0, 2, 0);
    gui_tab_t *tab3 = gui_tab_create(homelist_tab, "tab3", 0, 0, 210, 0, 3, 0);
    gui_tab_t *tab4 = gui_tab_create(homelist_tab, "tab4", 0, 0, 210, 0, 4, 0);
    gui_tab_t *tab5 = gui_tab_create(homelist_tab, "tab5", 0, 0, 210, 0, 5, 0);
    gui_tab_t *tab6 = gui_tab_create(homelist_tab, "tab6", 0, 0, 210, 0, 6, 0);
    gui_tab_t *tab7 = gui_tab_create(homelist_tab, "tab7", 0, 0, 210, 0, 7, 0);


    design_homelist_0(tab0);
    design_homelist_1(tab1);
    design_homelist_2(tab2);
    design_homelist_3(tab3);
    design_homelist_4(tab4);
    design_homelist_5(tab5);
    design_homelist_6(tab6);
    design_homelist_7(tab7);
}
