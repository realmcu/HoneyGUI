/*
 * File      : gui_server.c
 */

#include <guidef.h>
#include <string.h>
#include <gui_api.h>
#include <stdarg.h>
#include "tlsf.h"

static struct gui_indev *indev = NULL;
static struct gui_os_api *os_api = NULL;
static struct gui_dispdev *dc = NULL;
static struct gui_fs *fs = NULL;
static struct gui_ftl *ftl = NULL;
static struct acc_engine *acc = NULL;

void gui_dc_info_register(struct gui_dispdev *info)
{
    dc = info;

    if (dc->fb_height == dc->screen_height)
    {
        dc->pfb_type = PFB_X_DIRECTION;
        uint32_t last_section = (dc->screen_width % dc->fb_width) ? 1 : 0;
        dc->section_total = dc->screen_width / dc->fb_width + last_section;
    }
    else if (dc->fb_width == dc->screen_width)
    {
        dc->pfb_type = PFB_Y_DIRECTION;
        uint32_t last_section = (dc->screen_height % dc->fb_height) ? 1 : 0;
        dc->section_total = dc->screen_height / dc->fb_height + last_section;
    }
    else
    {
        GUI_ASSERT(NULL != NULL);
    }
}
void gui_acc_info_register(struct acc_engine *info)
{
    acc = info;
}

void gui_fs_info_register(struct gui_fs *info)
{
    fs = info;
}

void gui_ftl_info_register(struct gui_ftl *info)
{
    ftl = info;
}

void gui_indev_info_register(struct gui_indev *info)
{
    indev = info;
}

struct gui_dispdev *gui_get_dc(void)
{
    return dc;
}

struct acc_engine *gui_get_acc(void)
{
    return acc;
}
struct gui_indev *gui_get_indev(void)
{
    return indev;
}


static tlsf_t tlsf = NULL;
static tlsf_t lower_tlsf = NULL;

void gui_os_api_register(gui_os_api_t *info)
{

    if ((info->mem_addr != NULL) && (info->mem_size != 0))
    {
        tlsf = tlsf_create_with_pool(info->mem_addr, info->mem_size);
    }


    if ((info->lower_mem_addr != NULL) && (info->lower_mem_size != 0))
    {
        lower_tlsf = tlsf_create_with_pool(info->lower_mem_addr, info->lower_mem_size);
    }

    os_api = info;
}





void ext_button_set_indicate(void (*callback)(void))
{
    if (indev->ext_button_indicate)
    {
        indev->ext_button_indicate(callback);
    }
}


gui_touch_port_data_t *touchpad_get_data(void)
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

