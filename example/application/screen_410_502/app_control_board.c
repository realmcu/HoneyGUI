/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "root_image/ui_resource.h"
#include "gui_img.h"
#include "app_main_watch.h"
#include "gui_canvas_rect.h"
#include "guidef.h"
#include "watch_adapt.h"

/*============================================================================*
 *                           Types
 *============================================================================*/
typedef struct switch_state
{
    uint8_t sw_1 : 1; //BT
    uint8_t sw_2 : 1; //LOCAL_PLAY
    uint8_t sw_3 : 1; //PHONE
    uint8_t sw_4 : 1; //EARPHONE
    uint8_t sw_5 : 1; //MUTE
    uint8_t sw_6 : 1; //NOBOTHER
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

/* update switch display state */
void switch_bt(bool state)
{
    sw_state.sw_1 = state;
    GUI_WIDGET_POINTER_BY_NAME_ROOT(obj, "sw_bt", current_view);
    gui_img_t *img = (gui_img_t *)obj;
    if (sw_state.sw_1)
    {
        gui_img_set_image_data(img, CONTROL_BT_ON_BIN);
    }
    else
    {
        gui_img_set_image_data(img, CONTROL_BT_OFF_BIN);
    }
}

void switch_local_play(bool state)
{
    sw_state.sw_2 = state;
    GUI_WIDGET_POINTER_BY_NAME_ROOT(obj, "sw_local_play", current_view);
    gui_img_t *img = (gui_img_t *)obj;
    if (sw_state.sw_2)
    {
        gui_img_set_image_data(img, CONTROL_PLAY_ON_BIN);
    }
    else
    {
        gui_img_set_image_data(img, CONTROL_PLAY_OFF_BIN);
    }
}

void switch_phone(bool state)
{
    sw_state.sw_3 = state;
    GUI_WIDGET_POINTER_BY_NAME_ROOT(obj, "sw_phone", current_view);
    gui_img_t *img = (gui_img_t *)obj;
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

void switch_earphone(bool state)
{
    sw_state.sw_4 = state;
    GUI_WIDGET_POINTER_BY_NAME_ROOT(obj, "sw_earphone", current_view);
    gui_img_t *img = (gui_img_t *)obj;
    if (sw_state.sw_4)
    {
        gui_img_set_image_data(img, CONTROL_EARPHONE_ON_BIN);
    }
    else
    {
        gui_img_set_image_data(img, CONTROL_EARPHONE_OFF_BIN);
    }
}

void switch_mute(bool state)
{
    sw_state.sw_5 = state;
    GUI_WIDGET_POINTER_BY_NAME_ROOT(obj, "sw_mute", current_view);
    gui_img_t *img = (gui_img_t *)obj;
    if (sw_state.sw_5)
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

void switch_nobother(bool state)
{
    sw_state.sw_6 = state;
    GUI_WIDGET_POINTER_BY_NAME_ROOT(obj, "sw_nobother", current_view);
    gui_img_t *img = (gui_img_t *)obj;
    if (sw_state.sw_6)
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

static void switch_cb(void *obj, gui_event_t e, void *param)
{
    (void)e;
    (void)param;
    gui_img_t *img = (gui_img_t *)obj;
    gui_control_board_t *control_board = gui_get_control_board();
    if (strcmp(img->base.name, "sw_bt") == 0)
    {
        sw_state.sw_1 ^= 1;
        switch_bt(sw_state.sw_1);
        if (control_board && control_board->send_bt_status)
        {
            control_board->send_bt_status(sw_state.sw_1);
        }
    }
    else if (strcmp(img->base.name, "sw_local_play") == 0)
    {
        sw_state.sw_2 ^= 1;
        switch_local_play(sw_state.sw_2);
        if (control_board && control_board->send_local_play_status)
        {
            control_board->send_local_play_status(sw_state.sw_2);
        }
    }
    else if (strcmp(img->base.name, "sw_phone") == 0)
    {
        sw_state.sw_3 ^= 1;
        switch_phone(sw_state.sw_3);
        if (control_board && control_board->send_phone_status)
        {
            control_board->send_phone_status(sw_state.sw_3);
        }
    }
    else if (strcmp(img->base.name, "sw_earphone") == 0)
    {
        sw_state.sw_4 ^= 1;
        switch_earphone(sw_state.sw_4);
        if (control_board && control_board->send_earphone_status)
        {
            control_board->send_earphone_status(sw_state.sw_4);
        }
    }
    else if (strcmp(img->base.name, "sw_mute") == 0)
    {
        sw_state.sw_5 ^= 1;
        switch_mute(sw_state.sw_5);
    }
    else if (strcmp(img->base.name, "sw_nobother") == 0)
    {
        sw_state.sw_6 ^= 1;
        switch_nobother(sw_state.sw_6);
    }
}

static void control_board_design(gui_view_t *view)
{
    gui_view_switch_on_event(view, watchface_view->name, SWITCH_OUT_TO_LEFT_USE_ROTATE,
                             SWITCH_IN_FROM_RIGHT_USE_ROTATE,
                             GUI_EVENT_TOUCH_MOVE_LEFT);
    // draw background
    // gui_canvas_rect_t *canvas_rect = gui_canvas_rect_create(GUI_BASE(view), NULL, 0, 0,
    //                                                         SCREEN_WIDTH, SCREEN_HEIGHT, gui_rgb(0, 0, 0));

    // switch
    gui_img_t *sw_bt = gui_img_create_from_mem(view, "sw_bt", CONTROL_BT_OFF_BIN, 20, 100, 182, 121);
    gui_img_t *sw_local_play = gui_img_create_from_mem(view, "sw_local_play", CONTROL_PLAY_OFF_BIN, 207,
                                                       100, 182,
                                                       121);
    gui_img_t *sw_phone = gui_img_create_from_mem(view, "sw_phone", CONTROL_PHONE_OFF_BIN, 20,
                                                  100 + 125 * 1, 182, 121);
    gui_img_t *sw_earphone = gui_img_create_from_mem(view, "sw_earphone", CONTROL_EARPHONE_OFF_BIN, 207,
                                                     100 + 125 * 1, 182, 121);
    gui_img_t *sw_mute = gui_img_create_from_mem(view, "sw_mute", CONTROL_MUTE_OFF_BIN, 20,
                                                 100 + 125 * 2, 182, 121);
    gui_img_t *sw_nobother = gui_img_create_from_mem(view, "sw_nobother", CONTROL_NOBOTHER_OFF_BIN, 207,
                                                     100 + 125 * 2, 182, 121);
    gui_img_set_quality(sw_bt, true);
    gui_img_set_quality(sw_local_play, true);
    gui_img_set_quality(sw_phone, true);
    gui_img_set_quality(sw_earphone, true);
    gui_img_set_quality(sw_mute, true);
    gui_img_set_quality(sw_nobother, true);
    gui_obj_add_event_cb(sw_bt, (gui_event_cb_t)switch_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_obj_add_event_cb(sw_local_play, (gui_event_cb_t)switch_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_obj_add_event_cb(sw_phone, (gui_event_cb_t)switch_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_obj_add_event_cb(sw_earphone, (gui_event_cb_t)switch_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_obj_add_event_cb(sw_mute, (gui_event_cb_t)switch_cb, GUI_EVENT_TOUCH_CLICKED, NULL);
    gui_obj_add_event_cb(sw_nobother, (gui_event_cb_t)switch_cb, GUI_EVENT_TOUCH_CLICKED, NULL);

    gui_img_t *capsule = gui_img_create_from_mem(view, 0, CONTROL_CAPSULE_BIN, 136, 20, 0, 0);
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
        gui_img_set_image_data(sw_bt, CONTROL_BT_ON_BIN);
    }
    if (sw_state.sw_2)
    {
        gui_img_set_image_data(sw_local_play, CONTROL_PLAY_ON_BIN);
    }
    if (sw_state.sw_3)
    {
        gui_img_set_image_data(sw_phone, CONTROL_PHONE_ON_BIN);
        gui_img_set_image_data(img_capsule_phone, PHONE_ON_ICON_BIN);
    }
    if (sw_state.sw_4)
    {
        gui_img_set_image_data(sw_earphone, CONTROL_EARPHONE_ON_BIN);
    }
    if (sw_state.sw_5)
    {
        gui_img_set_image_data(sw_mute, CONTROL_MUTE_ON_BIN);
        gui_img_set_image_data(img_capsule_mute, MUTE_ON_ICON_BIN);
    }
    if (sw_state.sw_6)
    {
        gui_img_set_image_data(sw_nobother, CONTROL_NOBOTHER_ON_BIN);
        gui_img_set_image_data(img_capsule_nobother, NOBOTHER_ON_ICON_BIN);
    }
}



