#include "root_image_hongkong/ui_resource.h"
#include <gui_magic_img.h>
#include "gui_switch.h"
#include "gui_tab.h"
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
static void callback_vibrate_on()
{
    reset_animate();
    img->base.draw_img.data = ZHENDONGKAI_BIN;
}
static void callback_vibrate_off()
{
    reset_animate();
    img->base.draw_img.data = ZHENDONGGUAN_BIN;
}
static void callback_ring_on()
{
    reset_animate();
    img->base.draw_img.data = LINGSHENGKAI_BIN;
}
static void callback_ring_off()
{
    reset_animate();
    img->base.draw_img.data = LINGSHENGGUAN_BIN;
}
static void callback_theatre_on()
{
    reset_animate();
    img->base.draw_img.data = JUYUANMOSHIKAI_BIN;
}
static void callback_theatre_off()
{
    reset_animate();
    img->base.draw_img.data = JUYUANMOSHIGUAN_BIN;
}

static void callback_flashlight_on()
{
    reset_animate();
    img->base.draw_img.data = SHOUDIANTONGKAI_BIN;
}
static void callback_flashlight_off()
{
    reset_animate();
    img->base.draw_img.data = SHOUDIANTONGGUAN_BIN;
}
static void callback_signal_on()
{
    reset_animate();
    img->base.draw_img.data = XINHAOKAI_BIN;
}
static void callback_signal_off()
{
    reset_animate();
    img->base.draw_img.data = XINHAOGUAN_BIN;
}
static void callback_alarm_on()
{
    reset_animate();
    img->base.draw_img.data = NAOZHONGKAI_BIN;
}
static void callback_alarm_off()
{
    reset_animate();
    img->base.draw_img.data = NAOZHONGGUAN_BIN;
}
void page_tb_control1(void *parent)
{
    //gui_magic_img_create_from_mem(parent, "parent", CONTROLMENU_1_BIN, 0, 0, 0, 0);
    gui_switch_t *sw_vibrate     = gui_switch_create(parent, 10, 108, 169, 98, VIBRATE_OFF_BIN,
                                                     VIBRATE_ON_BIN);
    gui_switch_t *sw_ring        = gui_switch_create(parent, 190, 108, 169, 98, RING_OFF_BIN,
                                                     RING_ON_BIN);
    gui_switch_t *sw_theatre     = gui_switch_create(parent, 10, 220, 169, 98, THEATRE_OFF_BIN,
                                                     THEATRE_ON_BIN);
    gui_switch_t *sw_flashlight  = gui_switch_create(parent, 190, 220, 169, 98, FLASHLIGHT_OFF_BIN,
                                                     FLASHLIGHT_ON_BIN);
    gui_switch_t *sw_signal      = gui_switch_create(parent, 10, 332, 169, 98, SIGNAL_OFF_BIN,
                                                     SIGNAL_ON_BIN);
    gui_switch_t *sw_alarm_clock = gui_switch_create(parent, 190, 332, 169, 98,
                                                     ALARM_CLOCK_OFF_BIN, ALARM_CLOCK_ON_BIN);

    extern gui_tab_t *tb_control1;
    img =  gui_img_with_animate_create(tb_control1, ZHENDONGKAI_BIN, 0, 0);
    gui_img_with_animate_set_animate(img, 1000, 1, img_animate, img);
    img->animate->animate = false;
    img->base.base.not_show = true;
    gui_obj_add_event_cb(sw_vibrate, (gui_event_cb_t)callback_vibrate_on, GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(sw_vibrate, (gui_event_cb_t)callback_vibrate_off, GUI_EVENT_2, NULL);
    gui_obj_add_event_cb(sw_ring, (gui_event_cb_t)callback_ring_on, GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(sw_ring, (gui_event_cb_t)callback_ring_off, GUI_EVENT_2, NULL);
    gui_obj_add_event_cb(sw_theatre, (gui_event_cb_t)callback_theatre_on, GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(sw_theatre, (gui_event_cb_t)callback_theatre_off, GUI_EVENT_2, NULL);
    gui_obj_add_event_cb(sw_flashlight, (gui_event_cb_t)callback_flashlight_on, GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(sw_flashlight, (gui_event_cb_t)callback_flashlight_off, GUI_EVENT_2, NULL);
    gui_obj_add_event_cb(sw_signal, (gui_event_cb_t)callback_signal_on, GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(sw_signal, (gui_event_cb_t)callback_signal_off, GUI_EVENT_2, NULL);
    gui_obj_add_event_cb(sw_alarm_clock, (gui_event_cb_t)callback_alarm_on, GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(sw_alarm_clock, (gui_event_cb_t)callback_alarm_off, GUI_EVENT_2, NULL);
}
