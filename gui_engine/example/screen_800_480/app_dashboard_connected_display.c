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
#include "app_dashboard_connected_display.h"
#include "app_dashboard_data.h"
#include "trace.h"

uint8_t show_c_navigation_msg[40];
uint8_t show_c_navigation_unit[40];
uint8_t show_c_road_names[40];
uint8_t show_c_message_data[40];
uint8_t show_c_tel_number[11];

void app_dashboard_create_connected_display(gui_win_t *target_connected_display)
{
    /* set update callback */
    gui_win_set_animate(target_connected_display, 1000, -1, paint_connected_display_cb,
                        target_connected_display);

    /* set Image data */
    dashboard_c_background = gui_img_create_from_mem(target_connected_display, "dashboard_c_background",
                                                     BACKGROUND_C_BIN, 0, 0, 800, 480);
    dashboard_c_background->draw_img.blend_mode = IMG_FILTER_BLACK;
    speed_high_c_digital = gui_img_create_from_mem(target_connected_display, "speed_high_c_digital",
                                                   SPED_C0_BIN, 214, 232, 27, 40);
    speed_high_c_digital->draw_img.blend_mode = IMG_FILTER_BLACK;
    speed_low_c_digital = gui_img_create_from_mem(target_connected_display, "speed_low_c_digital",
                                                  SPED_C0_BIN, 251, 232, 27, 40);
    speed_low_c_digital->draw_img.blend_mode = IMG_FILTER_BLACK;
    hour_high_c_digital = gui_img_create_from_mem(target_connected_display, "hour_high_digital",
                                                  TIMER0_BIN, 355, 12, 9, 16);
    hour_high_c_digital->draw_img.blend_mode = IMG_FILTER_BLACK;
    hour_low_c_digital = gui_img_create_from_mem(target_connected_display, "hour_low_digital",
                                                 TIMER0_BIN, 366, 12, 9, 16);
    hour_low_c_digital->draw_img.blend_mode = IMG_FILTER_BLACK;
    min_high_c_digital = gui_img_create_from_mem(target_connected_display, "min_high_digital",
                                                 TIMER0_BIN, 387, 12, 9, 18);
    min_high_c_digital->draw_img.blend_mode = IMG_FILTER_BLACK;
    min_low_c_digital = gui_img_create_from_mem(target_connected_display, "min_low_digital", TIMER0_BIN,
                                                399, 12, 9, 18);
    min_low_c_digital->draw_img.blend_mode = IMG_FILTER_BLACK;
    tense_high_c_digital = gui_img_create_from_mem(target_connected_display, "tense_high_c_digital",
                                                   APM_CA_BIN, 417, 12, 9, 18);
    tense_high_c_digital->draw_img.blend_mode = IMG_FILTER_BLACK;
    tense_low_c_digital = gui_img_create_from_mem(target_connected_display, "tense_low_c_digital",
                                                  APM_CM_BIN,
                                                  432, 12, 9, 18);
    tense_low_c_digital->draw_img.blend_mode = IMG_FILTER_BLACK;
    bat_high_c_digital = gui_img_create_from_mem(target_connected_display, "bat_high_c_digital",
                                                 TIMER0_BIN, 238, 371, 9, 18);
    bat_high_c_digital->draw_img.blend_mode = IMG_FILTER_BLACK;
    bat_low_c_digital = gui_img_create_from_mem(target_connected_display, "bat_low_c_digital",
                                                TIMER0_BIN, 250, 371, 9, 18);
    bat_low_c_digital->draw_img.blend_mode = IMG_FILTER_BLACK;
    left_turn_light_c_status = gui_img_create_from_mem(target_connected_display,
                                                       "left_turn_light_c_status", TL_OF_C_BIN, 110, 10, 42, 40);
    left_turn_light_c_status->draw_img.blend_mode = IMG_FILTER_BLACK;
    right_turn_light_c_status = gui_img_create_from_mem(target_connected_display,
                                                        "right_turn_light_c_status", TR_OF_C_BIN, 665, 10, 42, 40);
    right_turn_light_c_status->draw_img.blend_mode = IMG_FILTER_BLACK;
    navi_c_status = gui_img_create_from_mem(target_connected_display, "navi_c_status", NAVI_C1_BIN, 505,
                                            170, 268, 218);
    navi_c_status->draw_img.blend_mode = IMG_FILTER_BLACK;
    refuse_c_button = gui_img_create_from_mem(target_connected_display, "refuse_c_button", REFUS_BIN,
                                              502, 410, 36, 36);
    refuse_c_button->draw_img.blend_mode = IMG_FILTER_BLACK;
    ans_c_button = gui_img_create_from_mem(target_connected_display, "ans_c_button", ANS_BIN, 740, 410,
                                           36, 36);
    ans_c_button->draw_img.blend_mode = IMG_FILTER_BLACK;
    tel_box_left_c_button = gui_img_create_from_mem(target_connected_display, "tel_box_left_c_button",
                                                    SYMB1_BIN, 554, 414, 36, 36);
    tel_box_left_c_button->draw_img.blend_mode = IMG_FILTER_BLACK;
    tel_box_right_c_button = gui_img_create_from_mem(target_connected_display, "tel_box_right_c_button",
                                                     SYMB2_BIN, 718, 414, 36, 36);
    tel_box_right_c_button->draw_img.blend_mode = IMG_FILTER_BLACK;
    dashboard_Cpointer = gui_img_create_from_mem(target_connected_display, "dashboard_Cpointer",
                                                 DASHBOARD_C2_BIN, 110, 133, 9, 18);
    dashboard_Cpointer->draw_img.blend_mode = IMG_FILTER_BLACK;
    short_c_message = gui_img_create_from_mem(target_connected_display, "short_c_message",
                                              MESSAGE_BIN, 221, 0, 359, 80);
    short_c_message->draw_img.blend_mode = IMG_FILTER_BLACK;

    /* set font data */
    app_navi_data current_navi_data;
    app_dashboard_data_get_navi_data_update(&current_navi_data);

    memcpy(&show_c_navigation_msg[0], &current_navi_data.navigation_msg[0],
           current_navi_data.navigation_num_len);
    short_c_navi_message_1 = gui_text_create(target_connected_display,  "short_c_navi_message_1", 490,
                                             88, 150,
                                             60);
    gui_text_set(short_c_navi_message_1, (char *)show_c_navigation_msg, "rtk_font_mem",
                 gui_rgb(UINT8_MAX, UINT8_MAX, UINT8_MAX), current_navi_data.navigation_num_len, 56);
    gui_text_mode_set(short_c_navi_message_1, RIGHT);

    memcpy(&show_c_navigation_unit[0], &current_navi_data.navigation_unit[0],
           current_navi_data.navigation_unit_len);
    short_c_navi_message_3 = gui_text_create(target_connected_display,  "short_c_navi_message_3", 640,
                                             108, 150,
                                             40);
    gui_text_set(short_c_navi_message_3, (char *)show_c_navigation_unit, "rtk_font_mem", gui_rgb(0xcc,
                 0xcc, 0xcc), current_navi_data.navigation_unit_len, 32);
    gui_text_mode_set(short_c_navi_message_3, LEFT);

    memcpy(&show_c_road_names[0], &current_navi_data.road_names[0],
           current_navi_data.road_num_len);
    short_c_navi_message_2 = gui_text_create(target_connected_display,  "short_c_navi_message_2", 490,
                                             148, 300,
                                             40);
    gui_text_set(short_c_navi_message_2, (char *)show_c_road_names, "rtk_font_mem", gui_rgb(0xcc, 0xcc,
                 0xcc), current_navi_data.road_num_len, 32);
    gui_text_mode_set(short_c_navi_message_2, CENTER);

    app_phone_data current_phone_status;
    app_dashboard_data_get_phone_status(&current_phone_status);
    short_c_tel_number = gui_text_create(target_connected_display,  "short_c_tel_number",  560, 410,
                                         158, 30);
    memcpy(&show_c_tel_number[0], &current_phone_status.current_phone_number[0],
           current_phone_status.current_phone_number_len);
    gui_text_set(short_c_tel_number, (char *)show_c_tel_number, "rtk_font_mem", gui_rgb(UINT8_MAX,
                 UINT8_MAX, UINT8_MAX), current_phone_status.current_phone_number_len, 28);
    gui_text_mode_set(short_c_tel_number, CENTER);
    short_c_tel_accept = gui_text_create(target_connected_display,  "short_c_tel_accept",  600, 410,
                                         800, 30);
    gui_text_set(short_c_tel_accept, "calling", "rtk_font_mem", gui_rgb(UINT8_MAX, UINT8_MAX,
                                                                        UINT8_MAX), 7, 28);
    app_message_data current_message_status;
    app_dashboard_data_get_message_data_update(&current_message_status);
    memcpy(&show_c_message_data[0], &current_message_status.wechat_msg[0],
           current_message_status.wechat_msg_len);
    short_c_message_data = gui_text_create(target_connected_display,  "short_c_message_data",  300, 10,
                                           240, 50);
    gui_text_set(short_c_message_data, (char *)show_c_message_data, "rtk_font_mem", gui_rgb(UINT8_MAX,
                 UINT8_MAX, UINT8_MAX), (current_message_status.wechat_msg_len - 1), 32);
    gui_text_mode_set(short_c_message_data, MUTI_LEFT);

    /* Prepare the intial data */
    app_dashboard_update_connected_display_time_info();
    app_dashboard_update_connected_display_tense_apm_info(app_dashboard_data_get_tense_timer_info());
    app_dashboard_update_connected_display_battery_info(app_dashboard_data_get_battery_level());
    app_dashboard_update_connected_display_speed_info(app_dashboard_data_get_car_speed());
    app_dashboard_update_connected_display_turn_info(app_dashboard_data_get_car_turn_info());
    app_dashboard_update_connected_display_phone_infor(&current_phone_status);
    app_dashboard_update_connected_display_message_infor(&current_message_status);
}

