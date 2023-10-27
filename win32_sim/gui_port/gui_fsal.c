#include "gui_fsal.h"
#include "string.h"
gui_list_t fs_list;
struct gui_fsal *fs_tab[10];
uint8_t fs_count = 0;

void fsal_register(struct gui_fsal *fs)
{
    fs_tab[fs_count++] = fs;
}

static struct gui_fsal *fsal_match_fs(const char *file)
{
    GUI_PRINTF("enter match\n");
    for (uint8_t i = 0; i < 10; i++)
    {
        struct gui_fsal *fs = fs_tab[i];
        if (fs != NULL)
        {
            GUI_PRINTF("fs name = %s\n", fs->name);
            if (strncmp(fs->path, file, sizeof(fs->name) - 1) == 0)
            {
                GUI_PRINTF("fs path = %s\n", fs->path);
                return fs;
            }
        }
    }
    return NULL;
}

int fsal_open(struct gui_file_fd *fd, const char *file, int flags)
{
    fd->fs = fsal_match_fs(file);
    fd->offset = 0;
    fd->fs->ops->open(fd, file, 0);
    return 0;
}
int fsal_seek(struct gui_file_fd *fd, int offset, int whence)
{
    return fd->fs->ops->seek(fd, offset, whence);
}
int fsal_read(struct gui_file_fd *fd, void *buffer, size_t len)
{
    return fd->fs->ops->read(fd, buffer, len, 0);
}
int fsal_write(struct gui_file_fd *fd, void *buffer, size_t len)
{
    return fd->fs->ops->write(fd, buffer, len, 0);
}
int fsal_close(struct gui_file_fd *fd)
{
    return fd->fs->ops->close(fd);
}
int fsal_open2()
{
    struct gui_file_fd fd;
    GUI_PRINTF("enter open2\n");
    fsal_open(&fd, "/mem/clock/calc.bin", 0);
    fsal_seek(&fd, 100, FSAL_SEEK_SET);
    GUI_PRINTF("addr = %d,offset = %d,", fd.addr_base, fd.offset);
    return 0;
}
extern void mem_fs_init(void);
/*call this func to use fsal*/
void fsal_init()
{
    GUI_PRINTF("enter fsal_init\n");
    mem_fs_init();
}
