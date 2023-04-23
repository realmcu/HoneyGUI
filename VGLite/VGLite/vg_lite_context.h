/****************************************************************************
*
*    Copyright 2012 - 2022 Vivante Corporation, Santa Clara, California.
*    All Rights Reserved.
*
*    Permission is hereby granted, free of charge, to any person obtaining
*    a copy of this software and associated documentation files (the
*    'Software'), to deal in the Software without restriction, including
*    without limitation the rights to use, copy, modify, merge, publish,
*    distribute, sub license, and/or sell copies of the Software, and to
*    permit persons to whom the Software is furnished to do so, subject
*    to the following conditions:
*
*    The above copyright notice and this permission notice (including the
*    next paragraph) shall be included in all copies or substantial
*    portions of the Software.
*
*    THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
*    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
*    IN NO EVENT SHALL VIVANTE AND/OR ITS SUPPLIERS BE LIABLE FOR ANY
*    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
*    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
*    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <math.h>
#include "vg_lite_options.h"
#include "vg_lite.h"
#include "vg_lite_kernel.h"


#define DUMP_COMMAND            0
#define DUMP_IMAGE              0

/* Enable FC buffer dump if SOC supports fast clear */
#define VG_TARGET_FC_DUMP       0


#if DUMP_COMMAND || DUMP_IMAGE
#ifdef __linux__
#include <unistd.h>
#endif
FILE* fp;
char filename[30];
#endif


/*** Global Context Access ***/
#define GET_CONTEXT()               &s_context

/*** Default command buffer size is 32KB. Double command buffer is used.
     App can call vg_lite_set_command_buffer_size(size) before vg_lite_init()
     to overwrite the default command buffer size.
***/
#define VG_LITE_COMMAND_BUFFER_SIZE (32 << 10)

#define CMDBUF_BUFFER(context)      (context).command_buffer[(context).command_buffer_current]
#define CMDBUF_INDEX(context)       (context).command_buffer_current
#define CMDBUF_SIZE(context)        (context).command_buffer_size
#define CMDBUF_OFFSET(context)      (context).command_offset[(context).command_buffer_current]
#define CMDBUF_SWAP(context)        (context).command_buffer_current = \
                                        ((context).command_buffer_current + 1) % CMDBUF_COUNT

/*** Command macros ***/
#define VG_LITE_END(interrupt)      (0x00000000 | interrupt)
#define VG_LITE_SEMAPHORE(id)       (0x10000000 | id)
#define VG_LITE_STALL(id)           (0x20000000 | id)
#define VG_LITE_STATE(address)      (0x30010000 | address)
#define VG_LITE_STATES(count, address)  (0x30000000 | ((count) << 16) | address)
#define VG_LITE_DATA(count)         (0x40000000 | count)
#define VG_LITE_CALL(count)         (0x60000000 | count)
#define VG_LITE_RETURN()            (0x70000000)
#define VG_LITE_NOP()               (0x80000000)

#define FC_BURST_BYTES              64
#define FC_BIT_TO_BYTES             64
#define DEST_ALIGNMENT_LIMITATION   64  /* To match hardware alignment requirement */

#define STATES_COUNT                208
#define MIN_TS_SIZE                 8 << 10

#define VG_LITE_RETURN_ERROR(func) \
    if ((error = func) != VG_LITE_SUCCESS) \
    return error

#define VG_LITE_BREAK_ERROR(func) \
    if ((error = func) != VG_LITE_SUCCESS) \
    break

#define VG_LITE_ERROR_HANDLER(func) \
    if ((error = func) != VG_LITE_SUCCESS) \
    goto ErrorHandler


/* Driver implementation internal structures.
*/
typedef struct vg_lite_states {
    uint32_t                    state;
    uint8_t                     init;
} vg_lite_states_t;

typedef struct vg_lite_hardware {
    vg_lite_states_t            hw_states[STATES_COUNT];
} vg_lite_hardware_t;

