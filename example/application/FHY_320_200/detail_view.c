/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "app_main.h"
#include "common_data.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_text.h"
#include "gui_list.h"

/*============================================================================*
 *                           Types
 *============================================================================*/


/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "detail_view"

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void detail_view_design(gui_view_t *view);
static void update_page_indicator(void);

/*============================================================================*
 *                            Variables
 *============================================================================*/
/* View Management */
static gui_view_t *current_view = NULL;
static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = detail_view_design,
};

void (*detail_page_design_func)(gui_obj_t *parent) = NULL;
gui_view_descriptor_t *descriptor_rec = NULL;

/*============================================================================*
 *                           Private Functions
 *============================================================================*/
static int gui_view_descriptor_register_init(void)
{
    gui_view_descriptor_register(&descriptor);
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_REGISTER(gui_view_descriptor_register_init);

static void detail_view_design(gui_view_t *view)
{
    gui_obj_t *parent = GUI_BASE(view);
    gui_color_t font_color;
    if (theme_bg_white)
    {
        gui_obj_hidden((void *)screen_bg, false);
    }
    else
    {
        font_color = FG_1_DARK;
    }
    gui_img_create_from_mem(parent, "indicator0", PAGE_INDICATOR_BIN, 140, 188, 0, 0);
    gui_img_create_from_mem(parent, "indicator1", PAGE_INDICATOR_BIN, 152, 188, 0, 0);
    gui_img_create_from_mem(parent, "indicator2", PAGE_INDICATOR_BIN, 164, 188, 0, 0);
    gui_img_create_from_mem(parent, "indicator3", PAGE_INDICATOR_BIN, 176, 188, 0, 0);

    gui_img_t *bt = gui_img_create_from_mem(parent, 0, BLUETOOTH_BIN, 12, 13, 0, 0);
    gui_img_t *home_bg = gui_img_create_from_mem(parent, 0, STATUSBAR_HOME_BG_BIN, 131, 10, 0, 0);
    gui_img_t *barn_inner = gui_img_create_from_mem(home_bg, 0, BARN_INNER_BIN, 23, 6, 0, 0);
    gui_img_t *barn_outer = gui_img_create_from_mem(home_bg, 0, BARN_OUTER_BIN, 20, 3, 0, 0);
    gui_img_t *earphone_l = gui_img_create_from_mem(home_bg, 0, EARPLUG_L_BIN, 7, 3, 0, 0);
    gui_img_t *earphone_r = gui_img_create_from_mem(home_bg, 0, EARPLUG_R_BIN, 42, 3, 0, 0);

    if (theme_bg_white)
    {
        gui_img_set_a8_fg_color(bt, FG_1_LIGHT.color.argb_full);
        gui_img_set_a8_fg_color(home_bg, BG_2_LIGHT.color.argb_full);
        gui_img_set_a8_fg_color(barn_inner, FG_1_LIGHT.color.argb_full);
        gui_img_set_a8_fg_color(barn_outer, FG_1_LIGHT.color.argb_full);
        gui_img_set_a8_fg_color(earphone_l, FG_1_LIGHT.color.argb_full);
        gui_img_set_a8_fg_color(earphone_r, FG_1_LIGHT.color.argb_full);
    }
    else
    {
        gui_img_set_a8_fg_color(bt, FG_1_DARK.color.argb_full);
        gui_img_set_a8_fg_color(home_bg, BG_2_DARK.color.argb_full);
        gui_img_set_a8_fg_color(barn_inner, FG_1_DARK.color.argb_full);
        gui_img_set_a8_fg_color(barn_outer, FG_1_DARK.color.argb_full);
        gui_img_set_a8_fg_color(earphone_l, FG_1_DARK.color.argb_full);
        gui_img_set_a8_fg_color(earphone_r, FG_1_DARK.color.argb_full);
    }
}