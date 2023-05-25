/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <string.h>
#include "trace.h"
#include "console.h"
#include "gap_scan.h"
#include "gap_bond_le.h"
#include "app_timer.h"
#include "os_mem.h"
#include "gap.h"
#include "gap_br.h"
#include "audio.h"
#include "audio_probe.h"
#include "audio_volume.h"
#include "board.h"
#include "bt_hfp.h"
#include "bt_iap.h"
#include "btm.h"
#include "bt_bond.h"
#include "bt_types.h"
#include "bt_pbap.h"
#include "remote.h"
#include "stdlib.h"
#include "patch_header_check.h"
#include "fmc_api.h"
#include "test_mode.h"
#include "rtl_pinmux.h"
#include "app_cmd.h"
#include "app_transfer.h"
#include "app_report.h"
#include "app_bt_policy_api.h"
#include "app_cfg.h"
#include "app_eq.h"
#include "app_hfp.h"
#include "app_linkback.h"
#include "app_bond.h"
#include "app_bt_policy_int.h"
#include "wdg.h"
#include "gap_conn_le.h"
#include "gap_ext_scan.h"
#include "dp_ble_info.h"
#include "dp_br_info.h"
#include "gap_msg.h"

#if F_APP_CLI_BINARY_MP_SUPPORT
#include "mp_test.h"
#endif
#if F_APP_LISTENING_MODE_SUPPORT
#include "app_listening_mode.h"
#endif
#if F_APP_ANC_SUPPORT
#include "app_anc.h"
#endif
#if F_APP_APT_SUPPORT
#include "app_audio_passthrough.h"
#endif
#if F_APP_ADC_SUPPORT
#include "app_adc.h"
#endif
#if F_APP_SPP_CAPTURE_DSP_DATA
#include "app_sniff_mode.h"
#include "pm.h"
#endif
#if F_APP_BRIGHTNESS_SUPPORT
#include "app_audio_passthrough_brightness.h"
#endif
#if F_APP_PBAP_CMD_SUPPORT
#include "app_pbap.h"
#endif
#if F_APP_DUAL_AUDIO_EFFECT
#include "app_dual_audio_effect.h"
#endif
#if F_APP_ONE_WIRE_UART_SUPPORT
#include "app_one_wire_uart.h"
#endif

/* BBPro2 specialized feature */
#if F_APP_LOCAL_PLAYBACK_SUPPORT
#include "app_playback_update_file.h"
#endif
#if F_APP_HEARABLE_SUPPORT
#include "ftl.h"
#include "app_hearable.h"
#endif

#if F_APP_CAP_TOUCH_SUPPORT
#include "app_cap_touch.h"
#endif

// end of BBPro2 specialized feature

#if F_APP_SS_SUPPORT
#include "app_ss_cmd.h"
#endif

#include "app_bt_policy_api.h"

#include "app_bt_policy_api.h"


#if F_APP_BLE_AMS_CLIENT_SUPPORT
#include "ams.h"
#endif

#if F_APP_BLE_ANCS_CLIENT_SUPPORT
#include "ancs.h"
#endif

#if DATA_CAPTURE_V2_SUPPORT
#include "app_data_capture.h"
#endif

#include "gatt_builtin_services.h"


/* Define application support status */
#define SNK_SUPPORT_GET_SET_LE_NAME                 1
#define SNK_SUPPORT_GET_SET_BR_NAME                 1
#define SNK_SUPPORT_GET_SET_VP_LANGUAGE             1
#define SNK_SUPPORT_GET_BATTERY_LEVEL               1
#define SNK_SUPPORT_GET_SET_KEY_REMAP               F_APP_KEY_EXTEND_FEATURE

//for CMD_INFO_REQ
#define CMD_INFO_STATUS_VALID       0x00
#define CMD_INFO_STATUS_ERROR       0x01
#define CMD_SUPPORT_VER_CHECK_LEN   3

//for CMD parse
#define IQS773_CALI_WORK_MODE_SET   0
#define IQS773_CALI_SENSITIVITY_SET 1

//for CMD_LINE_IN_CTRL
#define CFG_LINE_IN_STOP            0x00
#define CFG_LINE_IN_START           0x01

//for CMD_GET_FLASH_DATA and EVENT_REPORT_FLASH_DATA
#define START_TRANS                 0x00
#define CONTINUE_TRANS              0x01
#define SUPPORT_IMAGE_TYPE          0x02

#define TRANS_DATA_INFO             0x00
#define CONTINUE_TRANS_DATA         0x01
#define END_TRANS_DATA              0x02
#define SUPPORT_IMAGE_TYPE_INFO     0x03

#define SYSTEM_CONFIGS              0x00
#define ROM_PATCH_IMAGE             0x01
#define APP_IMAGE                   0x02
#define DSP_SYSTEM_IMAGE            0x03
#define DSP_APP_IMAGE               0x04
#define FTL_DATA                    0x05
#define ANC_IMAGE                   0x06
#define LOG_PARTITION               0x07
#define CORE_DUMP_PARTITION         0x08

#define FLASH_ALL                   0xFF
#define ALL_DUMP_IMAGE_MASK         ((0x01 << SYSTEM_CONFIGS) | (0x01 << ROM_PATCH_IMAGE) | (0x01 << APP_IMAGE) \
                                     | (0x01 << DSP_SYSTEM_IMAGE) | (0x01 << DSP_APP_IMAGE) \
                                     | (0x01 << FTL_DATA) |(0x01 << CORE_DUMP_PARTITION))

/* BBPro2 specialized feature */
//for CMD_DSP_DEBUG_SIGNAL_IN
#define CFG_SET_DSP_DEBUG_SIGNAL_IN     0x71
#define DSP_DEBUG_SIGNAL_IN_PAYLOAD_LEN 16
// end of BBPro2 specialized feature

#define CLEAR_BOND_INFO_SUCCESS     0x00
#define CLEAR_BOND_INFO_FAIL        0x01

#define DEVICE_BUD_SIDE_BOTH        2

#if F_APP_DEVICE_CMD_SUPPORT
static bool enable_auto_reject_conn_req_flag = false;
static bool enable_auto_accept_conn_req_flag = true;
static bool enable_report_attr_info_flag = false;
#endif

#if F_APP_SPP_CAPTURE_DSP_DATA
static uint8_t dsp_capture_data_master_retry = 0;
static uint8_t *dsp_spp_capture_cmd_ptr;
static uint8_t dsp_spp_capture_cmd_ptr_len;
static uint8_t dsp_spp_capture_app_idx;
static uint8_t dsp_capture_data_state = 0;

T_AUDIO_TRACK_HANDLE dsp_spp_capture_audio_track_handle;
#endif

static uint8_t uart_rx_seqn = 0; // uart receive sequence number
static uint8_t dlps_status = SET_DLPS_ENABLE;

static T_SRC_SUPPORT_VER_FORMAT src_support_version_br_link[MAX_BR_LINK_NUM];
static T_SRC_SUPPORT_VER_FORMAT src_support_version_le_link[MAX_BLE_LINK_NUM];
static T_SRC_SUPPORT_VER_FORMAT src_support_version_uart;

static T_OS_QUEUE cmd_parse_cback_list;

T_FLASH_DATA flash_data;
extern T_LINKBACK_ACTIVE_NODE linkback_active_node;

// for get FW version type
typedef enum
{
    GET_PRIMARY_FW_VERSION           = 0x00,
    GET_SECONDARY_FW_VERSION         = 0x01,
    GET_PRIMARY_OTA_FW_VERSION       = 0x02,
    GET_SECONDARY_OTA_FW_VERSION     = 0x03,
} T_CMD_GET_FW_VER_TYPE;

typedef enum
{
    APP_TIMER_SWITCH_TO_HCI_DOWNLOAD_MODE,
    APP_TIMER_ENTER_DUT_FROM_SPP_WAIT_ACK,
    APP_TIMER_OTA_JIG_DELAY_POWER_OFF,
    APP_TIMER_OTA_JIG_DELAY_WDG_RESET,
    APP_TIMER_OTA_JIG_DLPS_ENABLE,
    APP_TIMER_DSP_SPP_CAPTRUE_CHECK_LINK,
    APP_TIMER_IO_PIN_PULL_HIGH,
    APP_TIMER_STOP_PERIODIC_INQUIRY,
} T_APP_CMD_TIMER;

typedef enum
{
    IMU_SENSOR_DATA_START_REPORT = 2,
    IMU_SENSOR_DATA_STOP_REPORT = 3,
} T_CMD_IMU_SENSOR_DATA;

static uint8_t app_cmd_timer_id = 0;
static uint8_t timer_idx_switch_to_hci_mode = 0;
static uint8_t timer_idx_enter_dut_from_spp_wait_ack = 0;
static uint8_t timer_idx_io_pin_pull_high = 0;

#if F_APP_SPP_CAPTURE_DSP_DATA
static uint8_t timer_idx_dsp_spp_captrue_check_link = 0;
#endif

#if F_APP_OTA_TOOLING_SUPPORT
static uint8_t timer_idx_ota_parking_power_off = 0;
static uint8_t timer_idx_ota_parking_wdg_reset = 0;
static uint8_t timer_idx_ota_parking_dlps_enable = 0;
#endif


static void app_cmd_handle_remote_cmd(uint16_t msg, void *buf, uint8_t len);

/**
    * @brief  get image size of bank area
    * @param  image_id image ID
    * @return size
    */
static uint32_t get_bank_size_by_img_id(IMG_ID image_id)
{
    uint32_t bank_size = 0;

    return bank_size;
}

#if F_APP_DEVICE_CMD_SUPPORT
bool app_cmd_get_auto_reject_conn_req_flag(void)
{
    return enable_auto_reject_conn_req_flag;
}

bool app_cmd_get_auto_accept_conn_req_flag(void)
{
    return enable_auto_accept_conn_req_flag;
}

bool app_cmd_get_report_attr_info_flag(void)
{
    return enable_report_attr_info_flag;
}

void app_cmd_set_report_attr_info_flag(bool flag)
{
    enable_report_attr_info_flag = flag;
}
#endif

#if F_APP_SPP_CAPTURE_DSP_DATA
uint8_t app_cmd_dsp_capture_data_state(void)
{
    return dsp_capture_data_state;
}

bool app_cmd_spp_capture_executing_check(void)
{
    return (dsp_capture_data_state & (DSP_CAPTURE_DATA_LOG_EXECUTING | DSP_CAPTURE_RAW_DATA_EXECUTING))
           ? true : false;
}

