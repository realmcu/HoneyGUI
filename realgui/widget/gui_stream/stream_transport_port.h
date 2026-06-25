/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/**
 * @file    stream_transport_port.h
 * @brief   Porting seam for the stream_transport core.
 *
 * The transport core (stream_transport.c) calls ONLY the stp_port_* primitives
 * declared here, so it carries no platform dependency of its own and a producer
 * may create its transport BEFORE the GUI task is up.
 *
 * A ready-made implementation lives in stream_transport_port.c, which selects
 * its backend automatically at compile time:
 *
 *   - PC simulator (_HONEYGUI_SIMULATOR_ defined): self-contained POSIX /
 *     pthread (per-queue mutex + condvar ring buffer, libc malloc, printf log).
 *     It does NOT depend on HoneyGUI's OS abstraction, so the shared gui port
 *     stays untouched.
 *
 *   - ARM / RTOS target (default): forwards straight to the RTK OS primitives
 *     (malloc / os_msg_* / DBG_DIRECT), mirroring the board port in
 *     board/evb/eBadge/src/ports/realgui_port/gui_port_os.c.
 *
 * To target a different RTOS, either adjust the ARM branch of
 * stream_transport_port.c, or compile that file with STP_PORT_CUSTOM defined
 * and provide your own stp_port_* definitions elsewhere.
 */

#ifndef STREAM_TRANSPORT_PORT_H
#define STREAM_TRANSPORT_PORT_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__GNUC__) || defined(__clang__)
#define STP_PORT_PRINTF_FMT __attribute__((format(printf, 1, 2)))
#else
#define STP_PORT_PRINTF_FMT
#endif

/* ---- memory ------------------------------------------------------------- */

/** Allocate @p size bytes.  Returns NULL on failure. */
void *stp_port_malloc(size_t size);

/** Allocate @p num * @p size zero-initialised bytes.  Returns NULL on failure. */
void *stp_port_calloc(size_t num, size_t size);

/** Free a block from stp_port_malloc / stp_port_calloc.  NULL is a no-op. */
void stp_port_free(void *ptr);

/* ---- message queue (single-producer / single-consumer) ------------------ *
 * Fixed message size fixed at creation; send/recv copy exactly one slot.
 * timeout_ms: 0 = poll (non-blocking), 0xFFFFFFFF = wait forever.            */

/**
 * Create a queue holding up to @p max_msgs messages of @p msg_size bytes each.
 * On success writes the opaque handle to *@p handle and returns true.
 */
bool stp_port_mq_create(void **handle, const char *name, uint32_t msg_size, uint32_t max_msgs);

/** Copy @p size bytes from @p msg into the queue.  Returns false on timeout/full. */
bool stp_port_mq_send(void *handle, const void *msg, uint32_t size, uint32_t timeout_ms);

/** Copy one message out into @p msg (@p size bytes).  Returns false on timeout/empty. */
bool stp_port_mq_recv(void *handle, void *msg, uint32_t size, uint32_t timeout_ms);

/* ---- logging ------------------------------------------------------------ */

/** printf-style diagnostic log line. */
void stp_port_log(const char *fmt, ...) STP_PORT_PRINTF_FMT;

#ifdef __cplusplus
}
#endif

#endif /* STREAM_TRANSPORT_PORT_H */
