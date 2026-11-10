/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/**
 * @file gui_vfs.h
 * @brief Virtual File System abstraction layer for HoneyGUI
 *
 * Provides unified interface for accessing resources from:
 * - ROMFS (XIP, zero-copy)
 * - Standard filesystem (POSIX)
 * - Memory buffers
 * - Network/Remote storage
 */

#ifndef GUI_VFS_H
#define GUI_VFS_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declaration for generic adapter */
struct gui_fs_adapter;
typedef struct gui_fs_adapter gui_fs_adapter_t;

/* File access modes */
typedef enum
{
    GUI_VFS_READ    = 0x01,
    GUI_VFS_WRITE   = 0x02,
    GUI_VFS_APPEND  = 0x04,
    GUI_VFS_CREATE  = 0x08,
} gui_vfs_mode_t;

/* Seek origins */
typedef enum
{
    GUI_VFS_SEEK_SET = 0,
    GUI_VFS_SEEK_CUR = 1,
    GUI_VFS_SEEK_END = 2,
} gui_vfs_seek_t;

/* File type */
typedef enum
{
    GUI_VFS_TYPE_FILE = 0,
    GUI_VFS_TYPE_DIR  = 1,
} gui_vfs_type_t;

/* File handle (opaque) */
typedef struct gui_vfs_file gui_vfs_file_t;

/* Directory handle (opaque) */
typedef struct gui_vfs_dir gui_vfs_dir_t;

/* File info */
typedef struct
{
    char name[256];
    gui_vfs_type_t type;
    size_t size;
} gui_vfs_stat_t;

/* VFS operations interface */
typedef struct gui_vfs_ops
{
    /* File operations */
    void *(*open)(const char *path, gui_vfs_mode_t mode, void *user_data);
    int (*close)(void *file);
    int (*read)(void *file, void *buf, size_t size);
    int (*write)(void *file, const void *buf, size_t size);
    int (*seek)(void *file, int offset, gui_vfs_seek_t whence);
    int (*tell)(void *file);

    /* Direct access (optional, for XIP) */
    const void *(*get_addr)(void *file, size_t *size);

    /* Directory operations */
    void *(*opendir)(const char *path, void *user_data);
    int (*readdir)(void *dir, gui_vfs_stat_t *stat);
    int (*closedir)(void *dir);

    /* File info */
    int (*stat)(const char *path, gui_vfs_stat_t *stat, void *user_data);
} gui_vfs_ops_t;

/* VFS mount point */
typedef struct gui_vfs_mount
{
    const char *prefix;           /* Mount point prefix, e.g., "/rom", "/sd" */
    const gui_vfs_ops_t *ops;     /* Operations */
    void *user_data;              /* Backend-specific data */
    struct gui_vfs_mount *next;   /* Linked list */
} gui_vfs_mount_t;

/* VFS API */

/**
 * @brief Initialize VFS system
 */
void gui_vfs_init(void);

/**
 * @brief Mount a filesystem
 * @param prefix Mount point prefix (e.g., "/rom", "/sd")
 * @param ops VFS operations
 * @param user_data Backend-specific data
 * @return 0 on success, -1 on error
 */
int gui_vfs_mount(const char *prefix, const gui_vfs_ops_t *ops, void *user_data);

/**
 * @brief Unmount a filesystem
 * @param prefix Mount point prefix
 * @return 0 on success, -1 on error
 */
int gui_vfs_unmount(const char *prefix);

/**
 * @brief Mount ROMFS filesystem
 * @param prefix Mount point prefix
 * @param base_addr Base address of ROMFS image
 * @param base_offset Offset within the image
 * @return 0 on success, -1 on error
 */
int gui_vfs_mount_romfs(const char *prefix, const void *base_addr, uint32_t base_offset);

/**
 * @brief Mount POSIX filesystem
 * @param prefix Mount point prefix
 * @param root_path Root directory path
 * @return 0 on success, -1 on error
 */
int gui_vfs_mount_posix(const char *prefix, const char *root_path);

