#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "miwear_api.h"
#include "miwear_config.h"
//#include "miwear_conn.h"
#include "miwear_type.h"
#include "pb_decode.h"
#include "wear.pb.h"
#include "wear_packet_encode.h"
#include "miwear_api.h"
//#include "miwear_bt_converter.h"
//#include "miwear_conn.h"
#include "miwear_type.h"
#include "gap_adv.h"
#include "rtthread.h"
#include <profile_server.h>
#include "miwear_service.h"
#include <gap.h>
#include <gap_adv.h>
#include <gap_bond_le.h>
#include <gap_conn_le.h>
#include "board.h"
#include "ble_task.h"
#include "ble_gap_init.h"
#include "ble_gap_msg.h"
#include "os_timer.h"

static T_SERVER_ID miwear_service_id;


static int encode_system_device_status(uint8_t *buffer, int len)
{
    DeviceStatus device_status = DeviceStatus_init_zero;
    DeviceStatus_Battery battery = DeviceStatus_Battery_init_zero;
    battery.capacity = 50;
    battery.charge_status = DeviceStatus_Battery_ChargeStatus_NOT_CHARGING;
    battery.has_charge_status = true;
    device_status.battery = battery;

    System system = System_init_zero;
    system.payload.device_status = device_status;
    system.which_payload = System_device_status_tag;

    int real_size = encode_wear_system_proto(System_SystemID_GET_DEVICE_STATUS,
                                             &system, buffer, len);
    return real_size;
}


static int encode_system_device_info(uint8_t *buffer, int len)
{
    DeviceInfo device_info = DeviceInfo_init_zero;

    device_info.serial_number = "4f35c793a7e7";
    device_info.firmware_version = "1.0.1";
    device_info.imei = "123456";
    device_info.model = "rs";

    System system = System_init_zero;
    system.payload.device_info = device_info;
    system.which_payload = System_device_info_tag;

    int real_size = encode_wear_system_proto(System_SystemID_GET_DEVICE_INFO,
                                             &system, buffer, len);
    return real_size;
}


static void miwear_rcvr_proto_data(uint16_t conn_handle,
                                   WearPacket *wear_packet)
{
    uint8_t output[128];
    int output_size = 0;
    rt_kprintf("%s, conn_handle = %d, receive proto packet id = %d, type = %d\n",
               __func__, conn_handle, wear_packet->id, wear_packet->type);
    switch (wear_packet->type)
    {
    case WearPacket_Type_SYSTEM:
        if (wear_packet->id == System_SystemID_GET_DEVICE_INFO)
        {
            output_size = encode_system_device_info(output, 128);
        }
        else if (wear_packet->id == System_SystemID_GET_DEVICE_STATUS)
        {
            output_size = encode_system_device_status(output, 128);
        }
        if (output_size == 0)
        {
            rt_kprintf("%s, no support or encode failed\n", __func__);
            return;
        }
        miwear_proto_data_send(output, output_size, 0, NULL);
        break;
    default:
        rt_kprintf("%s, no support\n", __func__);
        break;
    }
}
void *user_confirm_timer_handle;
#define TIMER_ID_USR_COMFIRM  60
uint16_t gs_conn_handle;

void user_confirm_timeout_handler(void *p_xtimer)
{
    uint32_t timer_id = 0;

    os_timer_id_get(&p_xtimer, &timer_id);

    rt_kprintf("=====user_confirm_timeout_handler, TimerID(%u)=====\r\n", timer_id);
    miwear_schd_user_confirm(gs_conn_handle);
}


extern miwear_status_t miwear_did_get(char *did_buf, uint8_t *p_did_len);
extern miwear_status_t miwear_hmac_sha256_with_psk(const uint8_t *plain_data, uint16_t plen,
                                                   uint8_t *hash_data);
