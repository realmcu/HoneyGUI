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

static void *clock_num_array[10] = {0};

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
        {
            GUI_WIDGET_POINTER_BY_NAME_ROOT(hour, "hour", current_view);
            gui_img_rotation((gui_img_t *)hour, (timeinfo->tm_hour % 12) * 30.0f + timeinfo->tm_min / 2.f);
            GUI_WIDGET_POINTER_BY_NAME_ROOT(min, "min", current_view);
            gui_img_rotation((gui_img_t *)min, timeinfo->tm_min * 6.0f);
        }
        break;
    case 2:
        {
            GUI_WIDGET_POINTER_BY_NAME_ROOT(m_decimal, "m_decimal", current_view);
            gui_obj_t *m_single = gui_list_entry(m_decimal->brother_list.next, gui_obj_t, brother_list);
            gui_obj_t *h_decimal = gui_list_entry(m_single->brother_list.next, gui_obj_t, brother_list);
            gui_obj_t *h_single = gui_list_entry(h_decimal->brother_list.next, gui_obj_t, brother_list);

            int tm_hour = time_format_24 ? timeinfo->tm_hour : (timeinfo->tm_hour % 12 == 0 ? 12 :
                                                                timeinfo->tm_hour % 12);
            gui_img_set_image_data((gui_img_t *)h_decimal, clock_num_array[tm_hour / 10]);
            gui_img_set_image_data((gui_img_t *)h_single, clock_num_array[tm_hour % 10]);
            gui_img_set_image_data((gui_img_t *)m_decimal, clock_num_array[timeinfo->tm_min / 10]);
            gui_img_set_image_data((gui_img_t *)m_single, clock_num_array[timeinfo->tm_min % 10]);
        }
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
            gui_img_set_image_data((gui_img_t *)h_decimal, clock_num_array[tm_hour / 10]);
            gui_img_set_image_data((gui_img_t *)h_single, clock_num_array[tm_hour % 10]);
            gui_img_set_image_data((gui_img_t *)m_decimal, clock_num_array[timeinfo->tm_min / 10]);
            gui_img_set_image_data((gui_img_t *)m_single, clock_num_array[timeinfo->tm_min % 10]);
            gui_img_set_image_data((gui_img_t *)m_decimal_mask, clock_num_array[timeinfo->tm_min / 10]);
            gui_img_set_image_data((gui_img_t *)m_single_mask, clock_num_array[timeinfo->tm_min % 10]);
        }
        break;
    case 5:
        {
            GUI_WIDGET_POINTER_BY_NAME_ROOT(min, "min", current_view);
            gui_obj_t *min_mask = gui_list_entry(min->brother_list.next, gui_obj_t, brother_list);
            gui_obj_t *hour = gui_list_entry(min_mask->brother_list.next, gui_obj_t, brother_list);
            gui_obj_t *hour_mask = gui_list_entry(hour->brother_list.next, gui_obj_t, brother_list);
            gui_img_rotation((gui_img_t *)min, timeinfo->tm_min * 6.0f);
            gui_img_rotation((gui_img_t *)min_mask, ((gui_img_t *)min)->degrees);
            gui_img_rotation((gui_img_t *)hour, (timeinfo->tm_hour % 12) * 30.0f + timeinfo->tm_min / 2.f);
            gui_img_rotation((gui_img_t *)hour_mask, ((gui_img_t *)hour)->degrees);
        }
        break;
    default:
        break;
    }

}

static void clock1_bg_update(void *obj)
{
    GUI_UNUSED(obj);

    gui_img_t *img_0 = (gui_img_t *)obj;
    theme_color_index++;
    theme_color_index %= 5;
    uint32_t color = theme_color_array[theme_color_index];

    gui_img_t *img_1 = (gui_img_t *)gui_list_entry(GUI_BASE(img_0)->brother_list.next, gui_obj_t,
                                                   brother_list);
    gui_img_t *img_2 = (gui_img_t *)gui_list_entry(GUI_BASE(img_1)->brother_list.next, gui_obj_t,
                                                   brother_list);
    gui_img_t *img_3 = (gui_img_t *)gui_list_entry(GUI_BASE(img_2)->brother_list.next, gui_obj_t,
                                                   brother_list);
    GUI_WIDGET_POINTER_BY_NAME_ROOT(center, "center", current_view);
    img_0->fg_color_set = color;
    img_1->fg_color_set = color;
    img_2->fg_color_set = color;
    img_3->fg_color_set = color;
    gui_img_a8_recolor((void *)center, color);
}

