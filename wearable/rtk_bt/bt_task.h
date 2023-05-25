/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file      app_task.h
   * @brief     Routines to create App task and handle events & messages
   * @author    jane
   * @date      2017-06-02
   * @version   v1.0
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
   **************************************************************************************
  */
#ifndef _BT_TASK_H_
#define _BT_TASK_H_
#include "app_msg.h"
#include "stdbool.h"

/** @defgroup PERIPH_APP_TASK Peripheral App Task
  * @brief Peripheral App Task
  * @{
  */

extern void driver_init(void);

/**
 * @brief  Initialize App task
 * @return void
 */
void app_task_init(void);

bool app_send_msg_to_bt_task(T_IO_MSG *p_msg);
/** End of PERIPH_APP_TASK
* @}
*/


#endif

