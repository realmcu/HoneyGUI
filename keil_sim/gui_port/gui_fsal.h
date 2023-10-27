/*
 * File      : gui_fsal.h
 */
#ifndef __GUI_FSAL_H__
#define __GUI_FSAL_H__



#ifdef __cplusplus
extern "C" {
#endif
#include "guidef.h"
#include <stdint.h>
#include <stdbool.h>

struct gui_file_fd
{
    int fd;
    uint32_t addr_base;
    int offset;
    struct gui_fsal *fs;
    uint16_t magic;              /* file descriptor magic number */
    uint16_t type;               /* Type (regular or socket) */
    int ref_count;               /* Descriptor reference count */
};
enum fsal_seek_mode
{
    FSAL_SEEK_SET,
    FSAL_SEEK_CUR,
    FSAL_SEEK_END,
};
struct gui_fsal_ops
{
    int (*open)(struct gui_file_fd *fd, const char *file, int flags);
    int (*seek)(struct gui_file_fd *fd, int offset, int whence);
    int (*read)(struct gui_file_fd *fd, void *buffer, uint32_t size, uint32_t count);
    int (*write)(struct gui_file_fd *fd, const void *buffer, uint32_t size, uint32_t count);
    int (*close)(struct gui_file_fd *fd);
};

struct gui_fsal
{
    const char *name;
    char *path;             /* File system mount point */
    const struct gui_fsal_ops *ops; /* Operations for file system type */
    void *data;             /* Specific file system data */
};
typedef struct gui_file_fd gui_file_fd_t;
void fsal_register(struct gui_fsal *fs);
void fsal_init(void);
int fsal_open(struct gui_file_fd *fd, const char *file, int flags);
int fsal_seek(struct gui_file_fd *fd, int offset, int whence);
int fsal_read(struct gui_file_fd *fd, void *buffer, size_t len);
int fsal_write(struct gui_file_fd *fd, void *buffer, size_t len);
int fsal_close(struct gui_file_fd *fd);


#ifdef __cplusplus
}
#endif

#endif