static void clock2_bg_update(void *obj)
{
    GUI_UNUSED(obj);
    theme_color_index++;
    theme_color_index %= 5;
    uint32_t color = theme_color_array[theme_color_index];

    GUI_WIDGET_POINTER_BY_NAME_ROOT(m_decimal, "m_decimal", current_view);
    GUI_WIDGET_POINTER_BY_NAME_ROOT(m_single, "m_single", current_view);

    gui_img_a8_recolor((void *)m_decimal, color);
    gui_img_a8_recolor((void *)m_single, color);
}

static void clock3_bg_update(void *obj)
{
    GUI_UNUSED(obj);

    gui_img_t *vec = (gui_img_t *)obj;
    theme_color_index++;
    theme_color_index %= 5;
    uint32_t color = theme_color_array[theme_color_index];

    gui_img_a8_fix_bg(vec, color);
}

static void clock4_bg_update(void *obj)
{
    GUI_UNUSED(obj);
    gui_img_t *vec = (gui_img_t *)obj;
    theme_color_index++;
    theme_color_index %= 5;
    uint32_t color = theme_color_array[theme_color_index];

    GUI_WIDGET_POINTER_BY_NAME_ROOT(m_decimal, "m_decimal", current_view);
    GUI_WIDGET_POINTER_BY_NAME_ROOT(m_single, "m_single", current_view);

    gui_img_a8_recolor(vec, color);
    // gui_img_set_mode(vec, IMG_SRC_OVER_MODE_2);
    gui_img_a8_recolor((void *)m_decimal, color);
    gui_img_a8_recolor((void *)m_single, color);
}

static void clock5_bg_update(void *obj)
{
    GUI_UNUSED(obj);
    gui_img_t *bg = (gui_img_t *)obj;
    theme_color_index++;
    theme_color_index %= 5;
    uint32_t color = theme_color_array[theme_color_index];

    GUI_WIDGET_POINTER_BY_NAME_ROOT(min, "min", current_view);
    GUI_WIDGET_POINTER_BY_NAME_ROOT(hour, "hour", current_view);
    GUI_WIDGET_POINTER_BY_NAME_ROOT(center, "center", current_view);
    gui_img_a8_recolor(bg, color);
    gui_img_a8_recolor((void *)center, color);
    gui_img_a8_recolor((void *)min, color);

    uint8_t r = (uint8_t)((0xFF - (uint8_t)(color >> 16)) * 0.2f + (uint8_t)(color >> 16));
    uint8_t g = (uint8_t)((0xFF - (uint8_t)(color >> 8)) * 0.2f + (uint8_t)(color >> 8));
    uint8_t b = (uint8_t)((0xFF - (uint8_t)(color)) * 0.2f + (uint8_t)(color));
    gui_img_a8_recolor((void *)hour, GUI_COLOR_ARGB8888(0xFF, r, g, b));

}

