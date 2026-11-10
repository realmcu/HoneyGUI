/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

/**
 * @file gui_port_os.c
 * @brief OS port for the browser (WebAssembly) simulator
 *
 * A WebAssembly module runs on the browser's single thread, so this port has
 * no real threads. It provides the engine's thread API with cooperative
 * scheduling instead:
 *
 * - The GUI server thread is the one the page started main() on. Its loop
 *   yields to the browser through Asyncify (emscripten_sleep) every frame.
 * - Every other thread the engine or a port creates (for example the sysval
 *   async worker) runs as an Emscripten fiber. A fiber runs until it sleeps
 *   (gui_thread_mdelay) or waits on an empty/full message queue, then control
 *   goes back to the GUI server, which resumes the fibers once their wake-up
 *   time has passed. Fibers never run in parallel, so a thread cannot be
 *   interrupted in the middle of a statement.
 *
 * Both suspend the call stack with Asyncify, so every function that can be on
 * the stack must be Asyncify-transformed. A project loaded as a side module
 * has to be linked with -sASYNCIFY too (or run through "wasm-opt --asyncify
 * --pass-arg=asyncify-import-globals"); otherwise resuming traps.
 *
 * The desktop simulator also runs a 10 ms timer thread for gui_tick_hook.
 * Calling into the module from a JS timer while the server is suspended in
 * emscripten_sleep is unsafe under Asyncify, so the hook runs from the
 * server's gui_thread_mdelay() instead, once per elapsed 10 ms period.
 */

#include "guidef.h"
#include "gui_api.h"
#include "gui_port.h"
#include "gui_server.h"
#include "web_sim.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <emscripten.h>
#include <emscripten/fiber.h>

/*============================================================================*
 *                            Threads
 *============================================================================*/

#define PORT_MAX_THREADS       8
#define PORT_FIBER_C_STACK     (64 * 1024)
#define PORT_FIBER_ASYNC_STACK (32 * 1024)
#define PORT_MAIN_ASYNC_STACK  (64 * 1024)
#define PORT_TICK_PERIOD_MS    10

typedef struct port_thread
{
    emscripten_fiber_t fiber;
    void (*entry)(void *param);
    void *param;
    void *c_stack;
    void *async_stack;
    double wake_ms;     /* do not resume before this time */
    bool used;
} port_thread_t;

static port_thread_t threads[PORT_MAX_THREADS];
static emscripten_fiber_t main_fiber;
static uint8_t main_async_stack[PORT_MAIN_ASYNC_STACK];
static bool main_fiber_ready;

/* Index of the running fiber, or -1 when the GUI server (main) is running. */
static int current = -1;

static void (*server_entry)(void *param);
static void *server_param;

static struct gui_os_api os_api;
static double next_tick_ms;

static void port_fiber_yield_until(double wake_ms);

static void fiber_entry(void *arg)
{
    port_thread_t *t = (port_thread_t *)arg;

    t->entry(t->param);

    /* The thread function returned: park the fiber for good. */
    t->wake_ms = 1e300;
    for (;;)
    {
        current = -1;
        emscripten_fiber_swap(&t->fiber, &main_fiber);
    }
}

static void *port_thread_create(const char *name, void (*entry)(void *param), void *param,
                                uint32_t stack_size, uint8_t priority)
{
    (void)stack_size;
    (void)priority;

    /* The GUI server keeps the context main() runs on. */
    if (strcmp(name, GUI_SERVER_THREAD_NAME) == 0)
    {
        server_entry = entry;
        server_param = param;
        return (void *)entry;
    }

    for (int i = 0; i < PORT_MAX_THREADS; i++)
    {
        port_thread_t *t = &threads[i];
        if (t->used)
        {
            continue;
        }
        t->c_stack = malloc(PORT_FIBER_C_STACK);
        t->async_stack = malloc(PORT_FIBER_ASYNC_STACK);
        if (t->c_stack == NULL || t->async_stack == NULL)
        {
            free(t->c_stack);
            free(t->async_stack);
            return NULL;
        }
        t->entry = entry;
        t->param = param;
        t->wake_ms = 0;
        t->used = true;
        emscripten_fiber_init(&t->fiber, fiber_entry, t, t->c_stack, PORT_FIBER_C_STACK,
                              t->async_stack, PORT_FIBER_ASYNC_STACK);
        return t;
    }

    gui_log("web_sim: no free thread slot for \"%s\"\n", name);
    return NULL;
}

