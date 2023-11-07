
#include "guidef.h"
#include "gui_port.h"
#include <pthread.h>
#include <stdio.h>
#include <dirent.h>

int port_closedir(gui_fs_DIR *d)
{
    return closedir((DIR *)d);
}
struct gui_fs_dirent *port_readdir(gui_fs_DIR *d)
{
    return (struct gui_fs_dirent *)readdir((DIR *)d);
}
gui_fs_DIR *gui_port_opendir(const char *name)
{
    return (gui_fs_DIR *)opendir(name);
}
int gui_port_lseek(int fd, int offset, int whence)
{
    return (int)lseek(fd, (off_t)offset, whence);
}
static struct gui_fs fs_api =
{
    .closedir  = port_closedir,
    .close     = close,
    .lseek     = gui_port_lseek,
    .open      = open,
    .opendir   = gui_port_opendir,
    .read      = read,
    .readdir   = port_readdir,
    .write     = write,
};
extern void gui_fs_info_register(struct gui_fs *info);
void gui_port_fs_init(void)
{
    gui_fs_info_register(&fs_api);
}
