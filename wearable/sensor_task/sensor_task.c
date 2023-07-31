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
#include "sensor_msg.h"
#ifdef SENSOR_MODE
#include "os_ext_api.h"
#include "pm.h"
#endif

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

    sensor_mode_uart_init();
    sensor_mode_hw_tim_init();

    while (true)
    {
        T_IO_MSG io_msg;
        if (os_msg_recv(sensor_queue_handle, &io_msg, 0xFFFFFFFF) == true)
        {
            switch (io_msg.subtype)
            {
            case SENSOR_MSG_UART_TEST:
                {
                    uint32_t start_time = sensor_mode_hw_tim_read();
                    sensor_mode_uart_write();
                    uint32_t stop_time = sensor_mode_hw_tim_read();
                    DBG_DIRECT("SENSOR_MSG_UART_TEST time %d us", stop_time - start_time);
                }
                break;

            default:
                break;
            }
        }
    }
}

#ifdef SENSOR_MODE
void scheduler_hook(void *from, void *to)
{
    POWERScenarioMode mode = power_scenario_mode_get(POWER_SCENARIO_OPERATION_MODE);
    DBG_DIRECT("scheduler_hook mode = %d line %d", mode, __LINE__);
    char *from_task_name;
    char *to_task_name;
    os_task_name_get(from, &from_task_name);
    os_task_name_get(to, &to_task_name);
    uint32_t actual_mhz = 0;
    int32_t ret = 0;

    DBG_DIRECT("scheduler_hook from_task_name %s to_task_name %s", from_task_name, to_task_name);

    extern bool dlps_flag;
    if (dlps_flag)
    {
        return;
    }

    if (strcmp(to_task_name, "sensor") != 0 && strcmp(to_task_name, "IDLE") != 0)
    {
        //need set to HP mode
        //need set clock to 200M
        mode = power_scenario_mode_get(POWER_SCENARIO_OPERATION_MODE);
        if (mode != SCENARIO_OPERATION_HP_MODE)
        {
            ret = power_scenario_mode_set(POWER_SCENARIO_OPERATION_MODE, SCENARIO_OPERATION_HP_MODE);
            DBG_DIRECT("from xxx to not sensor neither IDLE scenario_ret %d", ret);
            disable_xip_access(false);
            ret = pm_cpu_freq_set(200, &actual_mhz);
            DBG_DIRECT("pm_cpu_freq_set ret %d actual_mhz %d line %d", ret, actual_mhz, __LINE__);
            ret = pm_dsp1_freq_set(320, &actual_mhz);
            DBG_DIRECT("dsp: ret %d, actual_mhz %d", ret, actual_mhz);
        }
    }

    if ((strcmp(to_task_name, "sensor") == 0) && ((strcmp(from_task_name, "IDLE") == 0) ||
                                                  (strcmp(from_task_name, "Tmr Svc") == 0)))
    {
        //need set to sensor mode
        //need set clock to 40
        mode = power_scenario_mode_get(POWER_SCENARIO_OPERATION_MODE);
        if (mode != SCENARIO_OPERATION_SENSOR_MODE)
        {
            ret = pm_cpu_freq_set(40, &actual_mhz);
            DBG_DIRECT("pm_cpu_freq_set ret %d actual_mhz %d", ret, actual_mhz);
            ret = pm_dsp1_freq_set(160, &actual_mhz);
            DBG_DIRECT("dsp: ret %d, actual_mhz %d", ret, actual_mhz);
            uint32_t scenario_ret = power_scenario_mode_set(POWER_SCENARIO_OPERATION_MODE,
                                                            SCENARIO_OPERATION_SENSOR_MODE);
            disable_xip_access(true);
            DBG_DIRECT("from IDLE to sensor scenario_ret %d", ret);
        }
    }
    mode = power_scenario_mode_get(POWER_SCENARIO_OPERATION_MODE);
    DBG_DIRECT("scheduler_hook mode = %d line %d\n", mode, __LINE__);
}
#endif

void sensor_task_init(void)
{
#ifdef SENSOR_MODE
    os_ext_func_init();
    os_sched_set_hook(scheduler_hook);
#endif
    os_task_create(&sensor_task_handle, "sensor", sensor_task_entry, NULL, 1024, 1);
}

/** @} */ /* End of group PERIPH_APP_TASK */


