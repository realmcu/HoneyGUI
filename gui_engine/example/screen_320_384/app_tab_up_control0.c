#include "root_image_hongkong_bee4/ui_resource.h"
#include <gui_magic_img.h>
#include "gui_switch.h"
#include "gui_curtain.h"
#include "gui_img_with_animate.h"
static gui_img_with_animate_t *img;
static void img_animate(gui_img_with_animate_t *img)
{
    gui_log("%f\n", img->animate->progress_percent);
    if (img->animate->progress_percent < 0.5f)
    {
        GET_BASE(img)->y = img->animate->progress_percent * 2 * 100 - 100;
    }
    else if (img->animate->progress_percent >= 0.5f)
    {
        GET_BASE(img)->y = (1 - img->animate->progress_percent) * 2 * 100 - 100;
    }
    if (img->animate->progress_percent == 1.0f)
    {
        img->animate->current_repeat_count = 0;
        img->base.base.not_show = true;
    }

}
static void reset_animate()
{
    img->animate->animate = true;
    img->base.base.not_show = false;
    img->animate->current_frame = 0;
    img->animate->current_repeat_count = 0;
    img->animate->progress_percent = 0;
}
#if 0
static void callback_BT_on()
{
    reset_animate();
    img->base.draw_img.data = WURAOKAI_BIN;
}
static void callback_BT_off()
{
    reset_animate();
    img->base.draw_img.data = WURAOGUAN_BIN;
}
static void callback_charge_on()
{
    reset_animate();
    img->base.draw_img.data = JINGYINKAI_BIN;
}
static void callback_charge_off()
{
    reset_animate();
    img->base.draw_img.data = JINGYINGUAN_BIN;
}

static void callback_QR_on()
{
    reset_animate();
    img->base.draw_img.data = SHIZHONGKAI_BIN;
}
static void callback_QR_off()
{
    reset_animate();
    img->base.draw_img.data = SHIZHONGGUAN_BIN;
}

#else
static void callback_headset_on()
{
    reset_animate();
    img->base.draw_img.data = TEXT_HEADSET_CONNECT_BIN;
}
static void callback_headset_off()
{
    reset_animate();
    img->base.draw_img.data = TEXT_HEADSET_DISCONNECT_BIN;
}
static void callback_disturb_on()
{
    reset_animate();
    img->base.draw_img.data = TEXT_DISTURB_ON_BIN;
}
static void callback_disturb_off()
{
    reset_animate();
    img->base.draw_img.data = TEXT_DISTURB_OFF_BIN;
}
static void callback_bright_on()
{
    reset_animate();
    img->base.draw_img.data = TEXT_BRIGHT_SCREEN_ON_BIN;
}
static void callback_bright_off()
{
    reset_animate();
    img->base.draw_img.data = TEXT_BRIGHT_SCREEN_OFF_BIN;
}
#endif
void page_tb_up_control0(void *parent)
{

    gui_magic_img_create_from_mem(parent, "decoration0", APP_LOGO_BIN, 128, 24, 0, 0);
    gui_switch_t *sw_BT  = gui_switch_create(parent, 16, 90, 169, 98, APP_BT_BIN,
                                             APP_BT_BIN);
    gui_switch_t *sw_charge        = gui_switch_create(parent, 164, 90, 169, 98, APP_CHARGE_BIN,
                                                       APP_CHARGE_BIN);
    gui_switch_t *sw_headset        = gui_switch_create(parent, 16, 185, 169, 98, APP_HEADSET_BIN,
                                                        APP_HEADSET0_BIN);
    gui_switch_t *sw_disturb      = gui_switch_create(parent, 164, 185, 169, 98, APP_DISTURB_BIN,
                                                      APP_DISTURB0_BIN);
    gui_switch_t *sw_QR       = gui_switch_create(parent, 16, 280, 169, 98, APP_QD_BIN,
                                                  APP_QD_BIN);
    gui_switch_t *sw_bright         = gui_switch_create(parent, 164, 280, 169, 98,
                                                        APP_BRIGHT_SCREEN_BIN,
                                                        APP_BRIGHT_SCREEN0_BIN);
    img =  gui_img_with_animate_create(GET_BASE(parent)->parent, TEXT_DISTURB_ON_BIN, 0, 0);
    gui_img_with_animate_set_animate(img, 1000 * 10, 1, img_animate, img);
    img->animate->animate = false;
    img->base.base.not_show = true;
    //gui_obj_add_event_cb(sw_BT, (gui_event_cb_t)callback_BT_on, GUI_EVENT_1, NULL);
    //gui_obj_add_event_cb(sw_BT, (gui_event_cb_t)callback_BT_off, GUI_EVENT_2, NULL);
    //gui_obj_add_event_cb(sw_charge, (gui_event_cb_t)callback_charge_on, GUI_EVENT_1, NULL);
    //gui_obj_add_event_cb(sw_charge, (gui_event_cb_t)callback_charge_off, GUI_EVENT_2, NULL);
    gui_obj_add_event_cb(sw_headset, (gui_event_cb_t)callback_headset_on, GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(sw_headset, (gui_event_cb_t)callback_headset_off, GUI_EVENT_2, NULL);
    gui_obj_add_event_cb(sw_disturb, (gui_event_cb_t)callback_disturb_on, GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(sw_disturb, (gui_event_cb_t)callback_disturb_off, GUI_EVENT_2, NULL);
    //gui_obj_add_event_cb(sw_QR, (gui_event_cb_t)callback_QR_on, GUI_EVENT_1, NULL);
    //gui_obj_add_event_cb(sw_QR, (gui_event_cb_t)callback_QR_off, GUI_EVENT_2, NULL);
    gui_obj_add_event_cb(sw_bright, (gui_event_cb_t)callback_bright_on, GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(sw_bright, (gui_event_cb_t)callback_bright_off, GUI_EVENT_2, NULL);
}


