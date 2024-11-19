/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-12-19     bernard      the first version
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "jerry_utf8.h"



bool jerry_str_is_utf8(const void *str, int size)
{
    bool ret = true;
    unsigned char *start = (unsigned char *)str;
    unsigned char *end = (unsigned char *)str + size;

    while (start < end)
    {
        if (*start < 0x80)              // (10000000): ASCII < 0x80
        {
            start++;
        }
        else if (*start < (0xC0))       // (11000000): 0x80 <= invalid ASCII < 0xC0
        {
            ret = false;
            break;
        }
        else if (*start < (0xE0))       // (11100000): two bytes UTF8
        {
            if (start >= end - 1)
            {
                break;
            }

            if ((start[1] & (0xC0)) != 0x80)
            {
                ret = false;
                break;
            }

            start += 2;
        }
        else if (*start < (0xF0))       // (11110000): three bytes UTF-8
        {
            if (start >= end - 2)
            {
                break;
            }

            if ((start[1] & (0xC0)) != 0x80 || (start[2] & (0xC0)) != 0x80)
            {
                ret = false;
                break;
            }

            start += 3;
        }
        else
        {
            ret = false;
            break;
        }
    }

    return ret;
}

int jerry_str2utf8(char *str, int len, char **utf8)
{


    return 0;
}