void miwear_schd_event_handler(uint16_t conn_handle, miwear_schd_evt_t *p_event)
{
    char did[32] = {0};
    //uint8_t hmac[32];
    uint8_t len;
    rt_kprintf("miwear_schd_event_handler:id = %d\n", p_event->id);
    switch (p_event->id)
    {
    case MIWEAR_SCHD_EVT_REG_START:
        rt_kprintf("binding :%s\r\n", "MIWEAR_SCHD_EVT_REG_START");
        break;
    case MIWEAR_SCHD_EVT_REG_RESTART:
        rt_kprintf("binding :%s\r\n", "MIWEAR_SCHD_EVT_REG_RESTART");
        break;
    case MIWEAR_SCHD_EVT_REG_SUCCESS:
        rt_kprintf("binding :%s\r\n", "MIWEAR_SCHD_EVT_REG_SUCCESS");
        break;
    case MIWEAR_SCHD_EVT_USER_CONFIRM:
        //miwear_schd_user_confirm(conn_handle);
        gs_conn_handle = conn_handle;
        rt_kprintf("binding :%s\r\n", "MIWEAR_SCHD_EVT_USER_CONFIRM");
        os_timer_create(&user_confirm_timer_handle, "userConfirmTimer", TIMER_ID_USR_COMFIRM,
                        1000, false, user_confirm_timeout_handler);
        os_timer_start(&user_confirm_timer_handle);
        // miwear_schd_user_cancel(conn_handle);
        break;
    case MIWEAR_SCHD_EVT_REG_FAILED:
        rt_kprintf("reg fail reason: %d\n", p_event->data.fail_reason);
        break;
    case MIWEAR_SCHD_EVT_ADMIN_LOGIN_START:
        rt_kprintf("login :%s\r\n", "MIWEAR_SCHD_EVT_ADMIN_LOGIN_START");
        break;
    case MIWEAR_SCHD_EVT_ADMIN_LOGIN_RESTART:
        rt_kprintf("login :%s\r\n", "MIWEAR_SCHD_EVT_ADMIN_LOGIN_RESTART");
        break;
    case MIWEAR_SCHD_EVT_ADMIN_LOGIN_DEVICE:
        rt_kprintf("device type: %s\n",
                   p_event->data.device.device_type
                   == MIWEAR_COMPANION_DEVICE_ANDROID
                   ? "Android"
                   : "iOS");
        if (p_event->data.device.device_name)
        {
            rt_kprintf("device name: %s\n", p_event->data.device.device_name);
        }
        if (p_event->data.device.device_id)
        {
            rt_kprintf("device id: %s\n", p_event->data.device.device_id);
        }
        rt_kprintf("login :%s\r\n", "MIWEAR_SCHD_EVT_ADMIN_LOGIN_DEVICE");
        break;

    case MIWEAR_SCHD_EVT_ADMIN_LOGIN_SUCCESS:
        break;
    case MIWEAR_SCHD_EVT_ADMIN_LOGIN_FAILED:
        rt_kprintf("login fail reason: %d\n", p_event->data.fail_reason);
        break;
    case MIWEAR_SCHD_EVT_DID_REQ:
        rt_kprintf("MIWEAR_SCHD_EVT_DID_REQ\n");
        len = sizeof(did);
        miwear_did_get(did, &len);
        did[len] = '\0';
        miwear_schd_did_result(conn_handle, did, strlen(did));
        break;
    case MIWEAR_SCHD_EVT_PSK_HMAC_REQ:

//        miwear_hmac_sha256_with_psk(p_event->data.hmac_in, sizeof(p_event->data.hmac_in), hmac);
//        miwear_schd_psk_hmac_result(conn_handle, hmac);
        break;
    case MIWEAR_SCHD_EVT_BLE_BOND_APPLY:
        break;
    default:
        break;
    }
}

void miwear_write_handler(miwear_write_evt_t evt,
                          miwear_write_evt_param_t *param)
{
    rt_kprintf("miwear_write_handler line: %d\n", __LINE__);
    switch (evt)
    {
    case MIWEAR_WRITE_EVT_PROTO:
        miwear_rcvr_proto_data(param->conn_handle, param->proto_packet);
        break;
    default:
        break;
    }
}



static T_APP_RESULT app_miwear_service_callback(T_SERVER_ID service_id, void *p_data)
{
    if (service_id == miwear_service_id)
    {

    }
    return APP_RESULT_SUCCESS;
}

