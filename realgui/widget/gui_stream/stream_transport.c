/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/**
 * @file  stream_transport.c
 * @brief Implementation of the SPSC live-stream frame transport.
 *
 * Queue layout
 * ------------
 *   - One free queue per size class (free_mq[c]).  Capacity equals that
 *     class' buffer count, so a release (send) can never fail: the descriptor
 *     originally came from there.
 *   - One ready queue with capacity equal to the total buffer count, so a
 *     commit (send) can never fail either: at most every buffer can be ready
 *     at once.
 *
 * Only the small ::stp_frame_t descriptor travels through the queues; the
 * encoded payload stays in the transport-owned pool the whole time.  Because
 * the queues are internally synchronised and each queue has a single sender
 * and a single receiver of opposite roles, no extra locking is needed.
 *
 * The CONDITIONAL drop path keeps a consumer-private "carry" ring (touched
 * only by the consumer thread) so it can recycle stale frames ahead of the
 * newest keep point without ever re-queuing -- which would otherwise reorder
 * frames relative to the producer.
 */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <string.h>
#include "stream_transport.h"
#include "stream_transport_port.h"   /* gui-free seam: memory / mq / log */

/*============================================================================*
 *                         Types
 *============================================================================*/
struct stp_transport
{
    /* Configuration snapshot. */
    uint8_t            *pool;
    uint32_t            pool_size;
    uint32_t            align;
    stp_class_cfg_t    *classes;     /* duplicated, length == class_count       */
    uint32_t            class_count;
    bool                allow_oversize_fit;
    stp_drop_mode_t     drop_mode;
    stp_keep_cond_t     keep_cond;
    void               *keep_cond_user;

    /* Queues (handles owned by the OS port). */
    void              **free_mq;     /* [class_count]                           */
    void               *ready_mq;
    uint32_t            total_buffers;

    /* Pool accounting. */
    uint32_t            bytes_used;     /* buffers + internal alignment padding  */
    uint32_t            bytes_overhead; /* alignment padding only                */

    /* Producer-only state. */
    uint32_t            seq_next;

    /* Lock-free depth accounting (replaces a port-level queue-count probe).
     * Each counter has exactly one writer and is read as a best-effort
     * snapshot: ready depth    = ready_produced - ready_taken;
     *           per-class free = buf_count - cls_acquired[c] + cls_released[c].
     * Producer-side counters are bumped BEFORE the send that publishes their
     * effect, so produced >= taken holds across the queue's release/acquire
     * barrier and the unsigned differences never underflow. */
    uint32_t            ready_produced; /* producer: frames sent to ready_mq    */
    uint32_t            ready_taken;    /* consumer: frames taken from ready_mq  */
    uint32_t           *cls_acquired;   /* producer: [class_count] free taken    */
    uint32_t           *cls_released;   /* consumer: [class_count] free returned */

    /* Consumer-private carry ring (CONDITIONAL drop). */
    stp_frame_t        *carry;       /* [total_buffers]                         */
    uint32_t            carry_head;
    uint32_t            carry_tail;
    uint32_t            carry_count;

    /* Best-effort statistics (each counter has a single writer). */
    uint32_t            stat_committed;       /* producer */
    uint32_t            stat_dropped_acquire; /* producer */
    uint32_t            stat_dropped_consume; /* consumer */
};

/*============================================================================*
 *                         Private helpers
 *============================================================================*/

static uintptr_t stp_align_up(uintptr_t p, uint32_t a)
{
    uintptr_t rem;

    if (a <= 1u)
    {
        return p;
    }
    rem = p % (uintptr_t)a;
    if (rem == 0u)
    {
        return p;
    }
    return p + ((uintptr_t)a - rem);
}

/* Worst-case pool bytes to carve every class' buffers: each buffer may waste up
 * to (align-1) on alignment, plus one extra alignment for a possibly-unaligned
 * malloc base.  Accumulate in 64 bits and reject configs that overflow 32. */