#if F_APP_TEST_SUPPORT
bool app_cmd_spp_capture_audio_dsp_ctrl_send_handler(uint8_t *cmd_ptr, uint16_t cmd_len,
                                                     uint8_t cmd_path, uint8_t app_idx, uint8_t *ack_pkt, bool send_flag)
{
    bool send_cmd_flag = send_flag;
    uint32_t actual_mhz;
    uint16_t h2d_cmd_id = (uint16_t)(cmd_ptr[2] | (cmd_ptr[3] << 8));
    APP_PRINT_TRACE3("CMD_AUDIO_DSP_CTRL_SEND %x %x %x", cmd_ptr[2], h2d_cmd_id,
                     app_cfg_const.enable_dsp_capture_data_by_spp);

    //for capture dsp data handle stop cmd
    if (app_cfg_const.enable_dsp_capture_data_by_spp)
    {
        if (cmd_ptr[2] == H2D_CMD_DSP_DAC_ADC_DATA_TO_MCU)
        {
            uint8_t i = 10; //begin of para 1
            uint8_t end = 0;
            dsp_capture_data_state &= ~(DSP_CAPTURE_DATA_START_MASK | DSP_CAPTURE_DATA_SWAP_TO_MASTER);

            if (cmd_ptr[4] == VENDOR_SPP_CAPTURE_DSP_LOG)
            {
                end = 13;//end of para 2
            }
            else if (cmd_ptr[4] == VENDOR_SPP_CAPTURE_DSP_RWA_DATA)
            {
                end = 17; //end of para 3
            }
            APP_PRINT_TRACE1("CMD_AUDIO_DSP_CTRL_SEND %b", TRACE_BINARY(end, cmd_ptr));

            while (i <= end)
            {
                if (cmd_ptr[i] != 0)
                {
                    dsp_capture_data_state |= DSP_CAPTURE_DATA_START_MASK;
                    break;
                }
                i++;
            }

            if (dsp_capture_data_state & DSP_CAPTURE_DATA_START_MASK)
            {
                memset(&(g_vendor_spp_data), 0, sizeof(T_VENDOR_SPP_DATA));
                g_vendor_spp_data.spp_ptr = malloc(VENDOR_SPP_ALLOCATE_SIZE);
                g_temp_pkt = malloc(VENDOR_SPP_MAX_SIZE);
                send_cmd_flag = false;
                dsp_capture_data_state |= DSP_CAPTURE_DATA_SWAP_TO_MASTER;
                dsp_capture_data_state |= (cmd_ptr[4] == VENDOR_SPP_CAPTURE_DSP_LOG) ?
                                          DSP_CAPTURE_DATA_LOG_EXECUTING : DSP_CAPTURE_RAW_DATA_EXECUTING;
                app_dlps_disable(APP_DLPS_ENTER_CHECK_SPP_CAPTURE);
                app_sniff_mode_b2s_disable_all(SNIFF_DISABLE_MASK_SPP_CAPTURE);
                gap_br_vendor_data_rate_set(0); // force 3M not support yet, 0 : 2M/3M

                bt_acl_pkt_type_set(app_db.br_link[app_idx].bd_addr, BT_ACL_PKT_TYPE_3M);
                dsp_capture_data_state &= ~DSP_CAPTURE_DATA_START_MASK;
                dsp_spp_capture_cmd_ptr = os_mem_alloc(RAM_TYPE_DATA_OFF, cmd_len + 2);
                dsp_spp_capture_cmd_ptr_len = cmd_len - 2;
                memcpy(dsp_spp_capture_cmd_ptr, &cmd_ptr[2], dsp_spp_capture_cmd_ptr_len);

                pm_cpu_freq_set(100, &actual_mhz);
                bt_link_qos_set(app_db.br_link[app_idx].bd_addr, BT_QOS_TYPE_GUARANTEED, 6);
                app_test_dsp_capture_data_set_param(app_idx);
                app_bt_policy_abandon_engage();

                if (app_hfp_get_call_status() ||
                    ((app_db.br_link[app_idx].acl_link_role == BT_LINK_ROLE_MASTER)))
                {
                    // send H2D_SPPCAPTURE_SET cmd
                    audio_probe_dsp_send(dsp_spp_capture_cmd_ptr, dsp_spp_capture_cmd_ptr_len);
                    free(dsp_spp_capture_cmd_ptr);
                }
                else
                {
                    bt_link_role_switch(app_db.br_link[app_idx].bd_addr, true);
                }
            }
            else
            {
                // reset to default
                dsp_capture_data_state &= ~((cmd_ptr[4] == VENDOR_SPP_CAPTURE_DSP_LOG) ?
                                            DSP_CAPTURE_DATA_LOG_EXECUTING : DSP_CAPTURE_RAW_DATA_EXECUTING);

                if ((dsp_capture_data_state & (DSP_CAPTURE_DATA_LOG_EXECUTING | DSP_CAPTURE_RAW_DATA_EXECUTING)) ==
                    0)
                {
                    app_dlps_enable(APP_DLPS_ENTER_CHECK_SPP_CAPTURE);
                    app_sniff_mode_b2s_enable_all(SNIFF_DISABLE_MASK_SPP_CAPTURE);
                    bt_acl_pkt_type_set(app_db.br_link[app_idx].bd_addr, BT_ACL_PKT_TYPE_2M);
                }

                pm_cpu_freq_set(40, &actual_mhz);
                bt_link_role_switch(app_db.br_link[app_idx].bd_addr, false);
                bt_link_qos_set(app_db.br_link[app_idx].bd_addr, BT_QOS_TYPE_GUARANTEED, 40);
            }
        }
        else if (h2d_cmd_id == H2D_SPPCAPTURE_SET)
        {
            uint32_t plan_profs;
            uint32_t bond_flag;
            dsp_spp_capture_app_idx = app_idx;

            if (cmd_ptr[6] == CHANGE_MODE_TO_SCO)
            {
                send_cmd_flag = false;
                dsp_spp_capture_cmd_ptr = os_mem_alloc(RAM_TYPE_DATA_OFF, cmd_len + 2);
                dsp_spp_capture_cmd_ptr_len = cmd_len - 2;
                memcpy(dsp_spp_capture_cmd_ptr, &cmd_ptr[2], dsp_spp_capture_cmd_ptr_len);
                dsp_capture_data_state |= DSP_CAPTURE_DATA_CHANGE_MODE_TO_SCO_MASK;
                plan_profs = (app_db.br_link[app_idx].connected_profile & (~RDTP_PROFILE_MASK) &
                              (~SPP_PROFILE_MASK));

                if (plan_profs)
                {
                    app_bt_policy_disconnect(app_db.br_link[app_idx].bd_addr, plan_profs);
                }

                app_start_timer(&timer_idx_dsp_spp_captrue_check_link, "dsp_spp_captrue_check_link",
                                app_cmd_timer_id, APP_TIMER_DSP_SPP_CAPTRUE_CHECK_LINK, app_idx, false,
                                1500);
            }
            else if (cmd_ptr[6] == CHANGE_MODE_EXIST)
            {
                if (dsp_spp_capture_audio_track_handle != NULL)
                {
                    audio_track_release(dsp_spp_capture_audio_track_handle);
                }
                dsp_capture_data_state &= ~DSP_CAPTURE_DATA_CHANGE_MODE_TO_SCO_MASK;

                bt_bond_flag_get(app_db.br_link[app_idx].bd_addr, &bond_flag);
                if (bond_flag & (BOND_FLAG_HFP | BOND_FLAG_HSP | BOND_FLAG_A2DP))
                {
                    plan_profs = get_profs_by_bond_flag(bond_flag);
                    app_bt_policy_default_connect(app_db.br_link[app_idx].bd_addr, plan_profs, false);
                }
            }
        }
    }

    return send_cmd_flag;
}
#endif
#endif

static bool app_cmd_distribute_payload(uint8_t *buf, uint16_t len)
{
    uint8_t module_type = buf[0];
    uint8_t msg_type    = buf[1];
    T_APP_CMD_PARSE_CBACK_ITEM *p_item;

    p_item = (T_APP_CMD_PARSE_CBACK_ITEM *)cmd_parse_cback_list.p_first;

    while (p_item != NULL)
    {
        if (p_item->module_type == module_type)
        {
            p_item->parse_cback(msg_type, buf + 2, len - 2);

            return true;
        }

        p_item = p_item->p_next;
    }

    return false;
}

static bool app_cmd_compose_payload(uint8_t *data, uint16_t data_len)
{
    static uint8_t cur_seq = 0;
    static uint8_t *buf = NULL;
    static uint16_t buf_offset = 0;

    uint8_t type = data[1];
    uint8_t seq = data[2];
    uint16_t total_len = data[3] + (data[4] << 8);

    data += 5;
    data_len -= 5;

    if (cur_seq != seq || data_len == 0)
    {
        cur_seq = 0;
        buf_offset = 0;

        if (buf)
        {
            free(buf);
            buf = NULL;
        }

        return CMD_SET_STATUS_PROCESS_FAIL;
    }

    if (type == PKT_TYPE_SINGLE || type == PKT_TYPE_START)
    {
        cur_seq = 0;
        buf_offset = 0;

        if (buf)
        {
            free(buf);
            buf = NULL;
        }

        buf = malloc(sizeof(uint8_t) * total_len);
    }

    memcpy(buf + buf_offset, data, data_len);

    if (type == PKT_TYPE_SINGLE || type == PKT_TYPE_END)
    {
        app_cmd_distribute_payload(buf, total_len);
        free(buf);
        buf = NULL;
        cur_seq = 0;
        buf_offset = 0;
    }
    else
    {
        cur_seq += 1;
        buf_offset += data_len;
    }

    return CMD_SET_STATUS_COMPLETE;
}

static void app_cmd_bt_event_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
#if (F_APP_SPP_CAPTURE_DSP_DATA == 1)
    T_BT_EVENT_PARAM *param = event_buf;
#endif
    bool handle = true;

    switch (event_type)
    {
#if F_APP_SPP_CAPTURE_DSP_DATA
    case BT_EVENT_ACL_ROLE_MASTER:
        {
            if (dsp_capture_data_state & DSP_CAPTURE_DATA_SWAP_TO_MASTER)
            {
                audio_probe_dsp_send(dsp_spp_capture_cmd_ptr, dsp_spp_capture_cmd_ptr_len);
                dsp_capture_data_state |= DSP_CAPTURE_DATA_START_MASK;
                free(dsp_spp_capture_cmd_ptr);
            }
        }
        break;

    case BT_EVENT_ACL_ROLE_SWITCH_FAIL:
        {
            if (dsp_capture_data_state & DSP_CAPTURE_DATA_SWAP_TO_MASTER)
            {
                if (dsp_capture_data_master_retry < 3)
                {
                    bt_link_role_switch(param->acl_role_switch_fail.bd_addr, true);
                    dsp_capture_data_master_retry++;
                }
                else
                {
                    audio_probe_dsp_send(dsp_spp_capture_cmd_ptr, dsp_spp_capture_cmd_ptr_len);
                    dsp_capture_data_state |= DSP_CAPTURE_DATA_START_MASK;
                    dsp_capture_data_state &= ~DSP_CAPTURE_DATA_SWAP_TO_MASTER;
                    dsp_capture_data_master_retry = 0;
                    free(dsp_spp_capture_cmd_ptr);
                }
            }
        }
        break;
#endif

    case BT_EVENT_ACL_CONN_IND:
        {
            gap_br_stop_inquiry();
        }
        break;

    default:
        handle = false;
        break;
    }

    if (handle == true)
    {
        APP_PRINT_INFO1("app_cmd_bt_cback: event_type 0x%04x", event_type);
    }
}

