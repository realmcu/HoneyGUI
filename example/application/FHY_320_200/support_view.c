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
#define CURRENT_VIEW_NAME "support_view"

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void support_view_design(gui_view_t *view);
static void update_page_indicator(void);
static void resetting_design(gui_obj_t *parent);
/*============================================================================*
 *                            Variables
 *============================================================================*/
/* View Management */
static gui_view_t *current_view = NULL;
static const gui_view_descriptor_t *quick_view = NULL;
static const gui_view_descriptor_t *detail_view = NULL;
static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = support_view_design,
};

static int8_t page_index = 0;
static gui_img_t **page_indicator_array = NULL;

static char reset_warning_str[] =
    "You're about to reset your\nearbuds and smart charging\ncase to default settings.";
static char resetting_warning_str[] = "Resetting your\nheadphones...";
static char *tips_str_array[5] =
{
    "More in JBL\nHeadphones app.",
    "Horizontal swipe\nto browse Quick\nAccess.",
    "Swipe up to enter\nmenu.",
    "Swipe down to\ncheck more\ninformation.",
    "Add more feature\nto Quick Access."
};

bool support_reset = false;
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
    gui_view_switch_direct(current_view, detail_view, SWITCH_OUT_ANIMATION_MOVE_TO_RIGHT,
                           SWITCH_IN_ANIMATION_MOVE_FROM_LEFT);
}

static void click_button_reset(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    gui_obj_child_free((void *)current_view);
    resetting_design((void *)current_view);
}

static void update_page_indicator(void)
{
    gui_color_t indicator_color;
    gui_color_t indicator_color_cur;
    if (theme_bg_white)
    {
        indicator_color = FG_2_LIGHT;
        indicator_color_cur = FG_1_LIGHT;
    }
    else
    {
        indicator_color = FG_2_DARK;
        indicator_color_cur = FG_1_DARK;
    }

    for (int i = 0; i < 6; i++)
    {
        gui_img_a8_recolor((gui_img_t *)page_indicator_array[i], indicator_color.color.argb_full);
        gui_img_a8_mix_alpha((gui_img_t *)page_indicator_array[i], indicator_color.color.rgba.a);
    }
    gui_img_a8_recolor(page_indicator_array[page_index], indicator_color_cur.color.argb_full);
    gui_img_a8_mix_alpha(page_indicator_array[page_index], indicator_color_cur.color.rgba.a);
}