static void clock1_design(gui_view_t *view)
{
    gui_obj_t *parent = GUI_BASE(view);
    // Background
    gui_img_t *vec = gui_img_create_from_mem(parent, 0, CLOCK1_BG_BIN, 0, 0, 0, 0);
    gui_img_set_mode(vec, IMG_BYPASS_MODE);
    // gui_img_a8_recolor(vec, GUI_COLOR_ARGB8888(255, 0x2F, 0x2F, 0x2F));
    // gui_img_a8_fix_bg(vec, SCREEN_BG_DARK.color.argb_full);

    // time numbers
    gui_img_t *img = gui_img_create_from_mem(parent, 0, CLOCK1_NUM_12_BIN, 144, 6, 0, 0);
    gui_img_a8_recolor(img, theme_color_array[theme_color_index]);
    gui_img_a8_fix_bg(img, SCREEN_BG_DARK.color.argb_full);
    gui_obj_create_timer(GUI_BASE(img), 1000, true, clock1_bg_update);
    gui_obj_start_timer(GUI_BASE(img));
    img = gui_img_create_from_mem(parent, 0, CLOCK1_NUM_6_BIN, 151, 168, 0, 0);
    gui_img_a8_recolor(img, theme_color_array[theme_color_index]);
    gui_img_a8_fix_bg(img, SCREEN_BG_DARK.color.argb_full);
    img = gui_img_create_from_mem(parent, 0, CLOCK1_NUM_9_BIN, 19, 86, 0, 0);
    gui_img_a8_recolor(img, theme_color_array[theme_color_index]);
    gui_img_a8_fix_bg(img, SCREEN_BG_DARK.color.argb_full);
    img = gui_img_create_from_mem(parent, 0, CLOCK1_NUM_3_BIN, 266, 86, 0, 0);
    gui_img_a8_recolor(img, theme_color_array[theme_color_index]);
    gui_img_a8_fix_bg(img, SCREEN_BG_DARK.color.argb_full);

    // time hands
    img = gui_img_create_from_mem(parent, "min", CLOCK1_MIN_BIN, 160, 100, 0, 0);
    gui_img_set_quality(img, true);
    gui_img_a8_recolor(img, FG_WHITE.color.argb_full);
    gui_img_set_focus(img, 160 - 152, 100 - 18); // img target is (152, 18), focus is (160, 100)
    img = gui_img_create_from_mem(parent, "hour", CLOCK1_HOUR_BIN, 160, 100, 0, 0);
    gui_img_set_quality(img, true);
    gui_img_a8_recolor(img, FG_WHITE.color.argb_full);
    gui_img_set_focus(img, 160 - 148, 100 - 48); // img target is (148, 48), focus is (160, 100)
    img = gui_img_create_from_mem(parent, "center", CLOCK1_CENTER_BIN, 154, 94, 0, 0);
    gui_img_a8_recolor(img, theme_color_array[theme_color_index]);
    img = gui_img_create_from_mem(parent, 0, CLOCK1_CENTER_EDGE_BIN, 154, 94, 0, 0);
    gui_img_a8_recolor(img, SCREEN_BG_DARK.color.argb_full);
}

static void clock2_design(gui_view_t *view)
{
    clock_num_array[0] = CLOCK2_NUM_0_BIN;
    clock_num_array[1] = CLOCK2_NUM_1_BIN;
    clock_num_array[2] = CLOCK2_NUM_2_BIN;
    clock_num_array[3] = CLOCK2_NUM_3_BIN;
    clock_num_array[4] = CLOCK2_NUM_4_BIN;
    clock_num_array[5] = CLOCK2_NUM_5_BIN;
    clock_num_array[6] = CLOCK2_NUM_6_BIN;
    clock_num_array[7] = CLOCK2_NUM_7_BIN;
    clock_num_array[8] = CLOCK2_NUM_8_BIN;
    clock_num_array[9] = CLOCK2_NUM_9_BIN;

    gui_obj_t *parent = GUI_BASE(view);

    gui_img_t *m_decimal = gui_img_create_from_mem(parent, "m_decimal", clock_num_array[0], 105, 75,
                                                   0, 0);
    gui_img_t *m_single = gui_img_create_from_mem(parent, "m_single", clock_num_array[8], 210, 75, 0,
                                                  0);
    gui_img_t *h_decimal = gui_img_create_from_mem(parent, "h_decimal", clock_num_array[1], 0, -15, 0,
                                                   0);
    gui_img_t *h_single = gui_img_create_from_mem(parent, "h_single", clock_num_array[0], 105, -15, 0,
                                                  0);
    gui_img_a8_recolor(h_decimal, FG_WHITE.color.argb_full);
    gui_img_a8_recolor(h_single, FG_WHITE.color.argb_full);
    gui_img_a8_recolor(m_decimal, theme_color_array[theme_color_index]);
    gui_img_a8_recolor(m_single, theme_color_array[theme_color_index]);

    gui_obj_create_timer(GUI_BASE(m_decimal), 1000, true, clock2_bg_update);
}

