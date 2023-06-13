
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
#include "rtl_hal_peripheral.h"
#include "drv_dlps.h"
#include "time.h"


typedef void (* rtc_irq_cb)(void *args);

typedef struct
{
    rtc_irq_cb rtc_cb;
    void *args;
} drv_rtc_irq;


void drv_rtc_second_attach_irq(void (*hdr)(void *args), void *args);
void drv_rtc_minute_attach_irq(void (*hdr)(void *args), void *args);

uint32_t drv_rtc_count(void);
uint32_t drv_rtc_clock_src_freq(void);
void drv_rtc_set_comp(bool start, time_t time_stamp);


#ifdef __cplusplus
}
#endif

#endif
