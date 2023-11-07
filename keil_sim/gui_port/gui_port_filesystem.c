
#include "guidef.h"
#include "gui_port.h"
#include <stdio.h>



static struct gui_fs fs_api =
{
    .closedir  = NULL,
    .close     = NULL,
    .lseek     = NULL,
    .open      = NULL,
    .opendir   = NULL,
    .read      = NULL,
    .readdir   = NULL,
    .write     = NULL,
};
extern void gui_fs_info_register(struct gui_fs *info);
void gui_port_fs_init(void)
{
    GUI_UNUSED(fs_api);
    gui_fs_info_register(&fs_api);
}
