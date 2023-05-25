#include <bas.h>
#include "app_ble_bas.h"
#include "trace.h"
#include "ble_gap_msg.h"



static T_SERVER_ID bas_srv_id;


static T_APP_RESULT app_bas_callback(T_SERVER_ID service_id, void *p_data)
{
    if (service_id == bas_srv_id)
    {
        T_BAS_CALLBACK_DATA *p_bas_cb_data = (T_BAS_CALLBACK_DATA *)p_data;
        switch (p_bas_cb_data->msg_type)
        {
        case SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION:
            {
                switch (p_bas_cb_data->msg_data.notification_indification_index)
                {
                case BAS_NOTIFY_BATTERY_LEVEL_ENABLE:
                    {
                        APP_PRINT_INFO0("BAS_NOTIFY_BATTERY_LEVEL_ENABLE");
                    }
                    break;

                case BAS_NOTIFY_BATTERY_LEVEL_DISABLE:
                    {
                        APP_PRINT_INFO0("BAS_NOTIFY_BATTERY_LEVEL_DISABLE");
                    }
                    break;
                default:
                    break;
                }
            }
            break;

        case SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE:
            {
                if (p_bas_cb_data->msg_data.read_value_index == BAS_READ_BATTERY_LEVEL)
                {
                    uint8_t battery_level = 90;
                    APP_PRINT_INFO1("BAS_READ_BATTERY_LEVEL: battery_level %d", battery_level);
                    bas_set_parameter(BAS_PARAM_BATTERY_LEVEL, 1, &battery_level);
                }
            }
            break;

        default:
            break;
        }
    }
    return APP_RESULT_SUCCESS;
}

static void app_bas_msg(T_IO_MSG *p_gap_msg)
{
    APP_PRINT_INFO1("line = %d \n", __LINE__);
    APP_PRINT_INFO1("app_handle_gap_msg: subtype %d \n", p_gap_msg->subtype);
}

void app_ble_bas_init(void)
{
    bas_srv_id  = bas_add_service(app_bas_callback);
    le_msg_handler_cback_register(app_bas_msg);
}