extern gui_win_t *win_main_display;
void paint_connected_display_cb(gui_win_t *win)
{
    if (app_dashboard_data_get_show_main_display() == true)
    {
        win->base.not_show = app_dashboard_data_get_show_main_display();
        if (win_main_display->base.not_show)
        {
            win_main_display->base.not_show = false;
        }
        return;
    }

    app_phone_data current_phone_status;
    app_dashboard_data_get_phone_status(&current_phone_status);
    app_dashboard_update_connected_display_time_info();
    app_dashboard_update_connected_display_tense_apm_info(app_dashboard_data_get_tense_timer_info());
    app_dashboard_update_connected_display_battery_info(app_dashboard_data_get_battery_level());
    app_dashboard_update_connected_display_speed_info(app_dashboard_data_get_car_speed());
    app_dashboard_update_connected_display_turn_info(app_dashboard_data_get_car_turn_info());
    app_dashboard_update_connected_display_phone_infor(&current_phone_status);
    app_dashboard_update_connected_display_navi_infor(app_dashboard_data_get_navi_status());
    app_message_data current_message_status;
    app_dashboard_data_get_message_data_update(&current_message_status);
    app_dashboard_update_connected_display_message_infor(&current_message_status);
}

void app_dashboard_update_connected_display_time_info(void)
{
    uint32_t connected_display_hour = app_dashboard_data_get_current_timer() / 3600;
    uint32_t connected_display_min = (app_dashboard_data_get_current_timer() -
                                      (connected_display_hour * 3600)) / 60;

    app_update_gui_widget(hour_high_c_digital,
                          connected_display_hour / 10,
                          connected_display_time_resource_def,
                          sizeof(connected_display_time_resource_def));
    app_update_gui_widget(hour_low_c_digital,
                          connected_display_hour % 10,
                          connected_display_time_resource_def,
                          sizeof(connected_display_time_resource_def));
    app_update_gui_widget(min_high_c_digital,
                          connected_display_min / 10,
                          connected_display_time_resource_def,
                          sizeof(connected_display_time_resource_def));
    app_update_gui_widget(min_low_c_digital,
                          connected_display_min % 10,
                          connected_display_time_resource_def,
                          sizeof(connected_display_time_resource_def));
}

