/*********************************************************************************************************
 *               Copyright(c) 2022, vcare Corporation. All rights reserved.
 **********************************************************************************************************
 * @file     drv_heart_vc_common.c
 * @brief
 * @details
 * @author
 * @date
 * @version  v2.02
 *********************************************************************************************************
 */

#include <stdlib.h>
#include "string.h"
#include "module_heart_vc_common.h"
#include "vcSportMotionIntAlgo.h"
#include "Spo2Algo.h"
#include "algo.h"
#include "vcHr02AHci.h"

#include "board.h"
#include "trace.h"
#include "platform_utils.h"

#include "trace.h"
#include <stdlib.h>
#include "rtl_rcc.h"
#include "rtl_pinmux.h"
#include "rtl_nvic.h"
#include "rtl_gpio.h"
#include "trace.h"
#include "rtl_gpio.h"
#include "rtl_rcc.h"
#include "drv_gpio.h"
#include "drv_i2c.h"
#include "drv_touch.h"
#include "touch_zt2717.h"
#include "string.h"
#include "trace.h"
#include "utils.h"

#define GsensorEn 0
#define SportMotionEn 0
static void psp_get_hr(uint16_t *ppg);
static vcComSampleData_t g_sampleData = {0};
// Heart rate mode
vcComMode_t vcMode = VCWORK_MODE_HRWORK;
/* Spo2 value */
float vcSpo2Value = 0.0;
uint16_t bleVcSpo2Value = 0;
/* Heart rate value */
int HeartRateValue = 0;
uint16_t bleheartRate = 0;
/* wear status */
uint8_t wearStatus = 0;
/* temperature */
int16_t cal_temperature;
/* Sport Mode */
AlgoSportMode vcSportMode =
    SPORT_TYPE_NORMAL; // SPORT_TYPE_RUNNING; //SPORT_TYPE_NORMAL;//SPORT_TYPE_RIDE_BIKE;

uint8_t cash_num[] = {0, 2, 3, 5, 6, 8, 9, 11, 12, 14, 15, 17, 18, 20, 21, 22, 24, 26, 27, 28};/* 29->20 */
uint8_t cash_num2[] = {0, 1, 2, 4, 5, 7, 8, 10, 11, 13};/* 14->10 */
int16_t xData[60] = {0};
int16_t yData[60] = {0};
int16_t zData[60] = {0};

extern vcHr02_t vcHr02;

extern uint32_t IIC_Write_Bytes(uint8_t StartAddress, uint8_t *pBuffer, uint8_t len);
extern uint32_t IIC_Read_Bytes(uint8_t StartAddress, uint8_t *pBuffer, uint8_t len);

/********************************************************************/
void vcHr02Init(vcComMode_t vcHr02WorkMode)
{
    //sample rate and mcucounter rate changed in drvVCxxStartSample
    /* HeartRate Mode */
    if (vcHr02WorkMode == VCWORK_MODE_HRWORK)
    {
        drvVCxxStartSample(VCWORK_MODE_HRWORK, 1000);
        Algo_Init();
    }
    /* Spo2 Mode */
    else if (vcHr02WorkMode == VCWORK_MODE_SPO2WORK)
    {
        drvVCxxStartSample(VCWORK_MODE_SPO2WORK, 400);
        //vcSportMotionAlgoTerminate();
        vcSpo2AlgoInit();
    }
    /* Light Crosstalk Test Mode */
    else if (vcHr02WorkMode == VCWORK_MODE_CROSSTALKTEST)
    {
        drvVCxxStartSample(VCWORK_MODE_CROSSTALKTEST, 400);
    }
    /* Low power wear detection mode */
    else if (vcHr02WorkMode == VCWORK_MODE_LPDETECTION)
    {
        drvVCxxStartSample(VCWORK_MODE_LPDETECTION, 400);
    }
}

/********************************************************************

 *******************************************************************/
/* Heart rate data struct */
#define HRS_I2C_BUS 0x33
#define HRS_INT P0_4
#define HRS_I2C_SCL P0_5
#define HRS_I2C_SDA P0_2
#define GPIO_PORT           GPIO_GetPort(HRS_INT)
#define HRS_INT_IRQ GPIOA4_IRQn
#define HRS_INT_HANDLER    GPIOA4_Handler
static uint16_t ppg_data[32];

