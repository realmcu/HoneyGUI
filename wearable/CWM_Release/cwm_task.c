/*
 * Copyright (c) 2015-2020, RealTek Wristband Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author        Notes
 * 2020-11-18     howie  first version
 */
#include "rtthread.h"
#include "string.h"
#include "CWM_CustomDataStruct.h"
#include "CWM_EventStructure.h"
#include "cwm_lib.h"
#include "OsApi.h"


static rt_thread_t  cwm_thread = RT_NULL;
static rt_ubase_t start_tick = 0;
static rt_ubase_t end_tick = 0;


uint64_t CWM_OS_GetTimeNs(void)
{
    return 1000 * 1000 * rt_tick_get();
}

int CWM_OS_dbgOutput(const char *format)
{
    rt_kprintf(format);
    return 0;
}
void *CWM_OS_malloc(int size)
{
    rt_kprintf("CWM_OS_malloc \n");
    uint8_t *p_n = NULL;
    if (size == 0)
    {
        size = 1;
    }

    p_n = rt_malloc(size);

    return (void *)p_n;
}

void CWM_OS_free(void *ptr)
{
    rt_kprintf("CWM_OS_free \n");

    if (ptr == NULL)
    {
        return;
    }

    rt_free(ptr);
}
static uint32_t listen_cnt = 0;
void CWM_AP_SensorListen(pSensorEVT_t sensorEVT)
{
    listen_cnt++;
    //rt_kprintf("SensorListen %d\n", listen_cnt);
}

float hwAcc[25][3] =
{
    {0.0, 1.0, 9.8},
    {0.4, 0.0, 9.8},
    {0.0, 2.0, 8.8},
    {0.0, 0.0, 9.8},
    {1.0, 0.0, 7.8},
    {0.0, 1.0, 9.8},
    {0.4, 0.0, 9.8},
    {0.0, 2.0, 8.8},
    {0.0, 0.0, 9.8},
    {1.0, 0.0, 7.8},
    {0.0, 2.2, 9.8},
    {0.0, 0.0, 9.8},
    {0.0, 0.3, 9.3},
    {0.5, 0.0, 9.8},
    {0.0, 0.0, 9.3},
    {0.3, 0.0, 9.8},
    {0.0, 0.0, 9.5},
    {0.0, 3.0, 9.8},
    {0.1, 0.0, 9.8},
    {0.2, 0.0, 7.8},
    {0.0, 0.0, 9.8},
    {0.1, 0.0, 7.8},
    {0.0, 1.0, 9.8},
    {0.2, 0.0, 9.6},
    {0.0, 3.0, 9.8}
};

static void cwm_task_entry(void *parameter)
{

    OsAPI device_func =
    {
        .malloc       = CWM_OS_malloc,
        .free         = CWM_OS_free,
        .GetTimeNs    = CWM_OS_GetTimeNs,
        .dbgOutput    = CWM_OS_dbgOutput,
    };


    SettingControl_t scl;
    memset(&scl, 0, sizeof(scl));
    scl.iData[0] = 1;
    CWM_SettingControl(SCL_GET_LIB_INFO, &scl);
    rt_kprintf("version:%d.%d.%d.%d product:%d\n", scl.iData[1], scl.iData[2], scl.iData[3],
               scl.iData[4], scl.iData[5]);

    CWM_LibPreInit(&device_func);
    rt_kprintf("CWM_LibPreInit\r\n");

    memset(&scl, 0, sizeof(scl));
    scl.iData[0] = 1;
    scl.iData[1] = 2; //skip auto check
    scl.iData[2] = 0;
    scl.iData[3] = 0;
    CWM_SettingControl(SCL_CHIP_VENDOR_CONFIG, &scl);
    rt_kprintf("SCL_CHIP_VENDOR_CONFIG\r\n");


//    memset(&scl, 0, sizeof(scl));
//    scl.iData[0] = 1;
//    scl.iData[1] = 1;
//    CWM_SettingControl(SCL_LIB_DEBUG, &scl);


    CWM_LibPostInit(CWM_AP_SensorListen);
    rt_kprintf("CWM_LibPostInit\r\n");

    CWM_CustomSensorInput_Fifo_Init(25 * 2);
    //get chip information
    char chipInfo[64];
    uintptr_t buf_addr = 0;


    buf_addr = (uintptr_t)chipInfo;
    memset(&scl, 0, sizeof(scl));
    scl.iData[0] = 1;
    scl.iData[1] = 1;
    scl.iData[2] = (int)buf_addr;
    scl.iData[3] = sizeof(chipInfo);
    scl.iData[4] = 0;
    scl.iData[5] = 0;
    scl.iData[6] = 0;
    CWM_SettingControl(SCL_GET_CHIP_INFO, &scl);
    rt_kprintf("have_security = %d.%d ret_buff_size = %d  chipInfo = %s\r\n", scl.iData[5],
               scl.iData[6], scl.iData[4], (uint8_t *)(&chipInfo));
    rt_kprintf("chip_settings = %d, %d, %d\r\n", scl.iData[9], scl.iData[10], scl.iData[11]);

//    memset(&scl, 0, sizeof(scl));
//    scl.iData[0] = 1;
//    scl.iData[3] = 9;
//    scl.iData[4] = 5;
//    scl.iData[7] = -1;
//    CWM_SettingControl(SCL_LOG, &scl);
    CWM_Sensor_Enable(IDX_REQUEST_SENSOR);

    memset(&scl, 0, sizeof(scl));
    scl.iData[0] = 1;
    scl.iData[1] = ACTIVITY_MODE_NORMAL;
    CWM_SettingControl(SCL_SET_ACTIVITY_MODE, &scl);

    CWM_Sensor_Enable(IDX_ACCEL);
    CWM_Sensor_Enable(IDX_ALGO_ACTIVITY_OUTPUT); //step alg enable
    CWM_Sensor_Enable(IDX_ALGO_WATCH_HANDUP);    //hand up alg enable

    CustomSensorData SensorDataInput;

    while (1)
    {
        listen_cnt = 0;
        start_tick = rt_tick_get();
        CWM_CustomSensorInput_Fifo_Start(0, 1000 * 1000 * rt_tick_get(), 20 * 1e6);
        for (uint8_t i = 0; i < 25; i++)
        {
            SensorDataInput.sensorType = 0;
            SensorDataInput.fData[0] = hwAcc[i][0];
            SensorDataInput.fData[1] = hwAcc[i][1];
            SensorDataInput.fData[2] = hwAcc[i][2];
            SensorDataInput.fData[3] = 1;
            CWM_CustomSensorInput(&SensorDataInput);

        }
        CWM_CustomSensorInput_Fifo_End(0);

        CWM_process();
        end_tick = rt_tick_get();
        rt_kprintf("cwm task dec tick = %d\r\n", end_tick - start_tick);
        rt_thread_mdelay(500);
        extern void cache_staticstic_dump(void);
        cache_staticstic_dump();
    }
}





static int cwm_task_init(void)
{
    cwm_thread = rt_thread_create("cwm_task",
                                  cwm_task_entry,
                                  RT_NULL,
                                  512 * 10,
                                  8,
                                  2);
    if (cwm_thread != RT_NULL)
    {
        rt_thread_startup(cwm_thread);
    }

    return 0;
}
INIT_APP_EXPORT(cwm_task_init);