static bool port_thread_delete(void *handle)
{
    port_thread_t *t = (port_thread_t *)handle;

    if (t == NULL || t < threads || t >= threads + PORT_MAX_THREADS || !t->used)
    {
        return false;
    }
    if (current >= 0 && t == &threads[current])
    {
        /* A fiber cannot free the stack it is running on: park it for good
         * and keep the slot. Does not return. */
        port_fiber_yield_until(1e300);
        return true;
    }
    /* The fiber is suspended, so its stacks are no longer in use. */
    free(t->c_stack);
    free(t->async_stack);
    memset(t, 0, sizeof(*t));
    return true;
}

static void port_run_tick_hook(void)
{
    if (os_api.gui_tick_hook == NULL)
    {
        return;
    }
    double now = emscripten_get_now();
    if (next_tick_ms == 0)
    {
        next_tick_ms = now;
    }
    /* Catch up at most a few periods after a long stall (hidden tab). */
    for (int i = 0; i < 4 && now >= next_tick_ms; i++)
    {
        os_api.gui_tick_hook();
        next_tick_ms += PORT_TICK_PERIOD_MS;
    }
    if (now >= next_tick_ms)
    {
        next_tick_ms = now + PORT_TICK_PERIOD_MS;
    }
}

/* GUI server context: run every fiber whose wake-up time has passed. */
static void port_run_ready_fibers(void)
{
    for (int i = 0; i < PORT_MAX_THREADS; i++)
    {
        port_thread_t *t = &threads[i];
        if (t->used && emscripten_get_now() >= t->wake_ms)
        {
            current = i;
            emscripten_fiber_swap(&main_fiber, &t->fiber);
            current = -1;
        }
    }
}

/* Fiber context: hand control back to the GUI server until wake_ms. */
static void port_fiber_yield_until(double wake_ms)
{
    port_thread_t *t = &threads[current];

    t->wake_ms = wake_ms;
    current = -1;
    emscripten_fiber_swap(&t->fiber, &main_fiber);
}

/* Number of times the GUI server yielded to the browser, that is, frames run.
 * Lets the page and tests tell a running simulator from a stalled one. */
static uint32_t server_frames;

/* Emscripten starts a fiber through dynCall_vi(). In a MAIN_MODULE build the
 * linker does not generate it, because function pointers are not resolved
 * statically, so provide it here. */
EMSCRIPTEN_KEEPALIVE void dynCall_vi(void (*fn)(void *), void *arg)
{
    fn(arg);
}

EMSCRIPTEN_KEEPALIVE uint32_t web_sim_frame_count(void)
{
    return server_frames;
}

static bool port_thread_mdelay(uint32_t ms)
{
    if (current >= 0)
    {
        port_fiber_yield_until(emscripten_get_now() + ms);
        return true;
    }

    server_frames++;
    port_run_tick_hook();
    port_run_ready_fibers();
    /* Yield to the browser so it can paint the canvas and deliver input. */
    emscripten_sleep(ms);
    return true;
}

void web_sim_run_server(void)
{
    if (server_entry == NULL)
    {
        return;
    }
    if (!main_fiber_ready)
    {
        emscripten_fiber_init_from_current_context(&main_fiber, main_async_stack,
                                                   sizeof(main_async_stack));
        main_fiber_ready = true;
    }
    server_entry(server_param);
}

/*============================================================================*
 *                         Message queues
 *============================================================================*/

typedef struct port_mq
{
    uint8_t  *buf;
    uint32_t  msg_size;
    uint32_t  max_msgs;
    uint32_t  head;
    uint32_t  tail;
    uint32_t  count;
} port_mq_t;

static bool port_mq_create(void *handle, const char *name, uint32_t msg_size,
                           uint32_t max_msgs)
{
    (void)name;

    port_mq_t *mq = (port_mq_t *)calloc(1, sizeof(*mq));
    if (mq == NULL)
    {
        return false;
    }
    mq->buf = (uint8_t *)malloc((size_t)msg_size * max_msgs);
    if (mq->buf == NULL)
    {
        free(mq);
        return false;
    }
    mq->msg_size = msg_size;
    mq->max_msgs = max_msgs;

    *(void **)handle = mq;
    return true;
}

/* Wait, from a fiber, until @p ready() or the timeout expires. The GUI server
 * never waits here: it is the only context that can make progress by running
 * the fibers, so blocking it would hang. The engine only calls the server's
 * receive with a zero timeout in the simulator build. */
