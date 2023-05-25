/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include "app_util.h"

uint8_t app_util_calc_checksum(uint8_t *dataPtr, uint16_t len)
{
    uint8_t check_sum;

    check_sum = 0;
    while (len)
    {
        check_sum += *dataPtr;
        dataPtr++;
        len--;
    }
    return (uint8_t)(0xff - check_sum + 1); //((~check_sum)+1);
}
