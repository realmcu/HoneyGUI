#include "guidef.h"
#include "gui_port.h"
#include "gui_api.h"
#include "stdio.h"
#include "stdlib.h"
#include <pthread.h>

void *port_thread_create(const char *name, void (*entry)(void *param), void *param,
                         uint32_t stack_size, uint8_t priority)
{
    pthread_t *thread = malloc(sizeof(pthread_t));
    pthread_create(thread, NULL, entry, param);
    pthread_setname_np(*thread, name);
    return thread;
}
bool port_thread_delete(void *handle)
{
    pthread_t *thread = handle;
    void *ret = NULL;
    //pthread_detach(*thread);
    //pthread_kill(*thread, 3);
    pthread_cancel(*thread);
    pthread_join(*thread, &ret);
    return true;
}
bool port_thread_mdelay(uint32_t ms)
{
    usleep(ms * 1000);
    return true;
}

static uint32_t gui_tick = 0;


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

#if ENABLE_RTK_GUI_OS_HEAP == 0
#define PORT_GUI_MEMHEAP_SIZE 1024*1204*10
static uint8_t gui_memheap[PORT_GUI_MEMHEAP_SIZE] = {0};
#endif

static struct gui_os_api os_api =
{
    .name = "win_gcc",
    .thread_create = port_thread_create,
    .thread_delete = port_thread_delete,
    .thread_mdelay = port_thread_mdelay,
    .thread_ms_get = port_thread_ms_get,
    .f_malloc = port_malloc,
    .f_realloc = port_realloc,
    .f_free = port_free,
#if ENABLE_RTK_GUI_OS_HEAP == 0
    .mem_size = PORT_GUI_MEMHEAP_SIZE,
    .mem_addr = gui_memheap,
#endif
    .log = printf,
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

