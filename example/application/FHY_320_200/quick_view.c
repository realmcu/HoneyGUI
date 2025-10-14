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
typedef enum
{
    ANC = 0,
    AA,
    TT
} AMBIENT_SOUND_STATUS;

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "quick_view"

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void quick_view_design(gui_view_t *view);
static void update_page_indicator(void);

/*============================================================================*
 *                            Variables
 *============================================================================*/
/* View Management */
static gui_view_t *current_view = NULL;
static const gui_view_descriptor_t *menu_view = NULL;
static const gui_view_descriptor_t *inform_center_view = NULL;
static const gui_view_descriptor_t *wallpaper_view = NULL;
static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = quick_view_design,
};

static char time_str[8] = {0};
static int8_t page_index = 0;
static gui_img_t *page_indicator_array[QUICK_PAGE_NUM_MAX] = {0};

int8_t quick_page_num = 4;
void (*quick_page_design_func_array[QUICK_PAGE_NUM_MAX])(gui_obj_t *parent) =
{
    page_dark_light_design,
    page_ambient_sound_design,
    page_music_design,
    page_volume_design
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
    menu_view = gui_view_descriptor_get("menu_view");
    inform_center_view = gui_view_descriptor_get("inform_center_view");
    wallpaper_view = gui_view_descriptor_get("wallpaper_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);


static void time_update_cb(void *obj)
{
    if (!timeinfo)
    {
        return;
    }
    sprintf(time_str, "%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min);
    gui_text_content_set((gui_text_t *)obj, time_str, strlen(time_str));
}

static void list_timer_cb(void *obj)
{
    uint8_t index = ((gui_list_t *)obj)->last_created_note_index;
    if (index != page_index)
    {
        page_index = index;
        update_page_indicator();
    }
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

    for (int i = 0; i < quick_page_num; i++)
    {
        if (page_indicator_array[i])
        {
            gui_img_set_a8_fg_color((gui_img_t *)page_indicator_array[i], indicator_color.color.argb_full);
        }
    }
    gui_img_set_a8_fg_color(page_indicator_array[page_index], indicator_color_cur.color.argb_full);
}

static void note_design(gui_obj_t *obj, void *p)
{
    GUI_UNUSED(p);
    uint16_t index = ((gui_list_note_t *)obj)->index;
    quick_page_design_func_array[index](obj);

}

static void create_indicator(gui_obj_t *parent)
{
    switch (quick_page_num)
    {
    case 1 :
        {
            page_indicator_array[0] = gui_img_create_from_mem(parent, 0, PAGE_INDICATOR_BIN, 158, 188, 0, 0);
        }
        break;
    case 2 :
        {
            page_indicator_array[0] = gui_img_create_from_mem(parent, 0, PAGE_INDICATOR_BIN, 150, 188, 0, 0);
            page_indicator_array[1] = gui_img_create_from_mem(parent, 0, PAGE_INDICATOR_BIN, 162, 188, 0, 0);
        }
        break;
    case 3 :
        {
            page_indicator_array[0] = gui_img_create_from_mem(parent, 0, PAGE_INDICATOR_BIN, 142, 188, 0, 0);
            page_indicator_array[1] = gui_img_create_from_mem(parent, 0, PAGE_INDICATOR_BIN, 154, 188, 0, 0);
            page_indicator_array[2] = gui_img_create_from_mem(parent, 0, PAGE_INDICATOR_BIN, 166, 188, 0, 0);
        }
        break;
    case 4 :
        {
            page_indicator_array[0] = gui_img_create_from_mem(parent, 0, PAGE_INDICATOR_BIN, 140, 188, 0, 0);
            page_indicator_array[1] = gui_img_create_from_mem(parent, 0, PAGE_INDICATOR_BIN, 152, 188, 0, 0);
            page_indicator_array[2] = gui_img_create_from_mem(parent, 0, PAGE_INDICATOR_BIN, 164, 188, 0, 0);
            page_indicator_array[3] = gui_img_create_from_mem(parent, 0, PAGE_INDICATOR_BIN, 176, 188, 0, 0);
        }
        break;
    default:
        break;
    }

}

static void quick_view_design(gui_view_t *view)
{
    gui_view_switch_on_event(view, menu_view, SWITCH_INIT_STATE,
                             SWITCH_IN_FROM_BOTTOM_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_UP);
    gui_view_switch_on_event(view, inform_center_view, SWITCH_INIT_STATE,
                             SWITCH_IN_FROM_TOP_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_DOWN);

    gui_view_switch_on_event(view, wallpaper_view, SWITCH_INIT_STATE,
                             SWITCH_IN_FROM_LEFT_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_RIGHT);

    gui_obj_t *parent = GUI_BASE(view);
    gui_color_t font_color;
    if (theme_bg_white)
    {
        gui_set_bg_color(SCREEN_BG_LIGHT);
    }
    else
    {
        font_color = FG_1_DARK;
    }
    create_indicator(parent);

    gui_img_t *bt = gui_img_create_from_mem(parent, 0, BLUETOOTH_BIN, 12, 13, 0, 0);
    gui_img_t *home_bg = gui_img_create_from_mem(parent, 0, STATUSBAR_HOME_BG_BIN, 131, 10, 0, 0);
    gui_img_t *barn_inner = gui_img_create_from_mem(home_bg, 0, BARN_INNER_BIN, 23, 6, 0, 0);
    gui_img_t *barn_outer = gui_img_create_from_mem(home_bg, 0, BARN_OUTER_BIN, 20, 3, 0, 0);
    gui_img_t *earphone_l = gui_img_create_from_mem(home_bg, 0, EARPLUG_L_BIN, 7, 3, 0, 0);
    gui_img_t *earphone_r = gui_img_create_from_mem(home_bg, 0, EARPLUG_R_BIN, 42, 3, 0, 0);

    gui_text_t *text = gui_text_create(parent, 0, 25, 8, 50, 15);
    gui_text_set(text, "AAC", GUI_FONT_SRC_BMP, font_color, 3, 28);
    gui_text_type_set(text, CAPTION_2_BIN, FONT_SRC_MEMADDR);
    gui_text_mode_set(text, LEFT);
    text = gui_text_create(parent, 0, 275, 8, 50, 15);
    gui_text_set(text, time_str, GUI_FONT_SRC_BMP, font_color, strlen(time_str), 28);
    gui_text_type_set(text, CAPTION_2_BIN, FONT_SRC_MEMADDR);
    gui_obj_create_timer(GUI_BASE(text), 3000, true, time_update_cb);
    gui_text_mode_set(text, LEFT);

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
    if (quick_page_num)
    {
        update_page_indicator();
    }
    gui_list_t *list = gui_list_create(parent, "list", 0, 0, 0, 0, 320, 0, HORIZONTAL,
                                       note_design, NULL,
                                       false);
    gui_list_set_style(list, LIST_CLASSIC); //if LIST_CARD style, must set style before set note num
    gui_list_set_note_num(list, quick_page_num);
    gui_list_set_auto_align(list, true);
    gui_list_set_offset(list, -page_index * 320);
    gui_list_set_inertia(list, false);
    gui_obj_create_timer(GUI_BASE(list), 10, true, list_timer_cb);
}