static bool stp_pool_bytes(const stp_config_t *cfg, uint32_t *need_out)
{
    uint32_t a    = (cfg->align == 0u) ? 1u : cfg->align;
    uint64_t need = a;
    uint32_t c;

    for (c = 0; c < cfg->class_count; c++)
    {
        need += (uint64_t)cfg->classes[c].buf_count *
                ((uint64_t)cfg->classes[c].buf_size + (a - 1u));
    }
    if (need > 0xFFFFFFFFu)
    {
        return false;
    }
    *need_out = (uint32_t)need;
    return true;
}

static bool stp_is_keep_point(stp_transport_t *t, const stp_frame_t *f)
{
    if (t->keep_cond != NULL)
    {
        return t->keep_cond(f, t->keep_cond_user);
    }
    return f->is_keyframe != 0u;
}

/* Consumer-only carry-ring operations (no locking required). */
static void stp_carry_push(stp_transport_t *t, const stp_frame_t *f)
{
    t->carry[t->carry_tail] = *f;
    t->carry_tail = (t->carry_tail + 1u) % t->total_buffers;
    t->carry_count++;
}

static void stp_carry_pop(stp_transport_t *t, stp_frame_t *out)
{
    *out = t->carry[t->carry_head];
    t->carry_head = (t->carry_head + 1u) % t->total_buffers;
    t->carry_count--;
}

/* Consumer-only: take one frame off the ready queue and account for it.
 * ready_taken has a single writer (the consumer); it advances only on a
 * successful recv, so ready_produced - ready_taken stays non-negative. */
static bool stp_ready_recv(stp_transport_t *t, stp_frame_t *out, uint32_t timeout)
{
    if (stp_port_mq_recv(t->ready_mq, out, sizeof(*out), timeout))
    {
        t->ready_taken++;
        return true;
    }
    return false;
}

/*============================================================================*
 *                         Public API
 *============================================================================*/

void stp_config_default(stp_config_t *cfg)
{
    if (cfg == NULL)
    {
        return;
    }
    memset(cfg, 0, sizeof(*cfg));
    cfg->drop_mode          = STP_DROP_UNCONDITIONAL;
    cfg->allow_oversize_fit = true;
}

