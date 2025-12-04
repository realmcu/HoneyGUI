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
    gui_vfs_mount_t *mount = (gui_vfs_mount_t *)malloc(sizeof(gui_vfs_mount_t));
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
            free(m);
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

    gui_vfs_file_t *file = (gui_vfs_file_t *)malloc(sizeof(gui_vfs_file_t));
    if (!file) { return NULL; }

    file->ops = mount->ops;
    file->mount = mount;
    file->backend_handle = mount->ops->open(rel_path, mode, mount->user_data);

    if (!file->backend_handle)
    {
        free(file);
        return NULL;
    }

    return file;
}

int gui_vfs_close(gui_vfs_file_t *file)
{
    if (!file || !file->ops->close) { return -1; }

    int ret = file->ops->close(file->backend_handle);
    free(file);
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

    gui_vfs_dir_t *dir = (gui_vfs_dir_t *)malloc(sizeof(gui_vfs_dir_t));
    if (!dir) { return NULL; }

    dir->ops = mount->ops;
    dir->mount = mount;
    dir->backend_handle = mount->ops->opendir(rel_path, mount->user_data);

    if (!dir->backend_handle)
    {
        free(dir);
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
    free(dir);
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

void *gui_vfs_load_file(const char *path, size_t *size)
{
    gui_vfs_file_t *file = gui_vfs_open(path, GUI_VFS_READ);
    if (!file) { return NULL; }

    /* Try XIP first (zero-copy) */
    const void *addr = gui_vfs_get_addr(file, size);
    if (addr)
    {
        /* XIP supported, but we need to copy for consistency */
        void *buf = malloc(*size);
        if (buf)
        {
            memcpy(buf, addr, *size);
        }
        gui_vfs_close(file);
        return buf;
    }

    /* Get file size */
    gui_vfs_seek(file, 0, GUI_VFS_SEEK_END);
    int file_size = gui_vfs_tell(file);
    gui_vfs_seek(file, 0, GUI_VFS_SEEK_SET);

    if (file_size <= 0)
    {
        gui_vfs_close(file);
        return NULL;
    }

    /* Allocate and read */
    void *buf = malloc(file_size);
    if (!buf)
    {
        gui_vfs_close(file);
        return NULL;
    }

    int read_size = gui_vfs_read(file, buf, file_size);
    gui_vfs_close(file);

    if (read_size != file_size)
    {
        free(buf);
        return NULL;
    }

    if (size) { *size = file_size; }
    return buf;
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
