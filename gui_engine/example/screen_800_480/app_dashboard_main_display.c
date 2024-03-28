#include <gui_tabview.h>
#include <gui_win.h>
#include <gui_text.h>
#include <gui_curtain.h>
#include "resource_8762g.h"
#include <gui_app.h>
#include "gui_tab.h"
#include "draw_font.h"
#include "acc_engine.h"
#include "nanosvg.h"
#include "gui_img.h"
#include "time.h"
#include "gui_button.h"
#include "app_dashboard_main_display.h"
#include "app_dashboard_data.h"
#include "trace.h"

uint8_t show_message_data[40];
uint8_t show_tel_number[11];

void app_dashboard_create_main_display(gui_win_t *target_main_display)
{
    /* set update callback */
    gui_win_set_animate(target_main_display, 1000, -1, paint_main_display_cb, target_main_display);

    /* set Image data */
    dashboard_background = gui_img_create_from_mem(target_main_display, "dashboard_background",
                                                   BACKGROUND_BIN, 0, 0, 800, 480);
    dashboard_background->draw_img.blend_mode = IMG_FILTER_BLACK;
    speed_high_digital = gui_img_create_from_mem(target_main_display, "speed_high_digital", SPED0_BIN,
                                                 360, 202, 40, 60);
    speed_high_digital->draw_img.blend_mode = IMG_FILTER_BLACK;
    speed_low_digital = gui_img_create_from_mem(target_main_display, "speed_low_digital", SPED0_BIN,
                                                406, 202, 40, 60);
    speed_low_digital->draw_img.blend_mode = IMG_FILTER_BLACK;
    bluetooth_status = gui_img_create_from_mem(target_main_display, "bluetooth_status", BTOF_BIN, 48,
                                               10, 23, 30);
    bluetooth_status->draw_img.blend_mode = IMG_FILTER_BLACK;
    left_turn_light_status = gui_img_create_from_mem(target_main_display, "left_turn_light_status",
                                                     TL_OF_BIN, 48, 208, 42, 40);
    left_turn_light_status->draw_img.blend_mode = IMG_FILTER_BLACK;
    right_turn_light_status = gui_img_create_from_mem(target_main_display, "right_turn_light_status",
                                                      TR_OF_BIN, 710, 208, 42, 40);
    right_turn_light_status->draw_img.blend_mode = IMG_FILTER_BLACK;
    hour_high_digital = gui_img_create_from_mem(target_main_display, "hour_high_digital", TIMER0_BIN,
                                                355, 12, 9, 16);
    hour_high_digital->draw_img.blend_mode = IMG_FILTER_BLACK;
    hour_low_digital = gui_img_create_from_mem(target_main_display, "hour_low_digital", TIMER0_BIN, 366,
                                               12, 9, 16);
    hour_low_digital->draw_img.blend_mode = IMG_FILTER_BLACK;
    min_high_digital = gui_img_create_from_mem(target_main_display, "min_high_digital", TIMER0_BIN, 387,
                                               12, 9, 18);
    min_high_digital->draw_img.blend_mode = IMG_FILTER_BLACK;
    min_low_digital = gui_img_create_from_mem(target_main_display, "min_low_digital", TIMER0_BIN, 399,
                                              12, 9, 18);
    min_low_digital->draw_img.blend_mode = IMG_FILTER_BLACK;
    tense_high_digital = gui_img_create_from_mem(target_main_display, "tense_high_digital", APM_A_BIN,
                                                 417,
                                                 12, 9, 18);
    tense_high_digital->draw_img.blend_mode = IMG_FILTER_BLACK;
    tense_low_digital = gui_img_create_from_mem(target_main_display, "tense_low_digital", APM_M_BIN,
                                                432,
                                                12, 9, 18);
    tense_low_digital->draw_img.blend_mode = IMG_FILTER_BLACK;
    bat_high_digital = gui_img_create_from_mem(target_main_display, "bat_high_digital", TIMER0_BIN, 394,
                                               360, 9, 18);
    bat_high_digital->draw_img.blend_mode = IMG_FILTER_BLACK;
    bat_low_digital = gui_img_create_from_mem(target_main_display, "bat_low_digital", TIMER0_BIN, 406,
                                              360, 9, 18);
    bat_low_digital->draw_img.blend_mode = IMG_FILTER_BLACK;
    tel_box = gui_img_create_from_mem(target_main_display, "tel_box", TELBOX_BIN, 253, 410, 295, 49);
    tel_box->draw_img.blend_mode = IMG_FILTER_BLACK;
    tel_accept = gui_img_create_from_mem(target_main_display, "tel_box", TELBOX_BIN, 253, 410, 295, 49);
    tel_accept->draw_img.blend_mode = IMG_FILTER_BLACK;
    tel_reject_end = gui_img_create_from_mem(target_main_display, "tel_box", TELBOX_BIN, 253, 410, 295,
                                             49);
    tel_reject_end->draw_img.blend_mode = IMG_FILTER_BLACK;
    refuse_button = gui_img_create_from_mem(target_main_display, "refuse_button", REFUS_BIN, 263, 416,
                                            36, 36);
    refuse_button->draw_img.blend_mode = IMG_FILTER_BLACK;
    ans_button = gui_img_create_from_mem(target_main_display, "ans_button", ANS_BIN, 500, 416, 36, 36);
    ans_button->draw_img.blend_mode = IMG_FILTER_BLACK;
    tel_box_left_button = gui_img_create_from_mem(target_main_display, "tel_box_left_button", SYMB1_BIN,
                                                  315, 416, 36, 36);
    tel_box_left_button->draw_img.blend_mode = IMG_FILTER_BLACK;
    tel_box_right_button = gui_img_create_from_mem(target_main_display, "tel_box_right_button",
                                                   SYMB2_BIN, 479, 416, 36, 36);
    tel_box_right_button->draw_img.blend_mode = IMG_FILTER_BLACK;
    dashboard_pointer = gui_img_create_from_mem(target_main_display, "dashboard_Cpointer",
                                                DASHBOARD_2_BIN, 243, 84,
                                                9, 18);
    dashboard_pointer->draw_img.blend_mode = IMG_FILTER_BLACK;
    short_message = gui_img_create_from_mem(target_main_display, "short_message", MESSAGE_BIN, 221,
                                            0, 359, 80);
    short_message->draw_img.blend_mode = IMG_FILTER_BLACK;

    /* set font data */
    app_phone_data current_phone_status;
    app_dashboard_data_get_phone_status(&current_phone_status);
    short_tel_number = gui_text_create(target_main_display,  "short_tel_number",  322, 415, 158, 30);
    memcpy(&show_tel_number[0], &current_phone_status.current_phone_number[0],
           current_phone_status.current_phone_number_len);
    gui_text_set(short_tel_number, (char *)show_tel_number, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX,
                 UINT8_MAX, UINT8_MAX), current_phone_status.current_phone_number_len, 28);
    gui_text_mode_set(short_tel_number, CENTER);
    short_tel_accept = gui_text_create(target_main_display, "short_tel_accept", 360, 415, 800, 30);
    gui_text_set(short_tel_accept, "calling", GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX, UINT8_MAX,
                                                                        UINT8_MAX),
                 7, 32);

    app_message_data current_message_status;
    app_dashboard_data_get_message_data_update(&current_message_status);
    memcpy(&show_message_data[0], &current_message_status.wechat_msg[0],
           current_message_status.wechat_msg_len);
    short_message_data = gui_text_create(target_main_display,  "short_message_data",  300, 10, 240, 50);
    gui_text_set(short_message_data, (char *)show_message_data, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX,
                 UINT8_MAX, UINT8_MAX), (current_message_status.wechat_msg_len - 1), 32);
    gui_text_mode_set(short_message_data, MULTI_LEFT);

    /* Prepare the intial data */
    app_dashboard_update_main_display_time_info();
    app_dashboard_update_main_display_tense_apm_info(app_dashboard_data_get_tense_timer_info());
    app_dashboard_update_main_display_battery_info(app_dashboard_data_get_battery_level());
    app_dashboard_update_main_display_speed_info(app_dashboard_data_get_car_speed());
    app_dashboard_update_main_display_turn_info(app_dashboard_data_get_car_turn_info());
    app_dashboard_update_main_display_bluetooth_info(app_dashboard_data_get_bluetooth_status());
    app_dashboard_update_main_display_phone_infor(&current_phone_status);
    app_dashboard_update_main_display_message_infor(&current_message_status);
}

