
#include "rtthread.h"
#define DBG_TAG     "VXX_DEMO"
#define DBG_LVL     DBG_LOG
#include <rtdbg.h>
#include "rtl876x.h"
#include "vcHr02Hci.h"

// mdl file
static const uint16_t vxx_sample[] =
{
#include "PPG_HR.txt"
};


/* G-Sensor Data */
int16_t xData[40] = {0};
int16_t yData[40] = {0};
int16_t zData[40] = {0};

/* The algorithm of hert rate data struct */
AlgoInputData_t algoInputData;
AlgoOutputData_t algoOutputData;
const uint8_t vcHr02SampleRate = 25;


void vxx_demo(void)
{
    rt_trustzone_enter(1);

    Algo_Init();

    for (uint32_t i = 0; i < sizeof(vxx_sample) / sizeof(uint16_t); i++)
    {
        algoInputData.ppgSample = vxx_sample[i];
        algoInputData.axes.x = 0;//The direction vertical with ARM.
        algoInputData.axes.y = 0;//The direction parallel with ARM.
        algoInputData.axes.z = 0;//The direction upside.
        Algo_Input(&algoInputData, 1000 / vcHr02SampleRate, VCWORK_MODE_HRWORK, 0, 0);

        if ((i % 25) == 0)
        {
            Algo_Output(&algoOutputData);
            rt_kprintf("algoOutputData.hrData = %d \n", algoOutputData.hrData);
        }
    }

    rt_trustzone_exit();
}

MSH_CMD_EXPORT(vxx_demo, vxx demo test);
