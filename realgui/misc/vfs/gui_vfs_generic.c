/**
 * @file gui_vfs_generic.c
 * @brief Generic filesystem adapter for VFS
 *
 * Supports any filesystem by providing a simple adapter interface:
 * - FATFS
 * - LittleFS
 * - SPIFFS
 * - Custom filesystems
 */

#include "gui_vfs.h"
#include "gui_vfs_generic.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "gui_api.h"

/* Generic backend data */
typedef struct
{
    char root_path[64];
    const gui_fs_adapter_t *adapter;
} generic_backend_t;

/* Global backend (simplified) */
static generic_backend_t *g_generic_backend = NULL;

/* Static buffer for path building (thread-unsafe, but simple) */
static char g_path_buffer[256];

/* Helper: build full path */
static void build_path(const generic_backend_t *backend, const char *rel_path,
                       char *full_path, size_t size)
{
    if (backend->root_path[0])
    {
        snprintf(full_path, size, "%s/%s", backend->root_path, rel_path);
    }
    else
    {
        snprintf(full_path, size, "%s", rel_path);
    }
}

/* VFS operations using generic adapter */

static void *generic_open(const char *path, gui_vfs_mode_t mode, void *user_data)
{
    generic_backend_t *backend = (generic_backend_t *)user_data;
    if (!backend || !backend->adapter->fs_open) { return NULL; }

    build_path(backend, path, g_path_buffer, sizeof(g_path_buffer));

    /* Convert VFS mode to generic flags */
    int flags = 0;
    if (mode & GUI_VFS_READ) { flags |= 0x01; }
    if (mode & GUI_VFS_WRITE) { flags |= 0x02; }
    if (mode & GUI_VFS_CREATE) { flags |= 0x04; }
    if (mode & GUI_VFS_APPEND) { flags |= 0x08; }

    return backend->adapter->fs_open(g_path_buffer, flags);
}

static int generic_close(void *file)
{
    if (!g_generic_backend || !g_generic_backend->adapter->fs_close) { return -1; }
    return g_generic_backend->adapter->fs_close(file);
}

static int generic_read(void *file, void *buf, size_t size)
{
    if (!g_generic_backend || !g_generic_backend->adapter->fs_read) { return -1; }
    return g_generic_backend->adapter->fs_read(file, buf, size);
}

static int generic_write(void *file, const void *buf, size_t size)
{
    if (!g_generic_backend || !g_generic_backend->adapter->fs_write) { return -1; }
    return g_generic_backend->adapter->fs_write(file, buf, size);
}

static int generic_seek(void *file, int offset, gui_vfs_seek_t whence)
{
    if (!g_generic_backend || !g_generic_backend->adapter->fs_seek) { return -1; }
    return g_generic_backend->adapter->fs_seek(file, offset, whence);
}

static int generic_tell(void *file)
{
    if (!g_generic_backend || !g_generic_backend->adapter->fs_tell) { return -1; }
    return g_generic_backend->adapter->fs_tell(file);
}

static void *generic_opendir(const char *path, void *user_data)
{
    generic_backend_t *backend = (generic_backend_t *)user_data;
    if (!backend || !backend->adapter->fs_opendir) { return NULL; }

    build_path(backend, path, g_path_buffer, sizeof(g_path_buffer));

    return backend->adapter->fs_opendir(g_path_buffer);
}

static int generic_readdir(void *dir, gui_vfs_stat_t *stat)
{
    if (!g_generic_backend || !g_generic_backend->adapter->fs_readdir) { return -1; }

    int is_dir;
    size_t size;
    int ret = g_generic_backend->adapter->fs_readdir(dir, stat->name, sizeof(stat->name), &is_dir,
                                                     &size);

    if (ret == 0)
    {
        stat->type = is_dir ? GUI_VFS_TYPE_DIR : GUI_VFS_TYPE_FILE;
        stat->size = size;
    }

    return ret;
}

static int generic_closedir(void *dir)
{
    if (!g_generic_backend || !g_generic_backend->adapter->fs_closedir) { return -1; }
    return g_generic_backend->adapter->fs_closedir(dir);
}

static int generic_stat(const char *path, gui_vfs_stat_t *stat, void *user_data)
{
    generic_backend_t *backend = (generic_backend_t *)user_data;
    if (!backend || !backend->adapter->fs_stat) { return -1; }

    build_path(backend, path, g_path_buffer, sizeof(g_path_buffer));

    int is_dir;
    size_t size;
    int ret = backend->adapter->fs_stat(g_path_buffer, &is_dir, &size);

    if (ret == 0)
    {
        const char *name = strrchr(path, '/');
        name = name ? name + 1 : path;
        strncpy(stat->name, name, sizeof(stat->name) - 1);
        stat->name[sizeof(stat->name) - 1] = '\0';
        stat->type = is_dir ? GUI_VFS_TYPE_DIR : GUI_VFS_TYPE_FILE;
        stat->size = size;
    }

    return ret;
}

