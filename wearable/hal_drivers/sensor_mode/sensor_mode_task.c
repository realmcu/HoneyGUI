/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file      app_task.c
   * @brief     Routines to create App task and handle events & messages
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
#include <watch_msg.h>



/*============================================================================*
 *                              Variables
 *============================================================================*/

static void *sensor_mode_queue_handle;   //!< IO queue handle


bool app_send_msg_to_sensor_mode_task(T_IO_MSG *p_msg)
{
    if (os_msg_send(sensor_mode_queue_handle, p_msg, 0) == false)
    {
        APP_PRINT_ERROR0("send_io_msg_to_app fail io queue");
        return false;
    }
    return true;
}



/**
 * @brief        App task to handle events & messages
 * @param[in]    p_param    Parameters sending to the task
 * @return       void
 */
static void sensor_mode_task_entry(void *p_param)
{
    T_IO_MSG io_msg;
    os_alloc_secure_ctx(1024);

    os_msg_queue_create(&sensor_mode_queue_handle, "sensor mode", 20, sizeof(T_IO_MSG));


    while (true)
    {
        if (os_msg_recv(sensor_mode_queue_handle, &io_msg, 0xFFFFFFFF) == true)
        {


        }
    }
}

void sensor_mode_task_init(void)
{
    void *app_task_handle;   //!< APP Task handle
    os_task_create(&app_task_handle, "bt_task", sensor_mode_task_entry, NULL, 1024, 2);
}

/** @} */ /* End of group PERIPH_APP_TASK */


