/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/**
 * @file  stream_transport.h
 * @brief Live video-stream frame transport (single-producer / single-consumer).
 *
 * This is the transport core that the gui_stream widget builds on: the widget
 * drives the consumer side and renders the delivered frames.  The transport is
 * self-contained and has no widget dependency, so it can also be used on its
 * own.
 *
 * The transport hands encoded frames from a data-receive task (the
 * *producer*) to a UI render task (the *consumer*) through a pair of
 * lock-free-feeling message queues built on top of a thin porting layer
 * (see stream_transport_port.h, so the core carries no GUI dependency of its
 * own).  It never copies frame payloads: only a small
 * fixed-size descriptor (::stp_frame_t) travels through the queues while the
 * encoded bytes stay inside a transport-owned memory pool.
 *
 * Lifecycle of one buffer (exactly one frame per buffer):
 *
 *     FREE ---stp_acquire_free()---> producer fills ---stp_commit()--->
 *     READY ---stp_acquire_*()---> consumer renders ---stp_release()---> FREE
 *
 * Memory model
 * ------------
 * The transport owns the pool: ::stp_instance_create allocates it through the
 * porting allocator (stp_port_malloc -> gui_malloc), sizing it from the size
 * classes, then slices it into buffers grouped by *size class*; each class has
 * its own free queue so acquiring a buffer for a known frame size yields a
 * best-fit match.  Unequal class sizes are fully supported (use a single class
 * for the common equal-size case).
 *
 * Threading model
 * ---------------
 * Strictly one producer thread and one consumer thread (SPSC).  Producer-only
 * APIs: ::stp_acquire_free / ::stp_commit.  Consumer-only APIs:
 * ::stp_acquire_oldest / ::stp_acquire_latest / ::stp_consume / ::stp_release.
 * The underlying queues are internally synchronised, so no extra locking is
 * required.
 *
 * Frame-drop policy (consumer side)
 * ---------------------------------
 *   - ::STP_DROP_UNCONDITIONAL  jump to the newest ready frame, recycling the
 *                               rest.  Suited to intra-only streams (every
 *                               frame is independently decodable).  Default.
 *   - ::STP_DROP_CONDITIONAL    catch up only as far as the newest "keep
 *                               point" (e.g. a key frame) reported by a
 *                               registered condition callback, so inter-frame
 *                               decoding stays valid.
 *   - ::STP_DROP_NONE           never drop, deliver strictly oldest-first.
 *                               Suited to continuously inter-coded streams.
 *
 * The three policies are reachable either through the explicit primitives or
 * through the single ::stp_consume dispatcher driven by stp_config_t::drop_mode.
 */

#ifndef __STREAM_TRANSPORT_H__
#define __STREAM_TRANSPORT_H__

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/*============================================================================*
 *                         Types
 *============================================================================*/

/** Consumer-side frame-drop policy used by ::stp_consume. */
typedef enum
{
    STP_DROP_UNCONDITIONAL = 0, /*!< Default: skip to the newest ready frame.   */
    STP_DROP_CONDITIONAL,       /*!< Skip only down to the newest keep point.   */
    STP_DROP_NONE,              /*!< Never drop, deliver oldest-first.          */
} stp_drop_mode_t;

/**
 * @brief Frame descriptor exchanged through the queues (passed by value).
 *
 * @c addr / @c capacity / @c class_idx are filled by the transport when a free
 * buffer is acquired and must not be altered by the caller.  The producer sets
 * @c used (via ::stp_commit) and the @c is_keyframe hint; @c seq is assigned by
 * the transport on commit.
 */
typedef struct stp_frame
{
    void    *addr;        /*!< Buffer start address inside the user pool.       */
    uint32_t capacity;    /*!< Usable bytes of this buffer.                     */
    uint32_t used;        /*!< Bytes actually written by the producer.          */
    uint32_t seq;         /*!< Monotonic sequence number (transport assigned).  */
    uint16_t class_idx;   /*!< Owning size class (used to recycle on release).  */
    uint8_t  is_keyframe; /*!< Producer hint: 1 if independently decodable.     */
    uint8_t  reserved;    /*!< Padding / future use.                            */
} stp_frame_t;

/**
 * @brief "Keep point" predicate for ::STP_DROP_CONDITIONAL.
 *
 * Return true when @p f marks a frame the consumer may safely catch up to
 * (typically a key frame).  When no callback is registered the transport falls
 * back to the stp_frame_t::is_keyframe hint.
 */
typedef bool (*stp_keep_cond_t)(const stp_frame_t *f, void *user);

/** One memory size class: @c buf_count buffers of @c buf_size bytes each. */
typedef struct stp_class_cfg
{
    uint32_t buf_size;  /*!< Usable bytes of every buffer in this class.        */
    uint32_t buf_count; /*!< Number of buffers carved out for this class.       */
} stp_class_cfg_t;