extern gui_win_t *win_connected_display;
void paint_main_display_cb(gui_win_t *win)
{
    if (app_dashboard_data_get_show_main_display() == false)
    {
        win->base.not_show = !app_dashboard_data_get_show_main_display();
        if (win_connected_display->base.not_show)
        {
            win_connected_display->base.not_show = false;
        }
        return;
    }

    app_phone_data current_phone_status;
    app_dashboard_data_get_phone_status(&current_phone_status);
    app_dashboard_update_main_display_time_info();
    app_dashboard_update_main_display_tense_apm_info(app_dashboard_data_get_tense_timer_info());
    app_dashboard_update_main_display_battery_info(app_dashboard_data_get_battery_level());
    app_dashboard_update_main_display_speed_info(app_dashboard_data_get_car_speed());
    app_dashboard_update_main_display_turn_info(app_dashboard_data_get_car_turn_info());
    app_dashboard_update_main_display_bluetooth_info(app_dashboard_data_get_bluetooth_status());
    app_dashboard_update_main_display_phone_infor(&current_phone_status);
    app_message_data current_message_status;
    app_dashboard_data_get_message_data_update(&current_message_status);
    app_dashboard_update_main_display_message_infor(&current_message_status);
}

void app_dashboard_update_main_display_time_info(void)
{
    uint32_t main_display_hour = app_dashboard_data_get_current_timer() / 3600;
    uint32_t main_display_min = (app_dashboard_data_get_current_timer() - (main_display_hour * 3600)) /
                                60;

    app_update_gui_widget(hour_high_digital,
                          main_display_hour / 10,
                          main_display_time_resource_def,
                          sizeof(main_display_time_resource_def));
    app_update_gui_widget(hour_low_digital,
                          main_display_hour % 10,
                          main_display_time_resource_def,
                          sizeof(main_display_time_resource_def));
    app_update_gui_widget(min_high_digital,
                          main_display_min / 10,
                          main_display_time_resource_def,
                          sizeof(main_display_time_resource_def));
    app_update_gui_widget(min_low_digital,
                          main_display_min % 10,
                          main_display_time_resource_def,
                          sizeof(main_display_time_resource_def));
}