stp_transport_t *stp_instance_create(const stp_config_t *cfg)
{
    stp_transport_t *t;
    uintptr_t        base;
    uintptr_t        end;
    uintptr_t        cur;
    uint32_t         overhead  = 0;
    uint32_t         total     = 0;
    uint32_t         pool_need = 0;
    uint32_t         c;
    uint32_t         i;

    if (cfg == NULL || cfg->classes == NULL || cfg->class_count == 0u)
    {
        stp_port_log("stp: invalid config\n");
        return NULL;
    }

    if (!stp_pool_bytes(cfg, &pool_need))
    {
        stp_port_log("stp: configured pool size overflows 32 bits\n");
        return NULL;
    }

    for (c = 0; c < cfg->class_count; c++)
    {
        if (cfg->classes[c].buf_size == 0u || cfg->classes[c].buf_count == 0u)
        {
            stp_port_log("stp: class[%u] must have non-zero size and count\n",
                         (unsigned)c);
            return NULL;
        }
        if (c > 0u && cfg->classes[c].buf_size < cfg->classes[c - 1u].buf_size)
        {
            stp_port_log("stp: warning, class sizes not ascending; best-fit may be "
                         "suboptimal\n");
        }
    }

    t = (stp_transport_t *)stp_port_calloc(1, sizeof(*t));
    if (t == NULL)
    {
        stp_port_log("stp: out of memory (handle)\n");
        return NULL;
    }

    t->classes = (stp_class_cfg_t *)stp_port_malloc(cfg->class_count *
                                                    sizeof(stp_class_cfg_t));
    t->free_mq = (void **)stp_port_calloc(cfg->class_count, sizeof(void *));
    t->cls_acquired = (uint32_t *)stp_port_calloc(cfg->class_count, sizeof(uint32_t));
    t->cls_released = (uint32_t *)stp_port_calloc(cfg->class_count, sizeof(uint32_t));
    if (t->classes == NULL || t->free_mq == NULL ||
        t->cls_acquired == NULL || t->cls_released == NULL)
    {
        stp_port_log("stp: out of memory (metadata)\n");
        stp_instance_destroy(t);
        return NULL;
    }
    memcpy(t->classes, cfg->classes, cfg->class_count * sizeof(stp_class_cfg_t));

    /* Allocate the frame pool from the porting allocator, sized from the size
     * classes -- the caller no longer supplies one. */
    t->pool = (uint8_t *)stp_port_malloc(pool_need);
    if (t->pool == NULL)
    {
        stp_port_log("stp: out of memory (pool, %u bytes)\n", (unsigned)pool_need);
        stp_instance_destroy(t);
        return NULL;
    }

    t->class_count        = cfg->class_count;
    t->pool_size          = pool_need;
    t->align              = (cfg->align == 0u) ? 1u : cfg->align;
    t->allow_oversize_fit = cfg->allow_oversize_fit;
    t->drop_mode          = cfg->drop_mode;
    t->keep_cond          = cfg->keep_cond;
    t->keep_cond_user     = cfg->keep_cond_user;

    /* Carve the pool: one free queue per class, then split buffers into it. */
    base = (uintptr_t)t->pool;
    end  = base + (uintptr_t)t->pool_size;
    cur  = base;

    for (c = 0; c < t->class_count; c++)
    {
        if (!stp_port_mq_create(&t->free_mq[c], "stp_free", sizeof(stp_frame_t),
                                t->classes[c].buf_count))
        {
            stp_port_log("stp: failed to create free queue for class %u\n",
                         (unsigned)c);
            stp_instance_destroy(t);
            return NULL;
        }

        for (i = 0; i < t->classes[c].buf_count; i++)
        {
            uintptr_t   aligned = stp_align_up(cur, t->align);
            uintptr_t   tail    = aligned + (uintptr_t)t->classes[c].buf_size;
            stp_frame_t f;

            if (aligned < cur || tail < aligned || tail > end)
            {
                stp_port_log("stp: pool too small at class %u buffer %u "
                             "(need more than %u bytes)\n",
                             (unsigned)c, (unsigned)i, (unsigned)t->pool_size);
                stp_instance_destroy(t);
                return NULL;
            }

            overhead += (uint32_t)(aligned - cur);

            memset(&f, 0, sizeof(f));
            f.addr      = (void *)aligned;
            f.capacity  = t->classes[c].buf_size;
            f.class_idx = (uint16_t)c;
            (void)stp_port_mq_send(t->free_mq[c], &f, sizeof(f), 0);

            cur = tail;
            total++;
        }
    }

    t->total_buffers  = total;
    t->bytes_used     = (uint32_t)(cur - base);
    t->bytes_overhead = overhead;

    if (!stp_port_mq_create(&t->ready_mq, "stp_ready", sizeof(stp_frame_t), total))
    {
        stp_port_log("stp: failed to create ready queue\n");
        stp_instance_destroy(t);
        return NULL;
    }

    t->carry = (stp_frame_t *)stp_port_calloc(total, sizeof(stp_frame_t));
    if (t->carry == NULL)
    {
        stp_port_log("stp: out of memory (carry)\n");
        stp_instance_destroy(t);
        return NULL;
    }

    stp_port_log("stp: created %u buffers across %u classes, pool %u/%u bytes used "
                 "(%u overhead)\n",
                 (unsigned)total, (unsigned)t->class_count,
                 (unsigned)t->bytes_used, (unsigned)t->pool_size,
                 (unsigned)t->bytes_overhead);
    return t;
}

void stp_instance_destroy(stp_transport_t *t)
{
    if (t == NULL)
    {
        return;
    }
    /* The port's message-queue API exposes no delete primitive, so the
     * free/ready queues persist for the process lifetime (a deliberate
     * create-once convention).  Everything else this instance owns -- the
     * frame pool and the bookkeeping arrays -- is reclaimed here. */
    if (t->pool != NULL)
    {
        stp_port_free(t->pool);
    }
    if (t->carry != NULL)
    {
        stp_port_free(t->carry);
    }
    if (t->classes != NULL)
    {
        stp_port_free(t->classes);
    }
    if (t->free_mq != NULL)
    {
        stp_port_free(t->free_mq);
    }
    if (t->cls_acquired != NULL)
    {
        stp_port_free(t->cls_acquired);
    }
    if (t->cls_released != NULL)
    {
        stp_port_free(t->cls_released);
    }
    stp_port_free(t);
}