/** Creation parameters for a transport instance. */
typedef struct stp_config
{
    uint32_t               align;       /*!< Buffer start alignment (0/1 = none).*/

    const stp_class_cfg_t *classes;     /*!< Size classes, ascending @c buf_size.*/
    uint32_t               class_count; /*!< Number of size classes (>= 1).     */

    stp_drop_mode_t        drop_mode;       /*!< Policy used by ::stp_consume.  */
    bool                   allow_oversize_fit; /*!< Escalate to a larger class
                                                 *   when the best fit is empty.*/
    stp_keep_cond_t        keep_cond;       /*!< Predicate for CONDITIONAL.     */
    void                  *keep_cond_user;  /*!< Opaque arg for @c keep_cond.   */
} stp_config_t;

/** Snapshot of pool / buffer occupancy, filled by ::stp_get_usage. */
typedef struct stp_usage
{
    uint32_t pool_size;       /*!< Total pool bytes allocated by the transport. */
    uint32_t bytes_used;      /*!< Bytes carved into buffers (incl. alignment). */
    uint32_t bytes_overhead;  /*!< Bytes lost to alignment padding only.        */
    uint32_t bytes_free;      /*!< Trailing pool bytes left unused.             */
    uint32_t total_buffers;   /*!< Total buffers across all classes.            */
    uint32_t ready_count;     /*!< Buffers currently committed and waiting.     */
} stp_usage_t;

/** Opaque transport handle. */
typedef struct stp_transport stp_transport_t;

/*============================================================================*
 *                         Functions
 *============================================================================*/

/**
 * @brief Fill @p cfg with recommended defaults.
 *
 * Sets @c drop_mode = ::STP_DROP_UNCONDITIONAL and @c allow_oversize_fit = true.
 * The caller must still supply @c classes and @c class_count before calling
 * ::stp_instance_create.
 */
void stp_config_default(stp_config_t *cfg);

/**
 * @brief Create a transport instance and its frame pool.
 *
 * The frame pool is allocated internally through the porting allocator
 * (stp_port_malloc -> gui_malloc); its size is derived from @c classes (sum of
 * buf_size * buf_count plus per-buffer alignment slack).
 *
 * @param cfg  Configuration (copied internally; @c classes is duplicated).
 * @return A handle, or NULL on invalid arguments / OOM.
 *         On NULL the failure reason is logged via the port (stp_port_log).
 */
stp_transport_t *stp_instance_create(const stp_config_t *cfg);

/** @brief Destroy a transport instance and release everything it owns,
 *  including the internally allocated frame pool. */
void stp_instance_destroy(stp_transport_t *t);

/* ---- Producer side --------------------------------------------------------*/

/**
 * @brief Acquire a free buffer able to hold @p frame_size bytes (best fit).
 *
 * Scans size classes from the smallest that fits upward.  When the best class
 * is empty and stp_config_t::allow_oversize_fit is set, a larger class is used.
 *
 * @return true and fills @p out on success; false if no buffer is available
 *         (the frame should be dropped) or @p frame_size exceeds every class.
 */
bool stp_acquire_free(stp_transport_t *t, uint32_t frame_size, stp_frame_t *out);

/**
 * @brief Publish a previously acquired buffer to the ready queue.
 *
 * @param f           Descriptor returned by ::stp_acquire_free.
 * @param used_len    Bytes actually written (must be <= @c f->capacity).
 * @param is_keyframe Non-zero if the frame is independently decodable.
 * @return true on success; false if @p used_len is invalid.
 */
bool stp_commit(stp_transport_t *t, stp_frame_t *f, uint32_t used_len,
                bool is_keyframe);

/* ---- Consumer side --------------------------------------------------------*/

/**
 * @brief Take the oldest ready frame (FIFO head).  No frames are dropped.
 * @param timeout  Milliseconds to wait, 0 = poll, 0xFFFFFFFF = wait forever.
 */
bool stp_acquire_oldest(stp_transport_t *t, uint32_t timeout, stp_frame_t *out);

/**
 * @brief Take the newest ready frame, recycling every older one (uncond. drop).
 * @param timeout  Milliseconds to wait for at least one frame.
 */
bool stp_acquire_latest(stp_transport_t *t, uint32_t timeout, stp_frame_t *out);

/**
 * @brief Take one frame honouring stp_config_t::drop_mode.
 *
 *   - UNCONDITIONAL -> ::stp_acquire_latest
 *   - NONE          -> ::stp_acquire_oldest
 *   - CONDITIONAL   -> deliver oldest-first but, when a newer keep point is
 *                      already queued, recycle the stale frames ahead of it
 *                      first so the consumer catches up to a decodable point.
 */
bool stp_consume(stp_transport_t *t, uint32_t timeout, stp_frame_t *out);

/** @brief Return a consumed buffer to its free class.  Always succeeds. */
bool stp_release(stp_transport_t *t, stp_frame_t *f);

/* ---- Introspection --------------------------------------------------------*/

/** @brief Number of frames currently committed and waiting to be consumed. */
uint32_t stp_ready_count(stp_transport_t *t);

/** @brief Fill @p usage with a best-effort occupancy snapshot. */
void stp_get_usage(stp_transport_t *t, stp_usage_t *usage);

/** @brief Log a human-readable pool / class / occupancy report via the port. */
void stp_dump_usage(stp_transport_t *t);

#ifdef __cplusplus
}
#endif

#endif /* __STREAM_TRANSPORT_H__ */
