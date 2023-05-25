/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#include <stdlib.h>
#include "os_mem.h"
#include "os_timer.h"
#include "os_sync.h"
#include "console.h"
#include "gap.h"
#include "gap_br.h"
#include "trace.h"
#include "bt_types.h"
#include "rtl_gpio.h"
#include "app_timer.h"
#include "transmit_service.h"
#include "bt_iap.h"
#include "app_transfer.h"
#include "app_cmd.h"
#include "app_cfg.h"
#include "bt_spp.h"
#include "app_sdp.h"
#include "dp_ble_info.h"
#include "dp_br_info.h"
#include "gap_msg.h"

#define UART_TX_QUEUE_NO                    16
#define DT_QUEUE_NO                         16

#define DT_STATUS_IDLE                      0
#define DT_STATUS_ACTIVE                    1

typedef enum
{
    APP_TIMER_LAUNCH_APP,
    APP_TIMER_UART_ACK,
    APP_TIMER_UART_WAKE_UP,
    APP_TIMER_UART_TX_WAKE_UP,
    APP_TIMER_DATA_TRANSFER,
} T_APP_TRANSFER_TIMER;

typedef struct
{
    uint8_t     *pkt_ptr;
    uint16_t    pkt_len;
    uint8_t     active;
    uint8_t     link_idx;
} T_DT_QUEUE;

typedef struct
{
    uint8_t     dt_queue_w_idx;
    uint8_t     dt_queue_r_idx;
    uint8_t     dt_resend_count;
    uint8_t     dt_status;
} T_DT_QUEUE_CTRL;

typedef struct
{
    uint8_t     *packet_ptr;
    uint16_t    packet_len;
    uint8_t     active;
    uint8_t     extra_param;
} T_UART_TX_QUEUE;

static uint8_t app_transfer_timer_id = 0;

static uint8_t timer_idx_launch_app[MAX_BR_LINK_NUM] = {0};
static uint8_t timer_idx_uart_ack = 0;
static uint8_t timer_idx_data_transfer = 0;

static T_DT_QUEUE_CTRL     dt_queue_ctrl;
static T_DT_QUEUE          dt_queue[DT_QUEUE_NO];

static T_UART_TX_QUEUE uart_tx_queue[UART_TX_QUEUE_NO];
static uint8_t uart_tx_rIndex;                 /**<uart transfer packet read index*/
static uint8_t uart_tx_wIndex;                 /**<uart transfer packet write index*/
static uint8_t uart_resend_count;              /**<uart resend count*/


void app_transfer_queue_recv_ack_check(uint16_t event_id, uint8_t cmd_path)
{
    uint16_t    tx_queue_id = ((dt_queue[dt_queue_ctrl.dt_queue_r_idx].pkt_ptr[4]) |
                               (dt_queue[dt_queue_ctrl.dt_queue_r_idx].pkt_ptr[5] << 8));

    bool move_to_next = (event_id == tx_queue_id) ? true : false;

    app_pop_data_transfer_queue(cmd_path, move_to_next);
}

void app_transfer_queue_reset(uint8_t cmd_path)
{
    if (cmd_path == CMD_PATH_SPP || cmd_path == CMD_PATH_LE || cmd_path == CMD_PATH_IAP)
    {
        app_stop_timer(&timer_idx_data_transfer);

        for (uint8_t idx = 0 ; idx < DT_QUEUE_NO; idx++)
        {
            if (dt_queue[idx].active)
            {
                dt_queue[idx].active = 0;

                if (dt_queue[idx].pkt_ptr != NULL)
                {
                    free(dt_queue[idx].pkt_ptr);
                    dt_queue[idx].pkt_ptr = NULL;
                }
            }
        }

        dt_queue_ctrl.dt_queue_w_idx = 0;
        dt_queue_ctrl.dt_queue_r_idx = 0;
        dt_queue_ctrl.dt_resend_count = 0;
        dt_queue_ctrl.dt_status = DT_STATUS_IDLE;
    }
}

