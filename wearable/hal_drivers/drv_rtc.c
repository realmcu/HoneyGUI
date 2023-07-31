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
#include "rtl_hal_peripheral.h"
#include "drv_rtc.h"
#include "trace.h"
#include "patch_header_check.h"

#define RTC_SRC_FREQ            32000
#define RTC_PRESCALER_VAL       0




static drv_rtc_irq comp0_irq;
static drv_rtc_irq comp1_irq;
static bool drv_rtc_started = false;

static bool rtc_system_wakeup_dlps_check(void)
{

#if defined RTL8772F
    if (HAL_READ32(SOC_VENDOR2_REG_BASE, 0x0058) == 0x80)
    {
        DBG_DIRECT("RTC Wake up");
        return true;
    }
#endif

    return false;
}

void drv_rtc_dlps_init(void)
{
    RTC_SystemWakeupConfig(ENABLE);
    drv_dlps_wakeup_cbacks_register("rtc", rtc_system_wakeup_dlps_check);
}

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

    drv_rtc_started = true;

    drv_rtc_second_attach_irq(NULL, NULL);

    drv_rtc_dlps_init();
}

uint32_t drv_rtc_count(void)
{
    return RTC_GetCounter();
}

uint32_t drv_rtc_clock_src_freq(void)
{
    return RTC_SRC_FREQ;
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
#if defined RTL8772F || defined RTL8762G
    RTC_WKConfig(RTC_WK_COMP0, ENABLE);
#endif
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
#if defined RTL8772F || defined RTL8762G
    RTC_WKConfig(RTC_WK_COMP1, ENABLE);
#endif
    RTC_INTConfig(RTC_INT_COMP1, ENABLE);
}

void drv_rtc_set_comp(bool start, time_t time_stamp)
{
    uint32_t CompareValue;

    if (start == true)
    {
        CompareValue = RTC_GetCounter() + (RTC_SRC_FREQ / (RTC_PRESCALER_VAL + 1)) * (60 - time_stamp % 60);
    }
    else
    {
        CompareValue = RTC_GetCompValue(RTC_COMP1) + (RTC_SRC_FREQ / (RTC_PRESCALER_VAL + 1)) *
                       (60 - time_stamp % 60);
    }

    RTC_SetCompValue(RTC_COMP1, CompareValue & 0xFFFFFFFF);
}

/**
  * @brief   RTC interrupt handler
  * @param   No parameter.
  * @return  void
  */
void RTC_Handler()
{
    //uint32_t CompareValue = 0;
#if defined RTL8772F || defined RTL8762G
    DBG_DIRECT("RTC->CR0 = 0x%x, RTC->INT_CLR = 0x%x, RTC->INT_SR = 0x%x, RTC->CNT = 0x%x, RTC->COMP0 = 0x%x",
               RTC->RTC_CR0, RTC->RTC_INT_CLEAR, RTC->RTC_INT_SR, RTC->RTC_CNT0, RTC->RTC_COMP_0);
    DBG_DIRECT("refuse_reason 0x%x error_code 0x%x", platform_pm_get_refuse_reason(),
               platform_pm_get_error_code());
#endif

    if (RTC_GetINTStatus(RTC_INT_COMP0) == SET)
    {
        RTC_ClearINTPendingBit(RTC_INT_COMP0);
        RTC_ClearCompINT(RTC_COMP0);
#if defined RTL8772F || defined RTL8762G
        RTC_ClearWakeupStatusBit(RTC_WK_COMP0);
#endif
        uint32_t CompareValue;
        CompareValue = RTC_GetCompValue(RTC_COMP0) + (RTC_SRC_FREQ / (RTC_PRESCALER_VAL + 1));
        RTC_SetCompValue(RTC_COMP0, CompareValue & 0xFFFFFFFF);
        if (comp0_irq.rtc_cb != NULL)
        {
            comp0_irq.rtc_cb(comp0_irq.args);
        }
    }
    if (RTC_GetINTStatus(RTC_INT_COMP1) == SET)
    {
        RTC_ClearINTPendingBit(RTC_INT_COMP1);
        RTC_ClearCompINT(RTC_COMP1);
#if defined RTL8772F || defined RTL8762G
        RTC_ClearWakeupStatusBit(RTC_WK_COMP1);
#endif
        if (comp1_irq.rtc_cb != NULL)
        {
            comp1_irq.rtc_cb(comp1_irq.args);
        }
    }
    if (RTC_GetINTStatus(RTC_INT_COMP2) == SET)
    {
        RTC_ClearINTPendingBit(RTC_INT_COMP2);
        RTC_ClearCompINT(RTC_COMP2);
#if defined RTL8772F || defined RTL8762G
        RTC_ClearWakeupStatusBit(RTC_WK_COMP2);
#endif
    }
    if (RTC_GetINTStatus(RTC_INT_COMP3) == SET)
    {
        RTC_ClearINTPendingBit(RTC_INT_COMP3);
        RTC_ClearCompINT(RTC_COMP3);
#if defined RTL8772F || defined RTL8762G
        RTC_ClearWakeupStatusBit(RTC_WK_COMP2);
#endif
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