/* ---- Producer side --------------------------------------------------------*/

bool stp_acquire_free(stp_transport_t *t, uint32_t frame_size, stp_frame_t *out)
{
    uint32_t c;

    if (t == NULL || out == NULL)
    {
        return false;
    }

    for (c = 0; c < t->class_count; c++)
    {
        if (t->classes[c].buf_size < frame_size)
        {
            continue; /* class too small, try a larger one */
        }
        /* Ascending sizes => the first fitting class is the best fit. */
        if (stp_port_mq_recv(t->free_mq[c], out, sizeof(*out), 0))
        {
            out->used = 0u;
            t->cls_acquired[c]++;
            return true;
        }
        if (!t->allow_oversize_fit)
        {
            break; /* best class is empty and escalation is disabled */
        }
    }

    t->stat_dropped_acquire++;
    stp_port_log("stp: no free buffer for %u-byte frame, dropping (drop@acquire=%u)\n",
                 (unsigned)frame_size, (unsigned)t->stat_dropped_acquire);
    return false;
}

bool stp_commit(stp_transport_t *t, stp_frame_t *f, uint32_t used_len,
                bool is_keyframe)
{
    if (t == NULL || f == NULL)
    {
        return false;
    }
    if (used_len > f->capacity)
    {
        stp_port_log("stp: commit rejected, used %u > capacity %u\n",
                     (unsigned)used_len, (unsigned)f->capacity);
        return false;
    }

    f->used        = used_len;
    f->is_keyframe = is_keyframe ? 1u : 0u;
    f->seq         = t->seq_next++;

    /* Publish the depth increment BEFORE the send: a consumer must never be
     * able to take the frame (and bump ready_taken) before ready_produced has
     * counted it, otherwise the unsigned ready depth could underflow. */
    t->ready_produced++;
    if (!stp_port_mq_send(t->ready_mq, f, sizeof(*f), 0))
    {
        /* Capacity equals the total buffer count, so this should be
         * unreachable; recover the buffer rather than leak it. */
        t->ready_produced--;
        stp_port_log("stp: ready queue unexpectedly full, recycling buffer\n");
        (void)stp_release(t, f);
        return false;
    }

    t->stat_committed++;
    return true;
}

/* ---- Consumer side --------------------------------------------------------*/

bool stp_acquire_oldest(stp_transport_t *t, uint32_t timeout, stp_frame_t *out)
{
    if (t == NULL || out == NULL)
    {
        return false;
    }
    return stp_ready_recv(t, out, timeout);
}

bool stp_acquire_latest(stp_transport_t *t, uint32_t timeout, stp_frame_t *out)
{
    stp_frame_t cur;
    stp_frame_t nxt;

    if (t == NULL || out == NULL)
    {
        return false;
    }

    if (!stp_ready_recv(t, &cur, timeout))
    {
        return false; /* nothing became ready within the timeout */
    }

    /* Drain every newer frame, recycling the ones we skip past. */
    while (stp_ready_recv(t, &nxt, 0))
    {
        (void)stp_release(t, &cur);
        t->stat_dropped_consume++;
        cur = nxt;
    }

    *out = cur;
    return true;
}

/* CONDITIONAL: deliver oldest-first, but recycle stale frames sitting ahead of
 * the newest keep point so the consumer can catch up to a decodable boundary
 * without losing the fresh frames that follow it. */
