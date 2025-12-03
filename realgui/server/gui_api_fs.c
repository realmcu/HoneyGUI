/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_api_fs.c
  * @brief Application Programming Interface for UI
  * @details File System
  * @author sienna_shen@realsil.com.cn
  * @date 2025/3/5
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */


/*============================================================================*
 *                        Header Files
 *============================================================================*/

#include "gui_api_fs.h"
#include "gui_api_os.h"

/*============================================================================*
 *                            Macros
 *============================================================================*/


/*============================================================================*
 *                            Variables
 *============================================================================*/

static struct gui_fs *fs = NULL;

/*============================================================================*
 *                           Private Functions
 *============================================================================*/



/*============================================================================*
 *                           Public Functions
 *============================================================================*/


void gui_fs_info_register(struct gui_fs *info)
{
    fs = info;
}

int gui_open(const char *file, int flags)
{
    GUI_ASSERT(fs);
    GUI_ASSERT(fs->open);
    return fs->open(file, flags);
}

int gui_close(int d)
{
    GUI_ASSERT(fs);
    GUI_ASSERT(fs->close);
    return fs->close(d);
}
int gui_read(int fd, void *buf, size_t len)
{
    GUI_ASSERT(fs);
    GUI_ASSERT(fs->read);
    return fs->read(fd, buf, len);
}
int gui_write(int fd, const void *buf, size_t len)
{
    GUI_ASSERT(fs);
    GUI_ASSERT(fs->write);
    return fs->write(fd, buf, len);
}
int gui_lseek(int fd, int offset, int whence)
{
    GUI_ASSERT(fs);
    GUI_ASSERT(fs->lseek);
    return fs->lseek(fd, offset, whence);
}
int gui_ioctl(int fd, int cmd)
{
    GUI_ASSERT(fs);
    GUI_ASSERT(fs->ioctl);
    return fs->ioctl(fd, cmd);
}
int gui_closedir(gui_dir_t *d)
{
    GUI_ASSERT(fs);
    GUI_ASSERT(fs->closedir);
    return fs->closedir(d);
}
gui_dir_t *gui_opendir(const char *name)
{
    GUI_ASSERT(fs);
    GUI_ASSERT(fs->opendir);
    return fs->opendir(name);
}
struct gui_fs_dirent *gui_readdir(gui_dir_t *d)
{
    GUI_ASSERT(fs);
    GUI_ASSERT(fs->readdir);
    return fs->readdir(d);
}

void gui_fstat(int fildes, gui_fstat_t *buf)
{
    GUI_ASSERT(fs);
    GUI_ASSERT(fs->fstat);
    fs->fstat(fildes, buf);
}



void *gui_get_file_address(const char *file)
{
    if (file == NULL)
    {
        return NULL;
    }

    int fd = gui_open(file,  0);
    if (fd <= 0)
    {
        gui_log("!!!filename = %s, open fail!\n", file);
        return NULL;
    }
    void *rst;
    rst = (void *)(intptr_t)gui_ioctl(fd, 0);
    gui_close(fd);
    gui_log("filename = %s, rst:%p\n", file, rst);
    return rst;

}