void app_pop_data_transfer_queue(uint8_t cmd_path, bool next_flag)
{
    T_UART_TX_QUEUE *p_queue;
    uint8_t     app_idx;
    uint8_t     *pkt_ptr;
    uint16_t    pkt_len;
    uint16_t    event_id;

    APP_PRINT_TRACE2("app_pop_data_transfer_queue: cmd_path %d, next_flag %d", cmd_path, next_flag);

    if (CMD_PATH_UART == cmd_path)
    {
        T_UART_TX_QUEUE *tx_queue;
        app_stop_timer(&timer_idx_uart_ack);

        if (next_flag)
        {
            tx_queue = &(uart_tx_queue[uart_tx_rIndex]);
            if (tx_queue->active)
            {
                uart_resend_count = 0;
                event_id = ((tx_queue->packet_ptr[3]) | (tx_queue->packet_ptr[4] << 8));

                if (event_id == EVENT_LEGACY_DATA_TRANSFER)
                {
                    uint8_t type;

                    type = tx_queue->packet_ptr[6];
                    if ((type == PKT_TYPE_SINGLE) || (type == PKT_TYPE_END))
                    {
                        uint8_t app_index;
                        uint8_t local_server_chann =  RFC_SPP_CHANN_NUM;

                        app_index = tx_queue->packet_ptr[5];
                        bt_spp_credits_give(br_db.br_link[app_index].bd_addr, local_server_chann, tx_queue->extra_param);
                    }
                }
                free(tx_queue->packet_ptr);
                tx_queue->active = 0;
                uart_tx_rIndex++;
                if (uart_tx_rIndex >= UART_TX_QUEUE_NO)
                {
                    uart_tx_rIndex = 0;
                }
            }
        }

        p_queue = &(uart_tx_queue[uart_tx_rIndex]);
        if (p_queue->active)
        {
            //app_dlps_disable(APP_DLPS_ENTER_CHECK_CONSOLE);
//            if (console_cfg_const.enable_tx_wake_up)
//            {
//                gpio_write_output_level(console_cfg_const.tx_wake_up_pinmux, 0);
//                app_start_timer(&timer_idx_uart_tx_wake_up, "uart_tx_wake_up",
//                                app_transfer_timer_id, APP_TIMER_UART_TX_WAKE_UP, 0, false,
//                                10);
//            }
//            else
            {
                console_write(p_queue->packet_ptr, p_queue->packet_len);

                event_id = ((p_queue->packet_ptr[4]) | (p_queue->packet_ptr[5] << 8));
                if ((event_id == EVENT_ACK) || (event_id == EVENT_RECORDING_DATA))
                {
                    app_pop_data_transfer_queue(CMD_PATH_UART, true);
                }
                else
                {
                    //wait ack or timeout
                    app_start_timer(&timer_idx_uart_ack, "uart_ack",
                                    app_transfer_timer_id, APP_TIMER_UART_ACK, event_id, false,
                                    800);
                }
            }
        }
        else
        {
//            if (console_cfg_const.enable_tx_wake_up)
//            {
//                gpio_write_output_level(console_cfg_const.tx_wake_up_pinmux, 1);
//            }
        }
    }
    else
    {
        if (next_flag == true)
        {
            app_stop_timer(&timer_idx_data_transfer);
            if (dt_queue[dt_queue_ctrl.dt_queue_r_idx].active)
            {
                dt_queue[dt_queue_ctrl.dt_queue_r_idx].active = 0;

                if (dt_queue[dt_queue_ctrl.dt_queue_r_idx].pkt_ptr != NULL)
                {
                    free(dt_queue[dt_queue_ctrl.dt_queue_r_idx].pkt_ptr);
                    dt_queue[dt_queue_ctrl.dt_queue_r_idx].pkt_ptr = NULL;
                }

                dt_queue_ctrl.dt_queue_r_idx++;
                if (dt_queue_ctrl.dt_queue_r_idx == DT_QUEUE_NO)
                {
                    dt_queue_ctrl.dt_queue_r_idx = 0;
                }
            }

            dt_queue_ctrl.dt_resend_count = 0;
            dt_queue_ctrl.dt_status = DT_STATUS_IDLE;
        }

        app_idx = dt_queue[dt_queue_ctrl.dt_queue_r_idx].link_idx;
        pkt_ptr = dt_queue[dt_queue_ctrl.dt_queue_r_idx].pkt_ptr;
        pkt_len = dt_queue[dt_queue_ctrl.dt_queue_r_idx].pkt_len;
        event_id = ((dt_queue[dt_queue_ctrl.dt_queue_r_idx].pkt_ptr[4]) |
                    (dt_queue[dt_queue_ctrl.dt_queue_r_idx].pkt_ptr[5] << 8));

        APP_PRINT_INFO8("app_pop_data_transfer_queue: dt_status %d, active %d, connected_profile 0x%x, rfc_credit %d, rtk_eap_session_status %d, pkt_len:%d, event_id:0x%x, r idx:%d",
                        dt_queue_ctrl.dt_status,
                        dt_queue[dt_queue_ctrl.dt_queue_r_idx].active,
                        br_db.br_link[app_idx].connected_profile,
                        br_db.br_link[app_idx].rfc_credit,
                        br_db.br_link[app_idx].rtk_eap_session_status,
                        pkt_len, event_id, dt_queue_ctrl.dt_queue_r_idx);

        if (dt_queue_ctrl.dt_status == DT_STATUS_IDLE)
        {
            if (dt_queue[dt_queue_ctrl.dt_queue_r_idx].active == CMD_PATH_IAP)
            {
                if (br_db.br_link[app_idx].connected_profile & IAP_PROFILE_MASK)
                {
                    if (br_db.br_link[app_idx].rtk_eap_session_status == DATA_SESSION_OPEN)
                    {
                        if (bt_iap_data_send(br_db.br_link[app_idx].bd_addr, br_db.br_link[app_idx].rtk_eap_session_id,
                                             pkt_ptr,
                                             pkt_len) == true)
                        {
//                        if (app_cfg_const.enable_embedded_cmd)
                            {
                                if (event_id == EVENT_ACK)
                                {
                                    dt_queue[dt_queue_ctrl.dt_queue_r_idx].active = 0;
                                    free(dt_queue[dt_queue_ctrl.dt_queue_r_idx].pkt_ptr);
                                    dt_queue[dt_queue_ctrl.dt_queue_r_idx].pkt_ptr = NULL;
                                    dt_queue_ctrl.dt_queue_r_idx++;
                                    if (dt_queue_ctrl.dt_queue_r_idx == DT_QUEUE_NO)
                                    {
                                        dt_queue_ctrl.dt_queue_r_idx = 0;
                                    }
//                                  if (app_cfg_const.enable_dsp_capture_data_by_spp == true)
                                    {
                                        app_pop_data_transfer_queue(CMD_PATH_SPP, true);
                                    }
//                                  else
//                                  {
//                                      app_start_timer(&timer_idx_data_transfer, "data_transfer",
//                                                    app_transfer_timer_id, APP_TIMER_DATA_TRANSFER, 0x00, false,
//                                                    1);
//                                  }
                                }
                                else
                                {
//                                  if (app_cfg_const.enable_dsp_capture_data_by_spp == true)
                                    {
#if 0
                                        APP_PRINT_ERROR2("dsp_capture_send_data_decode, id = %x, pkt_len = %x",
                                                         pkt_ptr[13] << 8 | pkt_ptr[12],
                                                         pkt_len);
#endif
                                        app_pop_data_transfer_queue(CMD_PATH_SPP, true);
                                    }
//                                  else
//                                  {
//                                      dt_queue_ctrl.dt_status = DT_STATUS_ACTIVE;
//                                      app_start_timer(&timer_idx_data_transfer, "data_transfer",
//                                                    app_transfer_timer_id, APP_TIMER_DATA_TRANSFER, 0x01, false,
//                                                    2000);
//                                  }
                                }
                            }
                        }
                        else
                        {
                            APP_PRINT_TRACE1("send spp fail, app_idx =%d", app_idx);
//                          if (app_cfg_const.enable_dsp_capture_data_by_spp == true)
                            {
                                app_pop_data_transfer_queue(CMD_PATH_SPP, true);
                            }
//                          else
//                          {
//                                app_start_timer(&timer_idx_data_transfer, "data_transfer",
//                                        app_transfer_timer_id, APP_TIMER_DATA_TRANSFER, 0x00, false,
//                                        100);
//                          }
                        }
                    }
                    else if (br_db.br_link[app_idx].rtk_eap_session_status == DATA_SESSION_CLOSE)
                    {
                        if (br_db.br_link[app_idx].iap_authen_flag)
                        {

                            char boundle_id[] = "com.realtek.EADemo2";
                            T_BT_IAP_APP_LAUNCH_METHOD method = BT_IAP_APP_LAUNCH_WITH_USER_ALERT;
                            bt_iap_app_launch(br_db.br_link[app_idx].bd_addr, boundle_id, sizeof(boundle_id), method);
                            br_db.br_link[app_idx].rtk_eap_session_status = DATA_SESSION_LAUNCH;

                            app_start_timer(&timer_idx_launch_app[app_idx], "launch_app",
                                            app_transfer_timer_id, APP_TIMER_LAUNCH_APP, app_idx, false,
                                            60000);
                        }
                        else
                        {
                            free(dt_queue[dt_queue_ctrl.dt_queue_r_idx].pkt_ptr);
                            dt_queue[dt_queue_ctrl.dt_queue_r_idx].pkt_ptr = NULL;
                            dt_queue[dt_queue_ctrl.dt_queue_r_idx].active = 0;
                            dt_queue_ctrl.dt_queue_r_idx++;
                            if (dt_queue_ctrl.dt_queue_r_idx == DT_QUEUE_NO)
                            {
                                dt_queue_ctrl.dt_queue_r_idx = 0;
                            }
                            app_start_timer(&timer_idx_data_transfer, "data_transfer",
                                            app_transfer_timer_id, APP_TIMER_DATA_TRANSFER, 0x00, false,
                                            1);
                        }
                    }
                }
            }
            else if (dt_queue[dt_queue_ctrl.dt_queue_r_idx].active == CMD_PATH_SPP)
            {
                if (br_db.br_link[app_idx].connected_profile & SPP_PROFILE_MASK)
                {
                    if (br_db.br_link[app_idx].rfc_credit)
                    {
                        uint8_t local_server_chann =  RFC_SPP_CHANN_NUM;

                        if (bt_spp_data_send(br_db.br_link[app_idx].bd_addr, local_server_chann,
                                             pkt_ptr, pkt_len, false) == true)
                        {
//                        if (app_cfg_const.enable_embedded_cmd)
                            {
                                if (event_id == EVENT_ACK)
                                {
                                    dt_queue[dt_queue_ctrl.dt_queue_r_idx].active = 0;
                                    free(dt_queue[dt_queue_ctrl.dt_queue_r_idx].pkt_ptr);
                                    dt_queue[dt_queue_ctrl.dt_queue_r_idx].pkt_ptr = NULL;
                                    dt_queue_ctrl.dt_queue_r_idx++;
                                    if (dt_queue_ctrl.dt_queue_r_idx == DT_QUEUE_NO)
                                    {
                                        dt_queue_ctrl.dt_queue_r_idx = 0;
                                    }

//                                if (app_cfg_const.enable_dsp_capture_data_by_spp == true)
//                                {
//                                    app_pop_data_transfer_queue(CMD_PATH_SPP, true);
//                                }
//                                else
                                    {
                                        app_start_timer(&timer_idx_data_transfer, "data_transfer",
                                                        app_transfer_timer_id, APP_TIMER_DATA_TRANSFER, 0x00, false,
                                                        1);
                                    }
                                }
                                else
                                {
//                                if (app_cfg_const.enable_dsp_capture_data_by_spp == true)
//                                {
//#if 0
//                                    APP_PRINT_ERROR2("dsp_capture_send_data_decode, id = %x, pkt_len = %x",
//                                                     pkt_ptr[13] << 8 | pkt_ptr[12],
//                                                     pkt_len);
//#endif
//                                    app_pop_data_transfer_queue(CMD_PATH_SPP, true);
//                                }
//                                else
                                    {
                                        dt_queue_ctrl.dt_status = DT_STATUS_ACTIVE;
                                        app_start_timer(&timer_idx_data_transfer, "data_transfer",
                                                        app_transfer_timer_id, APP_TIMER_DATA_TRANSFER, 0x01, false,
                                                        2000);
                                    }
                                }
                            }
                        }
                        else
                        {
                            APP_PRINT_TRACE1("send spp fail, app_idx =%d", app_idx);
//                        if (app_cfg_const.enable_dsp_capture_data_by_spp == true)
//                        {
//                            app_pop_data_transfer_queue(CMD_PATH_SPP, true);
//                        }
//                        else
                            {
                                app_start_timer(&timer_idx_data_transfer, "data_transfer",
                                                app_transfer_timer_id, APP_TIMER_DATA_TRANSFER, 0x00, false,
                                                100);
                            }
                        }
                    }
                    else
                    {
                        free(dt_queue[dt_queue_ctrl.dt_queue_r_idx].pkt_ptr);
                        dt_queue[dt_queue_ctrl.dt_queue_r_idx].pkt_ptr = NULL;
                        dt_queue[dt_queue_ctrl.dt_queue_r_idx].active = 0;
                        dt_queue_ctrl.dt_queue_r_idx++;
                        if (dt_queue_ctrl.dt_queue_r_idx == DT_QUEUE_NO)
                        {
                            dt_queue_ctrl.dt_queue_r_idx = 0;
                        }
                        dt_queue_ctrl.dt_resend_count = 0;
                        app_start_timer(&timer_idx_data_transfer, "data_transfer",
                                        app_transfer_timer_id, APP_TIMER_DATA_TRANSFER, 0x00, false,
                                        1);
                    }
                }
            }
            else if (dt_queue[dt_queue_ctrl.dt_queue_r_idx].active == CMD_PATH_LE)
            {
                if (le_db.le_link[app_idx].state == GAP_CONN_STATE_CONNECTED)
                {
                    if (le_db.le_link[app_idx].transmit_srv_tx_enable_fg == (0x01 | 0x02))//TX_ENABLE_READY
                    {
                        if (transmit_srv_tx_data(le_db.le_link[app_idx].conn_id, pkt_len, pkt_ptr) == true)
                        {
                            dt_queue_ctrl.dt_status = DT_STATUS_ACTIVE;
//                        if (app_cfg_const.enable_embedded_cmd)
                            {
                                if (event_id != EVENT_ACK)
                                {
                                    app_start_timer(&timer_idx_data_transfer, "data_transfer",
                                                    app_transfer_timer_id, APP_TIMER_DATA_TRANSFER, 0x01, false,
                                                    2000);
                                }
                            }
                        }
                        else
                        {
                            app_start_timer(&timer_idx_data_transfer, "data_transfer",
                                            app_transfer_timer_id, APP_TIMER_DATA_TRANSFER, 0x00, false,
                                            100);
                        }
                    }
                    else
                    {
                        app_start_timer(&timer_idx_data_transfer, "data_transfer",
                                        app_transfer_timer_id, APP_TIMER_DATA_TRANSFER, 0x00, false,
                                        1000);
                    }
                }
                else
                {
                    free(dt_queue[dt_queue_ctrl.dt_queue_r_idx].pkt_ptr);
                    dt_queue[dt_queue_ctrl.dt_queue_r_idx].pkt_ptr = NULL;
                    dt_queue[dt_queue_ctrl.dt_queue_r_idx].active = 0;
                    dt_queue_ctrl.dt_queue_r_idx++;
                    if (dt_queue_ctrl.dt_queue_r_idx == DT_QUEUE_NO)
                    {
                        dt_queue_ctrl.dt_queue_r_idx = 0;
                    }
                    dt_queue_ctrl.dt_resend_count = 0;
                    //set timer to pop queue
                    app_start_timer(&timer_idx_data_transfer, "data_transfer",
                                    app_transfer_timer_id, APP_TIMER_DATA_TRANSFER, 0x00, false,
                                    1);
                }
            }
        }
    }
}

