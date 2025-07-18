/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "root_image/ui_resource.h"
#include "gui_img.h"
#include "app_main_watch.h"
#include "gui_canvas_round_rect.h"
#include "gui_canvas_rect.h"
#include "guidef.h"

/*============================================================================*
 *                           Types
 *============================================================================*/
typedef struct switch_state
{
    uint8_t sw_1 : 1; //LTE
    uint8_t sw_2 : 1; //WIFI
    uint8_t sw_3 : 1; //phone
    uint8_t sw_4 : 1; //mute
    uint8_t sw_5 : 1; //nobother
} switch_state_t;

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define SCREEN_WIDTH (int16_t)gui_get_screen_width()
#define SCREEN_HEIGHT (int16_t)gui_get_screen_height()
#define CURRENT_VIEW_NAME "app_control_view"

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/
static void control_board_design(gui_view_t *view);

/*============================================================================*
 *                            Variables
 *============================================================================*/
static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *watchface_view = NULL;
static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = control_board_design,
};

static switch_state_t sw_state = {0};
static gui_img_t *img_capsule_phone;
static gui_img_t *img_capsule_nobother;
static gui_img_t *img_capsule_mute;

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
    watchface_view = gui_view_descriptor_get("watchface_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

static void switch_cb(void *obj, gui_event_t e, void *param)
{
    gui_img_t *img = (gui_img_t *)obj;
    if (strcmp(img->base.name, "sw_lte") == 0)
    {
        sw_state.sw_1 = !sw_state.sw_1;
        if (sw_state.sw_1)
        {
            gui_img_set_image_data(img, CONTROL_LTE_ON_BIN);
        }
        else
        {
            gui_img_set_image_data(img, CONTROL_LTE_OFF_BIN);
        }
    }
    else if (strcmp(img->base.name, "sw_wifi") == 0)
    {
        sw_state.sw_2 = !sw_state.sw_2;
        if (sw_state.sw_2)
        {
            gui_img_set_image_data(img, CONTROL_WIFI_ON_BIN);
        }
        else
        {
            gui_img_set_image_data(img, CONTROL_WIFI_OFF_BIN);
        }
    }
    else if (strcmp(img->base.name, "sw_phone") == 0)
    {
        sw_state.sw_3 = !sw_state.sw_3;
        if (sw_state.sw_3)
        {
            gui_img_set_image_data(img, CONTROL_PHONE_ON_BIN);
            gui_img_set_image_data(img_capsule_phone, PHONE_ON_ICON_BIN);
        }
        else
        {
            gui_img_set_image_data(img, CONTROL_PHONE_OFF_BIN);
            gui_img_set_image_data(img_capsule_phone, PHONE_OFF_ICON_BIN);
        }
    }
    else if (strcmp(img->base.name, "sw_mute") == 0)
    {
        sw_state.sw_4 = !sw_state.sw_4;
        if (sw_state.sw_4)
        {
            gui_img_set_image_data(img, CONTROL_MUTE_ON_BIN);
            gui_img_set_image_data(img_capsule_mute, MUTE_ON_ICON_BIN);
        }
        else
        {
            gui_img_set_image_data(img, CONTROL_MUTE_OFF_BIN);
            gui_img_set_image_data(img_capsule_mute, MUTE_OFF_ICON_BIN);
        }
    }
    else if (strcmp(img->base.name, "sw_nobother") == 0)
    {
        sw_state.sw_5 = !sw_state.sw_5;
        if (sw_state.sw_5)
        {
            gui_img_set_image_data(img, CONTROL_NOBOTHER_ON_BIN);
            gui_img_set_image_data(img_capsule_nobother, NOBOTHER_ON_ICON_BIN);
        }
        else
        {
            gui_img_set_image_data(img, CONTROL_NOBOTHER_OFF_BIN);
            gui_img_set_image_data(img_capsule_nobother, NOBOTHER_OFF_ICON_BIN);
        }
    }
}

static void control_board_design(gui_view_t *view)
{
    gui_view_switch_on_event(view, watchface_view, SWITCH_OUT_TO_LEFT_USE_ROTATE,
                             SWITCH_IN_FROM_RIGHT_USE_ROTATE,
                             GUI_EVENT_TOUCH_MOVE_LEFT);
    // draw background
    gui_canvas_rect_t *canvas_rect = gui_canvas_rect_create(GUI_BASE(view), NULL, 0, 0,
                                                            SCREEN_WIDTH, SCREEN_HEIGHT, gui_rgb(0, 0, 0));

    // switch
    gui_img_t *sw_lte = gui_img_create_from_mem(view, "sw_lte", CONTROL_LTE_OFF_BIN, 20, 100, 182, 121);
    gui_img_t *sw_wifi = gui_img_create_from_mem(view, "sw_wifi", CONTROL_WIFI_OFF_BIN, 207, 100, 182,
                                                 121);
    gui_img_t *sw_phone = gui_img_create_from_mem(view, "sw_phone", CONTROL_PHONE_OFF_BIN, 20,
                                                  100 + 125 * 1, 182, 121);
    gui_img_t *sw_mute = gui_img_create_from_mem(view, "sw_mute", CONTROL_MUTE_OFF_BIN, 20,
                                                 100 + 125 * 2, 182, 121);
    gui_img_t *sw_nobother = gui_img_create_from_mem(view, "sw_nobother", CONTROL_NOBOTHER_OFF_BIN, 207,
                                                     100 + 125 * 2, 182, 121);
    gui_img_set_quality(sw_lte, true);
    gui_img_set_quality(sw_wifi, true);
    gui_img_set_quality(sw_phone, true);
    gui_img_set_quality(sw_mute, true);
    gui_img_set_quality(sw_nobother, true);
    gui_obj_add_event_cb(sw_lte, (gui_event_cb_t)switch_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_obj_add_event_cb(sw_wifi, (gui_event_cb_t)switch_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_obj_add_event_cb(sw_phone, (gui_event_cb_t)switch_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_obj_add_event_cb(sw_mute, (gui_event_cb_t)switch_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_obj_add_event_cb(sw_nobother, (gui_event_cb_t)switch_cb, GUI_EVENT_TOUCH_CLICKED, NULL);

    // capsule on top
    gui_canvas_round_rect_t *capsule = gui_canvas_round_rect_create(GUI_BASE(view), NULL,
                                                                    136, 20, 138, 47, 20, gui_rgb(196, 196, 196));
    img_capsule_phone = gui_img_create_from_mem(capsule, "capsule_phone", PHONE_OFF_ICON_BIN, 15, 9, 0,
                                                0);
    gui_img_set_mode(img_capsule_phone, IMG_SRC_OVER_MODE);
    gui_img_set_quality(img_capsule_phone, true);
    img_capsule_nobother = gui_img_create_from_mem(capsule, "capsule_nobother", NOBOTHER_OFF_ICON_BIN,
                                                   56, 9, 0,
                                                   0);
    gui_img_set_mode(img_capsule_nobother, IMG_SRC_OVER_MODE);
    gui_img_set_quality(img_capsule_nobother, true);
    img_capsule_mute = gui_img_create_from_mem(capsule, "capsule_mute", MUTE_OFF_ICON_BIN, 98, 9, 0, 0);
    gui_img_set_mode(img_capsule_mute, IMG_SRC_OVER_MODE);
    gui_img_set_quality(img_capsule_mute, true);

    if (sw_state.sw_1)
    {
        gui_img_set_image_data(sw_lte, CONTROL_LTE_ON_BIN);
    }
    if (sw_state.sw_2)
    {
        gui_img_set_image_data(sw_wifi, CONTROL_WIFI_ON_BIN);
    }
    if (sw_state.sw_3)
    {
        gui_img_set_image_data(sw_phone, CONTROL_PHONE_ON_BIN);
        gui_img_set_image_data(img_capsule_phone, PHONE_ON_ICON_BIN);
    }
    if (sw_state.sw_4)
    {
        gui_img_set_image_data(sw_mute, CONTROL_MUTE_ON_BIN);
        gui_img_set_image_data(img_capsule_mute, MUTE_ON_ICON_BIN);
    }
    if (sw_state.sw_5)
    {
        gui_img_set_image_data(sw_nobother, CONTROL_NOBOTHER_ON_BIN);
        gui_img_set_image_data(img_capsule_nobother, NOBOTHER_ON_ICON_BIN);
    }

    gui_img_t *img_charge = gui_img_create_from_mem(view, NULL, CONTROL_PAD_BIN, 207,
                                                    100 + 125 * 1, 182, 121);
    gui_img_t *img_num = gui_img_create_from_mem(img_charge, NULL, UI_TEXT_9_BIN, 35, 40, 0, 0);
    gui_img_scale(img_num, 0.8, 0.8);
    gui_img_set_mode(img_num, IMG_SRC_OVER_MODE);
    gui_img_set_quality(img_num, true);
    img_num = gui_img_create_from_mem(img_charge, NULL, UI_TEXT_6_BIN, 65, 40, 0, 0);
    gui_img_scale(img_num, 0.8, 0.8);
    gui_img_set_mode(img_num, IMG_SRC_OVER_MODE);
    gui_img_set_quality(img_num, true);
    img_num = gui_img_create_from_mem(img_charge, NULL, UI_TEXT_PERCENT_BIN, 95, 40, 0, 0);
    gui_img_scale(img_num, 0.8, 0.8);
    gui_img_set_mode(img_num, IMG_SRC_OVER_MODE);
    gui_img_set_quality(img_num, true);
}
