/**
*****************************************************************************************
*     Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file      sensor_task.c
   * @brief     Routines to create sensor task and handle events & messages
   * @author    howie_wang
   * @date      2023-06-07
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
#include <app_msg.h>
#include <string.h>
#include "board.h"
#include "trace.h"
#include "hp_msg_test.h"


/*============================================================================*
 *                              Variables
 *============================================================================*/
static void *hp_task_handle;    //!< Hp Task handle
static void *hp_queue_handle;   //!< Hp queue handle


bool app_send_msg_to_hp_task(T_IO_MSG *p_msg)
{
    if (os_msg_send(hp_queue_handle, p_msg, 0) == false)
    {
        APP_PRINT_ERROR0("send_io_msg_to_app fail sensor queue");
        return false;
    }
    return true;
}

/**
 * @brief        Hp task to handle events & messages
 * @param[in]    p_param    Parameters sending to the task
 * @return       void
 */
static void hp_task_entry(void *p_param)
{
    os_alloc_secure_ctx(1024);

    os_msg_queue_create(&hp_queue_handle, "hpQ", 20, sizeof(T_IO_MSG));

    while (true)
    {
        T_IO_MSG io_msg;
        if (os_msg_recv(hp_queue_handle, &io_msg, 0xFFFFFFFF) == true)
        {
            switch (io_msg.subtype)
            {
            case HP_MSG_UART_TEST:
                {
                    DBG_DIRECT("HP_MSG_UART_TEST");
                }
                break;

            default:
                break;
            }
        }
    }
}

void hp_task_init(void)
{
    os_task_create(&hp_task_handle, "hp", hp_task_entry, NULL, 1024, 1);
}

/** @} */ /* End of group PERIPH_APP_TASK */


