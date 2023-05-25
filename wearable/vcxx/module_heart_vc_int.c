
/*********************************************************************************************************************
*               Copyright(c) 2020, Vcare Corporation. All rights reserved.
**********************************************************************************************************************
* @file     module_heart_vc.c
* @brief    Application example.
* @details
              The resources needed to use this chip include an RTC or a general timer, an external interrupt line,
            and an I2C interface.
              First of all, confirm the count frequency of the RTC or the general timer, assign it to 'mcuOscData',
            if the count frequency is 30kHz, mcuOscData = 30000; then confirm the frequency of the I2C clock, assign
            it to 'mcuI2cClock', if the frequency of the I2C clock is 400kHz, mcuI2cClock = 400.
              Add the code to get the RTC or the general timer count value in function 'vcHr02GetRtcCountFromMCU()',
            add I2C read and write function code in 'vcHr02ReadRegisters(uint8_t startAddress, uint8_t *pRegisters,
            uint8_t len)' and 'vcHr02ReadRegisters(uint8_t startAddress, uint8_t *pRegisters, uint8_t len)', but do
            not change the name of the function.
               How To Use The Chip: Call the program to initialize the peripherals of the main control chip first,
            these peripherals include I2C, an external interrupt, RTC or general timer. Then the chip ID can be read
            to verify whether the chip is working properly or whether I2C communication is normal, the register add-
            ress is 0x00, if your chip is VC31B/VC32S/VC51B/VC52S, the chip ID is 0x21, but also need to verify whe-
            ther your I2C supports continuous multi byte read and write, because the driver code involves continuous
            multi byte read and write. After initializing the peripherals, call the chip initialization function
            'vcHr02Init(&vcHr02,vcMode)', the parameter 'vcHr02' is a defined heart rate data structure, 'vcMode'
            is also defined variable which can be used to switch the chip working mode. After the initialization of
            the chip is completed, the peripheral lights will be on, and the lights will be dimmed when the fingers
            are close to the chip from far to near, the interrupt pin of the chip will have interrupt level.
              About the function 'vcHr02_process(AlgoSportMode_t vcSportMode)': It can be used as an interrupt serv-
            ice function or as an independent task, which is added to the task queue by the interrupt trigger, but
            we strongly recommend the second one. The parameter 'vcSportMode' is a defined variable which can be us-
            ed to switch the sport mode in heart rate mode.
*---------------------------------------------------------------------------------------------------------------------
*---------------------------------------------------------------------------------------------------------------------
           使用芯片需要的外设资源包括一个实时时钟源或者一个通用定时器(计数频率不得低于10K，最佳是在30K左右)，一个外部
        中断，一路I2C。
           首先，确认实时时钟源或者通用定时器的计数频率，并将这个频率赋值给变量‘mcuOscData’，如果计数频率是30KHz，则
        mcuOscData = 30000；然后确认I2C的时钟频率，并将这个频率赋值给变量‘mcuI2cClock’，如果I2C的时钟频率是400KHz，则
        mcuI2cClock = 400。
           在函数‘vcHr02GetRtcCountFromMCU()'’里添加获取实时时钟源或通用定时器的计数值的代码，注意不是返回差值（与VC31
        和VC51不同），在函数‘vcHr02ReadRegisters(uint8_t startAddress, uint8_t *pRegisters, uint8_t len)’添加I2C的读函
        数的代码，在函数‘vcHr02ReadRegisters(uint8_t startAddress, uint8_t *pRegisters, uint8_t len)’添加I2C的写函数的
        代码，但不要改变函数名称。读函数中三个变量的意义分别是：寄存器地址、指向寄存器内容存储位置的指针（存寄存器里读
        回来的数据的变量地址）、读的长度，写函数中三个变量的意义分别是：寄存器地址、指向要写入寄存器的内容的指针、写的
        长度。
           芯片使用：先调用初始化主控芯片外设的代码，初始化实时时钟源或通用定时器，I2C，外部中断，然后可以通过读芯片的
        ID来判断芯片是否正常工作或者I2C通讯是否正常，寄存器地址是0x00，如果使用的芯片是VC31B/VC32S/VC51B/VC52S，芯片的
        ID是0x21，但要确认I2C是否支持连续读写多个字节，因为驱动里涉及连续读写多个字节的操作。外设初始化完成之后，调用
        ‘vcHr02Init(&vcHr02,vcMode)’来初始化芯片，变量‘vcHr02’是已经定义的心率数据结构体，变量‘vcMode’也是已经定义的，
        可以通过改变这个变量的值，切换芯片的工作模式，初始化芯片之后，外围的等会亮，并且手指由远到近靠近时，灯会由亮到暗
        调整，芯片的中断引脚也会有中断电平。
           关于‘vcHr02_process(AlgoSportMode_t vcSportMode)’函数：可以作为中断服务函数，也可以作为独立任务，通过中断触发
        将它添加到任务队列，但我们强烈推荐第二种方式。‘vcSportMode’是一个已经定义的在心率模式下切换运动模式的变量。

       *若用户是用定时读取方式读取Gsensor数据，用于计步等功能，为了更好的动态心率性能，我们推荐按照我们的中断的时间间隔，
       在‘vcHr02_process(AlgoSportMode_t vcSportMode)’函数中对应位置，读取Gsensor数据，中断的时间间隔（佩戴状态下）可以
       根据FIFO的长度可调（40ms整数倍）。如果用户心率非24小时连续测量方式，即进入心率测量功能界面测量心率，退出界面停止
       心率测量或定时唤醒心率芯片测量心率方式，则调用‘vcHr02StopSample(vcHr02_t *pvcHr02)’函数让芯片进入休眠时，可以重新
       使能定时器，定时读取Gsensor数据，因为调用上述函数使心率芯片休眠时，中断脚将不会有中断电平，‘vcHr02_process(AlgoS-
       portMode_t vcSportMode)’函数将不会执行，如果用户是24小时连续心率测量，即不会休眠，可以忽略。若用户是按照Gsensor中
       断触发读取Gsensor数据的方式，请将Gsensor读取的数据保存到buffer中，Gsensor每读一次数据就将buffer中的上一次数据用这
       一次的数据覆盖，在‘vcHr02_process(AlgoSportMode_t vcSportMode)’函数中传入算法函数中。

* @author
* @date
* @version  V1.9.8
**********************************************************************************************************************
*/
#include "vcHr02Hci.h"

