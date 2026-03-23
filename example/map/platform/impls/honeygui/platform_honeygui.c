#include "../../map_platform.h"
#include "gui_api_os.h"
#include "gui_vfs.h"
#include "tp_algo.h"
#include <stdarg.h>
#include <stdio.h>

static void *honeygui_malloc(size_t size)
{
    return gui_lower_malloc(size);
}

static void honeygui_free(void *ptr)
{
    gui_lower_free(ptr);
}

static void *honeygui_calloc(size_t num, size_t size)
{
    return gui_lower_calloc(num, size);
}

static void *honeygui_realloc(void *ptr, size_t size)
{
    return gui_lower_realloc(ptr, size);
}

static map_file_t honeygui_fopen(const char *path, const char *mode)
{
    gui_vfs_mode_t vfs_mode = 0;

    if (mode && mode[0] == 'r')
    {
        vfs_mode = GUI_VFS_READ;
    }
    else if (mode && mode[0] == 'w')
    {
        vfs_mode = GUI_VFS_WRITE | GUI_VFS_CREATE;
    }
    else if (mode && mode[0] == 'a')
    {
        vfs_mode = GUI_VFS_WRITE | GUI_VFS_APPEND;
    }

    return (map_file_t)gui_vfs_open(path, vfs_mode);
}

static size_t honeygui_fread(void *ptr, size_t size, size_t count, map_file_t file)
{
    if (!file || !ptr)
    {
        return 0;
    }

    int bytes_read = gui_vfs_read((gui_vfs_file_t *)file, ptr, size * count);
    if (bytes_read < 0)
    {
        return 0;
    }

    return (size_t)bytes_read / size;
}

static size_t honeygui_fwrite(const void *ptr, size_t size, size_t count, map_file_t file)
{
    if (!file || !ptr)
    {
        return 0;
    }

    int bytes_written = gui_vfs_write((gui_vfs_file_t *)file, ptr, size * count);
    if (bytes_written < 0)
    {
        return 0;
    }

    return (size_t)bytes_written / size;
}

static int honeygui_fseek(map_file_t file, long offset, int origin)
{
    if (!file)
    {
        return -1;
    }

    gui_vfs_seek_t vfs_whence;
    switch (origin)
    {
    case 0:
        vfs_whence = GUI_VFS_SEEK_SET;
        break;
    case 1:
        vfs_whence = GUI_VFS_SEEK_CUR;
        break;
    case 2:
        vfs_whence = GUI_VFS_SEEK_END;
        break;
    default:
        return -1;
    }

    return gui_vfs_seek((gui_vfs_file_t *)file, (int)offset, vfs_whence);
}

static long honeygui_ftell(map_file_t file)
{
    if (!file)
    {
        return -1;
    }

    return (long)gui_vfs_tell((gui_vfs_file_t *)file);
}

static int honeygui_fclose(map_file_t file)
{
    if (!file)
    {
        return -1;
    }

    return gui_vfs_close((gui_vfs_file_t *)file);
}

static void honeygui_log(map_log_level_t level, const char *fmt, ...)
{
    (void)level;

    va_list args;
    va_start(args, fmt);

    char buffer[256];
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    gui_log(buffer);

    va_end(args);
}

static uint64_t honeygui_get_ms(void)
{
    return (uint64_t)gui_ms_get();
}

static void honeygui_sleep_ms(uint32_t ms)
{
    gui_thread_mdelay(ms);
}

static map_gps_handle_t honeygui_gps_init(const char *config)
{
    (void)config;
    return NULL;
}

static int honeygui_gps_get_position(map_gps_handle_t handle, map_gps_position_t *position,
                                     uint32_t delta_ms)
{
    (void)delta_ms;

    if (!handle || !position)
    {
        return -1;
    }

    position->valid = false;
    return -1;
}

static void honeygui_gps_deinit(map_gps_handle_t handle)
{
    (void)handle;
}

static bool honeygui_get_touch_state(map_touch_state_t *state)
{
    if (!state)
    {
        return false;
    }

    touch_info_t *tp = tp_get_info();
    if (!tp)
    {
        state->valid = false;
        return false;
    }

    state->x = tp->deltaX;
    state->y = tp->deltaY;
    state->pressed = tp->pressed || tp->pressing;
    state->valid = true;

    static int log_counter = 0;
    if (state->pressed && (++log_counter % 10 == 0))
    {
        gui_log("[PLATFORM] Touch: deltaX=%d, deltaY=%d, pressed=%d, pressing=%d\n",
                tp->deltaX, tp->deltaY, tp->pressed, tp->pressing);
    }

    return true;
}

static void honeygui_display_flush(void)
{
    extern void gui_fb_change(void);
    gui_fb_change();
}

static const map_platform_ops_t honeygui_platform_ops =
{
    .mem = {
        .malloc  = honeygui_malloc,
        .free    = honeygui_free,
        .calloc  = honeygui_calloc,
        .realloc = honeygui_realloc,
    },
    .fs = {
        .open  = honeygui_fopen,
        .read  = honeygui_fread,
        .write = honeygui_fwrite,
        .seek  = honeygui_fseek,
        .tell  = honeygui_ftell,
        .close = honeygui_fclose,
    },
    .log = {
        .log = honeygui_log,
    },
    .time = {
        .get_ms   = honeygui_get_ms,
        .sleep_ms = honeygui_sleep_ms,
    },
    .gps = {
        .init         = honeygui_gps_init,
        .get_position = honeygui_gps_get_position,
        .deinit       = honeygui_gps_deinit,
    },
    .input = {
        .get_touch_state = honeygui_get_touch_state,
    },
    .display = {
        .flush = honeygui_display_flush,
    },
};

const map_platform_ops_t *map_platform_honeygui_get(void)
{
    return &honeygui_platform_ops;
}