static uint16_t ppg_data_count;
void vcHr02_process(AlgoSportMode vcSportMode)
{
    uint8_t vcSportFlag = 0;
    uint8_t algoCallNum = 0;
    uint8_t ppgLength = 0;
    uint8_t irqReason = 0;
    AlgoInputData_t algoInputData;
    AlgoOutputData_t algoOutputData;

    vcComMode_t workMode = drvVCxxGetWorkMode();
    if (workMode == VCWORK_MODE_HRWORK)
    {
        if (true == drvVCxxGetSampleValues(&g_sampleData, &ppgLength))
        {
            APP_PRINT_INFO0("unwear -> wear");
            /* unwear -> wear */
            Algo_Init();
            HeartRateValue = 0;
            bleheartRate = 0;
        }//APP_PRINT_INFO1("ppg:%b", TRACE_BINARY(32, g_sampleData.ppgValue));

        uint8_t temp[sizeof(vcComSampleData_t)];
        memcpy(temp, &g_sampleData, sizeof(vcComSampleData_t));
        //for(int i=0;i<sizeof(vcComSampleData_t);i++)
        //DBG_DIRECT("%d", temp[i]);
        irqReason = drvVCxxGetIntFlag();
        if (irqReason & (VCXX_INT_FLAG_FIFO_READ | VCXX_INT_FLAG_PS))
        {
            //========================================
            //       GsensorEn code sample
            //========================================
#if GsensorEn

            stk8321_get_fifo_data(&fifo_len, buff);
            if (fifo_len >= 14)
            {

                fifo_len = 14;
            }
            else if (fifo_len < 14)
            {

                for (uint8_t i = fifo_len; i < 14; i++)
                {
                    xData[i] = xData[fifo_len - 1];
                    yData[i] = yData[fifo_len - 1];
                    zData[i] = zData[fifo_len - 1];
                }
            }
            for (uint8_t i = 0; i < 10; i++)
            {

                xData[i] = xData[cash_num2[i]] >> 5;
                yData[i] = yData[cash_num2[i]] >> 5;
                zData[i] = zData[cash_num2[i]] >> 5;
            }
#endif
            if (irqReason & VCXX_INT_FLAG_FIFO_READ)
            {
                if (vcHr02.wearStatus == VCHR02WEARST_ISWEAR)
                {
#if GsensorEn
                    if (ppgLength > 20)
                    {
                        ppgLength = 20;
                    }
                    if (ppgLength >
                        10)
                    {
                        for (uint8_t i = 10; i < ppgLength; i++)
                        {
                            xData[i] = xData[len - 1];
                            yData[i] = yData[len - 1];
                            zData[i] = zData[len - 1];
                        }
                    }
#endif
                    algoInputData.envSample = vcHr02.sampleData.envValue[0];
                    {
//
                    }
                    if (ppgLength == 16)
                    {
                        for (int i = 0; i < 16; i++)
                        {
                            ppg_data[i * 2] = vcHr02.sampleData.ppgValue[i];
                            ppg_data[i * 2 + 1] = vcHr02.sampleData.ppgValue[i];
                        }
                        //APP_PRINT_INFO1("ppg:%b", TRACE_BINARY(32*2, ppg_data));
                        psp_get_hr(ppg_data);
                    }

                    for (algoCallNum = 0; algoCallNum < ppgLength; algoCallNum++)
                    {
                        //APP_PRINT_INFO1("0x%x,", vcHr02.sampleData.ppgValue[algoCallNum]);
                        algoInputData.ppgSample = vcHr02.sampleData.ppgValue[algoCallNum];
                        algoInputData.axes.x = abs(yData[algoCallNum]); //(&buff[algoCallNum])->AXIS_Y;
                        algoInputData.axes.y = abs(xData[algoCallNum]); //(&buff[algoCallNum])->AXIS_X;
                        algoInputData.axes.z = abs(zData[algoCallNum]); //(&buff[algoCallNum])->AXIS_Z;
                        Algo_Input(&algoInputData, 1000 / 16, vcSportMode, 0, 0);/* 1000/25(25hz) */
                    }
                    Algo_Output(&algoOutputData);
                    HeartRateValue = algoOutputData.hrData;
                    bleheartRate = HeartRateValue;


                    //APP_PRINT_INFO1("[HEART RATE VCARE IS %d]", bleheartRate*16/24+12);
                    if (HeartRateValue == -1)
                    {
                        Algo_Init();
                    }
                }
            }
            else if (irqReason & VCXX_INT_FLAG_PS)
            {
#if SportMotionEn
                green_led_off_state_gsensor_abs_sum_diff_func(xData[0], yData[0], zData[0]);
#endif
                HeartRateValue = -2;
            }
        }
    }
    else if (workMode == VCWORK_MODE_SPO2WORK)
    {
        if (true == drvVCxxGetSampleValues(&g_sampleData, &ppgLength))
        {
            bleVcSpo2Value = 0;
            vcSpo2Value = 0;
            vcSpo2AlgoInit();
            vcSportMotionAlgoInit();
        }
        irqReason = drvVCxxGetIntFlag();
        if (irqReason & (VCXX_INT_FLAG_FIFO_READ | VCXX_INT_FLAG_PS))
        {
            //========================================
            //       GsensorEn code sample
            //========================================
#if GsensorEn

            stk8321_get_fifo_data(&fifo_len, buff);
            if (fifo_len >= 14)
            {

                fifo_len = 14;
            }
            else if (fifo_len < 14)
            {

                for (uint8_t i = fifo_len; i < 14; i++)
                {
                    xData[i] = xData[fifo_len - 1];
                    yData[i] = yData[fifo_len - 1];
                    zData[i] = zData[fifo_len - 1];
                }
            }
            for (uint8_t i = 0; i < 10; i++)
            {

                xData[i] = xData[cash_num2[i]] >> 5;
                yData[i] = yData[cash_num2[i]] >> 5;
                zData[i] = zData[cash_num2[i]] >> 5;
            }
#endif
            if (irqReason & VCXX_INT_FLAG_FIFO_READ)
            {
                for (algoCallNum = 0; algoCallNum < ppgLength / 2; algoCallNum++)
                {
                    float vcIrPPG = 0.0f, vcRedPPG = 0.0f;
                    vcIrPPG = vcHr02.sampleData.ppgValue[algoCallNum * 2];
                    vcRedPPG = vcHr02.sampleData.ppgValue[algoCallNum * 2 + 1];
                    vcSpo2Value = vcSpo2Calculate(vcRedPPG, vcIrPPG, 2.0);
#if GsensorEn
                    vcSportFlag = vcSportMotionCalculate(xData[algoCallNum], yData[algoCallNum], zData[algoCallNum]);
#endif
                    if (((uint8_t)vcSpo2Value > 0) && (!vcSportFlag))
                    {
                        bleVcSpo2Value = (uint8_t)vcSpo2Value;
                    }
                }
            }
            else if (irqReason & VCXX_INT_FLAG_PS)
            {
#if SportMotionEn
                green_led_off_state_gsensor_abs_sum_diff_func(xData[0], yData[0], zData[0]);
#endif
            }
        }
    }
    else if (workMode == VCWORK_MODE_LPDETECTION)
    {
        if (drvVCxxGetSampleValues(&g_sampleData, &ppgLength))
        {
        }
    }
    else if (workMode == VCWORK_MODE_CROSSTALKTEST)
    {
//      static uint16_t bioVal[2] = {0};
        uint16_t preVal = 0, maxLedCur = 0;
        uint8_t psVal = 0;
        drvVCxxGetSampleValues(&g_sampleData, &ppgLength);
        drvVCxxGetTestData(&preVal, &maxLedCur, &psVal);
        uint16_t tmpVal;
        drvVCxxGetBioTestData(&tmpVal);

    }
}

