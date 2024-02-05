/*
 * File      : gui_server.c
 */

#include <guidef.h>
#include <string.h>
#include <gui_api.h>
#include <stdarg.h>
#if ENABLE_RTK_GUI_OS_HEAP == 0
#include "tlsf.h"
#endif

static struct gui_indev *indev;
static struct gui_os_api *os_api;
static struct gui_dispdev *dc = NULL;
static struct gui_fs *fs = NULL;

void gui_dc_info_register(struct gui_dispdev *info)
{
    dc = info;
}
char gui_dc_get_gpu_type(void)
{
    return dc->gpu_type;
}
unsigned char *gui_dc_get_fb(void)
{
    return dc->frame_buf;
}
void gui_dc_set_gpu_type(char type)
{
    dc->gpu_type = type;
}

void gui_fs_info_register(struct gui_fs *info)
{
    fs = info;
}


#if ENABLE_RTK_GUI_OS_HEAP == 0
static tlsf_t tlsf;
#endif
void gui_os_api_register(struct gui_os_api *info)
{
#if ENABLE_RTK_GUI_OS_HEAP == 0
    tlsf = tlsf_create_with_pool(info->mem_addr, info->mem_size);
#endif
    os_api = info;
}

void gui_indev_info_register(struct gui_indev *info)
{
    indev = info;
}
struct gui_indev *gui_get_indev(void)
{
    return indev;
}


void ext_button_set_indicate(void (*callback)(void))
{
    if (indev->ext_button_indicate)
    {
        indev->ext_button_indicate(callback);
    }
}


struct gui_touch_data *touchpad_get_data(void)
{
    if (indev->tp_get_data)
    {
        return indev->tp_get_data();
    }
    GUI_ASSERT(NULL != NULL);
    return NULL;
}

gui_kb_port_data_t *kb_get_data(void)
{
    if (indev->kb_get_port_data)
    {
        return indev->kb_get_port_data();
    }
    GUI_ASSERT(NULL != NULL);
    return NULL;
}



struct gui_dispdev *gui_get_dc(void)
{
    return dc;
}

float gui_get_scale_x(void)
{
    return dc->scale_x;
}

float gui_get_scale_y(void)
{
    return dc->scale_y;
}

uint32_t gui_get_screen_width(void)
{
    return dc->screen_width;
}
uint32_t gui_get_screen_height(void)
{
    return dc->screen_height;
}
int gui_flash_boost(void)
{
    if (dc->flash_seq_trans_enable)
    {
        return dc->flash_seq_trans_enable();
    }
    return 1;

}
int gui_flash_boost_disable(void)
{
    if (dc->flash_seq_trans_disable)
    {
        return dc->flash_seq_trans_disable();
    }
    return 1;

}

void *gui_thread_create(const char *name, void (*entry)(void *param), void *param,
                        uint32_t stack_size, uint8_t priority)
{
    if (os_api->thread_create)
    {
        return os_api->thread_create(name, entry, param, stack_size, priority);
    }
    return NULL;
}
bool gui_thread_delete(void *handle)
{
    if (os_api->thread_delete)
    {
        return os_api->thread_delete(handle);
    }
    return false;
}

bool gui_thread_suspend(void *handle)
{
    if (os_api->thread_suspend)
    {
        return os_api->thread_suspend(handle);
    }
    return false;
}

bool gui_thread_resume(void *handle)
{
    if (os_api->thread_resume)
    {
        return os_api->thread_resume(handle);
    }
    return false;
}

bool gui_thread_mdelay(uint32_t ms)
{
    if (os_api->thread_create)
    {
        return os_api->thread_mdelay(ms);
    }
    return false;
}
void gui_set_tick_hook(void (*hook)(void))
{
    os_api->gui_tick_hook = hook;
}

bool gui_mq_create(void *handle, const char *name, uint32_t msg_size, uint32_t max_msgs)
{
    if (os_api->mq_create)
    {
        return os_api->mq_create(handle, name, msg_size, max_msgs);
    }
    return NULL;
}

bool gui_mq_send(void *handle, void *buffer, uint32_t size, uint32_t timeout)
{
    if (os_api->mq_send)
    {
        return os_api->mq_send(handle, buffer, size, timeout);
    }
    return false;
}

bool gui_mq_send_urgent(void *handle, void *buffer, uint32_t size, uint32_t timeout)
{
    if (os_api->mq_send_urgent)
    {
        return os_api->mq_send_urgent(handle, buffer, size, timeout);
    }
    return false;
}

