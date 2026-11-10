/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/**
 * @file gui_port_filesystem.c
 * @brief Filesystem port for the browser (WebAssembly) simulator
 *
 * Emscripten provides an in-memory POSIX filesystem (MEMFS). The page writes
 * the project's runtime assets into it before starting the simulator, so the
 * same POSIX VFS backend as the desktop simulator works unchanged.
 */

#include "guidef.h"
#include "gui_port.h"
#include "gui_vfs.h"

void gui_port_fs_init(void)
{
    gui_vfs_init();

    /* Same mount point as the desktop simulator. */
    gui_vfs_mount_posix("/pc", ".");
}
