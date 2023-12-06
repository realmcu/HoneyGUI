/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019/01/13     Bernard      code cleanup
 */

#ifndef __ROMFS_H__
#define __ROMFS_H__


#include "stdint.h"
#include "string.h"


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

#define O_ASYNC      020000
#define O_DIRECT     040000
#define O_LARGEFILE 0100000
#define O_NOATIME  01000000
#define O_PATH    010000000
#define O_TMPFILE 020200000
#define O_NDELAY O_NONBLOCK

#define O_SEARCH  O_PATH
#define O_EXEC    O_PATH

#define O_ACCMODE (03|O_SEARCH)
#define S_IFMT               00170000
#define S_IFSOCK             0140000
#define S_IFLNK              0120000
#define S_IFREG              0100000
#define S_IFBLK              0060000
#define S_IFDIR              0040000
#define S_IFCHR              0020000
#define S_IFIFO              0010000
#define S_ISUID              0004000
#define S_ISGID              0002000
#define S_ISVTX              0001000

#define S_ISLNK(m)           (((m) & S_IFMT) == S_IFLNK)
#define S_ISREG(m)           (((m) & S_IFMT) == S_IFREG)
#define S_ISDIR(m)           (((m) & S_IFMT) == S_IFDIR)
#define S_ISCHR(m)           (((m) & S_IFMT) == S_IFCHR)
#define S_ISBLK(m)           (((m) & S_IFMT) == S_IFBLK)
#define S_ISFIFO(m)          (((m) & S_IFMT) == S_IFIFO)
#define S_ISSOCK(m)          (((m) & S_IFMT) == S_IFSOCK)

#define S_IRWXU              00700
#define S_IRUSR              00400
#define S_IWUSR              00200
#define S_IXUSR              00100

#define S_IRWXG              00070
#define S_IRGRP              00040
#define S_IWGRP              00020
#define S_IXGRP              00010

#define S_IRWXO              00007
#define S_IROTH              00004
#define S_IWOTH              00002
#define S_IXOTH              00001
#define SEEK_SET 0 /* start of stream (see fseek) */
#define SEEK_CUR 1 /* current position in stream (see fseek) */
#define SEEK_END 2 /* end of stream (see fseek) */

#define ROMFS_DIRENT_FILE   0x00
#define ROMFS_DIRENT_DIR    0x01

#define DT_UNKNOWN           0x00
#define DT_REG               0x01
#define DT_DIR               0x02
#define ESUCCESS     0  /* Operation Success */

#define EPERM        1  /* Operation not permitted */
#define ENOENT       2  /* No such file or directory */
#define ESRCH        3  /* No such process */
#define EINTR        4  /* Interrupted system call */
#define EIO          5  /* I/O error */
#define ENXIO        6  /* No such device or address */
#define E2BIG        7  /* Argument list too long */
#define ENOEXEC      8  /* Exec format error */
#define EBADF        9  /* Bad file number */
#define ECHILD      10  /* No child processes */
#define EAGAIN      11  /* Try again */
#define FS_ENOMEM      12  /* Out of memory */
#define EACCES      13  /* Permission denied */
#define EFAULT      14  /* Bad address */
#define ENOTBLK     15  /* Block device required */
#define EBUSY       16  /* Device or resource busy */
#define EEXIST      17  /* File exists */
#define EXDEV       18  /* Cross-device link */
#define ENODEV      19  /* No such device */
#define ENOTDIR     20  /* Not a directory */
#define EISDIR      21  /* Is a directory */
#define FS_EINVAL      22  /* Invalid argument */
#define ENFILE      23  /* File table overflow */
#define EMFILE      24  /* Too many open files */
#define ENOTTY      25  /* Not a typewriter */
#define ETXTBSY     26  /* Text file busy */
#define EFBIG       27  /* File too large */
#define ENOSPC      28  /* No space left on device */
#define ESPIPE      29  /* Illegal seek */
#define EROFS       30  /* Read-only file system */
#define EMLINK      31  /* Too many links */
#define EPIPE       32  /* Broken pipe */
#define FS_EDOM        33  /* Math argument out of domain of func */
#define FS_ERANGE      34  /* Math result not representable */
#define EVRF        35  /* Verification failed */
#define ENOF        36  /* Not found */


#define ENODATA     61  /* No data available */
#define EOVERFLOW   75  /* Value too large for defined data type */
#define ERESTART    85  /* Interrupted system call should be restarted */
#define ECONNABORTED    103 /* Software caused connection abort */
#define EOPNOTSUPP  95  /* Operation not supported on transport endpoint */
#define ECONNRESET  104 /* Connection reset by peer */
#define ENOBUFS     105 /* No buffer space available */
#define ESHUTDOWN   108 /* Cannot send after transport endpoint shutdown */
#define ETIMEDOUT   110 /* Connection timed out */
#define EINPROGRESS 115 /* Operation now in progress */
#define ENOTSUPP        524     /* Operation is not supported */
typedef signed long off_t;


struct romfs_fd
{
    char *path;                  /* Name (below mount point) */
    int ref_count;               /* Descriptor reference count */

    uint32_t flags;              /* Descriptor flags */
    size_t   size;               /* Size in bytes */
    off_t    pos;                /* Current file position */

    void *data;                  /* Specific file system data */
};

struct romfs_dirent
{
    uint32_t      type;  /* dirent type */

    const char       *name; /* dirent name */
    const uint8_t *data; /* file date ptr */
    size_t        size;  /* file size */
};
typedef struct
{
    int fd;     /* directory file */
    char buf[512];
    int num;
    int cur;
} DIR;
struct dirent
{
    uint8_t d_type;           /* The type of the file */
    uint8_t d_namlen;         /* The length of the not including the terminating null file name */
    uint16_t d_reclen;        /* length of this record */
    char d_name[100];   /* The null-terminated file name */
};
/* stat structure */
#include <stdint.h>
#include <time.h>
struct stat
{
    struct rt_device *st_dev;
    uint16_t  st_ino;
    uint16_t  st_mode;
    uint16_t  st_nlink;
    uint16_t  st_uid;
    uint16_t  st_gid;
    struct rt_device *st_rdev;
    uint32_t  st_size;
    uint32_t  st_atime;
    long      st_spare1;
    uint32_t    st_mtime;
    long      st_spare2;
    uint32_t    st_ctime;
    long      st_spare3;
    uint32_t  st_blksize;
    uint32_t  st_blocks;
    long      st_spare4[2];
};
void romfs_mount(void *addr);
extern const struct romfs_dirent romfs_root;
int open(const char *file, int flags, ...);
int close(int fd);
off_t lseek(int fd, off_t offset, int whence);
int read(int fd, void *buf, size_t len);
int ioctl(int fildes, int cmd, ...);
int write(int fd, const void *buf, size_t len);
struct dirent *readdir(DIR *d);
DIR *opendir(const char *name);
int closedir(DIR *d);
char *strdup(const char *s);
int fstat(int fildes, struct stat *buf);
#endif
