/*
 * Copyright (c) 2022
 *
 */

#include <string.h>
#include "touch_816s_8772f_454.h"
#include "sensor_touch_test.h"
#include "trace.h"
#include "drv_gpio.h"
#include <app_msg.h>
#include "sensor_msg.h"


void sensor_mode_uart_write(void)
{
    uint8_t buffer[5] = {0x0, 0x1, 0x2, 0x3, 0x4};
    drv_uart4_write(buffer, 5);

    return;
}



void sensor_rtc_int(void *p_value)
{
    DBG_DIRECT("sensor_rtc_int");
    T_IO_MSG msg;
    msg.subtype = SENSOR_MSG_UART_TEST;
    app_send_msg_to_sensor_task(&msg);
}

void sensor_mode_uart_rtc_int_init(void)
{
    drv_rtc_second_attach_irq(sensor_rtc_int, NULL);
}

/************** end of file ********************/
