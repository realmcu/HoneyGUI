/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

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


/* POSIX adapter structure */
static const gui_fs_adapter_t posix_adapter =
{
    .fs_open = NULL,
    .fs_close = NULL,
    .fs_read = NULL,
    .fs_write = NULL,
    .fs_seek = NULL,
    .fs_tell = NULL,
    .fs_opendir = NULL,   /* Not needed for basic file operations */
    .fs_readdir = NULL,
    .fs_closedir = NULL,
    .fs_stat = NULL,
};

/**
 * @brief Initialize filesystem port for win32 simulator
 *
 * This function initializes VFS and mounts:
 * - /sd: PC filesystem (./sdcard) using POSIX backend
 * - /data: Current directory using generic adapter
 */
void gui_port_fs_init(void)
{
    /* Initialize VFS */
    gui_vfs_init();

    // Mount ROMFS
    gui_vfs_mount_romfs("/rom", 0x00, 0);

    /* Mount current directory using generic adapter (as example) */
    gui_vfs_mount_generic("/vfs_adapter", ".", &posix_adapter);
}