/*
 * @brief  Register writing.
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
    *       7bit I2C own address:0x33
    *       8bit I2C own address:0x66(the least significant bit is the W/R control bit)
    ************************************************************************************/
    /*------------------INSERT YOUR CODE HERE-----------------*/
//  IIC_Write_Bytes(startAddress, pRegisters, len);

    uint8_t *write_buf = NULL;
    if (len > 0)
    {
        write_buf = malloc(len);
    }
    if (write_buf != NULL)
    {
        uint8_t *start_buf = write_buf;
        write_buf[0] = startAddress;
        memcpy(write_buf + 1, pRegisters, len);
        drv_i2c0_write(HRS_I2C_BUS, write_buf, len + 1);

        free(start_buf);
        return VCHR02RET_ISOK;
    }
    return VCHR02RET_ISERR;

}

/*
 * @brief  Register reading.
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
    *       7bit I2C own address:0x33
    *       8bit I2C own address:0x67(the least significant bit is the W/R control bit)
    ************************************************************************************/
    /*------------------INSERT YOUR CODE HERE-----------------*/
//  IIC_Read_Bytes(startAddress, pRegisters, len);
    uint8_t reg = startAddress;
    if (len == 0)
    {
        return VCHR02RET_ISERR;
    }
    if (drv_i2c0_write(HRS_I2C_BUS, &reg, 1) == 0)
    {
        APP_PRINT_INFO1(" Gsensor read fifo fail = %d!", __LINE__);
        return VCHR02RET_ISERR;
    }

    platform_delay_us(1);

    if (drv_i2c0_read(HRS_I2C_BUS, pRegisters, len) == 0)
    {
        APP_PRINT_INFO1(" Gsensor read fifo fail line = %d!", __LINE__);
        return VCHR02RET_ISERR;
    }


    return VCHR02RET_ISOK;
}

