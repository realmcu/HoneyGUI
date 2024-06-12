
#include "app_dashboard_data.h"
#include <gui_api.h>
#include "app_dashboard_main_display.h"
#include "app_dashboard_connected_display.h"
#ifndef _WIN32
#include "trace.h"
#include "communicate_parse_navigation.h"
#include "communicate_parse_notify.h"
#include "os_timer.h"
#include "os_sync.h"
#include "communicate_protocol.h"
#include "communicate_parse.h"
#endif
uint32_t current_counter = 0x0;
bool is_blink_the_right_light = false;
bool is_the_light_on = false;
uint8_t temp_count = 0;

char str_def1[] = {0xe5, 0x85, 0xac, 0xe9, 0x87, 0x8c, '\0'};
char str_def2[] = {0xe8, 0xbf, 0x9b, 0xe5, 0x85, 0xa5, '\0'};
char str_def3[] = {0xe7, 0xb1, 0xb3, '\0'};
char str_def4[] = {0x20, 0xe8, 0xbf, 0x9b, 0xe5, 0x85, 0xa5, '\0'};

bool COMMUNICATE_ALLOW_DLPS = true;

bool communicate_navi_dlps_check(void)
{
    return COMMUNICATE_ALLOW_DLPS;
}
void app_dashboard_auto_refresh_data_demo(void)
{
    uint8_t current_battery_level_temp = 0x0;
    uint8_t current_speed_temp = 0x0;
    T_TURN_INFO current_turn_info_temp;
    T_BLUETOOTH_INFO current_bluetooth_status_temp;
    T_NAVI_INFO current_navi_information;

    temp_count++;
    if (temp_count % 2 == 0)
    {
        return;
    }
    current_counter++;

    current_battery_level_temp = app_dashboard_data_get_battery_level();
    current_speed_temp = app_dashboard_data_get_car_speed();

    current_battery_level_temp++;
    current_speed_temp++;
    if (current_speed_temp >= 50)
    {
        current_speed_temp = 0;
    }

    if (current_battery_level_temp >= 100)
    {
        current_battery_level_temp = 0;
    }

    /* Demo turn on/off the light */
    if (current_counter % 10 == 0)
    {
        is_blink_the_right_light = !is_blink_the_right_light;
    }
    if (current_counter % 1 == 0)
    {
        is_the_light_on = !is_the_light_on;
    }
    current_turn_info_temp = T_TURN_INFO_NONE;
    if (is_blink_the_right_light)
    {
        if (is_the_light_on)
        {
            current_turn_info_temp = T_TURN_INFO_RIGHT;
        }
    }
    else
    {
        if (is_the_light_on)
        {
            current_turn_info_temp = T_TURN_INFO_LEFT;
        }
    }

    app_dashboard_data_set_battery_level(current_battery_level_temp);
    app_dashboard_data_set_car_speed(current_speed_temp);
    app_dashboard_data_set_car_turn_info(current_turn_info_temp);
}

void *refresh_count_timer;
static void refresh_count_timer_cb(void *p_handle)
{
    app_dashboard_data_set_current_timer();
}

void app_dashboard_initialize_data(void)
{
    app_current_dashboard_data.current_bluetooth_status = T_BLUETOOTH_INFO_OFF;
    app_current_dashboard_data.show_main_display = true;
    app_current_dashboard_data.refresh_timer_value = 36900;
    app_current_dashboard_data.tense_apm_display = T_TENSE_AM_DISPLAY;
    gui_log("app_dashboard_initialize_data exit\n");

}

void app_dashboard_data_set_current_timer(void)
{
    app_current_dashboard_data.refresh_timer_value++;
    if (app_current_dashboard_data.refresh_timer_value == 43200)
    {
        app_current_dashboard_data.refresh_timer_value = 0;
        app_dashboard_data_set_tense_timer_info(!app_dashboard_data_get_tense_timer_info());
    }
}

void app_dashboard_data_set_tense_timer_info(T_TENSE_APM_INFO current_tense_apm_info)
{
    app_current_dashboard_data.tense_apm_display = current_tense_apm_info;
}

