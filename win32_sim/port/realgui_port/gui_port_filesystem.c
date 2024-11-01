
#include "guidef.h"
#include "gui_port.h"
#include <pthread.h>
#include <stdio.h>
#include <dirent.h>

#if 0
/* file api port*/
static int port_open(const char *file, int flags, ...)
{

}

static int port_close(int d)
{

}

static int port_read(int fd, void *buf, size_t len)
{

}

static int port_write(int fd, const void *buf, size_t len)
{

}

static off_t port_lseek(int fd, off_t offset, int whence)
{

}
static int port_rename(const char *from, const char *to)
{

}
static int port_unlink(const char *pathname)
{

}
static int port_stat(const char *file, struct stat *buf)
{

}
static int port_fstat(int fildes, struct stat *buf)
{

}
static int port_fsync(int fildes)
{

}
static int port_fcntl(int fildes, int cmd, ...)
{

}
static int port_ioctl(int fildes, int cmd, ...)
{

}
static int port_ftruncate(int fd, off_t length)
{

}

/* directory api port*/
static int port_closedir(gui_fs_dir *d)
{
    return closedir((DIR *)d);
}

static gui_fs_dir *port_opendir(const char *name)
{
    return (gui_fs_dir *)opendir(name);
}

static struct gui_fs_dirent *port_readdir(gui_fs_dir *d)
{
    return (struct gui_fs_dirent *)readdir((DIR *)d);
}
#endif

static struct gui_fs fs_api =
{
    /* file api port*/
    .open      = open,
    .close     = close,
    .read      = (int (*)(int, void *, size_t))read,
    .write     = (int (*)(int, const void *, size_t))write,
    .lseek     = (int (*)(int, int, int))lseek,
    /* directory api port*/
    .opendir   = (gui_fs_dir * (*)(const char *name))opendir,
    .closedir  = (int (*)(gui_fs_dir * d))closedir,
    .readdir   = (struct gui_fs_dirent * (*)(gui_fs_dir * d))readdir,

};

extern void gui_fs_info_register(struct gui_fs *info);
void gui_port_fs_init(void)
{
    gui_fs_info_register(&fs_api);
}
