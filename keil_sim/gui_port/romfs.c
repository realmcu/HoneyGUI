#include "romfs.h"
#include "trace.h"
#include "stdarg.h"

// default romfs address
static void *romfs_addr = (void *)0x04400000;

void romfs_mount(void *addr)
{
    romfs_addr = addr;
}


int check_dirent(struct romfs_dirent *dirent)
{
    if ((dirent->type != ROMFS_DIRENT_FILE && dirent->type != ROMFS_DIRENT_DIR)
        || dirent->size == ~0)
    {
        return -1;
    }
    return 0;
}

struct romfs_dirent *romfs_lookup(struct romfs_dirent *root_dirent, const char *path, size_t *size)
{
    size_t index, found;
    const char *subpath, *subpath_end;
    struct romfs_dirent *dirent;
    size_t dirent_size;

    /* Check the root_dirent. */
    if (check_dirent(root_dirent) != 0)
    {
        return NULL;
    }

    if (path)
    {
        if (path[0] == '/' && path[1] == '\0')
        {
            *size = root_dirent->size;
            return root_dirent;
        }
    }

    /* goto root directory entries */
    dirent = (struct romfs_dirent *)root_dirent->data;
    dirent_size = root_dirent->size;

    /* get the end position of this subpath */
    subpath_end = path;
    /* skip /// */
    if (subpath_end)
    {
        while (*subpath_end == '/')
        {
            subpath_end ++;
        }
        subpath = subpath_end;
        while ((*subpath_end != '/') && *subpath_end)
        {
            subpath_end ++;
        }
    }

    while (dirent != NULL)
    {
        found = 0;

        /* search in folder */
        for (index = 0; index < dirent_size; index ++)
        {
            if (check_dirent(&dirent[index]) != 0)
            {
                return NULL;
            }
            if (subpath_end && subpath && (subpath_end - subpath) >= 0)
            {
                if (strlen(dirent[index].name) == (subpath_end - subpath) &&
                    strncmp(dirent[index].name, subpath, (subpath_end - subpath)) == 0)
                {
                    dirent_size = dirent[index].size;

                    /* skip /// */
                    if (subpath_end)
                    {
                        while (*subpath_end == '/')
                        {
                            subpath_end ++;
                        }
                        subpath = subpath_end;
                        while ((*subpath_end != '/') && *subpath_end)
                        {
                            subpath_end ++;
                        }
                    }

                    if (subpath != NULL)
                    {
                        if (!(*subpath))
                        {
                            *size = dirent_size;
                            return &dirent[index];
                        }
                    }

                    if (dirent[index].type == ROMFS_DIRENT_DIR)
                    {
                        /* enter directory */
                        dirent = (struct romfs_dirent *)dirent[index].data;
                        found = 1;
                        break;
                    }
                    else
                    {
                        /* return file dirent */
                        //if (subpath != NULL)
                        {
                            break;    /* not the end of path */
                        }

                        //return &dirent[index];
                    }
                }
            }
        }

        if (!found)
        {
            break;    /* not found */
        }
    }

    /* not found */
    return NULL;
}

int romfs_read(struct romfs_fd *file, void *buf, size_t count)
{
    size_t length;
    struct romfs_dirent *dirent;

    dirent = (struct romfs_dirent *)file->data;

    if (check_dirent(dirent) != 0)
    {
        return -EIO;
    }

    if (count < file->size - file->pos)
    {
        length = count;
    }
    else
    {
        length = file->size - file->pos;
    }

    if (length > 0)
    {
        memcpy(buf, &(dirent->data[file->pos]), length);
    }

    /* update file current position */
    file->pos += length;

    return length;
}

int romfs_lseek(struct romfs_fd *file, off_t offset)
{
    if (offset <= file->size)
    {
        file->pos = offset;
        return file->pos;
    }

    return -EIO;
}

int romfs_close(struct romfs_fd *file)
{
    file->data = NULL;
    return 0;
}

int romfs_open(struct romfs_fd *file)
{
    size_t size;
    struct romfs_dirent *dirent;
    struct romfs_dirent *root_dirent;

    if (file)
    {
        root_dirent = file->data;

        if (check_dirent(root_dirent) != 0)
        {
            return -EIO;
        }

        if (file->flags & (O_CREAT | O_WRONLY | O_APPEND | O_TRUNC | O_RDWR))
        {
            return -FS_EINVAL;
        }

        dirent = romfs_lookup(root_dirent, file->path, &size);
        if (dirent == NULL)
        {
            return -ENOENT;
        }

        /* entry is a directory file type */
        if (dirent->type == ROMFS_DIRENT_DIR)
        {
            if (!(file->flags & O_DIRECTORY))
            {
                return -ENOENT;
            }
        }
        else
        {
            /* entry is a file, but open it as a directory */
            if (file->flags & O_DIRECTORY)
            {
                return -ENOENT;
            }
        }

        file->data = dirent;
        file->size = size;
        file->pos = 0;
    }

    return 0;
}

