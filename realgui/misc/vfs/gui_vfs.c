/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/**
 * @file gui_vfs.c
 * @brief Virtual File System implementation
 */

#include "gui_vfs.h"
#include "gui_api_os.h"
#include <string.h>
#include <stdlib.h>

/* Internal file structure */
struct gui_vfs_file
{
    const gui_vfs_ops_t *ops;
    void *backend_handle;
    gui_vfs_mount_t *mount;
};

/* Internal directory structure */
struct gui_vfs_dir
{
    const gui_vfs_ops_t *ops;
    void *backend_handle;
    gui_vfs_mount_t *mount;
};

/* Global mount list */
static gui_vfs_mount_t *g_mount_list = NULL;

void gui_vfs_init(void)
{
    g_mount_list = NULL;
    gui_log("[VFS] Initialized\n");
}

int gui_vfs_mount(const char *prefix, const gui_vfs_ops_t *ops, void *user_data)
{
    if (!prefix || !ops)
    {
        gui_log("[VFS] Mount failed: invalid parameters\n");
        return -1;
    }

    /* Check if already mounted */
    gui_vfs_mount_t *m = g_mount_list;
    while (m)
    {
        if (strcmp(m->prefix, prefix) == 0)
        {
            gui_log("[VFS] Mount failed: %s already mounted\n", prefix);
            return -1; /* Already mounted */
        }
        m = m->next;
    }

    /* Create new mount point */
    gui_vfs_mount_t *mount = (gui_vfs_mount_t *)gui_malloc(sizeof(gui_vfs_mount_t));
    if (!mount)
    {
        gui_log("[VFS] Mount failed: out of memory\n");
        return -1;
    }

    mount->prefix = prefix;
    mount->ops = ops;
    mount->user_data = user_data;
    mount->next = g_mount_list;
    g_mount_list = mount;

    gui_log("[VFS] Mounted: %s\n", prefix);
    return 0;
}

int gui_vfs_unmount(const char *prefix)
{
    if (!prefix) { return -1; }

    gui_vfs_mount_t **pp = &g_mount_list;
    while (*pp)
    {
        if (strcmp((*pp)->prefix, prefix) == 0)
        {
            gui_vfs_mount_t *m = *pp;
            *pp = m->next;
            gui_free(m);
            gui_log("[VFS] Unmounted: %s\n", prefix);
            return 0;
        }
        pp = &(*pp)->next;
    }

    gui_log("[VFS] Unmount failed: %s not found\n", prefix);
    return -1; /* Not found */
}

/* Find mount point for given path */
static gui_vfs_mount_t *find_mount(const char *path, const char **rel_path)
{
    gui_vfs_mount_t *best_match = NULL;
    size_t best_len = 0;

    gui_vfs_mount_t *m = g_mount_list;
    while (m)
    {
        size_t prefix_len = strlen(m->prefix);
        if (strncmp(path, m->prefix, prefix_len) == 0)
        {
            if (prefix_len > best_len)
            {
                best_match = m;
                best_len = prefix_len;
            }
        }
        m = m->next;
    }

    if (best_match && rel_path)
    {
        *rel_path = path + best_len;
        /* Skip leading slash */
        if (**rel_path == '/') { (*rel_path)++; }
    }

    return best_match;
}

gui_vfs_file_t *gui_vfs_open(const char *path, gui_vfs_mode_t mode)
{
    if (!path) { return NULL; }

    const char *rel_path;
    gui_vfs_mount_t *mount = find_mount(path, &rel_path);
    if (!mount || !mount->ops->open) { return NULL; }

    gui_vfs_file_t *file = (gui_vfs_file_t *)gui_malloc(sizeof(gui_vfs_file_t));
    if (!file) { return NULL; }

    file->ops = mount->ops;
    file->mount = mount;
    file->backend_handle = mount->ops->open(rel_path, mode, mount->user_data);

    if (!file->backend_handle)
    {
        gui_free(file);
        return NULL;
    }

    return file;
}

int gui_vfs_close(gui_vfs_file_t *file)
{
    if (!file || !file->ops->close) { return -1; }

    int ret = file->ops->close(file->backend_handle);
    gui_free(file);
    return ret;
}

int gui_vfs_read(gui_vfs_file_t *file, void *buf, size_t size)
{
    if (!file || !file->ops->read) { return -1; }
    return file->ops->read(file->backend_handle, buf, size);
}

int gui_vfs_write(gui_vfs_file_t *file, const void *buf, size_t size)
{
    if (!file || !file->ops->write) { return -1; }
    return file->ops->write(file->backend_handle, buf, size);
}

int gui_vfs_seek(gui_vfs_file_t *file, int offset, gui_vfs_seek_t whence)
{
    if (!file || !file->ops->seek) { return -1; }
    return file->ops->seek(file->backend_handle, offset, whence);
}

int gui_vfs_tell(gui_vfs_file_t *file)
{
    if (!file || !file->ops->tell) { return -1; }
    return file->ops->tell(file->backend_handle);
}

const void *gui_vfs_get_addr(gui_vfs_file_t *file, size_t *size)
{
    if (!file || !file->ops->get_addr) { return NULL; }
    return file->ops->get_addr(file->backend_handle, size);
}

gui_vfs_dir_t *gui_vfs_opendir(const char *path)
{
    if (!path) { return NULL; }

    const char *rel_path;
    gui_vfs_mount_t *mount = find_mount(path, &rel_path);
    if (!mount || !mount->ops->opendir) { return NULL; }

    gui_vfs_dir_t *dir = (gui_vfs_dir_t *)gui_malloc(sizeof(gui_vfs_dir_t));
    if (!dir) { return NULL; }

    dir->ops = mount->ops;
    dir->mount = mount;
    dir->backend_handle = mount->ops->opendir(rel_path, mount->user_data);

    if (!dir->backend_handle)
    {
        gui_free(dir);
        return NULL;
    }

    return dir;
}

