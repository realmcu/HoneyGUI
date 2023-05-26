/*
 * Copyright (c) 2022
 *
 *
 *
 *
 *
 *
 */

#include "rtl876x.h"
#include "rtl_rtc.h"
#include "rtl_nvic.h"
#include "trace.h"
#include "drv_rtc.h"

#define RTC_SRC_FREQ            32000
#define RTC_PRESCALER_VAL       0




static drv_rtc_irq comp0_irq;
static drv_rtc_irq comp1_irq;
static bool drv_rtc_started = false;


void drv_rtc_init(void)
{
    RTC_NvCmd(ENABLE);
    RTC_ResetCounter();
    RTC_Cmd(ENABLE);
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = RTC_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
    RTC_SetPrescaler(0);//no div
    RTC_SystemWakeupConfig(ENABLE);
    drv_rtc_started = true;
}



void drv_rtc_second_attach_irq(void (*hdr)(void *args), void *args)
{
    if (drv_rtc_started == false)
    {
        drv_rtc_init();
    }
    comp0_irq.rtc_cb = hdr;
    comp0_irq.args = args;

    uint32_t CompareValue;
    CompareValue = RTC_GetCounter() + RTC_SRC_FREQ / (RTC_PRESCALER_VAL + 1);
    RTC_SetCompValue(RTC_COMP0, CompareValue & 0xFFFFFFFF);
    RTC_WKConfig(RTC_WK_COMP0, ENABLE);
    RTC_INTConfig(RTC_INT_COMP0, ENABLE);
}

void drv_rtc_minute_attach_irq(void (*hdr)(void *args), void *args)
{
    if (drv_rtc_started == false)
    {
        drv_rtc_init();
    }
    comp1_irq.rtc_cb = hdr;
    comp1_irq.args = args;

    uint32_t CompareValue;
    CompareValue = RTC_GetCounter() + RTC_SRC_FREQ / (RTC_PRESCALER_VAL + 1) * 60; //todo
    RTC_SetCompValue(RTC_COMP1, CompareValue & 0xFFFFFFFF);
    RTC_INTConfig(RTC_INT_COMP1, ENABLE);
}

void mcu_rtc_set_comp(bool start, time_t time_stamp)
{
    uint32_t CompareValue;

    if (start == true)
    {
        CompareValue = RTC_GetCounter() + (RTC_SRC_FREQ / (RTC_PRESCALER_VAL + 1)) * (60 - time_stamp % 60);
    }
    else
    {
        CompareValue = RTC_GetCompValue(RTC_COMP0) + (RTC_SRC_FREQ / (RTC_PRESCALER_VAL + 1)) *
                       (60 - time_stamp % 60);
    }

    RTC_SetCompValue(RTC_COMP0, CompareValue & 0xFFFFFFFF);
}

/**
  * @brief   RTC interrupt handler
  * @param   No parameter.
  * @return  void
  */
void RTC_Handler()
{
    uint32_t CompareValue = 0;
    APP_PRINT_INFO5("RTC->CR0 = 0x%x, RTC->INT_CLR = 0x%x, RTC->INT_SR = 0x%x, RTC->CNT = 0x%x, RTC->COMP0 = 0x%x",
                    RTC->RTC_CR0, RTC->RTC_INT_CLEAR, RTC->RTC_INT_SR, RTC->RTC_CNT0, RTC->RTC_COMP_0);

    if (RTC_GetINTStatus(RTC_INT_COMP0) == SET)
    {
        RTC_ClearINTPendingBit(RTC_INT_COMP0);
        RTC_ClearCompINT(RTC_COMP0);
        RTC_ClearWakeupStatusBit(RTC_WK_COMP0);
        uint32_t CompareValue;
        CompareValue = RTC_GetCompValue(RTC_COMP0) + (RTC_SRC_FREQ / (RTC_PRESCALER_VAL + 1));
        RTC_SetCompValue(RTC_COMP0, CompareValue & 0xFFFFFFFF);
        comp0_irq.rtc_cb(comp0_irq.args);
    }
    if (RTC_GetINTStatus(RTC_INT_COMP1) == SET)
    {
        RTC_ClearINTPendingBit(RTC_INT_COMP1);
        RTC_ClearCompINT(RTC_COMP1);
        RTC_ClearWakeupStatusBit(RTC_WK_COMP1);
    }
    if (RTC_GetINTStatus(RTC_INT_COMP2) == SET)
    {
        RTC_ClearINTPendingBit(RTC_INT_COMP2);
        RTC_ClearCompINT(RTC_COMP2);
        RTC_ClearWakeupStatusBit(RTC_WK_COMP2);
    }
    if (RTC_GetINTStatus(RTC_INT_COMP3) == SET)
    {
        RTC_ClearINTPendingBit(RTC_INT_COMP3);
        RTC_ClearCompINT(RTC_COMP3);
        RTC_ClearWakeupStatusBit(RTC_WK_COMP3);
    }

    if (RTC_GetINTStatus(RTC_INT_TICK) == SET)
    {
        APP_PRINT_INFO0("RTC_INT_TICK");
        RTC_ClearINTPendingBit(RTC_INT_TICK);
    }

    if (RTC_GetINTStatus(RTC_INT_OVF) == SET)
    {
        APP_PRINT_INFO0("RTC Overflow");
        RTC_ClearOverFlowINT();
    }
}



