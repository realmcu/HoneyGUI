#include "app_ble_bwps.h"
#include "trace.h"
#include "ble_gap_msg.h"
#include "watch_msg.h"
#include "gap_msg.h"
#include "wristband_private_service.h"
#ifdef RTL8762G_DASHBOARD
#include "app_cluster_data.h"
#endif
static T_SERVER_ID bwps_srv_id;


static T_APP_RESULT app_bwps_callback(T_SERVER_ID service_id, void *p_data)
{
    T_APP_RESULT app_result = APP_RESULT_SUCCESS;
    if (service_id == SERVICE_PROFILE_GENERAL_ID)
    {
        T_SERVER_APP_CB_DATA *p_param = (T_SERVER_APP_CB_DATA *)p_data;
        switch (p_param->eventId)
        {
        case PROFILE_EVT_SRV_REG_COMPLETE:// srv register result event.
            APP_PRINT_INFO1("PROFILE_EVT_SRV_REG_COMPLETE: result %d",
                            p_param->event_data.service_reg_result);
            break;

        case PROFILE_EVT_SEND_DATA_COMPLETE:
            APP_PRINT_INFO5("PROFILE_EVT_SEND_DATA_COMPLETE: conn_id %d, cause 0x%x, service_id %d, attrib_idx 0x%x, credits %d",
                            p_param->event_data.send_data_result.conn_id,
                            p_param->event_data.send_data_result.cause,
                            p_param->event_data.send_data_result.service_id,
                            p_param->event_data.send_data_result.attrib_idx,
                            p_param->event_data.send_data_result.credits);
            if (p_param->event_data.send_data_result.cause == GAP_SUCCESS)
            {
                APP_PRINT_INFO0("PROFILE_EVT_SEND_DATA_COMPLETE success");
            }
            else
            {
                APP_PRINT_ERROR0("PROFILE_EVT_SEND_DATA_COMPLETE failed");
            }
            break;

        default:
            break;
        }
    }

    return app_result;
}

static void gap_bwps_msg(T_IO_MSG *p_gap_msg)
{
    APP_PRINT_INFO1("line = %d \n", __LINE__);
    APP_PRINT_TRACE2("gap_bwps_msg: subtype %d,%d", p_gap_msg->type, p_gap_msg->subtype);
    T_LE_GAP_MSG gap_msg;
    memcpy(&gap_msg, &p_gap_msg->u.param, sizeof(p_gap_msg->u.param));
    switch (p_gap_msg->type)
    {
    case IO_MSG_TYPE_BT_STATUS:
        switch (p_gap_msg->subtype)
        {
        case GAP_MSG_LE_CONN_STATE_CHANGE:
            switch ((T_GAP_CONN_STATE)gap_msg.msg_data.gap_conn_state_change.new_state)
            {
            case GAP_CONN_STATE_DISCONNECTED:
                app_cluster_data_set_bluetooth_status(T_BLUETOOTH_INFO_OFF);
                app_cluster_data_set_show_main_display(0xff, BP_LE_DISC_EVENT);
                break;

            case GAP_CONN_STATE_CONNECTED:
                app_cluster_data_set_bluetooth_status(T_BLUETOOTH_INFO_ON);
                app_cluster_data_set_show_main_display(0xff, BP_LE_CONN_EVENT);
                break;

            default:
                break;
            }
            break;

        default:
            break;
        }
        break;

    default:
        break;
    }
}

static void app_bwps_msg(T_IO_MSG *p_gap_msg)
{
    APP_PRINT_INFO1("line = %d \n", __LINE__);
    APP_PRINT_TRACE2("app_bwps_msg: subtype %d,%d", p_gap_msg->type, p_gap_msg->subtype);
    switch (p_gap_msg->type)
    {
    case IO_MSG_TYPE_WRISTBNAD:
        switch (p_gap_msg->subtype)
        {
        case IO_MSG_PROTOCOL_SEND:
            {
                uint8_t *len = p_gap_msg->u.buf;
                //todo here con id todo
                wristband_send(0, len[0], (uint8_t *)p_gap_msg->u.buf + 1);
                free(p_gap_msg->u.buf);
            }
            break;
        case IO_MSG_ANCS_DISCOVERY:
            {
            }
            break;
        case IO_MSG_TYPE_AMS:
            {
            }
            break;
        case IO_MSG_UPDATE_CONPARA:
            {
            }
            break;
        case IO_MSG_SWITCH_IBEACON_ADV:
            {
                //update_ibeacon_adv_data();
            }
            break;
        case IO_MSG_SWITCH_NORMAL_ADV:
            {
                //update_normal_adv_data();
            }
            break;

        default:
            break;
        }
        break;

    default:
        break;
    }
}
void app_ble_bwps_init(void)
{
    APP_PRINT_TRACE0("app_ble_bwps_init");
    bwps_srv_id  = bwps_service_add_service(app_bwps_callback);
    le_msg_handler_cback_register(gap_bwps_msg);
    app_msg_handler_cback_register(app_bwps_msg);
}