T_TENSE_APM_INFO app_dashboard_data_get_tense_timer_info(void)
{
    return app_current_dashboard_data.tense_apm_display;
}

uint32_t app_dashboard_data_get_current_timer(void)
{
    return app_current_dashboard_data.refresh_timer_value;
}

app_dashboard_data *app_dashboard_get_data(void)
{
    return &app_current_dashboard_data;
}

void app_dashboard_data_set_battery_level(uint8_t battery_level)
{
    app_current_dashboard_data.battery_level = battery_level;
}

uint8_t app_dashboard_data_get_battery_level(void)
{
    return app_current_dashboard_data.battery_level;
}

void app_dashboard_data_set_car_speed(uint8_t current_car_speed)
{
    app_current_dashboard_data.current_speed = current_car_speed;
}

uint8_t app_dashboard_data_get_car_speed(void)
{
    return app_current_dashboard_data.current_speed;
}

void app_dashboard_data_set_car_turn_info(T_TURN_INFO current_car_turn_info)
{
    app_current_dashboard_data.current_turn_information = current_car_turn_info;
}

T_TURN_INFO app_dashboard_data_get_car_turn_info(void)
{
    return app_current_dashboard_data.current_turn_information;
}

void app_dashboard_data_set_bluetooth_status(T_BLUETOOTH_INFO current_bluetooth_status)
{
    app_current_dashboard_data.current_bluetooth_status = current_bluetooth_status;
}

T_BLUETOOTH_INFO app_dashboard_data_get_bluetooth_status(void)
{
    return app_current_dashboard_data.current_bluetooth_status;
}

void app_dashboard_data_set_navi_status(T_NAVI_INFO current_navi_status)
{
    app_current_dashboard_data.current_navi_info = current_navi_status;
}

T_NAVI_INFO app_dashboard_data_get_navi_status(void)
{
    return app_current_dashboard_data.current_navi_info;
}

void app_dashboard_data_set_phone_status(app_phone_data current_phone_status)
{
    memcpy(&app_current_dashboard_data.current_phone_status,
           &current_phone_status,
           sizeof(app_phone_data));
}

void app_dashboard_data_get_phone_status(app_phone_data *rtn_phone_status)
{
    memcpy(rtn_phone_status,
           &app_current_dashboard_data.current_phone_status,
           sizeof(app_phone_data));
}

void app_update_gui_widget(gui_img_t *target_image_obj,
                           uint8_t target_value,
                           void *target_resource_file[],
                           uint8_t max_index_of_resource)
{
    void *target_digital_resource_loc = 0x0;

    if (target_value > max_index_of_resource - 1)
    {
        return;
    }

    if (target_resource_file[target_value] != 0x0)
    {
        gui_img_set_attribute(target_image_obj,
                              target_image_obj->base.name,
                              target_resource_file[target_value],
                              target_image_obj->base.x,
                              target_image_obj->base.y);
    }
}
#ifndef _WIN32
void app_dashboard_data_set_show_main_display(uint8_t Value, T_LE_EVENT event)
{

    if (event == BP_UPDATE_VALUE_EVENT)
    {

        if (Value == NAVI_START)
        {
            app_current_dashboard_data.show_main_display = false;
        }
        else if (Value == NAVI_IDLE)
        {
            COMMUNICATE_ALLOW_DLPS = true;
            app_current_dashboard_data.show_main_display = true;
        }

    }
    else if (event == BP_LE_DISC_EVENT)
    {
        COMMUNICATE_ALLOW_DLPS = true;
        app_current_dashboard_data.show_main_display = true;
        //message off
        app_message_data current_message_status;
        app_dashboard_data_get_message_data_update(&current_message_status);
        current_message_status.wechat_notify_message = false;
        app_dashboard_data_set_message_data_update(current_message_status);
        //phone nume off
        app_phone_data current_phone_status;
        app_dashboard_data_get_phone_status(&current_phone_status);
        current_phone_status.phone_status = T_PHONE_STATUS_NONE;
        app_dashboard_data_set_phone_status(current_phone_status);
    }

}
#endif
uint8_t app_dashboard_data_get_show_main_display(void)
{
    return app_current_dashboard_data.show_main_display;
}