bool gui_mq_recv(void *handle, void *buffer, uint32_t size, uint32_t timeout)
{
    if (os_api->mq_recv)
    {
        return os_api->mq_recv(handle, buffer, size, timeout);
    }
    return false;
}


void *gui_malloc(uint32_t n)
{
    void *ptr = NULL;
#if ENABLE_RTK_GUI_OS_HEAP == 1
    if (os_api->f_malloc)
    {
        ptr = os_api->f_malloc(n);
        if (ptr == NULL)
        {
            GUI_ASSERT(NULL != NULL);
        }
        return ptr;
    }
    else
    {
        GUI_ASSERT(NULL != NULL);
        return ptr;
    }
#else
    ptr = tlsf_malloc(tlsf, n);
    if (ptr == NULL)
    {
        GUI_ASSERT(NULL != NULL);
    }
    // if (ptr == (void *)0x00000000009196C8)
    // {
    //     GUI_ASSERT(NULL != NULL);
    // }
    return ptr;
#endif
}

void *gui_realloc(void *ptr_old, uint32_t n)
{
    void *ptr = NULL;
#if ENABLE_RTK_GUI_OS_HEAP == 1
    if (os_api->f_realloc)
    {
        ptr = os_api->f_realloc(ptr_old, n);
        if (ptr == NULL)
        {
            GUI_ASSERT(NULL != NULL);
        }
        return ptr;
    }
    else
    {
        GUI_ASSERT(NULL != NULL);
        return ptr;
    }
#else
    ptr = tlsf_realloc(tlsf, ptr_old, n);
    if (ptr == NULL)
    {
        GUI_ASSERT(NULL != NULL);
    }
    // if (ptr == (void *)0x00000000009196C8)
    // {
    //     GUI_ASSERT(NULL != NULL);
    // }
    return ptr;
#endif
}

void gui_free(void *rmem)
{
#if ENABLE_RTK_GUI_OS_HEAP == 1
    if (os_api->f_free)
    {
        os_api->f_free(rmem);
    }
#else
    tlsf_free(tlsf, rmem);
#endif
}

void gui_mem_debug(void)
{
#if ENABLE_RTK_GUI_OS_HEAP == 1
    gui_log("can't use thie func");
#else
    tlsf_walk_pool(tlsf_get_pool(tlsf), NULL, NULL);
#endif
}

void gui_log(const char *format, ...)
{
    if (os_api->log == NULL)
    {
        return;
    }
    va_list args;
    va_start(args, format);
    char buf[256];
    vsnprintf(buf, sizeof(buf), format, args);
    os_api->log(buf);
    va_end(args);
}

void gui_assert_handler(const char *ex_string, const char *func, uint32_t line)
{
    volatile char dummy = 0;
    gui_log("(%s) assertion failed at function:%s, line number:%d \n", ex_string, func, line);
    while (dummy == 0);
}

/* file api*/
int gui_fs_open(const char *file, int flags)
{
    if (fs)
    {
        if (fs->open)
        {
            return fs->open(file, flags);
        }
    }

    return 0;
}
int gui_fs_close(int d)

{
    if (fs)
    {
        if (fs->close)
        {
            return fs->close(d);
        }
    }

    return 0;
}
int gui_fs_read(int fd, void *buf, size_t len)
{
    if (fs)
    {
        if (fs->read)
        {
            return fs->read(fd, buf, len);
        }
    }

    return 0;
}
int gui_fs_write(int fd, const void *buf, size_t len)
{
    if (fs)
    {
        if (fs->write)
        {
            return fs->write(fd, buf, len);
        }
    }

    return 0;
}
int gui_fs_lseek(int fd, int offset, int whence)
{
    if (fs)
    {
        if (fs->lseek)
        {
            return fs->lseek(fd, offset, whence);
        }
    }

    return 0;
}
int gui_fs_ioctl(int fd, int cmd)
{
    if (fs)
    {
        if (fs->ioctl)
        {
            return fs->ioctl(fd, cmd);
        }
    }

    return 0;
}
int gui_fs_closedir(gui_fs_DIR *d)
{
    if (fs)
    {
        if (fs->closedir)
        {
            return fs->closedir(d);
        }
    }

    return 0;
}
gui_fs_DIR *gui_fs_opendir(const char *name)
{
    if (fs)
    {
        if (fs->opendir)
        {
            return fs->opendir(name);
        }
    }

    return 0;
}
struct gui_fs_dirent *gui_fs_readdir(gui_fs_DIR *d)
{
    if (fs)
    {
        if (fs->readdir)
        {
            return fs->readdir(d);
        }
    }

