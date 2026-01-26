/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdlib.h>
#include <string.h>


#ifdef ENABLE_HONEYGUI
#include "gui_api.h"
void *nanovg_port_malloc(size_t size)
{
    return gui_malloc(size);
}

void *nanovg_port_realloc(void *ptr, size_t size)
{
    return gui_realloc(ptr, size);
}

void nanovg_port_free(void *ptr)
{
    gui_free(ptr);
}
#else

__attribute__((weak))  void *nanovg_port_malloc(size_t size)
{
    return malloc(size);
}

__attribute__((weak))  void *nanovg_port_realloc(void *ptr, size_t size)
{
    return realloc(ptr, size);
}

__attribute__((weak))  void nanovg_port_free(void *ptr)
{
    free(ptr);
}

#endif


void *nanovg_realloc(void *ptr, size_t size)
{
    return nanovg_port_realloc(ptr, size);
}

void *nanovg_malloc(size_t size)
{
    return nanovg_port_malloc(size);
}
void nanovg_free(void *ptr)
{
    nanovg_port_free(ptr);
}