/* Generic VFS operations table */
static const gui_vfs_ops_t generic_ops =
{
    .open = generic_open,
    .close = generic_close,
    .read = generic_read,
    .write = generic_write,
    .seek = generic_seek,
    .tell = generic_tell,
    .get_addr = NULL,  /* No XIP support */
    .opendir = generic_opendir,
    .readdir = generic_readdir,
    .closedir = generic_closedir,
    .stat = generic_stat,
};

/**
 * @brief Mount generic filesystem
 * @param prefix Mount point (e.g., "/sd", "/flash")
 * @param root_path Root path in the filesystem (can be empty)
 * @param adapter Filesystem adapter operations
 * @return 0 on success, -1 on error
 */
int gui_vfs_mount_generic(const char *prefix, const char *root_path,
                          const gui_fs_adapter_t *adapter)
{
    if (!adapter)
    {
        gui_log("[VFS] Generic mount failed: invalid adapter\n");
        return -1;
    }

    if (g_generic_backend)
    {
        free(g_generic_backend);
    }

    g_generic_backend = (generic_backend_t *)malloc(sizeof(generic_backend_t));
    if (!g_generic_backend)
    {
        gui_log("[VFS] Generic mount failed: out of memory\n");
        return -1;
    }

    if (root_path)
    {
        strncpy(g_generic_backend->root_path, root_path, sizeof(g_generic_backend->root_path) - 1);
        g_generic_backend->root_path[sizeof(g_generic_backend->root_path) - 1] = '\0';
    }
    else
    {
        g_generic_backend->root_path[0] = '\0';
    }

    g_generic_backend->adapter = adapter;

    int ret = gui_vfs_mount(prefix, &generic_ops, g_generic_backend);
    if (ret == 0)
    {
        gui_log("[VFS] Generic FS mounted at %s (path: %s)\n",
                prefix, root_path ? root_path : "");
    }
    return ret;
}

/* ========== Example Adapters ========== */

#ifdef EXAMPLE_ADAPTERS

/* Example 1: FATFS Adapter */
#include "ff.h"

static void *fatfs_adapter_open(const char *path, int flags)
{
    FIL *fp = malloc(sizeof(FIL));
    if (!fp) { return NULL; }

    BYTE mode = 0;
    if (flags & 0x01) { mode |= FA_READ; }
    if (flags & 0x02) { mode |= FA_WRITE; }
    if (flags & 0x04) { mode |= FA_CREATE_ALWAYS; }
    if (flags & 0x08) { mode |= FA_OPEN_APPEND; }

    if (f_open(fp, path, mode) != FR_OK)
    {
        free(fp);
        return NULL;
    }
    return fp;
}

static int fatfs_adapter_close(void *file)
{
    FIL *fp = (FIL *)file;
    f_close(fp);
    free(fp);
    return 0;
}

static int fatfs_adapter_read(void *file, void *buf, size_t size)
{
    FIL *fp = (FIL *)file;
    UINT br;
    if (f_read(fp, buf, size, &br) != FR_OK) { return -1; }
    return br;
}

static const gui_fs_adapter_t fatfs_adapter =
{
    .fs_open = fatfs_adapter_open,
    .fs_close = fatfs_adapter_close,
    .fs_read = fatfs_adapter_read,
    /* ... other operations */
};

void mount_fatfs_example(void)
{
    gui_vfs_mount_generic("/sd", "0:", &fatfs_adapter);
}

/* Example 2: LittleFS Adapter */
#include "lfs.h"

static lfs_t g_lfs;
static void *littlefs_adapter_open(const char *path, int flags)
{
    lfs_file_t *file = malloc(sizeof(lfs_file_t));
    if (!file) { return NULL; }

    int lfs_flags = 0;
    if (flags & 0x01) { lfs_flags |= LFS_O_RDONLY; }
    if (flags & 0x02) { lfs_flags |= LFS_O_WRONLY; }
    if (flags & 0x04) { lfs_flags |= LFS_O_CREAT; }
    if (flags & 0x08) { lfs_flags |= LFS_O_APPEND; }

    if (lfs_file_open(&g_lfs, file, path, lfs_flags) < 0)
    {
        free(file);
        return NULL;
    }
    return file;
}

static const gui_fs_adapter_t littlefs_adapter =
{
    .fs_open = littlefs_adapter_open,
    /* ... other operations */
};

void mount_littlefs_example(void)
{
    gui_vfs_mount_generic("/flash", "", &littlefs_adapter);
}

#endif /* EXAMPLE_ADAPTERS */
