/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <stdio.h>
#include "gui_win.h"
#include "root_image/ui_resource.h"
#include "gui_components_init.h"
#include "gui_view.h"
#include "gui_img.h"
/*============================================================================*
 *                           Types
 *============================================================================*/


/*============================================================================*
 *                            Macros
 *============================================================================*/


/*============================================================================*
 *                           Function Declaration
 *============================================================================*/


/*============================================================================*
 *                            Variables
 *============================================================================*/
#ifdef _WIN32
unsigned char *resource_root = NULL;
#endif

/*============================================================================*
 *                           Private Functions
 *============================================================================*/

#define SWITCH_DUR_MS (600)
#define WATCHFACE_DUR_MS (5000)
#define CARD_MARGIN (20+25)
static  uint32_t switch_array[] =
{
    FRAME_0024_BIN,
    FRAME_0025_BIN,
    FRAME_0026_BIN,
    FRAME_0027_BIN,
    FRAME_0028_BIN,
    FRAME_0029_BIN,
    FRAME_0030_BIN,
    FRAME_0031_BIN,
    FRAME_0032_BIN,
    FRAME_0033_BIN,
    FRAME_0034_BIN,
    FRAME_0035_BIN,
    FRAME_0036_BIN,
    FRAME_0037_BIN,
    FRAME_0038_BIN,
    FRAME_0040_BIN,
    FRAME_0041_BIN,
    FRAME_0043_BIN,
    FRAME_0045_BIN,
    FRAME_0047_BIN,
    FRAME_0053_BIN,
    FRAME_0061_BIN,
    FRAME_0069_BIN,
    FRAME_0077_BIN,
    FRAME_0085_BIN,
    FRAME_0093_BIN,
    FRAME_0097_BIN,
    FRAME_0103_BIN,
};
static  uint32_t watchface_array[] =
{
    WATCHFACE_FRAME_0000_BIN,
    WATCHFACE_FRAME_0001_BIN,
    WATCHFACE_FRAME_0002_BIN,
    WATCHFACE_FRAME_0003_BIN,
    WATCHFACE_FRAME_0004_BIN,
    WATCHFACE_FRAME_0005_BIN,
    WATCHFACE_FRAME_0006_BIN,
    WATCHFACE_FRAME_0007_BIN,
    WATCHFACE_FRAME_0008_BIN,
    WATCHFACE_FRAME_0009_BIN,
    WATCHFACE_FRAME_0010_BIN,
    WATCHFACE_FRAME_0011_BIN,
    WATCHFACE_FRAME_0012_BIN,
    WATCHFACE_FRAME_0013_BIN,
    WATCHFACE_FRAME_0014_BIN,
    WATCHFACE_FRAME_0015_BIN,
    WATCHFACE_FRAME_0016_BIN,
    WATCHFACE_FRAME_0017_BIN,
    WATCHFACE_FRAME_0018_BIN,
    WATCHFACE_FRAME_0019_BIN,
    WATCHFACE_FRAME_0020_BIN,
    WATCHFACE_FRAME_0021_BIN,
    WATCHFACE_FRAME_0022_BIN,
    WATCHFACE_FRAME_0023_BIN,
    WATCHFACE_FRAME_0024_BIN,
    WATCHFACE_FRAME_0025_BIN,
    WATCHFACE_FRAME_0026_BIN,
    WATCHFACE_FRAME_0027_BIN,
    WATCHFACE_FRAME_0028_BIN,
    WATCHFACE_FRAME_0029_BIN,
    WATCHFACE_FRAME_0030_BIN,
    WATCHFACE_FRAME_0031_BIN,
    WATCHFACE_FRAME_0032_BIN,
    WATCHFACE_FRAME_0033_BIN,
    WATCHFACE_FRAME_0034_BIN,
    WATCHFACE_FRAME_0035_BIN,
    WATCHFACE_FRAME_0036_BIN,
    WATCHFACE_FRAME_0037_BIN,
    WATCHFACE_FRAME_0038_BIN,
    WATCHFACE_FRAME_0039_BIN,
    WATCHFACE_FRAME_0040_BIN,
    WATCHFACE_FRAME_0041_BIN,
    WATCHFACE_FRAME_0042_BIN,
    WATCHFACE_FRAME_0043_BIN,
    WATCHFACE_FRAME_0044_BIN,
    WATCHFACE_FRAME_0045_BIN,
    WATCHFACE_FRAME_0046_BIN,
    WATCHFACE_FRAME_0047_BIN,
    WATCHFACE_FRAME_0048_BIN,
    WATCHFACE_FRAME_0049_BIN,
    WATCHFACE_FRAME_0050_BIN,
    WATCHFACE_FRAME_0051_BIN,
    WATCHFACE_FRAME_0052_BIN,
    WATCHFACE_FRAME_0053_BIN,
    WATCHFACE_FRAME_0054_BIN,
    WATCHFACE_FRAME_0055_BIN,
    WATCHFACE_FRAME_0056_BIN,
    WATCHFACE_FRAME_0057_BIN,
    WATCHFACE_FRAME_0058_BIN,
    WATCHFACE_FRAME_0059_BIN,
    WATCHFACE_FRAME_0060_BIN,
    WATCHFACE_FRAME_0061_BIN,
    WATCHFACE_FRAME_0062_BIN,
    WATCHFACE_FRAME_0063_BIN,
    WATCHFACE_FRAME_0064_BIN,
    WATCHFACE_FRAME_0065_BIN,
    WATCHFACE_FRAME_0066_BIN,
    WATCHFACE_FRAME_0067_BIN,
    WATCHFACE_FRAME_0068_BIN,
    WATCHFACE_FRAME_0069_BIN,
    WATCHFACE_FRAME_0070_BIN,
    WATCHFACE_FRAME_0071_BIN,
    WATCHFACE_FRAME_0072_BIN,
    WATCHFACE_FRAME_0073_BIN,
    WATCHFACE_FRAME_0074_BIN,
    WATCHFACE_FRAME_0075_BIN,
    WATCHFACE_FRAME_0076_BIN,
    WATCHFACE_FRAME_0077_BIN,
    WATCHFACE_FRAME_0078_BIN,
    WATCHFACE_FRAME_0079_BIN,
    WATCHFACE_FRAME_0080_BIN,
    WATCHFACE_FRAME_0081_BIN,
    WATCHFACE_FRAME_0082_BIN,
    WATCHFACE_FRAME_0083_BIN,
    WATCHFACE_FRAME_0084_BIN,
    WATCHFACE_FRAME_0085_BIN,
    WATCHFACE_FRAME_0086_BIN,
    WATCHFACE_FRAME_0087_BIN,
    WATCHFACE_FRAME_0088_BIN,
    WATCHFACE_FRAME_0089_BIN,
    WATCHFACE_FRAME_0090_BIN,
    WATCHFACE_FRAME_0091_BIN,
    WATCHFACE_FRAME_0092_BIN,
    WATCHFACE_FRAME_0093_BIN,
    WATCHFACE_FRAME_0094_BIN,
    WATCHFACE_FRAME_0095_BIN,
    WATCHFACE_FRAME_0096_BIN,
    WATCHFACE_FRAME_0097_BIN,
    WATCHFACE_FRAME_0098_BIN,
    WATCHFACE_FRAME_0099_BIN,
    WATCHFACE_FRAME_0100_BIN,
    WATCHFACE_FRAME_0101_BIN,
    WATCHFACE_FRAME_0102_BIN,
    WATCHFACE_FRAME_0103_BIN,
    WATCHFACE_FRAME_0104_BIN,
    WATCHFACE_FRAME_0105_BIN,
    WATCHFACE_FRAME_0106_BIN,
    WATCHFACE_FRAME_0107_BIN,
    WATCHFACE_FRAME_0108_BIN,
    WATCHFACE_FRAME_0109_BIN,
    WATCHFACE_FRAME_0110_BIN,
    WATCHFACE_FRAME_0111_BIN,
    WATCHFACE_FRAME_0112_BIN,
    WATCHFACE_FRAME_0113_BIN,
    WATCHFACE_FRAME_0114_BIN,
    WATCHFACE_FRAME_0115_BIN,
    WATCHFACE_FRAME_0116_BIN,
    WATCHFACE_FRAME_0117_BIN,
    WATCHFACE_FRAME_0118_BIN,
    WATCHFACE_FRAME_0119_BIN,
};
static gui_img_t *mask_top;
static gui_img_t *mask_middle;
static gui_img_t *mask_bottom;
static gui_img_t *mask_top_about;
static gui_img_t *mask_middle_about;
static gui_img_t *mask_bottom_about;
typedef struct button
{
    int y;
    int h;
    char top_bottom_middle; //0:top 1:bottom 2:middle
    bool pressed;
    void *func;
} button_t;
static void view_switch_in(gui_view_t *view);
static void view_switch_in_watchface(gui_view_t *view);
static void view_switch_in_camera(gui_view_t *view);
static void view_switch_in_ota(gui_view_t *view);
static void view_switch_in_about(gui_view_t *view);
static void view_switch_in_keyboard(gui_view_t *view);
static gui_view_t *image_466_466_view = NULL;
static bool password_flag = false;
static gui_view_descriptor_t gui_view_descriptor_image_466_466 =
{
    .name = "image_466_466",
    .pView = &image_466_466_view,
    .on_switch_in = view_switch_in,
};
static gui_view_t *image_466_466_view_watchface = NULL;
static gui_view_descriptor_t gui_view_descriptor_image_466_466_watchface =
{
    .name = "image_466_466_view_watchface",
    .pView = &image_466_466_view_watchface,
    .on_switch_in = view_switch_in_watchface,
};
static gui_view_t *image_466_466_view_keyboard = NULL;
static gui_view_descriptor_t gui_view_descriptor_image_466_466_keyboard =
{
    .name = "image_466_466_view_keyboard",
    .pView = &image_466_466_view_keyboard,
    .on_switch_in = view_switch_in_keyboard,
};
static gui_view_t *image_466_466_view_camera = NULL;
static gui_view_descriptor_t gui_view_descriptor_image_466_466_camera =
{
    .name = "image_466_466_view_camera",
    .pView = &image_466_466_view_camera,
    .on_switch_in = view_switch_in_camera,
};
static gui_view_t *image_466_466_view_ota = NULL;
static gui_view_descriptor_t gui_view_descriptor_image_466_466_ota =
{
    .name = "image_466_466_view_ota",
    .pView = &image_466_466_view_ota,
    .on_switch_in = view_switch_in_ota,
};
static gui_view_t *image_466_466_view_about = NULL;
static gui_view_descriptor_t gui_view_descriptor_image_466_466_about =
{
    .name = "image_466_466_view_about",
    .pView = &image_466_466_view_about,
    .on_switch_in = view_switch_in_about,
};
static void press_setting_cb_key(void *obj, gui_event_t e, void *param);
static void release_setting_cb_key(void *obj, gui_event_t e, void *param);
static void press_setting_cb_enter(void *obj, gui_event_t e, void *param);
static void release_setting_cb_enter(void *obj, gui_event_t e, void *param);
static void press_setting_cb_about(void *obj, gui_event_t e, void *param);
static void release_setting_cb_about(void *obj, gui_event_t e, void *param);
#define WIFI_ENTER_IMAGE_NAME "enter_wifi_image"
static char before_keyboard = 0;
#define BEFORE_KEYBOARD_NONE 0
#define BEFORE_KEYBOARD_CAMERA 1
#define BEFORE_KEYBOARD_OTA 2
static int input_ip4_x = 0;
static gui_img_t *ip4_input_field[12];
static int ip4_input_index = 0;
static uint32_t ip4_input_field_bin[] =
{
    DIGIT_0_BIN,
    DIGIT_1_BIN,
    DIGIT_2_BIN,
    DIGIT_3_BIN,
    DIGIT_4_BIN,
    DIGIT_5_BIN,
    DIGIT_6_BIN,
    DIGIT_7_BIN,
    DIGIT_8_BIN,
    DIGIT_9_BIN,
};
static char ip_string_buffer[16];
static int ip_string_buffer_index = 0;
static void jump_to_camera_view(void)
{
    gui_view_switch_direct(image_466_466_view_keyboard, &gui_view_descriptor_image_466_466_camera,
                           SWITCH_OUT_ANIMATION_MOVE_TO_LEFT, SWITCH_IN_ANIMATION_MOVE_FROM_RIGHT
                          );
}
static void jump_to_ota_view(void)
{
    gui_view_switch_direct(image_466_466_view_keyboard, &gui_view_descriptor_image_466_466_ota,
                           SWITCH_OUT_ANIMATION_MOVE_TO_LEFT, SWITCH_IN_ANIMATION_MOVE_FROM_RIGHT
                          );
}
static void jump_to_about_view(void)
{
    gui_view_switch_direct(image_466_466_view, &gui_view_descriptor_image_466_466_about,
                           SWITCH_OUT_ANIMATION_MOVE_TO_LEFT, SWITCH_IN_ANIMATION_MOVE_FROM_RIGHT
                          );
}
static void jump_to_keyboard_view(void)
{
    gui_view_switch_direct(image_466_466_view, &gui_view_descriptor_image_466_466_keyboard,
                           SWITCH_OUT_ANIMATION_MOVE_TO_LEFT, SWITCH_IN_ANIMATION_MOVE_FROM_RIGHT
                          );
}
static void jump_to_keyboard_view_camera(void)
{
    before_keyboard = BEFORE_KEYBOARD_CAMERA;
    jump_to_keyboard_view();
}
static void jump_to_keyboard_view_ota(void)
{
    before_keyboard = BEFORE_KEYBOARD_OTA;
    jump_to_keyboard_view();
}
static void reset(void)
{
    gui_log("reset called\n");
}
static button_t buttons[] =
{
    {100, 70, 0, 0, jump_to_keyboard_view_camera},
    {170, 70, 2, 0, jump_to_keyboard_view_ota},
    {240, 70, 2, 0, jump_to_about_view},
};
static button_t buttons_about[] =
{
    {100, 70, 0, 0, 0},
    {170, 70, 2, 0, 0},
    {240, 70, 2, 0, 0},
    {310, 70, 1, 0, 0},
};
#ifdef _WIN32
#define FILE_POINTER(ADDRESS) (const uint8_t *)(resource_root + ADDRESS)
#else
#define FILE_POINTER(ADDRESS) (const uint8_t *)(0x704D1400 + ADDRESS)
#endif
#define ARRAY_SIZE(array) (sizeof(array)/sizeof(array[0]))
static bool switch_playing = false;
extern const unsigned char _binary_root_0x00950000_bin_start[];
extern const unsigned char _binary_root_0x00950000_bin_end[];
extern const unsigned char _binary_root_0x00950000_bin_size[];
// Update the watch time and the JSON data
static void time_update_cb(void *param)
{
    GUI_UNUSED(param);
    gui_log("Time update callback triggered.\n");
}
static void view_switch_in(gui_view_t *view);
static void view_switch_in_camera(gui_view_t *view)
{
    gui_view_switch_on_event(view, &gui_view_descriptor_image_466_466,
                             SWITCH_OUT_TO_RIGHT_USE_TRANSLATION,
                             SWITCH_IN_FROM_LEFT_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_RIGHT);
    GUI_UNUSED(view);
    {
        gui_img_create_from_mem(view,
                                0, (void *)FILE_POINTER(CAMERA_MONITOR_WITH_TITLE_BIN), 0, 0, 0, 0);
    }
}
static void view_switch_in_ota(gui_view_t *view)
{
    gui_view_switch_on_event(view, &gui_view_descriptor_image_466_466,
                             SWITCH_OUT_TO_RIGHT_USE_TRANSLATION,
                             SWITCH_IN_FROM_LEFT_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_RIGHT);
    GUI_UNUSED(view);
    {
        gui_img_create_from_mem(view,
                                0, (void *)FILE_POINTER(OTA_UPDATE_OVERLAY_BIN), 0, 0, 0, 0);
    }
}
static void view_switch_in_about(gui_view_t *view)
{
    gui_view_switch_on_event(view, &gui_view_descriptor_image_466_466,
                             SWITCH_OUT_TO_RIGHT_USE_TRANSLATION,
                             SWITCH_IN_FROM_LEFT_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_RIGHT);
    GUI_UNUSED(view);
    {
        gui_img_t *img =  gui_img_create_from_mem(view,
                                                  0, (void *)FILE_POINTER(ABOUT_CARDS_68PX_HEIGHT_BIN), 0, 0, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        {
            gui_img_t *img2 =
                gui_img_create_from_mem(img,
                                        0, (void *)FILE_POINTER(WIFI2_BIN), 75, 124, 0, 0);
            gui_img_set_mode(img2, IMG_SRC_OVER_MODE);
        }
        {
            gui_img_t *img2 =
                gui_img_create_from_mem(img,
                                        0, (void *)FILE_POINTER(RECT7_BIN), 75 + 5, 192, 0, 0);
            gui_img_set_mode(img2, IMG_SRC_OVER_MODE);
        }
        {
            gui_img_t *img2 =
                gui_img_create_from_mem(img,
                                        0, (void *)FILE_POINTER(SQUARE4_BIN), 75 + 1, 258 + 2, 0, 0);
            gui_img_set_mode(img2, IMG_SRC_OVER_MODE);
        }
        {
            gui_img_t *img2 =
                gui_img_create_from_mem(img,
                                        0, (void *)FILE_POINTER(FLOPPY_BIN), 75 + 2, 326 + 2, 0, 0);
            gui_img_set_mode(img2, IMG_SRC_OVER_MODE);
        }

        gui_obj_add_event_cb(img, press_setting_cb_about, GUI_EVENT_TOUCH_PRESSED, NULL);
        gui_obj_add_event_cb(img, release_setting_cb_about, GUI_EVENT_TOUCH_RELEASED, NULL);
        mask_top_about = gui_img_create_from_mem(img,
                                                 0, (void *)FILE_POINTER(HIGHLIGHT_TOP_ROUNDED_BIN), CARD_MARGIN, 0, 0, 0);
        gui_img_set_mode(mask_top_about, IMG_SRC_OVER_MODE);
        gui_obj_hidden((void *)mask_top_about, true);
        mask_middle_about = gui_img_create_from_mem(img,
                                                    0, (void *)FILE_POINTER(HIGHLIGHT_MIDDLE_RECT_BIN), CARD_MARGIN, 100, 0, 0);
        gui_img_set_mode(mask_middle_about, IMG_SRC_OVER_MODE);
        gui_obj_hidden((void *)mask_middle_about, true);
        mask_bottom_about = gui_img_create_from_mem(img,
                                                    0, (void *)FILE_POINTER(HIGHLIGHT_BOTTOM_ROUNDED_BIN), CARD_MARGIN, 200, 0, 0);
        gui_img_set_mode(mask_bottom_about, IMG_SRC_OVER_MODE);
        gui_obj_hidden((void *)mask_bottom_about, true);
    }
}
static  uint32_t key_array[] =
{
    BUTTON_0_BIN,
    BUTTON_1_BIN,
    BUTTON_2_BIN,
    BUTTON_3_BIN,
    BUTTON_4_BIN,
    BUTTON_5_BIN,
    BUTTON_6_BIN,
    BUTTON_7_BIN,
    BUTTON_8_BIN,
    BUTTON_9_BIN,
    BUTTON_BACKSPACE_BIN,
    BUTTON_DOT_BIN,
};
static  uint32_t key_highlight_array[] =
{
    BUTTON_0_HIGHLIGHT_BIN,
    BUTTON_1_HIGHLIGHT_BIN,
    BUTTON_2_HIGHLIGHT_BIN,
    BUTTON_3_HIGHLIGHT_BIN,
    BUTTON_4_HIGHLIGHT_BIN,
    BUTTON_5_HIGHLIGHT_BIN,
    BUTTON_6_HIGHLIGHT_BIN,
    BUTTON_7_HIGHLIGHT_BIN,
    BUTTON_8_HIGHLIGHT_BIN,
    BUTTON_9_HIGHLIGHT_BIN,
    BUTTON_BACKSPACE_HIGHLIGHT_BIN,
    BUTTON_DOT_HIGHLIGHT_BIN,
};
static void view_switch_in_keyboard(gui_view_t *view)
{
    input_ip4_x = 0;
    memset(ip4_input_field, 0, sizeof(ip4_input_field));
    ip4_input_index = 0;
    memset(ip_string_buffer, 0, sizeof(ip_string_buffer));
    ip_string_buffer_index = 0;
    gui_view_switch_on_event(view, &gui_view_descriptor_image_466_466,
                             SWITCH_OUT_TO_RIGHT_USE_TRANSLATION,
                             SWITCH_IN_FROM_LEFT_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_RIGHT);
    GUI_UNUSED(view);
    GUI_UNUSED(key_highlight_array);
    {
        for (size_t i = 0; i < 12; i++)
        {
            int x = i % 3 * 90 + 100;
            int y = i / 3 * 90 + 107;
            gui_img_t *img = gui_img_create_from_mem(view,
                                                     0, (void *)FILE_POINTER(key_array[i]), x, y, 0, 0);
            gui_img_set_mode(img, IMG_SRC_OVER_MODE);
            gui_obj_add_event_cb(img, press_setting_cb_key, GUI_EVENT_TOUCH_PRESSED, (void *)i);
            gui_obj_add_event_cb(img, release_setting_cb_key, GUI_EVENT_TOUCH_RELEASED, (void *)i);
        }

    }
    {
        gui_img_t *img = gui_img_create_from_mem(view,
                                                 0, (void *)FILE_POINTER(IP_INPUT_FIELD_BIN), 50, 50, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
    }
    {
        gui_img_t *img = gui_img_create_from_mem(view,
                                                 WIFI_ENTER_IMAGE_NAME, (void *)FILE_POINTER(ENTER_WIFI_BIN), 50, 0, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
        gui_obj_add_event_cb(img, press_setting_cb_enter, GUI_EVENT_TOUCH_PRESSED, 0);
        gui_obj_add_event_cb(img, release_setting_cb_enter, GUI_EVENT_TOUCH_RELEASED, 0);

    }
}
static void switch_widget_play(void *p)
{
    GUI_UNUSED(switch_array);
    gui_obj_t *obj = (gui_obj_t *)p;
    GUI_UNUSED(obj);
    uint64_t ms_cur = gui_ms_get();
    static uint64_t ms_start  = 0;
    if (switch_playing == true)
    {
        ms_start = ms_cur;
        switch_playing = false;
    }
    uint64_t dur = ms_cur - ms_start;
    int array_size = ARRAY_SIZE(switch_array);
    int index = (array_size - 1) * dur / SWITCH_DUR_MS;

    if (index >= array_size || index < 0 || dur >= SWITCH_DUR_MS)
    {
        gui_obj_delete_timer(obj);
        index = array_size - 1;
        reset();
    }

    {

        gui_img_t *img = (gui_img_t *)obj;
        gui_img_set_image_data(img, (void *)FILE_POINTER(switch_array[index]));
        gui_img_refresh_size(img);
        gui_log("switch index:%d\n", index);


    }
}

static void switch_widget_play_watchface(void *p)
{
    GUI_UNUSED(watchface_array);
    gui_obj_t *obj = (gui_obj_t *)p;
    GUI_UNUSED(obj);
    uint64_t ms_cur = gui_ms_get();
    static uint64_t ms_start  = 0;

    uint64_t dur = ms_cur - ms_start;
    int array_size = ARRAY_SIZE(watchface_array);
    int index = (array_size - 1) * dur / WATCHFACE_DUR_MS;
    index = index % array_size;


    {

        gui_img_t *img = (gui_img_t *)obj;
        gui_img_set_image_data(img, (void *)FILE_POINTER(watchface_array[index]));
        //  gui_img_refresh_size(img);
        gui_log("switch index:%d\n", index);


    }
}
static int setting_y;
static void press_setting_cb_timer(void *p)
{
    gui_obj_t *obj = (gui_obj_t *)p;
    GUI_UNUSED(obj);

    IMPORT_GUI_TOUCHPAD
    if (tp->type != TOUCH_ORIGIN_FROM_Y
        && tp->type != TOUCH_HOLD_Y
        && tp->type != TOUCH_UP_SLIDE
        && tp->type != TOUCH_DOWN_SLIDE
        && tp->type != TOUCH_UP_SLIDE_TWO_PAGE
        && tp->type != TOUCH_DOWN_SLIDE_TWO_PAGE
        && tp->type != TOUCH_INIT
        && tp->type != TOUCH_INVALID)
    {
        gui_obj_stop_timer(obj);
        return;
    }


    // int dx = tp->deltaX;
    int dy = tp->deltaY;
    //gui_log("press_setting_cb_timer dx:%d dy:%d\n",dx,dy);
    int offset_y = setting_y + dy;
    gui_obj_t *o = GUI_BASE(obj);
    if (offset_y > 0)
    {
        offset_y = 0;
    }
    else if (offset_y < -(o->h - (int)gui_get_screen_height()))
    {
        offset_y = -(o->h - (int)gui_get_screen_height());
    }
    gui_obj_move(obj, 0, offset_y);
}
static void release_setting_cb_timer(void *p)
{
    gui_obj_t *obj = (gui_obj_t *)p;
    GUI_UNUSED(obj);
    gui_log("release_setting_cb_timer\n");
}
static void click_switch_widget_cb(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    //gui_obj_t *o = GUI_BASE(obj);
    gui_obj_create_timer(obj, 1, true, switch_widget_play);
    switch_playing = true;
}
static void press_setting_cb(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    gui_obj_create_timer(obj, 1, true, press_setting_cb_timer);
    gui_obj_start_timer(obj);
    gui_obj_t *o = GUI_BASE(obj);
    IMPORT_GUI_TOUCHPAD
    for (size_t i = 0; i < sizeof(buttons) / sizeof(buttons[0]); i++)
    {
        if (-o->y + tp->y >= buttons[i].y &&
            -o->y + tp->y <= buttons[i].y + buttons[i].h)
        {
            gui_log("press button %d\n", i);
            buttons[i].pressed = true;
            if (buttons[i].top_bottom_middle == 0)
            {
                gui_obj_hidden((void *)mask_top, false);
                gui_obj_move((void *)mask_top, CARD_MARGIN,  buttons[i].y);
            }
            else if (buttons[i].top_bottom_middle == 1)
            {
                gui_obj_hidden((void *)mask_bottom, false);
                gui_obj_move((void *)mask_bottom, CARD_MARGIN, buttons[i].y);
            }
            else if (buttons_about[i].top_bottom_middle == 2)
            {
                gui_obj_hidden((void *)mask_middle, false);
                gui_obj_move((void *)mask_middle, CARD_MARGIN,  buttons[i].y);
            }
        }

    }



}
static void press_setting_cb_about(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);

    gui_obj_t *o = GUI_BASE(obj);
    IMPORT_GUI_TOUCHPAD
    for (size_t i = 0; i < sizeof(buttons_about) / sizeof(buttons_about[0]); i++)
    {
        if (-o->y + tp->y >= buttons_about[i].y &&
            -o->y + tp->y <= buttons_about[i].y + buttons_about[i].h)
        {
            gui_log("press button %d\n", i);
            buttons_about[i].pressed = true;

            if (buttons_about[i].top_bottom_middle == 0)
            {
                gui_obj_hidden((void *)mask_top_about, false);
                gui_obj_move((void *)mask_top_about, CARD_MARGIN,  buttons_about[i].y);
            }
            else if (buttons_about[i].top_bottom_middle == 1)
            {
                gui_obj_hidden((void *)mask_bottom_about, false);
                gui_obj_move((void *)mask_bottom_about, CARD_MARGIN, buttons_about[i].y);
            }
            else if (buttons_about[i].top_bottom_middle == 2)
            {
                gui_obj_hidden((void *)mask_middle_about, false);
                gui_obj_move((void *)mask_middle_about, CARD_MARGIN,  buttons_about[i].y);
            }
        }

    }



}
static void press_setting_cb_key(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    // gui_obj_t *o = GUI_BASE(obj);
    int index = (int)(uintptr_t)param;
    gui_img_set_image_data((gui_img_t *)obj,
                           (void *)FILE_POINTER(key_highlight_array[index]));
}
static void press_setting_cb_enter(void *obj, gui_event_t e, void *param)
{
    if (!password_flag)
    {
        return;
    }

    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);

    gui_img_set_image_data((gui_img_t *)obj,
                           (void *)FILE_POINTER(IP_INPUT_FIELD_PRESS_BIN));
}
static void release_setting_cb_enter(void *obj, gui_event_t e, void *param)
{
    if (!password_flag)
    {
        return;
    }
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);

    gui_img_set_image_data((gui_img_t *)obj,
                           (void *)FILE_POINTER(IP_INPUT_FIELD_HL_BIN));
    IMPORT_GUI_TOUCHPAD
    if (_UI_ABS(tp->deltaY) < 5 && _UI_ABS(tp->deltaX) < 5)
    {

        {

            {
                gui_log("click button\n");
                switch (before_keyboard)
                {
                case BEFORE_KEYBOARD_CAMERA:
                    jump_to_camera_view();
                    /* code */
                    break;
                case BEFORE_KEYBOARD_OTA:
                    jump_to_ota_view();
                    /* code */
                    break;

                default:
                    break;
                }
                // if (buttons[i].func)
                // {
                //     void (*func)(void) = buttons[i].func;
                //     func();
                // }

            }
        }

    }
}

