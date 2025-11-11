/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "app_main.h"
#include "common_data.h"
#include "gui_img.h"
#include "gui_text.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "ota_view"

#define ICON_TARGET_SCALE  0.5f

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void ota_view_design(gui_view_t *view);

/*============================================================================*
 *                            Variables
 *============================================================================*/
/* View Management */
static gui_view_t *current_view = NULL;
static const gui_view_descriptor_t *oobe_view = NULL;
static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = ota_view_design,
};

static uint8_t animation_cnt = 0;

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
    oobe_view = gui_view_descriptor_get("oobe_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

static void exit_wait(void *p)
{
    GUI_UNUSED(p);
    gui_view_switch_direct(current_view, oobe_view, SWITCH_OUT_NONE_ANIMATION,
                           SWITCH_IN_NONE_ANIMATION);
}

static void ota_cb(void *p)
{
    if (f_status.ota == OTA_FAIL)
    {
        msg_2_regenerate_current_view();
        return;
    }
    static bool positive = 1;
    gui_text_t *text = (gui_text_t *)p;
    gui_img_t *bg = (void *)gui_list_entry(GUI_BASE(text)->parent->child_list.next, gui_obj_t,
                                           brother_list);

    if (f_status.ota == OTA_DONE)
    {
        gui_img_t *icon = (void *)gui_list_entry(GUI_BASE(text)->brother_list.prev, gui_obj_t,
                                                 brother_list);
        float scale = bg->scale_x;
        scale += 0.05f;
        if (scale >= 3.f)
        {
            gui_text_color_set(text, gui_rgb(0xFF, 0xFF, 0xFF));
            gui_text_content_set(text, "Complete!", 10);
            gui_img_set_image_data(icon, ICON_OTA_COMPLETE_BIN);
            gui_img_a8_mix_alpha(icon, 0xFF);

            gui_obj_create_timer(p, 2000, false, exit_wait);
            gui_obj_start_timer(p);
        }
        else
        {
            uint8_t val = 50 * (3.f - scale);
            gui_text_color_set(text, gui_rgb(155 + val, 155 + val, 155 + val));
            gui_img_a8_mix_alpha(icon, val);
        }
        gui_img_scale(bg, scale, scale);
    }
    else if (ota_val >= 100)
    {
        f_status.ota = OTA_DONE;
        GUI_BASE(text)->timer->interval_ms = 10;
        gui_img_set_image_data(bg, OTA_BG_BIN);
    }
    else
    {
        ota_val++; // test
        sprintf(ota_str, "%d%%", ota_val);
        gui_text_content_set(text, ota_str, strlen(ota_str));

        void *bg_data = bg->data;
        if (bg_data == OTA_BG_BIN)
        {
            if (positive)
            {
                bg_data = OTA_BG_BIG_BIN;
                positive = 0;
            }
            else
            {
                bg_data = OTA_BG_SMALL_BIN;
                positive = 1;
            }
        }
        else
        {
            bg_data = OTA_BG_BIN;
        }
        gui_img_set_image_data(bg, bg_data);
    }
}

static void entrance_animation(void *p)
{
    GUI_UNUSED(p);
    if (gui_view_get_current() != current_view) { return; }
    animation_cnt++;
    uint8_t cnt_max_0 = 15;
    uint8_t cnt_max_1 = 30;
    gui_img_t *icon = (gui_img_t *)p;

    if (animation_cnt <= cnt_max_0) //scale
    {
        float scale = ICON_TARGET_SCALE + ICON_TARGET_SCALE * (cnt_max_0 - animation_cnt) / (cnt_max_0);
        gui_img_scale(icon, scale, scale);
    }
    else if (animation_cnt == cnt_max_0 + 1)
    {
        gui_obj_move((void *)icon, 138, 60);
        gui_img_scale(icon, 1.f, 1.f);
        gui_img_set_focus(icon, 0, 0);
        gui_img_set_image_data(icon, ICON_OTA_SMALL_BIN);
        gui_img_refresh_size(icon);

        sprintf(ota_str, "%d%%", ota_val);
        gui_text_t *text = gui_text_create(GUI_BASE(p)->parent, 0, 0, 107, 320, 28);
        gui_text_set(text, ota_str, GUI_FONT_SRC_BMP, gui_rgb(150, 150, 150), strlen(ota_str), 28);
        gui_text_type_set(text, CAPTION_2_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, CENTER);
    }
    else if (animation_cnt <= cnt_max_1) //wait
    {
        gui_text_t *text = (void *)gui_list_entry(GUI_BASE(icon)->parent->child_list.prev, gui_obj_t,
                                                  brother_list);
        uint8_t val = 150 + 105 * (animation_cnt - cnt_max_0) / (cnt_max_1 - cnt_max_0);
        gui_text_color_set(text, gui_rgb(val, val, val));
    }
    else
    {
        gui_obj_t *text = gui_list_entry(GUI_BASE(icon)->parent->child_list.prev, gui_obj_t, brother_list);
        gui_obj_create_timer(GUI_BASE(text), 80, true, ota_cb);
        gui_obj_stop_timer(GUI_BASE(icon));
        animation_cnt = 0;
    }
}

static void entrance_wait(void *p)
{
    GUI_UNUSED(p);
    if (gui_view_get_current() != current_view) { return; }
    gui_obj_stop_timer(p);
    gui_obj_t *bg = p;
    gui_img_set_mode((void *)bg, IMG_BYPASS_MODE);
    gui_obj_t *icon = gui_list_entry(GUI_BASE(bg)->brother_list.next, gui_obj_t, brother_list);
    gui_obj_create_timer(GUI_BASE(icon), 10, true, entrance_animation);
}

static void click_2_make_ota_fail(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    if (f_status.ota == OTA_DOING)
    {
        f_status.ota = OTA_FAIL;
        msg_2_regenerate_current_view();
    }
}

static void click_button_retry(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);

    // to do: OTA retry
    ota_val = 0;
    f_status.ota = OTA_DOING;
    msg_2_regenerate_current_view();
}