#include "ble_gap_msg.h"
#include "gap_msg.h"
#include "trace.h"

static T_GAP_CONN_STATE gap_conn_state = GAP_CONN_STATE_DISCONNECTED; /**< GAP connection state */
static void app_handle_conn_state_evt(uint8_t conn_id, T_GAP_CONN_STATE new_state,
                                      uint16_t disc_cause)
{
    APP_PRINT_INFO4("app_handle_conn_state_evt: conn_id %d old_state %d new_state %d, disc_cause 0x%x",
                    conn_id, gap_conn_state, new_state, disc_cause);
    switch (new_state)
    {
    case GAP_CONN_STATE_DISCONNECTED:
        {
            miwear_gap_disconnect_t gap_disconn;
            if (disc_cause == (HCI_ERR | HCI_ERR_REMOTE_USER_TERMINATE))
            {
                gap_disconn.reason = MIWEAR_REMOTE_USER_TERMINATED;
            }
            else if (disc_cause == (HCI_ERR | HCI_ERR_LOCAL_HOST_TERMINATE))
            {
                gap_disconn.reason = MIWEAR_LOCAL_HOST_TERMINATED;
            }
            else
            {
                gap_disconn.reason = MIWEAR_CONNECTION_TIMEOUT;
            }

            //bt_addr_to_miwear_addr(info.le.dst, gap_disconn.peer_addr);

            miwear_gap_evt_param_t disconn_param;
            disconn_param.conn_handle = conn_id;
            disconn_param.disconnect = gap_disconn;

            miwear_gap_event_callback(MIWEAR_GAP_EVT_DISCONNECT, &disconn_param);
        }
        break;

    case GAP_CONN_STATE_CONNECTED:
        {
            miwear_gap_conn_param_t gap_conn_param;
            le_get_conn_param(GAP_PARAM_CONN_INTERVAL, &gap_conn_param.min_conn_interval, conn_id);
            le_get_conn_param(GAP_PARAM_CONN_INTERVAL, &gap_conn_param.max_conn_interval, conn_id);
            le_get_conn_param(GAP_PARAM_CONN_LATENCY, &gap_conn_param.slave_latency, conn_id);
            le_get_conn_param(GAP_PARAM_CONN_TIMEOUT, &gap_conn_param.conn_sup_timeout, conn_id);

            uint8_t  remote_bd[6];
            T_GAP_REMOTE_ADDR_TYPE remote_bd_type;
            le_get_conn_addr(conn_id, remote_bd, (unsigned char *)&remote_bd_type);


            miwear_gap_connect_t gap_conn;

            gap_conn.conn_param = gap_conn_param;
            if (remote_bd_type == GAP_REMOTE_ADDR_LE_PUBLIC)
            {
                gap_conn.type = MIWEAR_ADDRESS_TYPE_PUBLIC;
            }
            else
            {
                gap_conn.type = MIWEAR_ADDRESS_TYPE_RANDOM;
            }

            gap_conn.role = MIWEAR_GAP_PERIPHERAL;
            memcpy(gap_conn.peer_addr, remote_bd, 6);


            miwear_gap_evt_param_t conn_param;
            conn_param.conn_handle = conn_id;
            conn_param.connect = gap_conn;

            miwear_gap_event_callback(MIWEAR_GAP_EVT_CONNECTED, &conn_param);
        }
        break;

    default:
        break;
    }
    gap_conn_state = new_state;
}

