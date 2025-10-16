/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "app_main.h"
#include "common_data.h"
#include "gui_img.h"
#include "gui_win.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "clock_view"

#define TAB_START_POS_X 17
#define TAB_START_POS_Y 60
#define TAB_STOP_POS_X  224
#define TAB_SIZE        80

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void clock_view_design(gui_view_t *view);

/*============================================================================*
 *                            Variables
 *============================================================================*/
/* View Management */
static gui_view_t *current_view = NULL;
static const gui_view_descriptor_t *lock_view = NULL;
static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = clock_view_design,
};

static void *clock4_num_array[10] = {0};

static const gui_view_descriptor_t *descriptor_rec = NULL;

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
    lock_view = gui_view_descriptor_get("lock_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

static void time_update(void *obj)
{
    GUI_UNUSED(obj);

    if (!timeinfo)
    {
        return;
    }
    switch (clock_style + 1)
    {
    case 1:
        break;
    case 2:
        break;
    case 3:
        {
            GUI_WIDGET_POINTER_BY_NAME_ROOT(hour, "hour", current_view);
            gui_img_rotation((gui_img_t *)hour, (timeinfo->tm_hour % 12) * 30.0f + timeinfo->tm_min / 2.f);
            GUI_WIDGET_POINTER_BY_NAME_ROOT(min, "min", current_view);
            gui_img_rotation((gui_img_t *)min, timeinfo->tm_min * 6.0f);
        }
        break;
    case 4:
        {
            GUI_WIDGET_POINTER_BY_NAME_ROOT(h_decimal, "h_decimal", current_view);
            gui_obj_t *h_single = gui_list_entry(h_decimal->brother_list.next, gui_obj_t, brother_list);
            gui_obj_t *m_decimal = gui_list_entry(h_single->brother_list.next, gui_obj_t, brother_list);
            gui_obj_t *m_single = gui_list_entry(m_decimal->brother_list.next, gui_obj_t, brother_list);
            gui_obj_t *m_decimal_mask = gui_list_entry(m_single->brother_list.next, gui_obj_t, brother_list);
            gui_obj_t *m_single_mask = gui_list_entry(m_decimal_mask->brother_list.next, gui_obj_t,
                                                      brother_list);

            int tm_hour = time_format_24 ? timeinfo->tm_hour : (timeinfo->tm_hour % 12 == 0 ? 12 :
                                                                timeinfo->tm_hour % 12);
            gui_img_set_image_data((gui_img_t *)h_decimal, clock4_num_array[tm_hour / 10]);
            gui_img_set_image_data((gui_img_t *)h_single, clock4_num_array[tm_hour % 10]);
            gui_img_set_image_data((gui_img_t *)m_decimal, clock4_num_array[timeinfo->tm_min / 10]);
            gui_img_set_image_data((gui_img_t *)m_single, clock4_num_array[timeinfo->tm_min % 10]);
            gui_img_set_image_data((gui_img_t *)m_decimal_mask, clock4_num_array[timeinfo->tm_min / 10]);
            gui_img_set_image_data((gui_img_t *)m_single_mask, clock4_num_array[timeinfo->tm_min % 10]);
        }
        break;
    case 5:
        break;
    default:
        break;
    }

}

static void clock3_bg_update(void *obj)
{
    GUI_UNUSED(obj);
    gui_img_t *vec = (gui_img_t *)obj;
    static uint8_t cnt = 0;
    uint32_t color = 0;
    cnt++;
    switch (cnt % 5)
    {
    case 0:
        color = theme_color.color.argb_full;
        break;
    case 1:
        color = GUI_COLOR_ARGB8888(255, 0xFF, 0x59, 0x01);
        break;
    case 2:
        color = GUI_COLOR_ARGB8888(255, 0x8F, 0xBF, 0x80);
        break;
    case 3:
        color = GUI_COLOR_ARGB8888(255, 0x7C, 0x9E, 0xFF);
        break;
    case 4:
        color = GUI_COLOR_ARGB8888(255, 0xB4, 0x90, 0xFF);
        break;

    default:
        color = theme_color.color.argb_full;
        break;
    }
    theme_color.color.argb_full = color;
    gui_img_a8_fix_bg(vec, color);
}

static void clock4_bg_update(void *obj)
{
    GUI_UNUSED(obj);
    gui_img_t *vec = (gui_img_t *)obj;
    static uint8_t cnt = 0;
    uint32_t color = 0;
    cnt++;
    GUI_WIDGET_POINTER_BY_NAME_ROOT(m_decimal, "m_decimal", current_view);
    GUI_WIDGET_POINTER_BY_NAME_ROOT(m_single, "m_single", current_view);

    switch (cnt % 5)
    {
    case 0:
        color = theme_color.color.argb_full;
        break;
    case 1:
        color = GUI_COLOR_ARGB8888(255, 0xFF, 0x59, 0x01);
        break;
    case 2:
        color = GUI_COLOR_ARGB8888(255, 0x8F, 0xBF, 0x80);
        break;
    case 3:
        color = GUI_COLOR_ARGB8888(255, 0x7C, 0x9E, 0xFF);
        break;
    case 4:
        color = GUI_COLOR_ARGB8888(255, 0xB4, 0x90, 0xFF);
        break;

    default:
        color = theme_color.color.argb_full;
        break;
    }
    theme_color.color.argb_full = color;
    gui_img_a8_recolor(vec, color);
    gui_img_a8_recolor((void *)m_decimal, color);
    gui_img_a8_recolor((void *)m_single, color);
}

static void clock3_design(gui_view_t *view)
{
    gui_obj_t *parent = GUI_BASE(view);
    // Background
    gui_img_t *vec = gui_img_create_from_mem(parent, 0, CLOCK3_VECTOR_BIN, 0, 0, 0, 0);
    gui_img_a8_recolor(vec, FG_1_DARK.color.argb_full);
    gui_img_a8_fix_bg(vec, theme_color.color.argb_full);
    gui_img_set_mode(vec, IMG_2D_SW_FIX_A8_BGFG);
    gui_obj_create_timer(GUI_BASE(vec), 1000, true, clock3_bg_update);

    // time hands
    gui_img_t *img = gui_img_create_from_mem(parent, "min", CLOCK3_MIN_BIN, 160, 100, 0, 0);
    gui_img_set_mode(img, IMG_2D_SW_SRC_OVER_MODE);
    // gui_img_a8_recolor(img, SCREEN_BG_DARK.color.argb_full);
    gui_img_set_focus(img, 160 - 146, 100 - 7); // img target is (146, 7), focus is (160, 100)
    img = gui_img_create_from_mem(parent, "hour", CLOCK3_HOUR_BIN, 160, 100, 0, 0);
    gui_img_set_mode(img, IMG_2D_SW_SRC_OVER_MODE);
    // gui_img_a8_recolor(img, SCREEN_BG_DARK.color.argb_full);
    gui_img_set_focus(img, 160 - 146, 100 - 27); // img target is (146, 27), focus is (160, 100)
    img = gui_img_create_from_mem(parent, 0, CLOCK3_CENTER_BIN, 146, 86, 0, 0);
    gui_img_a8_recolor(img, FG_WHITE.color.argb_full);
}

static void clock4_design(gui_view_t *view)
{
    if (!clock4_num_array[0])
    {
        clock4_num_array[0] = CLOCK_4_NUM_0_BIN;
        clock4_num_array[1] = CLOCK_4_NUM_1_BIN;
        clock4_num_array[2] = CLOCK_4_NUM_2_BIN;
        clock4_num_array[3] = CLOCK_4_NUM_3_BIN;
        clock4_num_array[4] = CLOCK_4_NUM_4_BIN;
        clock4_num_array[5] = CLOCK_4_NUM_5_BIN;
        clock4_num_array[6] = CLOCK_4_NUM_6_BIN;
        clock4_num_array[7] = CLOCK_4_NUM_7_BIN;
        clock4_num_array[8] = CLOCK_4_NUM_8_BIN;
        clock4_num_array[9] = CLOCK_4_NUM_9_BIN;
    }

    gui_obj_t *parent = GUI_BASE(view);

    gui_img_t *vec = gui_img_create_from_mem(parent, "bg", CLOCK4_VECTOR_BIN, 55, 0, 0, 0);
    gui_img_a8_recolor(vec, theme_color.color.argb_full);
    gui_img_t *mask = gui_img_create_from_mem(parent, 0, CLOCK4_MASK_BIN, 55, 0, 0, 0);
    gui_img_set_mode(mask, IMG_2D_SW_SRC_OVER_MODE);

    gui_img_t *h_decimal = gui_img_create_from_mem(parent, "h_decimal", clock4_num_array[1], 102, 30, 0,
                                                   0);
    gui_img_t *h_single = gui_img_create_from_mem(parent, "h_single", clock4_num_array[0], 157, 30, 0,
                                                  0);
    gui_img_a8_recolor(h_decimal, FG_WHITE.color.argb_full);
    gui_img_a8_recolor(h_single, FG_WHITE.color.argb_full);
    gui_img_t *m_decimal = gui_img_create_from_mem(parent, "m_decimal", clock4_num_array[0], 102, 105,
                                                   0, 0);
    gui_img_t *m_single = gui_img_create_from_mem(parent, "m_single", clock4_num_array[8], 157, 105, 0,
                                                  0);
    gui_img_t *m_decimal_mask = gui_img_create_from_mem(parent, "m_decimal_mask", clock4_num_array[0],
                                                        102, 104, 0, 0);
    gui_img_t *m_single_mask = gui_img_create_from_mem(parent, "m_single_mask", clock4_num_array[8],
                                                       157, 104, 0, 0);
    gui_img_a8_recolor(m_decimal, theme_color.color.argb_full);
    gui_img_a8_recolor(m_single, theme_color.color.argb_full);
    gui_img_a8_recolor(m_decimal_mask, SCREEN_BG_DARK.color.argb_full);
    gui_img_a8_recolor(m_single_mask, SCREEN_BG_DARK.color.argb_full);
    gui_img_a8_mix_alpha(m_decimal_mask, 0xFF * 0.3f);
    gui_img_a8_mix_alpha(m_single_mask, 0xFF * 0.3f);

    gui_obj_create_timer(GUI_BASE(vec), 1000, true, clock4_bg_update);
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

static void clock_view_design(gui_view_t *view)
{
    gui_set_bg_color(SCREEN_BG_DARK);
    switch (clock_style + 1)
    {
    case 1:
        break;
    case 2:
        break;
    case 3:
        clock3_design(view);
        break;
    case 4:
        clock4_design(view);
        break;
    case 5:
        break;
    default:
        clock3_design(view);
        break;
    }

    if (switch_from_lock_screen)
    {
        descriptor_rec = gui_view_get_current()->descriptor;
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
        gui_view_switch_on_event(view, lock_view, SWITCH_OUT_NONE_ANIMATION,
                                 SWITCH_IN_NONE_ANIMATION,
                                 GUI_EVENT_TOUCH_CLICKED);
    }

    gui_obj_create_timer(GUI_BASE(view), 30000, true, time_update);
}