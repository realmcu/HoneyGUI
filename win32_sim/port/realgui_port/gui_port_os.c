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
#include "gui_queue.h"
#include <unistd.h>

static volatile uint32_t gui_tick;

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
    //pthread_detach(*thread);
    //pthread_kill(*thread, 3);
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

static pthread_mutex_t port_mutex;
static pthread_mutex_t queue_mutex;
static pthread_cond_t port_cond;
static Queue q;


static bool port_mq_create(void *handle, const char *name, uint32_t msg_size, uint32_t max_msgs)
{
    (void)name;
    (void)msg_size;
    (void)max_msgs;
    void **queue = handle;

    if (pthread_mutex_init(&port_mutex, NULL) != 0)
    {
        return false;
    }
    if (pthread_cond_init(&port_cond, NULL) != 0)
    {
        pthread_mutex_destroy(&port_mutex);
        return false;
    }
    if (pthread_mutex_init(&queue_mutex, NULL) != 0)
    {
        pthread_cond_destroy(&port_cond);
        pthread_mutex_destroy(&port_mutex);
        return false;
    }

    QueueInit(&q);
    *queue = &q;

    return true;
}

static bool port_mq_send(void *handle, void *buffer, uint32_t size, uint32_t timeout)
{
    (void)timeout;
    (void)size;
    QuDataType data = {0};
    GUI_ASSERT(handle != NULL);

    memcpy(&data, buffer, size);
    pthread_mutex_lock(&queue_mutex);
    QueuePush(&q, data);
    pthread_mutex_unlock(&queue_mutex);

    pthread_mutex_lock(&port_mutex);
    pthread_cond_signal(&port_cond);
    pthread_mutex_unlock(&port_mutex);
    if (handle != NULL)
    {
        return true;
    }
    else
    {
        return false;
    }

}

static bool port_mq_recv(void *handle, void *buffer, uint32_t size, uint32_t timeout)
{
    (void)handle;
    if (QueueEmpty(&q) == true)
    {
        pthread_mutex_lock(&port_mutex);

        struct timeval now;
        struct timespec outtime;
#ifdef _WIN32
        mingw_gettimeofday(&now, NULL);
#else
        gettimeofday(&now, NULL);
#endif
        outtime.tv_sec = now.tv_sec + timeout;
        outtime.tv_nsec = now.tv_usec * 1000;

        pthread_cond_timedwait(&port_cond, &port_mutex, &outtime);
        pthread_mutex_unlock(&port_mutex);
        return false;
    }
    else
    {
        QuDataType data;

        pthread_mutex_lock(&queue_mutex);
        data = QueueFront(&q);
        QueuePop(&q);
        pthread_mutex_unlock(&queue_mutex);

        memcpy(buffer, &data, size);

        return true;
    }

}


static uint32_t port_thread_ms_get(void)
{
    return gui_tick;
}


static void *port_malloc(uint32_t n)
{
    return malloc(n);
}


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

static void *port_realloc(void *ptr, uint32_t n)
{
    return realloc(ptr, n);
}

static void port_free(void *rmem)
{
    free(rmem);
}


#define PORT_GUI_MEMHEAP_SIZE 1024*1024*10
static uint8_t gui_memheap[PORT_GUI_MEMHEAP_SIZE] = {0};

#define PORT_GUI_LOWER_MEMHEAP_SIZE 1024*1024*10
static uint8_t gui_lower_memheap[PORT_GUI_LOWER_MEMHEAP_SIZE] = {0};


static struct gui_os_api os_api =
{
    .name = "posix_pthread",
    .thread_create = port_thread_create,
    .thread_delete = port_thread_delete,
    .thread_mdelay = port_thread_mdelay,
    .thread_ms_get = port_thread_ms_get,
    .mq_create = port_mq_create,
    .mq_send = port_mq_send,
    .mq_recv = port_mq_recv,
    .f_malloc = port_malloc,
    .f_realloc = port_realloc,
    .f_free = port_free,

    .mem_addr = gui_memheap,
    .mem_size = PORT_GUI_MEMHEAP_SIZE,

    .lower_mem_addr = gui_lower_memheap,
    .lower_mem_size = PORT_GUI_LOWER_MEMHEAP_SIZE,
    .mem_threshold_size = 10 * 1024,

    .log = (void *)port_log,
};


static void *rtk_gui_timer(void *arg)
{
    (void)arg;
    while (true)
    {
        usleep(10 * 1000); //10ms
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

    int64_t start_milliseconds = ((int64_t)currentTime.tv_sec * 1000) + (currentTime.tv_usec / 1000);
    while (1)
    {
        struct timeval currentTime;

        gettimeofday(&currentTime, NULL);
        gui_tick = ((int64_t)currentTime.tv_sec * 1000) + (currentTime.tv_usec / 1000) - start_milliseconds;
    }
}

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

}