/* Include your INT,I2C,Timer header file */
//#include "INT.h"
//#include "I2C.h"
//#include "Timer.h"

/* 如果Gsensor数据读取代码已经添加，请将这个宏置1 */
#define GsensorEn 0

/* Heart rate value */
int HeartRateValue = 0;

/* Timer clock frequency
   Be used to adjust INT frequency */
const uint32_t mcuOscData = 30000;

/* MCU I2C clock frequency */
const uint16_t mcuI2cClock = 400;

/* Heart rate sample frequency */
const uint8_t vcHr02SampleRate = 25;

/* INT Flag */
bool vcHr02IRQFlag = 0;

/* Spo2 value */
float vcSpo2Value = 0.0;
int real_spo = 0;

/* Spo2 data */
float vcRedPPG = 0.0;
float vcIrPPG = 0.0;

uint16_t GsensorLength = 0;

/* G-Sensor Data */
int16_t xData[40] = {0};
int16_t yData[40] = {0};
int16_t zData[40] = {0};

/* The algorithm of hert rate data struct */
AlgoInputData_t algoInputData;
AlgoOutputData_t algoOutputData;

/* Heart rate data struct */
vcHr02_t vcHr02;

/* Heart rate mode */
vcHr02Mode_t vcMode = VCWORK_MODE_HRWORK;

/* Sport Mode In Heart Rate Mode */
AlgoSportMode_t vcSportMode = SPORT_TYPE_NORMAL;

/*
 * @brief  Initialize the chip working mode and set some parameters.
 *
 *
 **/
void vcHr02Init(vcHr02_t *pVcHr02, vcHr02Mode_t vcHr02WorkMode)
{
    vcHr02StopSample(pVcHr02);
    vcHr02SoftReset(pVcHr02);
    vcSportMotionAlgoTerminate();
    pVcHr02->vcSampleRate = vcHr02SampleRate;
    pVcHr02->mcuOscValue = mcuOscData;
    pVcHr02->mcuSclRate = mcuI2cClock;
    pVcHr02->workMode = vcHr02WorkMode;
    /* HeartRate Mode */
    if (pVcHr02->workMode == VCWORK_MODE_HRWORK)
    {
        vcHr02StartSample(pVcHr02);
        Algo_Init();
    }
    /* Spo2 Mode */
    else if (pVcHr02->workMode == VCWORK_MODE_SPO2WORK)
    {
        vcHr02StartSample(pVcHr02);
        vcSportMotionAlgoInit();
        vcSpo2AlgoInit();
    }
    /* Light Crosstalk Test Mode */
    else if (pVcHr02->workMode == VCWORK_MODE_LPDETECTION)
    {
        vcHr02StartSample(pVcHr02);
        Algo_Init();
    }
    /* Light Crosstalk Test Mode */
    else if (pVcHr02->workMode == VCWORK_MODE_CROSSTALKTEST)
    {
        vcHr02StartSample(pVcHr02);
    }
}


/*
 * @brief  The processing flow under different working modes can be used as
 *         interrupt service function or as an independent task, but it is
 *         recommended to be treated as an independent task.
 *
 **/
