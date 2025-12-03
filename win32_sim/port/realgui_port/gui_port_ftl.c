
#include "guidef.h"
#include "gui_api.h"
#include "gui_port.h"
#include <stdio.h>


int port_ftl_read(uintptr_t addr, uint8_t *buf, size_t len)
{
    memcpy(buf, (void *)addr, len);
    return 0;
}

int port_ftl_write(uintptr_t addr, const uint8_t *buf, size_t len)
{
    memcpy((void *)addr, buf, len);
    return 0;
}

int port_ftl_erase(uintptr_t addr, size_t len)
{
    (void)addr;
    (void)len;
    return 0;
}

static struct gui_ftl ftl_port =
{
    .read      = port_ftl_read,
    .write     = port_ftl_write,
    .erase     = port_ftl_erase,
};

extern void gui_ftl_info_register(struct gui_ftl *info);
void gui_port_ftl_init(void)
{
    gui_ftl_info_register(&ftl_port);
}