void app_dashboard_data_set_navi_data_update(app_navi_data current_navi_data)
{
    memcpy(&app_current_dashboard_data.current_navi_data,
           &current_navi_data,
           sizeof(app_navi_data));
}

void app_dashboard_data_get_navi_data_update(app_navi_data *rtn_phone_status)
{
    memcpy(rtn_phone_status,
           &app_current_dashboard_data.current_navi_data,
           sizeof(app_navi_data));
}

void app_dashboard_data_update_navi_status(const uint8_t *pValue, uint16_t length)
{

    int32_t distance = 0;
#ifndef _WIN32
    gui_indev_wakeup();
    COMMUNICATE_ALLOW_DLPS = false;
    gui_msg_t msg;
    msg.event = GUI_EVENT_RESET_ACTIVE_TIME;
    uint8_t navi_type = 0;
    navi_type = ((uint8_t *)pValue)[0];
    if (navi_type == TURN_LEFT || navi_type == LEFT_FRONT || navi_type == LEFT_BACK
        || navi_type == LEFT_TURN_AROUND)
    {
        //turn_left();
        app_current_dashboard_data.current_navi_info = T_NAVI_INFO_2;
    }
    else if (navi_type == TURN_RIGHT || navi_type == RIGHT_FRONT || navi_type == RIGHT_BACK)
    {
        //turn_right();
        app_current_dashboard_data.current_navi_info = T_NAVI_INFO_3;
    }
    else
    {
        //stright_ahead();
        app_current_dashboard_data.current_navi_info = T_NAVI_INFO_1;
    }
#endif
    app_navi_data current_navi_data;
    app_dashboard_data_get_navi_data_update(&current_navi_data);

    memset(current_navi_data.navigation_msg, 0x0, sizeof(current_navi_data.navigation_msg));
    memset(current_navi_data.road_names, 0x0, sizeof(current_navi_data.road_names));

    distance = ((uint8_t *)pValue)[1];
    distance = (distance << 8) + ((uint8_t *)pValue)[2];

    if (distance / 1000)
    {
        sprintf((char *)current_navi_data.navigation_msg, "%d%s%d", distance / 1000, ".",
                (distance % 1000 / 100));
        current_navi_data.navigation_num_len = strlen((char *)current_navi_data.navigation_msg);

        memcpy(current_navi_data.navigation_unit, str_def1, strlen(str_def1));
        current_navi_data.navigation_unit_len = strlen(str_def1);

        memcpy(current_navi_data.road_names, str_def2, strlen(str_def2));
        current_navi_data.road_num_len = strlen(str_def2);
        memcpy(current_navi_data.road_names + current_navi_data.road_num_len, (uint8_t *)pValue + 3,
               length - 3);
        current_navi_data.road_num_len = length - 3 + strlen(str_def2);
    }
    else
    {
        sprintf((char *)current_navi_data.navigation_msg, "%d", distance);
        current_navi_data.navigation_num_len = strlen((char *)current_navi_data.navigation_msg);

        memcpy(current_navi_data.navigation_unit, str_def3, strlen(str_def3));
        current_navi_data.navigation_unit_len = strlen(str_def3);

        memcpy(current_navi_data.road_names, str_def4, strlen(str_def4));
        current_navi_data.road_num_len = strlen(str_def4);
        memcpy(current_navi_data.road_names + current_navi_data.road_num_len, (uint8_t *)pValue + 3,
               length - 3);
        current_navi_data.road_num_len = length - 3 + strlen(str_def4);
    }

    app_dashboard_data_set_navi_data_update(current_navi_data);

}

void app_dashboard_data_set_message_data_update(app_message_data current_message_status)
{
    memcpy(&app_current_dashboard_data.current_message_status,
           &current_message_status,
           sizeof(app_message_data));
}

