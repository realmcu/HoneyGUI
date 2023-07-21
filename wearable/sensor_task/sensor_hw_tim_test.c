/*
 * Copyright (c) 2022
 *
 */

#include <string.h>
#include "sensor_hw_tim_test.h"
#include "trace.h"
#include <app_msg.h>
#include "sensor_msg.h"
#include "hp_msg_test.h"


uint32_t sensor_mode_hw_tim_read(void)
{
    return drv_tim6_read();
}

void sensor_mode_hw_tim_init(void)
{
    drv_tim6_init();
    drv_tim6_start();
}


/************** end of file ********************/
