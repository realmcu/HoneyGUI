/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/**
 * @file gui_vfs_posix.c
 * @brief POSIX filesystem backend for VFS
 */

/* Only compile for systems with POSIX support */
#if defined(_WIN32) || defined(__linux__) || defined(__APPLE__) || defined(__unix__)

#include "gui_vfs.h"
#include "gui_api_os.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>


/* POSIX backend data */
typedef struct
{
    char root_path[256];
} posix_backend_t;

/* Helper: build full path */
static void build_path(const posix_backend_t *backend, const char *rel_path,
                       char *full_path, size_t size)
{
    snprintf(full_path, size, "%s/%s", backend->root_path, rel_path);
}

/* POSIX operations */

static void *posix_open(const char *path, gui_vfs_mode_t mode, void *user_data)
{
    posix_backend_t *backend = (posix_backend_t *)user_data;

    char full_path[512];
    build_path(backend, path, full_path, sizeof(full_path));

    const char *mode_str = "rb";
    if (mode & GUI_VFS_WRITE)
    {
        if (mode & GUI_VFS_APPEND) { mode_str = "ab"; }
        else if (mode & GUI_VFS_CREATE) { mode_str = "wb"; }
        else { mode_str = "r+b"; }
    }

    FILE *fp = fopen(full_path, mode_str);
    return (void *)fp;
}

static int posix_close(void *file)
{
    FILE *fp = (FILE *)file;
    return fclose(fp);
}

static int posix_read(void *file, void *buf, size_t size)
{
    FILE *fp = (FILE *)file;
    return fread(buf, 1, size, fp);
}

static int posix_write(void *file, const void *buf, size_t size)
{
    FILE *fp = (FILE *)file;
    return fwrite(buf, 1, size, fp);
}

static int posix_seek(void *file, int offset, gui_vfs_seek_t whence)
{
    FILE *fp = (FILE *)file;
    int origin;

    switch (whence)
    {
    case GUI_VFS_SEEK_SET: origin = SEEK_SET; break;
    case GUI_VFS_SEEK_CUR: origin = SEEK_CUR; break;
    case GUI_VFS_SEEK_END: origin = SEEK_END; break;
    default: return -1;
    }

    if (fseek(fp, offset, origin) != 0) { return -1; }
    return ftell(fp);
}

static int posix_tell(void *file)
{
    FILE *fp = (FILE *)file;
    return ftell(fp);
}


static void *posix_opendir(const char *path, void *user_data)
{
    posix_backend_t *backend = (posix_backend_t *)user_data;

    char full_path[512];
    build_path(backend, path, full_path, sizeof(full_path));

    DIR *dir = opendir(full_path);
    return (void *)dir;
}

static int posix_readdir(void *dir, gui_vfs_stat_t *vfs_stat)
{
    DIR *d = (DIR *)dir;
    struct dirent *entry = readdir(d);

    if (!entry) { return -1; }

    strncpy(vfs_stat->name, entry->d_name, sizeof(vfs_stat->name) - 1);
    vfs_stat->name[sizeof(vfs_stat->name) - 1] = '\0';

#if defined(_DIRENT_HAVE_D_TYPE) && defined(DT_DIR)
    vfs_stat->type = (entry->d_type == DT_DIR) ? GUI_VFS_TYPE_DIR : GUI_VFS_TYPE_FILE;
#else
    struct stat st;
    vfs_stat->type = (stat(entry->d_name, &st) == 0 && S_ISDIR(st.st_mode))
                     ? GUI_VFS_TYPE_DIR : GUI_VFS_TYPE_FILE;
#endif
    vfs_stat->size = 0;

    return 0;
}

static int posix_closedir(void *dir)
{
    DIR *d = (DIR *)dir;
    return closedir(d);
}

static int posix_stat(const char *path, gui_vfs_stat_t *vfs_stat, void *user_data)
{
    posix_backend_t *backend = (posix_backend_t *)user_data;

    char full_path[512];
    build_path(backend, path, full_path, sizeof(full_path));

    struct stat st;
    if (stat(full_path, &st) != 0) { return -1; }

    const char *name = strrchr(path, '/');
    name = name ? name + 1 : path;

    strncpy(vfs_stat->name, name, sizeof(vfs_stat->name) - 1);
    vfs_stat->name[sizeof(vfs_stat->name) - 1] = '\0';
    vfs_stat->type = S_ISDIR(st.st_mode) ? GUI_VFS_TYPE_DIR : GUI_VFS_TYPE_FILE;
    vfs_stat->size = st.st_size;

    return 0;
}


/* POSIX VFS operations table */
static const gui_vfs_ops_t posix_ops =
{
    .open = posix_open,
    .close = posix_close,
    .read = posix_read,
    .write = posix_write,
    .seek = posix_seek,
    .tell = posix_tell,
    .get_addr = NULL, /* No XIP support */
    .opendir = posix_opendir,
    .readdir = posix_readdir,
    .closedir = posix_closedir,
    .stat = posix_stat,
};

/**
 * @brief Mount POSIX filesystem
 * @param prefix Mount point (e.g., "/sd", "/data")
 * @param root_path Root directory path
 * @return 0 on success, -1 on error
 */
int gui_vfs_mount_posix(const char *prefix, const char *root_path)
{
    posix_backend_t *backend = (posix_backend_t *)gui_malloc(sizeof(posix_backend_t));
    if (!backend)
    {
        gui_log("[VFS] POSIX mount failed: out of memory\n");
        return -1;
    }

    strncpy(backend->root_path, root_path, sizeof(backend->root_path) - 1);
    backend->root_path[sizeof(backend->root_path) - 1] = '\0';

    int ret = gui_vfs_mount(prefix, &posix_ops, backend);
    if (ret == 0)
    {
        gui_log("[VFS] POSIX mounted at %s (path: %s)\n", prefix, root_path);
    }
    return ret;
}

#else /* No POSIX support */

#include "gui_vfs.h"

int gui_vfs_mount_posix(const char *prefix, const char *root_path)
{
    (void)prefix;
    (void)root_path;
    return -1; /* Not supported on this platform */
}

#endif /* POSIX support check */
