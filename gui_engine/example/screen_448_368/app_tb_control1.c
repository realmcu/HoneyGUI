#include "root_image_hongkong/ui_resource.h"
#include <gui_magic_img.h>
#include "gui_switch.h"
void page_tb_control1(void *parent)
{
    //gui_magic_img_create_from_mem(parent, "parent", CONTROLMENU_1_BIN, 0, 0, 0, 0);
    gui_switch_t *sw_vibrate     = gui_switch_create(parent, 10, 108, 169, 98, VIBRATE_OFF_BIN,
                                                     VIBRATE_ON_BIN);
    gui_switch_t *sw_ring        = gui_switch_create(parent, 190, 108, 169, 98, RING_OFF_BIN,
                                                     RING_ON_BIN);
    gui_switch_t *sw_emotion     = gui_switch_create(parent, 10, 220, 169, 98, EMOTION_OFF_BIN,
                                                     EMOTION_ON_BIN);
    gui_switch_t *sw_flashlight  = gui_switch_create(parent, 190, 220, 169, 98, FLASHLIGHT_OFF_BIN,
                                                     FLASHLIGHT_ON_BIN);
    gui_switch_t *sw_signal      = gui_switch_create(parent, 10, 332, 169, 98, SIGNAL_OFF_BIN,
                                                     SIGNAL_ON_BIN);
    gui_switch_t *sw_alarm_clock = gui_switch_create(parent, 190, 332, 169, 98,
                                                     ALARM_CLOCK_OFF_BIN, ALARM_CLOCK_ON_BIN);
}