/*
 * @brief  Get the count value of RTC or general timer of MCU.
 *
 **/
uint32_t vcHr02GetRtcCountFromMCU(void)
{
    /*------------------INSERT YOUR CODE HERE-----------------*/
    extern uint32_t drv_rtc_count(void);
    return drv_rtc_count();

//    uint32_t timeDiff;

//    timeDiff = TIM_GetCounter(TIM2);
//    return timeDiff;
}

void HRS_INT_HANDLER(void)
{
    /*  Mask GPIO interrupt */
    GPIO_INTConfig(GPIO_PORT, GPIO_GetPin(HRS_INT), DISABLE);
    GPIO_MaskINTConfig(GPIO_PORT, GPIO_GetPin(HRS_INT), ENABLE);
    GPIO_ClearINTPendingBit(GPIO_PORT, GPIO_GetPin(HRS_INT));
    //DBG_DIRECT("HRS_INT_HANDLER");
    //vcHr02IRQFlag = true;
    vcHr02_process(SPORT_TYPE_NORMAL);



    GPIO_INTConfig(GPIO_PORT, GPIO_GetPin(HRS_INT), ENABLE);
    GPIO_MaskINTConfig(GPIO_PORT, GPIO_GetPin(HRS_INT), DISABLE);
}
#include "psp.h"
static PPSP_INST sInst;