static void app_handle_conn_param_update_evt(uint8_t conn_id, uint8_t status, uint16_t cause)
{
    switch (status)
    {
    case GAP_CONN_PARAM_UPDATE_STATUS_SUCCESS:
        {
            APP_PRINT_INFO0("[MIWEAR_CONN_CB] miwear_conn_param_updated");
            uint16_t conn_interval;
            uint16_t conn_slave_latency;
            uint16_t conn_supervision_timeout;

            le_get_conn_param(GAP_PARAM_CONN_INTERVAL, &conn_interval, conn_id);
            le_get_conn_param(GAP_PARAM_CONN_LATENCY, &conn_slave_latency, conn_id);
            le_get_conn_param(GAP_PARAM_CONN_TIMEOUT, &conn_supervision_timeout, conn_id);
            miwear_gap_conn_param_t conn_param;
            conn_param.min_conn_interval = conn_interval;
            conn_param.max_conn_interval = conn_interval;
            conn_param.slave_latency = conn_slave_latency;
            conn_param.conn_sup_timeout = conn_supervision_timeout;

            miwear_gap_connect_update_t gap_update;
            gap_update.conn_param = conn_param;

            miwear_gap_evt_param_t update_param;
            update_param.conn_handle = conn_id;
            update_param.update_conn = gap_update;

            miwear_gap_event_callback(MIWEAR_GAP_EVT_CONN_PARAM_UPDATED, &update_param);
        }
        break;

    case GAP_CONN_PARAM_UPDATE_STATUS_FAIL:
        {
            APP_PRINT_ERROR1("app_handle_conn_param_update_evt update failed: cause 0x%x", cause);
        }
        break;

    case GAP_CONN_PARAM_UPDATE_STATUS_PENDING:
        {
            APP_PRINT_INFO0("app_handle_conn_param_update_evt update pending.");
        }
        break;

    default:
        break;
    }
}
void miwear_init(void);
static T_GAP_DEV_STATE gap_dev_state = {0, 0, 0, 0};                 /**< GAP device state */
static void app_handle_dev_state_evt(T_GAP_DEV_STATE new_state, uint16_t cause)
{
    APP_PRINT_INFO3("app_handle_dev_state_evt: init state %d, adv state %d, cause 0x%x",
                    new_state.gap_init_state, new_state.gap_adv_state, cause);
    if (gap_dev_state.gap_init_state != new_state.gap_init_state)
    {
        if (new_state.gap_init_state == GAP_INIT_STATE_STACK_READY)
        {
            APP_PRINT_INFO0("GAP stack ready");
            /*stack ready*/
            miwear_init();
        }
    }
    if (gap_dev_state.gap_adv_state != new_state.gap_adv_state)
    {
        if (new_state.gap_adv_state == GAP_ADV_STATE_IDLE)
        {
            if (new_state.gap_adv_sub_state == GAP_ADV_TO_IDLE_CAUSE_CONN)
            {
                APP_PRINT_INFO0("GAP adv stoped: because connection created");
            }
            else
            {
                APP_PRINT_INFO0("GAP adv stoped");
            }
        }
        else if (new_state.gap_adv_state == GAP_ADV_STATE_ADVERTISING)
        {
            APP_PRINT_INFO0("GAP adv start");
        }
    }

    gap_dev_state = new_state;
}

static void app_miwear_msg_cb(T_IO_MSG *msg)
{
    T_LE_GAP_MSG gap_msg;
    if (msg->type == IO_MSG_TYPE_BT_STATUS)
    {
        memcpy(&gap_msg, &msg->u.param, sizeof(msg->u.param));
        switch (msg->subtype)
        {
        case GAP_MSG_LE_DEV_STATE_CHANGE:
            app_handle_dev_state_evt(gap_msg.msg_data.gap_dev_state_change.new_state,
                                     gap_msg.msg_data.gap_dev_state_change.cause);
            break;
        case GAP_MSG_LE_CONN_STATE_CHANGE:
            {
                app_handle_conn_state_evt(gap_msg.msg_data.gap_conn_state_change.conn_id,
                                          (T_GAP_CONN_STATE)gap_msg.msg_data.gap_conn_state_change.new_state,
                                          gap_msg.msg_data.gap_conn_state_change.disc_cause);
            }
            break;
        case GAP_MSG_LE_CONN_PARAM_UPDATE:
            {
                app_handle_conn_param_update_evt(gap_msg.msg_data.gap_conn_param_update.conn_id,
                                                 gap_msg.msg_data.gap_conn_param_update.status,
                                                 gap_msg.msg_data.gap_conn_param_update.cause);
            }
            break;
        default:
            break;
        }
    }
}

