/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __HG_ROMFS_H__
#define __HG_ROMFS_H__

#include <stdint.h>
#include <stddef.h>

#define O_RDONLY         00
#define O_WRONLY         01
#define O_RDWR           02
#define O_CREAT        0100
#define O_EXCL         0200
#define O_NOCTTY       0400
#define O_TRUNC       01000
#define O_APPEND      02000
#define O_NONBLOCK    04000
#define O_DSYNC      010000
#define O_SYNC     04010000
#define O_RSYNC    04010000
#define O_BINARY    0100000
#define O_DIRECTORY 0200000
#define O_NOFOLLOW  0400000
#define O_CLOEXEC  02000000

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#define F_GETADDR 100  /* Custom fcntl command to get file address */

#define ROMFS_DIRENT_FILE   0x00
#define ROMFS_DIRENT_DIR    0x01

#define DT_UNKNOWN           0x00
#define DT_REG               0x01
#define DT_DIR               0x02

typedef signed long off_t;

struct romfs_fd
{
    char *path;
    int ref_count;
    uint32_t flags;
    size_t   size;
    off_t    pos;
    void *data;
};

struct romfs_dirent
{
    uint32_t      type;
    const char    *name;
    const uint8_t *data;
    size_t        size;
};

typedef struct
{
    intptr_t fd;
    char buf[512];
    int num;
    int cur;
} HG_DIR;

struct hg_dirent
{
    uint8_t d_type;
    uint8_t d_namlen;
    uint16_t d_reclen;
    uint32_t d_size;
    char d_name[100];
};

struct hg_stat
{
    uint32_t  st_size;
    uint32_t  st_mode;
};

/* HoneyGUI ROMFS Public API */
void hg_romfs_mount(void *addr);

/* File operations */
intptr_t hg_open(const char *file, int flags, ...);
int hg_close(intptr_t fd);
off_t hg_lseek(intptr_t fd, off_t offset, int whence);
int hg_read(intptr_t fd, void *buf, size_t len);
int hg_write(intptr_t fd, const void *buf, size_t len);
intptr_t hg_ioctl(intptr_t fildes, int cmd, ...);
int hg_fstat(intptr_t fildes, struct hg_stat *buf);

/* Directory operations */
HG_DIR *hg_opendir(const char *name);
int hg_closedir(HG_DIR *d);
struct hg_dirent *hg_readdir(HG_DIR *d);

/* Utility */
char *hg_strdup(const char *s);

#endif
