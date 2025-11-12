/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "app_main.h"
#include "common_data.h"
#include "gui_img.h"
#include "gui_win.h"
#include "gui_text.h"
#include "gui_scroll_text.h"
#include "gui_list.h"

/*============================================================================*
 *                           Types
 *============================================================================*/
typedef struct note_design_param
{
    void **page_design;
    uint32_t **img_data_array;
} note_design_param_t;

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "settings_menu_view"

#define TITLE "Case Settings"

#define LIST_Y  60

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void settings_menu_view_design(gui_view_t *view);
static void clear(gui_view_t *view);

/*============================================================================*
 *                            Variables
 *============================================================================*/
/* View Management */
static gui_view_t *current_view = NULL;
static const gui_view_descriptor_t *menu_view = NULL;
static const gui_view_descriptor_t *detail_view = NULL;
static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = settings_menu_view_design,
    .on_switch_out = clear,
};

static note_design_param_t *design_p = NULL;
static int16_t list_offset_his = 0;

static gui_img_t *bg_note = NULL;

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
    menu_view = gui_view_descriptor_get("menu_view");
    detail_view = gui_view_descriptor_get("detail_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);


static void click_button_back(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    gui_view_switch_direct(current_view, menu_view, SWITCH_OUT_ANIMATION_MOVE_TO_RIGHT,
                           SWITCH_IN_ANIMATION_MOVE_FROM_LEFT);
}

static void note_timer_cb(void *p)
{
    gui_obj_stop_timer(p);
    gui_view_switch_direct(current_view, detail_view, SWITCH_OUT_ANIMATION_MOVE_TO_LEFT,
                           SWITCH_IN_ANIMATION_MOVE_FROM_RIGHT);
}

static void switch_page(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    if (GUI_BASE(obj)->y < -28) { return; }
    detail_page_design_func = param;
    gui_obj_move(GUI_BASE(bg_note), 0, GUI_BASE(obj)->y + LIST_Y);
    gui_obj_hidden((void *)bg_note, false);
    gui_obj_create_timer(GUI_BASE(obj), 800, true, note_timer_cb);
}

static void note_design(gui_obj_t *obj, void *p)
{
    note_design_param_t *design_p = (note_design_param_t *)p;
    gui_list_note_t *note = (gui_list_note_t *)obj;
    uint16_t index = note->index;

    gui_img_t *img = gui_img_create_from_mem(note, 0, design_p->img_data_array[index], 8, 8, 0, 0);
    gui_color_t font_color;
    if (theme_bg_white)
    {
        gui_img_a8_recolor(img, FG_THEME3_LIGHT.color.argb_full);
        font_color = FG_1_LIGHT;
    }
    else
    {
        gui_img_a8_recolor(img, FG_THEME3_DARK.color.argb_full);
        font_color = FG_1_DARK;
    }
    char *text = (char *)page_name_array[index + 16];
    uint16_t text_length = strlen(text);
    int font_size = 30;
    if (text_length < 23)
    {
        gui_text_t *t = gui_text_create(note, 0, 56, 15, 244, 56);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, font_color, text_length, font_size);
        gui_text_type_set(t, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, LEFT);
    }
    else
    {
        gui_scroll_text_t *t = gui_scroll_text_create(note, 0, 56, 15, 244, 56);
        gui_scroll_text_set(t, text, GUI_FONT_SRC_BMP, font_color, text_length, font_size);
        gui_scroll_text_type_set(t, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
        gui_scroll_text_scroll_set(t, SCROLL_X, 260, 260, 5000, 0);
    }
    if (design_p->page_design[index] != NULL)
    {
        gui_obj_add_event_cb(note, switch_page, GUI_EVENT_TOUCH_CLICKED,
                             design_p->page_design[index]);
    }
}

static void list_timer_cb(void *obj)
{
    gui_list_t *list = (gui_list_t *)obj;
    list_offset_his = list->offset;
    int range = list->base.h - list->total_length;
    int t_y = list_offset_his * (SCROLL_BAR_BG_H - SCROLL_BAR_S_H) / range;
    gui_obj_t *scrollbar = gui_list_entry(GUI_BASE(obj)->parent->child_list.prev, gui_obj_t,
                                          brother_list);
    gui_img_translate((void *)scrollbar, 0, t_y);
}

static void settings_menu_view_design(gui_view_t *view)
{
    if (gui_view_get_current() && gui_view_get_current()->descriptor == menu_view)
    {
        list_offset_his = 0;
    }
    gui_view_set_animate_step(view, 10);

    gui_obj_t *parent = GUI_BASE(view);
    gui_color_t font_color;
    bg_note = gui_img_create_from_mem(parent, 0, MENU_LISTNOTE_BG_BIN, 0, 0, 0, 0);
    gui_obj_hidden((void *)bg_note, true);

    uint32_t *img_data_array[] =
    {
        ICON_TX_MANAGER_BIN,
        ICON_BRIGHTNESS_BIN,
        ICON_LIGHTDARKMODE_BIN,
        ICON_ICON_WALLPAPER_BIN,
        ICON_AUTODIMOFFSCREEN_BIN,
        ICON_QUICKWAKEUP_BIN,
        ICON_CASEBUTTON_BIN,
        ICON_CUSTOMIZE_INFOCENTER_BIN,
        ICON_AUTOLOCKSCREEN_BIN,
        ICON_NOTIFICATION_BIN,
        ICON_CLOCK_SETTINGS_BIN,
        ICON_24HOUR_TIME_BIN,
        ICON_LANGUAGE_BIN,
        ICON_REORDER_BIN,
        ICON_SUPPORT_BIN,
    };

    int array_size = sizeof(img_data_array) / sizeof(img_data_array[0]);
    void *page_design[] =
    {
        page_tx_management_design,
        page_screen_brightness_design,
        page_dark_light_design,
        page_lock_screen_design,
        page_auto_dim_off_screen_design,
        page_quick_wake_up_screen_design,
        page_case_button_customize_design,
        page_information_center_customize_design,
        page_unlock_slider_design,
        page_notification_design,
        page_clock_settings_design,
        page_time_format_design,
        page_language_design,
        page_reorder_quick_access_design,
        page_support_design
    };
    design_p = gui_malloc(sizeof(note_design_param_t));
    void **func_cb = gui_malloc(array_size * sizeof(void *));
    memset(func_cb, 0, array_size * sizeof(void *));
    memcpy(func_cb, page_design, sizeof(page_design));
    uint32_t **data_array = gui_malloc(array_size * sizeof(uint32_t *));
    memcpy(data_array, img_data_array, array_size * sizeof(void *));
    design_p->page_design = func_cb;
    design_p->img_data_array = data_array;
    gui_list_t *list = gui_list_create(view, 0, 0, LIST_Y, 0, 0, 56, 0,
                                       VERTICAL, note_design, design_p, 0);
    gui_list_set_style(list, LIST_CLASSIC);
    gui_list_set_note_num(list, array_size);
    gui_list_set_offset(list, list_offset_his);
    gui_list_set_out_scope(list, 20);
    gui_obj_create_timer(GUI_BASE(list), 10, true, list_timer_cb);
    gui_img_t *mask = gui_img_create_from_mem(parent, 0, MASK_BIN, 0, 0, 0, 0);
    if (theme_bg_white)
    {
        gui_set_bg_color(SCREEN_BG_LIGHT);
        gui_img_a8_recolor(mask, SCREEN_BG_LIGHT.color.argb_full);
        gui_img_a8_recolor(bg_note, BG_1_LIGHT.color.argb_full);
        font_color = FG_1_LIGHT;
    }
    else
    {
        gui_set_bg_color(SCREEN_BG_DARK);
        gui_img_a8_recolor(mask, SCREEN_BG_DARK.color.argb_full);
        gui_img_a8_recolor(bg_note, BG_1_DARK.color.argb_full);
        font_color = FG_1_DARK;
    }
    gui_img_a8_mix_alpha(bg_note, bg_note->fg_color_set >> 24);

    gui_text_t *title = gui_text_create(parent, 0, 0, 0, 320, 60);
    gui_text_set(title, TITLE, GUI_FONT_SRC_BMP, font_color, strlen(TITLE), 30);
    gui_text_type_set(title, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(title, MID_CENTER);

    gui_win_t *win_icon_back = (gui_win_t *)gui_win_create(parent, 0, 0, 0, 60, 60);
    gui_img_t *icon_back = gui_img_create_from_mem(win_icon_back, 0, ICON_BACK_BIN, 0, 8, 0, 0);
    gui_obj_add_event_cb(win_icon_back, click_button_back, GUI_EVENT_TOUCH_CLICKED, NULL);

    gui_img_t *scrollbar_bg = gui_img_create_from_mem(parent, 0, SCROLLBAR_BG_BIN, 310, 62, 0, 0);
    gui_img_t *scrollbar = gui_img_create_from_mem(parent, 0, SCROLLBAR_S_BIN, 310, 62, 0, 0);


    if (theme_bg_white)
    {
        gui_img_a8_recolor(icon_back, FG_1_LIGHT.color.argb_full);
        gui_img_a8_recolor(scrollbar_bg, FG_BLACK.color.argb_full);
        gui_img_a8_recolor(scrollbar, FG_BLACK.color.argb_full);
    }
    else
    {
        gui_img_a8_recolor(icon_back, FG_1_DARK.color.argb_full);
        gui_img_a8_recolor(scrollbar_bg, FG_WHITE.color.argb_full);
        gui_img_a8_recolor(scrollbar, FG_WHITE.color.argb_full);
    }
}

static void clear(gui_view_t *view)
{
    GUI_UNUSED(view);
    if (design_p)
    {
        gui_free(design_p->page_design);
        gui_free(design_p->img_data_array);
        gui_free(design_p);
        design_p = NULL;
    }
}