gui_wheel_port_data_t *wheel_get_data(void)
{
    if (indev->wheel_get_port_data)
    {
        return indev->wheel_get_port_data();
    }
    GUI_ASSERT(NULL != NULL);
    return NULL;
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


void *gui_malloc(size_t n)
{
    void *ptr = NULL;
    if ((n > os_api->mem_threshold_size) && (os_api->mem_threshold_size != 0))
    {
        ptr = gui_lower_malloc(n);
        return ptr;
    }
    if (tlsf != NULL)
    {
        ptr = tlsf_malloc(tlsf, n);
    }
    else
    {
        GUI_ASSERT(os_api->f_malloc != NULL);
        ptr = os_api->f_malloc(n);
    }
    if (ptr == NULL)
    {
        ptr = gui_lower_malloc(n);
    }
    // if (ptr == (void *)0x0000000000749D30)
    // {
    //     GUI_ASSERT(NULL != NULL);
    // }
    return ptr;
}

void *gui_realloc(void *ptr_old, size_t n)
{
    void *ptr = NULL;

    if (tlsf == NULL)
    {
        GUI_ASSERT(os_api->f_realloc != NULL);
        ptr = os_api->f_realloc(ptr_old, n);
        if (ptr == NULL)
        {
            return NULL;
        }
        return ptr;
    }
    else
    {
        if (
            (os_api->lower_mem_size != 0) && \
            ((uint32_t)(uintptr_t)ptr_old >= (uint32_t)(uintptr_t)os_api->lower_mem_addr) && \
            ((uint32_t)(uintptr_t)ptr_old <= (uint32_t)(uintptr_t)os_api->lower_mem_addr +
             (uint32_t)os_api->lower_mem_size)
        )
        {
            ptr = tlsf_realloc(lower_tlsf, ptr_old, n);
            return ptr;
        }
        else
        {
            ptr = tlsf_realloc(tlsf, ptr_old, n);
            if (ptr == NULL)
            {
                ptr = tlsf_malloc(lower_tlsf, n);
                memcpy(ptr, ptr_old, tlsf_block_size(ptr_old));
                tlsf_free(tlsf, ptr_old);
            }
            // if (ptr == (void *)0x0000000000749D30)
            // {
            //     GUI_ASSERT(NULL != NULL);
            // }
            return ptr;
        }
    }

}

void gui_free(void *rmem)
{

    if ((rmem != NULL) && \
        ((uint32_t)(uintptr_t)rmem >= (uint32_t)(uintptr_t)os_api->lower_mem_addr) && \
        ((uint32_t)(uintptr_t)rmem <= (uint32_t)(uintptr_t)os_api->lower_mem_addr +
         (uint32_t)os_api->lower_mem_size)
       )
    {
        GUI_ASSERT((uint32_t)(uintptr_t)os_api->lower_mem_addr != 0);
        GUI_ASSERT((uint32_t)(uintptr_t)os_api->lower_mem_size != 0);
        gui_lower_free(rmem);
        return;
    }
    if (tlsf != NULL)
    {
        tlsf_free(tlsf, rmem);
    }
    else
    {
        GUI_ASSERT(os_api->f_free != NULL);
        os_api->f_free(rmem);
    }
}

static uint32_t total_used_size = 0;
static void gui_walker(void *ptr, size_t size, int used, void *user)
{
    if (used)
    {
        total_used_size = total_used_size + size;
    }

    gui_log("\t%p %s size: %x; total = %d\n", ptr, used ? "used" : "free", (unsigned int)size,
            total_used_size);
}

void gui_mem_debug(void)
{
    total_used_size = 0;
    GUI_UNUSED(total_used_size);
    GUI_ASSERT(tlsf != NULL);
    gui_log("\t\n");
    tlsf_walk_pool(tlsf_get_pool(tlsf), gui_walker, &total_used_size);
    gui_log("\t\n");
    total_used_size = 0;
}


void *gui_lower_malloc(size_t n)
{
    void *ptr = NULL;
    GUI_ASSERT(lower_tlsf != NULL);
    ptr = tlsf_malloc(lower_tlsf, n);
    if (ptr == NULL)
    {
        GUI_ASSERT(NULL != NULL);
    }
    return ptr;
}

void *gui_lower_realloc(void *ptr_old, size_t n)
{
    void *ptr = NULL;
    GUI_ASSERT(lower_tlsf != NULL);
    ptr = tlsf_realloc(lower_tlsf, ptr_old, n);
    if (ptr == NULL)
    {
        GUI_ASSERT(NULL != NULL);
    }
    return ptr;
}

void gui_lower_free(void *rmem)
{
    GUI_ASSERT(lower_tlsf != NULL);
    tlsf_free(lower_tlsf, rmem);
}



void gui_lower_mem_debug(void)
{
    GUI_ASSERT(lower_tlsf != NULL);
    tlsf_walk_pool(tlsf_get_pool(lower_tlsf), NULL, NULL);
}

void gui_sleep_cb(void)
{
    if (os_api->gui_sleep_cb != NULL)
    {
        os_api->gui_sleep_cb();
    }
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
    gui_log("\033[1;31m");
    gui_log("(%s) assertion failed at function:%s, line number:%d \n", ex_string, func, line);
    gui_log("\033[0m");
#ifndef _WIN32
    *(volatile uint32_t *)0xFFFFFFFF = 0;
#endif
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
int gui_fs_closedir(gui_fs_dir *d)
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
gui_fs_dir *gui_fs_opendir(const char *name)
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
struct gui_fs_dirent *gui_fs_readdir(gui_fs_dir *d)
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

int gui_ftl_read(uint32_t addr, uint8_t *buf, uint32_t len)
{
    if (ftl->read)
    {
        ftl->read(addr, buf, len);
        return len;
    }
    else
    {
        return 0;
    }
}
int gui_ftl_write(uint32_t addr, const uint8_t *buf, uint32_t len)
{
    if (ftl->write)
    {
        ftl->write(addr, buf, len);
        return len;
    }
    else
    {
        return 0;
    }
}
int gui_ftl_erase(uint32_t addr, uint32_t len)
{
    if (ftl->erase)
    {
        ftl->erase(addr, len);
    }
    return 0;
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

uint32_t gui_us_get(void)
{
    if (os_api->thread_us_get == NULL)
    {
        return 0;
    }

    return os_api->thread_us_get();
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
    char *root_folder = GUI_ROOT_FOLDER;
#ifdef ENABLE_RTK_GUI_WATCHFACE_UPDATE
#ifdef _WIN32
    root_folder = "gui_engine\\example\\screen_448_368\\root_image_hongkong\\watch_face_update\\";
#endif
#endif
#if defined(_WIN32)
    {
        // gui_log("get file: %s\n", file);
        // check whether file has already been loaded
        FIEL_LOAD_NODE *file_node = fileload_get_node(file);
        if (file_node->mem_addr)
        {
            gui_log(">loaded before\n");
            return file_node->mem_addr;
        }

        char *path = gui_malloc(strlen(file) + strlen(root_folder) + 1);
        sprintf(path, "%s%s", root_folder, file);
#ifndef O_BINARY
#define O_BINARY 0100000
#endif
        int fd = gui_fs_open(path,  O_BINARY);

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
        void *imgbuf = gui_malloc(size);
        memset(imgbuf, 0, size);
        gui_fs_lseek(fd, 0, SEEK_SET);
        gui_fs_read(fd, imgbuf, size);
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
        rst = (void *)gui_fs_ioctl(fd, 0);
        gui_fs_close(fd);
        gui_log("filename = %s, rst:%p\n", file, rst);
        return rst;

    }
#endif
}

gui_color_t gui_rgba(unsigned char red,
                     unsigned char green,
                     unsigned char blue,
                     unsigned char opacity)
{
    gui_color_t color;

    // Use longer initialization to suppress warning.
    color.color.rgba.r = red;
    color.color.rgba.g = green;
    color.color.rgba.b = blue;
    color.color.rgba.a = opacity;

    return color;
}

gui_color_t gui_rgb(unsigned char red, unsigned char green, unsigned char blue)
{
    gui_color_t color;

    // Use longer initialization to suppress warning.
    color.color.rgba.r = red;
    color.color.rgba.g = green;
    color.color.rgba.b = blue;
    color.color.rgba.a = UINT8_MAX;

    return color;
}
