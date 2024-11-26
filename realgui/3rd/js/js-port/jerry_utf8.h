/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-12-19     bernard      the first version
 */

#ifndef JERRY_UTF8_H__
#define JERRY_UTF8_H__

#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

bool jerry_str_is_utf8(const void *str, int size);
int  jerry_str2utf8(char *str, int len, char **utf8);

#ifdef __cplusplus
}
#endif

#endif