static bool port_mq_wait(uint32_t timeout, bool (*ready)(port_mq_t *), port_mq_t *mq)
{
    if (current < 0)
    {
        return ready(mq);
    }

    double deadline = (timeout == 0xFFFFFFFFu) ? 1e300 : emscripten_get_now() + timeout;

    while (!ready(mq))
    {
        if (timeout == 0 || emscripten_get_now() >= deadline)
        {
            return false;
        }
        port_fiber_yield_until(emscripten_get_now() + 1);
    }
    return true;
}

static bool mq_not_full(port_mq_t *mq)
{
    return mq->count < mq->max_msgs;
}

static bool mq_not_empty(port_mq_t *mq)
{
    return mq->count > 0;
}

static bool port_mq_send(void *handle, void *buffer, uint32_t size, uint32_t timeout)
{
    (void)size;
    port_mq_t *mq = (port_mq_t *)handle;

    if (!port_mq_wait(timeout, mq_not_full, mq))
    {
        return false;
    }
    memcpy(mq->buf + (size_t)mq->tail * mq->msg_size, buffer, mq->msg_size);
    mq->tail = (mq->tail + 1) % mq->max_msgs;
    mq->count++;
    return true;
}

static bool port_mq_recv(void *handle, void *buffer, uint32_t size, uint32_t timeout)
{
    (void)size;
    port_mq_t *mq = (port_mq_t *)handle;

    if (!port_mq_wait(timeout, mq_not_empty, mq))
    {
        return false;
    }
    memcpy(buffer, mq->buf + (size_t)mq->head * mq->msg_size, mq->msg_size);
    mq->head = (mq->head + 1) % mq->max_msgs;
    mq->count--;
    return true;
}

/*============================================================================*
 *                            Timing
 *============================================================================*/

static double start_ms;

/* Convert through uint64_t: WebAssembly saturates an out-of-range double to
 * uint32_t conversion instead of wrapping, so the microsecond counter would
 * stick at UINT32_MAX after about 71 minutes. */
static uint32_t port_thread_ms_get(void)
{
    return (uint32_t)(uint64_t)(emscripten_get_now() - start_ms);
}

static uint32_t port_thread_us_get(void)
{
    return (uint32_t)(uint64_t)((emscripten_get_now() - start_ms) * 1000.0);
}

/*============================================================================*
 *                            Memory
 *============================================================================*/

static void *port_malloc(uint32_t n)
{
    return malloc(n);
}

static void *port_realloc(void *ptr, uint32_t n)
{
    return realloc(ptr, n);
}

static void port_free(void *rmem)
{
    free(rmem);
}

#define PORT_GUI_MEMHEAP_SIZE        (1024 * 1024 * 10)
#define PORT_GUI_LOWER_MEMHEAP_SIZE  (1024 * 1024 * 10)

/*============================================================================*
 *                            Logging
 *============================================================================*/

static void port_log(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    char buf[2048];
    vsnprintf(buf, sizeof(buf), format, args);
    va_end(args);
    printf("[GUI MODULE]%s", buf);
}

/*============================================================================*
 *                         OS API registration
 *============================================================================*/

static struct gui_os_api os_api_init =
{
    .name             = "web_single_thread",
    /* thread_suspend/resume/mq_send_urgent stay NULL as in the desktop port */
    .thread_create    = port_thread_create,
    .thread_delete    = port_thread_delete,
    .thread_mdelay    = port_thread_mdelay,
    .thread_ms_get    = port_thread_ms_get,
    .thread_us_get    = port_thread_us_get,
    .mq_create        = port_mq_create,
    .mq_send          = port_mq_send,
    .mq_recv          = port_mq_recv,
    .f_malloc         = port_malloc,
    .f_realloc        = port_realloc,
    .f_free           = port_free,

    .mem_size         = PORT_GUI_MEMHEAP_SIZE,
    .lower_mem_size   = PORT_GUI_LOWER_MEMHEAP_SIZE,
    .mem_threshold_size = 10 * 1024,

    .log              = (void *)port_log,
};

void gui_port_os_init(void)
{
    start_ms = emscripten_get_now();

    os_api = os_api_init;

    /* Heaps are allocated rather than static so the module file stays small. */
    os_api.mem_addr = calloc(1, PORT_GUI_MEMHEAP_SIZE);
    os_api.lower_mem_addr = calloc(1, PORT_GUI_LOWER_MEMHEAP_SIZE);

    gui_os_api_register(&os_api);
}
