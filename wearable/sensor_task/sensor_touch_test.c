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


void sensor_mode_i2c_read(void)
{
    uint16_t x = 0;
    uint16_t y = 0;
    bool pressing = 0;

    drv_touch_read(&x, &y, &pressing);

    DBG_DIRECT("sensor_mode_i2c_read x %d y %d", x, y);

    return;
}

uint32_t sensor_mode_i2c_write(uint16_t slave_addr, uint8_t *buffer, uint32_t size)
{
    return 0;
}

void sensor_touch_int(void)
{
    T_IO_MSG msg;
    msg.subtype = SENSOR_MSG_TOUCH_TEST;
    app_send_msg_to_sensor_task(&msg);
}

void sensor_mode_touch_int_init(void)
{
    rtk_touch_hal_set_indicate(sensor_touch_int);

    rtk_touch_hal_int_config(1);
}

/************** end of file ********************/
