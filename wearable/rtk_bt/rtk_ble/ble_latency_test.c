/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file      app_task.c
   * @brief     Routines to create App task and handle events & messages
   * @author    jane
   * @date      2017-06-02
   * @version   v1.0
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
   **************************************************************************************
  */

/*============================================================================*
 *                              Header Files
 *============================================================================*/

#include <ble_task.h>
#include <ble_cb.h>
#include <simple_ble_service.h>
#include <bas.h>
#include "board.h"
#include "trace.h"
#if (SUPPORT_NORMAL_OTA == 1)
#include "ota_service.h"
#endif
#if SUPPORT_SILENT_OTA
#include "dfu_service.h"
#endif
#include "wristband_private_service.h"
#include "rtthread.h"
#include "stdlib.h"
#include "gap_conn_le.h"

static uint32_t cnt = 0;
static uint32_t latency_cnt = 0;

static void task_entry(void *p_param)
{
    uint16_t desired_interval_min;
    uint16_t desired_interval_max;
    uint16_t desired_latency;
    uint16_t desired_superv_tout;
    while (1)
    {
        cnt++;
        if (cnt % 2 == 0)
        {
            latency_cnt++;
            if (latency_cnt == 20)
            {
                latency_cnt = 1;
            }
            desired_interval_min = 0x18;
            desired_interval_max = 0x24;
            desired_latency = latency_cnt;
            desired_superv_tout = 5000;
        }
        else
        {
            desired_interval_min = 0x18;
            desired_interval_max = 0x24;
            desired_latency = 0;
            desired_superv_tout = 5000;
        }
        rt_kprintf("send connection parameter update request, desired_latency = %d! \n", desired_latency);
        le_update_conn_param(0, desired_interval_min, desired_interval_max,
                             desired_latency,
                             desired_superv_tout / 10, desired_interval_min * 2 - 2, desired_interval_max * 2 - 2);
        rt_thread_mdelay(10000);

    }
}


void latency_test(void)
{
    rt_thread_startup(rt_thread_create("throughput_task", task_entry, NULL, 1024,
                                       25, 2));
}

MSH_CMD_EXPORT(latency_test, test test test);


static void set_interval(uint8_t argc, char **argv)
{
    uint16_t desired_interval_min = atoi(argv[1]);;
    uint16_t desired_interval_max = atoi(argv[1]);;
    uint16_t desired_latency = 0;
    uint16_t desired_superv_tout = 5000;
    le_update_conn_param(0, desired_interval_min, desired_interval_max,
                         desired_latency,
                         desired_superv_tout / 10, desired_interval_min * 2 - 2, desired_interval_max * 2 - 2);
}
MSH_CMD_EXPORT(set_interval, test test test);


/** @} */ /* End of group PERIPH_APP_TASK */


