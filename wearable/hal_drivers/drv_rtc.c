/*
 * Copyright (c) 2022
 *
 *
 *
 *
 *
 *
 */
#include "wristband_sdk_config.h"
#if (BSP_USING_RTC == 1)
#include "rtl876x.h"
#include "rtl_rtc.h"
#include "rtl_nvic.h"
#include "trace.h"
#include "drv_rtc.h"


struct rtl_rtc_config rtc_config[] =
{
#if (BSP_USING_RTC_COMP0 == 1)
    RTC_COMP0_CONFIG,
#endif
#if (BSP_USING_RTC_COMP1 == 1)
    RTC_COMP1_CONFIG,
#endif
#if (BSP_USING_RTC_COMP2 == 1)
    RTC_COMP2_CONFIG,
#endif
#if (BSP_USING_RTC_COMP3 == 1)
    RTC_COMP3_CONFIG,
#endif
};


void mcu_rtc_set_comp(bool start, time_t time_stamp)
{
    uint32_t CompareValue;

    if (start == true)
    {
        CompareValue = RTC_GetCounter() + (rtc_config[WATCH_CLOCK_RTC_COMP_INDEX].rtc_clock_src_freq /
                                           (rtc_config[WATCH_CLOCK_RTC_COMP_INDEX].rtc_prescaler_vaule + 1)) * (60 - time_stamp % 60);
    }
    else
    {
        CompareValue = RTC_GetCompValue(rtc_config[WATCH_CLOCK_RTC_COMP_INDEX].rtc_comp_index) +
                       (rtc_config[WATCH_CLOCK_RTC_COMP_INDEX].rtc_clock_src_freq /
                        (rtc_config[WATCH_CLOCK_RTC_COMP_INDEX].rtc_prescaler_vaule + 1)) * (60 - time_stamp % 60);
    }

    RTC_SetCompValue(rtc_config[WATCH_CLOCK_RTC_COMP_INDEX].rtc_comp_index, CompareValue & 0xFFFFFFFF);
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

#if (MODULE_USING_WATCH_CLOCK == 1)
    if (RTC_GetINTStatus(rtc_config[WATCH_CLOCK_RTC_COMP_INDEX].rtc_int_comp) == SET)
    {
        watch_clock_update();
        RTC_ClearINTPendingBit(rtc_config[WATCH_CLOCK_RTC_COMP_INDEX].rtc_int_comp);
        RTC_ClearCompINT(rtc_config[WATCH_CLOCK_RTC_COMP_INDEX].rtc_comp_index);
        RTC_ClearWakeupStatusBit(rtc_config[WATCH_CLOCK_RTC_COMP_INDEX].rtc_wk);
    }
#endif

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

/**
  * @brief   rtc init
  * @param   init offset secod, sometimes is zero
  * @return  void
  */
static void mcu_rtc_init(struct rtl_rtc_config *rtc_cfg)
{
    RTC_SetPrescaler(rtc_cfg->rtc_prescaler_vaule);

    RTC_INTConfig(rtc_cfg->rtc_int_comp, ENABLE);
}

static void mcu_rtc_irq_init(void)
{
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = RTC_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}

static void rtc_enter_dlps(void *drv_io)
{
    struct rtl_rtc_config *rtc_cfg = drv_io;
}

static void rtc_exit_dlps(void *drv_io)
{
    struct rtl_rtc_config *rtc_cfg = drv_io;

    mcu_rtc_irq_init();
    DBG_DIRECT("rtc_exit_dlps");
}

static bool rtc_allowed_enter_dlps_check(void *drv_io)
{
    struct rtl_rtc_config *rtc_cfg = drv_io;
    return rtc_cfg->rtc_enter_dlps_flag;
}

static bool rtc_system_wakeup_dlps_check(void *drv_io)
{
    struct rtl_rtc_config *rtc_cfg = drv_io;
//    if (HAL_READ32(SOC_VENDOR2_REG_BASE, 0x0058) == 0x80)
//    {
//        DBG_DIRECT("RTC Wake up");
//        rtc_cfg->rtc_enter_dlps_flag = false;
//        return true;
//    }
    return false;
}

void drv_rtc_dlps_handler_cbacks_register(struct rtl_rtc_config *rtc_cfg)
{
    if (rtc_cfg->rtc_dlps_register_flag == false)
    {
        rtc_cfg->cbacks.enter_dlps_cb = rtc_enter_dlps;
        rtc_cfg->cbacks.exit_dlps_cb = rtc_exit_dlps;
        rtc_cfg->cbacks.allowed_dlps_check_cb = rtc_allowed_enter_dlps_check;
        rtc_cfg->cbacks.system_wakeup_check_cb = rtc_system_wakeup_dlps_check;

#if BSP_USING_DLPS
        drv_dlps_handler_cbacks_register(&rtc_cfg->cbacks, rtc_cfg);
#endif

        rtc_cfg->rtc_dlps_register_flag = true;
    }
}

void drv_rtc_dlps_init(struct rtl_rtc_config *rtc_cfg)
{
    rtc_cfg->rtc_enter_dlps_flag = true;
    RTC_WKConfig(rtc_cfg->rtc_wk, ENABLE);
    RTC_SystemWakeupConfig(ENABLE);
    drv_rtc_dlps_handler_cbacks_register(rtc_cfg);
}

void hw_rtc_init(void)
{
#if (MODULE_USING_WATCH_CLOCK == 1)
    mcu_rtc_init(&(rtc_config[WATCH_CLOCK_RTC_COMP_INDEX]));
    mcu_rtc_irq_init();
    rtc_config[WATCH_CLOCK_RTC_COMP_INDEX].rtc_count = RTC_GetCounter;
    rtc_config[WATCH_CLOCK_RTC_COMP_INDEX].set_comp = mcu_rtc_set_comp;
    watch_clock_init(0);
    //drv_rtc_dlps_init(&rtc_config[WATCH_CLOCK_RTC_COMP_INDEX]);
#endif

    RTC_NvCmd(ENABLE);
    RTC_ResetCounter();
    RTC_Cmd(ENABLE);
}

#endif

