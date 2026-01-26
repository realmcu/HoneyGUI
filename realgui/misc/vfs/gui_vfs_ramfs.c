/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/**
 * @file gui_vfs_ramfs.c
 * @brief RAM filesystem implementation (read/write, XIP support)
 */

#include "gui_vfs.h"
#include "gui_vfs_ramfs.h"
#include "gui_api_os.h"
#include <string.h>
#include <stdlib.h>

#define RAMFS_MAX_FILES 32
#define RAMFS_MAX_PATH 128

typedef struct ramfs_file_entry
{
    char path[RAMFS_MAX_PATH];
    void *data;
    size_t size;
    size_t capacity;
    int in_use;
} ramfs_file_entry_t;

typedef struct ramfs_backend
{
    void *base_addr;
    size_t total_size;
    size_t used_size;
    ramfs_file_entry_t files[RAMFS_MAX_FILES];
} ramfs_backend_t;

typedef struct ramfs_file_handle
{
    ramfs_file_entry_t *entry;
    size_t position;
    int writable;
} ramfs_file_handle_t;

typedef struct ramfs_dir_handle
{
    ramfs_backend_t *backend;
    int index;
} ramfs_dir_handle_t;

static ramfs_file_entry_t *ramfs_find_file(ramfs_backend_t *backend, const char *path)
{
    for (int i = 0; i < RAMFS_MAX_FILES; i++)
    {
        if (backend->files[i].in_use && strcmp(backend->files[i].path, path) == 0)
        {
            return &backend->files[i];
        }
    }
    return NULL;
}

static ramfs_file_entry_t *ramfs_create_file(ramfs_backend_t *backend, const char *path)
{
    for (int i = 0; i < RAMFS_MAX_FILES; i++)
    {
        if (!backend->files[i].in_use)
        {
            strncpy(backend->files[i].path, path, RAMFS_MAX_PATH - 1);
            backend->files[i].path[RAMFS_MAX_PATH - 1] = '\0';
            backend->files[i].data = NULL;
            backend->files[i].size = 0;
            backend->files[i].capacity = 0;
            backend->files[i].in_use = 1;
            return &backend->files[i];
        }
    }
    return NULL;
}

static void *ramfs_vfs_open(const char *path, gui_vfs_mode_t mode, void *user_data)
{
    ramfs_backend_t *backend = (ramfs_backend_t *)user_data;
    if (!backend || !path) { return NULL; }

    ramfs_file_entry_t *entry = ramfs_find_file(backend, path);

    if (!entry && (mode & (GUI_VFS_WRITE | GUI_VFS_CREATE)))
    {
        entry = ramfs_create_file(backend, path);
    }

    if (!entry) { return NULL; }

    ramfs_file_handle_t *handle = (ramfs_file_handle_t *)gui_malloc(sizeof(
                                                                        ramfs_file_handle_t));
    if (!handle) { return NULL; }

    handle->entry = entry;
    handle->position = (mode & GUI_VFS_APPEND) ? entry->size : 0;
    handle->writable = (mode & (GUI_VFS_WRITE | GUI_VFS_APPEND | GUI_VFS_CREATE)) ? 1 : 0;

    return handle;
}

static int ramfs_vfs_close(void *file)
{
    if (!file) { return -1; }
    gui_free(file);
    return 0;
}

static int ramfs_vfs_read(void *file, void *buf, size_t size)
{
    ramfs_file_handle_t *handle = (ramfs_file_handle_t *)file;
    if (!handle || !buf) { return -1; }

    ramfs_file_entry_t *entry = handle->entry;
    if (!entry->data) { return 0; }

    size_t available = entry->size - handle->position;
    size_t to_read = (size < available) ? size : available;

    memcpy(buf, (uint8_t *)entry->data + handle->position, to_read);
    handle->position += to_read;

    return to_read;
}

static int ramfs_vfs_write(void *file, const void *buf, size_t size)
{
    ramfs_file_handle_t *handle = (ramfs_file_handle_t *)file;
    if (!handle || !buf || !handle->writable) { return -1; }

    ramfs_file_entry_t *entry = handle->entry;
    size_t new_size = handle->position + size;

    if (new_size > entry->capacity)
    {
        size_t new_capacity = (new_size + 1023) & ~1023;
        void *new_data = gui_malloc(new_capacity);
        if (!new_data) { return -1; }

        if (entry->data)
        {
            memcpy(new_data, entry->data, entry->size);
            gui_free(entry->data);
        }

        entry->data = new_data;
        entry->capacity = new_capacity;
    }

    memcpy((uint8_t *)entry->data + handle->position, buf, size);
    handle->position += size;

    if (handle->position > entry->size)
    {
        entry->size = handle->position;
    }

    return size;
}