void app_dashboard_update_main_display_tense_apm_info(T_TENSE_APM_INFO current_tense_apm_info)
{
    app_dashboard_refresh_main_display_tense_apm_info(tense_high_digital, tense_low_digital,
                                                      current_tense_apm_info);
}

void app_dashboard_refresh_main_display_tense_apm_info(gui_img_t *tense_high_image_obj,
                                                       gui_img_t *tense_low_image_obj,
                                                       T_TENSE_APM_INFO current_tense_apm_info)
{
    void *tense_high_digital_info = APM_A_BIN;
    void *tense_low_digital_info = APM_M_BIN;

    if (current_tense_apm_info == T_TENSE_AM_DISPLAY)
    {
        tense_high_digital_info = APM_A_BIN;
        tense_low_digital_info = APM_M_BIN;
    }
    else if (current_tense_apm_info == T_TENSE_PM_DISPLAY)
    {
        tense_high_digital_info = APM_P_BIN;
        tense_low_digital_info = APM_M_BIN;
    }

    if (tense_high_digital_info != 0x0 && tense_low_digital_info != NULL)
    {
        gui_img_set_attribute(tense_high_image_obj,
                              tense_high_image_obj->base.name,
                              (void *)tense_high_digital_info,
                              tense_high_image_obj->base.x,
                              tense_high_image_obj->base.y);
    }

    if (tense_high_digital_info != 0x0 && tense_low_digital_info != NULL)
    {
        gui_img_set_attribute(tense_low_image_obj,
                              tense_low_image_obj->base.name,
                              (void *)tense_low_digital_info,
                              tense_low_image_obj->base.x,
                              tense_low_image_obj->base.y);
    }
}

void app_dashboard_update_main_display_battery_info(uint8_t battery_level)
{
    app_update_gui_widget(bat_high_digital,
                          battery_level / 10,
                          main_display_battery_resource_def,
                          sizeof(main_display_battery_resource_def));

    app_update_gui_widget(bat_low_digital,
                          battery_level % 10,
                          main_display_battery_resource_def,
                          sizeof(main_display_battery_resource_def));
}

void app_dashboard_update_main_display_speed_info(uint8_t current_speed)
{
    app_update_gui_widget(speed_high_digital,
                          current_speed / 10,
                          main_display_speed_resource_def,
                          sizeof(main_display_speed_resource_def));

    app_update_gui_widget(speed_low_digital,
                          current_speed % 10,
                          main_display_speed_resource_def,
                          sizeof(main_display_speed_resource_def));

    app_update_gui_widget(dashboard_pointer,
                          current_speed / UPDATE_DASHBOARD_GRANULARITY,
                          dashboard_pointer_num,
                          sizeof(dashboard_pointer_num));
}

void app_dashboard_update_main_display_turn_info(T_TURN_INFO current_turn_information)
{
    app_dashboard_refresh_main_display_turn_info(left_turn_light_status, right_turn_light_status,
                                                 current_turn_information);
}

