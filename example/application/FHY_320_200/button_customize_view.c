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
#define CURRENT_VIEW_NAME "button_customize_view"

#define LIST_Y  60

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void button_customize_view_design(gui_view_t *view);

/*============================================================================*
 *                            Variables
 *============================================================================*/
/* View Management */
static gui_view_t *current_view = NULL;
static const gui_view_descriptor_t *detail_view = NULL;
static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = button_customize_view_design,
};

static const char *text_array[] =
{
    "Wake up/dim off screen",
    "Go to quick access",
    "Shut down/enable screen",
    "Flashlight",
    "Timer",
};

static int16_t list_offset_his = 0;

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
    gui_view_switch_direct(current_view, detail_view->name, SWITCH_OUT_ANIMATION_MOVE_TO_RIGHT,
                           SWITCH_IN_ANIMATION_MOVE_FROM_LEFT);
}

static void click_note(void *obj, gui_event_t e, void *param)
{
    (void)obj;
    (void)e;
    (void)param;
    int16_t index = *((int16_t *)param);
    if (case_button_customize_type_index != 2 && index == 2) { return; }
    case_button_customize_index_array[case_button_customize_type_index] = index;
    msg_2_regenerate_current_view();
}

static void note_design(gui_obj_t *obj, void *p)
{
    GUI_UNUSED(p);
    gui_list_note_t *note = (gui_list_note_t *)obj;
    uint16_t index = note->index;

    gui_img_t *bg = gui_img_create_from_mem(note, 0, SELECTOR_BG_BIN, 16, 16, 0, 0);
    gui_color_t font_color;
    if (theme_bg_white)
    {
        font_color = FG_1_LIGHT;
        gui_img_a8_recolor(bg, font_color.color.argb_full);
        if (case_button_customize_index_array[case_button_customize_type_index] == index)
        {
            gui_img_t *circle = gui_img_create_from_mem(note, 0, SELECTOR_CIRCLE_BIN, 23, 23, 0, 0);
            gui_img_a8_recolor(circle, FG_THEME3_LIGHT.color.argb_full);
        }
        else
        {
            gui_img_a8_mix_alpha(bg, 122);
        }
    }
    else
    {
        font_color = FG_1_DARK;
        gui_img_a8_recolor(bg, font_color.color.argb_full);
        if (case_button_customize_index_array[case_button_customize_type_index] == index)
        {
            gui_img_t *circle = gui_img_create_from_mem(note, 0, SELECTOR_CIRCLE_BIN, 23, 23, 0, 0);
            gui_img_a8_recolor(circle, FG_THEME3_DARK.color.argb_full);
        }
        else
        {
            gui_img_a8_mix_alpha(bg, 122);
        }
    }

    char *text = (char *)text_array[index];
    uint16_t text_length = strlen(text);
    int font_size = 30;
    if (text_length < 23)
    {
        gui_text_t *t = gui_text_create(note, 0, 56, 15, 256, 56);
        gui_text_set(t, text, GUI_FONT_SRC_BMP, font_color, text_length, font_size);
        gui_text_type_set(t, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, LEFT);
    }
    else
    {
        gui_scroll_text_t *t = gui_scroll_text_create(note, 0, 56, 15, 256, 56);
        gui_scroll_text_set(t, text, GUI_FONT_SRC_BMP, font_color, text_length, font_size);
        gui_scroll_text_type_set(t, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
        gui_scroll_text_scroll_set(t, SCROLL_X, 260, 260, 5000, 0);
    }
    gui_obj_add_event_cb(note, click_note, GUI_EVENT_TOUCH_CLICKED, &(note->index));
}

static void list_timer_cb(void *obj)
{
    gui_list_t *list = (gui_list_t *)obj;
    list_offset_his = list->offset;
    int range = list->base.h - list->total_length;
    int t_y = list->offset * (SCROLL_BAR_BG_H - SCROLL_BAR_M_H) / range;
    gui_obj_t *scrollbar = gui_list_entry(GUI_BASE(obj)->parent->child_list.prev, gui_obj_t,
                                          brother_list);
    gui_img_translate((void *)scrollbar, 0, t_y);
}

static void button_customize_view_design(gui_view_t *view)
{
    gui_view_set_animate_step(view, 10);

    gui_obj_t *parent = GUI_BASE(view);
    gui_color_t font_color;

    gui_list_t *list = gui_list_create(view, 0, 0, LIST_Y, 0, 0, 56, 0,
                                       VERTICAL, note_design, NULL, 0);
    gui_list_set_style(list, LIST_CLASSIC);
    gui_list_set_note_num(list, 5);
    gui_list_set_offset(list, list_offset_his);
    gui_list_set_out_scope(list, 20);
    gui_obj_create_timer(GUI_BASE(list), 10, true, list_timer_cb);
    gui_img_t *mask = gui_img_create_from_mem(parent, 0, MASK_BIN, 0, 0, 0, 0);
    if (theme_bg_white)
    {
        gui_set_bg_color(SCREEN_BG_LIGHT);
        gui_img_a8_recolor(mask, SCREEN_BG_LIGHT.color.argb_full);
        font_color = FG_1_LIGHT;
    }
    else
    {
        gui_set_bg_color(SCREEN_BG_DARK);
        gui_img_a8_recolor(mask, SCREEN_BG_DARK.color.argb_full);
        font_color = FG_1_DARK;
    }

    gui_text_t *title = gui_text_create(parent, 0, 0, 0, 320, 60);
    gui_text_set(title, (void *)case_button_customize_type[case_button_customize_type_index],
                 GUI_FONT_SRC_BMP, font_color, strlen(case_button_customize_type[case_button_customize_type_index]),
                 30);
    gui_text_type_set(title, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(title, MID_CENTER);


    gui_img_t *icon_back = gui_img_create_from_mem(parent, 0, ICON_BACK_BIN, 0, 8, 0, 0);
    gui_obj_add_event_cb(icon_back, click_button_back, GUI_EVENT_TOUCH_CLICKED, NULL);

    gui_img_t *scrollbar_bg = gui_img_create_from_mem(parent, 0, SCROLLBAR_BG_BIN, 310, 62, 0, 0);
    gui_img_t *scrollbar = gui_img_create_from_mem(parent, 0, SCROLLBAR_M_BIN, 310, 62, 0, 0);


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