static void tips_note_design(gui_obj_t *obj, void *p)
{
    GUI_UNUSED(p);
    gui_list_note_t *note = (gui_list_note_t *)obj;
    int16_t index = note->index;
    gui_color_t font_color;
    void *img_data;
    switch (index)
    {
    case 0:
        {
            if (theme_bg_white)
            {
                font_color = FG_1_LIGHT;
            }
            else
            {
                font_color = FG_1_DARK;
            }
            gui_img_t *img = gui_img_create_from_mem(obj, 0, QRCODE_BIN, 16, 14, 0, 0);
            gui_img_set_mode(img, IMG_2D_SW_SRC_OVER_MODE);
            gui_text_t *t = gui_text_create(img, 0, 140, 33, 164, 60);
            gui_text_set(t, tips_str_array[index], GUI_FONT_SRC_BMP, font_color, strlen(tips_str_array[index]),
                         20);
            gui_text_type_set(t, HEADING_1_BIN, FONT_SRC_MEMADDR);
            gui_text_mode_set(t, MULTI_CENTER);
        }
        break;
    case 1:
        {
            if (theme_bg_white)
            {
                font_color = FG_1_LIGHT;
                img_data = TIP_2_FG_LIGHT_BIN;
            }
            else
            {
                font_color = FG_1_DARK;
                img_data = TIP_2_FG_DARK_BIN;
            }
            gui_img_t *img = gui_img_create_from_mem(obj, 0, img_data, 16, 30, 0, 0);
            gui_img_set_mode(img, IMG_BYPASS_MODE);
            gui_text_t *t = gui_text_create(img, 0, 156, 10, 148, 60);
            gui_text_set(t, tips_str_array[index], GUI_FONT_SRC_BMP, font_color, strlen(tips_str_array[index]),
                         20);
            gui_text_type_set(t, HEADING_1_BIN, FONT_SRC_MEMADDR);
            gui_text_mode_set(t, MULTI_CENTER);
        }
        break;
    case 2:
        {
            if (theme_bg_white)
            {
                font_color = FG_1_LIGHT;
                img_data = TIP_3_FG_LIGHT_BIN;
            }
            else
            {
                font_color = FG_1_DARK;
                img_data = TIP_3_FG_DARK_BIN;
            }
            gui_img_t *img = gui_img_create_from_mem(obj, 0, img_data, 16, 10, 0, 0);
            gui_img_set_mode(img, IMG_BYPASS_MODE);
            gui_text_t *t = gui_text_create(img, 0, 156, 33, 148, 60);
            gui_text_set(t, tips_str_array[index], GUI_FONT_SRC_BMP, font_color, strlen(tips_str_array[index]),
                         20);
            gui_text_type_set(t, HEADING_1_BIN, FONT_SRC_MEMADDR);
            gui_text_mode_set(t, MULTI_CENTER);
        }
        break;
    case 3:
        {
            if (theme_bg_white)
            {
                font_color = FG_1_LIGHT;
                img_data = TIP_4_FG_LIGHT_BIN;
            }
            else
            {
                font_color = FG_1_DARK;
                img_data = TIP_4_FG_DARK_BIN;
            }
            gui_img_t *img = gui_img_create_from_mem(obj, 0, img_data, 16, 10, 0, 0);
            gui_img_set_mode(img, IMG_BYPASS_MODE);
            gui_text_t *t = gui_text_create(img, 0, 156, 20, 148, 60);
            gui_text_set(t, tips_str_array[index], GUI_FONT_SRC_BMP, font_color, strlen(tips_str_array[index]),
                         20);
            gui_text_type_set(t, HEADING_1_BIN, FONT_SRC_MEMADDR);
            gui_text_mode_set(t, MULTI_CENTER);
        }
        break;
    case 4:
        {
            if (theme_bg_white)
            {
                font_color = FG_1_LIGHT;
                img_data = TIP_5_FG_LIGHT_BIN;
            }
            else
            {
                font_color = FG_1_DARK;
                img_data = TIP_5_FG_DARK_BIN;
            }
            gui_img_t *img = gui_img_create_from_mem(obj, 0, img_data, 16, 24, 0, 0);
            gui_img_set_mode(img, IMG_BYPASS_MODE);
            gui_text_t *t = gui_text_create(img, 0, 156, 20, 148, 60);
            gui_text_set(t, tips_str_array[index], GUI_FONT_SRC_BMP, font_color, strlen(tips_str_array[index]),
                         20);
            gui_text_type_set(t, HEADING_1_BIN, FONT_SRC_MEMADDR);
            gui_text_mode_set(t, MULTI_CENTER);
        }
        break;
    default:
        break;
    }
}

static void reset_note_design(gui_obj_t *obj, void *p)
{
    GUI_UNUSED(p);
    gui_list_note_t *note = (gui_list_note_t *)obj;
    int16_t index = note->index;
    gui_color_t font_color;
    gui_color_t img_color;
    gui_color_t bg_color;
    if (theme_bg_white)
    {
        font_color = FG_1_LIGHT;
        img_color = BG_THEME3_BRIGHT_LIGHT;
        bg_color = SCREEN_BG_LIGHT;
    }
    else
    {
        font_color = FG_1_DARK;
        img_color = BG_THEME3_BRIGHT_DARK;
        bg_color = SCREEN_BG_DARK;
    }
    if (index == 0)
    {
        int font_size = 30;
        gui_text_t *t = gui_text_create(note, 0, 12, 0, 296, 90);
        gui_text_set(t, reset_warning_str, GUI_FONT_SRC_BMP, font_color, strlen(reset_warning_str),
                     font_size);
        gui_text_type_set(t, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, MULTI_LEFT);
    }
    else
    {
        gui_img_t *img = gui_img_create_from_mem(note, 0, BUTTON_BG_ELLIPSE_201_80_BIN, 60, 5, 0, 0);
        gui_img_a8_recolor(img, img_color.color.argb_full);
        gui_img_a8_fix_bg(img, bg_color.color.argb_full);
        if (f_status.ble)
        {
            gui_obj_add_event_cb(img, click_button_reset, GUI_EVENT_TOUCH_CLICKED, NULL);
        }
        else
        {
            // try to connnect ble, if connected, send message to GUI task to regenerate current view
            gui_img_a8_mix_alpha(img, 122);
            font_color.color.argb_full = theme_bg_white ? GUI_COLOR_ARGB8888(0xFF, 0x93, 0x93,
                                                                             0x93) : GUI_COLOR_ARGB8888(0xFF, 122, 122, 122);
        }
        gui_text_t *t = gui_text_create(img, 0, 0, 0, 201, 80);
        gui_text_set(t, "Reset", GUI_FONT_SRC_BMP, font_color, 5, 32);
        gui_text_type_set(t, CAPTION_1_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, MID_CENTER);
    }
}


