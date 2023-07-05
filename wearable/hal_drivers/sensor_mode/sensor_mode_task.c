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
#include <watch_msg.h>
#include "os_ext_api.h"



/*============================================================================*
 *                              Variables
 *============================================================================*/
static void *sensor_task_handle;    //!< Sensor Task handle
static void *sensor_queue_handle;   //!< Sensor queue handle


bool app_send_msg_to_sensor_task(T_IO_MSG *p_msg)
{
    if (os_msg_send(sensor_queue_handle, p_msg, 0) == false)
    {
        APP_PRINT_ERROR0("send_io_msg_to_app fail sensor queue");
        return false;
    }
    return true;
}



/**
 * @brief        Sensor task to handle events & messages
 * @param[in]    p_param    Parameters sending to the task
 * @return       void
 */
static void sensor_task_entry(void *p_param)
{
    os_alloc_secure_ctx(1024);

    os_msg_queue_create(&sensor_queue_handle, "sensorQ", 20, sizeof(T_IO_MSG));

    while (true)
    {
        T_IO_MSG io_msg;
        if (os_msg_recv(sensor_queue_handle, &io_msg, 0xFFFFFFFF) == true)
        {
            DBG_DIRECT("=====%s %d", __FUNCTION__, __LINE__);

        }
    }
}


#include "platform_scenario_draft.h"

void scheduler_hook(void *from, void *to)
{
    //rt_kprintf("from task [%s] to task [%s] \n", from->name, to->name);

    //OperationModeType mode = platform_scenario_get_mode(PLATFORM_SCENARIO_OPERATION_MODE);
    //rt_kprintf("PLATFORM_SCENARIO_OPERATION_MODE = %d \n", mode);
    char *from_task_name;
    char *to_task_name;
    os_task_name_get(from, &from_task_name);
    os_task_name_get(to, &to_task_name);

    if ((strcmp(from_task_name, "sensor") == 0) && (strcmp(to_task_name, "IDLE") != 0))
    {
        //need set to HP mode
        //need set clock to 200M
        uint32_t ret = platform_scenario_set_mode(PLATFORM_SCENARIO_OPERATION_MODE, OPERATION_HP_MODE);
        DBG_DIRECT("from sensor to IDLE ret %d", ret);
    }

    if ((strcmp(to_task_name, "sensor") == 0) && (strcmp(from_task_name, "IDLE") == 0))
    {
        //need set to sensor mode
        //need set clock to 40M
        //uint32_t ret = platform_scenario_set_mode(PLATFORM_SCENARIO_OPERATION_MODE, OPERATION_SENSOR_MODE);
        DBG_DIRECT("from IDLE to sensor ret %d");
    }
    OperationModeType mode = platform_scenario_get_mode(PLATFORM_SCENARIO_OPERATION_MODE);
    DBG_DIRECT("PLATFORM_SCENARIO_OPERATION_MODE = %d \n", mode);
}

void sensor_task_init(void)
{
    os_task_create(&sensor_task_handle, "sensor", sensor_task_entry, NULL, 1024, 1);
    os_ext_func_init();
    os_sched_set_hook(scheduler_hook);
}

/** @} */ /* End of group PERIPH_APP_TASK */


