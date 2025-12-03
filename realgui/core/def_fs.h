/**
 * @file def_fs.h
 * @author wenjing_jiang(wenjing_jiang@realsil.com.cn)
 * @brief
 * @version 0.1
 * @date 2024-06-14
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef __DEF_FS_H__
#define __DEF_FS_H__



#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


typedef struct gui_fs_dirent
{
    uint8_t d_type;           /* The type of the file */
    uint8_t d_namlen;         /* The length of the file name not included the terminating null */
    uint16_t d_reclen;        /* length of this record */
    char *d_name;         /* The null-terminated file name */
    void *dirent;
} gui_dirent_t;

typedef struct gui_fs_dir
{
    void *dir;
    gui_dirent_t *dirent;
} gui_dir_t;


typedef struct gui_fs_stat
{
    void *information;//by howie
    uint32_t  st_size;
} gui_fstat_t;
/* gui_fs struct define start */
struct gui_fs
{
    int (*open)(const char *file, int flags, ...);
    int (*close)(int d);
    int (*read)(int fd, void *buf, size_t len);
    int (*write)(int fd, const void *buf, size_t len);
    int (*lseek)(int fd, int offset, int whence);
    /* directory api*/
    gui_dir_t *(*opendir)(const char *name);
    struct gui_fs_dirent *(*readdir)(gui_dir_t *d);
    int (*closedir)(gui_dir_t *d);
    int (*ioctl)(int fildes, int cmd, ...);
    void (*fstat)(int fildes, gui_fstat_t *buf);
};
/* gui_fs struct define end */

/* gui_ftl struct define start */
struct gui_ftl
{
    int (*read)(uintptr_t addr, uint8_t *buf, size_t len);
    int (*write)(uintptr_t addr, const uint8_t *buf, size_t len);
    int (*erase)(uintptr_t addr, size_t len);
};
/* gui_ftl struct define end */



#ifdef __cplusplus
}
#endif

#endif