void vcHr02_process(AlgoSportMode_t sportMode)
{
    uint8_t algoCallNum = 0;
    uint8_t ppgLength = 0;
    uint8_t vcSportFlag = 0;

    if (vcHr02IRQFlag)
    {
        vcHr02IRQFlag = false;

        if (vcHr02.workMode == VCWORK_MODE_HRWORK)
        {
            if (VCHR02RET_UNWEARTOISWEAR == vcHr02GetSampleValues(&vcHr02, &ppgLength))
            {
                Algo_Init();
            }

            if (vcHr02.vcFifoReadFlag || vcHr02.vcPsFlag)
            {
#if GsensorEn

                /*
                Gsensor数据处理示例:
                1.此处Gsensor采样率为36Hz，定时读取方式；
                2.800mS（心率芯片FIFO长度为20,20×40ms）中断间隔时，定时方式读取Gsensor数据
                  长度众数（800ms读一次Gsensor数据，一段时间内读到29个数据的次数最多）是29；
                3.心率芯片的FIFO长度是20，所以也只要20组Gsensor数据，心率芯片FIFO长度为多少
                  就需要多少组Gsensor数据；

                */
                /* 在下一行添加读取Gsensor数据的代码 */
                /* ReadGsensorFIFO(+-4G11bit or +-8G12bit 256/g) */

                if (GsensorLength >= 29)
                {
                    GsensorLength = 29;
                }
                else if (GsensorLength >= 1)
                {
                    //小于29个数据补足29个；
                    for (uint8_t i = GsensorLength; i < 29; i++)
                    {
                        xData[i] = xData[GsensorLength - 1];
                        yData[i] = yData[GsensorLength - 1];
                        zData[i] = zData[GsensorLength - 1];
                    }
                }

                for (uint8_t i = 0; i < 20; i++)
                {
                    //cash_num[20]数组为抽样数组；此处将29个数据抽成20个，匹配800mS中断的20个PPG
                    xData[i] = yData[cash_num[i]] >> 5;
                    yData[i] = xData[cash_num[i]] >> 5;
                    zData[i] = zData[cash_num[i]] >> 5;
                }
#endif
                if (vcHr02.vcFifoReadFlag)
                {
                    vcHr02.vcFifoReadFlag = 0;
                    if (vcHr02.wearStatus == VCHR02WEARST_ISWEAR)
                    {
#if GsensorEn
                        if (GsensorLength <
                            ppgLength) //此处为了防止丢中断，保证每个PPG数据参与算法运算，丢中断的情况下，Gsensor数据个数补足到ppgLength；
                        {
                            for (uint8_t i = GsensorLength; i < ppgLength; i++)
                            {
                                xData[i] = xData[GsensorLength - 1];
                                yData[i] = yData[GsensorLength - 1];
                                zData[i] = zData[GsensorLength - 1];
                            }
                        }
#endif
                        algoInputData.envSample = vcHr02.sampleData.envValue[0];
                        for (algoCallNum = 0; algoCallNum < ppgLength; algoCallNum++)
                        {
                            algoInputData.ppgSample = vcHr02.sampleData.ppgValue[algoCallNum];
                            algoInputData.axes.x = xData[algoCallNum];//The direction vertical with ARM.
                            algoInputData.axes.y = yData[algoCallNum];//The direction parallel with ARM.
                            algoInputData.axes.z = zData[algoCallNum];//The direction upside.
                            Algo_Input(&algoInputData, 1000 / vcHr02SampleRate, sportMode, 0, 0);
                        }

                        Algo_Output(&algoOutputData);
                        HeartRateValue = algoOutputData.hrData;

                        if (HeartRateValue == -1)
                        {
                            Algo_Init();
                        }
                    }
                }
                else
                {
#if SportMotionEn
                    green_led_off_state_gsensor_abs_sum_diff_func(xData[0], yData[0], zData[0]);
#endif
                    HeartRateValue = -2;
                }
            }
        }
        else if (vcHr02.workMode == VCWORK_MODE_SPO2WORK)
        {
            if (VCHR02RET_UNWEARTOISWEAR == vcHr02GetSampleValues(&vcHr02, &ppgLength))
            {
                vcSpo2AlgoInit();
                vcSportMotionAlgoInit();
            }
            if (vcHr02.vcFifoReadFlag || vcHr02.vcPsFlag)
            {
#if GsensorEn
                /* ReadGsensorFIFO(+-4G11bit or +-8G12bit 256/g) */
                if (GsensorLength >= 29)
                {
                    GsensorLength = 29;
                }
                else if (GsensorLength >= 1)
                {
                    for (uint8_t i = GsensorLength; i < 29; i++)
                    {
                        xData[i] = xData[GsensorLength - 1];
                        yData[i] = yData[GsensorLength - 1];
                        zData[i] = zData[GsensorLength - 1];
                    }
                }

                for (uint8_t i = 0; i < 20; i++)
                {
                    xData[i] = yData[cash_num[i]] >> 5;
                    yData[i] = xData[cash_num[i]] >> 5;
                    zData[i] = zData[cash_num[i]] >> 5;
                }
#endif
                if (vcHr02.vcFifoReadFlag)
                {
                    vcHr02.vcFifoReadFlag = 0;
                    if (ppgLength <= 50)
                    {
                        for (algoCallNum = 0; algoCallNum < ppgLength / 2; algoCallNum++)
                        {
                            vcIrPPG = vcHr02.sampleData.ppgValue[algoCallNum * 2];
                            vcRedPPG = vcHr02.sampleData.ppgValue[algoCallNum * 2 + 1];
                            vcSpo2Value = vcSpo2Calculate(vcRedPPG, vcIrPPG);
#if GsensorEn
                            vcSportFlag = vcSportMotionCalculate(xData[algoCallNum], yData[algoCallNum], zData[algoCallNum]);
#endif
                            if ((!vcSportFlag) && (vcSpo2Value > 0))
                            {
                                real_spo = vcSpo2Value;
                            }
                        }
                    }
                }
                else
                {
#if SportMotionEn
                    green_led_off_state_gsensor_abs_sum_diff_func(xData[0], yData[0], zData[0]);
#endif
                }
            }
        }
        else if (vcHr02.workMode == VCWORK_MODE_LPDETECTION)
        {
            vcHr02GetSampleValues(&vcHr02, &ppgLength);
        }
        else if (vcHr02.workMode == VCWORK_MODE_CROSSTALKTEST)
        {
            vcHr02GetSampleValues(&vcHr02, &ppgLength);
            /* If Pass: */
            if ((vcHr02.sampleData.maxLedCur >= 100) && (vcHr02.sampleData.preValue[0] <= 2))
            {
                //PASS：
                //Display the value of vcHr02.sampleData.maxLedCur and vcHr02.sampleData.preValue[0]
            }
        }
    }
}


