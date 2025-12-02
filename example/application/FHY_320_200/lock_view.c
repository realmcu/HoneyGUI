/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "app_main.h"
#include "common_data.h"
#include "gui_img.h"
#include "gui_win.h"
#include "tp_algo.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME LOCK_VIEW

#define TAB_MAX_POS_X  180
#define TAB_UNLOCK_POS_X  102

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void lock_view_design(gui_view_t *view);

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
    .on_switch_in = lock_view_design,
};

static const char *descriptor_rec = NULL;

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

static void pressing_tab(void *obj)
{
    GUI_UNUSED(obj);

    touch_info_t *tp = tp_get_info();
    gui_img_t *img = (gui_img_t *)obj;
    if (tp->pressing && tp->x <= 128 && tp->y >= 28)
    {
        int16_t x = tp->deltaX;
        if (x < 0)
        {
            x = 0;
        }
        else if (x > TAB_MAX_POS_X)
        {
            x = TAB_MAX_POS_X;
        }
        gui_img_translate(img, (float)x, 0);
    }
    else if (img->t_x != 0)
    {
        int16_t x = (int16_t)img->t_x;
        const int16_t step = 20;
        if (x >= TAB_MAX_POS_X)
        {
            gui_view_switch_direct(current_view, QUICK_VIEW, SWITCH_OUT_NONE_ANIMATION,
                                   SWITCH_IN_ANIMATION_FADE);
            gui_obj_stop_timer(GUI_BASE(img));
            return;
        }
        else if (x >= TAB_UNLOCK_POS_X)
        {
            x += step;
        }
        else
        {
            x -= step;
        }
        if (x <= 0)
        {
            x = 0;
        }
        else if (x >= TAB_MAX_POS_X)
        {
            x = TAB_MAX_POS_X;
        }
        gui_img_translate(img, (float)x, 0);
    }
}

static void click_button_back(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    switch_from_lock_screen = false;
    gui_view_switch_direct(current_view, descriptor_rec, SWITCH_OUT_NONE_ANIMATION,
                           SWITCH_IN_NONE_ANIMATION);
}

static void lock_view_design(gui_view_t *view)
{
    gui_obj_t *parent = GUI_BASE(view);

    void *img_data;
    switch (wallpaper_index)
    {
    case 0:
        img_data = WALLPAPER_1_BIN;
        break;
    case 1:
        img_data = WALLPAPER_2_BIN;
        break;
    case 2:
        img_data = WALLPAPER_3_BIN;
        break;
    case 3:
        img_data = WALLPAPER_4_BIN;
        break;
    case 4:
        img_data = WALLPAPER_5_BIN;
        break;
    default:
        img_data = WALLPAPER_1_BIN;
        break;
    }
    gui_img_t *wallpaper = gui_img_create_from_mem(parent, 0, img_data, 0, 0, 0, 0);
    gui_img_set_mode(wallpaper, IMG_BYPASS_MODE);

    if (switch_from_lock_screen)
    {
        descriptor_rec = gui_view_get_current()->descriptor->name;
        gui_win_t *win_icon_back = (gui_win_t *)gui_win_create(view, 0, 0, 0, 52, 52);
        gui_img_t *icon_back = gui_img_create_from_mem(win_icon_back, 0, ICON_BACK_BIN, 0, 0, 0, 0);
        gui_obj_add_event_cb(win_icon_back, click_button_back, GUI_EVENT_TOUCH_CLICKED, NULL);
        if (theme_bg_white)
        {
            gui_img_a8_recolor(icon_back, FG_1_LIGHT.color.argb_full);
        }
        else
        {
            gui_img_a8_recolor(icon_back, FG_1_DARK.color.argb_full);
        }
    }
    else
    {
        bool theme_bg_rec = theme_bg_white;
        theme_bg_white = false;
        status_bar_design(parent);
        theme_bg_white = theme_bg_rec;
        if (!f_status.unlock_slider)
        {
            gui_view_switch_on_event(view, QUICK_VIEW, SWITCH_OUT_NONE_ANIMATION,
                                     SWITCH_IN_NONE_ANIMATION,
                                     GUI_EVENT_TOUCH_CLICKED);
            return;
        }
        else
        {
            gui_img_t *bg = gui_img_create_from_mem(parent, 0, BUTTON_BG_ELLIPSE_240_48_BIN, 40, 128, 0, 0);
            gui_img_a8_recolor(bg, 0xFFFFFF);
            gui_img_a8_mix_alpha(bg, 51);
            gui_img_t *unlock = gui_img_create_from_mem(bg, 0, ICON_UNLOCK_BIN, 192, 7, 0, 0);
            gui_img_a8_recolor(unlock, 0xFFFFFF);
            gui_img_a8_mix_alpha(unlock, 153);
            gui_img_t *tab = gui_img_create_from_mem(bg, 0, ICON_LOCK_TAB_BIN, 0, 0, 0, 0);
            gui_img_set_mode(tab, IMG_2D_SW_SRC_OVER_MODE);
            gui_obj_create_timer(GUI_BASE(tab), 10, true, pressing_tab);
            gui_obj_start_timer(GUI_BASE(tab));
        }
    }
}