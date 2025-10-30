#include <gui_tabview.h>
/**
*********************22, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file      app_tb_conn.c
   * @brief     Source file for chargebox project, mainly used for initializing modules
   * @author    sienna_shen
   * @date      2024-05-22
   * @version   v1.0
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2022 Realtek Semiconductor Corporation</center></h2>
   **************************************************************************************
  */

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include <gui_img.h>
#include <gui_win.h>
#include <gui_text.h>
#include <string.h>
#include <stdio.h>
#include "gui_tab.h"
#include "gui_page.h"
#include "gui_button.h"
#include "app_chargebox.h"
#ifndef _WIN32
#include "gap_scan.h"
#include "gap_bond_le.h"
#include "gap_conn_le.h"
#include "gcs_client.h"
#include "link_mgr.h"
#include "app_le_transfer.h"
#include "platform_utils.h"
#include "app_task.h"
#endif

extern uint8_t sample_addr[6];

gui_page_t *page_device;
bool find_srv = false;

void le_scan_and_showdev(void)
{
#ifndef _WIN32
    le_bond_clear_all_keys();
    T_IO_MSG io_gui_msg_send;
    io_gui_msg_send.type = IO_MSG_TYPE_GUI;
    io_gui_msg_send.subtype = GUI_BLE_SUBEVENT_SCAN;
    if (false == app_send_msg_to_apptask(&io_gui_msg_send))
    {
        gui_log("scan_and_showdev fail");
    }
#endif
}

void le_conn_and_findsrv(void)
{
#ifndef _WIN32
    le_scan_stop();

    T_GAP_LE_CONN_REQ_PARAM conn_req_param;

    conn_req_param.scan_interval = 0x10;
    conn_req_param.scan_window = 0x10;
    conn_req_param.conn_interval_min = 80;
    conn_req_param.conn_interval_max = 80;
    conn_req_param.conn_latency = 0;
    conn_req_param.supv_tout = 1000;
    conn_req_param.ce_len_min = 2 * (conn_req_param.conn_interval_min - 1);
    conn_req_param.ce_len_max = 2 * (conn_req_param.conn_interval_max - 1);
    le_set_conn_param(GAP_CONN_PARAM_1M, &conn_req_param);

    le_connect(GAP_PHYS_CONN_INIT_1M_BIT, sample_addr, GAP_REMOTE_ADDR_LE_PUBLIC,
               GAP_LOCAL_ADDR_LE_PUBLIC, 1000);

    find_srv = true;
#endif
}

void le_disc(void)
{
#ifndef _WIN32
    uint8_t link_num = le_get_active_link_num();
    for (int i = 0; i < link_num; i++)
    {
        le_disconnect(i);
    }
#endif
}

void page_tb_conn(void *parent)
{
    page_device = gui_page_create(parent, "BT_Addr", 0, 40, 385, 200);
    gui_img_t *overlap1 = gui_img_create_from_mem(parent, "overlap", OVERLAP_BIN, 0, -30, 0, 0);
    gui_img_t *overlap2 = gui_img_create_from_mem(parent, "overlap", OVERLAP_BIN, 0, 240, 0, 0);

    gui_button_t *button_scan = gui_button_create(parent, 20, 250, 106, 63, SCAN_BIN, SCAN_BLUE_BIN,
                                                  NULL, BUTTON_BG_ICON, 0);
    gui_button_click(button_scan, (gui_event_cb_t)le_scan_and_showdev,
                     NULL);


    gui_button_t *button_conn = gui_button_create(parent, 140, 250, 106, 63, CONN_BIN, CONN_BLUE_BIN,
                                                  NULL, BUTTON_BG_ICON, 0);
    gui_button_click(button_conn, (gui_event_cb_t)le_conn_and_findsrv,
                     NULL);

    gui_button_t *button_disc = gui_button_create(parent, 260, 250, 106, 63, DISC_BIN, DISC_BLUE_BIN,
                                                  NULL, BUTTON_BG_ICON, 0);
    gui_button_click(button_disc, (gui_event_cb_t)le_disc, NULL);
}

