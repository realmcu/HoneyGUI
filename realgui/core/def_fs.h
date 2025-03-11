/**
 * @file def_vg.h
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

#if defined __WIN32


//#define GUI_ROOT_FOLDER "realgui\\example\\screen_480_480\\root\\"
extern char *defaultPath;
#define GUI_ROOT_FOLDER defaultPath
#else
#define GUI_ROOT_FOLDER "/"
#endif

typedef struct gui_fs_dirent
{
    uint8_t d_type;           /* The type of the file */
    uint8_t d_namlen;         /* The length of the not including the terminating null file name */
    uint16_t d_reclen;        /* length of this record */
    char *d_name;         /* The null-terminated file name */
    void *dirent;
} gui_fs_dirent_t;

typedef struct
{
    void *dir;
    gui_fs_dirent_t *dirent;
} gui_fs_dir;


typedef struct gui_fs_stat
{
    void *information;//by howie
    uint32_t  st_size;
} gui_fs_stat_t;
/* gui_fs struct define start */
struct gui_fs
{
    int (*open)(const char *file, int flags, ...);
    int (*close)(int d);
    int (*read)(int fd, void *buf, size_t len);
    int (*write)(int fd, const void *buf, size_t len);
    int (*lseek)(int fd, int offset, int whence);
    /* directory api*/
    gui_fs_dir *(*opendir)(const char *name);
    struct gui_fs_dirent *(*readdir)(gui_fs_dir *d);
    int (*closedir)(gui_fs_dir *d);
    int (*ioctl)(int fildes, int cmd, ...);
    void (*fstat)(int fildes, gui_fs_stat_t *buf);
};
/* gui_fs struct define end */

/* gui_ftl struct define start */
struct gui_ftl
{
    int (*read)(uint32_t addr, uint8_t *buf, uint32_t len);
    int (*write)(uint32_t addr, const uint8_t *buf, uint32_t len);
    int (*erase)(uint32_t addr, uint32_t len);
};
/* gui_ftl struct define end */



#ifdef __cplusplus
}
#endif

#endif