bool app_push_data_transfer_queue(uint8_t cmd_path, uint8_t *data, uint16_t data_len,
                                  uint8_t extra_param)
{
    APP_PRINT_TRACE4("app_push_data_transfer_queue: cmd_path %d, data_len %d w_idx %d r_idx %d",
                     cmd_path,
                     data_len,
                     dt_queue_ctrl.dt_queue_w_idx,
                     dt_queue_ctrl.dt_queue_r_idx);

    if (CMD_PATH_UART == cmd_path)
    {
        if (uart_tx_queue[uart_tx_wIndex].active == 0)
        {
            uart_tx_queue[uart_tx_wIndex].active = 1;
            uart_tx_queue[uart_tx_wIndex].packet_ptr = data;
            uart_tx_queue[uart_tx_wIndex].packet_len = data_len;
            uart_tx_queue[uart_tx_wIndex].extra_param = extra_param;
            uart_tx_wIndex++;

            APP_PRINT_TRACE2("app_push_data_transfer_queue: uart,w_idx %d r_idx %d",
                             uart_tx_wIndex,
                             uart_tx_rIndex);
            if (uart_tx_wIndex >= UART_TX_QUEUE_NO)
            {
                uart_tx_wIndex = 0;
                if (uart_tx_rIndex == (UART_TX_QUEUE_NO - 1))
                {
                    app_pop_data_transfer_queue(cmd_path, false);
                }
            }
            else if ((uart_tx_wIndex - uart_tx_rIndex) == 1)
            {
                app_pop_data_transfer_queue(cmd_path, false);
            }
            return true;
        }
        else
        {
            APP_PRINT_TRACE0("app_push_data_transfer_queue: uart_tx_queue full");
            return false;
        }
    }
    else
    {
        if (dt_queue[dt_queue_ctrl.dt_queue_w_idx].active == 0)
        {
            dt_queue[dt_queue_ctrl.dt_queue_w_idx].active = cmd_path;
            dt_queue[dt_queue_ctrl.dt_queue_w_idx].link_idx = extra_param;
            dt_queue[dt_queue_ctrl.dt_queue_w_idx].pkt_ptr = data;
            dt_queue[dt_queue_ctrl.dt_queue_w_idx].pkt_len = data_len;
            dt_queue_ctrl.dt_queue_w_idx++;
            if (dt_queue_ctrl.dt_queue_w_idx == DT_QUEUE_NO)
            {
                dt_queue_ctrl.dt_queue_w_idx = 0;
            }

            app_pop_data_transfer_queue(cmd_path, false);
            return true;
        }
        else
        {
            APP_PRINT_TRACE1("app_push_data_transfer_queue: dt_queue[%d] full", dt_queue_ctrl.dt_queue_w_idx);
            return false;
        }
    }
}

