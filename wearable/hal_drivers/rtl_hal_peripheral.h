/*
 * Copyright (c) 2022
 *
 *
 *
 *
 *
 *
 */

#ifndef __RTL_HAL_PERIPHERAL_H__
#define __RTL_HAL_PERIPHERAL_H__


#if defined RTL8772F || defined RTL8762G

#include "app_section.h"
#include "utils.h"
#include "rtl_nvic.h"
#include "rtl_pinmux.h"
#include "rtl_rcc.h"
#include "rtl_io_dlps.h"
#include "rtl_uart.h"
#include "rtl_tim.h"
#include "flash_nor_device.h"

#include "rtl_gpio.h"
#include "rtl_i2c.h"
#include "rtl_spi.h"
#include "rtl_gdma.h"
#include "rtl_rtc.h"
#include "rtl_lcdc.h"
#include "rtl_lcdc_dbib.h"
#include "rtl_lcdc_dbic.h"
#include "rtl_lcdc_edpi.h"



#define RTL_HAL_RAM_CODE    RAM_FUNCTION




#elif defined RTL8762D

#include "app_section.h"
#include "platform_utils.h"
#include "rtl876x_nvic.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_rcc.h"
#include "rtl876x_io_dlps.h"
#include "rtl876x_uart.h"
#include "flash_device.h"


#include "rtl876x_gpio.h"
#include "rtl876x_i2c.h"
#include "rtl876x_spi.h"
#include "rtl876x_gdma.h"
#include "rtl876x_rtc.h"
#include "rtl876x_if8080.h"



#define RTL_HAL_RAM_CODE    DATA_RAM_FUNCTION

#endif






#endif  /* __RTL_HAL_PERIPHERAL_H__ */

/******************* end of file ***************/