int open(const char *file, int flags, ...)
{
    int result;
    struct romfs_fd *fd;

    /* allocate a fd */
    fd = malloc(sizeof(struct romfs_fd));
    if (fd)
    {
        fd->flags = flags;
        fd->size  = 0;
        fd->pos   = 0;
        fd->data  = (void *)romfs_addr;
        fd->path = (char *)file;
    }

    result = romfs_open(fd);
    if (result < 0)
    {
        free(fd);
        return -1;
    }

    int f = (int)fd;

    return f;
}

int close(int fd)
{
    int result;
    struct romfs_fd *d = (struct romfs_fd *)fd;

    if (d == NULL)
    {
        return -1;
    }

    result = romfs_close(d);

    if (result < 0)
    {
        return -1;
    }

    free(d);
    d = NULL;

    return 0;
}

int read(int fd, void *buf, size_t len)
{
    int result;
    struct romfs_fd *d = (struct romfs_fd *)fd;

    /* get the fd */
    if (d == NULL)
    {
        return -1;
    }

    result = romfs_read(d, buf, len);
    if (result < 0)
    {
        return -1;
    }

    /* release the ref-count of fd */

    return result;
}

off_t lseek(int fd, off_t offset, int whence)
{
    int result;
    struct romfs_fd *d = (struct romfs_fd *)fd;

    if (d == NULL)
    {
        return -1;
    }

    switch (whence)
    {
    case SEEK_SET:
        break;

    case SEEK_CUR:
        offset += d->pos;
        break;

    case SEEK_END:
        offset += d->size;
        break;

    default:

        return -1;
    }

    if (offset < 0)
    {
        return -1;
    }
    result = romfs_lseek(d, offset);
    if (result < 0)
    {
        return -1;
    }

    /* release the ref-count of fd */
    return offset;
}
/**
 * this function is a POSIX compliant version, which will close a directory
 * stream.
 *
 * @param d the directory stream.
 *
 * @return 0 on successful, -1 on failed.
 */
int closedir(DIR *d)
{
    if (d->fd == 0)
    {

        return -1;
    }
    close(d->fd);
    free(d);
    return 0;
}
/**
 * this function is a POSIX compliant version, which will open a directory.
 *
 * @param name the path name to be open.
 *
 * @return the DIR pointer of directory, NULL on open directory failed.
 */
DIR *opendir(const char *name)
{
    int fd;
    DIR *t;

    t = NULL;

    fd = open(name, O_RDONLY | O_DIRECTORY);
    if (fd >= 0)
    {
        /* open successfully */
        t = (DIR *) malloc(sizeof(DIR));
        if (t == NULL)
        {
            close(fd);
        }
        else
        {
            memset(t, 0, sizeof(DIR));
            t->fd = fd;
        }

        return t;
    }

    /* open failed */

    return NULL;
}
static int dfs_romfs_getdents(struct romfs_fd *file, struct dirent *dirp, uint32_t count)
{
    size_t index;
    const char *name;
    struct dirent *d;
    struct romfs_dirent *dirent, *sub_dirent;
    dirent = (struct romfs_dirent *)file->data;
    if (check_dirent(dirent) != 0)
    {
        return -1;
    }


    /* enter directory */
    dirent = (struct romfs_dirent *)dirent->data;

    /* make integer count */
    count = (count / sizeof(struct dirent));
    if (count == 0)
    {
        return -2;
    }

    index = 0;
    for (index = 0; index < count && file->pos < file->size; index ++)
    {
        d = dirp + index;

        sub_dirent = &dirent[file->pos];
        name = sub_dirent->name;

        /* fill dirent */
        if (sub_dirent->type == ROMFS_DIRENT_DIR)
        {
            d->d_type = DT_DIR;
        }
        else
        {
            d->d_type = DT_REG;
        }

        d->d_namlen = strlen(name);
        d->d_reclen = (uint16_t)sizeof(struct dirent);
        strncpy(d->d_name, name, strlen(name) + 1);

        /* move to next position */
        ++ file->pos;
    }

    return index * sizeof(struct dirent);
}
/**
 * @ingroup Fd
 *
 * This function will return a file descriptor structure according to file
 * descriptor.
 *
 * @return NULL on on this file descriptor or the file descriptor structure
 * pointer.
 */
