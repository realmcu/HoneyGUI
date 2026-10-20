/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#define _GNU_SOURCE
#include "guidef.h"
#include "gui_port.h"
#include "gui_api.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdarg.h"
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>


static volatile uint32_t gui_tick;

/*============================================================================*
 *                            Thread helpers
 *============================================================================*/

static void *port_thread_create(const char *name, void (*entry)(void *param), void *param,
                                uint32_t stack_size, uint8_t priority)
{
    (void)stack_size;
    (void)priority;
    pthread_t *thread = malloc(sizeof(pthread_t));
    pthread_create(thread, NULL, (void *(*)(void *))entry, param);
    pthread_setname_np(*thread, name);
    return thread;
}

static bool port_thread_delete(void *handle)
{
    pthread_t thread = *((pthread_t *)handle);
    void *ret = NULL;
    int res = 0;
    res = pthread_cancel(thread);
    pthread_join(thread, &ret);
    free(handle);
    return res;
}

static bool port_thread_mdelay(uint32_t ms)
{
    usleep(ms * 1000);
    return true;
}

/*============================================================================*
 *                         Message queue (per-instance, bounded ring buffer)
 *
 * Each gui_mq_create() call heap-allocates an independent port_mq_t with its
 * own mutex and two condvars (not_empty / not_full) and a fixed-size ring
 * buffer.  This allows any number of concurrent MQ instances -- unlike the
 * previous single-static-Queue implementation.
 *============================================================================*/

typedef struct port_mq
{
    uint8_t        *buf;       /* msg_size * max_msgs bytes, ring storage   */
    uint32_t        msg_size;
    uint32_t        max_msgs;
    uint32_t        head;      /* next slot to dequeue                      */
    uint32_t        tail;      /* next slot to enqueue                      */
    uint32_t        count;     /* pending messages                          */
    pthread_mutex_t lock;
    pthread_cond_t  not_empty;
    pthread_cond_t  not_full;
} port_mq_t;

/* Millisecond-granular timed wait on @p cond.
 * Returns 0 on signal/spurious-wake, non-zero on timeout.
 * timeout_ms == 0xFFFFFFFF means wait forever. */
static int port_mq_timedwait(pthread_cond_t *cond, pthread_mutex_t *lock,
                             uint32_t timeout_ms)
{
    if (timeout_ms == 0xFFFFFFFFu)
    {
        return pthread_cond_wait(cond, lock);
    }

    struct timeval now;
#ifdef _WIN32
    mingw_gettimeofday(&now, NULL);
#else
    gettimeofday(&now, NULL);
#endif
    uint64_t deadline_us = (uint64_t)now.tv_sec * 1000000ULL
                           + (uint64_t)now.tv_usec
                           + (uint64_t)timeout_ms * 1000ULL;
    struct timespec ts;
    ts.tv_sec  = (time_t)(deadline_us / 1000000ULL);
    ts.tv_nsec = (long)((deadline_us % 1000000ULL) * 1000ULL);
    return pthread_cond_timedwait(cond, lock, &ts);
}

static bool port_mq_create(void *handle, const char *name, uint32_t msg_size,
                           uint32_t max_msgs)
{
    (void)name;

    port_mq_t *mq = (port_mq_t *)calloc(1, sizeof(*mq));
    if (mq == NULL)
    {
        return false;
    }

    mq->buf = (uint8_t *)malloc((size_t)msg_size * max_msgs);
    if (mq->buf == NULL)
    {
        free(mq);
        return false;
    }

    mq->msg_size = msg_size;
    mq->max_msgs = max_msgs;

    if (pthread_mutex_init(&mq->lock, NULL) != 0)
    {
        free(mq->buf);
        free(mq);
        return false;
    }
    if (pthread_cond_init(&mq->not_empty, NULL) != 0)
    {
        pthread_mutex_destroy(&mq->lock);
        free(mq->buf);
        free(mq);
        return false;
    }
    if (pthread_cond_init(&mq->not_full, NULL) != 0)
    {
        pthread_cond_destroy(&mq->not_empty);
        pthread_mutex_destroy(&mq->lock);
        free(mq->buf);
        free(mq);
        return false;
    }

    *(void **)handle = mq;
    return true;
}

static bool port_mq_send(void *handle, void *buffer, uint32_t size, uint32_t timeout)
{
    (void)size;
    port_mq_t *mq = (port_mq_t *)handle;

    pthread_mutex_lock(&mq->lock);
    while (mq->count == mq->max_msgs)
    {
        if (timeout == 0 ||
            port_mq_timedwait(&mq->not_full, &mq->lock, timeout) != 0)
        {
            pthread_mutex_unlock(&mq->lock);
            return false;
        }
    }

    memcpy(mq->buf + (size_t)mq->tail * mq->msg_size, buffer, mq->msg_size);
    mq->tail = (mq->tail + 1) % mq->max_msgs;
    mq->count++;
    pthread_cond_signal(&mq->not_empty);
    pthread_mutex_unlock(&mq->lock);
    return true;
}