static void ota_view_design(gui_view_t *view)
{
    gui_view_set_animate_step(view, 400);
    gui_obj_add_event_cb(view, click_2_make_ota_fail, GUI_EVENT_TOUCH_CLICKED, NULL); //test

    animation_cnt = 0;
    gui_set_bg_color(SCREEN_BG_DARK);

    gui_obj_t *parent = GUI_BASE(view);
    gui_img_t *bg;
    gui_img_t *icon;
    if (f_status.ota == OTA_DOING)
    {
        bg = gui_img_create_from_mem(parent, 0, OTA_BG_BIN, 160, 100, 0, 0);
        gui_img_set_mode(bg, IMG_COVER_MODE);
        gui_img_set_focus(bg, 105, 100);

        int16_t focus = 40;
        icon = gui_img_create_from_mem(parent, 0, ICON_OTA_BIN, 120 + focus, 60, 0, 0);
        gui_img_a8_recolor(icon, GUI_COLOR_ARGB8888(255, 0xFF, 0xFF, 0xFF));
        gui_img_set_focus(icon, focus, 0);
        // gui_img_set_quality(icon, true);
        gui_obj_create_timer(GUI_BASE(bg), 10, true, entrance_wait);
    }
    else if (f_status.ota == OTA_FAIL)
    {
        bg = gui_img_create_from_mem(parent, 0, OTA_BG_FAIL_BIN, 60, 0, 0, 0);
        icon = gui_img_create_from_mem(parent, 0, ICON_OTA_ERROR_BIN, 138, 60, 0, 0);
        gui_img_a8_recolor(icon, GUI_COLOR_ARGB8888(255, 0xFF, 0xFF, 0xFF));
        gui_img_t *button_bg = gui_img_create_from_mem(parent, 0, BUTTON_BG_ELLIPSE_84_40_BIN, 118, 117, 0,
                                                       0);
        gui_img_a8_recolor(button_bg, JBL_GREY_5.color.argb_full);
        gui_text_t *text = gui_text_create(button_bg, 0, 0, 0, 84, 40);
        gui_text_set(text, "Retry", GUI_FONT_SRC_BMP, FG_1_DARK, 5, 20);
        gui_text_type_set(text, HEADING_1_BIN, FONT_SRC_MEMADDR);
        gui_text_mode_set(text, MID_CENTER);
        gui_obj_add_event_cb(button_bg, click_button_retry, GUI_EVENT_TOUCH_CLICKED, NULL); //test
    }
}