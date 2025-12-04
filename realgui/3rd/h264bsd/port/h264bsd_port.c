/**
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file h264bsd_port.c
  * @brief Custom configuration
  * @details Custom configuration
  * @author wanghao@realsil.com.cn
  * @date 2025/4/11
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2025 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */


#include <stdlib.h>
#include <string.h>
#include "h264bsd_port.h"


#ifdef ENABLE_HONEYGUI
#include "gui_api.h"
void *h264bsd_port_malloc(size_t size)
{
    return gui_malloc(size);
}

void *h264bsd_port_realloc(void *ptr, size_t size)
{
    return gui_realloc(ptr, size);
}

void h264bsd_port_free(void *ptr)
{
    gui_free(ptr);
}
#else

__attribute__((weak))  void *h264bsd_port_malloc(size_t size)
{
    return malloc(size);
}

__attribute__((weak))  void *h264bsd_port_realloc(void *ptr, size_t size)
{
    return realloc(ptr, size);
}

__attribute__((weak))  void h264bsd_port_free(void *ptr)
{
    free(ptr);
}

#endif


void *h264bsd_realloc(void *ptr, size_t size)
{
    return h264bsd_port_realloc(ptr, size);
}

void *h264bsd_malloc(size_t size)
{
    return h264bsd_port_malloc(size);
}
void h264bsd_free(void *ptr)
{
    h264bsd_port_free(ptr);
}

void *h264bsd_lower_realloc(void *ptr, size_t size)
{
    return h264bsd_port_realloc(ptr, size);
}

void *h264bsd_lower_malloc(size_t size)
{
    return h264bsd_port_malloc(size);
}
void h264bsd_lower_free(void *ptr)
{
    h264bsd_port_free(ptr);
}
