/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*
 * File      : nanovg_port.h
 */
#ifndef __NANOVG_PORT_H__
#define __NANOVG_PORT_H__


#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

void *nanovg_malloc(size_t size);
void *nanovg_realloc(void *ptr, size_t size);
void nanovg_free(void *ptr);



#ifdef __cplusplus
}
#endif

#endif
