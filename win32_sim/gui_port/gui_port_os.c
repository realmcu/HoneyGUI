#include "guidef.h"
#include "gui_port.h"
#include "gui_api.h"
#include "stdio.h"
#include "stdlib.h"
#include <pthread.h>
#include "gui_queue.h"
#include <unistd.h>

static uint32_t gui_tick = 0;

static void *port_thread_create(const char *name, void (*entry)(void *param), void *param,
                                uint32_t stack_size, uint8_t priority)
{
    pthread_t *thread = malloc(sizeof(pthread_t));
    pthread_create(thread, NULL, (void *(*)(void *))entry, param);
    pthread_setname_np(*thread, name);
    return thread;
}

static bool port_thread_delete(void *handle)
{
    pthread_t *thread = handle;
    void *ret = NULL;
    //pthread_detach(*thread);
    //pthread_kill(*thread, 3);
    pthread_cancel(*thread);
    pthread_join(*thread, &ret);
    return true;
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
    pthread_mutex_init(&port_mutex, NULL);
    pthread_cond_init(&port_cond, NULL);

    pthread_mutex_init(&queue_mutex, NULL);
    QueueInit(&q);
}
static bool port_mq_send(void *handle, void *buffer, uint32_t size, uint32_t timeout)
{
    pthread_mutex_lock(&queue_mutex);
    QueuePush(&q, 1);
    pthread_mutex_unlock(&queue_mutex);

    pthread_mutex_lock(&port_mutex);
    pthread_cond_signal(&port_cond);
    pthread_mutex_unlock(&port_mutex);

}

static bool port_mq_recv(void *handle, void *buffer, uint32_t size, uint32_t timeout)
{
    if (QueueEmpty(&q) == true)
    {
        pthread_mutex_lock(&port_mutex);
        struct timespec timeout_time;
        timeout_time.tv_sec = timeout;
        pthread_cond_timedwait(&port_cond, &port_mutex, &timeout_time);
        pthread_mutex_unlock(&port_mutex);
    }
    pthread_mutex_lock(&queue_mutex);
    QueuePop(&q);
    pthread_mutex_unlock(&queue_mutex);
}


static uint32_t port_thread_ms_get(void)
{
    return 10 * gui_tick;
}


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


#define PORT_GUI_MEMHEAP_SIZE 1024*1024*10
static uint8_t gui_memheap[PORT_GUI_MEMHEAP_SIZE] = {0};
static uint8_t gui_lower_memheap[PORT_GUI_MEMHEAP_SIZE] = {0};


static struct gui_os_api os_api =
{
    .name = "win_gcc",
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
    .lower_mem_size = PORT_GUI_MEMHEAP_SIZE,
    .mem_threshold_size = 10 * 1024,

    .log = (void *)printf,
};


static void *rtk_gui_timer(void *arg)
{
    while (true)
    {
        usleep(10 * 1000); //10ms
        gui_tick++;
        if (os_api.gui_tick_hook != NULL)
        {
            os_api.gui_tick_hook();
        }
    }
}
void gui_port_os_init(void)
{
    pthread_t thread;
    pthread_create(&thread, NULL, rtk_gui_timer, NULL);
    gui_os_api_register(&os_api);

}