static bool port_mq_recv(void *handle, void *buffer, uint32_t size, uint32_t timeout)
{
    (void)size;
    port_mq_t *mq = (port_mq_t *)handle;

    pthread_mutex_lock(&mq->lock);
    while (mq->count == 0)
    {
        if (timeout == 0 ||
            port_mq_timedwait(&mq->not_empty, &mq->lock, timeout) != 0)
        {
            pthread_mutex_unlock(&mq->lock);
            return false;
        }
    }

    memcpy(buffer, mq->buf + (size_t)mq->head * mq->msg_size, mq->msg_size);
    mq->head = (mq->head + 1) % mq->max_msgs;
    mq->count--;
    pthread_cond_signal(&mq->not_full);
    pthread_mutex_unlock(&mq->lock);
    return true;
}

/*============================================================================*
 *                            Timing
 *============================================================================*/

static uint32_t port_thread_ms_get(void)
{
    return gui_tick;
}

static uint32_t port_thread_us_get(void)
{
    struct timeval now;
#ifdef _WIN32
    mingw_gettimeofday(&now, NULL);
#else
    gettimeofday(&now, NULL);
#endif
    return (uint32_t)((uint64_t)now.tv_sec * 1000000ULL + (uint64_t)now.tv_usec);
}

/*============================================================================*
 *                            Memory
 *============================================================================*/

static void *port_malloc(uint32_t n)
{
    return malloc(n);
}

static void *port_realloc(void *ptr, uint32_t n)
{
    return realloc(ptr, n);
}

static void port_free(void *rmem)
{
    free(rmem);
}

/*============================================================================*
 *                            Logging
 *============================================================================*/

int (*write_to_file)(const void *buf, size_t len) = NULL;

static void port_log(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    char buf[2048];
    vsnprintf(buf, sizeof(buf), format, args);
    printf("[GUI MODULE]%s", buf);
    if (write_to_file != NULL)
    {
        write_to_file(buf, strlen(buf));
    }
    va_end(args);
}

/*============================================================================*
 *                     Memory heaps
 *============================================================================*/

#define PORT_GUI_MEMHEAP_SIZE        (1024 * 1024 * 10)
static uint8_t gui_memheap[PORT_GUI_MEMHEAP_SIZE] = {0};

#define PORT_GUI_LOWER_MEMHEAP_SIZE  (1024 * 1024 * 10)
static uint8_t gui_lower_memheap[PORT_GUI_LOWER_MEMHEAP_SIZE] = {0};

/*============================================================================*
 *                         OS API registration struct
 *============================================================================*/

static struct gui_os_api os_api =
{
    .name             = "posix_pthread",
    .thread_create    = port_thread_create,
    .thread_delete    = port_thread_delete,
    .thread_mdelay    = port_thread_mdelay,
    .thread_ms_get    = port_thread_ms_get,
    .thread_us_get    = port_thread_us_get,
    .mq_create        = port_mq_create,
    .mq_send          = port_mq_send,
    .mq_recv          = port_mq_recv,
    .f_malloc         = port_malloc,
    .f_realloc        = port_realloc,
    .f_free           = port_free,

    .mem_addr         = gui_memheap,
    .mem_size         = PORT_GUI_MEMHEAP_SIZE,

    .lower_mem_addr   = gui_lower_memheap,
    .lower_mem_size   = PORT_GUI_LOWER_MEMHEAP_SIZE,
    .mem_threshold_size = 10 * 1024,

    .log              = (void *)port_log,

};

/*============================================================================*
 *                         Internal timer threads
 *============================================================================*/

static void *rtk_gui_timer(void *arg)
{
    (void)arg;
    while (true)
    {
        usleep(10 * 1000); /* 10 ms */
        if (os_api.gui_tick_hook != NULL)
        {
            os_api.gui_tick_hook();
        }
    }
}

#include <stdio.h>
#include <sys/time.h>
static void *rtk_gui_tick(void *arg)
{
    (void)arg;
    struct timeval currentTime;

    gettimeofday(&currentTime, NULL);

    int64_t start_milliseconds = ((int64_t)currentTime.tv_sec * 1000) +
                                 (currentTime.tv_usec / 1000);
    while (1)
    {
        struct timeval cur;
        gettimeofday(&cur, NULL);
        gui_tick = (uint32_t)(((int64_t)cur.tv_sec * 1000) +
                              (cur.tv_usec / 1000) - start_milliseconds);
    }
}

/*============================================================================*
 *                              Port init
 *============================================================================*/

void gui_port_os_init(void)
{
    pthread_t thread;
    pthread_create(&thread, NULL, rtk_gui_timer, NULL);
    {
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, rtk_gui_tick, NULL) != 0)
        {
            fprintf(stderr, "Error creating thread\n");
            return;
        }
    }

    gui_os_api_register(&os_api);

#if defined(__has_include)
#if __has_include("shell_port.h")
    extern int gui_port_console_init(void);
    gui_port_console_init();
#endif
#endif
}
