#include "gui_fsal.h"
#include "gui_mem_fs.h"
#include "string.h"
char *get_relative_path(const char *file)
{
    if (strncmp(file, "/mem/", sizeof("/mem/") - 1) == 0)
    {
        return (char *)(file + sizeof("/mem") - 1);
    }
    return NULL;
}
uint32_t get_addr_from_name(const char *file)
{
    GUI_PRINTF("file name = %s\n", file);
    char *relative_path = get_relative_path(file);
    GUI_PRINTF("relative_path name = %s\n", relative_path);
    for (size_t i = 0; i < FILE_TAB_NUM; i++)
    {
        if (strcmp(file_tab[i].file_name, relative_path) == 0)
        {
            return file_tab[i].addr;
        }
    }
    return NULL;
}
int mem_open(struct gui_file_fd *fd, const char *file, int flags)
{
    GUI_PRINTF("enter mem_open\n");
    fd->addr_base =  get_addr_from_name(file);
    return 0;
}
int mem_seek(struct gui_file_fd *fd, int offset, int whence)
{
    GUI_PRINTF("enter mem_seek\n");
    switch (whence)
    {
    case FSAL_SEEK_SET:
        fd->offset = offset;
        break;
    case FSAL_SEEK_CUR:
        fd->offset += offset;
        break;
    case FSAL_SEEK_END:
        GUI_ASSERT(FSAL_SEEK_END);
        break;
    default:
        break;
    }
    return 0;
}
int mem_read(struct gui_file_fd *fd, void *buffer, uint32_t size, uint32_t count)
{
    GUI_PRINTF("enter mem_read\n");
    switch (size)
    {
    case sizeof(uint8_t):
        *(uint8_t *)buffer = *((uint8_t *)(fd->addr_base + fd->offset));
        break;
    case sizeof(uint16_t):
        *(uint8_t *)buffer = *((uint16_t *)(fd->addr_base + fd->offset));
        break;
    default:
        memcpy(buffer, (void *)(fd->addr_base + fd->offset), size);
        break;
    }
    return 0;
}
int mem_write(struct gui_file_fd *fd, const void *buffer, uint32_t size, uint32_t count)
{
    GUI_ASSERT(1);
    return 0;
}
int mem_close(struct gui_file_fd *fd)
{
    fd->offset = 0;
    GUI_FREE(fd);
    return 0;
}
static struct gui_fsal_ops ops =
{
    .open = mem_open,
    .seek = mem_seek,
    .read = mem_read,
    .write = mem_write,
    .close = mem_close,
};

static struct gui_fsal mem_fs =
{
    .name = "mem",
    .ops = &ops,
    .path = "/mem",
};

void mem_fs_init()
{
    GUI_PRINTF("enter mem_fs_init\n");
    fsal_register(&mem_fs);
}