void app_dashboard_data_get_message_data_update(app_message_data *rtn_phone_status)
{
    memcpy(rtn_phone_status,
           &app_current_dashboard_data.current_message_status,
           sizeof(app_message_data));
}

void app_dashboard_data_set_reject_end_call(void)
{
    app_phone_data current_phone_status;
    app_dashboard_data_get_phone_status(&current_phone_status);
#ifndef _WIN32
    if ((current_phone_status.phone_status == T_PHONE_STATUS_ACCEPT) ||
        (current_phone_status.phone_status == T_PHONE_STATUS_ONGOING))
    {
        struct protocol_pack p = {0};
        p.l2_cmd_id = NOTIFY_COMMAND_ID;
        p.l2_key = NOTIFY_CALL_EVENT_REJECT_END;
        p.l2_lenght = 1;
        p.l2_payload[0] = 0;
        package_prepare_send(&p);
    }
#endif
}

void app_dashboard_data_set_accept_call(void)
{
    app_phone_data current_phone_status;
    app_dashboard_data_get_phone_status(&current_phone_status);
#ifndef _WIN32
    if (current_phone_status.phone_status == T_PHONE_STATUS_ONGOING)
    {
        struct protocol_pack p = {0};
        p.l2_cmd_id = NOTIFY_COMMAND_ID;
        p.l2_key = NOTIFY_CALL_EVENT_ACCEPT;
        p.l2_lenght = 1;
        p.l2_payload[0] = 0;
        package_prepare_send(&p);
    }
#endif
}

void app_dashboard_data_update_phone_status(uint8_t key, const uint8_t *pValue, uint16_t length)
{
#ifndef _WIN32
    if (length <= 11)
    {
        gui_indev_wakeup();
        app_phone_data current_phone_status;
        app_dashboard_data_get_phone_status(&current_phone_status);

        memset(current_phone_status.current_phone_number, 0x0,
               sizeof(current_phone_status.current_phone_number));

        if (key == KEY_INCOMMING_CALL)
        {
            current_phone_status.phone_status = T_PHONE_STATUS_ONGOING;
        }
        else if (key == KEY_INCOMMING_CALL_ACCEPT)
        {
            current_phone_status.phone_status = T_PHONE_STATUS_ACCEPT;
        }
        else if (key == KEY_INCOMMING_CALL_REFUSE)
        {
            current_phone_status.phone_status = T_PHONE_STATUS_NONE;
        }
        current_phone_status.current_phone_number_len = length;
        memcpy(current_phone_status.current_phone_number,
               pValue,
               length);
        app_dashboard_data_set_phone_status(current_phone_status);
    }
    else
    {
        APP_PRINT_INFO0("app_dashboard_data_update_phone_status over size");
    }
#endif
}

void *dashboard_message_timer;
static void dashboard_message_timer_cb(void *p_handle)
{
#ifndef _WIN32
    APP_PRINT_INFO0("dashboard_message_timer_cb");
    app_message_data current_message_status;
    app_dashboard_data_get_message_data_update(&current_message_status);
    memset(current_message_status.wechat_msg, 0x0, sizeof(current_message_status.wechat_msg));
    current_message_status.wechat_notify_message = false;
    app_dashboard_data_set_message_data_update(current_message_status);

    os_timer_stop(&p_handle);
#endif
}

void app_dashboard_data_update_message_status(const uint8_t *pValue, uint16_t length,
                                              uint8_t messaye_type)
{
#ifndef _WIN32
    gui_indev_wakeup();
    app_message_data current_message_status;
    app_dashboard_data_get_message_data_update(&current_message_status);

    memset(current_message_status.wechat_msg, 0x0, sizeof(current_message_status.wechat_msg));

    current_message_status.wechat_msg_len = length;
    memcpy(&current_message_status.wechat_msg[0], &pValue[0], length);
    current_message_status.wechat_notify_message = true;
    app_dashboard_data_set_message_data_update(current_message_status);

    os_timer_create(&dashboard_message_timer, "gui-tick", 0, 3000, true, dashboard_message_timer_cb);
    os_timer_start(&dashboard_message_timer);
#endif
}