static void tips_list_timer_cb(void *obj)
{
    uint8_t index = ((gui_list_t *)obj)->last_created_note_index;
    if (GUI_BASE(obj)->matrix->m[0][2] == 0)
    {
        if (index != page_index)
        {
            page_index = index;
            update_page_indicator();
        }
    }
    else if (page_index != 5 && *quick_view->pView)
    {
        page_index = 5;
        update_page_indicator();
    }
}

static void reset_list_timer_cb(void *obj)
{
    gui_list_t *list = (gui_list_t *)obj;
    int16_t list_offset = list->offset;
    gui_obj_t *scrollbar_bg = gui_list_entry(GUI_BASE(obj)->brother_list.next, gui_obj_t,
                                             brother_list);
    gui_obj_t *scrollbar = gui_list_entry(scrollbar_bg->brother_list.next, gui_obj_t,
                                          brother_list);
    int range = list->base.h - list->total_length;
    int t_y = list_offset * (SCROLL_BAR_BG_H - SCROLL_BAR_L_H) / range;
    gui_img_translate((void *)scrollbar, 0, t_y);
}

static void resetting_timer_cb(void *obj)
{
    static uint8_t cnt = 0;
    uint8_t cnt_max = 50;
    cnt++;
    if (f_status.factory_reset_done)
    {
        gui_view_switch_direct(current_view, quick_view, SWITCH_OUT_NONE_ANIMATION,
                               SWITCH_IN_NONE_ANIMATION);
    }
    if (cnt > cnt_max)
    {
        cnt = 0;
    }
    gui_img_rotation(obj, cnt * 360.f / cnt_max);
}