static void app_cmd_audio_cback(T_AUDIO_EVENT event_type, void *event_buf, uint16_t buf_len)
{
#if F_APP_SPP_CAPTURE_DSP_DATA
    T_AUDIO_EVENT_PARAM *param = event_buf;
#endif
    bool handle = true;

    switch (event_type)
    {
#if F_APP_SPP_CAPTURE_DSP_DATA
    case AUDIO_EVENT_TRACK_STATE_CHANGED:
        {
            if (param->track_state_changed.handle == dsp_spp_capture_audio_track_handle)
            {
                APP_PRINT_ERROR1("track_state, %x", param->track_state_changed.state);

                if (param->track_state_changed.state == AUDIO_TRACK_STATE_STARTED)
                {
                    uint8_t send_pkt_num = 5;
                    uint8_t seq_num;
                    uint8_t p_audio_buf[4] = {0, 0, 0, 0};
                    uint8_t buf[4] = {0, 0, 0, 0};

                    for (seq_num = 0; seq_num < send_pkt_num; seq_num++)
                    {
                        uint16_t written_len;
                        audio_track_write(dsp_spp_capture_audio_track_handle,
                                          0,//              timestamp,
                                          seq_num,
                                          AUDIO_STREAM_STATUS_CORRECT,
                                          1,//            frame_num,
                                          (uint8_t *)p_audio_buf,
                                          sizeof(p_audio_buf),
                                          &written_len);
                    }

                    dsp_capture_data_state |= DSP_CAPTURE_DATA_ENTER_SCO_MODE_MASK;

                    // send H2D_SPPCAPTURE_SET cmd
                    audio_probe_dsp_send(dsp_spp_capture_cmd_ptr, dsp_spp_capture_cmd_ptr_len);
                    buf[0] = 1;
                    app_report_event(CMD_PATH_SPP, EVENT_AUDIO_DSP_CTRL_INFO, dsp_spp_capture_app_idx, buf, 4);
                    free(dsp_spp_capture_cmd_ptr);
                }

                if (param->track_state_changed.state == AUDIO_TRACK_STATE_RELEASED)
                {
                    dsp_spp_capture_audio_track_handle = NULL;
                    dsp_capture_data_state &= ~DSP_CAPTURE_DATA_ENTER_SCO_MODE_MASK;
                }
            }
        }
        break;

    case AUDIO_EVENT_TRACK_DATA_IND:
        {
            if (remote_session_role_get() != REMOTE_SESSION_ROLE_SECONDARY)
            {
                if (param->track_state_changed.handle == dsp_spp_capture_audio_track_handle)
                {
                    T_APP_BR_LINK *p_link;
                    p_link = app_find_br_link(app_db.br_link[dsp_spp_capture_app_idx].bd_addr);
                    if (p_link == NULL)
                    {
                        break;
                    }

                    APP_PRINT_TRACE1("app_cmd_audio_cback: data ind len %u", param->track_data_ind.len);
                    uint32_t timestamp;
                    uint16_t seq_num;
                    uint8_t frame_num;
                    uint16_t read_len;
                    T_AUDIO_STREAM_STATUS status;
                    uint8_t *buf;

                    buf = malloc(param->track_data_ind.len);

                    if (buf == NULL)
                    {
                        return;
                    }

                    if (audio_track_read(dsp_spp_capture_audio_track_handle,
                                         &timestamp,
                                         &seq_num,
                                         &status,
                                         &frame_num,
                                         buf,
                                         param->track_data_ind.len,
                                         &read_len) == true)
                    {
                        if (p_link->duplicate_fst_sco_data)
                        {
                            p_link->duplicate_fst_sco_data = false;
                            bt_sco_data_send(app_db.br_link[dsp_spp_capture_app_idx].bd_addr, seq_num - 1, buf, read_len);
                        }
                        bt_sco_data_send(app_db.br_link[dsp_spp_capture_app_idx].bd_addr, seq_num, buf, read_len);
                    }

                    free(buf);
                }
            }
        }
        break;
#endif

    default:
        handle = false;
        break;
    }

    if (handle == true)
    {
        APP_PRINT_INFO1("app_cmd_audio_cback: event_type 0x%04x", event_type);
    }
}

static void app_cmd_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    switch (timer_evt)
    {
    case APP_TIMER_SWITCH_TO_HCI_DOWNLOAD_MODE:
        {
            app_stop_timer(&timer_idx_switch_to_hci_mode);
            sys_hall_set_hci_download_mode(true);
            set_hci_mode_flag(true);
            chip_reset(RESET_ALL_EXCEPT_AON);
        }
        break;

    case APP_TIMER_ENTER_DUT_FROM_SPP_WAIT_ACK:
        {

        }
        break;

#if (F_APP_SPP_CAPTURE_DSP_DATA == 1)
    case APP_TIMER_DSP_SPP_CAPTRUE_CHECK_LINK:
        {
            app_stop_timer(&timer_idx_dsp_spp_captrue_check_link);

            if ((app_db.br_link[param].connected_profile & (~RDTP_PROFILE_MASK) & (~SPP_PROFILE_MASK)))
            {
                app_start_timer(&timer_idx_dsp_spp_captrue_check_link, "dsp_spp_captrue_check_link",
                                app_cmd_timer_id, APP_TIMER_DSP_SPP_CAPTRUE_CHECK_LINK, param, false,
                                1500);
            }
            else
            {
                T_AUDIO_FORMAT_INFO format_info;

                format_info.type = AUDIO_FORMAT_TYPE_MSBC;
                format_info.attr.msbc.chann_mode = AUDIO_SBC_CHANNEL_MODE_MONO;
                format_info.attr.msbc.sample_rate = 16000;
                format_info.attr.msbc.bitpool = 26;
                format_info.attr.msbc.allocation_method = 0;
                format_info.attr.msbc.subband_num = 8;
                format_info.attr.msbc.block_length = 15;

                if (dsp_spp_capture_audio_track_handle)
                {
                    audio_track_release(dsp_spp_capture_audio_track_handle);
                }

                dsp_spp_capture_audio_track_handle = audio_track_create(AUDIO_STREAM_TYPE_VOICE,
                                                                        AUDIO_STREAM_MODE_NORMAL,
                                                                        AUDIO_STREAM_USAGE_LOCAL,
                                                                        format_info,
                                                                        app_cfg_const.voice_out_volume_default,
                                                                        app_cfg_const.voice_volume_in_default,
                                                                        AUDIO_DEVICE_OUT_DEFAULT | AUDIO_DEVICE_IN_DEFAULT,
                                                                        NULL,
                                                                        NULL);

                audio_track_latency_set(dsp_spp_capture_audio_track_handle, 15, false);
                audio_track_start(dsp_spp_capture_audio_track_handle);
            }
        }
        break;
#endif

#if (F_APP_OTA_TOOLING_SUPPORT == 1)
    case APP_TIMER_OTA_JIG_DELAY_POWER_OFF:
        {
            app_stop_timer(&timer_idx_ota_parking_power_off);

            if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
            {
                app_relay_async_single(APP_MODULE_TYPE_OTA, APP_REMOTE_MSG_OTA_PARKING);
            }
            else
            {
                app_cmd_ota_tooling_parking();
            }
        }
        break;

    case APP_TIMER_OTA_JIG_DELAY_WDG_RESET:
        {
            app_stop_timer(&timer_idx_ota_parking_wdg_reset);
            chip_reset(RESET_ALL);
        }
        break;

    case APP_TIMER_OTA_JIG_DLPS_ENABLE:
        {
            app_stop_timer(&timer_idx_ota_parking_dlps_enable);
            app_dlps_enable(APP_DLPS_ENTER_CHECK_OTA_TOOLING_PARK);
        }
        break;
#endif

    case APP_TIMER_IO_PIN_PULL_HIGH:
        {
            app_stop_timer(&timer_idx_io_pin_pull_high);

            uint8_t pin_num = (uint8_t)param;

            Pad_Config(pin_num, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_HIGH);
        }
        break;

#if F_APP_DEVICE_CMD_SUPPORT
    case APP_TIMER_STOP_PERIODIC_INQUIRY:
        {
            bt_periodic_inquiry_stop();
        }
        break;
#endif

    default:
        break;
    }
}

uint16_t app_cmd_relay_cback(uint8_t *buf, uint8_t msg_type, bool total)
{
    return 0;
}

static void app_cmd_parse_cback(uint8_t msg_type, uint8_t *buf, uint16_t len,
                                T_REMOTE_RELAY_STATUS status)
{
    switch (msg_type)
    {
    case APP_REMOTE_MSG_CMD_GET_FW_VERSION:
    case APP_REMOTE_MSG_CMD_REPORT_FW_VERSION:
    case APP_REMOTE_MSG_CMD_GET_OTA_FW_VERSION:
    case APP_REMOTE_MSG_CMD_REPORT_OTA_FW_VERSION:
        {
            app_cmd_handle_remote_cmd(msg_type, buf, len);
        }
        break;

    case APP_REMOTE_MSG_SYNC_EQ_CTRL_BY_SRC:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                app_cmd_update_eq_ctrl(*((uint8_t *)buf), false);
            }
        }
        break;

    /* BBPro2 specialized feature*/
    case APP_REMOTE_MSG_DSP_DEBUG_SIGNAL_IN_SYNC:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                audio_probe_dsp_send(buf, len);
            }
        }
        break;
    // end of BBPro2 specialized feature

    case APP_REMOTE_MSG_SYNC_RAW_PAYLOAD:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                app_cmd_compose_payload(buf, len);
            }
        }
        break;

    default:
        break;
    }
}

void app_cmd_init(void)
{

}

bool app_cmd_cback_register(P_APP_CMD_PARSE_CBACK parse_cb,
                            T_APP_CMD_MODULE_TYPE module_type, uint8_t msg_max)
{
    return false;
}

void app_cmd_set_event_broadcast(uint16_t event_id, uint8_t *buf, uint16_t len)
{
    T_APP_BR_LINK *br_link;
    T_APP_LE_LINK *le_link;
    uint8_t        i;

    for (i = 0; i < MAX_BR_LINK_NUM; i ++)
    {
        br_link = &br_db.br_link[i];

        if (br_link->cmd_set_enable == true)
        {
            if (br_link->connected_profile & SPP_PROFILE_MASK)
            {
                app_report_event(CMD_PATH_SPP, event_id, i, buf, len);
            }

            if (br_link->connected_profile & IAP_PROFILE_MASK)
            {
                app_report_event(CMD_PATH_IAP, event_id, i, buf, len);
            }
        }
    }

    for (i = 0; i < MAX_BLE_LINK_NUM; i++)
    {
        le_link = &le_db.le_link[i];

        if (le_link->state == GAP_CONN_STATE_CONNECTED)
        {
            if (le_link->cmd_set_enable == true)
            {
                app_report_event(CMD_PATH_LE, event_id, i, buf, len);
            }
        }
    }
}

void app_read_flash(uint32_t start_addr, uint8_t cmd_path, uint8_t app_idx)
{

}

//T_FLASH_DATA initialization
void app_flash_data_set_param(uint8_t flash_type, uint8_t cmd_path, uint8_t app_idx)
{

}

static void app_cmd_get_fw_version(uint8_t *p_data)
{

}

#if F_APP_OTA_TOOLING_SUPPORT
void app_cmd_ota_tooling_parking(void)
{
    APP_PRINT_INFO2("app_cmd_ota_tooling_parking %d, %d", app_cfg_nv.ota_tooling_start,
                    app_db.device_state);

    app_dlps_disable(APP_DLPS_ENTER_CHECK_OTA_TOOLING_PARK);
    app_start_timer(&timer_idx_ota_parking_dlps_enable, "ota_parking_dlps_enable",
                    app_cmd_timer_id, APP_TIMER_OTA_JIG_DLPS_ENABLE, NULL, false,
                    3500);

    // clear phone record
    app_bond_clear_non_rws_keys();

    // clear dongle info
    app_db.jig_subcmd = 0;
    app_db.jig_dongle_id = 0;
    app_db.ota_tooling_start = 0;

    // remove OTA power on flag
    if (app_cfg_nv.ota_tooling_start)
    {
        app_cfg_nv.ota_tooling_start = 0;
        app_cfg_store(&app_cfg_nv.eq_idx_anc_mode_record, 4);
    }

    // power off
    if (app_db.device_state == APP_DEVICE_STATE_ON)
    {
        app_db.power_off_cause = POWER_OFF_CAUSE_OTA_TOOL;
        app_mmi_handle_action(MMI_DEV_POWER_OFF);
        app_start_timer(&timer_idx_ota_parking_wdg_reset, "ota_jig_delay_wdg_reset",
                        app_cmd_timer_id, APP_TIMER_OTA_JIG_DELAY_WDG_RESET, NULL, false,
                        3000);
    }
}

