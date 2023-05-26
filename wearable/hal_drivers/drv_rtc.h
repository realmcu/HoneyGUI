
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


#ifdef __cplusplus
}
#endif

#endif
