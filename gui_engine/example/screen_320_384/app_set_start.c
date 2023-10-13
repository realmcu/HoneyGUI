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
#include <gui_magic_img.h>
#include "gui_switch.h"
#include "gui_canvas.h"
#include "gui_img.h"

static void app_set_start_ui_design(gui_app_t *app);
gui_tabview_t *tablist_tab_v;
gui_img_t *tablist_set_start;
gui_img_t *tablist_sos_call;
gui_img_t *tablist_digital;

static gui_app_t app_set_start =
{
    .screen =
    {
        .name = "app_hongkong",
        .x    = 0,
        .y    = 0,
    },
    .ui_design = app_set_start_ui_design,
    .thread_entry = NULL,
    .active_ms = 1000000,
};

void *get_app_set_start(void)
{
    return &app_set_start;
}
gui_win_t *dynamic_set_start;
int idx = 0;
extern void exit_app_set_start(void *obj, gui_event_t e);
extern void enter_app_set_start(void *obj, gui_event_t e);

void tab_go_to1(void *obj, gui_event_t e)
{

    gui_tabview_jump_tab(tablist_tab_v, 1, 0);

}
void tab_callback_to0(void *obj, gui_event_t e)
{

    gui_tabview_jump_tab(tablist_tab_v, 0, 0);

}
void tab_go_to2(void *obj, gui_event_t e)
{

    gui_tabview_jump_tab(tablist_tab_v, 2, 0);

}
void tab_callback_to1(void *obj, gui_event_t e)
{

    gui_tabview_jump_tab(tablist_tab_v, 1, 0);

}

static void app_set_start_ui_design(gui_app_t *app)
{
    gui_log("app_set_start_ui_design\n");


    tablist_tab_v = gui_tabview_create(&(app->screen), "tabview", 0, 0, 320, 384);
    gui_tabview_set_style(tablist_tab_v, CLASSIC);

    gui_tab_t *tab_digital = gui_tab_create(tablist_tab_v, "tb_blood",           0, 0, 0, 0, 2, 0);
    gui_tab_t *tab_call = gui_tab_create(tablist_tab_v, "tb_heart",           0, 0, 0, 0, 1, 0);
    gui_tab_t *tab_all = gui_tab_create(tablist_tab_v, "tab_sport",          0, 0, 0, 0, 0, 0);

    tablist_set_start = gui_img_create_from_mem(tab_all, "tablist_set_start", APP_MULT_SET_BIN, 0, 0, 0,
                                                0);
    tablist_sos_call = gui_img_create_from_mem(tab_call, "add", APP_SOS_CALL_BIN, 0, 0, 0, 0);
    tablist_digital = gui_img_create_from_mem(tab_digital, "add", APP_DIGITAL_BIN, 0, 0, 0, 0);
    //GET_BASE(tablist_sos_call)->not_show = true;
    //GET_BASE(tablist_digital)->not_show = true;
    //gui_tree_free(tablist_tab);
    // gui_tabview_jump_tab(tab_all, idx, 0);
    gui_win_t *win_start = gui_win_create(tab_all, "win_start", 0, 173, 320, 70);
    gui_obj_add_event_cb(win_start, (gui_event_cb_t)tab_go_to1, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_win_t *win_start_back = gui_win_create(tab_all, "win_start_back", 24, 24, 56, 56);
    gui_obj_add_event_cb(win_start_back, (gui_event_cb_t)exit_app_set_start, GUI_EVENT_TOUCH_CLICKED,
                         NULL);

    gui_win_t *win_call = gui_win_create(tab_call, "win_call_sos", 0, 291, 320, 70);
    gui_obj_add_event_cb(win_call, (gui_event_cb_t)tab_go_to2, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_win_t *win_call_back = gui_win_create(tab_call, "wiwin_call_sos", 24, 24, 56, 56);
    gui_obj_add_event_cb(win_call_back, (gui_event_cb_t)tab_callback_to0, GUI_EVENT_TOUCH_CLICKED,
                         NULL);

    gui_win_t *win_digital_back = gui_win_create(tab_digital, "win_digital_back", 24, 24, 56, 56);
    gui_obj_add_event_cb(win_digital_back, (gui_event_cb_t)tab_callback_to1, GUI_EVENT_TOUCH_CLICKED,
                         NULL);

}


