/**
 * @file gui_vfs_generic.h
 * @brief Generic filesystem adapter interface
 */

#ifndef GUI_VFS_GENERIC_H
#define GUI_VFS_GENERIC_H

#include <stddef.h>
#include "gui_vfs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Generic filesystem adapter operations
 *
 * Implement these functions to adapt any filesystem to VFS.
 * All functions are optional except fs_open, fs_close, fs_read.
 */
struct gui_fs_adapter
{
    /* File operations */
    void *(*fs_open)(const char *path, int flags);
    int (*fs_close)(void *file);
    int (*fs_read)(void *file, void *buf, size_t size);
    int (*fs_write)(void *file, const void *buf, size_t size);
    int (*fs_seek)(void *file, int offset, int whence);
    int (*fs_tell)(void *file);

    /* Directory operations (optional) */
    void *(*fs_opendir)(const char *path);
    int (*fs_readdir)(void *dir, char *name, size_t name_size, int *is_dir, size_t *size);
    int (*fs_closedir)(void *dir);

    /* File info (optional) */
    int (*fs_stat)(const char *path, int *is_dir, size_t *size);
};

/**
 * @brief Mount generic filesystem
 * @param prefix Mount point (e.g., "/sd", "/flash")
 * @param root_path Root path in the filesystem (can be empty or NULL)
 * @param adapter Filesystem adapter operations
 * @return 0 on success, -1 on error
 *
 * @example
 * // Mount FATFS
 * gui_vfs_mount_generic("/sd", "0:", &fatfs_adapter);
 *
 * // Mount LittleFS
 * gui_vfs_mount_generic("/flash", "", &littlefs_adapter);
 *
 * // Mount SPIFFS
 * gui_vfs_mount_generic("/spiffs", "/spiffs", &spiffs_adapter);
 */
int gui_vfs_mount_generic(const char *prefix, const char *root_path,
                          const gui_fs_adapter_t *adapter);

#ifdef __cplusplus
}
#endif

#endif /* GUI_VFS_GENERIC_H */
