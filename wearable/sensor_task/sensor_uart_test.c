/*
 * Copyright (c) 2022
 *
 */

#include <string.h>
#include "sensor_uart_test.h"
#include "trace.h"
#include <app_msg.h>
#include "sensor_msg.h"
#include "hp_msg_test.h"


static uint32_t count = 0;

void sensor_mode_uart_write(void)
{
    uint8_t buffer[5] = {0x0, 0x1, 0x2, 0x3, 0x4};
    drv_uart5_write(buffer, 5);

    return;
}

void sensor_rtc_int(void *p_value)
{
    DBG_DIRECT("sensor_rtc_int");
    if (count % 2 == 0)
    {
        T_IO_MSG msg;
        msg.subtype = SENSOR_MSG_UART_TEST;
        app_send_msg_to_sensor_task(&msg);
    }
    else
    {
        T_IO_MSG msg;
        msg.subtype = HP_MSG_UART_TEST;
        app_send_msg_to_hp_task(&msg);
    }

    count++;
}

void sensor_mode_uart_init(void)
{
    drv_uart5_init(SENSOR_UART_TX, SENSOR_UART_RX);

    drv_rtc_second_attach_irq(sensor_rtc_int, NULL);
}


/************** end of file ********************/