static struct romfs_fd *fd_get(int fd)
{
    return (void *)fd;
}
static int dfs_romfs_ioctl(struct romfs_fd *file, int cmd, void *args)
{
    switch (cmd)
    {
    case 0:
        {
            size_t length;
            struct romfs_dirent *dirent;

            dirent = (struct romfs_dirent *)file->data;

            if (check_dirent(dirent) != 0)
            {
                return -EIO;
            }

            return (int) & (dirent->data[file->pos]);

        }
        break;
    }
    return -EIO;
}
/**
 * this function is a POSIX compliant version, which will return a pointer
 * to a dirent structure representing the next directory entry in the
 * directory stream.
 *
 * @param d the directory stream pointer.
 *
 * @return the next directory entry, NULL on the end of directory or failed.
 */
struct dirent *readdir(DIR *d)
{
    int result;
    struct romfs_fd *fd;

    fd = fd_get(d->fd);
    if (fd == NULL)
    {
        DBG_DIRECT("fd == NULL");
        return NULL;
    }

    if (d->num)
    {
        struct dirent *dirent_ptr;
        dirent_ptr = (struct dirent *)&d->buf[d->cur];
        d->cur += dirent_ptr->d_reclen;
    }

    if (!d->num || d->cur >= d->num)
    {
        /* get a new entry */
        result = dfs_romfs_getdents(fd,
                                    (struct dirent *)d->buf,
                                    sizeof(d->buf) - 1);
        if (result <= 0)
        {


            return NULL;
        }

        d->num = result;
        d->cur = 0; /* current entry index */
    }



    return (struct dirent *)(d->buf + d->cur);
}
/**
 * this function is a POSIX compliant version, which will write specified data
 * buffer length for an open file descriptor.
 *
 * @param fd the file descriptor
 * @param buf the data buffer to be written.
 * @param len the data buffer length.
 *
 * @return the actual written data buffer length.
 */
int write(int fd, const void *buf, size_t len)
{
    (void)fd;
    (void)buf;
    (void)len;
    return 0;
}
int fcntl(int fildes, int cmd, ...)
{
    int ret = -1;
    struct romfs_fd *d;

    /* get the fd */
    d = fd_get(fildes);
    if (d)
    {
        void *arg;
        va_list ap;

        va_start(ap, cmd);
        arg = va_arg(ap, void *);
        va_end(ap);

        ret = dfs_romfs_ioctl(d, cmd, arg);
    }
    else { ret = -EBADF; }

    if (ret < 0)
    {
        ret = -1;
    }

    return ret;
}
/**
 * this function is a POSIX compliant version, which shall perform a variety of
 * control functions on devices.
 *
 * @param fildes the file description
 * @param cmd the specified command
 * @param data represents the additional information that is needed by this
 * specific device to perform the requested function.
 *
 * @return 0 on successful completion. Otherwise, -1 shall be returned and errno
 * set to indicate the error.
 */
int ioctl(int fildes, int cmd, ...)
{
    void *arg;
    va_list ap;

    va_start(ap, cmd);
    arg = va_arg(ap, void *);
    va_end(ap);

    /* we use fcntl for this API. */
    return fcntl(fildes, cmd, arg);
}
/**
 * this function is a POSIX compliant version, which will get file status.
 *
 * @param fildes the file description
 * @param buf the data buffer to save stat description.
 *
 * @return 0 on successful, -1 on failed.
 */
int fstat(int fildes, struct stat *buf)
{
    struct romfs_fd *d;

    /* get the fd */
    d = fd_get(fildes);
    if (d == NULL)
    {

        return -1;
    }

    /* it's the root directory */
    buf->st_dev = 0;

    buf->st_mode = S_IFREG | S_IRUSR | S_IRGRP | S_IROTH |
                   S_IWUSR | S_IWGRP | S_IWOTH;


    buf->st_size    = d->size;
    buf->st_mtime   = 0;


    return 0;
}
/**
 * This function will duplicate a string.
 *
 * @param s the string to be duplicated
 *
 * @return the duplicated string pointer
 */
char *strdup(const char *s)
{
    size_t len = strlen(s) + 1;
    char *tmp = (char *)malloc(len);

    if (!tmp)
    {
        return NULL;
    }

    memcpy(tmp, s, len);

    return tmp;
}

