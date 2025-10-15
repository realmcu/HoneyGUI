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

#define BG_AUDIO_X              20
#define BG_TOOLS_X              120
#define BG_SETTINGS_X           220
#define BG_AUDIO_SCALE_X        10
#define BG_TOOLS_SCALE_X        110
#define BG_SETTINGS_SCALE_X     210
#define BG_Y                    66
#define BG_SCALE_Y              56

#define ICON_POS                18
#define ICON_SCALE_POS          28



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
    gui_img_t *icon = (gui_img_t *)gui_list_entry(GUI_BASE(obj)->child_list.next, gui_obj_t,
                                                  brother_list);
    // gui_img_scale(img, 1.2f, 1.2f);
    gui_img_set_image_data(img, MENU_BUTTON_BG_SCALE_BIN);
    gui_img_refresh_size(img);
    gui_obj_move((void *)icon, ICON_SCALE_POS, ICON_SCALE_POS);

    if (strcmp(o->name, "audio") == 0)
    {
        gui_img_set_image_data(icon, MENU_AUDIO_SCALE_BIN);
        gui_img_refresh_size(icon);
        gui_obj_move((void *)img, BG_AUDIO_SCALE_X, BG_SCALE_Y);
        gui_view_switch_direct(current_view, audio_menu_view, SWITCH_OUT_ANIMATION_MOVE_TO_LEFT,
                               SWITCH_IN_ANIMATION_MOVE_FROM_RIGHT);
    }
    else if (strcmp(o->name, "tools") == 0)
    {
        gui_img_set_image_data(icon, MENU_TOOLS_SCALE_BIN);
        gui_img_refresh_size(icon);
        gui_obj_move((void *)img, BG_TOOLS_SCALE_X, BG_SCALE_Y);
        gui_view_switch_direct(current_view, tools_menu_view, SWITCH_OUT_ANIMATION_MOVE_TO_LEFT,
                               SWITCH_IN_ANIMATION_MOVE_FROM_RIGHT);
    }
    else if (strcmp(o->name, "settings") == 0)
    {
        gui_img_set_image_data(icon, MENU_SETTINGS_SCALE_BIN);
        gui_img_refresh_size(icon);
        gui_obj_move((void *)img, BG_SETTINGS_SCALE_X, BG_SCALE_Y);
        gui_view_switch_direct(current_view, case_menu_view, SWITCH_OUT_ANIMATION_MOVE_TO_LEFT,
                               SWITCH_IN_ANIMATION_MOVE_FROM_RIGHT);
    }
}

static void show_bg(void *obj)
{
    static bool flag = false;
    if (gui_view_get_next())
    {
        gui_obj_hidden((void *)obj, false);
        flag = false;
    }
    else
    {
        if (flag)
        {
            flag = false;
            gui_obj_hidden((void *)obj, true);
        }
        else
        {
            flag = true;
        }
    }
}

static void menu_view_design(gui_view_t *view)
{
    gui_view_set_animate_step(view, 20);
    gui_view_switch_on_event(view, quick_view, SWITCH_OUT_TO_BOTTOM_USE_TRANSLATION,
                             SWITCH_INIT_STATE,
                             GUI_EVENT_TOUCH_MOVE_DOWN);

    gui_obj_t *parent = GUI_BASE(view);
    gui_color_t font_color;
    gui_img_t *bg = gui_img_create_from_mem(parent, 0, SCREEN_BG_BIN, 0, 0, 0, 0);
    gui_obj_create_timer((void *)bg, 10, true, show_bg);
    if (theme_bg_white)
    {
        gui_set_bg_color(SCREEN_BG_LIGHT);
        gui_img_a8_recolor(bg, SCREEN_BG_LIGHT.color.argb_full);
        font_color = FG_1_LIGHT;
    }
    else
    {
        gui_img_a8_recolor(bg, GUI_COLOR_ARGB8888(255, 0, 0, 0));
        font_color = FG_1_DARK;
    }
    gui_text_t *title = gui_text_create(parent, 0, 0, 0, gui_get_screen_width(), 40);
    gui_text_set(title, "Menu", GUI_FONT_SRC_BMP, font_color, 4, 20);
    gui_text_type_set(title, HEADING_1_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(title, MID_CENTER);

    int16_t focus = 0; //40
    gui_img_t *audio_bg = gui_img_create_from_mem(parent, "audio", MENU_BUTTON_BG_BIN,
                                                  BG_AUDIO_X + focus,
                                                  BG_Y + focus, 0, 0);
    gui_img_t *audio = gui_img_create_from_mem(audio_bg, 0, MENU_AUDIO_BIN, ICON_POS, ICON_POS, 0, 0);
    gui_img_t *tools_bg = gui_img_create_from_mem(parent, "tools", MENU_BUTTON_BG_BIN,
                                                  BG_TOOLS_X + focus,
                                                  BG_Y + focus, 0, 0);
    gui_img_t *tools = gui_img_create_from_mem(tools_bg, 0, MENU_TOOLS_BIN, ICON_POS, ICON_POS, 0, 0);
    gui_img_t *settings_bg = gui_img_create_from_mem(parent, "settings", MENU_BUTTON_BG_BIN,
                                                     BG_SETTINGS_X + focus, BG_Y + focus, 0, 0);
    gui_img_t *settings = gui_img_create_from_mem(settings_bg, 0, MENU_SETTINGS_BIN, ICON_POS, ICON_POS,
                                                  0, 0);
    gui_img_set_focus(audio_bg, focus, focus);
    gui_img_set_focus(tools_bg, focus, focus);
    gui_img_set_focus(settings_bg, focus, focus);
    gui_obj_add_event_cb(audio_bg, click_button, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_obj_add_event_cb(tools_bg, click_button, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_obj_add_event_cb(settings_bg, click_button, GUI_EVENT_TOUCH_CLICKED, NULL);
    if (theme_bg_white)
    {
        gui_img_a8_recolor(audio_bg, BG_THEME1_MID_LIGHT.color.argb_full);
        gui_img_a8_recolor(tools_bg, BG_THEME2_MID_LIGHT.color.argb_full);
        gui_img_a8_recolor(settings_bg, BG_THEME3_MID_LIGHT.color.argb_full);
        gui_img_a8_recolor(audio, FG_THEME1_LIGHT.color.argb_full);
        gui_img_a8_recolor(tools, FG_THEME2_LIGHT.color.argb_full);
        gui_img_a8_recolor(settings, FG_THEME3_LIGHT.color.argb_full);
    }
    else
    {
        gui_img_a8_recolor(audio_bg, BG_THEME1_MID_DARK.color.argb_full);
        gui_img_a8_recolor(tools_bg, BG_THEME2_MID_DARK.color.argb_full);
        gui_img_a8_recolor(settings_bg, BG_THEME3_MID_DARK.color.argb_full);
        gui_img_a8_recolor(audio, FG_THEME1_DARK.color.argb_full);
        gui_img_a8_recolor(tools, FG_THEME2_DARK.color.argb_full);
        gui_img_a8_recolor(settings, FG_THEME3_DARK.color.argb_full);
    }
}