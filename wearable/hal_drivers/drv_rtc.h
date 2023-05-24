
/*
 * Copyright (c) 2022
 *
 *
 *
 *
 *
 *
 */

#ifndef __DRV_RTC_H__
#define __DRV_RTC_H__


#ifdef __cplusplus
extern "C" {
#endif


#include "stdbool.h"
#include "rtl_rcc.h"
#include "rtl_rtc.h"
#include "rtl_nvic.h"
#include "rtl_pinmux.h"
#include "wristband_sdk_config.h"
#include "drv_dlps.h"
#include "time.h"


#define _CONTACT_RTC_COMP_INDEX(x) RTC_COMP##x##_INDEX
#define CONTACT_RTC_COMP_INDEX(x) _CONTACT_RTC_COMP_INDEX(x)

#if (MODULE_USING_WATCH_CLOCK == 1)
#define WATCH_CLOCK_RTC_COMP_INDEX CONTACT_RTC_COMP_INDEX(WATCH_CLOCK_RTC_COMP)
#endif


struct rtl_rtc_config
{
    const char *name;
    RTC_COMP_INDEX_T rtc_comp_index;
    uint32_t rtc_int_comp;
    uint32_t rtc_prescaler_vaule;
    uint32_t rtc_clock_src_freq;
    uint32_t (*rtc_count)(void);
    void (*set_comp)(bool start, time_t time_stamp);
    bool rtc_enter_dlps_flag;
    bool rtc_dlps_register_flag;
    uint32_t rtc_wk;
    T_DRV_DLPS_CBS cbacks;
};

#define RTC_COMP0_CONFIG                                               \
    {                                                              \
        .name = "rtc_comp0",                                           \
                .rtc_comp_index = RTC_COMP0,                                \
                                  .rtc_int_comp = RTC_INT_COMP0,                              \
                                                  .rtc_wk = RTC_WK_COMP0,                   \
                                                            .rtc_prescaler_vaule = 0,                                   \
                                                                                   .rtc_clock_src_freq = 32000,                                \
    }
#define RTC_COMP1_CONFIG                                               \
    {                                                              \
        .name = "rtc_comp1",                                           \
                .rtc_comp_index = RTC_COMP1,                                \
                                  .rtc_int_comp = RTC_INT_COMP1,                              \
                                                  .rtc_prescaler_vaule = 0,                                   \
                                                                         .rtc_clock_src_freq = 32000,                                \
    }
#define RTC_COMP2_CONFIG                                               \
    {                                                              \
        .name = "rtc_comp2",                                           \
                .rtc_comp_index = RTC_COMP2,                                \
                                  .rtc_int_comp = RTC_INT_COMP2,                              \
                                                  .rtc_prescaler_vaule = 0,                                  \
                                                                         .rtc_clock_src_freq = 32000,                                \
    }
#define RTC_COMP3_CONFIG                                               \
    {                                                              \
        .name = "rtc_comp3",                                           \
                .rtc_comp_index = RTC_COMP3,                                \
                                  .rtc_int_comp = RTC_INT_COMP3,                              \
                                                  .rtc_prescaler_vaule = 0,                                   \
                                                                         .rtc_clock_src_freq = 32000,                                \
    }

enum
{
#if (BSP_USING_RTC_COMP0 == 1)
    RTC_COMP0_INDEX,
#endif
#if (BSP_USING_RTC_COMP1 == 1)
    RTC_COMP1_INDEX,
#endif
#if (BSP_USING_RTC_COMP2 == 1)
    RTC_COMP2_INDEX,
#endif
#if (BSP_USING_RTC_COMP3 == 1)
    RTC_COMP3_INDEX,
#endif
};

#ifdef __cplusplus
}
#endif

#endif