void app_cmd_stop_ota_parking_power_off(void)
{
    // avoid timeout to clear ota dongle info
    app_stop_timer(&timer_idx_ota_parking_power_off);

    // avoid timeout to reset system when receive new dongle command
    app_stop_timer(&timer_idx_ota_parking_wdg_reset);

    // clear set dlps
    app_cfg_nv.need_set_lps_mode = 0;
}
#endif

void app_cmd_set_event_ack(uint8_t cmd_path, uint8_t app_idx, uint8_t *buf)
{
    app_report_event(cmd_path, EVENT_ACK, app_idx, buf, 3);
}

static void app_cmd_handle_remote_cmd(uint16_t msg, void *buf, uint8_t len)
{
    switch (msg)
    {
#if (F_APP_OTA_TOOLING_SUPPORT == 1)
    case APP_REMOTE_MSG_CMD_GET_FW_VERSION:
        {
            uint8_t *p_info = (uint8_t *)buf;

            // get secondary's fw version by app_cmd_get_fw_version()
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                uint8_t data[15] = {0};
                data[0] = p_info[0];    // cmd path
                data[1] = p_info[1];    // app idx

                app_cmd_get_fw_version(&data[2]);
                app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_CMD, APP_REMOTE_MSG_CMD_REPORT_FW_VERSION,
                                                    data, sizeof(data));
            }
        }
        break;

    case APP_REMOTE_MSG_CMD_REPORT_FW_VERSION:
        {
            uint8_t *p_info = (uint8_t *)buf;
            uint8_t report_data[9];

            memcpy(&report_data[0], &p_info[2], 9);

            // primary return secondary's fw version
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
                app_report_event(p_info[0], EVENT_FW_VERSION, p_info[1], report_data, sizeof(report_data));
            }
        }
        break;

    case APP_REMOTE_MSG_CMD_GET_OTA_FW_VERSION:
        {
            uint8_t *p_info = (uint8_t *)buf;

            // get secondary's fw version by app_ota_get_brief_img_version_for_dongle()
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
            {
                uint8_t data[IMG_LEN_FOR_DONGLE + 3] = {0};
                data[0] = p_info[0];    // cmd path
                data[1] = p_info[1];    // app idx
                data[2] = GET_SECONDARY_OTA_FW_VERSION;
                app_ota_get_brief_img_version_for_dongle(&data[3]);
                app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_CMD, APP_REMOTE_MSG_CMD_REPORT_OTA_FW_VERSION,
                                                    data, sizeof(data));
            }
        }
        break;

    case APP_REMOTE_MSG_CMD_REPORT_OTA_FW_VERSION:
        {
            uint8_t *p_info = (uint8_t *)buf;
            uint8_t report_data[IMG_LEN_FOR_DONGLE + 1];

            memcpy(&report_data[0], &p_info[2], IMG_LEN_FOR_DONGLE + 1);

            // primary return secondary's fw version
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
                app_report_event(p_info[0], EVENT_FW_VERSION, p_info[1], report_data, sizeof(report_data));
            }
        }
        break;
#endif

    default:
        break;
    }
}

bool app_cmd_get_tool_connect_status(void)
{
    bool tool_connect_status = false;
    T_APP_BR_LINK *br_link;
    T_APP_LE_LINK *le_link;
    uint8_t        i;

    for (i = 0; i < MAX_BR_LINK_NUM; i ++)
    {
        br_link = &br_db.br_link[i];

        if (br_link->connected_profile & (SPP_PROFILE_MASK | IAP_PROFILE_MASK))
        {
            if (br_link->cmd_set_enable == true)
            {
                tool_connect_status = true;
            }
        }
    }

    for (i = 0; i < MAX_BLE_LINK_NUM; i++)
    {
        le_link = &le_db.le_link[i];
        if (le_link->state == GAP_CONN_STATE_CONNECTED)
        {
            if (le_link->cmd_set_enable == true)
            {
                tool_connect_status = true;
            }
        }
    }

    return tool_connect_status;
}

void app_cmd_update_eq_ctrl(uint8_t value, bool is_need_relay)
{

}

T_SRC_SUPPORT_VER_FORMAT *app_cmd_get_src_version(uint8_t cmd_path, uint8_t app_idx)
{
    T_SRC_SUPPORT_VER_FORMAT *version = NULL;

    if (cmd_path == CMD_PATH_UART)
    {
        version = &src_support_version_uart;
    }
    else if (cmd_path == CMD_PATH_LE)
    {
        version = &src_support_version_le_link[app_idx];
    }
    else if ((cmd_path == CMD_PATH_SPP) || (cmd_path == CMD_PATH_IAP))
    {
        version = &src_support_version_br_link[app_idx];
    }

    return version;
}

bool app_cmd_check_specific_version(T_SRC_SUPPORT_VER_FORMAT *version, uint8_t ver_major,
                                    uint8_t ver_minor)
{
    if (version->cmd_set_ver_major > ver_major ||
        (version->cmd_set_ver_major == ver_minor && version->cmd_set_ver_minor >= ver_minor))
    {
        // SRC support version is compatible.
        return true;
    }

    return false;
}

bool app_cmd_check_src_cmd_version(uint8_t cmd_path, uint8_t app_idx)
{
    T_SRC_SUPPORT_VER_FORMAT *version = app_cmd_get_src_version(cmd_path, app_idx);

    if (version)
    {
        if (app_cmd_check_specific_version(version, CMD_SET_VER_MAJOR, CMD_SET_VER_MINOR))
        {
            // SRC support version is new, which is valid.
            return true;
        }
        else if (version->cmd_set_ver_major == 0 && version->cmd_set_ver_minor == 0)
        {
            // SRC never update support version
            return true;
        }
    }

    return false;
}

bool app_cmd_check_src_eq_spec_version(uint8_t cmd_path, uint8_t app_idx)
{
    T_SRC_SUPPORT_VER_FORMAT *version = app_cmd_get_src_version(cmd_path, app_idx);

    if (version)
    {
        uint8_t eq_spec_minor = EQ_SPEC_VER_MINOR;

        if (version->eq_spec_ver_major > EQ_SPEC_VER_MAJOR ||
            (version->eq_spec_ver_major == EQ_SPEC_VER_MAJOR && version->eq_spec_ver_minor >= eq_spec_minor))
        {
            // SRC support version is new, which is valid.
            return true;
        }
        else if (version->eq_spec_ver_major == 0 && version->eq_spec_ver_minor == 0)
        {
            // SRC never update support version
            return true;
        }
    }

    return false;
}