static void resetting_design(gui_obj_t *parent)
{
    gui_view_set_animate_step((void *)parent, 400);
    gui_view_switch_on_event((void *)parent, quick_view, SWITCH_OUT_NONE_ANIMATION,
                             SWITCH_IN_NONE_ANIMATION,
                             GUI_EVENT_TOUCH_CLICKED); //test

    gui_color_t font_color;
    gui_color_t img_color;
    if (theme_bg_white)
    {
        font_color = FG_1_LIGHT;
        img_color = FG_THEME3_LIGHT;
    }
    else
    {
        font_color = FG_1_DARK;
        img_color = FG_THEME3_DARK;
    }
    gui_img_t *reset = gui_img_create_from_mem(parent, 0, ICON_RESET_BIG_BIN, 138 + 22, 34 + 17, 0, 0);
    gui_img_set_focus(reset, 22, 17);
    gui_img_set_quality(reset, true);
    gui_obj_create_timer(GUI_BASE(reset), 100, true, resetting_timer_cb);
    gui_img_a8_recolor(reset, img_color.color.argb_full);

    gui_text_t *t = gui_text_create(parent, 0, 0, 100, 320, 60);
    gui_text_set(t, (void *)resetting_warning_str, GUI_FONT_SRC_BMP, font_color,
                 strlen(resetting_warning_str), 30);
    gui_text_type_set(t, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(t, MULTI_CENTER);
}

static void factory_reset_design(gui_obj_t *parent)
{
    gui_color_t mask_color;
    gui_color_t scrollbar_color;
    if (theme_bg_white)
    {
        mask_color = SCREEN_BG_LIGHT;
        scrollbar_color = FG_BLACK;
    }
    else
    {
        mask_color = SCREEN_BG_DARK;
        scrollbar_color = FG_WHITE;
    }

    gui_list_t *list = gui_list_create(parent, 0, 0, 60, 0, 0, 90, 0,
                                       VERTICAL, reset_note_design, NULL, 0);
    gui_list_set_style(list, LIST_CLASSIC);
    gui_list_set_note_num(list, 2);
    gui_list_set_out_scope(list, 20);
    gui_obj_create_timer(GUI_BASE(list), 10, true, reset_list_timer_cb);

    gui_img_t *scrollbar_bg = gui_img_create_from_mem(parent, 0, SCROLLBAR_BG_BIN, 310, 86, 0, 0);
    gui_img_t *scrollbar = gui_img_create_from_mem(parent, 0, SCROLLBAR_L_BIN, 310, 86, 0, 0);

    gui_img_t *mask = gui_img_create_from_mem(parent, 0, MASK_BIN, 0, 0, 0, 0);
    gui_img_a8_recolor(mask, mask_color.color.argb_full);
    gui_img_a8_fix_bg(mask, mask_color.color.argb_full);
    gui_img_a8_recolor(scrollbar_bg, scrollbar_color.color.argb_full);
    gui_img_a8_recolor(scrollbar, scrollbar_color.color.argb_full);
}

static void product_tips_design(gui_obj_t *parent)
{
    gui_view_switch_on_event((void *)parent, quick_view, SWITCH_OUT_TO_LEFT_USE_TRANSLATION,
                             SWITCH_IN_FROM_RIGHT_USE_TRANSLATION, GUI_EVENT_TOUCH_MOVE_LEFT);
    gui_color_t img_color;
    if (theme_bg_white)
    {
        img_color = SCREEN_BG_LIGHT;
    }
    else
    {
        img_color = SCREEN_BG_DARK;
    }

    page_index = 0;
    gui_list_t *list = gui_list_create(parent, 0, 0, 44, 0, 140, 320, 0, HORIZONTAL,
                                       tips_note_design, NULL, false);
    gui_list_set_style(list, LIST_CLASSIC);
    gui_list_set_note_num(list, 5);
    gui_list_set_auto_align(list, true);
    gui_list_set_inertia(list, false);
    gui_obj_create_timer(GUI_BASE(list), 10, true, tips_list_timer_cb);

    page_indicator_array = gui_malloc(sizeof(gui_img_t *) * 6);
    page_indicator_array[0] = gui_img_create_from_mem(parent, 0, PAGE_INDICATOR_BIN, 126, 188, 0, 0);
    page_indicator_array[1] = gui_img_create_from_mem(parent, 0, PAGE_INDICATOR_BIN, 138, 188, 0, 0);
    page_indicator_array[2] = gui_img_create_from_mem(parent, 0, PAGE_INDICATOR_BIN, 150, 188, 0, 0);
    page_indicator_array[3] = gui_img_create_from_mem(parent, 0, PAGE_INDICATOR_BIN, 162, 188, 0, 0);
    page_indicator_array[4] = gui_img_create_from_mem(parent, 0, PAGE_INDICATOR_BIN, 174, 188, 0, 0);
    page_indicator_array[5] = gui_img_create_from_mem(parent, 0, PAGE_INDICATOR_HEART_BIN, 187, 187, 0,
                                                      0);

    update_page_indicator();
}

static void support_view_design(gui_view_t *view)
{
    gui_view_set_animate_step(view, 10);
    gui_obj_t *parent = GUI_BASE(view);
    if (!strcmp(gui_view_get_current()->descriptor->name, "oobe_view"))
    {
        product_tips_design(parent);
        return;
    }


    if (support_reset)
    {
        factory_reset_design(parent);
        gui_text_t *t = gui_text_create(parent, 0, 0, 0, 320, 60);
        gui_text_set(t, (void *)page_name_array[FACTORY_RESET], GUI_FONT_SRC_BMP,
                     theme_bg_white ? FG_1_LIGHT : FG_1_DARK, strlen(page_name_array[FACTORY_RESET]), 30);
        gui_text_type_set(t, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, MID_CENTER);
    }
    else
    {
        product_tips_design(parent);
        gui_text_t *t = gui_text_create(parent, 0, 0, 0, 320, 60);
        gui_text_set(t, (void *)page_name_array[PRODUCT_TIPS], GUI_FONT_SRC_BMP,
                     theme_bg_white ? FG_1_LIGHT : FG_1_DARK, strlen(page_name_array[PRODUCT_TIPS]), 30);
        gui_text_type_set(t, CAPTION_3_30_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(t, MID_CENTER);
    }

    gui_win_t *win_icon_back = (gui_win_t *)gui_win_create(parent, 0, 0, 0, 60, 60);
    gui_img_t *icon_back = gui_img_create_from_mem(win_icon_back, 0, ICON_BACK_BIN, 0, 8, 0, 0);
    gui_obj_add_event_cb(win_icon_back, click_button_back, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_img_a8_recolor(icon_back, theme_bg_white ? FG_1_LIGHT.color.argb_full :
                       FG_1_DARK.color.argb_full);
}

static void clear(gui_view_t *view)
{
    GUI_UNUSED(view);
    if (page_indicator_array)
    {
        gui_free(page_indicator_array);
        page_indicator_array = NULL;
    }
}
