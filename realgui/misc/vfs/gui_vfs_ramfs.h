/**
 * @file gui_vfs_ramfs.h
 * @brief RAM filesystem for VFS (read/write, XIP support)
 */

#ifndef GUI_VFS_RAMFS_H
#define GUI_VFS_RAMFS_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Mount RAM filesystem
 * @param prefix Mount point prefix (e.g., "/ram")
 * @param base_addr Base address of RAM region
 * @param size Size of RAM region
 * @return 0 on success, -1 on error
 */
int gui_vfs_mount_ramfs(const char *prefix, void *base_addr, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* GUI_VFS_RAMFS_H */
