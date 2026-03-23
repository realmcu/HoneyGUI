#include "../../map_platform.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

static void *posix_malloc(size_t size)
{
    return malloc(size);
}

static void posix_free(void *ptr)
{
    free(ptr);
}

static void *posix_calloc(size_t num, size_t size)
{
    return calloc(num, size);
}

static void *posix_realloc(void *ptr, size_t size)
{
    return realloc(ptr, size);
}

static map_file_t posix_fopen(const char *path, const char *mode)
{
    return (map_file_t)fopen(path, mode);
}

static size_t posix_fread(void *ptr, size_t size, size_t count, map_file_t file)
{
    return fread(ptr, size, count, (FILE *)file);
}

static size_t posix_fwrite(const void *ptr, size_t size, size_t count, map_file_t file)
{
    return fwrite(ptr, size, count, (FILE *)file);
}

static int posix_fseek(map_file_t file, long offset, int origin)
{
    return fseek((FILE *)file, offset, origin);
}

static long posix_ftell(map_file_t file)
{
    return ftell((FILE *)file);
}

static int posix_fclose(map_file_t file)
{
    return fclose((FILE *)file);
}

static const char *log_level_str[] =
{
    "[ERROR]", "[WARN ]", "[INFO ]", "[DEBUG]"
};

static void posix_log(map_log_level_t level, const char *fmt, ...)
{
    va_list args;
    printf("%s ", log_level_str[level]);
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

static uint64_t posix_get_ms(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

static void posix_sleep_ms(uint32_t ms)
{
    usleep(ms * 1000);
}

static map_gps_handle_t posix_gps_init(const char *config)
{
    (void)config;
    return NULL;
}

static int posix_gps_get_position(map_gps_handle_t handle, map_gps_position_t *position,
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

static void posix_gps_deinit(map_gps_handle_t handle)
{
    (void)handle;
}

static bool posix_get_touch_state(map_touch_state_t *state)
{
    if (!state)
    {
        return false;
    }

    state->valid = false;
    return false;
}

static void posix_display_flush(void)
{
}

static const map_platform_ops_t posix_platform_ops =
{
    .mem = {
        .malloc  = posix_malloc,
        .free    = posix_free,
        .calloc  = posix_calloc,
        .realloc = posix_realloc,
    },
    .fs = {
        .open  = posix_fopen,
        .read  = posix_fread,
        .write = posix_fwrite,
        .seek  = posix_fseek,
        .tell  = posix_ftell,
        .close = posix_fclose,
    },
    .log = {
        .log = posix_log,
    },
    .time = {
        .get_ms   = posix_get_ms,
        .sleep_ms = posix_sleep_ms,
    },
    .gps = {
        .init         = posix_gps_init,
        .get_position = posix_gps_get_position,
        .deinit       = posix_gps_deinit,
    },
    .input = {
        .get_touch_state = posix_get_touch_state,
    },
    .display = {
        .flush = posix_display_flush,
    },
};

const map_platform_ops_t *map_platform_posix_get(void)
{
    return &posix_platform_ops;
}