static void clock3_design(gui_view_t *view)
{
    gui_obj_t *parent = GUI_BASE(view);
    // Background
    gui_img_t *vec = gui_img_create_from_mem(parent, 0, CLOCK3_VECTOR_BIN, 0, 0, 0, 0);
    gui_img_a8_recolor(vec, FG_1_DARK.color.argb_full);
    gui_img_a8_fix_bg(vec, theme_color_array[theme_color_index]);
    gui_img_set_mode(vec, IMG_2D_SW_FIX_A8_BGFG);
    gui_obj_create_timer(GUI_BASE(vec), 1000, true, clock3_bg_update);
    gui_obj_start_timer(GUI_BASE(vec));

    // time hands
    gui_img_t *img = gui_img_create_from_mem(parent, "min", CLOCK3_MIN_BIN, 160, 100, 0, 0);
    // gui_img_set_mode(img, IMG_2D_SW_SRC_OVER_MODE);
    gui_img_set_quality(img, true);
    gui_img_a8_recolor(img, SCREEN_BG_DARK.color.argb_full);
    gui_img_set_focus(img, 160 - 149, 100 - 16); // img target is (149, 16), focus is (160, 100)
    img = gui_img_create_from_mem(parent, "hour", CLOCK3_HOUR_BIN, 160, 100, 0, 0);
    // gui_img_set_mode(img, IMG_2D_SW_SRC_OVER_MODE);
    gui_img_set_quality(img, true);
    gui_img_a8_recolor(img, SCREEN_BG_DARK.color.argb_full);
    gui_img_set_focus(img, 160 - 145, 100 - 47); // img target is (145, 47), focus is (160, 100)
    img = gui_img_create_from_mem(parent, 0, CLOCK3_CENTER_BIN, 145, 86, 0, 0);
    gui_img_a8_recolor(img, FG_WHITE.color.argb_full);
}

static void clock4_design(gui_view_t *view)
{
    clock_num_array[0] = CLOCK4_NUM_0_BIN;
    clock_num_array[1] = CLOCK4_NUM_1_BIN;
    clock_num_array[2] = CLOCK4_NUM_2_BIN;
    clock_num_array[3] = CLOCK4_NUM_3_BIN;
    clock_num_array[4] = CLOCK4_NUM_4_BIN;
    clock_num_array[5] = CLOCK4_NUM_5_BIN;
    clock_num_array[6] = CLOCK4_NUM_6_BIN;
    clock_num_array[7] = CLOCK4_NUM_7_BIN;
    clock_num_array[8] = CLOCK4_NUM_8_BIN;
    clock_num_array[9] = CLOCK4_NUM_9_BIN;

    gui_obj_t *parent = GUI_BASE(view);

    gui_img_t *vec = gui_img_create_from_mem(parent, "bg", CLOCK4_VECTOR_BIN, 61, 1, 0, 0);
    gui_img_a8_recolor(vec, theme_color_array[theme_color_index]);
    gui_img_t *mask = gui_img_create_from_mem(parent, 0, CLOCK4_MASK_BIN, 61, 1, 0, 0);
    // gui_img_a8_recolor(mask, FG_WHITE.color.argb_full);
    gui_img_set_mode(mask, IMG_2D_SW_SRC_OVER_MODE);

    // gui_img_t *vec = gui_img_create_from_mem(parent, "bg", CLOCK4_BG_BIN, 61, 1, 0, 0);
    // gui_img_set_mode(vec, IMG_BYPASS_MODE);
    // gui_img_t *mask = gui_img_create_from_mem(parent, 0, CLOCK4_MASK_BIN, 61, 1, 0, 0);
    // gui_img_a8_recolor(mask, theme_color_array[theme_color_index]);
    // gui_img_set_mode(mask, IMG_2D_SW_FIX_A8_SRC_OVER);

    gui_img_t *h_decimal = gui_img_create_from_mem(parent, "h_decimal", clock_num_array[1], 102, 30, 0,
                                                   0);
    gui_img_t *h_single = gui_img_create_from_mem(parent, "h_single", clock_num_array[0], 157, 30, 0,
                                                  0);
    gui_img_a8_recolor(h_decimal, FG_WHITE.color.argb_full);
    gui_img_a8_recolor(h_single, FG_WHITE.color.argb_full);
    gui_img_t *m_decimal = gui_img_create_from_mem(parent, "m_decimal", clock_num_array[0], 102, 105,
                                                   0, 0);
    gui_img_t *m_single = gui_img_create_from_mem(parent, "m_single", clock_num_array[8], 157, 105, 0,
                                                  0);
    gui_img_t *m_decimal_mask = gui_img_create_from_mem(parent, "m_decimal_mask", clock_num_array[0],
                                                        102, 104, 0, 0);
    gui_img_t *m_single_mask = gui_img_create_from_mem(parent, "m_single_mask", clock_num_array[8],
                                                       157, 104, 0, 0);
    gui_img_a8_recolor(m_decimal, theme_color_array[theme_color_index]);
    gui_img_a8_recolor(m_single, theme_color_array[theme_color_index]);
    gui_img_a8_recolor(m_decimal_mask, SCREEN_BG_DARK.color.argb_full);
    gui_img_a8_recolor(m_single_mask, SCREEN_BG_DARK.color.argb_full);
    gui_img_a8_mix_alpha(m_decimal_mask, 0xFF * 0.3f);
    gui_img_a8_mix_alpha(m_single_mask, 0xFF * 0.3f);

    // gui_obj_create_timer(GUI_BASE(mask), 1000, true, clock4_bg_update);
    // gui_obj_start_timer(GUI_BASE(mask));
    gui_obj_create_timer(GUI_BASE(vec), 1000, true, clock4_bg_update);
    gui_obj_start_timer(GUI_BASE(vec));
}

