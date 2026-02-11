/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_API_OS_H__
#define __GUI_API_OS_H__
#ifdef __cplusplus
extern "C" {
#endif


/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "guidef.h"
#include "tlsf.h"

/*============================================================================*
 *                         Macros
 *============================================================================*/

#define GUI_ASSERT(EX)                                                         \
    if (!(EX))                                                                    \
    {                                                                             \
        gui_assert_handler(#EX, __FUNCTION__, __LINE__);                           \
    } //!< assert here
#define GUI_LINE(EX)                                                         \
    if ((EX))                                                                    \
    {                                                                             \
        gui_log("\033[1;31m");   \
        gui_log("[GUI] debug at function:%s, line number:%d \n", __FUNCTION__, __LINE__); \
        gui_log("\033[0m");   \
    } //!< print line number
#define __param_name(line) param_##line
#define _param_name(line) __param_name(line)
#define param_name() _param_name(__LINE__)

#define PRINT_MATRIX(matrix) gui_log("-----------------MATRIX------------------------\n"); \
    gui_log("%f    ",matrix->m[0][0]); \
    gui_log("%f    ",matrix->m[0][1]); \
    gui_log("%f  \n",matrix->m[0][2]); \
    gui_log("%f    ",matrix->m[1][0]); \
    gui_log("%f    ",matrix->m[1][1]); \
    gui_log("%f  \n",matrix->m[1][2]); \
    gui_log("%f    ",matrix->m[2][0]); \
    gui_log("%f    ",matrix->m[2][1]); \
    gui_log("%f  \n",matrix->m[2][2]); \
    gui_log("-----------------------------------------------\n");  //!< print matrix(3*3)

#define PRINT_POX(pox)       gui_log("-----------------POX---------------------------\n"); \
    gui_log("%f  \n",(pox)->p[0]); \
    gui_log("%f  \n",(pox)->p[1]); \
    gui_log("%f  \n",(pox)->p[2]); \
    gui_log("-----------------------------------------------\n"); //!< print position


/*============================================================================*
 *                         Functions
 *============================================================================*/

void gui_os_api_register(gui_os_api_t *info);

tlsf_t gui_get_tlsf(void);

void *gui_thread_create(const char *name, void (*entry)(void *param), void *param,
                        uint32_t stack_size, uint8_t priority);

bool gui_thread_delete(void *handle);

bool gui_thread_mdelay(uint32_t ms);

void gui_set_tick_hook(void (*hook)(void));

bool gui_thread_suspend(void *handle);

bool gui_thread_resume(void *handle);

bool gui_mq_create(void *handle, const char *name, uint32_t msg_size, uint32_t max_msgs);

bool gui_mq_send(void *handle, void *buffer, uint32_t size, uint32_t timeout);

bool gui_mq_send_urgent(void *handle, void *buffer, uint32_t size, uint32_t timeout);

bool gui_mq_recv(void *handle, void *buffer, uint32_t size, uint32_t timeout);

void *gui_malloc(size_t n);
void *gui_calloc(size_t num, size_t size);
void *gui_realloc(void *ptr_old, size_t n);
void gui_free(void *rmem);
void gui_mem_debug(void);
char *gui_strdup(const char *s);

void *gui_lower_malloc(size_t n);
void *gui_lower_realloc(void *ptr_old, size_t n);
void gui_lower_free(void *rmem);
void gui_lowr_mem_debug(void);
void gui_sleep_cb(void);

void gui_log(const char *format, ...);
void gui_log_hexdump(const char *name, uint8_t *buf, uint16_t size);

#if defined(__GNUC__) || defined(__clang__)
#define GUI_NORETURN __attribute__((noreturn))
#elif defined(_MSC_VER)
#define GUI_NORETURN __declspec(noreturn)
#else
#define GUI_NORETURN
#endif

GUI_NORETURN void gui_assert_handler(const char *ex_string, const char *func, uint32_t line);


uint32_t gui_ms_get(void);
uint32_t gui_us_get(void);

/**
 * @brief Get ram used in byte.
 *
 * @return uint32_t
 */
uint32_t gui_mem_used(void);
/**
 * @brief Get low ram (another ram) used in byte.
 *
 * @return uint32_t
 */
uint32_t gui_low_mem_used(void);

#ifdef __cplusplus
}
#endif
#endif
