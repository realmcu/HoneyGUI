/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <string.h>
#include "trace.h"
#include "btm.h"
#include "gap_br.h"
#include "bt_spp.h"
#include "app_sdp.h"
#include "app_spp.h"
#include "app_br_link_util.h"
#include "bt_spp.h"

/** @brief  SPP UUID */
static const uint8_t spp_service_class_uuid128[16] =
{
    0x00, 0x00, 0x11, 0x01, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5f, 0x9b, 0x34, 0xfb
};


static bool is_spp_server = true;//APP SPP role flag; false: client, true: server.


/**
 * @brief     SPP Tx function for test.
 * @param[in] Remote BT address..
 * @return    void
 */
static void spp_app_tx_data_test(uint8_t *bd_addr)
{
    T_APP_BR_LINK *p_link;
    uint8_t spp_tx_data_test[15] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x08, 0x09, 0x0a};

    p_link = app_find_br_link(bd_addr);
    if (p_link != NULL)
    {
        APP_PRINT_TRACE1("spp_app_tx_data_test: p_link->rfc_spp_credit %d",
                         p_link->rfc_spp_credit);
        if (p_link->rfc_spp_credit)
        {
            bt_spp_data_send(bd_addr, RFC_SPP_CHANN_NUM, spp_tx_data_test, sizeof(spp_tx_data_test),
                             false);
        }
    }
}


/**
 * @brief     SPP Rx function for test.
 * @param[in] Remote BT address..
 * @param[in] Pointer to SPP Rx buffer.
 * @param[in] SPP Rx buffer length.
 * @return    void
 */
static void spp_app_rx_data_test(uint8_t *bd_addr, uint8_t *buf, uint16_t len)
{
    APP_PRINT_TRACE2("spp_app_rx_data_test: bd_addr %s len %d", TRACE_BDADDR(bd_addr), len);
    for (uint16_t i = 0; i < len; i++)
    {
        APP_PRINT_TRACE2("spp_app_rx_data_test: buf[i:%d] 0x%x", i, buf[i]);
    }

    if (spp_demo_role_get())
    {
#if (DLPS_EN == 1)
        bt_sniff_mode_enable(bd_addr, 500);
#endif
    }
    else
    {
        spp_app_tx_data_test(bd_addr);
    }
}


/**
 * @brief    BT Manager SPP related events are handled in this function
 * @note     Then the event handling function shall be called according to the
 *           event_type of T_BT_EVENT.
 * @param[in] event_type BT manager event type
 * @param[in] event_buf  Pointer to BT manager event buffer.
 * @param[in] buf_len    BT manager event buffer length.
 * @return   void
 */
static void spp_app_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BT_EVENT_PARAM *param = event_buf;
    T_APP_BR_LINK *p_link;
    bool handle = true;

    switch (event_type)
    {

    case BT_EVENT_SPP_CONN_CMPL:
        {
            APP_PRINT_INFO0(" BT_EVENT_SPP_CONN_CMPL\n");
            if (param->spp_conn_cmpl.local_server_chann != RFC_SPP_CHANN_NUM)
            {
                return;
            }

            p_link = app_find_br_link(param->spp_conn_cmpl.bd_addr);
            if (p_link != NULL)
            {
                p_link->rfc_spp_credit = param->spp_conn_cmpl.link_credit;
                p_link->rfc_spp_frame_size = param->spp_conn_cmpl.frame_size;

                //check credit
                if (p_link->rfc_spp_credit)
                {
                    //spp send first data
                    if (spp_demo_role_get())
                    {
                        spp_app_tx_data_test(param->spp_conn_cmpl.bd_addr);
                    }
                }
            }
        }
        break;

    case BT_EVENT_SPP_CREDIT_RCVD:
        {
            if (param->spp_credit_rcvd.local_server_chann != RFC_SPP_CHANN_NUM)
            {
                return;
            }

            p_link = app_find_br_link(param->spp_credit_rcvd.bd_addr);
            if (p_link == NULL)
            {
                APP_PRINT_ERROR0("spp_app_bt_cback: no acl link found");
                return;
            }

            if ((p_link->rfc_spp_credit == 0) && (param->spp_credit_rcvd.link_credit))
            {
                //send data when spp tx credit is no more 0 or send first data.
                if (spp_demo_role_get())
                {
                    spp_app_tx_data_test(param->spp_conn_cmpl.bd_addr);
                }
            }
            p_link->rfc_spp_credit = param->spp_credit_rcvd.link_credit;
        }
        break;

    case BT_EVENT_SPP_DATA_IND:
        {
            if (param->spp_data_ind.local_server_chann != RFC_SPP_CHANN_NUM)
            {
                return;
            }

            p_link = app_find_br_link(param->spp_data_ind.bd_addr);
            if (p_link == NULL)
            {
                APP_PRINT_ERROR0("spp_app_bt_cback: no acl link found");
                return;
            }
            //spp data recieved.
            spp_app_rx_data_test(param->spp_data_ind.bd_addr, param->spp_data_ind.data,
                                 param->spp_data_ind.len);
            bt_spp_credits_give(param->spp_data_ind.bd_addr, param->spp_data_ind.local_server_chann,
                                1);
        }
        break;

    case BT_EVENT_SPP_CONN_IND:
        {
            if (param->spp_conn_ind.local_server_chann != RFC_SPP_CHANN_NUM)
            {
                return;
            }

            p_link = app_find_br_link(param->spp_conn_ind.bd_addr);
            if (p_link == NULL)
            {
                APP_PRINT_ERROR0("spp_app_bt_cback: no acl link found");
                return;
            }

            uint8_t local_server_chann = param->spp_conn_ind.local_server_chann;
            uint16_t frame_size = param->spp_conn_ind.frame_size;
            bt_spp_connect_cfm(p_link->bd_addr, local_server_chann, true, frame_size,
                               7);
        }
        break;

    case BT_EVENT_SPP_DISCONN_CMPL:
        {
            if (param->spp_disconn_cmpl.local_server_chann != RFC_SPP_CHANN_NUM)
            {
                return;
            }

            p_link = app_find_br_link(param->spp_disconn_cmpl.bd_addr);
            if (p_link != NULL)
            {
                p_link->rfc_spp_credit = 0;
            }
        }
        break;
    default:
        handle = false;
        break;
    }

    if (handle == true)
    {
        APP_PRINT_INFO1("spp_app_bt_cback: event_type 0x%04x", event_type);
    }
}



void app_spp_init(void)
{
    APP_PRINT_INFO0("app_spp_init");

    //initialize the SPP capability with SPP_DEMO_LINK_NUM and SPP_DEMO_RFC_SPP_SERVICE_NUM;
    //only one SPP server channel exists in the unique link.
    bt_spp_init(MAX_BR_LINK_NUM, 0x03);

    //register spp uuid together with local server channel.
    //register spp service with uuid.
    bt_spp_service_register((uint8_t *)spp_service_class_uuid128, RFC_SPP_CHANN_NUM);
    bt_mgr_cback_register(spp_app_bt_cback);


    //enable or disable ERTM mode
    bt_spp_ertm_mode_set(false);//disable spp l2cap ERTM mode by default.

}

bool spp_demo_role_get(void)
{
    return is_spp_server;
}

void spp_demo_role_set(bool role)
{
    is_spp_server = role;
}