void app_miwear_init(void)
{
    miwear_service_id = miwear_ble_service_add_service(app_miwear_service_callback);
    le_msg_handler_cback_register(app_miwear_msg_cb);
    rt_kprintf("miwear_service_id = %d \n", miwear_service_id);

}

void miwear_init(void)
{
    miwear_log_printf("%s, mi config miwear_log_level = %d, pid = %d\n, oob = %d, verify = %d",
                      __func__, miwear_config.miwear_log_level,
                      miwear_config.pid, miwear_config.oob_mode, miwear_config.verify_mode);

    miwear_bluetooth_init(miwear_write_handler);

    miwear_scheduler_init(miwear_schd_event_handler);

    miwear_arch_gatts_srv_init_cmp_t gatts_srv_init_param;

    int srv_num = 1;
    miwear_gatts_srv_handle_t p_srv_handle[srv_num];
    miwear_gatts_char_handle_t char_handle1[3];

    char_handle1[0].char_index = MIWEAR_MI_SERVICE_CHAR_VERSION;
    char_handle1[0].char_value_handle = server_get_start_handle(miwear_service_id) + 2;
    char_handle1[1].char_index = MIWEAR_MI_SERVICE_CHAR_PROTO_TX;
    char_handle1[1].char_value_handle = server_get_start_handle(miwear_service_id) + 5;
    char_handle1[2].char_index = MIWEAR_MI_SERVICE_CHAR_PROTO_RX;
    char_handle1[2].char_value_handle = server_get_start_handle(miwear_service_id) + 9;

    p_srv_handle[0].srv_index = MIWEAR_MI_SERVICE;
    p_srv_handle[0].srv_handle = server_get_start_handle(miwear_service_id);
    p_srv_handle[0].char_num = 3;
    p_srv_handle[0].p_char_handle = char_handle1;


    gatts_srv_init_param.srv_num = srv_num;
    gatts_srv_init_param.p_gatts_srv_handle = p_srv_handle;

    rt_kprintf("%s, miwear service char num(%d), version handle(%d), proto tx "
               "handle(%d), proto rx handle(%d)\n",
               __func__, p_srv_handle[0].char_num,
               p_srv_handle[0].p_char_handle[0].char_value_handle,
               p_srv_handle[0].p_char_handle[1].char_value_handle,
               p_srv_handle[0].p_char_handle[2].char_value_handle);


    gatts_srv_init_param.status = MIWEAR_SUCCESS;
    miwear_arch_evt_param_t arch_param;

    arch_param.srv_init_cmp = gatts_srv_init_param;

    miwear_arch_event_callback(MIWEAR_ARCH_EVT_GATTS_SRV_INIT_CMP, &arch_param);



    uint8_t adv_data[31] = {0};

    uint8_t scan_data[] =
    {
        21,           /* length     */
        GAP_ADTYPE_LOCAL_NAME_COMPLETE,
        'y', 'u', 'n', 'm', 'a', 'i', '.', 'j', 'm', 'p', 'r', 'o', 'p', 'e', '.', 's', 'r', '8', '0', '3',
    };

    adv_data[0] = 0x02;
    adv_data[1] = 0x01;
    adv_data[2] = 0x06;


    uint8_t srv_data[31];
    uint16_t srv_data_len = 31;


    mibeacon_mi_service_data_get(srv_data, &srv_data_len);

    rt_kprintf("%s - miwear srv_data_len = %d\n", __func__, srv_data_len);
    for (int i = 0; i < srv_data_len; i++)
    {
        rt_kprintf("%s - srv data, %d - 0x%02x\n", __func__, i, srv_data[i]);
    }

    memcpy(adv_data + 3, srv_data, srv_data_len);


    le_adv_set_param(GAP_PARAM_ADV_DATA, 3 + srv_data_len, adv_data);


    le_adv_set_param(GAP_PARAM_SCAN_RSP_DATA, sizeof(scan_data),
                     (void *)scan_data);

    le_adv_update_param();
}


MSH_CMD_EXPORT(miwear_init, le_scan test);