    return 0;
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


void gui_log_hexdump(const char *name, uint8_t *buf, uint16_t size)
{
    gui_log("gui Hex: %s \n", name);
    for (uint32_t i = 0; i < (size - 1); i++)
    {
        gui_log("0x%x-", buf[i]);
    }
    gui_log("0x%x\n", buf[size - 1]);
}

void gui_display_on(void)
{
    gui_log("gui_display_on \n");
    if (dc->lcd_power_on)
    {
        dc->lcd_power_on();
    }
}

void gui_display_off(void)
{
    gui_log("gui_display_off \n");
    if (dc->lcd_power_off)
    {
        dc->lcd_power_off();
    }
}

uint32_t gui_ms_get(void)
{
    if (os_api->thread_ms_get == NULL)
    {
        return 0;
    }

    return os_api->thread_ms_get();
}

#if defined(_WIN32)
struct file_load_node
{
    struct file_load_node *nxt;
    uint32_t file_id;
    void *mem_addr;
};
typedef struct file_load_node FIEL_LOAD_NODE;

FIEL_LOAD_NODE *fielload_root;

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

static void fileload_insert_node(FIEL_LOAD_NODE *node)
{
    if (node == fielload_root)
    {
        return;
    }
    else
    {
        FIEL_LOAD_NODE *cur = fielload_root;
        FIEL_LOAD_NODE *nxt = fielload_root;
        while (nxt)
        {
            if (nxt->file_id > node->file_id)
            {
                if (nxt == fielload_root)
                {
                    fielload_root = node;
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

static FIEL_LOAD_NODE *fileload_get_node(const char *file)
{
    uint32_t file_id = fileload_get_id(file);
    FIEL_LOAD_NODE *root = fielload_root;
    FIEL_LOAD_NODE *node = NULL;

    if (!root)
    {
        // empty file load list
        fielload_root = (FIEL_LOAD_NODE *)gui_malloc(sizeof(FIEL_LOAD_NODE));
        node = fielload_root;
    }
    else
    {
        root = fielload_root;
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
        node = (FIEL_LOAD_NODE *)gui_malloc(sizeof(FIEL_LOAD_NODE));
    }

    // GUI_ASSERT(node != NULL);
    memset(node, 0, sizeof(FIEL_LOAD_NODE));
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
#if defined(_WIN32)
    {
        gui_log("get file: %s\n", file);
        // check whether file has already been loaded
        FIEL_LOAD_NODE *file_node = fileload_get_node(file);
        if (file_node->mem_addr)
        {
            gui_log(">loaded\n");
            return file_node->mem_addr;
        }

        char *path = gui_malloc(strlen(file) + strlen(GUI_ROOT_FOLDER) + 1);
        sprintf(path, "%s%s", GUI_ROOT_FOLDER, file);
#ifndef O_BINARY
#define O_BINARY 0100000
#endif
        int fd = gui_fs_open(path,  O_BINARY);
        gui_free(path);
        if (fd == -1)
        {
            return NULL;
        }

        int size = gui_fs_lseek(fd, 0, SEEK_END) - gui_fs_lseek(fd, 0, SEEK_SET);
        gui_log(">malloc: %d\n", size);
        void *imgbuf = gui_malloc(size);
        memset(imgbuf, 0, size);
        gui_fs_lseek(fd, 0, SEEK_SET);
        gui_fs_read(fd, imgbuf, size);
        file_node->mem_addr = imgbuf;
        return imgbuf;
    }
#else
    {
        char *path = gui_malloc(strlen(file) + strlen(GUI_ROOT_FOLDER) + 1);
        sprintf(path, "%s%s", GUI_ROOT_FOLDER, file);
        int fd = gui_fs_open(path,  0);
        gui_free(path);
        if (fd == -1)
        {
            gui_log("!!!filename = %s, open fail!\n", path);
            return NULL;
        }
        void *rst;
        rst = (void *)gui_fs_ioctl(fd, 0);
        gui_fs_close(fd);
        gui_log("filename = %s, rst:%p\n", file, rst);
        return rst;

    }
#endif
}