typedef struct vg_lite_context {
    vg_lite_kernel_context_t    context;
    vg_lite_hardware_t          hw;
    vg_lite_capabilities_t      capabilities;
    uint8_t                    *command_buffer[CMDBUF_COUNT];
    uint32_t                    command_buffer_size;
    uint32_t                    command_offset[CMDBUF_COUNT];
    uint32_t                    command_buffer_current;
    vg_lite_tess_buffer_t       tessbuf;
    vg_lite_buffer_t           *rtbuffer;                   /* DDRLess: this is used as composing buffer. */

    uint32_t                    scissor_enabled;
    uint32_t                    scissor_dirty;
    int32_t                     scissor[4];                 /* Scissor area: x, y, right, bottom. */

    uint32_t                    src_alpha_mode;
    uint32_t                    src_alpha_value;
    uint32_t                    dst_alpha_mode;
    uint32_t                    dst_alpha_value;

    uint32_t                    sbi_mode;
    uint32_t                    sync_mode;
    uint32_t                    flexa_mode;
    uint32_t                    stream_id;
    uint32_t                    segment_address;
    uint32_t                    segment_count;
    uint32_t                    segment_size;
    uint32_t                    stop_flag;
    uint8_t                     flexa_dirty;
    uint32_t                    start_flag;
    uint32_t                    reset_flag;
    uint8_t                     custom_cmdbuf;
    uint8_t                     custom_tessbuf;
    uint8_t                     use_dst;
    uint32_t                    enable_mask;
    uint32_t                    matrix_enable;
    uint32_t                    target_width;
    uint32_t                    target_height;
    uint8_t                     enable_scissor;
    uint32_t                    mirror_orient;
    uint32_t                    mirror_dirty;
    uint32_t                    gamma_value;
    uint32_t                    gamma_dirty;
    uint32_t                    premultiply_src;
    uint32_t                    premultiply_dst;
    uint32_t                    premultiply_lerp;
    uint32_t                    premultiply_dirty;
    uint32_t                    color_transform;
    uint32_t                    path_counter;
    uint32_t                    scissor_enable;
} vg_lite_context_t;

typedef struct vg_lite_ftable {
    uint32_t                    ftable[gcFEATURE_COUNT];
} vg_lite_ftable_t;

extern vg_lite_context_t        s_context;
extern vg_lite_ftable_t         s_ftable;

extern vg_lite_error_t push_state(vg_lite_context_t* context, uint32_t address, uint32_t data);
extern vg_lite_error_t push_clut(vg_lite_context_t* context, uint32_t address, uint32_t count, uint32_t* data);

extern void* vg_lite_os_malloc(size_t size);
extern void  vg_lite_os_free(void* memory);


/**************************** Capture ********************************************/
#ifndef vgliteDUMP_PATH
#   define vgliteDUMP_PATH                      "./"
#endif

#ifndef vgliteDUMP_KEY
#   define vgliteDUMP_KEY                          "process"
#endif

#define DUMP_CAPTURE                            0

#if DUMP_CAPTURE
void _SetDumpFileInfo();

vg_lite_error_t
vglitefDump(
    char* String,
    ...
);
#  define vglitemDUMP               vglitefDump

vg_lite_error_t
vglitefDumpBuffer(
    char* Tag,
    size_t Physical,
    void* Logical,
    size_t Offset,
    size_t Bytes
);
#   define vglitemDUMP_BUFFER       vglitefDumpBuffer
#else
inline static void __dummy_dump(
    char* Message,
    ...
)
{
}
#  define vglitemDUMP               __dummy_dump

inline static void
__dummy_dump_buffer(
    char* Tag,
    size_t Physical,
    void* Logical,
    size_t Offset,
    size_t Bytes
)
{
}
#   define vglitemDUMP_BUFFER       __dummy_dump_buffer
#endif
/**************************** Capture ********************************************/