static bool stp_consume_conditional(stp_transport_t *t, uint32_t timeout,
                                    stp_frame_t *out)
{
    stp_frame_t f;
    uint32_t    idx;
    uint32_t    k;
    uint32_t    newest_keep_off = 0u; /* offset from head; 0 => none past head */
    bool        found           = false;

    /* 1. Make sure there is at least one frame to consider (honour timeout). */
    if (t->carry_count == 0u)
    {
        if (!stp_ready_recv(t, &f, timeout))
        {
            return false;
        }
        stp_carry_push(t, &f);
    }

    /* 2. Pull everything currently ready into the carry ring (non-blocking). */
    while (t->carry_count < t->total_buffers &&
           stp_ready_recv(t, &f, 0))
    {
        stp_carry_push(t, &f);
    }

    /* 3. Locate the newest keep point currently held in the carry ring. */
    idx = t->carry_head;
    for (k = 0; k < t->carry_count; k++)
    {
        if (stp_is_keep_point(t, &t->carry[idx]))
        {
            newest_keep_off = k;
            found           = true;
        }
        idx = (idx + 1u) % t->total_buffers;
    }

    /* 4. Recycle stale frames ahead of that keep point (catch up). */
    if (found && newest_keep_off > 0u)
    {
        for (k = 0; k < newest_keep_off; k++)
        {
            stp_frame_t stale;
            stp_carry_pop(t, &stale);
            (void)stp_release(t, &stale);
            t->stat_dropped_consume++;
        }
    }

    /* 5. Deliver the oldest remaining frame. */
    stp_carry_pop(t, out);
    return true;
}

bool stp_consume(stp_transport_t *t, uint32_t timeout, stp_frame_t *out)
{
    if (t == NULL || out == NULL)
    {
        return false;
    }

    switch (t->drop_mode)
    {
    case STP_DROP_NONE:
        return stp_acquire_oldest(t, timeout, out);

    case STP_DROP_CONDITIONAL:
        return stp_consume_conditional(t, timeout, out);

    case STP_DROP_UNCONDITIONAL:
    default:
        return stp_acquire_latest(t, timeout, out);
    }
}

bool stp_release(stp_transport_t *t, stp_frame_t *f)
{
    if (t == NULL || f == NULL)
    {
        return false;
    }
    if (f->class_idx >= t->class_count)
    {
        stp_port_log("stp: release rejected, bad class_idx %u\n",
                     (unsigned)f->class_idx);
        return false;
    }
    f->used = 0u;
    /* Count the release before the send (single consumer writer) so the
     * producer's free-depth snapshot can never underflow. */
    t->cls_released[f->class_idx]++;
    return stp_port_mq_send(t->free_mq[f->class_idx], f, sizeof(*f), 0);
}

/* ---- Introspection --------------------------------------------------------*/

uint32_t stp_ready_count(stp_transport_t *t)
{
    if (t == NULL)
    {
        return 0u;
    }
    return (t->ready_produced - t->ready_taken) + t->carry_count;
}

void stp_get_usage(stp_transport_t *t, stp_usage_t *usage)
{
    if (t == NULL || usage == NULL)
    {
        return;
    }
    usage->pool_size      = t->pool_size;
    usage->bytes_used     = t->bytes_used;
    usage->bytes_overhead = t->bytes_overhead;
    usage->bytes_free     = t->pool_size - t->bytes_used;
    usage->total_buffers  = t->total_buffers;
    usage->ready_count    = stp_ready_count(t);
}

void stp_dump_usage(stp_transport_t *t)
{
    uint32_t c;

    if (t == NULL)
    {
        return;
    }

    stp_port_log("==== stream_transport usage ====\n");
    stp_port_log("pool : %u bytes | used %u | align overhead %u | free %u\n",
                 (unsigned)t->pool_size, (unsigned)t->bytes_used,
                 (unsigned)t->bytes_overhead,
                 (unsigned)(t->pool_size - t->bytes_used));
    stp_port_log("buffers: %u total | %u ready (incl. %u carried)\n",
                 (unsigned)t->total_buffers, (unsigned)stp_ready_count(t),
                 (unsigned)t->carry_count);
    for (c = 0; c < t->class_count; c++)
    {
        uint32_t freec = t->classes[c].buf_count -
                         t->cls_acquired[c] + t->cls_released[c];
        stp_port_log("  class[%u]: size %u | count %u | free %u\n",
                     (unsigned)c, (unsigned)t->classes[c].buf_size,
                     (unsigned)t->classes[c].buf_count,
                     (unsigned)freec);
    }
    stp_port_log("stats: committed %u | drop@acquire %u | drop@consume %u\n",
                 (unsigned)t->stat_committed, (unsigned)t->stat_dropped_acquire,
                 (unsigned)t->stat_dropped_consume);
    stp_port_log("================================\n");
}
