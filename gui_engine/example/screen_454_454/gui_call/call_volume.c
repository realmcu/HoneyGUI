#include "root_image/ui_resource.h"
#include "app_gui_main.h"
#include "gui_common.h"
#include "gui_text.h"
#include "gui_scroll_text.h"
#include "gui_switch.h"
#include "gui_app.h"
#include "gui_win.h"
#include "gui_progressbar.h"
#include "gui_interface.h"
#include "gui_curtainview.h"
#include "gui_curtain.h"
#include "call_mgr.h"
#ifndef _WIN32
#include "app_task.h"
#include "app_mmi.h"
#include "app_hfp.h"
#include "app_audio_if.h"
#endif

gui_progressbar_t *progressbar_calling_volume = NULL;
float volume_percentage;
uint8_t gui_curr_volume = 0;

static void switch_call_vol_update_cb(void *obj, uint16_t event)
{
    gui_log("switch_call_vol_update_cb\n");

#ifndef _WIN32
    switch (event)
    {
    case GUI_EVENT_CALL_VOLUME:
        {
            gui_curr_volume = app_audio_get_volume();
            volume_percentage = (float)gui_curr_volume / (float)0xf;
            gui_progressbar_set_percentage(progressbar_calling_volume, volume_percentage);
        }
        break;
    default:
        break;
    }
#endif
}

static void switch_calling_volume_up_touch_cb(void *parent, uint16_t event)
{
    gui_log("switch_call_vol_up_touch_cb\n");

    switch (event)
    {
    case GUI_EVENT_1:
    case GUI_EVENT_2:
        {
#ifndef _WIN32
            T_IO_MSG calling_volume_down_msg;
            calling_volume_down_msg.type = IO_MSG_TYPE_WRISTBNAD;
            calling_volume_down_msg.subtype = IO_MSG_MMI;
            calling_volume_down_msg.u.param = MMI_DEV_SPK_VOL_UP;
            app_send_msg_to_apptask(&calling_volume_down_msg);
#endif
        }
        break;

    default:
        break;
    }
}

static void switch_calling_volume_down_touch_cb(void *parent, uint16_t event)
{
    gui_log("switch_call_vol_down_touch_cb\n");

    switch (event)
    {
    case GUI_EVENT_1:
    case GUI_EVENT_2:
        {
#ifndef _WIN32
            T_IO_MSG calling_volume_down_msg;
            calling_volume_down_msg.type = IO_MSG_TYPE_WRISTBNAD;
            calling_volume_down_msg.subtype = IO_MSG_MMI;
            calling_volume_down_msg.u.param = MMI_DEV_SPK_VOL_DOWN;
            app_send_msg_to_apptask(&calling_volume_down_msg);
#endif
        }
        break;
    default:
        break;
    }
}

void design_curtain_call_vol(void *parent)
{
    gui_curtain_t *curtain_down_call_vol = gui_curtain_create(parent, "curtain_down_call_vol", 0, 0,
                                                              LCD_W, LCD_H,
                                                              CURTAIN_DOWN, 0.5f);
    gui_obj_add_event_cb(curtain_down_call_vol, (gui_event_cb_t)switch_call_vol_update_cb,
                         (gui_event_t)GUI_EVENT_CALL_VOLUME, NULL);
    gui_img_create_from_mem(curtain_down_call_vol, "img_calling_volume_base",
                            WATCH_BASE_GREY_90ALPHA_HALF_BIN, 0, 0, 0, 0);

    gui_img_create_from_mem(curtain_down_call_vol, "img_vol_bar",
                            ICON_CALL_VOLUME_BAR_2_BIN, 117, 67, 0, 0);
    progressbar_calling_volume = gui_progressbar_img_h_create(curtain_down_call_vol,
                                                              ICON_VOLUME_BAR_BLUE_BIN, 117, 67);
#ifndef _WIN32
    gui_curr_volume = app_audio_get_volume();
    volume_percentage = (float)gui_curr_volume / (float)0xf;
    gui_progressbar_set_percentage(progressbar_calling_volume, volume_percentage);
#endif
    //volume button
    gui_switch_t *switch_calling_volume_up = gui_switch_create(curtain_down_call_vol, 373, 42, 48, 48,
                                                               ICON_VOLUME_UP_BIN, ICON_VOLUME_UP_BIN);
    switch_calling_volume_up->off_hl_pic_addr = ICON_VOLUME_UP_TOUCH_BIN;
    switch_calling_volume_up->on_hl_pic_addr = ICON_VOLUME_UP_TOUCH_BIN;
    gui_obj_add_event_cb(switch_calling_volume_up, (gui_event_cb_t)switch_calling_volume_up_touch_cb,
                         GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_calling_volume_up, (gui_event_cb_t)switch_calling_volume_up_touch_cb,
                         GUI_EVENT_2, NULL);

    gui_switch_t *switch_calling_volume_down = gui_switch_create(curtain_down_call_vol, 37, 42, 48, 48,
                                                                 ICON_VOLUME_DOWN_BIN, ICON_VOLUME_DOWN_BIN);
    switch_calling_volume_down->off_hl_pic_addr = ICON_VOLUME_DOWN_TOUCH_BIN;
    switch_calling_volume_down->on_hl_pic_addr = ICON_VOLUME_DOWN_TOUCH_BIN;
    gui_obj_add_event_cb(switch_calling_volume_down,
                         (gui_event_cb_t)switch_calling_volume_down_touch_cb, GUI_EVENT_1, NULL);
    gui_obj_add_event_cb(switch_calling_volume_down,
                         (gui_event_cb_t)switch_calling_volume_down_touch_cb, GUI_EVENT_2, NULL);
}
