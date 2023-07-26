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

static bool uart_system_wakeup_dlps_check(void)
{
    if (System_WakeUpInterruptValue(SENSOR_UART_RX) == SET)
    {
        Pad_ClearWakeupINTPendingBit(SENSOR_UART_RX);
        System_WakeUpPinDisable(SENSOR_UART_RX);
        DBG_DIRECT("Uart5 Wake up");
        return true;
    }
    return false;
}

void drv_uart5_dlps_init(void)
{
    System_WakeUpPinEnable(SENSOR_UART_RX, PAD_WAKEUP_POL_LOW, PAD_WAKEUP_DEB_DISABLE);
    drv_dlps_wakeup_cbacks_register("uart5", uart_system_wakeup_dlps_check);
}

void sensor_mode_uart_init(void)
{
    drv_uart5_init(SENSOR_UART_TX, SENSOR_UART_RX);
    drv_uart5_dlps_init();

    drv_rtc_second_attach_irq(sensor_rtc_int, NULL);
}


/************** end of file ********************/