char *get_ip4_string(void)
{
    if (ip_string_buffer_index == 0)
    {
        gui_log("invalid ip string length\n");
        return NULL;
    }
    else if (ip_string_buffer_index > 15)
    {
        gui_log("invalid ip string length\n");
        return NULL;
    }

    ip_string_buffer[ip_string_buffer_index] = '\0';
    return ip_string_buffer;
}
#define DIGIT_WIDTH (20)
static void input_ip4(char *digit)
{
    gui_log("input ip4 %s\n", digit);


    // int y = 0;
    uint32_t key_bin = 0;;
    if (strcmp(digit, ".") == 0)
    {
        if (ip4_input_index >= 12)
        {
            return;
        }
        if (ip4_input_index == 0)
        {
            return;
        }
        input_ip4_x += DIGIT_WIDTH;
        ip_string_buffer[ip_string_buffer_index] = '.';
        ip_string_buffer_index ++;
        int count = ip_string_buffer_index - 1;
        while (count > 0)
        {
            if (ip_string_buffer[count - 1] == '.')
            {
                break;
            }
            count --;
        }
        printf("count:%d\n", ip_string_buffer_index - 1 - count);
        int num_digits = ip_string_buffer_index - 1 - count;
        if (num_digits == 1)
        {
            input_ip4_x += DIGIT_WIDTH;
            input_ip4_x += DIGIT_WIDTH;
        }
        else if (num_digits == 2)
        {
            input_ip4_x += DIGIT_WIDTH;
        }
        else if (num_digits == 0)
        {
            input_ip4_x -= DIGIT_WIDTH;
            ip_string_buffer_index --;
            ip_string_buffer[ip_string_buffer_index] = '\0';
        }



    }
    else if (strcmp(digit, "<-") == 0)
    {
        if (ip4_input_index > 0 && ip4_input_index <= 12)
        {

            gui_obj_tree_free(ip4_input_field[ip4_input_index - 1]);
            ip4_input_index --;
            if (ip_string_buffer[ip_string_buffer_index - 1] == '.')
            {
                input_ip4_x -= DIGIT_WIDTH;


                int count = ip_string_buffer_index - 1;
                while (count > 0)
                {
                    if (ip_string_buffer[count - 1] == '.')
                    {
                        break;
                    }
                    count --;
                }
                printf("count:%d\n", ip_string_buffer_index - 1 - count);
                int num_digits = ip_string_buffer_index - 1 - count;
                if (num_digits == 1)
                {
                    input_ip4_x -= DIGIT_WIDTH;
                    input_ip4_x -= DIGIT_WIDTH;
                }
                else if (num_digits == 2)
                {
                    input_ip4_x -= DIGIT_WIDTH;
                }
                // else if (num_digits==0)
                // {
                //     input_ip4_x -=DIGIT_WIDTH;
                //     ip_string_buffer_index --;
                //     ip_string_buffer[ip_string_buffer_index] = '\0';
                // }

                ip_string_buffer_index --;
                ip_string_buffer[ip_string_buffer_index] = '\0';
            }

            ip_string_buffer_index --;
            ip_string_buffer[ip_string_buffer_index] = '\0';

        }
        if (input_ip4_x > 0)
        {
            input_ip4_x -= DIGIT_WIDTH;
        }
        else
        {
            input_ip4_x = 0;
        }


    }
    else
    {
        int num = atoi(digit);
        if (num >= 0 && num <= 9)
        {
            if (ip4_input_index >= 12)
            {
                return;
            }
            key_bin = ip4_input_field_bin[num];
            gui_img_t *img = gui_img_create_from_mem(image_466_466_view_keyboard,
                                                     0, (void *)FILE_POINTER(key_bin), input_ip4_x + 80, 62, 0, 0);
            GUI_UNUSED(img);
            ip4_input_field[ip4_input_index] = img;
            ip4_input_index ++;
            input_ip4_x += DIGIT_WIDTH;
            ip_string_buffer[ip_string_buffer_index] = '0' + num;
            ip_string_buffer_index ++;

        }
        else
        {
            gui_log("invalid digit %s\n", digit);
            return;
        }
    }
    gui_log("ip_string_buffer:%s\n", ip_string_buffer);
    gui_log("ip_string_buffer_index:%d\n", ip_string_buffer_index);


}
static char *ip4_string[] =
{
    "0",
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9", "<-",
    ".",

};
static void release_setting_cb_key(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    // gui_obj_t *o = GUI_BASE(obj);
    int index = (int)(uintptr_t)param;
    gui_img_set_image_data((gui_img_t *)obj,
                           (void *)FILE_POINTER(key_array[index]));
    IMPORT_GUI_TOUCHPAD
    if (_UI_ABS(tp->deltaY) < 5 && _UI_ABS(tp->deltaX) < 5)
    {

        {

            {
                gui_log("click button %d\n", index);
                input_ip4(ip4_string[index]);
                char *ipstring = get_ip4_string();
                if (ipstring)
                {
                    int count = 0;
                    for (size_t i = 0; i < strlen(ipstring); i++)
                    {
                        if (ipstring[i] == '.')
                        {
                            count ++;
                        }
                    }
                    if (count == 3 && ipstring[strlen(ipstring) - 1] != '.')
                    {
                        gui_log("valid ip string:%s\n", ipstring);
                        password_flag = true;
                        gui_img_t *enter_img = NULL;
                        gui_obj_tree_get_widget_by_name(gui_obj_get_root(), WIFI_ENTER_IMAGE_NAME, (void *)&enter_img);
                        if (enter_img)
                        {
                            gui_img_set_image_data(enter_img,
                                                   (void *)FILE_POINTER(IP_INPUT_FIELD_HL_BIN));
                        }
                    }
                }

                // if (buttons[i].func)
                // {
                //     void (*func)(void) = buttons[i].func;
                //     func();
                // }

            }
        }

    }
}
static void release_setting_cb(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    //gui_obj_create_timer(obj, 1, true, release_setting_cb_timer);
    //gui_obj_start_timer(obj);
    gui_obj_t *o = GUI_BASE(obj);
    if (o->timer != NULL)
    {
        gui_obj_stop_timer(obj);
    }

    IMPORT_GUI_TOUCHPAD
//int dx = tp->deltaX;

    int dy = tp->deltaY;
    setting_y += dy;
    if (setting_y > 0)
    {
        setting_y = 0;
    }
    else if (setting_y < -(o->h - (int)gui_get_screen_height()))
    {
        setting_y = -(o->h - (int)gui_get_screen_height());
    }
    gui_obj_hidden((void *)mask_top, true);

    gui_obj_hidden((void *)mask_middle, true);

    gui_obj_hidden((void *)mask_bottom, true);

    if (_UI_ABS(tp->deltaY) < 5 && _UI_ABS(tp->deltaX) < 5)
    {
        for (size_t i = 0; i < sizeof(buttons) / sizeof(buttons[0]); i++)
        {
            if (buttons[i].pressed == true)
            {
                gui_log("click button %d\n", i);
                if (buttons[i].func)
                {
                    void (*func)(void) = buttons[i].func;
                    func();
                }

            }
        }

    }
    for (size_t i = 0; i < sizeof(buttons) / sizeof(buttons[0]); i++)
    {
        buttons[i].pressed = false;
    }

}
static void release_setting_cb_about(void *obj, gui_event_t e, void *param)
{
    GUI_UNUSED(obj);
    GUI_UNUSED(e);
    GUI_UNUSED(param);
    //gui_obj_create_timer(obj, 1, true, release_setting_cb_timer);
    //gui_obj_start_timer(obj);

//int dx = tp->deltaX;


    gui_obj_hidden((void *)mask_top_about, true);

    gui_obj_hidden((void *)mask_middle_about, true);

    gui_obj_hidden((void *)mask_bottom_about, true);



}

