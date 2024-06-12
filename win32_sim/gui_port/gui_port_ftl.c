
#include "guidef.h"
#include "gui_api.h"
#include "gui_port.h"
#include <pthread.h>
#include <stdio.h>
#include <dirent.h>


int port_ftl_read(uint32_t addr, uint8_t *buf, uint32_t len)
{
    memcpy(buf, (void *)(uintptr_t)addr, len);
    return 0;
}

int port_ftl_write(uint32_t addr, const uint8_t *buf, uint32_t len)
{
    memcpy((void *)(uintptr_t)addr, buf, len);
    return 0;
}

int port_ftl_erase(uint32_t addr, uint32_t len)
{
    return 0;
}

static struct gui_ftl ftl_port =
{
    .read      = (int (*)(uint32_t addr, uint8_t *buf, uint32_t len))port_ftl_read,
    .write     = (int (*)(uint32_t addr, const uint8_t *buf, uint32_t len))port_ftl_write,
    .erase     = (int (*)(uint32_t addr, uint32_t len))port_ftl_erase,
};

extern void gui_ftl_info_register(struct gui_ftl *info);
void gui_port_ftl_init(void)
{
    gui_ftl_info_register(&ftl_port);
}