void app_dashboard_update_connected_display_tense_apm_info(T_TENSE_APM_INFO
                                                           current_tense_apm_info)
{
    app_dashboard_refresh_connected_display_tense_apm_info(tense_high_c_digital, tense_low_c_digital,
                                                           current_tense_apm_info);
}

void app_dashboard_refresh_connected_display_tense_apm_info(gui_img_t *tense_high_image_obj,
                                                            gui_img_t *tense_low_image_obj,
                                                            T_TENSE_APM_INFO current_tense_apm_info)
{
    void *tense_high_digital_info = APM_CA_BIN;
    void *tense_low_digital_info = APM_CM_BIN;

    if (current_tense_apm_info == T_TENSE_AM_DISPLAY)
    {
        tense_high_digital_info = APM_CA_BIN;
        tense_low_digital_info = APM_CM_BIN;
    }
    else if (current_tense_apm_info == T_TENSE_PM_DISPLAY)
    {
        tense_high_digital_info = APM_CP_BIN;
        tense_low_digital_info = APM_CM_BIN;
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

void app_dashboard_update_connected_display_battery_info(uint8_t battery_level)
{
    app_update_gui_widget(bat_high_c_digital,
                          battery_level / 10,
                          connected_display_battery_resource_def,
                          sizeof(connected_display_battery_resource_def));
    app_update_gui_widget(bat_low_c_digital,
                          battery_level % 10,
                          connected_display_battery_resource_def,
                          sizeof(connected_display_battery_resource_def));
}

void app_dashboard_update_connected_display_speed_info(uint8_t current_speed)
{
    DBG_DIRECT("app_dashboard_update_connected_display_speed_info:%d", __LINE__);
    app_update_gui_widget(speed_high_c_digital,
                          current_speed / 10,
                          connected_display_speed_resource_def,
                          sizeof(connected_display_speed_resource_def));
    app_update_gui_widget(speed_low_c_digital,
                          current_speed % 10,
                          connected_display_speed_resource_def,
                          sizeof(connected_display_speed_resource_def));
    app_update_gui_widget(dashboard_Cpointer,
                          current_speed / UPDATE_DASHBOARD_GRANULARITY,
                          dashboard_Cpointer_num,
                          sizeof(dashboard_Cpointer_num));
}

void app_dashboard_update_connected_display_turn_info(T_TURN_INFO current_turn_information)
{
    app_dashboard_refresh_connected_display_turn_info(left_turn_light_c_status,
                                                      right_turn_light_c_status,
                                                      current_turn_information);
}

void app_dashboard_refresh_connected_display_turn_info(gui_img_t *left_turn_image_obj,
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

void app_dashboard_update_connected_display_message_infor(app_message_data *app_message_information)
{
    short_c_message->base.not_show = true;
    short_c_message_data->base.not_show = true;
    if (app_message_information->wechat_notify_message)
    {
        short_c_message->base.not_show = false;
        short_c_message_data->base.not_show = false;
    }
    memcpy(&show_c_message_data[0], &app_message_information->wechat_msg[0],
           app_message_information->wechat_msg_len);
    gui_text_set(short_c_message_data, (char *)show_c_message_data, "rtk_font_mem", gui_rgb(UINT8_MAX,
                 UINT8_MAX, UINT8_MAX), (app_message_information->wechat_msg_len - 1), 32);
}

void app_dashboard_update_connected_display_phone_infor(app_phone_data *app_call_information)
{
    refuse_c_button->base.not_show = true;
    ans_c_button->base.not_show = true;
    tel_box_left_c_button->base.not_show = true;
    tel_box_right_c_button->base.not_show = true;
    short_c_tel_number->base.not_show = true;
    short_c_tel_accept->base.not_show = true;

    if (app_call_information->phone_status == T_PHONE_STATUS_ONGOING)
    {
        refuse_c_button->base.not_show = false;
        ans_c_button->base.not_show = false;
        tel_box_left_c_button->base.not_show = false;
        tel_box_right_c_button->base.not_show = false;
        short_c_tel_number->base.not_show = false;
    }
    if (app_call_information->phone_status == T_PHONE_STATUS_ACCEPT)
    {
        short_c_tel_accept->base.not_show = false;
    }

    memcpy(&show_c_tel_number[0], &app_call_information->current_phone_number[0],
           app_call_information->current_phone_number_len);
    gui_text_set(short_c_tel_number, (char *)show_c_tel_number, "rtk_font_mem", gui_rgb(UINT8_MAX,
                 UINT8_MAX, UINT8_MAX), app_call_information->current_phone_number_len, 28);

}

void app_dashboard_update_connected_display_navi_infor(T_NAVI_INFO navi_information)
{
    navi_c_status->base.not_show = true;

    if (navi_information != T_NAVI_INFO_5)
    {
        navi_c_status->base.not_show = false;
    }

    app_update_gui_widget(navi_c_status,
                          navi_information % 10,
                          connected_display_navi_resource_def,
                          sizeof(connected_display_navi_resource_def));
    app_navi_data current_navi_data;
    app_dashboard_data_get_navi_data_update(&current_navi_data);
    memcpy(&show_c_navigation_msg[0], &current_navi_data.navigation_msg[0],
           current_navi_data.navigation_num_len);
    gui_text_set(short_c_navi_message_1, (char *)show_c_navigation_msg, "rtk_font_mem",
                 gui_rgb(UINT8_MAX, UINT8_MAX, UINT8_MAX), current_navi_data.navigation_num_len, 56);

    memcpy(&show_c_navigation_unit[0], &current_navi_data.navigation_unit[0],
           current_navi_data.navigation_unit_len);
    gui_text_set(short_c_navi_message_3, (char *)show_c_navigation_unit, "rtk_font_mem", gui_rgb(0xcc,
                 0xcc, 0xcc), current_navi_data.navigation_unit_len, 32);

    memcpy(&show_c_road_names[0], &current_navi_data.road_names[0],
           current_navi_data.road_num_len);
    gui_text_set(short_c_navi_message_2, (char *)show_c_road_names, "rtk_font_mem", gui_rgb(0xcc, 0xcc,
                 0xcc), current_navi_data.road_num_len, 32);
}
