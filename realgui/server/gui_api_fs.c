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

#ifndef _GUI_FS_ROOT_DIR
#define _GUI_FS_ROOT_DIR "realgui\\example\\screen_480_480\\root\\"
#endif

/*============================================================================*
 *                            Variables
 *============================================================================*/

static struct gui_fs *fs = NULL;
char *defaultPath = _GUI_FS_ROOT_DIR;
static char *buffer;

/*============================================================================*
 *                           Private Functions
 *============================================================================*/



/*============================================================================*
 *                           Public Functions
 *============================================================================*/


void gui_change_default_path(int argc, char **argv)
{
    for (int count = 1; count < argc; count++)
    {
        if (!strcmp(argv[count], "-p") && count + 1 < argc)
        {
            char *path = malloc(strlen(argv[count + 1]) + strlen("\\") + 1);
            if (path == NULL)
            {
                gui_log("Memory allocation failed\n");
                return;
            }
            sprintf(path, "%s%s", argv[count + 1], "\\");

            if (defaultPath != NULL)
            {
                // free(defaultPath);
            }
            defaultPath = path;
        }
    }
    GUI_UNUSED(buffer);
}


void gui_fs_info_register(struct gui_fs *info)
{
    fs = info;
}

int gui_fs_open(const char *file, int flags)
{
    GUI_ASSERT(fs);
    GUI_ASSERT(fs->open);
    return fs->open(file, flags);
}

int gui_fs_close(int d)
{
    GUI_ASSERT(fs);
    GUI_ASSERT(fs->close);
    return fs->close(d);
}
int gui_fs_read(int fd, void *buf, size_t len)
{
    GUI_ASSERT(fs);
    GUI_ASSERT(fs->read);
    return fs->read(fd, buf, len);
}
int gui_fs_write(int fd, const void *buf, size_t len)
{
    GUI_ASSERT(fs);
    GUI_ASSERT(fs->write);
    return fs->write(fd, buf, len);
}
int gui_fs_lseek(int fd, int offset, int whence)
{
    GUI_ASSERT(fs);
    GUI_ASSERT(fs->lseek);
    return fs->lseek(fd, offset, whence);
}
int gui_fs_ioctl(int fd, int cmd)
{
    GUI_ASSERT(fs);
    GUI_ASSERT(fs->ioctl);
    return fs->ioctl(fd, cmd);
}
int gui_fs_closedir(gui_fs_dir *d)
{
    GUI_ASSERT(fs);
    GUI_ASSERT(fs->closedir);
    return fs->closedir(d);
}
gui_fs_dir *gui_fs_opendir(const char *name)
{
    GUI_ASSERT(fs);
    GUI_ASSERT(fs->opendir);
    return fs->opendir(name);
}
struct gui_fs_dirent *gui_fs_readdir(gui_fs_dir *d)
{
    GUI_ASSERT(fs);
    GUI_ASSERT(fs->readdir);
    return fs->readdir(d);
}

void gui_fs_fstat(int fildes, gui_fs_stat_t *buf)
{
    GUI_ASSERT(fs);
    GUI_ASSERT(fs->fstat);
    fs->fstat(fildes, buf);
}

char *gui_strdup(const char *s)
{
    size_t len = strlen(s) + 1;
    char *tmp = (char *)gui_malloc(len);

    if (!tmp)
    {
        return NULL;
    }

    memcpy(tmp, s, len);

    return tmp;
}

char *gui_filepath_transforming(void *addr)
{
    // simulator on WIN32: address transforming and check existence
    char *path = gui_malloc(strlen(addr) + strlen(GUI_ROOT_FOLDER) + 1);
    GUI_ASSERT(path != NULL);
    sprintf(path, "%s%s", GUI_ROOT_FOLDER, (char *)addr);
    // check existence
    FILE *fd = fopen(path, "r");
    if (!fd)
    {
        gui_log("file not exist\n");
        gui_free(path);
        return NULL;
    }
    fclose(fd);
    return path;
}
const char *gui_get_path_by_relative(const char *relative_path)
{
    char *path = gui_malloc(strlen(relative_path) + strlen(GUI_ROOT_FOLDER) + 1);
    GUI_ASSERT(path != NULL);
    sprintf(path, "%s%s", GUI_ROOT_FOLDER, (char *)relative_path);
    return path;
}

