/**
 * @file map_platform.h
 * @brief Platform Abstraction Layer - Core platform services interface
 *
 * This module provides platform-independent interfaces for:
 * - Memory management (malloc/free/calloc/realloc)
 * - File system operations (open/read/write/seek/close)
 * - Logging (error/warn/info/debug)
 * - Time services (timestamp/sleep)
 * - GPS services (position retrieval)
 *
 * Platform implementations register their function pointers at runtime.
 */

#ifndef MAP_PLATFORM_H
#define MAP_PLATFORM_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * Memory Management Interface
 * ========================================================================= */

/**
 * @brief Memory management operations
 */
typedef struct
{
    void *(*malloc)(size_t size);
    void (*free)(void *ptr);
    void *(*calloc)(size_t num, size_t size);
    void *(*realloc)(void *ptr, size_t size);
} map_mem_ops_t;

/* ============================================================================
 * File System Interface
 * ========================================================================= */

typedef void *map_file_t;

#define MAP_SEEK_SET 0
#define MAP_SEEK_CUR 1
#define MAP_SEEK_END 2

/**
 * @brief File system operations
 */
typedef struct
{
    map_file_t (*open)(const char *path, const char *mode);
    size_t (*read)(void *ptr, size_t size, size_t count, map_file_t file);
    size_t (*write)(const void *ptr, size_t size, size_t count, map_file_t file);
    int (*seek)(map_file_t file, long offset, int origin);
    long (*tell)(map_file_t file);
    int (*close)(map_file_t file);
} map_fs_ops_t;

/* ============================================================================
 * Logging Interface
 * ========================================================================= */

typedef enum
{
    MAP_LOG_ERROR = 0,
    MAP_LOG_WARN  = 1,
    MAP_LOG_INFO  = 2,
    MAP_LOG_DEBUG = 3
} map_log_level_t;

/**
 * @brief Logging operations
 */
typedef struct
{
    void (*log)(map_log_level_t level, const char *fmt, ...);
} map_log_ops_t;

/* ============================================================================
 * Time Interface
 * ========================================================================= */

/**
 * @brief Time operations
 */
typedef struct
{
    uint64_t (*get_ms)(void);
    void (*sleep_ms)(uint32_t ms);
} map_time_ops_t;

/* ============================================================================
 * GPS Interface
 * ========================================================================= */

typedef void *map_gps_handle_t;

/**
 * @brief GPS position data structure
 */
typedef struct
{
    double   lat;
    double   lon;
    float    altitude;
    float    speed;
    float    heading;
    float    accuracy;
    uint64_t timestamp;
    bool     valid;
    uint8_t  satellites;
    float    hdop;
} map_gps_position_t;

/**
 * @brief GPS operations
 */
typedef struct
{
    map_gps_handle_t (*init)(const char *config);
    int (*get_position)(map_gps_handle_t handle, map_gps_position_t *position, uint32_t delta_ms);
    void (*deinit)(map_gps_handle_t handle);
} map_gps_ops_t;

/* ============================================================================
 * Input Interface
 * ========================================================================= */

/**
 * @brief Touch state
 */
typedef struct
{
    int16_t  x;
    int16_t  y;
    bool     pressed;
    bool     valid;
} map_touch_state_t;

/**
 * @brief Input operations
 */
typedef struct
{
    bool (*get_touch_state)(map_touch_state_t *state);
} map_input_ops_t;

/* ============================================================================
 * Display Interface
 * ========================================================================= */

/**
 * @brief Display operations
 */
typedef struct
{
    void (*flush)(void);
} map_display_ops_t;

/* ============================================================================
 * Complete Platform Interface
 * ========================================================================= */

/**
 * @brief Complete platform operations structure
 */
typedef struct
{
    map_mem_ops_t     mem;
    map_fs_ops_t      fs;
    map_log_ops_t     log;
    map_time_ops_t    time;
    map_gps_ops_t     gps;
    map_input_ops_t   input;
    map_display_ops_t display;
} map_platform_ops_t;

/* ============================================================================
 * Platform Registration API
 * ========================================================================= */

/**
 * @brief Register platform implementation
 *
 * Must be called before using any map library functions.
 * Typically called during system initialization.
 *
 * @param ops Platform operations structure
 * @return 0 on success, -1 on failure
 */
int map_platform_register(const map_platform_ops_t *ops);

/**
 * @brief Get registered platform operations
 *
 * @return Pointer to registered platform operations, or NULL if not registered
 */
const map_platform_ops_t *map_platform_get(void);

/**
 * @brief Check if platform is registered
 *
 * @return true if registered, false otherwise
 */
bool map_platform_is_registered(void);

/**
 * @brief Platform-independent logging macros
 */
#define MAP_LOG_ERROR(fmt, ...)    map_platform_get()->log.log(MAP_LOG_ERROR, fmt, ##__VA_ARGS__)
#define MAP_LOG_WARN(fmt, ...)     map_platform_get()->log.log(MAP_LOG_WARN,  fmt, ##__VA_ARGS__)
#define MAP_LOG_INFO(fmt, ...)     map_platform_get()->log.log(MAP_LOG_INFO,  fmt, ##__VA_ARGS__)
#define MAP_LOG_DEBUG(fmt, ...)    map_platform_get()->log.log(MAP_LOG_DEBUG, fmt, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif /* MAP_PLATFORM_H */