void app_dashboard_refresh_main_display_turn_info(gui_img_t *left_turn_image_obj,
                                                  gui_img_t *right_turn_image_obj,
                                                  T_TURN_INFO current_turn_information)
{
    void *left_turn_resource_info = TL_OF_BIN;
    void *right_turn_resource_info = TR_OF_BIN;

    if (current_turn_information & T_TURN_INFO_LEFT)
    {
        left_turn_resource_info = TL_ON_BIN;
    }

    if (current_turn_information & T_TURN_INFO_RIGHT)
    {
        right_turn_resource_info = TR_ON_BIN;
    }

    if (left_turn_resource_info != 0x0 && left_turn_image_obj != NULL)
    {
        gui_img_set_attribute(left_turn_image_obj,
                              left_turn_image_obj->base.name,
                              (void *)left_turn_resource_info,
                              left_turn_image_obj->base.x,
                              left_turn_image_obj->base.y);
    }

    if (right_turn_resource_info != 0x0 && right_turn_image_obj != NULL)
    {
        gui_img_set_attribute(right_turn_image_obj,
                              right_turn_image_obj->base.name,
                              (void *)right_turn_resource_info,
                              right_turn_image_obj->base.x,
                              right_turn_image_obj->base.y);
    }
}

void app_dashboard_update_main_display_bluetooth_info(T_BLUETOOTH_INFO current_bluetooth_info)
{
    app_dashboard_refresh_main_display_bluetooth_info(bluetooth_status, current_bluetooth_info);
}

void app_dashboard_refresh_main_display_bluetooth_info(gui_img_t *target_image_obj,
                                                       T_BLUETOOTH_INFO current_bluetooth_info)
{
    void *target_digital_resource_loc = 0x0;

    if (current_bluetooth_info == T_BLUETOOTH_INFO_ON)
    {
        target_digital_resource_loc = BTON_BIN;
    }
    else
    {
        target_digital_resource_loc = BTOF_BIN;
    }

    if (target_digital_resource_loc != 0x0)
    {
        gui_img_set_attribute(target_image_obj,
                              target_image_obj->base.name,
                              (void *)target_digital_resource_loc,
                              target_image_obj->base.x,
                              target_image_obj->base.y);
    }
}

void app_dashboard_update_main_display_message_infor(app_message_data *app_message_information)
{
    short_message->base.not_show = true;
    short_message_data->base.not_show = true;
    if (app_message_information->wechat_notify_message)
    {
        short_message->base.not_show = false;
        short_message_data->base.not_show = false;
    }

    memcpy(&show_message_data[0], &app_message_information->wechat_msg[0],
           app_message_information->wechat_msg_len);
    gui_text_set(short_message_data, (char *)show_message_data, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX,
                 UINT8_MAX, UINT8_MAX), (app_message_information->wechat_msg_len - 1), 32);
}

void app_dashboard_update_main_display_phone_infor(app_phone_data *app_call_information)
{
    tel_box->base.not_show = true;
    tel_accept->base.not_show = true;
    tel_reject_end->base.not_show = true;
    refuse_button->base.not_show = true;
    ans_button->base.not_show = true;
    tel_box_left_button->base.not_show = true;
    tel_box_right_button->base.not_show = true;
    short_tel_number->base.not_show = true;
    short_tel_accept->base.not_show = true;

    if (app_call_information->phone_status == T_PHONE_STATUS_ONGOING)
    {
        tel_box->base.not_show = false;
        refuse_button->base.not_show = false;
        ans_button->base.not_show = false;
        tel_box_left_button->base.not_show = false;
        tel_box_right_button->base.not_show = false;
        short_tel_number->base.not_show = false;
    }
//    if ((app_call_information->phone_status == T_PHONE_STATUS_ACCEPT) ||
//        (app_call_information->phone_status == T_PHONE_STATUS_ONGOING))
//    {
//            tel_reject_end->base.not_show = false;
//        gui_log("tel reject or end");
//    }
    if (app_call_information->phone_status == T_PHONE_STATUS_ACCEPT)
    {
        tel_accept->base.not_show = false;
        short_tel_accept->base.not_show = false;
        gui_log("tel accept");
    }

    memcpy(&show_tel_number[0], &app_call_information->current_phone_number[0],
           app_call_information->current_phone_number_len);
    gui_text_set(short_tel_number, (char *)show_tel_number, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX,
                 UINT8_MAX, UINT8_MAX), app_call_information->current_phone_number_len, 28);

}
