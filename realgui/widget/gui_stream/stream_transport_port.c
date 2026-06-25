/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/*
 * Porting layer for stream_transport -- thin wrappers over the GUI OS API.
 *
 * The MQ implementation, memory allocation, and logging are all provided by
 * gui_port_os.c (via gui_api_os.h), so this file contains no platform-specific
 * code and no #ifdef for simulator vs RTOS.  To substitute a different backend
 * compile with STP_PORT_CUSTOM defined and provide your own stp_port_*
 * definitions elsewhere.
 */

#include "stream_transport_port.h"

#ifndef STP_PORT_CUSTOM

#include <stdarg.h>
#include <stdio.h>      /* vsnprintf */
#include "gui_api_os.h" /* gui_malloc/calloc/free, gui_mq_*, gui_log */

/* ---- Memory -------------------------------------------------------------- */

void *stp_port_malloc(size_t size)
{
    return gui_malloc(size);
}

void *stp_port_calloc(size_t num, size_t size)
{
    return gui_calloc(num, size);
}

void stp_port_free(void *ptr)
{
    gui_free(ptr);
}

/* ---- Message queue ------------------------------------------------------- */

/* stp_port_mq_create takes void **handle; gui_mq_create takes void *handle.
 * The caller convention is the same (pass &queue_var), so the void ** value
 * is passed as void * through the GUI API and reconstructed by port_mq_create
 * via the *(void **)handle write inside gui_port_os.c. */

bool stp_port_mq_create(void **handle, const char *name,
                        uint32_t msg_size, uint32_t max_msgs)
{
    return gui_mq_create(handle, name, msg_size, max_msgs);
}

bool stp_port_mq_send(void *handle, const void *msg, uint32_t size,
                      uint32_t timeout_ms)
{
    return gui_mq_send(handle, (void *)msg, size, timeout_ms);
}

bool stp_port_mq_recv(void *handle, void *msg, uint32_t size,
                      uint32_t timeout_ms)
{
    return gui_mq_recv(handle, msg, size, timeout_ms);
}

/* ---- Logging ------------------------------------------------------------- */

void stp_port_log(const char *fmt, ...)
{
    char buf[256];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    gui_log("%s", buf);
}

#else  /* STP_PORT_CUSTOM */

/* The integrator supplies stp_port_* elsewhere; keep this translation unit
 * non-empty so strict toolchains don't warn about an empty file. */
typedef int stp_port_default_disabled_t;

#endif /* STP_PORT_CUSTOM */
