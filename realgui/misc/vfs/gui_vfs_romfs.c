/**
 * @file gui_vfs_romfs.c
 * @brief ROMFS backend for VFS (XIP support)
 *
 * Uses existing hg_romfs implementation
 */

#include "gui_vfs.h"
#include "gui_api_os.h"
#include "romfs/hg_romfs.h"
#include <string.h>
#include <stdlib.h>

#ifndef F_GETADDR
#define F_GETADDR 100  /* Custom fcntl command to get file address */
#endif

/* ROMFS backend data */
typedef struct
{
    const void *base_addr;
    uint32_t base_offset;
} romfs_backend_t;

/* Global backend (simplified for single ROMFS instance) */
static romfs_backend_t *g_romfs_backend = NULL;

/* ROMFS VFS operations */

static void *romfs_vfs_open(const char *path, gui_vfs_mode_t mode, void *user_data)
{
    (void)user_data;

    /* ROMFS is read-only */
    if (mode != GUI_VFS_READ) { return NULL; }

    /* Add leading slash if missing */
    char *full_path = NULL;
    if (path[0] != '/')
    {
        full_path = (char *)gui_malloc(strlen(path) + 2);
        if (!full_path) { return NULL; }
        full_path[0] = '/';
        strcpy(full_path + 1, path);
        path = full_path;
    }

    /* Use hg_open from hg_romfs */
    intptr_t fd = hg_open(path, 0);

    if (full_path) { gui_free(full_path); }

    if (fd == -1) { return NULL; }

    return (void *)fd;
}

static int romfs_vfs_close(void *file)
{
    return hg_close((intptr_t)file);
}

static int romfs_vfs_read(void *file, void *buf, size_t size)
{
    return hg_read((intptr_t)file, buf, size);
}

static int romfs_vfs_write(void *file, const void *buf, size_t size)
{
    (void)file; (void)buf; (void)size;
    return -1;  /* Read-only */
}

static int romfs_vfs_seek(void *file, int offset, gui_vfs_seek_t whence)
{
    return hg_lseek((intptr_t)file, offset, whence);
}

static int romfs_vfs_tell(void *file)
{
    return hg_lseek((intptr_t)file, 0, GUI_VFS_SEEK_CUR);
}

static const void *romfs_vfs_get_addr(void *file, size_t *size)
{
    /* Use hg_ioctl to get direct address (XIP) */
    void *addr = (void *)(intptr_t)hg_ioctl((intptr_t)file, F_GETADDR, 0);
    if (addr && size)
    {
        /* Get file size */
        int pos = hg_lseek((intptr_t)file, 0, GUI_VFS_SEEK_CUR);
        *size = hg_lseek((intptr_t)file, 0, GUI_VFS_SEEK_END);
        hg_lseek((intptr_t)file, pos, GUI_VFS_SEEK_SET);
    }
    return addr;
}

static void *romfs_vfs_opendir(const char *path, void *user_data)
{
    (void)user_data;

    /* Add leading slash if missing or empty */
    char *full_path = NULL;
    if (!path || path[0] == '\0' || path[0] != '/')
    {
        size_t len = path ? strlen(path) : 0;
        full_path = (char *)gui_malloc(len + 2);
        if (!full_path) { return NULL; }
        full_path[0] = '/';
        if (len > 0)
        {
            strcpy(full_path + 1, path);
        }
        else
        {
            full_path[1] = '\0';
        }
        path = full_path;
    }

    void *dir = hg_opendir(path);

    if (full_path) { gui_free(full_path); }

    return dir;
}

static int romfs_vfs_readdir(void *dir, gui_vfs_stat_t *stat)
{
    struct hg_dirent *entry = hg_readdir((HG_DIR *)dir);
    if (!entry) { return -1; }

    strncpy(stat->name, entry->d_name, sizeof(stat->name) - 1);
    stat->name[sizeof(stat->name) - 1] = '\0';
    stat->type = (entry->d_type == DT_DIR) ? GUI_VFS_TYPE_DIR : GUI_VFS_TYPE_FILE;
    stat->size = entry->d_size;

    return 0;
}

static int romfs_vfs_closedir(void *dir)
{
    return hg_closedir((HG_DIR *)dir);
}

static int romfs_vfs_stat(const char *path, gui_vfs_stat_t *stat, void *user_data)
{
    (void)user_data;

    intptr_t fd = hg_open(path[0] == '/' ? path : path, 0);
    if (fd == -1) { return -1; }

    struct hg_stat hg_st;
    int ret = hg_fstat(fd, &hg_st);
    hg_close(fd);

    if (ret == 0)
    {
        stat->size = hg_st.st_size;
        stat->type = GUI_VFS_TYPE_FILE;
    }
    return ret;
}

/* ROMFS VFS operations table */
static const gui_vfs_ops_t romfs_vfs_ops =
{
    .open = romfs_vfs_open,
    .close = romfs_vfs_close,
    .read = romfs_vfs_read,
    .write = romfs_vfs_write,
    .seek = romfs_vfs_seek,
    .tell = romfs_vfs_tell,
    .get_addr = romfs_vfs_get_addr,  /* XIP support */
    .opendir = romfs_vfs_opendir,
    .readdir = romfs_vfs_readdir,
    .closedir = romfs_vfs_closedir,
    .stat = romfs_vfs_stat,
};

/**
 * @brief Mount ROMFS
 * @param prefix Mount point (e.g., "/rom")
 * @param base_addr ROMFS base address
 * @param base_offset Base offset for address calculation
 * @return 0 on success, -1 on error
 */
int gui_vfs_mount_romfs(const char *prefix, const void *base_addr, uint32_t base_offset)
{
    if (g_romfs_backend)
    {
        gui_free(g_romfs_backend);
    }

    g_romfs_backend = (romfs_backend_t *)gui_malloc(sizeof(romfs_backend_t));
    if (!g_romfs_backend)
    {
        gui_log("[VFS] ROMFS mount failed: out of memory\n");
        return -1;
    }

    g_romfs_backend->base_addr = base_addr;
    g_romfs_backend->base_offset = base_offset;

    /* Mount ROMFS using hg_romfs */
    hg_romfs_mount((void *)base_addr);

    int ret = gui_vfs_mount(prefix, &romfs_vfs_ops, g_romfs_backend);
    if (ret == 0)
    {
        gui_log("[VFS] ROMFS mounted at %s (addr: %p, offset: 0x%08x)\n",
                prefix, base_addr, base_offset);
    }
    return ret;
}
