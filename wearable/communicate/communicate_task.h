/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file      communicate_task.h
   * @brief     Routines to create App task and handle events & messages
   * @author    howie
   * @date      2019-12-26
   * @version   v1.0
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
   **************************************************************************************
  */
#ifndef _COMMUNICATE_TASK_H_
#define _COMMUNICATE_TASK_H_

#include "app_msg.h"
#include "stdbool.h"
#include <communicate_protocol.h>
#include "board.h"
typedef void (* pfunc)(void);
/**
 * @brief  Initialize App task
 * @return void
 */


int communicate_task_init(void);
bool send_msg_to_communicatetask(T_IO_MSG *p_msg);
bool cb_send_to_queue(pfunc func);
#if USE_HRS_MASTER
bool cb_l1receive_to_queue(void *buf);
#endif
#endif