/**
 * @brief Mount RAM filesystem
 * @param prefix Mount point prefix
 * @param base_addr Base address of RAM region
 * @param size Size of RAM region
 * @return 0 on success, -1 on error
 */
int gui_vfs_mount_ramfs(const char *prefix, void *base_addr, size_t size);

/**
 * @brief Mount filesystem using generic adapter
 * @param prefix Mount point prefix
 * @param root_path Root directory path
 * @param adapter Filesystem adapter (see gui_vfs_generic.h)
 * @return 0 on success, -1 on error
 */
int gui_vfs_mount_generic(const char *prefix, const char *root_path,
                          const gui_fs_adapter_t *adapter);

/**
 * @brief Open a file
 * @param path File path (e.g., "/rom/image.png")
 * @param mode Access mode
 * @return File handle or NULL on error
 */
gui_vfs_file_t *gui_vfs_open(const char *path, gui_vfs_mode_t mode);

/**
 * @brief Close a file
 * @param file File handle
 * @return 0 on success, -1 on error
 */
int gui_vfs_close(gui_vfs_file_t *file);

/**
 * @brief Read from file
 * @param file File handle
 * @param buf Buffer to read into
 * @param size Number of bytes to read
 * @return Number of bytes read, or -1 on error
 */
int gui_vfs_read(gui_vfs_file_t *file, void *buf, size_t size);

/**
 * @brief Write to file
 * @param file File handle
 * @param buf Buffer to write from
 * @param size Number of bytes to write
 * @return Number of bytes written, or -1 on error
 */
int gui_vfs_write(gui_vfs_file_t *file, const void *buf, size_t size);

/**
 * @brief Seek in file
 * @param file File handle
 * @param offset Offset
 * @param whence Seek origin
 * @return New position, or -1 on error
 */
int gui_vfs_seek(gui_vfs_file_t *file, int offset, gui_vfs_seek_t whence);

/**
 * @brief Get current position in file
 * @param file File handle
 * @return Current position, or -1 on error
 */
int gui_vfs_tell(gui_vfs_file_t *file);

/**
 * @brief Get direct memory address (XIP support)
 * @param file File handle
 * @param size Output: file size
 * @return Pointer to file data, or NULL if not supported
 */
const void *gui_vfs_get_addr(gui_vfs_file_t *file, size_t *size);

/**
 * @brief Open directory
 * @param path Directory path
 * @return Directory handle or NULL on error
 */
gui_vfs_dir_t *gui_vfs_opendir(const char *path);

/**
 * @brief Read directory entry
 * @param dir Directory handle
 * @param stat Output: file info
 * @return 0 on success, -1 on end or error
 */
int gui_vfs_readdir(gui_vfs_dir_t *dir, gui_vfs_stat_t *stat);

/**
 * @brief Close directory
 * @param dir Directory handle
 * @return 0 on success, -1 on error
 */
int gui_vfs_closedir(gui_vfs_dir_t *dir);

/**
 * @brief Get file info
 * @param path File path
 * @param stat Output: file info
 * @return 0 on success, -1 on error
 */
int gui_vfs_stat(const char *path, gui_vfs_stat_t *stat);

/**
 * @brief Get mount point prefix for a path
 * @param path File path
 * @return Mount point prefix, or NULL if not found
 */
const char *gui_vfs_get_mount_prefix(const char *path);

/**
 * @brief Get direct memory address for a file (convenience function)
 * @param path File path
 * @return Pointer to file data, or NULL if not supported or file not found
 */
const void *gui_vfs_get_file_address(const char *path);

/**
 * @brief Copy file from one path to another
 * @param src_path Source file path
 * @param dst_path Destination file path
 * @return 0 on success, -1 on error
 */
int gui_vfs_copy_file(const char *src_path, const char *dst_path);

/**
 * @brief Copy directory recursively
 * @param src_path Source directory path
 * @param dst_path Destination directory path
 * @return 0 on success, -1 on error
 */
int gui_vfs_copy_dir(const char *src_path, const char *dst_path);

#ifdef __cplusplus
}
#endif

#endif /* GUI_VFS_H */