static void le_cmd_handle(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path, uint8_t app_idx,
                          uint8_t *ack_pkt)
{
    uint16_t cmd_id = (uint16_t)(cmd_ptr[0] | (cmd_ptr[1] << 8));

    APP_PRINT_TRACE1("le_cmd_handle: cmd_id 0x%04x", cmd_id);

    typedef struct
    {
        uint16_t cmd_id;
        uint8_t  status;
    } __attribute__((packed)) *ACK_PKT;

    switch (cmd_id)
    {
    case CMD_LE_START_ADVERTISING:
        {
            if (cmd_ptr[1] <= 31)
            {
                //app_ble_gap_start_advertising(APP_ADV_PURPOSE_VENDOR, cmd_ptr[0], cmd_ptr[1], &cmd_ptr[2]);
                //fixme later
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
            }

            app_cmd_set_event_ack(cmd_path, app_idx, ack_pkt);
        }
        break;

    case CMD_LE_START_SCAN:
        {
            app_cmd_set_event_ack(cmd_path, app_idx, ack_pkt);

//            typedef struct
//            {
//                uint8_t scan_type;      /**< Scan type. Write only. Default is GAP_SCAN_MODE_ACTIVE, @ref T_GAP_SCAN_MODE. */
//                uint16_t scan_interval;  /**< Scan Interval. Write only. In units of 0.625ms, range: 0x0004 to 0xFFFF. Default is 0x40. */
//                uint16_t scan_window;    /**< Scan Window. Write only. In units of 0.625ms, range: 0x0004 to 0xFFFF. Default is 0x20. */
//            } __attribute__((packed)) PHY_PARAMS;

//            struct
//            {
//                uint16_t cmd_id;
//                uint8_t local_addr_type;
//                uint8_t phys;
//                uint16_t duration;
//                uint16_t period;
//                uint8_t filter_policy;
//                uint8_t filter_duplicates;
//                PHY_PARAMS phy_param_1m;
//                PHY_PARAMS phy_param_coded;
//            } __attribute__((packed)) *cmd = (typeof(cmd))cmd_ptr;

//            /* Initialize extended scan parameters */
//            le_ext_scan_set_param(GAP_PARAM_EXT_SCAN_LOCAL_ADDR_TYPE,
//                                  sizeof((T_GAP_LOCAL_ADDR_TYPE)cmd->local_addr_type),
//                                  &cmd->local_addr_type);

//            le_ext_scan_set_param(GAP_PARAM_EXT_SCAN_PHYS, sizeof(cmd->phys), &cmd->phys);

//            le_ext_scan_set_param(GAP_PARAM_EXT_SCAN_DURATION,
//                                  sizeof(cmd->duration), &cmd->duration);

//            le_ext_scan_set_param(GAP_PARAM_EXT_SCAN_PERIOD,
//                                  sizeof(cmd->period), &cmd->period);

//            le_ext_scan_set_param(GAP_PARAM_EXT_SCAN_FILTER_POLICY,
//                                  sizeof(cmd->filter_policy), &cmd->filter_policy);

//            le_ext_scan_set_param(GAP_PARAM_EXT_SCAN_FILTER_DUPLICATES,
//                                  sizeof(cmd->filter_duplicates), &cmd->filter_duplicates);
//            if (cmd->phys & GAP_EXT_SCAN_PHYS_1M_BIT)
//            {
//                T_GAP_LE_EXT_SCAN_PARAM phy_param_1m =
//                {
//                    .scan_type = (T_GAP_SCAN_MODE)cmd->phy_param_1m.scan_type,
//                    .scan_interval = cmd->phy_param_1m.scan_interval,
//                    .scan_window = cmd->phy_param_1m.scan_window
//                };
//                APP_PRINT_TRACE3("le_handle_cmd: phy_param_1m scan_type %d, scan_interval %d, scan_window %d",
//                             phy_param_1m.scan_type , phy_param_1m.scan_interval, phy_param_1m.scan_window);
//                le_ext_scan_set_phy_param(LE_SCAN_PHY_LE_1M, &phy_param_1m);
//            }
//            if (cmd->phys & GAP_EXT_SCAN_PHYS_CODED_BIT)
//            {
//                T_GAP_LE_EXT_SCAN_PARAM phy_param_coded =
//                {
//                    .scan_type = (T_GAP_SCAN_MODE)cmd->phy_param_coded.scan_type,
//                    .scan_interval = cmd->phy_param_coded.scan_interval,
//                    .scan_window = cmd->phy_param_coded.scan_window
//                };
//                APP_PRINT_TRACE3("le_handle_cmd: phy_param_coded scan_type %d, scan_interval %d, scan_window %d",
//                             phy_param_coded.scan_type , phy_param_coded.scan_interval, phy_param_coded.scan_window);
//                le_ext_scan_set_phy_param(LE_SCAN_PHY_LE_CODED, &phy_param_coded);
//            }

            T_GAP_LE_EXT_SCAN_PARAM phy_param_1m =
            {
                .scan_type = GAP_SCAN_MODE_ACTIVE,
                .scan_interval = 400,
                .scan_window = 200,
            };
            le_ext_scan_set_phy_param(LE_SCAN_PHY_LE_1M, &phy_param_1m);

            le_ext_scan_set_phy_param(LE_SCAN_PHY_LE_CODED, &phy_param_1m);

            le_ext_scan_start();

//            APP_PRINT_TRACE6("le_handle_cmd: local_addr_type %d, phys 0x%x, duration %d, "
//                             "period %d, filter_policy 0x%x, filter_duplications 0x%x",
//                             cmd->local_addr_type, cmd->phys, cmd->duration,
//                             cmd->period, cmd->filter_policy, cmd->filter_duplicates);


        }
        break;

    case CMD_LE_STOP_SCAN:
        {
            app_cmd_set_event_ack(cmd_path, app_idx, ack_pkt);
            le_ext_scan_stop();
        }
        break;


    case CMD_LE_GET_RAND_ADDR:
        {

        }
        break;

    case CMD_LE_SET_DATA_LEN:
        {
            typedef struct
            {
                uint16_t cmd_id;
                uint8_t app_link_id;
                uint16_t tx_octets;
                uint16_t tx_time;
            } __attribute__((packed)) CMD_T;
            CMD_T *cmd = (CMD_T *)cmd_ptr;

            le_set_data_len(le_db.le_link[cmd->app_link_id].conn_id, cmd->tx_octets, cmd->tx_time);
            APP_PRINT_TRACE3("app_cmd le_cmd_handle: app_link_id %d, tx_octets %d, tx_time %d",
                             cmd->app_link_id, cmd->tx_octets, cmd->tx_time);
            app_cmd_set_event_ack(cmd_path, app_idx, ack_pkt);
        }
        break;

    case CMD_LE_SET_PHY:
        {
            typedef struct
            {
                uint16_t cmd_id;
                uint8_t app_link_id;
                uint8_t all_phys;
                uint8_t tx_phys;
                uint8_t rx_phys;
                uint8_t phy_options;
            } __attribute__((packed)) CMD_T;
            CMD_T *cmd = (CMD_T *)cmd_ptr;

            le_set_phy(le_db.le_link[cmd->app_link_id].conn_id, cmd->all_phys, cmd->tx_phys,
                       cmd->rx_phys, (T_GAP_PHYS_OPTIONS)cmd->phy_options);
            APP_PRINT_TRACE4("app_cmd le_cmd_handle: app_link_id %d, all_phys %d, tx_phys %d, rx_phys %d, phy_options",
                             cmd->app_link_id, cmd->all_phys, cmd->tx_phys, cmd->rx_phys);
            app_cmd_set_event_ack(cmd_path, app_idx, ack_pkt);
        }
        break;

    case CMD_LE_GET_REMOTE_FEATURES:
        {

        }
        break;

    case CMD_LE_START_PAIR:
        {
            typedef struct
            {
                uint16_t cmd_id;
                uint8_t  app_link_id;
            } __attribute__((packed)) CMD_T;
            CMD_T *cmd = (CMD_T *)cmd_ptr;

            le_bond_pair(le_db.le_link[cmd->app_link_id].conn_id);
            APP_PRINT_TRACE1("app_cmd le_cmd_handle CMD_LE_START_PAIR: app_link_id %d", cmd->app_link_id);
            app_cmd_set_event_ack(cmd_path, app_idx, ack_pkt);
        }
        break;

    case CMD_LE_GET_ALL_BONDED_DEV:
        {

        }
        break;

    case CMD_LE_GATT_SERV_CHG:
        {
            app_cmd_set_event_ack(cmd_path, app_idx, ack_pkt);

            typedef struct
            {
                uint16_t cmd_id;
                uint16_t conn_handle;
                uint16_t cid;
                uint16_t start_handle;
                uint16_t end_handle;
            }   __attribute__((packed)) CMD_T;
            CMD_T *cmd = (CMD_T *)cmd_ptr;

            APP_PRINT_TRACE4("app_cmd le_cmd_handle: conn_handle 0x%04x, cid 0x%04x, start_handle 0x%04x, end_handle 0x%04x",
                             cmd->conn_handle, cmd->cid, cmd->start_handle, cmd->end_handle);
            gatts_ext_service_changed_indicate(cmd->conn_handle, cmd->cid, cmd->start_handle, cmd->end_handle);
        }
        break;

    case CMD_LE_IGNORE_SLAVE_LATENCY:
        {
            app_cmd_set_event_ack(cmd_path, app_idx, ack_pkt);

            typedef struct
            {
                uint16_t cmd_id;
                uint8_t app_link_id;
                uint8_t  ignore_latency;
            } __attribute__((packed)) CMD_T;
            CMD_T *cmd = (CMD_T *)cmd_ptr;

            uint8_t conn_id = le_db.le_link[cmd->app_link_id].conn_id;
            APP_PRINT_TRACE2("app_cmd le_cmd_handle: app_link_id %d, need_ignore_latency %d", cmd->app_link_id,
                             cmd->ignore_latency);
            le_disable_slave_latency(conn_id, cmd->ignore_latency);
        }
        break;

    case CMD_LE_SET_RAND_ADDR:
        {
            typedef struct
            {
                uint16_t cmd_id;
                uint8_t addr[6];
            } __attribute__((packed)) CMD_T;
            CMD_T *cmd = (CMD_T *)cmd_ptr;

            APP_PRINT_TRACE1("app_cmd le_cmd_handle: cmd_id 0x%04x", cmd->cmd_id);

            T_GAP_CAUSE gap_cause = le_set_rand_addr(cmd->addr);
            ((ACK_PKT)ack_pkt)->status = (uint16_t)gap_cause;
            app_cmd_set_event_ack(cmd_path, app_idx, ack_pkt);
        }
        break;

    case CMD_LE_ATT_MTU_EXCHANGE:
        {

        }
        break;

    case CMD_LE_CREATE_CONN:
        {
            typedef struct
            {
                uint16_t cmd_id;
                uint8_t init_phys;
                uint8_t remote_bd_type;
                uint8_t remote_bd[6];
                uint8_t local_bd_type;
                uint16_t scan_timeout;
            } __attribute__((packed)) CMD_T;
            CMD_T *cmd = (CMD_T *)cmd_ptr;

            T_GAP_CAUSE gap_cause = le_connect(cmd->init_phys, cmd->remote_bd,
                                               (T_GAP_REMOTE_ADDR_TYPE)cmd->remote_bd_type,
                                               (T_GAP_LOCAL_ADDR_TYPE)cmd->local_bd_type, cmd->scan_timeout);

            APP_PRINT_TRACE5("app_cmd le_cmd_handle: init_phys %d, remote_bd_type %d, remote_db %s, local_bd_type %d, scan_timeout %d",
                             cmd->init_phys, cmd->remote_bd_type, TRACE_BDADDR(cmd->remote_bd), cmd->local_bd_type,
                             cmd->scan_timeout);

            ((ACK_PKT)ack_pkt)->status = (uint16_t)gap_cause;
            app_cmd_set_event_ack(cmd_path, app_idx, ack_pkt);
        }
        break;
    default:
        break;
    }
}