/*
 * @brief  main.
 *
 *
 **/
void main()
{
    /*-----------INSERT BOARD HARDWARE AND SOFTWARE INITIAL FUNCTION------------*/
    accInit();              //Accelerometer initial +-8G 12bit
    vcHr02PhyInit();        //vcHr02 board initial
    vcHr02Init(&vcHr02, vcMode);
    while (1)
    {
        vcHr02_process(vcSportMode);
    }
}


/*
 * @brief  MCU peripherals initialization.
 *
 *
 **/
void vcHr02PhyInit(void)
{
    /*****************************************************************************
    * 1.If Reset Pin of the chip is connected to MCU, initialize the corresponding
    *   pin on the MCU.
    *
    * 2.Initialize the I2C of MCU.
    *
    * 3.Initialize the external interrupt of MCU.
    *
    *
    ******************************************************************************/

    /*------------------INSERT YOUR CODE HERE-----------------*/

}


/*
 * @brief  Interrupt service function.
 *
 *
 **/
void vcHr02IRQHandler(void)
{
    vcHr02IRQFlag = true;
}


/*
 * @brief  Register writing.
 *
 *
 **/
vcHr02Ret_t vcHr02WriteRegisters(uint8_t startAddress, uint8_t *pRegisters, uint8_t len)
{
    /**********************************************************************************
    * @param
    *       startAddress:Register address.
    *       pRegisters  :Point to the data to write.
    *       len         :Byte to be transmitted.
    *
    *
    *       7bit I2C own address:0x33
    *       8bit I2C own address:0x66(the least significant bit is the W/R control bit)
    ************************************************************************************/


    /*------------------INSERT YOUR CODE HERE-----------------*/

    return VCHR02RET_ISOK;
}


/*
 * @brief  Register reading.
 *
 *
 **/
vcHr02Ret_t vcHr02ReadRegisters(uint8_t startAddress, uint8_t *pRegisters, uint8_t len)
{
    /***********************************************************************************
    * @param
    *       startAddress:Register address.
    *       pRegisters  :Point to the location to save.
    *       len         :Byte to be transmitted.
    *
    *
    *       7bit I2C own address:0x33
    *       8bit I2C own address:0x67(the least significant bit is the W/R control bit)
    ************************************************************************************/


    /*------------------INSERT YOUR CODE HERE-----------------*/

    return VCHR02RET_ISOK;
}


/*
 * @brief  Get the count value of RTC or general timer of MCU.
 *
 *
 **/
uint32_t vcHr02GetRtcCountFromMCU(void)
{
    /*------------------INSERT YOUR CODE HERE-----------------*/

    /*
        return RTC_GetCounter();
    */
}