bool app_transfer_check_active(uint8_t cmd_path)
{
    if (CMD_PATH_UART == cmd_path)
    {
        if (uart_tx_queue[uart_tx_wIndex].active == 0)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        if (dt_queue[dt_queue_ctrl.dt_queue_w_idx].active == 0)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
}

static void app_transfer_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    APP_PRINT_TRACE2("app_transfer_timeout_cb: timer_id %d, timer_chann %d", timer_evt, param);

    switch (timer_evt)
    {

    case APP_TIMER_LAUNCH_APP:
        app_stop_timer(&timer_idx_launch_app[param]);
        if (br_db.br_link[param].rtk_eap_session_status == DATA_SESSION_LAUNCH)
        {
            br_db.br_link[param].rtk_eap_session_status = DATA_SESSION_CLOSE;
        }
        break;

    case APP_TIMER_UART_ACK:
        app_stop_timer(&timer_idx_uart_ack);
        uart_resend_count++;
        APP_PRINT_INFO1("APP_TRANSFER_TIMER_UART_ACK_TO count = %d", uart_resend_count);
        if (uart_resend_count > console_cfg_const.dt_resend_num)
        {
            APP_PRINT_INFO0("uart timeout true");
            app_pop_data_transfer_queue(CMD_PATH_UART, true);
        }
        else
        {
            APP_PRINT_INFO0("uart timeout false");
            app_pop_data_transfer_queue(CMD_PATH_UART, false);
        }
        break;

    case APP_TIMER_DATA_TRANSFER:
        app_stop_timer(&timer_idx_data_transfer);
        dt_queue_ctrl.dt_status = DT_STATUS_IDLE;
        if (param == 0x01) //ack timeout
        {
            dt_queue_ctrl.dt_resend_count++;
            //dt_queue_ctrl.dt_status = DT_STATUS_IDLE;
            if (dt_queue_ctrl.dt_resend_count >= console_cfg_const.dt_resend_num)
            {
                app_pop_data_transfer_queue(dt_queue[dt_queue_ctrl.dt_queue_r_idx].active, true);
            }
            else
            {
                app_pop_data_transfer_queue(dt_queue[dt_queue_ctrl.dt_queue_r_idx].active, false);
            }
        }
        else
        {
            app_pop_data_transfer_queue(dt_queue[dt_queue_ctrl.dt_queue_r_idx].active, false);
        }
        break;

    default:
        break;
    }
}

void app_transfer_init(void)
{
    //app_db.external_mcu_mtu = 256;

    app_timer_reg_cb(app_transfer_timeout_cb, &app_transfer_timer_id);
}
