
#include "guidef.h"
#include "gui_port.h"
#include <pthread.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>

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


#endif
/* directory api port*/
static int port_closedir(gui_fs_dir *d)
{

    int r = closedir((DIR *)(d->dir));
    if (d->dirent)
    {
        if (d->dirent->d_name)
        {
            free(d->dirent->d_name);
            d->dirent->d_name = 0;
        }

        free(d->dirent);
        d->dirent = 0;
    }
    free(d);
    return r;
}

static gui_fs_dir *port_opendir(const char *name)
{

    DIR *dir = opendir(name);
    if (!dir)
    {
        return 0;
    }

    gui_fs_dir *fs_dir = malloc(sizeof(gui_fs_dir));
    memset(fs_dir, 0, sizeof(gui_fs_dir));
    fs_dir->dir = dir;
    return fs_dir;
}

static struct gui_fs_dirent *port_readdir(gui_fs_dir *d)
{
    struct dirent *dirent = readdir((DIR *)(d->dir));
    if (!dirent)
    {
        return 0;
    }

    struct gui_fs_dirent *fs_dirent = malloc(sizeof(struct gui_fs_dirent));
    memset(fs_dirent, 0, sizeof(struct gui_fs_dirent));
    if (d->dirent)
    {
        if (d->dirent->d_name)
        {
            free(d->dirent->d_name);
            d->dirent->d_name = 0;
        }
        free(d->dirent);
        d->dirent = 0;
    }
    d->dirent = fs_dirent;
    int d_name_length = strlen(dirent->d_name) + 1;
    fs_dirent->d_name = malloc(d_name_length);
    memcpy(fs_dirent->d_name, dirent->d_name,  d_name_length);
    fs_dirent->d_namlen = dirent->d_namlen;
    fs_dirent->d_reclen = dirent->d_reclen;
    fs_dirent->d_type = dirent->d_ino;
    fs_dirent->dirent = dirent;
    return fs_dirent;
}

void port_fstat(int fildes, gui_fs_stat_t *buf)
{
    struct stat st;
    int fd = fildes;
    fstat(fd, &st);
    buf->st_size = st.st_size;
}
static int port_open(const char *file, int flags, ...)
{
    return open(file, flags | O_BINARY);
}

static struct gui_fs fs_api =
{
    /* file api port*/
    .open      = port_open,
    .close     = close,
    .read      = (int (*)(int, void *, size_t))read,
    .write     = (int (*)(int, const void *, size_t))write,
    .lseek     = (int (*)(int, int, int))lseek,
    /* directory api port*/
    .opendir   = (gui_fs_dir * (*)(const char *name))port_opendir,
    .closedir  = (int (*)(gui_fs_dir * d))port_closedir,
    .readdir   = (struct gui_fs_dirent * (*)(gui_fs_dir * d))port_readdir,
    .fstat     = port_fstat,

};

extern void gui_fs_info_register(struct gui_fs *info);
void gui_port_fs_init(void)
{
    gui_fs_info_register(&fs_api);
}
