/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#include <stdlib.h>
#include <string.h>
#include "trace.h"
#include "os_mem.h"
#include "console.h"
#include "app_cmd.h"
#include "app_link_util.h"
#include "app_transfer.h"
#include "app_report.h"
#include "app_transfer.h"
#include "app_cfg.h"
#include "app_util.h"
#include "dp_ble_info.h"
#include "dp_br_info.h"
#include "gap_msg.h"

static uint8_t uart_tx_seqn = 0;

static void app_report_uart_event(uint16_t event_id, uint8_t *data, uint16_t len)
{
    if (console_get_mode() != CONSOLE_MODE_BINARY)
    {
        return;
    }

    if (console_cfg_const.enable_data_uart)
    {
        uint8_t *buf;

        buf = malloc(len + 7);
        if (buf == NULL)
        {
            return;
        }

        uart_tx_seqn++;
        if (uart_tx_seqn == 0)
        {
            uart_tx_seqn = 1;
        }

        buf[0] = CMD_SYNC_BYTE;
        buf[1] = uart_tx_seqn;
        buf[2] = (uint8_t)(2 + len);
        buf[3] = (uint8_t)((2 + len) >> 8);
        buf[4] = (uint8_t)event_id;
        buf[5] = (uint8_t)(event_id >> 8);
        if (len)
        {
            memcpy(&buf[6], data, len);
        }
        buf[len + 6] = app_util_calc_checksum(&buf[1], len + 5);
        if (app_push_data_transfer_queue(CMD_PATH_UART, buf, len + 7, 0) == false)
        {
            free(buf);
        }
    }
}


static void app_report_le_event(T_APP_LE_LINK *p_link, uint16_t event_id, uint8_t *data,
                                uint16_t len)
{
    if (p_link->state == GAP_CONN_STATE_CONNECTED)
    {
        uint8_t *buf;

        buf = malloc(len + 6);
        if (buf == NULL)
        {
            return;
        }

        buf[0] = CMD_SYNC_BYTE;
        p_link->tx_event_seqn++;
        if (p_link->tx_event_seqn == 0)
        {
            p_link->tx_event_seqn = 1;
        }
        buf[1] = p_link->tx_event_seqn;
        buf[2] = (uint8_t)(len + 2);
        buf[3] = (uint8_t)((len + 2) >> 8);
        buf[4] = (uint8_t)event_id;
        buf[5] = (uint8_t)(event_id >> 8);

        if (len)
        {
            memcpy(&buf[6], data, len);
        }

        if (app_push_data_transfer_queue(CMD_PATH_LE, buf, len + 6, p_link->id) == false)
        {
            free(buf);
        }
    }
}

static void app_report_spp_event(T_APP_BR_LINK *p_link, uint16_t event_id, uint8_t *data,
                                 uint16_t len)
{
    if (p_link->connected_profile & SPP_PROFILE_MASK)
    {
        uint8_t *buf;

        buf = malloc(len + 6);
        if (buf == NULL)
        {
            return;
        }

        buf[0] = CMD_SYNC_BYTE;
        p_link->tx_event_seqn++;
        if (p_link->tx_event_seqn == 0)
        {
            p_link->tx_event_seqn = 1;
        }
        buf[1] = p_link->tx_event_seqn;
        buf[2] = (uint8_t)(len + 2);
        buf[3] = (uint8_t)((len + 2) >> 8);
        buf[4] = (uint8_t)event_id;
        buf[5] = (uint8_t)(event_id >> 8);

        if (len)
        {
            memcpy(&buf[6], data, len);
        }

        if (app_push_data_transfer_queue(CMD_PATH_SPP, buf, len + 6, p_link->id) == false)
        {
            free(buf);
        }
    }
}

static void app_report_iap_event(T_APP_BR_LINK *p_link, uint16_t event_id, uint8_t *data,
                                 uint16_t len)
{
    if (p_link->connected_profile & IAP_PROFILE_MASK)
    {
        uint8_t *buf;

        buf = malloc(len + 6);
        if (buf == NULL)
        {
            return;
        }

        buf[0] = CMD_SYNC_BYTE;
        p_link->tx_event_seqn++;
        if (p_link->tx_event_seqn == 0)
        {
            p_link->tx_event_seqn = 1;
        }
        buf[1] = p_link->tx_event_seqn;
        buf[2] = (uint8_t)(len + 2);
        buf[3] = (uint8_t)((len + 2) >> 8);
        buf[4] = (uint8_t)event_id;
        buf[5] = (uint8_t)(event_id >> 8);

        if (len)
        {
            memcpy(&buf[6], data, len);
        }

        if (app_push_data_transfer_queue(CMD_PATH_IAP, buf, len + 6, p_link->id) == false)
        {
            free(buf);
        }
    }
}

void app_report_event(uint8_t cmd_path, uint16_t event_id, uint8_t app_index, uint8_t *data,
                      uint16_t len)
{
    APP_PRINT_TRACE4("app_report_event: cmd_path %d, event_id 0x%04x, app_index %d, data_len %d",
                     cmd_path,
                     event_id, app_index, len);

    switch (cmd_path)
    {
    case CMD_PATH_UART:
        app_report_uart_event(event_id, data, len);
        break;

    case CMD_PATH_LE:
        if (app_index < MAX_BLE_LINK_NUM)
        {
            app_report_le_event(&le_db.le_link[app_index], event_id, data, len);
        }
        break;

    case CMD_PATH_SPP:
        if (app_index < MAX_BR_LINK_NUM)
        {
            app_report_spp_event(&br_db.br_link[app_index], event_id, data, len);
        }
        break;

    case CMD_PATH_IAP:
        if (app_index < MAX_BR_LINK_NUM)
        {
            app_report_iap_event(&br_db.br_link[app_index], event_id, data, len);
        }
        break;

    default:
        break;
    }
}

