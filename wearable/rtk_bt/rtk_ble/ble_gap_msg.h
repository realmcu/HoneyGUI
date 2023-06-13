/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file
   * @brief     This file handles BLE peripheral application routines.
   * @author    jane
   * @date      2017-06-06
   * @version   v1.0
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
   **************************************************************************************
  */

#ifndef _BLE_GAP_MSG_APP__
#define _BLE_GAP_MSG_APP__

#ifdef __cplusplus
extern "C" {
#endif

#include "app_msg.h"
#include "stdbool.h"
#include "stddef.h"
#include "structure_list.h"




typedef void (*P_LE_MSG_HANDLER_CBACK)(T_IO_MSG *p_gap_msg);

typedef struct _T_LE_MSG_CBACK_ITEM
{
    slist_t slist;
    P_LE_MSG_HANDLER_CBACK              cback;
} T_LE_MSG_CBACK_ITEM;

void app_handle_io_msg(T_IO_MSG io_msg);

void le_msg_handler_cback_register(P_LE_MSG_HANDLER_CBACK cback);
void le_msg_handler_cback_unregister(P_LE_MSG_HANDLER_CBACK cback);
void app_msg_handler_cback_register(P_LE_MSG_HANDLER_CBACK cback);
void app_msg_handler_cback_unregister(P_LE_MSG_HANDLER_CBACK cback);


#ifdef __cplusplus
}
#endif

#endif

