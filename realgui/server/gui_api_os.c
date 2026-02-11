/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/

#include <stdarg.h>
#include "gui_api_os.h"
#include "guidef.h"
#include "tlsf.h"


/*============================================================================*
 *                            Macros
 *============================================================================*/



/*============================================================================*
 *                            Variables
 *============================================================================*/

static struct gui_os_api *os_api = NULL;
static tlsf_t tlsf = NULL;
static tlsf_t lower_tlsf = NULL;
static uint32_t total_used_size = 0;

/*============================================================================*
 *                           Private Functions
 *============================================================================*/

static void gui_walker(void *ptr, size_t size, int used, void *user)
{
    (void)user;
    if (used)
    {
        total_used_size = total_used_size + size;
    }

    gui_log("\t%p %s size: %x; total = %d\n", ptr, used ? "used" : "free", (unsigned int)size,
            total_used_size);
}
static void walker(void *ptr, size_t size, int used, void *user)
{
    (void)user;
    (void)ptr;
    if (used)
    {
        total_used_size = total_used_size + size;
    }
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

void gui_os_api_register(gui_os_api_t *info)
{

    if ((info->mem_addr != NULL) && (info->mem_size != 0))
    {
        tlsf = tlsf_create_with_pool(info->mem_addr, info->mem_size);
    }


    if ((info->lower_mem_addr != NULL) && (info->lower_mem_size != 0))
    {
        lower_tlsf = tlsf_create_with_pool(info->lower_mem_addr, info->lower_mem_size);
    }
    os_api = info;
}

tlsf_t gui_get_tlsf(void)
{
    return tlsf;
}

void *gui_thread_create(const char *name, void (*entry)(void *param), void *param,
                        uint32_t stack_size, uint8_t priority)
{
    if (os_api->thread_create)
    {
        return os_api->thread_create(name, entry, param, stack_size, priority);
    }
    return NULL;
}
bool gui_thread_delete(void *handle)
{
    if (os_api->thread_delete)
    {
        return os_api->thread_delete(handle);
    }
    return false;
}

bool gui_thread_suspend(void *handle)
{
    if (os_api->thread_suspend)
    {
        return os_api->thread_suspend(handle);
    }
    return false;
}

bool gui_thread_resume(void *handle)
{
    if (os_api->thread_resume)
    {
        return os_api->thread_resume(handle);
    }
    return false;
}

bool gui_thread_mdelay(uint32_t ms)
{
    if (os_api->thread_create)
    {
        return os_api->thread_mdelay(ms);
    }
    return false;
}
void gui_set_tick_hook(void (*hook)(void))
{
    os_api->gui_tick_hook = hook;
}

bool gui_mq_create(void *handle, const char *name, uint32_t msg_size, uint32_t max_msgs)
{
    if (os_api->mq_create)
    {
        return os_api->mq_create(handle, name, msg_size, max_msgs);
    }
    return NULL;
}

bool gui_mq_send(void *handle, void *buffer, uint32_t size, uint32_t timeout)
{
    if (os_api->mq_send)
    {
        return os_api->mq_send(handle, buffer, size, timeout);
    }
    return false;
}

bool gui_mq_send_urgent(void *handle, void *buffer, uint32_t size, uint32_t timeout)
{
    if (os_api->mq_send_urgent)
    {
        return os_api->mq_send_urgent(handle, buffer, size, timeout);
    }
    return false;
}

bool gui_mq_recv(void *handle, void *buffer, uint32_t size, uint32_t timeout)
{
    if (os_api->mq_recv)
    {
        return os_api->mq_recv(handle, buffer, size, timeout);
    }
    return false;
}


void *gui_malloc(size_t n)
{
    void *ptr = NULL;
    if ((n > os_api->mem_threshold_size) && (os_api->mem_threshold_size != 0))
    {
        ptr = gui_lower_malloc(n);
        return ptr;
    }
    if (tlsf != NULL)
    {
        ptr = tlsf_malloc(tlsf, n);
    }
    else
    {
        GUI_ASSERT(os_api->f_malloc != NULL);
        ptr = os_api->f_malloc(n);
    }
    if (ptr == NULL)
    {
        ptr = gui_lower_malloc(n);
    }
    // if (ptr == (void *)0x0000000000749D30)
    // {
    //     GUI_ASSERT(NULL != NULL);
    // }
    return ptr;
}

void *gui_calloc(size_t num, size_t size)
{
    void *ptr = NULL;
    size_t total_size = num * size;

    if (size != 0 && total_size / size != num)
    {
        return NULL;
    }

    if ((total_size > os_api->mem_threshold_size) && (os_api->mem_threshold_size != 0))
    {
        ptr = gui_lower_malloc(total_size);
        if (ptr != NULL)
        {
            memset(ptr, 0, total_size);
        }
        return ptr;
    }

    if (tlsf != NULL)
    {
        ptr = tlsf_malloc(tlsf, total_size);
    }
    else
    {
        GUI_ASSERT(os_api->f_malloc != NULL);
        ptr = os_api->f_malloc(total_size);
    }

    if (ptr == NULL)
    {
        ptr = gui_lower_malloc(total_size);
    }

    if (ptr != NULL)
    {
        memset(ptr, 0, total_size);
    }

    return ptr;
}

void *gui_realloc(void *ptr_old, size_t n)
{
    void *ptr = NULL;

    if (tlsf == NULL)
    {
        GUI_ASSERT(os_api->f_realloc != NULL);
        ptr = os_api->f_realloc(ptr_old, n);
        if (ptr == NULL)
        {
            return NULL;
        }
        return ptr;
    }
    else
    {
        if (
            (os_api->lower_mem_size != 0) && \
            ((uintptr_t)ptr_old >= (uintptr_t)os_api->lower_mem_addr) && \
            ((uintptr_t)ptr_old <= (uintptr_t)os_api->lower_mem_addr +
             (uint32_t)os_api->lower_mem_size)
        )
        {
            ptr = tlsf_realloc(lower_tlsf, ptr_old, n);
            return ptr;
        }
        else
        {
            ptr = tlsf_realloc(tlsf, ptr_old, n);
            if (ptr == NULL)
            {
                ptr = tlsf_malloc(lower_tlsf, n);
                memcpy(ptr, ptr_old, tlsf_block_size(ptr_old));
                tlsf_free(tlsf, ptr_old);
            }
            // if (ptr == (void *)0x0000000000749D30)
            // {
            //     GUI_ASSERT(NULL != NULL);
            // }
            return ptr;
        }
    }

}

void gui_free(void *rmem)
{

    if ((rmem != NULL) && \
        ((uintptr_t)rmem >= (uintptr_t)os_api->lower_mem_addr) && \
        ((uintptr_t)rmem <= (uintptr_t)os_api->lower_mem_addr +
         (uint32_t)os_api->lower_mem_size)
       )
    {
        GUI_ASSERT((uintptr_t)os_api->lower_mem_addr != 0);
        GUI_ASSERT((uintptr_t)os_api->lower_mem_size != 0);
        gui_lower_free(rmem);
        return;
    }
    if (tlsf != NULL)
    {
        tlsf_free(tlsf, rmem);
    }
    else
    {
        GUI_ASSERT(os_api->f_free != NULL);
        os_api->f_free(rmem);
    }
}

char *gui_strdup(const char *s)
{
    if (s == NULL)
    {
        return NULL;
    }

    size_t len = strlen(s) + 1;
    char *dup = (char *)gui_malloc(len);

    if (dup != NULL)
    {
        memcpy(dup, s, len);
    }

    return dup;
}

void gui_mem_debug(void)
{
    total_used_size = 0;
    GUI_UNUSED(total_used_size);
    GUI_ASSERT(tlsf != NULL);
    gui_log("\t\n");
    tlsf_walk_pool(tlsf_get_pool(tlsf), gui_walker, &total_used_size);
    gui_log("\t\n");
    total_used_size = 0;
}
uint32_t gui_mem_used(void)
{
    total_used_size = 0;
    GUI_UNUSED(total_used_size);
    GUI_ASSERT(tlsf != NULL);
    tlsf_walk_pool(tlsf_get_pool(tlsf), walker, &total_used_size);
    return total_used_size;
}

void *gui_lower_malloc(size_t n)
{
    void *ptr = NULL;
    GUI_ASSERT(lower_tlsf != NULL);
    ptr = tlsf_malloc(lower_tlsf, n);
    if (ptr == NULL)
    {
        GUI_ASSERT(NULL != NULL);
    }
    return ptr;
}

void *gui_lower_realloc(void *ptr_old, size_t n)
{
    void *ptr = NULL;
    GUI_ASSERT(lower_tlsf != NULL);
    ptr = tlsf_realloc(lower_tlsf, ptr_old, n);
    if (ptr == NULL)
    {
        GUI_ASSERT(NULL != NULL);
    }
    return ptr;
}

void gui_lower_free(void *rmem)
{
    GUI_ASSERT(lower_tlsf != NULL);
    tlsf_free(lower_tlsf, rmem);
}



void gui_lower_mem_debug(void)
{
    GUI_ASSERT(lower_tlsf != NULL);
    tlsf_walk_pool(tlsf_get_pool(lower_tlsf), NULL, NULL);
}
uint32_t gui_low_mem_used(void)
{
    total_used_size = 0;
    GUI_UNUSED(total_used_size);
    GUI_ASSERT(lower_tlsf != NULL);
    tlsf_walk_pool(tlsf_get_pool(lower_tlsf), walker, &total_used_size);
    return total_used_size;
}
void gui_sleep_cb(void)
{
    if (os_api->gui_sleep_cb != NULL)
    {
        os_api->gui_sleep_cb();
    }
}

void gui_log(const char *format, ...)
{
    if (os_api->log == NULL)
    {
        return;
    }
    va_list args;
    va_start(args, format);
    char buf[1024];
    vsnprintf(buf, sizeof(buf), format, args);
    os_api->log(buf);
    va_end(args);
}

void gui_log_hexdump(const char *name, uint8_t *buf, uint16_t size)
{
    gui_log("gui Hex: %s \n", name);
    for (uint16_t i = 0; i < (size - 1); i++)
    {
        gui_log("0x%x-", buf[i]);
    }
    gui_log("0x%x\n", buf[size - 1]);
}

void gui_assert_handler(const char *ex_string, const char *func, uint32_t line)
{
    gui_log("\033[1;31m");
    gui_log("(%s) assertion failed at function:%s, line number:%d \n", ex_string, func, line);
    gui_log("\033[0m");
#ifdef _HONEYGUI_SIMULATOR_
    abort();
#else
    *(volatile uint32_t *)0xFFFFFFFF = 0;
#endif
    while (1);
}


uint32_t gui_ms_get(void)
{
    if (os_api->thread_ms_get == NULL)
    {
        return 0;
    }

    return os_api->thread_ms_get();
}

uint32_t gui_us_get(void)
{
    if (os_api->thread_us_get == NULL)
    {
        return 0;
    }

    return os_api->thread_us_get();
}