void app_cmd_bt_cmd_handle(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path, uint8_t app_idx,
                           uint8_t *ack_pkt)
{
    uint16_t cmd_id = (uint16_t)(cmd_ptr[0] | (cmd_ptr[1] << 8));

    enum
    {
        PARAMS_START_POS = 2,
    } CMD_POS;

    switch (cmd_id)
    {
    case CMD_LE_START_ADVERTISING...CMD_LE_ATT_MTU_EXCHANGE:
        le_cmd_handle(cmd_ptr, cmd_len, cmd_path, app_idx, ack_pkt);
        break;

    case CMD_BT_READ_PAIRED_RECORD:
        {
        }
        break;

    case CMD_BT_CREATE_CONNECTION:
        {


        }
        break;

    case CMD_BT_DISCONNECT:
        {

        }
        break;

    case CMD_BT_READ_LINK_INFO:
        {

        }
        break;

    case CMD_BT_GET_REMOTE_NAME:
        {
            bt_remote_name_req(&cmd_ptr[2]);
            app_cmd_set_event_ack(cmd_path, app_idx, ack_pkt);
        }
        break;

    case CMD_BT_IAP_LAUNCH_APP:
        {
#if F_APP_IAP_RTK_SUPPORT && F_APP_IAP_SUPPORT
            uint8_t app_index = cmd_ptr[2];

            if (app_index < MAX_BR_LINK_NUM)
            {
                APP_IAP_HDL app_iap_hdl = NULL;
                app_iap_hdl = app_iap_search_by_addr(app_db.br_link[app_index].bd_addr);

                if ((app_db.br_link[app_index].connected_profile & IAP_PROFILE_MASK)
                    && (app_iap_is_authened(app_iap_hdl)))
                {
                    app_iap_rtk_launch(app_db.br_link[app_index].bd_addr, BT_IAP_APP_LAUNCH_WITH_USER_ALERT);
                }
                else
                {
                    ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
                }
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
            }

            app_cmd_set_event_ack(cmd_path, app_idx, ack_pkt);

#endif
        }
        break;

    case CMD_BT_SEND_AT_CMD:
        {
            uint8_t app_index = cmd_ptr[2];

            if (bt_hfp_send_vnd_at_cmd_req(br_db.br_link[app_index].bd_addr, (char *)&cmd_ptr[3]) == false)
            {
                ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
            }

            app_cmd_set_event_ack(cmd_path, app_idx, ack_pkt);
        }
        break;

    case CMD_BT_HFP_DIAL_WITH_NUMBER:
        {

        }
        break;

    case CMD_GET_BD_ADDR:
        {

        }
        break;



    case CMD_BT_GET_LOCAL_ADDR:
        {
            uint8_t temp_buff[6];
            memcpy(&temp_buff[0], app_cfg_nv.bud_local_addr, 6);
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            app_report_event(CMD_PATH_UART, EVENT_LOCAL_ADDR, 0, temp_buff, sizeof(temp_buff));
        }
        break;

    case CMD_GET_LEGACY_RSSI:
        {
            if (cmd_ptr[2] == LEGACY_RSSI)
            {
                uint8_t *bd_addr = (uint8_t *)(&cmd_ptr[3]);

                if (gap_br_read_rssi(bd_addr) != GAP_CAUSE_SUCCESS)
                {
                    ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                }
            }

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;

    case CMD_BT_BOND_INFO_CLEAR:
        {
            uint8_t *bd_addr = (uint8_t *)(&cmd_ptr[3]);
            uint8_t temp_buff = CLEAR_BOND_INFO_FAIL;
#if (F_APP_WATCH_CUSTOMER_CONN_MANAGER_SUPPORT == 1)
            T_APP_BOND_DEVICE *device = app_bt_bond_get_device_by_addr(bd_addr);
            if (device->used == 1)
            {
                gap_br_send_acl_disconn_req(bd_addr);
            }
#endif
            if (cmd_ptr[2] == 0) //clear BR/EDR bond info
            {
                if (bt_bond_delete(bd_addr) != true)
                {
                    APP_PRINT_ERROR0("CMD_BT_BOND_INFO_CLEAR bt_bond_delete error");
                }
#if (F_APP_WATCH_CUSTOMER_CONN_MANAGER_SUPPORT == 1)
                app_bt_bond_del_bond_device(bd_addr);
                app_bt_bond_save_device_info_to_ftl(app_db.bond_device);
#endif
                temp_buff = CLEAR_BOND_INFO_SUCCESS;
            }
            app_cmd_set_event_ack(cmd_path, app_idx, ack_pkt);
            app_report_event(CMD_PATH_UART, EVENT_BT_BOND_INFO_CLEAR, 0, &temp_buff, sizeof(temp_buff));
        }
        break;

    case CMD_GET_NUM_OF_CONNECTION:
        {

        }
        break;

    case CMD_SUPPORT_MULTILINK:
        {
#if F_APP_DUAL_AUDIO_EFFECT
            if (app_cfg_const.enable_multi_link == 0)
#endif

            }
        break;

    default:
        break;
    }
}

#if F_APP_DEVICE_CMD_SUPPORT
static void app_cmd_device_cmd_handle(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path,
                                      uint8_t app_idx, uint8_t *ack_pkt)
{
    uint16_t cmd_id = (uint16_t)(cmd_ptr[0] | (cmd_ptr[1] << 8));

    switch (cmd_id)
    {
//    case CMD_INQUIRY:
//        {
//            if ((cmd_ptr[2] == START_INQUIRY) && (cmd_ptr[3] <= MAX_INQUIRY_TIME))
//            {
//                if (cmd_ptr[4] == NORMAL_INQUIRY)
//                {
//                    if (gap_br_start_inquiry(false, cmd_ptr[3]) != GAP_CAUSE_SUCCESS)
//                    {
//                        ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
//                    }
//                }
//                else
//                {
//                    if (bt_periodic_inquiry_start(false, 3, 2, 1))
//                    {
//                        app_start_timer(&timer_idx_stop_periodic_inquiry, "stop_periodic_inquiry",
//                                        app_cmd_timer_id, APP_TIMER_STOP_PERIODIC_INQUIRY, app_idx, false,
//                                        cmd_ptr[3] * 1280);
//                    }
//                    else
//                    {
//                        ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
//                    }
//                }
//            }
//            else if (cmd_ptr[2] == STOP_INQUIRY)
//            {
//                if (cmd_ptr[4] == NORMAL_INQUIRY)
//                {
//                    if (gap_br_stop_inquiry() != GAP_CAUSE_SUCCESS)
//                    {
//                        ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
//                    }
//                }
//                else
//                {
//                    app_stop_timer(&timer_idx_stop_periodic_inquiry);

//                    if (bt_periodic_inquiry_stop() != true)
//                    {
//                        ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
//                    }
//                }
//            }
//            else
//            {
//                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
//            }
//            app_cmd_set_event_ack(cmd_path, app_idx, ack_pkt);
//        }
//        break;

    case CMD_SERVICES_SEARCH:
        {
            uint8_t bd_addr[6];

            memcpy(bd_addr, &cmd_ptr[3], 6);

            if (cmd_ptr[2] == START_SERVICES_SEARCH)
            {
                T_LINKBACK_SEARCH_PARAM search_param;
                if (linkback_profile_search_start(bd_addr, cmd_ptr[9], false, &search_param) == false)
                {
                    ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                }
            }
            else if (cmd_ptr[2] == STOP_SERVICES_SEARCH)
            {
                if (gap_br_stop_sdp_discov(bd_addr) != GAP_CAUSE_SUCCESS)
                {
                    ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                }
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
            }
            app_cmd_set_event_ack(cmd_path, app_idx, ack_pkt);
        }
        break;

    case CMD_SET_PAIRING_CONTROL:
        {
//            if (cmd_ptr[2] == FORWARD_ACL_ACF_REQ_TO_HOST)
//            {
//                enable_auto_accept_conn_req_flag = false;
//                enable_auto_reject_conn_req_flag = false;
//            }
//            else if (cmd_ptr[2] == ENABLE_AUTO_ACCEPT_ACL_ACF_REQ)
//            {
//                enable_auto_accept_conn_req_flag = true;
//                enable_auto_reject_conn_req_flag = false;
//            }
//            else if (cmd_ptr[2] == ENABLE_AUTO_REJECT_ACL_ACF_REQ)
//            {
//                enable_auto_accept_conn_req_flag = false;
//                enable_auto_reject_conn_req_flag = true;
//            }
//            else
//            {
//                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
//            }

//            if (ack_pkt[2] == CMD_SET_STATUS_COMPLETE)
//            {
//                if (gap_br_cfg_auto_accept_acl(enable_auto_accept_conn_req_flag) != GAP_CAUSE_SUCCESS)
//                {
//                    ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
//                }
//            }
//            app_cmd_set_event_ack(cmd_path, app_idx, ack_pkt);
        }
        break;

    case CMD_SET_PIN_CODE:
        {
            if ((cmd_ptr[2] >= 0x01) && (cmd_ptr[2] <= 0x08))
            {
                app_cfg_nv.pin_code_size = cmd_ptr[2];
                memcpy(app_cfg_nv.pin_code, &cmd_ptr[3], cmd_ptr[2]);

                //save to flash after set pin_code
                app_cfg_store(&app_cfg_nv.mic_channel, 12);
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
            }
            app_cmd_set_event_ack(cmd_path, app_idx, ack_pkt);
        }
        break;

    case CMD_GET_PIN_CODE:
        {
            uint8_t pin_code_size = app_cfg_nv.pin_code_size;
            uint8_t temp_buff[pin_code_size + 1];

            temp_buff[0] = pin_code_size;
            memcpy(&temp_buff[1], app_cfg_nv.pin_code, pin_code_size);

            app_cmd_set_event_ack(cmd_path, app_idx, ack_pkt);
            app_report_event(cmd_path, EVENT_REPORT_PIN_CODE, app_idx, temp_buff, sizeof(temp_buff));
        }
        break;

    case CMD_PAIR_REPLY:
        {
            uint8_t *bd_addr = app_test_get_acl_conn_ind_bd_addr();

            if (cmd_ptr[2] == ACCEPT_PAIRING_REQ)
            {
                if (gap_br_accept_acl_conn(bd_addr, GAP_BR_LINK_ROLE_SLAVE) != GAP_CAUSE_SUCCESS)
                {
                    ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                }
            }
            else if (cmd_ptr[2] == REJECT_PAIRING_REQ)
            {
                if (gap_br_reject_acl_conn(bd_addr, GAP_ACL_REJECT_LIMITED_RESOURCE) != GAP_CAUSE_SUCCESS)
                {
                    ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                }
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
            }
            app_cmd_set_event_ack(cmd_path, app_idx, ack_pkt);
        }
        break;

    case CMD_SSP_CONFIRMATION:
        {
            uint8_t *bd_addr = app_test_get_user_confirmation_bd_addr();

            if (cmd_ptr[2] == ACCEPT_PAIRING_REQ)
            {
                if (gap_br_user_cfm_req_cfm(bd_addr, GAP_CFM_CAUSE_ACCEPT) != GAP_CAUSE_SUCCESS)
                {
                    ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                }
            }
            else if (cmd_ptr[2] == REJECT_PAIRING_REQ)
            {
                if (gap_br_user_cfm_req_cfm(bd_addr, GAP_CFM_CAUSE_REJECT) != GAP_CAUSE_SUCCESS)
                {
                    ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                }
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
            }
            app_cmd_set_event_ack(cmd_path, app_idx, ack_pkt);
        }
        break;

    case CMD_GET_CONNECTED_DEV_ID:
        {
            uint8_t b2s_connected_num = 0;
            uint8_t b2s_connected_id[MAX_BR_LINK_NUM] = {0};

            for (uint8_t i = 0; i < MAX_BR_LINK_NUM; i++)
            {
                if (app_check_b2s_link_by_id(i))
                {
                    b2s_connected_id[b2s_connected_num] = i;
                    b2s_connected_num = b2s_connected_num + 1;
                }
            }

            uint8_t temp_buff[b2s_connected_num + 1];

            temp_buff[0] = b2s_connected_num;
            memcpy(&temp_buff[1], &b2s_connected_id[0], b2s_connected_num);

            app_cmd_set_event_ack(cmd_path, app_idx, ack_pkt);
            app_report_event(cmd_path, EVENT_REPORT_CONNECTED_DEV_ID, app_idx, temp_buff, sizeof(temp_buff));
        }
        break;

    case CMD_GET_REMOTE_DEV_ATTR_INFO:
        {
            uint8_t app_index = cmd_ptr[2];
            T_LINKBACK_SEARCH_PARAM search_param;
            uint8_t bd_addr[6];
            uint8_t prof = 0;

            memcpy(&bd_addr[0], app_db.br_link[app_index].bd_addr, 6);
            if (cmd_ptr[3] == GET_AVRCP_ATTR_INFO)
            {
                prof = AVRCP_PROFILE_MASK;
            }
            else if (cmd_ptr[3] == GET_PBAP_ATTR_INFO)
            {
                prof = PBAP_PROFILE_MASK;
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
            }

            if (ack_pkt[2] == CMD_SET_STATUS_COMPLETE)
            {
                if (linkback_profile_search_start(bd_addr, prof, false, &search_param) == false)
                {
                    ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                }
                else
                {
                    app_cmd_set_report_attr_info_flag(true);
                }
            }

            app_cmd_set_event_ack(cmd_path, app_idx, ack_pkt);
        }
        break;

    default:
        break;
    }
}
#endif

static void app_cmd_general_cmd_handle(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path,
                                       uint8_t app_idx, uint8_t *ack_pkt)
{

}

static void app_cmd_other_cmd_handle(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path,
                                     uint8_t app_idx, uint8_t *ack_pkt)
{

}

static void app_cmd_customized_cmd_handle(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path,
                                          uint8_t app_idx, uint8_t *ack_pkt)
{

}

void app_handle_cmd_set(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path, uint8_t rx_seqn,
                        uint8_t app_idx)
{
#if (F_APP_ANC_SUPPORT | F_APP_APT_SUPPORT | F_APP_BRIGHTNESS_SUPPORT | (F_APP_LISTENING_MODE_SUPPORT & NEW_FORMAT_LISTENING_CMD_REPORT))
    uint16_t param_len = cmd_len - COMMAND_ID_LENGTH;
#endif
    uint16_t cmd_id;
    uint8_t  ack_pkt[3];

#if (F_APP_ONE_WIRE_UART_SUPPORT == 1)
    if (app_cfg_const.one_wire_uart_support && one_wire_state == ONE_WIRE_STATE_IN_ONE_WIRE)
    {
        uint8_t  device_id;

        device_id  = cmd_ptr[0];
        cmd_ptr++;

        if (device_id != app_cfg_const.bud_side)
        {
            APP_PRINT_TRACE0("app_handle_cmd_set++: cmd not for this bud");
            return;
        }
    }
#endif

    cmd_id     = (uint16_t)(cmd_ptr[0] | (cmd_ptr[1] << 8));
    ack_pkt[0] = cmd_ptr[0];
    ack_pkt[1] = cmd_ptr[1];
    ack_pkt[2] = CMD_SET_STATUS_COMPLETE;

    APP_PRINT_TRACE4("app_handle_cmd_set++: cmd_id 0x%04x, cmd_len 0x%04x, cmd_path %u, rx_seqn 0x%02x",
                     cmd_id, cmd_len, cmd_path, rx_seqn);

    /* check duplicated seq num */
    if ((cmd_id != CMD_ACK) && (rx_seqn != 0))
    {
        uint8_t *check_rx_seqn = NULL;
        uint8_t  report_app_idx = app_idx;

        if (cmd_path == CMD_PATH_UART)
        {
            check_rx_seqn = &uart_rx_seqn;
            report_app_idx = 0;
        }
        else if (cmd_path == CMD_PATH_LE)
        {
            check_rx_seqn = &le_db.le_link[app_idx].rx_cmd_seqn;
        }
        else if ((cmd_path == CMD_PATH_SPP) || (cmd_path == CMD_PATH_IAP))
        {
            check_rx_seqn = &br_db.br_link[app_idx].rx_cmd_seqn;
        }

        if (check_rx_seqn)
        {
            if (*check_rx_seqn == rx_seqn)
            {
                app_report_event(cmd_path, EVENT_ACK, report_app_idx, ack_pkt, 3);
                return;
            }

            *check_rx_seqn = rx_seqn;
        }
    }

    if ((cmd_path == CMD_PATH_SPP) || (cmd_path == CMD_PATH_IAP))
    {
        br_db.br_link[app_idx].cmd_set_enable = true;
    }
    else if (cmd_path == CMD_PATH_LE)
    {
        if (le_db.le_link[app_idx].state == GAP_CONN_STATE_CONNECTED)
        {
            le_db.le_link[app_idx].cmd_set_enable = true;
        }
    }

    switch (cmd_id)
    {
    case CMD_ACK:
        {
            if (cmd_path == CMD_PATH_UART)
            {
                app_pop_data_transfer_queue(CMD_PATH_UART, true);
            }
            else if ((cmd_path == CMD_PATH_LE) || (cmd_path == CMD_PATH_SPP) || (cmd_path == CMD_PATH_IAP))
            {
                uint16_t event_id = (uint16_t)(cmd_ptr[2] | (cmd_ptr[3] << 8));
                uint8_t status = cmd_ptr[4];

                //if (!app_cfg_const.enable_dsp_capture_data_by_spp && !app_cfg_const.mems_support)
                {
                    app_transfer_queue_recv_ack_check(event_id, cmd_path);
                }

                if (event_id == EVENT_AUDIO_EQ_PARAM_REPORT)
                {
                    if (cmd_path == CMD_PATH_LE)
                    {
                        if (status != CMD_SET_STATUS_COMPLETE)
                        {
                            //app_eq_report_terminate_param_report(cmd_path, app_idx);
                        }
                        else
                        {
                            //app_eq_report_eq_param(cmd_path, app_idx);
                        }
                    }
                }
                else if (event_id == EVENT_OTA_ACTIVE_ACK || event_id == EVENT_OTA_ROLESWAP)
                {
                    if ((cmd_path == CMD_PATH_SPP) || (cmd_path == CMD_PATH_IAP))
                    {
                        //app_ota_cmd_ack_handle(event_id, status);
                    }
                }
#if F_APP_LOCAL_PLAYBACK_SUPPORT
                else if ((event_id == EVENT_PLAYBACK_GET_LIST_DATA) && (status == CMD_SET_STATUS_COMPLETE))
                {
                    if (cmd_path == CMD_PATH_SPP || cmd_path == CMD_PATH_IAP)
                    {
                        app_playback_trans_list_data_handle();
                    }
                }
#endif
            }
        }
        break;

    case CMD_BT_READ_PAIRED_RECORD:
    case CMD_BT_CREATE_CONNECTION:
    case CMD_BT_DISCONNECT:
    case CMD_BT_READ_LINK_INFO:
    case CMD_BT_GET_REMOTE_NAME:
    case CMD_BT_IAP_LAUNCH_APP:
    case CMD_BT_SEND_AT_CMD:
    case CMD_BT_HFP_DIAL_WITH_NUMBER:
    case CMD_GET_BD_ADDR:
    case CMD_LE_START_ADVERTISING:
    case CMD_LE_START_SCAN:
    case CMD_LE_STOP_SCAN:
    case CMD_LE_GET_RAND_ADDR:
    case CMD_BT_GET_LOCAL_ADDR:
    case CMD_LE_CREATE_CONN:
    case CMD_GET_LEGACY_RSSI:
    case CMD_BT_BOND_INFO_CLEAR:
    case CMD_GET_NUM_OF_CONNECTION:
    case CMD_SUPPORT_MULTILINK:
    case CMD_LE_SET_DATA_LEN:
    case CMD_LE_SET_PHY:
    case CMD_LE_GET_REMOTE_FEATURES:
    case CMD_LE_START_PAIR:
    case CMD_LE_GET_ALL_BONDED_DEV:
    case CMD_LE_GATT_SERV_CHG:
    case CMD_LE_IGNORE_SLAVE_LATENCY:
    case CMD_LE_SET_RAND_ADDR:
    case CMD_LE_ATT_MTU_EXCHANGE:
        {
            app_cmd_bt_cmd_handle(cmd_ptr, cmd_len, cmd_path, app_idx, ack_pkt);
        }
        break;

    case CMD_LEGACY_DATA_TRANSFER:
    case CMD_LE_DATA_TRANSFER:
        {
            //app_transfer_cmd_handle(cmd_ptr, cmd_len, cmd_path, app_idx, ack_pkt);
        }
        break;

    case CMD_MMI:
    case CMD_INFO_REQ:
    case CMD_SET_CFG:
    case CMD_GET_CFG_SETTING:
    case CMD_INDICATION:
    case CMD_LANGUAGE_GET:
    case CMD_LANGUAGE_SET:
    case CMD_GET_STATUS:
    case CMD_GET_BUD_INFO:
    case CMD_GET_FW_VERSION:
    case CMD_WDG_RESET:
    case CMD_GET_FLASH_DATA:
    case CMD_GET_PACKAGE_ID:
    case CMD_GET_EAR_DETECTION_STATUS:
    case CMD_REG_ACCESS:
    case CMD_SEND_RAW_PAYLOAD:
        {
            app_cmd_general_cmd_handle(cmd_ptr, cmd_len, cmd_path, app_idx, ack_pkt);
        }
        break;

#if F_APP_TTS_SUPPORT
    case CMD_TTS:
        {
            app_tts_cmd_handle(cmd_ptr, cmd_len, cmd_path, app_idx, ack_pkt);
        }
        break;
#endif

    case CMD_SET_VP_VOLUME:
    case CMD_AUDIO_DSP_CTRL_SEND:
    case CMD_AUDIO_CODEC_CTRL_SEND:
    case CMD_SET_VOLUME:
#if F_APP_APT_SUPPORT
    case CMD_SET_APT_VOLUME_OUT_LEVEL:
    case CMD_GET_APT_VOLUME_OUT_LEVEL:
#endif
#if F_APP_ADJUST_TONE_VOLUME_SUPPORT
    case CMD_SET_TONE_VOLUME_LEVEL:
    case CMD_GET_TONE_VOLUME_LEVEL:
#endif
    case CMD_DSP_TOOL_FUNCTION_ADJUSTMENT:
    case CMD_MIC_SWITCH:
    case CMD_DSP_TEST_MODE:
    case CMD_DUAL_MIC_MP_VERIFY:
    case CMD_SOUND_PRESS_CALIBRATION:
    case CMD_GET_LOW_LATENCY_MODE_STATUS:
    case CMD_SET_LOW_LATENCY_LEVEL:
        {
        }
        break;

#if DATA_CAPTURE_V2_SUPPORT
    case CMD_DSP_CAPTURE_V2_START_STOP:
        {
            app_data_capture_cmd_handle(cmd_ptr, cmd_len, cmd_path, app_idx, ack_pkt);
        }
        break;

    case CMD_DSP_CAPTURE_SCENARIO_ENTER_EXIT:
        {
            app_data_capture_mode_ctl(&cmd_ptr[2], cmd_len - 2, cmd_path, app_idx, ack_pkt);
        }
        break;
#endif

#if F_APP_CAP_TOUCH_SUPPORT
    case CMD_CAP_TOUCH_CTL:
        {
            CTC_PRINT_TRACE1("CMD_CAP_TOUCH_CTL %b", TRACE_BINARY(cmd_len, cmd_ptr));
            app_dlps_disable(APP_DLPS_ENTER_CHECK_CMD);
            //app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            app_cap_touch_cmd_param_handle(cmd_path, cmd_ptr, app_idx);
            app_dlps_enable(APP_DLPS_ENTER_CHECK_CMD);
        }
        break;
#endif

    case CMD_AUDIO_EQ_QUERY:
    case CMD_AUDIO_EQ_QUERY_PARAM:
    case CMD_AUDIO_EQ_PARAM_SET:
    case CMD_AUDIO_EQ_PARAM_GET:
    case CMD_AUDIO_EQ_INDEX_SET:
    case CMD_AUDIO_EQ_INDEX_GET:
#if F_APP_APT_SUPPORT
    case CMD_APT_EQ_INDEX_SET:
    case CMD_APT_EQ_INDEX_GET:
#endif
#if F_APP_USER_EQ_SUPPORT
    case CMD_RESET_EQ_DATA:
#endif
        {

        }
        break;

#if F_APP_DEVICE_CMD_SUPPORT
    //case CMD_INQUIRY:
    case CMD_SERVICES_SEARCH:
    case CMD_SET_PAIRING_CONTROL:
    case CMD_SET_PIN_CODE:
    case CMD_GET_PIN_CODE:
    case CMD_PAIR_REPLY:
    case CMD_SSP_CONFIRMATION:
    case CMD_GET_CONNECTED_DEV_ID:
    case CMD_GET_REMOTE_DEV_ATTR_INFO:
        {
            app_cmd_device_cmd_handle(cmd_ptr, cmd_len, cmd_path, app_idx, ack_pkt);
        }
        break;
#endif

#if F_APP_HFP_CMD_SUPPORT
    case CMD_SEND_DTMF:
    case CMD_GET_SUBSCRIBER_NUM:
    case CMD_GET_OPERATOR:
    case CMD_SEND_VGM:
    case CMD_SEND_VGS:
        {
            app_hfp_cmd_handle(cmd_ptr, cmd_len, cmd_path, app_idx, ack_pkt);
        }
        break;
#endif

#if F_APP_AVRCP_CMD_SUPPORT
    case CMD_AVRCP_LIST_SETTING_ATTR:
    case CMD_AVRCP_LIST_SETTING_VALUE:
    case CMD_AVRCP_GET_CURRENT_VALUE:
    case CMD_AVRCP_SET_VALUE:
    case CMD_AVRCP_ABORT_DATA_TRANSFER:
    case CMD_AVRCP_SET_ABSOLUTE_VOLUME:
    case CMD_AVRCP_GET_PLAY_STATUS:
    case CMD_AVRCP_GET_ELEMENT_ATTR:
        {
            app_avrcp_cmd_handle(cmd_ptr, cmd_len, cmd_path, app_idx, ack_pkt);
        }
        break;
#endif

    case CMD_OTA_DEV_INFO:
    case CMD_OTA_IMG_VER:
    case CMD_OTA_INACTIVE_BANK_VER:
    case CMD_OTA_START:
    case CMD_OTA_PACKET:
    case CMD_OTA_VALID:
    case CMD_OTA_RESET:
    case CMD_OTA_ACTIVE_RESET:
    case CMD_OTA_BUFFER_CHECK_ENABLE:
    case CMD_OTA_BUFFER_CHECK:
    case CMD_OTA_IMG_INFO:
    case CMD_OTA_SECTION_SIZE:
    case CMD_OTA_DEV_EXTRA_INFO:
    case CMD_OTA_PROTOCOL_TYPE:
    case CMD_OTA_GET_RELEASE_VER:
    case CMD_OTA_COPY_IMG:
    case CMD_OTA_CHECK_SHA256:
    case CMD_OTA_TEST_EN:
    case CMD_OTA_ROLESWAP:
        {
            //app_ota_cmd_handle(cmd_path, cmd_len, cmd_ptr, app_idx);
        }
        break;

#if F_APP_KEY_EXTEND_FEATURE
    case CMD_GET_SUPPORTED_MMI_LIST:
    case CMD_GET_SUPPORTED_CLICK_TYPE:
    case CMD_GET_SUPPORTED_CALL_STATUS:
    case CMD_GET_KEY_MMI_MAP:
    case CMD_SET_KEY_MMI_MAP:
    case CMD_RESET_KEY_MMI_MAP:
#if F_APP_RWS_KEY_SUPPORT
    case CMD_GET_RWS_KEY_MMI_MAP:
    case CMD_SET_RWS_KEY_MMI_MAP:
    case CMD_RESET_RWS_KEY_MMI_MAP:
#endif
        {
            ack_pkt[2] = app_key_handle_key_remapping_cmd(cmd_len, cmd_ptr, app_idx, cmd_path);
            if (ack_pkt[2] != CMD_SET_STATUS_COMPLETE)
            {
                app_cmd_set_event_ack(cmd_path, app_idx, ack_pkt);
            }
        }
        break;
#endif

#if F_APP_ANC_SUPPORT
    //for ANC command
    case CMD_ANC_TEST_MODE:
    case CMD_ANC_WRITE_GAIN:
    case CMD_ANC_READ_GAIN:
    case CMD_ANC_BURN_GAIN:
    case CMD_ANC_COMPARE:
    case CMD_ANC_GEN_TONE:
    case CMD_ANC_CONFIG_DATA_LOG:
    case CMD_ANC_READ_DATA_LOG:
    case CMD_ANC_READ_REGISTER:
    case CMD_ANC_WRITE_REGISTER:
    case CMD_ANC_QUERY:
    case CMD_ANC_ENABLE_DISABLE:
    case CMD_ANC_LLAPT_WRITE_GAIN:
    case CMD_ANC_LLAPT_READ_GAIN:
    case CMD_ANC_LLAPT_BURN_GAIN:
    case CMD_ANC_LLAPT_FEATURE_CONTROL:
    case CMD_RAMP_GET_INFO:
    case CMD_RAMP_BURN_PARA_START:
    case CMD_RAMP_BURN_PARA_CONTINUE:
    case CMD_RAMP_BURN_GRP_INFO:
    case CMD_RAMP_MULTI_DEVICE_APPLY:
    case CMD_LISTENING_MODE_CYCLE_SET:
    case CMD_LISTENING_MODE_CYCLE_GET:
    case CMD_ANC_GET_ADSP_INFO:
    case CMD_ANC_SEND_ADSP_PARA_START:
    case CMD_ANC_SEND_ADSP_PARA_CONTINUE:
    case CMD_ANC_ENABLE_DISABLE_ADAPTIVE_ANC:
    case CMD_ANC_SCENARIO_CHOOSE_INFO:
    case CMD_ANC_SCENARIO_CHOOSE_TRY:
    case CMD_ANC_SCENARIO_CHOOSE_RESULT:
        {
            app_anc_cmd_pre_handle(cmd_id, &cmd_ptr[PARAM_START_POINT], param_len, cmd_path, app_idx, ack_pkt);
        }
        break;
#endif

#if F_APP_APT_SUPPORT
    case CMD_LLAPT_QUERY:
    case CMD_LLAPT_ENABLE_DISABLE:
    case CMD_APT_VOLUME_INFO:
    case CMD_APT_VOLUME_SET:
    case CMD_APT_VOLUME_STATUS:
#if F_APP_LLAPT_SCENARIO_CHOOSE_SUPPORT
    case CMD_LLAPT_SCENARIO_CHOOSE_INFO:
    case CMD_LLAPT_SCENARIO_CHOOSE_TRY:
    case CMD_LLAPT_SCENARIO_CHOOSE_RESULT:
#endif
    case CMD_APT_GET_POWER_ON_DELAY_TIME:
    case CMD_APT_SET_POWER_ON_DELAY_TIME:
        {
            app_apt_cmd_pre_handle(cmd_id, &cmd_ptr[PARAM_START_POINT], param_len, cmd_path, app_idx, ack_pkt);
        }
        break;
#endif

#if F_APP_BRIGHTNESS_SUPPORT
    case CMD_LLAPT_BRIGHTNESS_INFO:
    case CMD_LLAPT_BRIGHTNESS_SET:
    case CMD_LLAPT_BRIGHTNESS_STATUS:
        {
            app_apt_brightness_cmd_pre_handle(cmd_id, &cmd_ptr[PARAM_START_POINT], param_len, cmd_path, app_idx,
                                              ack_pkt);
        }
        break;
#endif

#if F_APP_LISTENING_MODE_SUPPORT
#if NEW_FORMAT_LISTENING_CMD_REPORT
    case CMD_LISTENING_STATE_SET:
    case CMD_LISTENING_STATE_STATUS:
        {
            app_listening_cmd_pre_handle(cmd_id, &cmd_ptr[PARAM_START_POINT], param_len, cmd_path, app_idx,
                                         ack_pkt);
        }
        break;
#endif
#endif

#if F_APP_PBAP_CMD_SUPPORT
    case CMD_PBAP_DOWNLOAD:
    case CMD_PBAP_DOWNLOAD_CONTROL:
    case CMD_PBAP_DOWNLOAD_GET_SIZE:
        {
            app_pbap_cmd_handle(cmd_ptr, cmd_len, cmd_path, app_idx, ack_pkt);
        }
        break;
#endif

    case CMD_ASSIGN_BUFFER_SIZE:
    case CMD_TONE_GEN:
    case CMD_STRING_MODE:
    case CMD_SET_AND_READ_DLPS:
#if F_APP_BLE_ANCS_CLIENT_SUPPORT
    case CMD_ANCS_REGISTER ... CMD_ANCS_PERFORM_NOTIFICATION_ACTION:
#endif
    case CMD_LED_TEST:
    case CMD_SWITCH_TO_HCI_DOWNLOAD_MODE:
#if F_APP_ADC_SUPPORT
    case CMD_GET_PAD_VOLTAGE:
#endif
    case CMD_RF_XTAK_K:
    case CMD_RF_XTAL_K_GET_RESULT:
#if F_APP_ANC_SUPPORT
    case CMD_ANC_VENDOR_COMMAND:
#endif
#if F_APP_OTA_TOOLING_SUPPORT
    case CMD_OTA_TOOLING_PARKING:
#endif
    case CMD_MEMORY_DUMP:
    case CMD_AMS_REGISTER ... CMD_AMS_WRITE_ENTITY_UPD_CMD:
    case CMD_MP_TEST:
        {
            app_cmd_other_cmd_handle(cmd_ptr, cmd_len, cmd_path, app_idx, ack_pkt);
        }
        break;

#if (F_APP_ONE_WIRE_UART_SUPPORT == 1)
    case CMD_FORCE_ENGAGE:
    case CMD_AGING_TEST_CONTROL:
        {
            app_one_wire_cmd_handle(cmd_ptr, cmd_len, cmd_path, app_idx, ack_pkt);
        }
        break;
#endif

    case CMD_IO_PIN_PULL_HIGH:
    case CMD_ENTER_BAT_OFF_MODE:
#if F_APP_DUAL_AUDIO_EFFECT
    case CMD_CUSTOMIZED_SITRON_FEATURE:
#endif
    case CMD_MIC_MP_VERIFY_BY_HFP:
        {
            app_cmd_customized_cmd_handle(cmd_ptr, cmd_len, cmd_path, app_idx, ack_pkt);
        }
        break;

        /* BBPro2 specialized feature */
#if F_APP_LOCAL_PLAYBACK_SUPPORT
    case CMD_PLAYBACK_QUERY_INFO:
    case CMD_PLAYBACK_GET_LIST_DATA:
    case CMD_PLAYBACK_TRANS_START:
    case CMD_PLAYBACK_TRANS_CONTINUE:
    case CMD_PLAYBACK_REPORT_BUFFER_CHECK:
    case CMD_PLAYBACK_VALID_SONG:
    case CMD_PLAYBACK_TRIGGER_ROLE_SWAP:
    case CMD_PLAYBACK_TRANS_CANCEL:
    case CMD_PLAYBACK_PERMANENT_DELETE_SONG:
    case CMD_PLAYBACK_PERMANENT_DELETE_ALL_SONG:
    case CMD_PLAYBACK_PLAYLIST_ADD_SONG:
    case CMD_PLAYBACK_PLAYLIST_DELETE_SONG:
    case CMD_PLAYBACK_EXIT_TRANS:
        {
            app_playback_trans_cmd_handle(cmd_len, cmd_ptr, app_idx);
        }
        break;
#endif

    case CMD_DSP_DEBUG_SIGNAL_IN:
        {
            if ((cmd_len - 2) != DSP_DEBUG_SIGNAL_IN_PAYLOAD_LEN)
            {
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
            }
            else
            {
                uint8_t buf[4 + DSP_DEBUG_SIGNAL_IN_PAYLOAD_LEN];
                uint16_t payload_len = 4 + DSP_DEBUG_SIGNAL_IN_PAYLOAD_LEN;

                memcpy(buf + 4, &cmd_ptr[2], DSP_DEBUG_SIGNAL_IN_PAYLOAD_LEN);

                buf[0] = CFG_SET_DSP_DEBUG_SIGNAL_IN & 0xFF;
                buf[1] = (CFG_SET_DSP_DEBUG_SIGNAL_IN & 0xFF00) >> 8;
                buf[2] = (payload_len / 4) & 0xFF;
                buf[3] = ((payload_len / 4) & 0xFF00) >> 8;

                audio_probe_dsp_send(buf, payload_len);
                //app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_CMD, APP_REMOTE_MSG_DSP_DEBUG_SIGNAL_IN_SYNC,
                //                                    buf, payload_len);
            }

            app_cmd_set_event_ack(cmd_path, app_idx, ack_pkt);
        }
        break;

#if F_APP_HEARABLE_SUPPORT
    case CMD_HA_SET_PARAM:
    case CMD_HA_VER_REQ:
    case CMD_HA_ACCESS_EFFECT_INDEX:
    case CMD_HA_ACCESS_ON_OFF:
    case CMD_HA_GET_TOOL_EXTEND_REQ:
    case CMD_HA_SET_DSP_PARAM:
    case CMD_HA_ACCESS_PROGRAM_ID:
    case CMD_HA_SET_NR_PARAM:
    case CMD_HA_GET_PROGRAM_NUM:
    case CMD_HA_ACCESS_PROGRAM_NAME:
    case CMD_HA_SET_OVP_PARAM:
    case CMD_HA_SET_BEAMFORMING_PARAM:
        {
            app_ha_cmd_handle(cmd_ptr, cmd_len, cmd_path, app_idx, ack_pkt);
        }
        break;
#endif /*F_APP_HEARABLE_SUPPORT */
        /* end of BBPro2 specialized feature */

#if F_APP_SS_SUPPORT
    case CMD_SS_REQ:
        {
            app_ss_cmd_handle(cmd_ptr, cmd_len, cmd_path, app_idx, ack_pkt);
        }
        break;
#endif

#if F_APP_SAIYAN_MODE
    case CMD_SAIYAN_GAIN_CTL:
        {
            app_data_capture_gain_ctl(&cmd_ptr[2], cmd_len - 2, cmd_path, app_idx, ack_pkt);
        }
        break;
#endif

    /* customer customer command handler*/
    case CMD_XM_ENTER_HCI_MODE...CMD_XM_CRASH_DUMP:
    case CMD_XM_SET_MODE:
#if F_APP_A2DP_SOURCE_SUPPORT
    case CMD_INQUIRY:
#endif
    case CMD_BT_HFP_SCO_MAG:
        break;

    case CMD_GCSS_ADD...CMD_GCSS_WRITE_RSP:
        break;

    case CMD_GCSC_SERV_DISCOVER_ALL...CMD_GCSC_DISCOVER_ALL:
        break;

    default:
        {
            ack_pkt[2] = CMD_SET_STATUS_UNKNOW_CMD;
            app_cmd_set_event_ack(cmd_path, app_idx, ack_pkt);
        }
        break;
    }

    APP_PRINT_TRACE1("app_handle_cmd_set--: ack_status 0x%02x", ack_pkt[2]);
}
