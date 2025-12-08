/**
 * @file gui_port_filesystem.c
 * @brief Filesystem port for win32 simulator using VFS
 */

#include "guidef.h"
#include "gui_port.h"
#include "gui_vfs.h"
#include "gui_vfs_generic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>

#ifndef O_BINARY
#define O_BINARY 0
#endif

/* POSIX adapter for VFS */

static void *posix_adapter_open(const char *path, int flags)
{
    int posix_flags = O_BINARY;

    if (flags & 0x01) { posix_flags |= O_RDONLY; }
    if (flags & 0x02) { posix_flags |= O_WRONLY; }
    if (flags & 0x04) { posix_flags |= O_CREAT; }
    if (flags & 0x08) { posix_flags |= O_APPEND; }

    int fd = open(path, posix_flags, 0666);
    if (fd < 0) { return NULL; }

    return (void *)(intptr_t)fd;
}

static int posix_adapter_close(void *file)
{
    int fd = (int)(intptr_t)file;
    return close(fd);
}

static int posix_adapter_read(void *file, void *buf, size_t size)
{
    int fd = (int)(intptr_t)file;
    return read(fd, buf, size);
}

static int posix_adapter_write(void *file, const void *buf, size_t size)
{
    int fd = (int)(intptr_t)file;
    return write(fd, buf, size);
}

static int posix_adapter_seek(void *file, int offset, int whence)
{
    int fd = (int)(intptr_t)file;
    return lseek(fd, offset, whence);
}

static int posix_adapter_tell(void *file)
{
    int fd = (int)(intptr_t)file;
    return lseek(fd, 0, SEEK_CUR);
}

/* POSIX adapter structure */
static const gui_fs_adapter_t posix_adapter =
{
    .fs_open = posix_adapter_open,
    .fs_close = posix_adapter_close,
    .fs_read = posix_adapter_read,
    .fs_write = posix_adapter_write,
    .fs_seek = posix_adapter_seek,
    .fs_tell = posix_adapter_tell,
    .fs_opendir = NULL,   /* Not needed for basic file operations */
    .fs_readdir = NULL,
    .fs_closedir = NULL,
    .fs_stat = NULL,
};

/**
 * @brief Initialize filesystem port for win32 simulator
 *
 * This function initializes VFS and mounts:
 * - /pc: POSIX filesystem (host filesystem, virtual mount point)
 * - /vfs_adapter: Generic adapter example (host filesystem)
 */
void gui_port_fs_init(void)
{
    /* Initialize VFS */
    gui_vfs_init();

    /* Mount POSIX filesystem to /pc (virtual mount point) */
    gui_vfs_mount_posix("/pc", ".");

    /* Mount generic adapter to /vfs_adapter (example) */
    gui_vfs_mount_generic("/vfs_adapter", ".", &posix_adapter);
}
