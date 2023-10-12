#include "root_image_hongkong_bee4/ui_resource.h"
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
#if 1
static void callback_brightness_on()
{
    reset_animate();
    img->base.draw_img.data = TEXT_BRIGHT_ADJUST_BIN;
}
static void callback_brightness_off()
{
    reset_animate();
    img->base.draw_img.data = TEXT_BRIGHT_ADJUST_BIN;
}
static void callback_alarm_on()
{
    reset_animate();
    img->base.draw_img.data = TEXT_ALARM_ON_BIN;
}
static void callback_alarm_off()
{
    reset_animate();
    img->base.draw_img.data = TEXT_ALARM_OFF_BIN;
}

#else
static void callback_sound_on()
{
    reset_animate();
    img->base.draw_img.data = LINGSHENGKAI_BIN;
}
static void callback_sound_off()
{
    reset_animate();
    img->base.draw_img.data = LINGSHENGGUAN_BIN;
}
static void callback_set_on()
{
    reset_animate();
    img->base.draw_img.data = JUYUANMOSHIKAI_BIN;
}
static void callback_set_off()
{
    reset_animate();
    img->base.draw_img.data = JUYUANMOSHIGUAN_BIN;
}

static void callback_lock_on()
{
    reset_animate();
    img->base.draw_img.data = SHOUDIANTONGKAI_BIN;
}
static void callback_lock_off()
{
    reset_animate();
    img->base.draw_img.data = SHOUDIANTONGGUAN_BIN;
}
static void callback_find_off()
{
    reset_animate();
    img->base.draw_img.data = NAOZHONGGUAN_BIN;
}
static void callback_find_on()
{
    reset_animate();
    img->base.draw_img.data = NAOZHONGKAI_BIN;
}
#endif
void page_tb_up_control1(void *parent)
{
    gui_img_t *logo = gui_img_create_from_mem(parent, "decoration0", APP_LOGO_BIN, 128, 24, 0, 0);
    gui_switch_t *sw_brightness     = gui_switch_create(parent, 16, 90, 169, 98, APP_BRIGHT_BIN,
                                                        APP_BRIGHT_ALL_BIN);
    gui_switch_t *sw_sound        = gui_switch_create(parent, 164, 90, 169, 98, APP_SOUND_BIN,
                                                      APP_SOUND_BIN);
    gui_switch_t *sw_set     = gui_switch_create(parent, 16, 185, 169, 98, APP_SET_BIN,
                                                 APP_SET_BIN);
    gui_switch_t *sw_lock  = gui_switch_create(parent, 164, 185, 169, 98, APP_LOCK_BIN,
                                               APP_LOCK_BIN);
    gui_switch_t *sw_alarm      = gui_switch_create(parent, 16, 280, 169, 98, APP_ALARM_BIN,
                                                    APP_ALARM0_BIN);
    gui_switch_t *sw_find = gui_switch_create(parent, 164, 280, 169, 98,
                                              APP_FIND_BIN, APP_FIND_BIN);

    img =  gui_img_with_animate_create(parent, TEXT_BRIGHT_ADJUST_BIN, 0, 0);
    gui_img_with_animate_set_animate(img, 1000, 1, img_animate, img);
    img->animate->animate = false;
    img->base.base.not_show = true;
    gui_obj_add_event_cb(sw_brightness, (gui_event_cb_t)callback_brightness_on, GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(sw_brightness, (gui_event_cb_t)callback_brightness_off, GUI_EVENT_2, NULL);
    //gui_obj_add_event_cb(sw_sound, (gui_event_cb_t)callback_sound_on, GUI_EVENT_1, NULL);
    //gui_obj_add_event_cb(sw_sound, (gui_event_cb_t)callback_sound_off, GUI_EVENT_2, NULL);
    //gui_obj_add_event_cb(sw_set, (gui_event_cb_t)callback_set_on, GUI_EVENT_1, NULL);
    //gui_obj_add_event_cb(sw_set, (gui_event_cb_t)callback_set_off, GUI_EVENT_2, NULL);
    //gui_obj_add_event_cb(sw_lock, (gui_event_cb_t)callback_lock_on, GUI_EVENT_1, NULL);
    //gui_obj_add_event_cb(sw_lock, (gui_event_cb_t)callback_lock_off, GUI_EVENT_2, NULL);
    gui_obj_add_event_cb(sw_alarm, (gui_event_cb_t)callback_alarm_on, GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(sw_alarm, (gui_event_cb_t)callback_alarm_off, GUI_EVENT_2, NULL);
    //gui_obj_add_event_cb(sw_find, (gui_event_cb_t)callback_find_on, GUI_EVENT_1, NULL);
    //gui_obj_add_event_cb(sw_find, (gui_event_cb_t)callback_find_off, GUI_EVENT_2, NULL);
}
