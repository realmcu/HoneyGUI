#include "guidef.h"
#include "gui_port.h"
#include "gui_api.h"
#include "stdio.h"
#include "stdlib.h"
#include "cmsis_os2.h"                  // ARM::CMSIS:RTOS2:Keil RTX5

void *port_thread_create(const char *name, void (*entry)(void *param), void *param,
                         uint32_t stack_size, uint8_t priority)
{
    osThreadNew(entry, NULL, NULL);    // Create application main thread
    return NULL;
}
bool port_thread_delete(void *handle)
{

    return true;
}
bool port_thread_mdelay(uint32_t ms)
{
    osDelay(ms);
    return true;
}

uint32_t port_thread_ms_get(void)
{
    return 0;
}

void *port_malloc(uint32_t n)
{
    return malloc(n);
}

void *port_realloc(void *ptr, uint32_t n)
{
    return realloc(ptr, n);
}


void port_free(void *rmem)
{
    free(rmem);
}

#define PORT_MEMHEAP_SIZE       (1024*1024)
static uint8_t port_memheap[PORT_MEMHEAP_SIZE] = {0};
static uint8_t port_lower_memheap[PORT_MEMHEAP_SIZE] = {0};
static struct gui_os_api os_api =
{
    .name = "win_keil",
    .thread_create = port_thread_create,
    .thread_delete = port_thread_delete,
    .thread_mdelay = port_thread_mdelay,
    .thread_ms_get = port_thread_ms_get,
    .f_malloc = port_malloc,
    .f_realloc = port_realloc,
    .f_free = port_free,
    .mem_addr = port_memheap,
    .mem_size = PORT_MEMHEAP_SIZE,

    .lower_mem_addr = port_lower_memheap,
    .lower_mem_size = PORT_MEMHEAP_SIZE,
    .mem_threshold_size = 10 * 1024,

    .log = (log_func_t)printf,
};
void gui_port_os_init(void)
{

    gui_os_api_register(&os_api);

}