static int app_init(void)
{
    gui_set_bg_color(gui_color_css("#f1f2f1"));
#ifdef _WIN32
    resource_root = (unsigned char *)_binary_root_0x00950000_bin_start;
#endif
//     image_466_466_view = gui_view_create(gui_obj_get_root(),
//     &gui_view_descriptor_image_466_466,
// 0,0,0,0);
    {
        gui_win_t *win = gui_win_create(gui_obj_get_root(),
                                        0,
                                        0, 0, 0, 0);
        GUI_UNUSED(gui_view_descriptor_image_466_466);
        GUI_UNUSED(gui_view_descriptor_image_466_466_watchface);
        GUI_UNUSED(gui_view_descriptor_image_466_466_keyboard);
        GUI_UNUSED(gui_view_descriptor_image_466_466_ota);
        GUI_UNUSED(gui_view_descriptor_image_466_466_camera);
        image_466_466_view_watchface = gui_view_create(win,
                                                       &gui_view_descriptor_image_466_466_watchface,
                                                       0, 0, 0, 0);
    }






#ifdef _WIN32
    {
        gui_win_t *win = gui_win_create(gui_obj_get_root(),
                                        0,
                                        0, 0, 0, 0);
        gui_img_t *img =
            gui_img_create_from_mem(win,
                                    0, (void *)FILE_POINTER(WATCH_MASK_466_AA_BIN), 0, 0, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
    }
#endif



    return 0;
}
GUI_INIT_APP_EXPORT(app_init);
static void view_switch_in(gui_view_t *view)
{
    gui_view_switch_on_event(view, &gui_view_descriptor_image_466_466_watchface,
                             SWITCH_OUT_TO_LEFT_USE_TRANSLATION,
                             SWITCH_IN_FROM_RIGHT_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_LEFT);
    GUI_UNUSED(view);
    {
        gui_img_t *img =
            gui_img_create_from_mem(view,
                                    0, (void *)FILE_POINTER(PRIVACY_CARDS_68PX_HEIGHT_BIN), 0, 0, 0, 0);
        gui_img_set_mode(img, IMG_BYPASS_MODE);
        gui_obj_add_event_cb(img, press_setting_cb, GUI_EVENT_TOUCH_PRESSED, NULL);
        gui_obj_add_event_cb(img, release_setting_cb, GUI_EVENT_TOUCH_RELEASED, NULL);
        {
            gui_img_t *switch_widget =
                gui_img_create_from_mem(img,
                                        0, (void *)FILE_POINTER(FRAME_0024_BIN), 300, 310, 0, 0);
            GUI_UNUSED(switch_widget);
            gui_obj_add_event_cb(switch_widget, click_switch_widget_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
        }

        {
            gui_img_t *img2 =
                gui_img_create_from_mem(img,
                                        0, (void *)FILE_POINTER(CAMERA2_BIN), 75, 124, 0, 0);
            gui_img_set_mode(img2, IMG_SRC_OVER_MODE);
        }
        {
            gui_img_t *img2 =
                gui_img_create_from_mem(img,
                                        0, (void *)FILE_POINTER(CLOUDDOWNLOAD1_BIN), 75, 192, 0, 0);
            gui_img_set_mode(img2, IMG_SRC_OVER_MODE);
        }
        {
            gui_img_t *img2 =
                gui_img_create_from_mem(img,
                                        0, (void *)FILE_POINTER(INFO1_BIN), 75, 258 + 2, 0, 0);
            gui_img_set_mode(img2, IMG_SRC_OVER_MODE);
        }
        {
            gui_img_t *img2 =
                gui_img_create_from_mem(img,
                                        0, (void *)FILE_POINTER(RELOAD2_BIN), 75, 326 + 2, 0, 0);
            gui_img_set_mode(img2, IMG_SRC_OVER_MODE);
        }

        mask_top = gui_img_create_from_mem(img,
                                           0, (void *)FILE_POINTER(HIGHLIGHT_TOP_ROUNDED_BIN), CARD_MARGIN, 0, 0, 0);
        gui_img_set_mode(mask_top, IMG_SRC_OVER_MODE);
        gui_obj_hidden((void *)mask_top, true);
        mask_middle = gui_img_create_from_mem(img,
                                              0, (void *)FILE_POINTER(HIGHLIGHT_MIDDLE_RECT_BIN), CARD_MARGIN, 100, 0, 0);
        gui_img_set_mode(mask_middle, IMG_SRC_OVER_MODE);
        gui_obj_hidden((void *)mask_middle, true);
        mask_bottom = gui_img_create_from_mem(img,
                                              0, (void *)FILE_POINTER(HIGHLIGHT_BOTTOM_ROUNDED_BIN), CARD_MARGIN, 200, 0, 0);
        gui_img_set_mode(mask_bottom, IMG_SRC_OVER_MODE);
        gui_obj_hidden((void *)mask_bottom, true);
    }
    {
        gui_img_t *img =
            gui_img_create_from_mem(view,
                                    0, (void *)FILE_POINTER(IOS_NAV_466PX_GRADIENT_BIN), 0, 0, 0, 0);
        gui_img_set_mode(img, IMG_SRC_OVER_MODE);
    }
    setting_y = 0;
}
static void view_switch_in_watchface(gui_view_t *view)
{
    gui_view_switch_on_event(view, &gui_view_descriptor_image_466_466,
                             SWITCH_OUT_TO_RIGHT_USE_TRANSLATION,
                             SWITCH_IN_FROM_LEFT_USE_TRANSLATION,
                             GUI_EVENT_TOUCH_MOVE_RIGHT);
    GUI_UNUSED(view);
    {
        gui_img_t *img =
            gui_img_create_from_mem(view,
                                    0, (void *)FILE_POINTER(WATCHFACE_FRAME_0000_BIN), 0, 0, 0, 0);
        gui_img_set_mode(img, IMG_BYPASS_MODE);
        gui_obj_create_timer((void *)img, 10, true, switch_widget_play_watchface);

    }
}
