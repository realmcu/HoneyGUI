/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "app_main.h"
#include "common_data.h"
#include "gui_img.h"
#include "gui_win.h"

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

static const gui_view_descriptor_t *descriptor_rec = NULL;

static bool is_favorite = false;
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

static bool page_in_favorite(void)
{
    for (uint8_t i = 0; i < quick_page_num; i++)
    {
        if (detail_page_design_func == quick_page_design_func_array[i])
        {
            return true;
        }
    }
    return false;
}

static void page_add_in_favorite(void)
{
    if (quick_page_num < QUICK_PAGE_NUM_MAX)
    {
        quick_page_design_func_array[quick_page_num++] = detail_page_design_func;
    }
    else
    {
        for (uint8_t i = 0; i < QUICK_PAGE_NUM_MAX - 1; i++)
        {
            quick_page_design_func_array[i] = quick_page_design_func_array[i + 1];
        }
        quick_page_design_func_array[QUICK_PAGE_NUM_MAX - 1] = detail_page_design_func;
    }
}

static void page_delete_from_favorite(void)
{
    int8_t index = 0;
    while (1)
    {
        if (quick_page_design_func_array[index] == detail_page_design_func)
        {
            break;
        }
        index++;
    }
    while (index < quick_page_num - 1)
    {
        quick_page_design_func_array[index] = quick_page_design_func_array[index + 1];
        index++;
    }
    quick_page_design_func_array[--quick_page_num] = NULL;
}

static void click_button_back(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    f_status.silentnow = 0;
    gui_view_switch_direct(current_view, descriptor_rec, SWITCH_OUT_ANIMATION_MOVE_TO_RIGHT,
                           SWITCH_IN_ANIMATION_MOVE_FROM_LEFT);
}

static void click_button_favorite(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);

    gui_img_t *icon = (gui_img_t *)gui_list_entry(GUI_BASE(obj)->child_list.prev, gui_obj_t,
                                                  brother_list);
    is_favorite = !is_favorite;
    if (is_favorite)
    {
        gui_img_set_image_data(icon, ICON_IS_FAVORITE_BIN);
        page_add_in_favorite();
        gui_img_a8_recolor(icon, FG_WARNING.color.argb_full);
    }
    else
    {
        gui_img_set_image_data(icon, ICON_NOT_FAVORITE_BIN);
        page_delete_from_favorite();
        if (theme_bg_white)
        {
            gui_img_a8_recolor(icon, FG_1_LIGHT.color.argb_full);
        }
        else
        {
            gui_img_a8_recolor(icon, FG_1_DARK.color.argb_full);
        }
    }
}

static void detail_view_design(gui_view_t *view)
{
    gui_view_set_animate_step(view, 10);
    gui_view_t *view_c = gui_view_get_current();
    if (view_c && !(!strcmp(view_c->descriptor->name, "timer_view") ||
                    !strcmp(view_c->descriptor->name, "flashlight_view")        ||
                    !strcmp(view_c->descriptor->name, "button_customize_view")  ||
                    !strcmp(view_c->descriptor->name, "support_view") ||
                    !strcmp(view_c->descriptor->name, "auracast_view") ||
                    !strcmp(view_c->descriptor->name, "ss_view")))
    {
        descriptor_rec = view_c->descriptor;
    }
    gui_obj_t *parent = GUI_BASE(view);

    gui_win_t *win_icon_back = (gui_win_t *)gui_win_create(parent, 0, 0, 0, 60, 60);
    gui_img_t *icon_back = gui_img_create_from_mem(win_icon_back, 0, ICON_BACK_BIN, 0, 0, 0, 0);
    gui_obj_add_event_cb(win_icon_back, click_button_back, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_win_t *win_icon_favorite = NULL;
    gui_img_t *icon_favorite = NULL;
    is_favorite = false;
    if (detail_page_design_func == page_case_button_customize_design ||
        detail_page_design_func == page_reorder_quick_access_design  ||
        detail_page_design_func == page_information_center_customize_design ||
        detail_page_design_func == page_support_design ||
        detail_page_design_func == page_audio_source_design ||
        detail_page_design_func == page_tx_management_design)
    {
        icon_back->base.y = 8;
    }
    else
    {
        win_icon_favorite = (gui_win_t *)gui_win_create(parent, 0, 130, 0, 60, 60);
        icon_favorite = gui_img_create_from_mem(win_icon_favorite, 0, ICON_NOT_FAVORITE_BIN, 16,
                                                6, 0, 0);
        if (page_in_favorite())
        {
            gui_img_set_image_data(icon_favorite, ICON_IS_FAVORITE_BIN);
            is_favorite = true;
            gui_img_a8_recolor(icon_favorite, FG_WARNING.color.argb_full);
        }
        gui_obj_add_event_cb(win_icon_favorite, click_button_favorite, GUI_EVENT_TOUCH_CLICKED, NULL);
    }

    if (theme_bg_white)
    {
        gui_set_bg_color(SCREEN_BG_LIGHT);
        gui_img_a8_recolor(icon_back, FG_1_LIGHT.color.argb_full);
        if (icon_favorite && !is_favorite)
        {
            gui_img_a8_recolor(icon_favorite, FG_1_LIGHT.color.argb_full);
        }
    }
    else
    {
        gui_set_bg_color(SCREEN_BG_DARK);
        gui_img_a8_recolor(icon_back, FG_1_DARK.color.argb_full);
        if (icon_favorite && !is_favorite)
        {
            gui_img_a8_recolor(icon_favorite, FG_1_DARK.color.argb_full);
        }
    }
    detail_page_design_func(parent);
}