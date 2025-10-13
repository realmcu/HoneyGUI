/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "app_main.h"
#include "common_data.h"
#include "gui_img.h"
#include "gui_canvas_rect.h"
#include "gui_win.h"
#include "gui_text.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "menu_view"

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void menu_view_design(gui_view_t *view);

/*============================================================================*
 *                            Variables
 *============================================================================*/
/* View Management */
static gui_view_t *current_view = NULL;
static const gui_view_descriptor_t *quick_view = NULL;
static const gui_view_descriptor_t *audio_menu_view = NULL;
static const gui_view_descriptor_t *case_menu_view = NULL;
static const gui_view_descriptor_t *tools_menu_view = NULL;
static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = menu_view_design,
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
    audio_menu_view = gui_view_descriptor_get("audio_menu_view");
    case_menu_view = gui_view_descriptor_get("case_menu_view");
    tools_menu_view = gui_view_descriptor_get("tools_menu_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);


static void click_button(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);

    gui_img_t *img = (gui_img_t *)obj;
    gui_obj_t *o = GUI_BASE(obj);
    gui_img_scale(img, 1.25f, 1.25f);

    if (strcmp(o->name, "audio") == 0)
    {
        gui_view_switch_direct(current_view, audio_menu_view, SWITCH_OUT_ANIMATION_MOVE_TO_LEFT,
                               SWITCH_IN_ANIMATION_MOVE_FROM_RIGHT);
    }
    else if (strcmp(o->name, "tools") == 0)
    {
        gui_view_switch_direct(current_view, tools_menu_view, SWITCH_OUT_ANIMATION_MOVE_TO_LEFT,
                               SWITCH_IN_ANIMATION_MOVE_FROM_RIGHT);
    }
    else if (strcmp(o->name, "settings") == 0)
    {
        gui_view_switch_direct(current_view, case_menu_view, SWITCH_OUT_ANIMATION_MOVE_TO_LEFT,
                               SWITCH_IN_ANIMATION_MOVE_FROM_RIGHT);
    }
}


static void menu_view_design(gui_view_t *view)
{
    gui_view_set_animate_step(view, 20);
    gui_view_switch_on_event(view, quick_view, SWITCH_OUT_TO_BOTTOM_USE_TRANSLATION,
                             SWITCH_IN_FROM_TOP_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_DOWN);

    gui_obj_t *parent = GUI_BASE(view);
    gui_color_t font_color;
    if (theme_bg_white)
    {
        gui_obj_hidden((void *)screen_bg, false);
        font_color = FG_1_LIGHT;
    }
    else
    {
        font_color = FG_1_DARK;
    }
    gui_text_t *title = gui_text_create(parent, 0, 0, 0, gui_get_screen_width(), 40);
    gui_text_set(title, "Menu", GUI_FONT_SRC_BMP, font_color, 4, 20);
    gui_text_type_set(title, HEADING_1_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(title, MID_CENTER);


    int16_t focus = 40;
    gui_img_t *audio_bg = gui_img_create_from_mem(parent, "audio", MENU_BUTTON_BG_BIN, 20 + focus,
                                                  66 + focus, 0, 0);
    gui_img_t *audio = gui_img_create_from_mem(audio_bg, 0, MENU_AUDIO_BIN, 18, 18, 0, 0);
    gui_img_t *tools_bg = gui_img_create_from_mem(parent, "tools", MENU_BUTTON_BG_BIN, 120 + focus,
                                                  66 + focus, 0, 0);
    gui_img_t *tools = gui_img_create_from_mem(tools_bg, 0, MENU_TOOLS_BIN, 18, 18, 0, 0);
    gui_img_t *settings_bg = gui_img_create_from_mem(parent, "settings", MENU_BUTTON_BG_BIN,
                                                     220 + focus, 66 + focus, 0, 0);
    gui_img_t *settings = gui_img_create_from_mem(settings_bg, 0, MENU_SETTINGS_BIN, 18, 18, 0, 0);
    gui_img_set_focus(audio_bg, focus, focus);
    gui_img_set_focus(tools_bg, focus, focus);
    gui_img_set_focus(settings_bg, focus, focus);
    gui_obj_add_event_cb(audio_bg, click_button, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_obj_add_event_cb(tools_bg, click_button, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_obj_add_event_cb(settings_bg, click_button, GUI_EVENT_TOUCH_CLICKED, NULL);
    if (theme_bg_white)
    {
        gui_img_set_a8_fg_color(audio_bg, BG_THEME1_MID_LIGHT.color.argb_full);
        gui_img_set_a8_fg_color(tools_bg, BG_THEME2_MID_LIGHT.color.argb_full);
        gui_img_set_a8_fg_color(settings_bg, BG_THEME3_MID_LIGHT.color.argb_full);
        gui_img_set_a8_fg_color(audio, FG_THEME1_LIGHT.color.argb_full);
        gui_img_set_a8_fg_color(tools, FG_THEME2_LIGHT.color.argb_full);
        gui_img_set_a8_fg_color(settings, FG_THEME3_LIGHT.color.argb_full);
    }
    else
    {
        gui_img_set_a8_fg_color(audio_bg, BG_THEME1_MID_DARK.color.argb_full);
        gui_img_set_a8_fg_color(tools_bg, BG_THEME2_MID_DARK.color.argb_full);
        gui_img_set_a8_fg_color(settings_bg, BG_THEME3_MID_DARK.color.argb_full);
        gui_img_set_a8_fg_color(audio, FG_THEME1_DARK.color.argb_full);
        gui_img_set_a8_fg_color(tools, FG_THEME2_DARK.color.argb_full);
        gui_img_set_a8_fg_color(settings, FG_THEME3_DARK.color.argb_full);
    }

    // gui_obj_create_timer(GUI_BASE(view), 3000, true, time_update);
}