int gui_vfs_readdir(gui_vfs_dir_t *dir, gui_vfs_stat_t *stat)
{
    if (!dir || !dir->ops->readdir) { return -1; }
    return dir->ops->readdir(dir->backend_handle, stat);
}

int gui_vfs_closedir(gui_vfs_dir_t *dir)
{
    if (!dir || !dir->ops->closedir) { return -1; }

    int ret = dir->ops->closedir(dir->backend_handle);
    gui_free(dir);
    return ret;
}

int gui_vfs_stat(const char *path, gui_vfs_stat_t *stat)
{
    if (!path) { return -1; }

    const char *rel_path;
    gui_vfs_mount_t *mount = find_mount(path, &rel_path);
    if (!mount || !mount->ops->stat) { return -1; }

    return mount->ops->stat(rel_path, stat, mount->user_data);
}

const char *gui_vfs_get_mount_prefix(const char *path)
{
    if (!path) { return NULL; }

    const char *rel_path;
    gui_vfs_mount_t *mount = find_mount(path, &rel_path);

    return mount ? mount->prefix : NULL;
}

const void *gui_vfs_get_file_address(const char *path)
{
    gui_vfs_file_t *file = gui_vfs_open(path, GUI_VFS_READ);
    if (!file) { return NULL; }

    const void *addr = gui_vfs_get_addr(file, NULL);
    gui_vfs_close(file);

    return addr;
}

int gui_vfs_copy_file(const char *src_path, const char *dst_path)
{
    if (!src_path || !dst_path)
    {
        gui_log("[VFS] Copy failed: invalid path\n");
        return -1;
    }

    gui_vfs_file_t *src = gui_vfs_open(src_path, GUI_VFS_READ);
    if (!src)
    {
        gui_log("[VFS] Copy failed: cannot open %s\n", src_path);
        return -1;
    }

    /* Try XIP for zero-copy */
    size_t size;
    const void *addr = gui_vfs_get_addr(src, &size);
    if (addr)
    {
        gui_vfs_file_t *dst = gui_vfs_open(dst_path, (gui_vfs_mode_t)(GUI_VFS_WRITE | GUI_VFS_CREATE));
        if (!dst)
        {
            gui_vfs_close(src);
            gui_log("[VFS] Copy failed: cannot create %s\n", dst_path);
            return -1;
        }

        int written = gui_vfs_write(dst, addr, size);
        gui_vfs_close(dst);
        gui_vfs_close(src);

        if (written != (int)size)
        {
            gui_log("[VFS] Copy failed: write error\n");
            return -1;
        }

        gui_log("[VFS] Copied %s -> %s (%zu bytes, XIP)\n", src_path, dst_path, size);
        return 0;
    }

    /* Fallback: buffered copy */
    gui_vfs_seek(src, 0, GUI_VFS_SEEK_END);
    int file_size = gui_vfs_tell(src);
    gui_vfs_seek(src, 0, GUI_VFS_SEEK_SET);

    if (file_size <= 0)
    {
        gui_vfs_close(src);
        return -1;
    }

    gui_vfs_file_t *dst = gui_vfs_open(dst_path, (gui_vfs_mode_t)(GUI_VFS_WRITE | GUI_VFS_CREATE));
    if (!dst)
    {
        gui_vfs_close(src);
        gui_log("[VFS] Copy failed: cannot create %s\n", dst_path);
        return -1;
    }

    uint8_t buffer[512];
    int total = 0;

    while (total < file_size)
    {
        int to_read = (file_size - total > 512) ? 512 : (file_size - total);
        int read_size = gui_vfs_read(src, buffer, to_read);
        if (read_size <= 0) { break; }

        int written = gui_vfs_write(dst, buffer, read_size);
        if (written != read_size)
        {
            gui_vfs_close(src);
            gui_vfs_close(dst);
            gui_log("[VFS] Copy failed: write error\n");
            return -1;
        }

        total += read_size;
    }

    gui_vfs_close(src);
    gui_vfs_close(dst);

    gui_log("[VFS] Copied %s -> %s (%d bytes)\n", src_path, dst_path, total);
    return (total == file_size) ? 0 : -1;
}

int gui_vfs_copy_dir(const char *src_path, const char *dst_path)
{
    if (!src_path || !dst_path)
    {
        gui_log("[VFS] Copy dir failed: invalid path\n");
        return -1;
    }

    gui_vfs_dir_t *dir = gui_vfs_opendir(src_path);
    if (!dir)
    {
        gui_log("[VFS] Copy dir failed: cannot open %s\n", src_path);
        return -1;
    }

    gui_vfs_stat_t stat;
    int count = 0;

    while (gui_vfs_readdir(dir, &stat) == 0)
    {
        char src_file[512];
        char dst_file[512];

        snprintf(src_file, sizeof(src_file), "%s/%s", src_path, stat.name);
        snprintf(dst_file, sizeof(dst_file), "%s/%s", dst_path, stat.name);

        if (stat.type == GUI_VFS_TYPE_FILE)
        {
            if (gui_vfs_copy_file(src_file, dst_file) == 0)
            {
                count++;
            }
        }
    }

    gui_vfs_closedir(dir);
    gui_log("[VFS] Copied directory %s -> %s (%d files)\n", src_path, dst_path, count);
    return 0;
}
