/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "app_main.h"
#include "common_data.h"
#include "gui_img.h"
#include "tp_algo.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "lock_view"

#define TAB_START_POS_X 17 //21
#define TAB_START_POS_Y 64
#define TAB_STOP_POS_X  224 //228
#define TAB_SIZE        72

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void lock_view_design(gui_view_t *view);

/*============================================================================*
 *                            Variables
 *============================================================================*/
/* View Management */
static gui_view_t *current_view = NULL;
static const gui_view_descriptor_t *quick_view = NULL;
static const gui_view_descriptor_t *charging_view = NULL;
static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = lock_view_design,
};

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

static int gui_view_get_other_view_descriptor_init(void)
{
    /* you can get other view descriptor point here */
    quick_view = gui_view_descriptor_get("quick_view");
    charging_view = gui_view_descriptor_get("charging_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

static void pressing_tab(void *obj)
{
    GUI_UNUSED(obj);

    touch_info_t *tp = tp_get_info();
    gui_img_t *img = (gui_img_t *)obj;
    if (tp->pressing && tp->x >= TAB_START_POS_X && tp->x <= (TAB_START_POS_X + TAB_SIZE)
        && tp->y >= TAB_START_POS_Y && tp->y <= (TAB_START_POS_Y + TAB_SIZE))
    {
        int16_t x = tp->deltaX;
        if (x < 0)
        {
            x = 0;
        }
        else if (x > TAB_STOP_POS_X - TAB_START_POS_X)
        {
            x = TAB_STOP_POS_X - TAB_START_POS_X;
        }
        gui_img_translate(img, (float)x, 0);
    }
    else if (img->t_x != 0)
    {
        int16_t x = (int16_t)img->t_x;
        if (x == TAB_STOP_POS_X - TAB_START_POS_X)
        {
            gui_view_switch_direct(current_view, quick_view, SWITCH_OUT_NONE_ANIMATION,
                                   SWITCH_IN_ANIMATION_FADE);
            gui_obj_stop_timer(GUI_BASE(img));
            return;
        }
        const int16_t step = 40;
        x -= step;
        if (x <= 0)
        {
            x = 0;
            gui_img_set_image_data(img, DEFAULT_BIN);
            // gui_img_set_a8_fg_color(img, GUI_COLOR_ARGB8888(255, 0, 0xC8, 0xFF));
            // gui_obj_t *tab = gui_list_entry(GUI_BASE(obj)->child_list.next, gui_obj_t,
            //                                       brother_list);
            // gui_obj_hidden(tab, false);
        }
        gui_img_translate(img, (float)x, 0);
    }
    else
    {
        gui_img_set_image_data(img, DEFAULT_BIN);
    }
}

static void tab_scale(void *obj)
{
    static uint8_t cnt = 0;
    cnt++;
    uint8_t cnt_max = 5;
    gui_img_t *img = (gui_img_t *)obj;
    float scale = cnt / (float)cnt_max;
    gui_img_scale(img, scale, scale);
    if (cnt >= cnt_max)
    {
        cnt = 0;
        gui_obj_create_timer(GUI_BASE(img), 10, true, pressing_tab);
    }
}

static void press_tab(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);

    gui_img_t *img = (gui_img_t *)obj;
    gui_img_set_image_data(img, TAP_BIN);

    // gui_obj_t *tab = gui_list_entry(GUI_BASE(obj)->child_list.next, gui_obj_t,
    //                                               brother_list);
    // gui_img_set_a8_fg_color(img, FG_1_DARK.color.argb_full);
    // gui_obj_hidden(tab, true);

    gui_obj_create_timer(GUI_BASE(img), 10, true, tab_scale);
}

static void click_unlock(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);

    gui_view_switch_direct(current_view, charging_view, SWITCH_OUT_NONE_ANIMATION,
                           SWITCH_IN_NONE_ANIMATION);
}

static void lock_view_design(gui_view_t *view)
{
    gui_obj_t *parent = GUI_BASE(view);
    gui_set_bg_color(BG_1_LIGHT);
    gui_img_t *bg = gui_img_create_from_mem(parent, "bg", LOCK_SCROLLBAR_BG_BIN, 17, TAB_START_POS_Y, 0,
                                            0);
    gui_img_set_a8_fg_color(bg, GUI_COLOR_ARGB8888(255, 0, 0xC8, 0xFF));
    int16_t focus = 40;
    gui_img_t *tab_bg = gui_img_create_from_mem(parent, "tab_bg", DEFAULT_BIN, TAB_START_POS_X + focus,
                                                TAB_START_POS_Y + focus, 0, 0);

    // int16_t focus = 36;
    // gui_img_t *tab_bg = gui_img_create_from_mem(parent, "tab_bg", TAB_BG_BIN, TAB_START_POS_X,
    //                                          TAB_START_POS_Y, 0, 0);
    // gui_img_set_a8_fg_color(tab_bg, GUI_COLOR_ARGB8888(255, 0, 0xFF, 0xF6));
    // gui_img_t *tab = gui_img_create_from_mem(tab_bg, 0, ICON_TOUCH_BIN, 22, 22, 0, 0);
    // gui_img_set_a8_fg_color(tab, GUI_COLOR_ARGB8888(255, 0, 0x4C, 0x7A));

    gui_img_set_focus(tab_bg, focus, focus);
    gui_img_t *unlock = gui_img_create_from_mem(parent, 0, ICON_UNLOCK_BIN, 246, 82, 0, 0);
    gui_img_set_a8_fg_color(unlock, GUI_COLOR_ARGB8888(255, 0x05, 0x5B, 0x90));
    gui_obj_add_event_cb(tab_bg, press_tab, GUI_EVENT_TOUCH_PRESSED, NULL);
    // gui_obj_add_event_cb(unlock, click_unlock, GUI_EVENT_TOUCH_CLICKED, NULL);

    // gui_obj_create_timer(GUI_BASE(tab_bg), 10, true, pressing_tab);
}