static int ramfs_vfs_seek(void *file, int offset, gui_vfs_seek_t whence)
{
    ramfs_file_handle_t *handle = (ramfs_file_handle_t *)file;
    if (!handle) { return -1; }

    int new_pos;
    switch (whence)
    {
    case GUI_VFS_SEEK_SET:
        new_pos = offset;
        break;
    case GUI_VFS_SEEK_CUR:
        new_pos = handle->position + offset;
        break;
    case GUI_VFS_SEEK_END:
        new_pos = handle->entry->size + offset;
        break;
    default:
        return -1;
    }

    if (new_pos < 0) { new_pos = 0; }
    handle->position = new_pos;
    return new_pos;
}

static int ramfs_vfs_tell(void *file)
{
    ramfs_file_handle_t *handle = (ramfs_file_handle_t *)file;
    return handle ? (int)handle->position : -1;
}

static const void *ramfs_vfs_get_addr(void *file, size_t *size)
{
    ramfs_file_handle_t *handle = (ramfs_file_handle_t *)file;
    if (!handle || !handle->entry->data) { return NULL; }

    if (size) { *size = handle->entry->size; }
    return handle->entry->data;
}

static void *ramfs_vfs_opendir(const char *path, void *user_data)
{
    ramfs_backend_t *backend = (ramfs_backend_t *)user_data;
    (void)path;
    if (!backend) { return NULL; }

    ramfs_dir_handle_t *handle = (ramfs_dir_handle_t *)gui_malloc(sizeof(ramfs_dir_handle_t));
    if (!handle) { return NULL; }

    handle->backend = backend;
    handle->index = 0;
    return handle;
}

static int ramfs_vfs_readdir(void *dir, gui_vfs_stat_t *stat)
{
    ramfs_dir_handle_t *handle = (ramfs_dir_handle_t *)dir;
    if (!handle || !stat) { return -1; }

    while (handle->index < RAMFS_MAX_FILES)
    {
        ramfs_file_entry_t *entry = &handle->backend->files[handle->index++];
        if (entry->in_use)
        {
            strncpy(stat->name, entry->path, sizeof(stat->name) - 1);
            stat->name[sizeof(stat->name) - 1] = '\0';
            stat->type = GUI_VFS_TYPE_FILE;
            stat->size = entry->size;
            return 0;
        }
    }

    return -1;
}

static int ramfs_vfs_closedir(void *dir)
{
    if (!dir) { return -1; }
    gui_free(dir);
    return 0;
}

static int ramfs_vfs_stat(const char *path, gui_vfs_stat_t *stat, void *user_data)
{
    ramfs_backend_t *backend = (ramfs_backend_t *)user_data;
    if (!backend || !path || !stat) { return -1; }

    ramfs_file_entry_t *entry = ramfs_find_file(backend, path);
    if (!entry) { return -1; }

    strncpy(stat->name, entry->path, sizeof(stat->name) - 1);
    stat->name[sizeof(stat->name) - 1] = '\0';
    stat->type = GUI_VFS_TYPE_FILE;
    stat->size = entry->size;

    return 0;
}

static const gui_vfs_ops_t ramfs_ops =
{
    .open = ramfs_vfs_open,
    .close = ramfs_vfs_close,
    .read = ramfs_vfs_read,
    .write = ramfs_vfs_write,
    .seek = ramfs_vfs_seek,
    .tell = ramfs_vfs_tell,
    .get_addr = ramfs_vfs_get_addr,
    .opendir = ramfs_vfs_opendir,
    .readdir = ramfs_vfs_readdir,
    .closedir = ramfs_vfs_closedir,
    .stat = ramfs_vfs_stat,
};

int gui_vfs_mount_ramfs(const char *prefix, void *base_addr, size_t size)
{
    if (!prefix) { return -1; }

    ramfs_backend_t *backend = (ramfs_backend_t *)gui_malloc(sizeof(ramfs_backend_t));
    if (!backend) { return -1; }

    memset(backend, 0, sizeof(ramfs_backend_t));
    backend->base_addr = base_addr;
    backend->total_size = size;
    backend->used_size = 0;

    int ret = gui_vfs_mount(prefix, &ramfs_ops, backend);
    if (ret != 0)
    {
        gui_free(backend);
        return -1;
    }

    gui_log("[RAMFS] Mounted at %s (size: %zu bytes)\n", prefix, size);
    return 0;
}