static uint8_t ledPower[4];
static uint8_t adcGain[4];
static uint16_t acc[3][32];
static void psp_init()
{
    unsigned short data1size = 30;
    unsigned char data1[data1size];
    memset(data1, 0, data1size);
    PSP_GetVersionInfo(data1, &data1size);
    APP_PRINT_INFO1("PSP_GetVersionInfo:%b", TRACE_BINARY(data1size, data1));
#define PSP_MEMORY_SIZE (50000)

    uint32_t *pspmem = 0;
    pspmem = malloc(PSP_MEMORY_SIZE);
    APP_PRINT_INFO1("pspmem:%x", pspmem);
    PSP_ERROR pspError = PSP_ERROR_NONE;
    PSP_INST_PARAMS params;
    (void)PSP_GetDefaultParams(&params);
    APP_PRINT_INFO1("params.memorySize:%d", params.memorySize);
    APP_PRINT_INFO1("params.pMem:%p", params.pMem);
    APP_PRINT_INFO1("params.pSourceID:%s", params.pSourceID);
    APP_PRINT_INFO1("params.memorySize:%d", params.sourceIDSize);
    params.memorySize = PSP_MEMORY_SIZE;
    params.pMem = pspmem;
    params.pSourceID = (FX_UINT08 *) "ABCD123";
    params.sourceIDSize = 8;
    APP_PRINT_INFO1("params.memorySize:%d", params.memorySize);
    APP_PRINT_INFO1("params.pMem:%p", params.pMem);
    APP_PRINT_INFO1("params.pSourceID:%s", params.pSourceID);
    APP_PRINT_INFO1("params.memorySize:%d", params.sourceIDSize);
    pspError = PSP_Initialise(&params, &sInst);
    APP_PRINT_INFO1("params.memorySize:%d", params.memorySize);
    APP_PRINT_INFO1("params.pMem:%p", params.pMem);
    APP_PRINT_INFO1("params.pSourceID:%s", params.pSourceID);
    APP_PRINT_INFO1("params.memorySize:%d", params.sourceIDSize);
}
static void psp_get_hr(uint16_t *ppg)
{
    {

        PSP_METRIC_ID metricIdList[] = {PSP_METRIC_ID_HEARTRATE};
        PSP_EnableMetrics(sInst, sizeof(metricIdList), metricIdList);
        PSP_METRIC_ID requiredM[10];
        uint8_t numofrem = sizeof(requiredM);
        PSP_ListRequiredMetrics(sInst, requiredM, &numofrem);
//  APP_PRINT_INFO4("numofrem:%d,%x,%x,%x",numofrem, requiredM[0], requiredM[1],requiredM[2]);
#define PSP_NUMBER_OF_PPG_SAMPLES (32)
        {
            /* fill ppg metric array */
            int index = 0; static uint8_t data[32 * 2 + 18];
            PSP_METRIC_ID metricId = PSP_METRIC_ID_PPG_GREEN;

            int metricSize = (PSP_NUMBER_OF_PPG_SAMPLES * 2) + 15;
            data[index++] = PSP_METRIC_ID_PPG_GREEN;
            data[index++] = (uint8_t)(metricSize & 0xFF);
            data[index++] = (uint8_t)((metricSize >> 8) & 0xFF);
            data[index++] = 0;
            data[index++] = 4;
            data[index++] = 0;
            data[index++] = 0x60;
            data[index++] = 0x03;
            data[index++] = 0x00;
            data[index++] = 0x00;
            data[index++] = ledPower[0];
            data[index++] = ledPower[1];
            data[index++] = ledPower[2];
            data[index++] = ledPower[3];
            data[index++] = adcGain[0];
            data[index++] = adcGain[1];
            data[index++] = adcGain[2];
            data[index++] = adcGain[3];
            for (int sample = 0; sample < PSP_NUMBER_OF_PPG_SAMPLES; sample++)
            {
                data[index++] = (uint8_t)(ppg[sample] & 0xFF);
                data[index++] = (uint8_t)((ppg[sample] >> 8) & 0xFF);
            }
            /* transfer ppg metric to library */
            PSP_SetMetric(sInst, PSP_METRIC_ID_PPG_GREEN, data, index);
        }

        {

#define PSP_NUMBER_OF_ACC_SAMPLES (32)

            for (int i = 0; i < PSP_NUMBER_OF_ACC_SAMPLES; i++)
            {
                acc[0][i] = 0;
                acc[1][i] = 0;
                acc[2][i] = 512;
            }
            /* fill acc metric array */
            int index = 0; static uint8_t data[PSP_NUMBER_OF_ACC_SAMPLES * 6 + 7];
            PSP_METRIC_ID metricId = PSP_METRIC_ID_ACCELERATION;
            int metricSize = (PSP_NUMBER_OF_ACC_SAMPLES * 6) +
                             4;   // ( number of samples * 6 ) + index +Q + BP + AF
            data[index++] = PSP_METRIC_ID_ACCELERATION;
            data[index++] = (uint8_t)(metricSize & 0xFF);
            data[index++] = (uint8_t)((metricSize >> 8) & 0xFF);
            data[index++] = 0;
            data[index++] = 4;
            data[index++] = 0;
            data[index++] = 0x6E;
            for (int sample = 0; sample < PSP_NUMBER_OF_ACC_SAMPLES; sample++)
            {
                data[index++] = (uint8_t)(acc[0][sample] & 0xFF);   // X - LSB
                data[index++] = (uint8_t)((acc[0][sample] >> 8) & 0xFF);    // X - MSB
                data[index++] = (uint8_t)(acc[1][sample] & 0xFF);   // Y - LSB
                data[index++] = (uint8_t)((acc[1][sample] >> 8) & 0xFF);    // Y - MSB
                data[index++] = (uint8_t)(acc[2][sample] & 0xFF);   // Z - LSB
                data[index++] = (uint8_t)((acc[2][sample] >> 8) & 0xFF);    // Z - MSB
            }

            PSP_SetMetric(sInst, metricId, data, index);
        }
        {

            int index = 0; static uint8_t data[32 * 2 + 18];
            PSP_METRIC_ID metricId = PSP_METRIC_ID_PPG_AMBIENT;

            int metricSize = (PSP_NUMBER_OF_PPG_SAMPLES * 2) + 15;
            data[index++] = PSP_METRIC_ID_PPG_AMBIENT;
            data[index++] = (uint8_t)(metricSize & 0xFF);
            data[index++] = (uint8_t)((metricSize >> 8) & 0xFF);
            data[index++] = 0;
            data[index++] = 4;
            data[index++] = 0;
            data[index++] = 0x60;
            data[index++] = 0x03;
            data[index++] = 0x00;
            data[index++] = 0x00;

            data[index++] = ledPower[0];
            data[index++] = ledPower[1];
            data[index++] = ledPower[2];
            data[index++] = ledPower[3];
            data[index++] = adcGain[0];
            data[index++] = adcGain[1];
            data[index++] = adcGain[2];
            data[index++] = adcGain[3];
            for (int sample = 0; sample < PSP_NUMBER_OF_PPG_SAMPLES; sample++)
            {
                data[index++] = (uint8_t)(0 & 0xFF);
                data[index++] = (uint8_t)((0 >> 8) & 0xFF);
            }

            PSP_SetMetric(sInst, PSP_METRIC_ID_PPG_AMBIENT, data, index);
        }

        PSP_Process(sInst);

        PSP_METRIC_ID metrics[32];
        FX_UINT08 numberOfMetrics;
        numberOfMetrics = sizeof(metrics);
        PSP_ListUpdatedMetrics(sInst, metrics, &numberOfMetrics);
//APP_PRINT_INFO2("PSP_ListUpdatedMetrics:%d,%x",numberOfMetrics, metrics[0]);

        {
            /* Get updated output metric values */
            FX_UINT08 metricData[10];
            FX_UINT16 metricDataSize;

            metricDataSize = sizeof(metricData);
            int k = 0;
            PSP_GetMetric(sInst, metrics[k], metricData, &metricDataSize);
//if ( status == PSP_ERROR_NONE )
            {
                /* interpret metrics */

                {

                    /* interpret Heart Rate */
                    int metricSize = metricData[1];
                    metricSize |= metricData[2] << 8;
                    int metricIndex = metricData[3];
                    int metricQuality = metricData[4];
                    int metricHeartrate = metricData[5];
                    //APP_PRINT_INFO5("PSP_GetMetric:%x,%d,%d,%d, HR:%d",metricData[0],metricSize, metricIndex, metricQuality, metricHeartrate);
                    APP_PRINT_INFO1("[HEART RATE PSP IS %d]", metricHeartrate);
                }
            }
        }
    }
}
bool hrs_driver_init(void)
{
    psp_init();
    Pinmux_Config(HRS_INT, DWGPIO);
    Pad_Config(HRS_INT, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    /* Enable GPIO and hardware timer's clock */
    RCC_PeriphClockCmd(APBPeriph_GPIOA,  APBPeriph_GPIOA_CLOCK,  ENABLE);

    /* Initialize GPIO as interrupt mode */
    GPIO_InitTypeDef GPIO_Param;
    GPIO_StructInit(&GPIO_Param);
    GPIO_Param.GPIO_Pin = GPIO_GetPin(HRS_INT);
    GPIO_Param.GPIO_Mode = GPIO_Mode_IN;
    GPIO_Param.GPIO_ITCmd = ENABLE;
    GPIO_Param.GPIO_ITTrigger = GPIO_INT_Trigger_EDGE;
    GPIO_Param.GPIO_ITPolarity = GPIO_INT_POLARITY_ACTIVE_HIGH;
    GPIO_Init(GPIO_PORT, &GPIO_Param);

    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = HRS_INT_IRQ;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
    GPIO_INTConfig(GPIO_PORT, GPIO_GetPin(HRS_INT), ENABLE);
    GPIO_MaskINTConfig(GPIO_PORT, GPIO_GetPin(HRS_INT), DISABLE);
    //System_WakeUpPinEnable(HRS_INT, PAD_WAKEUP_POL_HIGH, PAD_WAKEUP_DEB_DISABLE);

//    Pinmux_Config(HRS_I2C_SCL, HRS_I2C_FUNC_SCL);
//    Pinmux_Config(HRS_I2C_SDA, HRS_I2C_FUNC_SDA);
//    {
//        /* add protect for I2C deadlock */
//        Pad_Config(HRS_I2C_SCL, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
//        Pad_Config(HRS_I2C_SDA, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_LOW);
//    }

//    Pad_Config(HRS_I2C_SCL, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE,
//               PAD_OUT_LOW);
//    Pad_Config(HRS_I2C_SDA, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE,
//               PAD_OUT_LOW);
    drv_i2c0_set_scl_sda(HRS_I2C_SCL, HRS_I2C_SDA);
    /* Enable I2C clock */

    vcHr02Init(VCWORK_MODE_HRWORK);


    return true;
}
void hrs_enter_dlps(void)
{
//    drvVCxxStopSample();
    Pad_Config(HRS_I2C_SCL, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(HRS_I2C_SDA, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
}
void hrs_exit_dlps(void)
{
    Pad_Config(HRS_I2C_SCL, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE,
               PAD_OUT_LOW);
    Pad_Config(HRS_I2C_SDA, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE,
               PAD_OUT_LOW);
}