static void clock5_design(gui_view_t *view)
{
    gui_obj_t *parent = GUI_BASE(view);
    // Background
    gui_img_t *bg = gui_img_create_from_mem(parent, "bg", CLOCK5_BG_BIN, 109, 49, 0, 0);
    gui_img_a8_recolor(bg, theme_color_array[theme_color_index]);
    gui_img_a8_fix_bg(bg, GUI_COLOR_ARGB8888(255, 0, 0, 0));
    gui_obj_create_timer(GUI_BASE(bg), 1000, true, clock5_bg_update);
    gui_obj_start_timer(GUI_BASE(bg));

    // time hands
    gui_img_t *img = gui_img_create_from_mem(parent, "min", CLOCK5_MIN_BIN, 160, 100, 0, 0);
    gui_img_set_quality(img, true);
    gui_img_a8_recolor(img, theme_color_array[theme_color_index]);
    gui_img_set_focus(img, 160 - 139, 100 - 13); // img target is (139, 13), focus is (160, 100)
    img = gui_img_create_from_mem(parent, "min_mask", CLOCK5_MIN_MASK_BIN, 160, 100, 0, 0);
    gui_img_set_quality(img, true);
    gui_img_a8_recolor(img, FG_WHITE.color.argb_full);
    gui_img_set_focus(img, 160 - 139, 100 - 13); // img target is (139, 13), focus is (160, 100)
    img = gui_img_create_from_mem(parent, "hour", CLOCK5_HOUR_BIN, 160, 100, 0, 0);
    gui_img_set_quality(img, true);
    uint32_t color = theme_color_array[theme_color_index];
    uint8_t r = (uint8_t)((0xFF - (uint8_t)(color >> 16)) * 0.2f + (uint8_t)(color >> 16));
    uint8_t g = (uint8_t)((0xFF - (uint8_t)(color >> 8)) * 0.2f + (uint8_t)(color >> 8));
    uint8_t b = (uint8_t)((0xFF - (uint8_t)(color)) * 0.2f + (uint8_t)(color));
    gui_img_a8_recolor((void *)img, GUI_COLOR_ARGB8888(0xFF, r, g, b));
    gui_img_set_focus(img, 160 - 139, 100 - 41); // img target is (139, 41), focus is (160, 100)

    img = gui_img_create_from_mem(parent, "hour_mask", CLOCK5_HOUR_MASK_BIN, 160, 100, 0, 0);
    gui_img_set_quality(img, true);
    gui_img_a8_recolor(img, FG_WHITE.color.argb_full);
    gui_img_set_focus(img, 160 - 139, 100 - 41); // img target is (139, 41), focus is (160, 100)
    img = gui_img_create_from_mem(parent, "center", CLOCK5_CENTER_BIN, 150, 92, 0, 0);
    gui_img_a8_recolor(img, theme_color_array[theme_color_index]);
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
        clock1_design(view);
        break;
    case 2:
        clock2_design(view);
        break;
    case 3:
        clock3_design(view);
        break;
    case 4:
        clock4_design(view);
        break;
    case 5:
        clock5_design(view);
        break;
    default:
        clock1_design(view);
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

    gui_obj_create_timer(GUI_BASE(view), 1000, true, time_update);
}