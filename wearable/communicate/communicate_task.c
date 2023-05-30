/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file      communicate_task.c
   * @brief     Routines to create App task and handle events & messages
   * @author    howie
   * @date      2019-12-26
   * @version   v1.0
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
   **************************************************************************************
  */

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include <os_msg.h>
#include <os_task.h>
#include <gap.h>
#include <gap_le.h>
#include <gap_msg.h>
#include <communicate_task.h>
#include <app_msg.h>
#include <ble_task.h>
#include "trace.h"
#include "ble_cb.h"
#include "communicate_protocol.h"
#include "communicate_parse.h"
#include "version.h"
/** @addtogroup  PERIPH_DEMO
    * @{
    */

/** @defgroup  PERIPH_APP_TASK Peripheral App Task
    * @brief This file handles the implementation of application task related functions.
    *
    * Create App task and handle events & messages
    * @{
    */
/*============================================================================*
 *                              Macros
 *============================================================================*/

#define L1SEND_TASK_PRIORITY          1        //!< Task priorities
#define L1SEND_TASK_STACK_SIZE        256 * 4  //!<  Task stack size


/*============================================================================*
 *                              Variables
 *============================================================================*/

void *l1send_task_handle;   //!< APP Task handle
void *l1send_queue_handle;  //!< Event queue handle
void *l1recv_queue_handle;  //!< Event queue handle

/*============================================================================*
 *                              Functions
 *============================================================================*/
void l1send_task(void *p_param);

/**
 * @brief  Initialize App task
 * @return void
 */
void communicate_task_init()
{
    os_task_create(&l1send_task_handle, "l1send", l1send_task, 0, L1SEND_TASK_STACK_SIZE,
                   L1SEND_TASK_PRIORITY);
}

void L1_send_event(L1SEND_TYPE_WRISTBAND event, void *res)
{

}

/**
 * @brief        L1send task
 * @param[in]    p_params    Parameters sending to the task
 * @return       void
 */

void l1send_task(void *pvParameters)
{
    uint8_t event;
    uint8_t psendbuf[30];

    os_msg_queue_create(&l1send_queue_handle, "l1send queue", 0x10, sizeof(uint8_t));
    os_msg_queue_create(&l1recv_queue_handle, "l1recv queue", 0x10, sizeof(uint8_t));

    while (true)
    {
        if (os_msg_recv(l1send_queue_handle, &event, 0xFFFFFFFF) == true)
        {
            APP_PRINT_INFO1("Send Event = %d", event);

        }
    }
}

/** @} */ /* End of group PERIPH_APP_TASK */
/** @} */ /* End of group PERIPH_DEMO */

