/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "app_dashboard_connected_display.h"
#include "app_dashboard_data.h"
#include "gui_components_init.h"
#ifndef _WIN32
#include "trace.h"
#endif

/*============================================================================*
 *                            Macros
 *============================================================================*/
#define CURRENT_VIEW_NAME "connect_view"

/*============================================================================*
 *                           Function Declaration
 *============================================================================*/


/*============================================================================*
 *                            Variables
 *============================================================================*/
static gui_view_t *current_view = NULL;
const static gui_view_descriptor_t *main_view = NULL;
static gui_view_descriptor_t const descriptor =
{
    /* change Here for current view */
    .name = (const char *)CURRENT_VIEW_NAME,
    .pView = &current_view,
    .on_switch_in = app_dashboard_create_connected_display,
    .on_switch_out = NULL,
    .keep = false,
};
uint8_t show_c_navigation_msg[40];
uint8_t show_c_navigation_unit[40];
uint8_t show_c_road_names[40];
uint8_t show_c_message_data[40];
uint8_t show_c_tel_number[11];

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
    main_view = gui_view_descriptor_get("main_view");
    gui_log("File: %s, Function: %s\n", __FILE__, __func__);
    return 0;
}
static GUI_INIT_VIEW_DESCRIPTOR_GET(gui_view_get_other_view_descriptor_init);

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
void app_dashboard_create_connected_display(gui_view_t *view)
{
    gui_view_set_animate_step(view, 1000);
    gui_view_switch_on_event(view, main_view, SWITCH_OUT_NONE_ANIMATION,
                             SWITCH_OUT_NONE_ANIMATION, GUI_EVENT_KB_SHORT_CLICKED); //switch test
    /* set update callback */
    gui_obj_create_timer(GUI_BASE(view), 25, true, paint_connected_display_cb);

    /* set Image data */
    dashboard_c_background = gui_img_create_from_mem(view, "dashboard_c_background",
                                                     BACKGROUND_C_BIN, 0, 0, 800, 480);
    gui_img_set_mode(dashboard_c_background, IMG_COVER_MODE);
    speed_high_c_digital = gui_img_create_from_mem(view, "speed_high_c_digital",
                                                   SPED_C0_BIN, 214, 232, 27, 40);
    speed_low_c_digital = gui_img_create_from_mem(view, "speed_low_c_digital",
                                                  SPED_C0_BIN, 251, 232, 27, 40);
    hour_high_c_digital = gui_img_create_from_mem(view, "hour_high_digital",
                                                  TIMER0_BIN, 355, 12, 9, 16);
    hour_low_c_digital = gui_img_create_from_mem(view, "hour_low_digital",
                                                 TIMER0_BIN, 366, 12, 9, 16);
    min_high_c_digital = gui_img_create_from_mem(view, "min_high_digital",
                                                 TIMER0_BIN, 387, 12, 9, 18);
    min_low_c_digital = gui_img_create_from_mem(view, "min_low_digital", TIMER0_BIN,
                                                399, 12, 9, 18);
    tense_high_c_digital = gui_img_create_from_mem(view, "tense_high_c_digital",
                                                   APM_CA_BIN, 417, 12, 9, 18);
    tense_low_c_digital = gui_img_create_from_mem(view, "tense_low_c_digital",
                                                  APM_CM_BIN,
                                                  432, 12, 9, 18);
    bat_high_c_digital = gui_img_create_from_mem(view, "bat_high_c_digital",
                                                 TIMER0_BIN, 238, 371, 9, 18);
    bat_low_c_digital = gui_img_create_from_mem(view, "bat_low_c_digital",
                                                TIMER0_BIN, 250, 371, 9, 18);
    left_turn_light_c_status = gui_img_create_from_mem(view,
                                                       "left_turn_light_c_status", TL_OF_C_BIN, 110, 10, 42, 40);
    right_turn_light_c_status = gui_img_create_from_mem(view,
                                                        "right_turn_light_c_status", TR_OF_C_BIN, 665, 10, 42, 40);
    navi_c_status = gui_img_create_from_mem(view, "navi_c_status", NAVI_C1_BIN, 505,
                                            170, 268, 218);
    refuse_c_button = gui_img_create_from_mem(view, "refuse_c_button", REFUS_BIN,
                                              502, 410, 36, 36);
    ans_c_button = gui_img_create_from_mem(view, "ans_c_button", ANS_BIN, 740, 410,
                                           36, 36);
    tel_box_left_c_button = gui_img_create_from_mem(view, "tel_box_left_c_button",
                                                    SYMB1_BIN, 554, 414, 36, 36);
    tel_box_right_c_button = gui_img_create_from_mem(view, "tel_box_right_c_button",
                                                     SYMB2_BIN, 718, 414, 36, 36);
    dashboard_Cpointer = gui_img_create_from_mem(view, "dashboard_Cpointer",
                                                 DASHBOARD_C2_BIN, 110, 133, 9, 18);
    short_c_message = gui_img_create_from_mem(view, "short_c_message",
                                              MESSAGE_BIN, 221, 0, 359, 80);

    /* set font data */
    app_navi_data current_navi_data;
    app_dashboard_data_get_navi_data_update(&current_navi_data);

    memcpy(&show_c_navigation_msg[0], &current_navi_data.navigation_msg[0],
           current_navi_data.navigation_num_len);
    short_c_navi_message_1 = gui_text_create(view,  "short_c_navi_message_1", 490,
                                             88, 150,
                                             60);
    gui_text_set(short_c_navi_message_1, (char *)show_c_navigation_msg, GUI_FONT_SRC_BMP,
                 gui_rgb(UINT8_MAX, UINT8_MAX, UINT8_MAX), current_navi_data.navigation_num_len, 56);
    gui_text_mode_set(short_c_navi_message_1, RIGHT);

    memcpy(&show_c_navigation_unit[0], &current_navi_data.navigation_unit[0],
           current_navi_data.navigation_unit_len);
    short_c_navi_message_3 = gui_text_create(view,  "short_c_navi_message_3", 640,
                                             108, 150,
                                             40);
    gui_text_set(short_c_navi_message_3, (char *)show_c_navigation_unit, GUI_FONT_SRC_BMP,
                 gui_rgb(0xcc,
                         0xcc, 0xcc), current_navi_data.navigation_unit_len, 32);
    gui_text_mode_set(short_c_navi_message_3, LEFT);

    memcpy(&show_c_road_names[0], &current_navi_data.road_names[0],
           current_navi_data.road_num_len);
    short_c_navi_message_2 = gui_text_create(view,  "short_c_navi_message_2", 490,
                                             148, 300,
                                             40);
    gui_text_set(short_c_navi_message_2, (char *)show_c_road_names, GUI_FONT_SRC_BMP, gui_rgb(0xcc,
                 0xcc,
                 0xcc), current_navi_data.road_num_len, 32);
    gui_text_mode_set(short_c_navi_message_2, CENTER);

    app_phone_data current_phone_status;
    app_dashboard_data_get_phone_status(&current_phone_status);
    short_c_tel_number = gui_text_create(view,  "short_c_tel_number",  560, 410,
                                         158, 30);
    memcpy(&show_c_tel_number[0], &current_phone_status.current_phone_number[0],
           current_phone_status.current_phone_number_len);
    gui_text_set(short_c_tel_number, (char *)show_c_tel_number, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX,
                 UINT8_MAX, UINT8_MAX), current_phone_status.current_phone_number_len, 28);
    gui_text_mode_set(short_c_tel_number, CENTER);
    short_c_tel_accept = gui_text_create(view,  "short_c_tel_accept",  600, 410,
                                         800, 30);
    gui_text_set(short_c_tel_accept, "calling", GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX, UINT8_MAX,
                                                                          UINT8_MAX), 7, 28);
    app_message_data current_message_status;
    app_dashboard_data_get_message_data_update(&current_message_status);
    memcpy(&show_c_message_data[0], &current_message_status.wechat_msg[0],
           current_message_status.wechat_msg_len);
    short_c_message_data = gui_text_create(view,  "short_c_message_data",  300, 10,
                                           240, 64);
    gui_text_set(short_c_message_data, (char *)show_c_message_data, GUI_FONT_SRC_BMP,
                 gui_rgb(UINT8_MAX,
                         UINT8_MAX, UINT8_MAX), (current_message_status.wechat_msg_len - 1), 32);
    gui_text_mode_set(short_c_message_data, MULTI_LEFT);

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
void paint_connected_display_cb(void *p)
{
    gui_win_t *win = (gui_win_t *)p;
    if (app_dashboard_data_get_show_main_display() == true)
    {
        gui_view_switch_direct(current_view, main_view, SWITCH_OUT_NONE_ANIMATION,
                               SWITCH_OUT_NONE_ANIMATION);
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
    gui_text_set(short_c_message_data, (char *)show_c_message_data, GUI_FONT_SRC_BMP,
                 gui_rgb(UINT8_MAX,
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
    gui_text_set(short_c_tel_number, (char *)show_c_tel_number, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX,
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
    gui_text_set(short_c_navi_message_1, (char *)show_c_navigation_msg, GUI_FONT_SRC_BMP,
                 gui_rgb(UINT8_MAX, UINT8_MAX, UINT8_MAX), current_navi_data.navigation_num_len, 56);

    memcpy(&show_c_navigation_unit[0], &current_navi_data.navigation_unit[0],
           current_navi_data.navigation_unit_len);
    gui_text_set(short_c_navi_message_3, (char *)show_c_navigation_unit, GUI_FONT_SRC_BMP,
                 gui_rgb(0xcc,
                         0xcc, 0xcc), current_navi_data.navigation_unit_len, 32);

    memcpy(&show_c_road_names[0], &current_navi_data.road_names[0],
           current_navi_data.road_num_len);
    gui_text_set(short_c_navi_message_2, (char *)show_c_road_names, GUI_FONT_SRC_BMP, gui_rgb(0xcc,
                 0xcc,
                 0xcc), current_navi_data.road_num_len, 32);
}