#if defined(_WIN32)
struct file_load_node
{
    struct file_load_node *nxt;
    uint32_t file_id;
    void *mem_addr;
};
typedef struct file_load_node FILE_LOAD_NODE;

FILE_LOAD_NODE *fileload_root;

static uint32_t fileload_get_id(const char *str)
{
    uint32_t seed = 131;
    uint32_t id = 0;

    while (*str != '\0')
    {
        id = id * seed + *str;
        str++;
    }
    return id;
}

static void fileload_insert_node(FILE_LOAD_NODE *node)
{
    if (node == fileload_root)
    {
        return;
    }
    else
    {
        FILE_LOAD_NODE *cur = fileload_root;
        FILE_LOAD_NODE *nxt = fileload_root;
        while (nxt)
        {
            if (nxt->file_id > node->file_id)
            {
                if (nxt == fileload_root)
                {
                    fileload_root = node;
                }
                else
                {
                    cur->nxt = node;
                }
                node->nxt = nxt;
                return;
            }
            cur = nxt;
            nxt = cur->nxt;
        }
        cur->nxt = node;
        return;
    }
}

static FILE_LOAD_NODE *fileload_get_node(const char *file)
{
    uint32_t file_id = fileload_get_id(file);
    FILE_LOAD_NODE *root = fileload_root;
    FILE_LOAD_NODE *node = NULL;

    if (!root)
    {
        // empty file load list
        fileload_root = (FILE_LOAD_NODE *)gui_malloc(sizeof(FILE_LOAD_NODE));
        node = fileload_root;
    }
    else
    {
        root = fileload_root;
        node = root;
        // search node by file_id
        while (node && (node->file_id < file_id))
        {
            node = node->nxt;
        }
        if (node && node->file_id == file_id)
        {
            return node;
        }
        node = (FILE_LOAD_NODE *)gui_malloc(sizeof(FILE_LOAD_NODE));
    }

    // GUI_ASSERT(node != NULL);
    memset(node, 0, sizeof(FILE_LOAD_NODE));
    node->file_id = file_id;
    fileload_insert_node(node);
    return node;
}
#endif

void *gui_get_file_address(const char *file)
{
    if (file == NULL)
    {
        return NULL;
    }
    char *root_folder = GUI_ROOT_FOLDER;
#if defined(_WIN32)
    {
        // gui_log("get file: %s\n", file);
        // check whether file has already been loaded
        FILE_LOAD_NODE *file_node = fileload_get_node(file);
        if (file_node->mem_addr)
        {
            //gui_log(">loaded before\n");
            return file_node->mem_addr;
        }

        char *path = gui_malloc(strlen(file) + strlen(root_folder) + 1);
        sprintf(path, "%s%s", root_folder, file);
        int fd = gui_fs_open(path,  GUI_FS_O_BINARY);

        if (fd <= 0)
        {
            gui_log("open file failed: %s\n", path);
            gui_free(path);
            return NULL;
        }
        gui_free(path);
        int size = gui_fs_lseek(fd, 0, SEEK_END) - gui_fs_lseek(fd, 0, SEEK_SET);
        {
            static uint32_t used = 0;
            used += size;
            //gui_log(">malloc: %d -- used: %d\n", size, used);
        }
        if (size == 0)
        {
            gui_fs_close(fd);
            return 0;
        }

        void *imgbuf = gui_malloc(size);
        memset(imgbuf, 0, size);
        gui_fs_lseek(fd, 0, SEEK_SET);
        gui_fs_read(fd, imgbuf, size);
        gui_fs_close(fd);
        file_node->mem_addr = imgbuf;
        return imgbuf;
    }
#else
    {
        char *path = gui_malloc(strlen(file) + strlen(root_folder) + 1);
        sprintf(path, "%s%s", root_folder, file);
        int fd = gui_fs_open(path,  0);
        gui_free(path);
        if (fd <= 0)
        {
            gui_log("!!!filename = %s, open fail!\n", path);
            return NULL;
        }
        void *rst;
        rst = (void *)(intptr_t)gui_fs_ioctl(fd, 0);
        gui_fs_close(fd);
        gui_log("filename = %s, rst:%p\n", file, rst);
        return rst;

    }
#endif
}
