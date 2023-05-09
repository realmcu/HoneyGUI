/****************************************************************************
*
*    The MIT License (MIT)
*
*    Copyright (c) 2014 - 2022 Vivante Corporation
*
*    Permission is hereby granted, free of charge, to any person obtaining a
*    copy of this software and associated documentation files (the "Software"),
*    to deal in the Software without restriction, including without limitation
*    the rights to use, copy, modify, merge, publish, distribute, sublicense,
*    and/or sell copies of the Software, and to permit persons to whom the
*    Software is furnished to do so, subject to the following conditions:
*
*    The above copyright notice and this permission notice shall be included in
*    all copies or substantial portions of the Software.
*
*    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
*    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
*    DEALINGS IN THE SOFTWARE.
*
*****************************************************************************
*
*    The GPL License (GPL)
*
*    Copyright (C) 2014 - 2022 Vivante Corporation
*
*    This program is free software; you can redistribute it and/or
*    modify it under the terms of the GNU General Public License
*    as published by the Free Software Foundation; either version 2
*    of the License, or (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program; if not, write to the Free Software Foundation,
*    Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*
*****************************************************************************
*
*    Note: This software is released under dual MIT and GPL licenses. A
*    recipient may use this file under the terms of either the MIT license or
*    GPL License. If you wish to use only one license not the other, you can
*    indicate your decision by deleting one of the above license notices in your
*    version of this file.
*
*****************************************************************************/

#include "vg_lite_platform.h"
#include "vg_lite_hal.h"
#include "vg_lite_ioctl.h"
#include "vg_lite_hw.h"
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/list.h>
#include <linux/wait.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/pagemap.h>
#include <linux/vmalloc.h>
#include <linux/mman.h>
#include <linux/mm_types.h>
#include <asm/io.h>
#include <linux/dma-mapping.h>
#include <linux/dma-buf.h>

MODULE_LICENSE("Dual MIT/GPL");


/*#define GPU_REG_START   0x02204000
#define GPU_REG_SIZE    0x00004000
#define GPU_IRQ         43*/
static ulong registerMemBase = 0x02204000;
module_param(registerMemBase, ulong, 0644);

static uint registerMemSize = 0x00004000;
module_param(registerMemSize, uint, 0644);

static uint irqLine         = 0;
module_param(irqLine, uint, 0644);

static uint contiguousSize = 0x02000000;
module_param(contiguousSize, uint, 0644);

static ulong contiguousBase = 0x38000000;
module_param(contiguousBase, ulong, 0644);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 1, 0)
# if gcdIRQ_SHARED
#  define gcdIRQF_FLAG    (IRQF_SHARED)
# else
#  define gcdIRQF_FLAG    0
# endif
#else
# if gcdIRQ_SHARED
#  define gcdIRQF_FLAG    (IRQF_DISABLED | IRQF_SHARED | IRQF_TRIGGER_HIGH)
# else
#  define gcdIRQF_FLAG    (IRQF_DISABLED | IRQF_TRIGGER_HIGH)
# endif
#endif

#define HEAP_NODE_USED  0xABBAF00D
#define GPU_PHY_BASE    0x0ULL

#if LINUX_VERSION_CODE >= KERNEL_VERSION (3,7,0)
#define VM_FLAGS (VM_IO | VM_DONTCOPY | VM_DONTEXPAND | VM_DONTDUMP)
#else
#define VM_FLAGS (VM_IO | VM_DONTCOPY | VM_DONTEXPAND | VM_RESERVED)
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,8,0)
#define current_mm_mmap_sem current->mm->mmap_lock
#else
#define current_mm_mmap_sem current->mm->mmap_sem
#endif

#define GET_PAGE_COUNT(size, page_size) \
    ( \
      ((size) + (page_size) - 1) / page_size \
    )

enum um_desc_type
{
    UM_PAGE_MAP,
    UM_PFN_MAP,
};

struct mapped_memory
{
    vg_lite_uint32_t  flags;

    union
    {
        struct
        {
            /* parse user dma_buf fd */
            vg_lite_pointer usr_dmabuf;

            /* Descriptor of a dma_buf imported. */
            struct dma_buf *dmabuf;
            struct sg_table *sgt;
            struct dma_buf_attachment *attachment;
            vg_lite_uintptr_t *dma_address_array;

            vg_lite_int32_t npages;
            vg_lite_int32_t pid;
            struct list_head list;
        } dmabuf_desc;

        struct
        {
            enum um_desc_type type;

            vg_lite_pointer   logical;
            vg_lite_uintptr_t physical;
            vg_lite_int32_t   page_count;

            union
            {
                /* UM_PAGE_MAP. */
                struct
                {
                    struct page  **pages;
                };

                /* UM_PFN_MAP. */
                struct
                {
                    vg_lite_long_t  *pfns;
                    vg_lite_int32_t *refs;
                    vg_lite_int32_t  pfns_valid;
                };
            };

            /* TODO: Map pages to sg table. */
            struct sg_table   sgt;
            vg_lite_uint32_t  alloc_from_res;

            /* record user data */
            vg_lite_uintptr_t user_vaddr;
            vg_lite_uint32_t  size;
            vg_lite_flag_t    vm_flags;
        } um_desc;
    };
};

/* Struct definitions. */
struct heap_node
{
    struct list_head list;
    uint32_t offset;
    unsigned long size;
    int32_t status;
};

struct client_data
{
    struct vg_lite_device *device;
    struct vm_area_struct *vm;
    void                  *contiguous_mapped;
    void                  *contiguous_klogical;
};

/* Data and objects declarations. */
static vg_lite_int32_t heap_size = 8 << 20;     /* Default heap size is 16MB. */
module_param(heap_size, int, S_IRUGO);

static vg_lite_int32_t verbose = 0;
module_param(verbose, int, S_IRUGO);

static vg_lite_int32_t cached = 0;
module_param(cached, int, S_IRUGO);

static struct vg_lite_device *device = NULL;
static struct client_data *private_data = NULL;
static vg_platform_t *platform = NULL;
static vg_module_parameters_t global_param = {0};
#ifdef BACKUP_COMMAND
static vg_lite_uint32_t global_flags = 0;
#endif

static vg_lite_error_t init_param(vg_module_parameters_t *param)
{
    if (!param)
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    param->irq_line = -1;

    param->register_mem_base = 0;
    param->register_mem_size = 0;

    param->contiguous_base = 0;
    param->contiguous_size = 0;

    return VG_LITE_SUCCESS;
}

static vg_lite_error_t sync_input_param(vg_module_parameters_t *param)
{
    if (!param)
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    param->irq_line = irqLine;

    param->register_mem_base = registerMemBase;
    param->register_mem_size = registerMemSize;

    param->contiguous_base = contiguousBase;
    param->contiguous_size = contiguousSize;

    return VG_LITE_SUCCESS;
}

static vg_lite_error_t sync_param(vg_module_parameters_t *param)
{
    if (device == NULL || param == NULL)
    {
        return VG_LITE_INVALID_ARGUMENT;
    }

    device->irq_line = param->irq_line;

    device->register_mem_base = param->register_mem_base;
    device->register_mem_size = param->register_mem_size;

    device->contiguous_base = param->contiguous_base;
    device->contiguous_size = param->contiguous_size;

    return VG_LITE_SUCCESS;
}

void vg_lite_hal_delay(uint32_t milliseconds)
{
    /* Delay the requested amount. */
    msleep(milliseconds);
}

void vg_lite_hal_barrier(void)
{
    /* Memory barrier. */
    smp_mb();

    /* Test for cached memory. */
    if (cached)
    {
        /* Flush the caches. */
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,19,0)
        flush_cache_all();
#elif defined (__arm64__) || defined (__aarch64__)
        __flush_icache_all();
#endif
    }
}

void vg_lite_hal_initialize(void)
{
    /* TODO: Turn on the power. */

    /* TODO: Turn on the clock. */
}

void vg_lite_hal_deinitialize(void)
{
    /* TODO: Remove clock. */

    /* TODO: Remove power. */
}

uint32_t vg_lite_hal_cpu_to_gpu(uint64_t cpu_physical)
{
    return (uint32_t)cpu_physical;
}

static int split_node(struct heap_node *node, unsigned long size)
{
    struct heap_node *split;

    /* Allocate a new node. */
    split = kmalloc(sizeof(struct heap_node), GFP_KERNEL);
    if (split == NULL)
    {
        return -1;
    }

    /* Fill in the data of this node of the remaning size. */
    split->offset = node->offset + size;
    split->size = node->size - size;
    split->status = 0;

    /* Add the new node behind the current node. */
    list_add(&split->list, &node->list);

    /* Adjust the size of the current node. */
    node->size = size;

    /* No error. */
    return 0;
}

vg_lite_error_t vg_lite_hal_allocate_contiguous(unsigned long size, void **logical, void **klogical,
                                                uint32_t *physical, void **node)
{
    unsigned long aligned_size;
    struct heap_node *pos;

    /* Align the size to 64 bytes. */
    aligned_size = VG_LITE_ALIGN(size, VGLITE_MEM_ALIGNMENT);

    /* Check if there is enough free memory available. */
    if (aligned_size > device->heap.free)
    {
        return VG_LITE_OUT_OF_MEMORY;
    }

    /* Walk the heap backwards. */
    list_for_each_entry_reverse(pos, &device->heap.list, list)
    {
        /* Check if the current node is free and is big enough. */
        if (pos->status == 0 && pos->size >= aligned_size)
        {
            /* See if we the current node is big enough to split. */
            if (pos->size - aligned_size >= VGLITE_MEM_ALIGNMENT)
            {
                if (0 != split_node(pos, aligned_size))
                {
                    return VG_LITE_OUT_OF_RESOURCES;
                }
            }

            /* Mark the current node as used. */
            pos->status = HEAP_NODE_USED;

            /* Return the logical/physical address. */
            *logical  = (uint8_t *)private_data->contiguous_mapped + pos->offset;
            *klogical = (uint8_t *)private_data->contiguous_klogical + pos->offset;
            *physical = vg_lite_hal_cpu_to_gpu(device->physical) + pos->offset;

            /* Update the heap free size. */
            device->heap.free -= aligned_size;

            *node = pos;
            return VG_LITE_SUCCESS;
        }
    }

    /* Out of memory. */
    return VG_LITE_OUT_OF_MEMORY;
}

void vg_lite_hal_free_contiguous(void *memory_handle)
{
    struct heap_node *pos, * node;

    /* Get pointer to node. */
    node = memory_handle;

    if (node->status != HEAP_NODE_USED)
    {
        if (verbose)
        {
            printk("vg_lite: ignoring corrupted memory handle %p\n", memory_handle);
        }
        return;
    }

    /* Mark node as free. */
    node->status = 0;

    /* Add node size to free count. */
    device->heap.free += node->size;

    /* Check if next node is free. */
    pos = node;
    list_for_each_entry_continue(pos, &device->heap.list, list)
    {
        if (pos->status == 0)
        {
            /* Merge the nodes. */
            node->size += pos->size;

            /* Delete the next node from the list. */
            list_del(&pos->list);
            kfree(pos);
        }
        break;
    }

    /* Check if the previous node is free. */
    pos = node;
    list_for_each_entry_continue_reverse(pos, &device->heap.list, list)
    {
        if (pos->status == 0)
        {
            /* Merge the nodes. */
            pos->size += node->size;

            /* Delete the current node from the list. */
            list_del(&node->list);
            kfree(node);
        }
        break;
    }
    /* TODO:the memory manager still have problem,we will refine it later.*/
    /*if(node->list.next == &device->heap.list && node->list.prev == &device->heap.list)
        kfree(node);*/
}

void vg_lite_hal_free_os_heap(void)
{
    /* TODO: Remove unfree node. */
}

uint32_t vg_lite_hal_peek(uint32_t address)
{
    /* Read data from the GPU register. */
    return *(uint32_t *)(uint8_t *)(device->register_base_mapped + address);
}

void vg_lite_hal_poke(uint32_t address, uint32_t data)
{
    /* Write data to the GPU register. */
    *(uint32_t *)(uint8_t *)(device->register_base_mapped + address) = data;
}

vg_lite_error_t vg_lite_hal_query_mem(vg_lite_kernel_mem_t *mem)
{
    if (device != NULL)
    {
        mem->bytes = device->heap.free;
        return VG_LITE_SUCCESS;
    }
    mem->bytes = 0;
    return VG_LITE_NO_CONTEXT;
}

vg_lite_error_t vg_lite_hal_map_memory(vg_lite_kernel_map_memory_t *node)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    void *_logical = NULL;
    uint64_t physical = node->physical;
    uint32_t offset = physical & (PAGE_SIZE - 1);
    uint64_t bytes = node->bytes + offset;
    uint32_t num_pages, pfn = 0;
    vg_lite_kernel_unmap_memory_t unmap_node;
    struct vm_area_struct *vma;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 4, 0)
    _logical = (void *)vm_mmap(NULL, 0L, bytes, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_NORESERVE, 0);
#else
    down_write(&current_mm_mmap_sem);
    _logical = (void *)do_mmap_pgoff(NULL, 0L, bytes,
                                     PROT_READ | PROT_WRITE, MAP_SHARED, 0);
    up_write(&current_mm_mmap_sem);
#endif

    if (!_logical)
    {
        node->logical = NULL;
        return VG_LITE_OUT_OF_MEMORY;
    }

    down_write(&current_mm_mmap_sem);

    vma = find_vma(current->mm, (unsigned long)_logical);
    if (vma == NULL)
    {
        return VG_LITE_OUT_OF_RESOURCES;
    }

    pfn = (physical >> PAGE_SHIFT);
    num_pages = GET_PAGE_COUNT(bytes, PAGE_SIZE);

    /* Make this mapping non-cached. */
    vma->vm_flags |= VM_FLAGS;

    vma->vm_page_prot = pgprot_writecombine(vma->vm_page_prot);

    if (remap_pfn_range(vma, vma->vm_start, pfn, num_pages << PAGE_SHIFT, vma->vm_page_prot) < 0)
    {
        error = VG_LITE_OUT_OF_MEMORY;
    }

    node->logical = (void *)((uint8_t *)_logical + offset);

    up_write(&current_mm_mmap_sem);

    if (error)
    {
        unmap_node.bytes = node->bytes;
        unmap_node.logical = node->logical;
        vg_lite_hal_unmap_memory(&unmap_node);
    }

    return error;
}

vg_lite_error_t vg_lite_hal_unmap_memory(vg_lite_kernel_unmap_memory_t *node)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    void *_logical;
    uint32_t bytes;
    uint32_t offset = (uint32_t)((uintptr_t)node->logical & (PAGE_SIZE - 1));

    if (unlikely(!current->mm))
    {
        return error;
    }

    _logical = (void *)((uint8_t *)node->logical - offset);
    bytes = GET_PAGE_COUNT(node->bytes + offset, PAGE_SIZE) * PAGE_SIZE;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0)
    if (vm_munmap((unsigned long)_logical, bytes) < 0)
    {
        error = VG_LITE_INVALID_ARGUMENT;
        printk("%s: vm_munmap failed\n", __func__);
    }
#else
    down_write(&current_mm_mmap_sem);
    if (do_munmap(current->mm, (unsigned long)_logical, bytes) < 0)
    {
        error = VG_LITE_INVALID_ARGUMENT;
        printk("%s: do_munmap failed\n", __func__);
    }
    up_write(&current_mm_mmap_sem);
#endif

    return error;
}

int32_t vg_lite_hal_wait_interrupt(uint32_t timeout, uint32_t mask, uint32_t *value)
{
    unsigned long jiffies;
    unsigned long result;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 3, 0)
    struct timespec64 tv;

    if (timeout == VG_LITE_INFINITE)
    {
        /* Set 1 second timeout. */
        tv.tv_sec  = 1;
        tv.tv_nsec = 0;
    }
    else
    {
        /* Convert timeout in ms to timeval. */
        tv.tv_sec  = timeout / 1000;
        tv.tv_nsec = (timeout % 1000) * 1000 * 1000;
    }

    jiffies = timespec64_to_jiffies(&tv);
#else
    struct timeval tv;

    if (timeout == VG_LITE_INFINITE)
    {
        /* Set 1 second timeout. */
        tv.tv_sec  = 1;
        tv.tv_usec = 0;
    }
    else
    {
        /* Convert timeout in ms to timeval. */
        tv.tv_sec  = timeout / 1000;
        tv.tv_usec = (timeout % 1000) * 1000;
    }

    /* Convert timeval to jiffies. */
    jiffies = timeval_to_jiffies(&tv);
#endif

    /* Wait for interrupt, ignoring timeout. */
    do
    {
        result = wait_event_interruptible_timeout(device->int_queue, device->int_flags & mask, jiffies);
    }
    while (timeout == VG_LITE_INFINITE && result == 0);

    /* Report the event(s) got. */
    if (value != NULL)
    {
        *value = device->int_flags & mask;
    }

    device->int_flags = 0;
    return (result != 0);
}

static vg_lite_int32_t import_dma_buf(struct device *dev, struct mapped_memory *mapped)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    struct sg_table *sgt = NULL;
    struct dma_buf_attachment *attachment = NULL;
    struct dma_buf *dmabuf = mapped->dmabuf_desc.usr_dmabuf;
    struct scatterlist *s = NULL;
    vg_lite_int32_t npages = 0;
    vg_lite_uintptr_t *dma_address_array = NULL;
    vg_lite_int32_t i, j, k = 0;

    get_dma_buf(dmabuf);
    attachment = dma_buf_attach(dmabuf, dev);
    if (!attachment)
    {
        vg_lite_kernel_error("dma_buf_attach failed, ");
        ONERROR(VG_LITE_NOT_SUPPORT);
    }

    sgt = dma_buf_map_attachment(attachment, DMA_BIDIRECTIONAL);
    if (!sgt)
    {
        vg_lite_kernel_error("dma_buf_map_attachment failed, ");
        ONERROR(VG_LITE_NOT_SUPPORT);
    }

    if (sgt->nents != 1)
    {
        ONERROR(VG_LITE_NOT_SUPPORT);
    }

    /* Prepare page array. */
    for_each_sg(sgt->sgl, s, sgt->orig_nents, i)
    npages += (sg_dma_len(s) + PAGE_SIZE - 1) / PAGE_SIZE;

    dma_address_array = (vg_lite_uintptr_t *)kmalloc(npages * sizeof(*dma_address_array), GFP_KERNEL);

    /* Fill page array. */
    for_each_sg(sgt->sgl, s, sgt->orig_nents, i)
    {
        for (j = 0; j < (sg_dma_len(s) + PAGE_SIZE - 1) / PAGE_SIZE; j++)
        {
            dma_address_array[k++] = sg_dma_address(s) + j * PAGE_SIZE;
        }
    }

    mapped->dmabuf_desc.dmabuf     = dmabuf;
    mapped->dmabuf_desc.dma_address_array  = dma_address_array;
    mapped->dmabuf_desc.attachment = attachment;
    mapped->dmabuf_desc.sgt        = sgt;
    mapped->dmabuf_desc.npages     = npages;

    return 0;
on_error:
    if (dma_address_array)
    {
        kfree(dma_address_array);
    }
    return -1;
}

static vg_lite_int32_t import_pfns(struct mapped_memory *mapped, vg_lite_uintptr_t logical,
                                   vg_lite_int32_t _page_count, vg_lite_uint32_t offset)
{
    vg_lite_int32_t result = 0;
    vg_lite_error_t error = 0;
    vg_lite_uint32_t i;
    struct vm_area_struct *vma;
    vg_lite_long_t *pfns;
    vg_lite_int32_t *refs;
    struct page **pages;
    vg_lite_int32_t page_count = 0;
    vg_lite_int32_t pfn_count = _page_count;
    vg_lite_uintptr_t memory = logical;

    if (!current->mm)
    {
        vg_lite_kernel_error("resource not enough, ");
        ONERROR(VG_LITE_OUT_OF_RESOURCES);
    }

    down_read(&current_mm_mmap_sem);
    vma = find_vma(current->mm, memory);
    if (!vma)
    {
        vg_lite_kernel_error("resource not enough, ");
        ONERROR(VG_LITE_OUT_OF_RESOURCES);
    }


    pfns = kzalloc(pfn_count * sizeof(unsigned long), GFP_KERNEL | __GFP_NOWARN);
    if (!pfns)
    {
        vg_lite_kernel_error("resource not enough, ");
        ONERROR(VG_LITE_OUT_OF_RESOURCES);
    }

    refs = kzalloc(pfn_count * sizeof(int), GFP_KERNEL | __GFP_NOWARN);
    if (!refs)
    {
        kfree(pfns);
        vg_lite_kernel_error("resource not enough, ");
        ONERROR(VG_LITE_OUT_OF_RESOURCES);
    }

    pages = kzalloc(pfn_count * sizeof(struct page *), GFP_KERNEL | __GFP_NOWARN);
    if (!pages)
    {
        kfree(pfns);
        kfree(refs);
        vg_lite_kernel_error("resource not enough, ");
        ONERROR(VG_LITE_OUT_OF_RESOURCES);
    }

    for (i = 0; i < pfn_count; i++)
    {
        /* protect pfns[i] */
        spinlock_t *ptl;
        pgd_t *pgd;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 9, 0)
        p4d_t *p4d;
#endif
        pud_t *pud;
        pmd_t *pmd;
        pte_t *pte;

        pgd = pgd_offset(current->mm, memory);
        if (pgd_none(*pgd) || pgd_bad(*pgd))
        {
            goto err;
        }

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 9, 0)
        p4d = p4d_offset(pgd, memory);
        if (p4d_none(READ_ONCE(*p4d)))
        {
            goto err;
        }

        pud = pud_offset(p4d, memory);
#elif (defined(CONFIG_X86)) && LINUX_VERSION_CODE >= KERNEL_VERSION (4, 12, 0)
        pud = pud_offset((p4d_t *)pgd, memory);
#elif (defined(CONFIG_CPU_CSKYV2)) && LINUX_VERSION_CODE >= KERNEL_VERSION (4, 11, 0)
        pud = pud_offset((p4d_t *)pgd, memory);
#else
        pud = pud_offset(pgd, memory);
#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(5, 9, 0) */
        if (pud_none(*pud) || pud_bad(*pud))
        {
            goto err;
        }

        pmd = pmd_offset(pud, memory);
        if (pmd_none(*pmd) || pmd_bad(*pmd))
        {
            goto err;
        }

        pte = pte_offset_map_lock(current->mm, pmd, memory, &ptl);

        if (!pte_present(*pte))
        {
            pte_unmap_unlock(pte, ptl);
            goto err;
        }

        pfns[i] = pte_pfn(*pte);
        pte_unmap_unlock(pte, ptl);

        /* Advance to next. */
        memory += PAGE_SIZE;
    }

    for (i = 0; i < pfn_count; i++)
    {
        if (pfn_valid(pfns[i]))
        {
            struct page *page = pfn_to_page(pfns[i]);

            refs[i] = get_page_unless_zero(page);
            pages[i] = page;
            page_count++;
        }
    }

    if (page_count != pfn_count)
    {
        vg_lite_kernel_hintmsg("page_count != pfn_count\n");
    }

    for (i = 1; i < pfn_count; i++)
    {
        if (pfns[i] != pfns[i - 1] + 1)
        {
            vg_lite_kernel_error("Not contiguous memory, vglite no support!\n");
            goto err;
        }
    }

    /* if physical from reserved memory */
    if (device->contiguous_base)
    {
        vg_lite_uintptr_t phy_addr;

        phy_addr = (vg_lite_uintptr_t)pfns[0] << PAGE_SHIFT;
        if (phy_addr >= device->contiguous_base &&
            phy_addr < device->contiguous_base + device->contiguous_size)
        {
            mapped->um_desc.alloc_from_res = 1;
        }
    }

    /* TODO: Map pages to sg table. */
    if (!mapped->um_desc.alloc_from_res)
    {
        vg_lite_kernel_hintmsg("This user memory is not from reserved memory.\n");
        result = sg_alloc_table_from_pages(&mapped->um_desc.sgt, pages, page_count, memory & ~PAGE_MASK,
                                           page_count * PAGE_SIZE, GFP_KERNEL | __GFP_NOWARN);
        if (unlikely(result < 0))
        {
            vg_lite_kernel_error("sg_alloc_table_from_pages failed, ");
            ONERROR(VG_LITE_OUT_OF_RESOURCES);
        }

        result = dma_map_sg(&device->pdev->dev, mapped->um_desc.sgt.sgl, mapped->um_desc.sgt.nents,
                            DMA_TO_DEVICE);
        if (unlikely(result != mapped->um_desc.sgt.nents))
        {
            sg_free_table(&mapped->um_desc.sgt);
            vg_lite_kernel_error("dma_map_sg failed, ");
            ONERROR(VG_LITE_OUT_OF_RESOURCES);
        }

        /* flush cache */
        dma_sync_sg_for_cpu(&device->pdev->dev, mapped->um_desc.sgt.sgl, mapped->um_desc.sgt.nents,
                            DMA_FROM_DEVICE);
        dma_sync_sg_for_device(device->dev, mapped->um_desc.sgt.sgl, mapped->um_desc.sgt.nents,
                               DMA_TO_DEVICE);
    }

    kfree(pages);
    pages = NULL;

    mapped->um_desc.physical = (vg_lite_uintptr_t)((vg_lite_uint8_t *)(pfns[0] << PAGE_SHIFT) + offset);
    mapped->um_desc.type     = UM_PFN_MAP;
    mapped->um_desc.pfns     = pfns;
    mapped->um_desc.refs     = refs;

    return 0;

err:
    kfree(pfns);
    kfree(refs);
    kfree(pages);
    vg_lite_kernel_error("not find pte...\n");

on_error:
    return -1;
}

static vg_lite_int32_t import_pages(struct mapped_memory *mapped, vg_lite_uintptr_t logical,
                                    vg_lite_int32_t _page_count, vg_lite_uint32_t offset, vg_lite_uint32_t _size, vg_lite_flag_t flags)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_int32_t i;
    vg_lite_int32_t result;
    vg_lite_int32_t page_count = _page_count;
    vg_lite_uintptr_t addr = logical;
    vg_lite_uintptr_t vm_flags = flags;
    struct page **pages;

    pages = kcalloc(page_count, sizeof(struct page *), GFP_KERNEL | __GFP_NOWARN);
    if (!pages)
    {
        vg_lite_kernel_error("out of memory, ");
        ONERROR(VG_LITE_OUT_OF_RESOURCES);
    }

    down_read(&current_mm_mmap_sem);
#if LINUX_VERSION_CODE > KERNEL_VERSION(5, 6, 0)
    result = pin_user_pages(addr & PAGE_MASK, page_count,
#elif LINUX_VERSION_CODE < KERNEL_VERSION(4, 6, 0)
    result = get_user_pages(current, current->mm, addr & PAGE_MASK, page_count,
#else
    result = get_user_pages(addr & PAGE_MASK, page_count,
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 9, 0) || defined(CONFIG_PPC)
                            (vm_flags & VM_WRITE) ? FOLL_WRITE : 0,
#else
                            (vm_flags & VM_WRITE) ? 1 : 0, 0,
#endif
                            pages, NULL);
    up_read(&current_mm_mmap_sem);

    if (result < page_count)
    {
        for (i = 0; i < result; i++)
        {
            if (pages[i])
#if LINUX_VERSION_CODE > KERNEL_VERSION(5, 6, 0)
                unpin_user_page(pages[i]);
#else
                put_page(pages[i]);
#endif
        }

        kfree(pages);
        vg_lite_kernel_error("user pages incorrect..., ");
        ONERROR(VG_LITE_INVALID_ARGUMENT);
    }

    for (i = 1; i < page_count; i++)
    {
        if (page_to_pfn(pages[i]) != page_to_pfn(pages[i - 1]) + 1)
        {
            vg_lite_kernel_error("Not contiguous memory, vglite no support, ");
            ONERROR(VG_LITE_NOT_SUPPORT);
        }

    }

    /* if physical from reserved memory */
    if (device->contiguous_base)
    {
        vg_lite_uintptr_t phy_addr;

        phy_addr = (vg_lite_uintptr_t)page_to_pfn(pages[0]) << PAGE_SHIFT;
        if (phy_addr >= device->contiguous_base &&
            phy_addr < device->contiguous_base + device->contiguous_size)
        {
            mapped->um_desc.alloc_from_res = 1;
        }
    }

    /* TODO: Map pages to sg table. */
    if (!mapped->um_desc.alloc_from_res)
    {
        vg_lite_kernel_hintmsg("This user memory is not from reserved memory.\n");
        result = sg_alloc_table_from_pages(&mapped->um_desc.sgt, pages, page_count, addr & ~PAGE_MASK,
                                           page_count * PAGE_SIZE, GFP_KERNEL | __GFP_NOWARN);
        if (unlikely(result < 0))
        {
            vg_lite_kernel_error("sg_alloc_table_from_pages failed, ");
            ONERROR(VG_LITE_OUT_OF_RESOURCES);
        }

        result = dma_map_sg(&device->pdev->dev, mapped->um_desc.sgt.sgl, mapped->um_desc.sgt.nents,
                            DMA_TO_DEVICE);
        if (unlikely(result != mapped->um_desc.sgt.nents))
        {
            sg_free_table(&(mapped->um_desc.sgt));
            vg_lite_kernel_error("dma_map_sg failed, ");
            ONERROR(VG_LITE_OUT_OF_RESOURCES);
        }

        /* flush cache */
        dma_sync_sg_for_cpu(&device->pdev->dev, mapped->um_desc.sgt.sgl, mapped->um_desc.sgt.nents,
                            DMA_FROM_DEVICE);
        dma_sync_sg_for_device(&device->pdev->dev, mapped->um_desc.sgt.sgl, mapped->um_desc.sgt.nents,
                               DMA_TO_DEVICE);
    }

    mapped->um_desc.physical = (vg_lite_uintptr_t)(page_to_phys(pages[0]) + offset);
    mapped->um_desc.type     = UM_PAGE_MAP;
    mapped->um_desc.pages    = pages;

    return error;

on_error:
    error = -1;
    return error;
}

#if defined(CONFIG_DMA_SHARED_BUFFER)
vg_lite_int32_t vg_lite_kernel_get_sgt(vg_lite_kernel_allocate_t *node, vg_lite_pointer *_sgt)
{
    vg_lite_uintptr_t physical = 0;
    vg_lite_uint32_t bytes = node->bytes;
    vg_lite_uint32_t num_pages = PAGE_ALIGN(bytes) >> PAGE_SHIFT;
    vg_lite_uint32_t flags = node->flags;
    struct sg_table *sgt = NULL;
    struct page *page = NULL;
    struct page **pages = NULL;
    vg_lite_uint32_t i;

    if ((flags & VG_LITE_MEMORY_ALLOCATOR_FLAG) == 0)
    {
        vg_lite_kernel_error("allocate memory not support!\n");
        return -1;
    }

    sgt = kmalloc(sizeof(*sgt), GFP_KERNEL | __GFP_NOWARN);
    if (!sgt)
    {
        vg_lite_kernel_error("kmalloc failed!\n");
        return -1;
    }

    physical = node->memory_gpu;

    if (flags == VG_LITE_RESERVED_ALLOCATOR)
    {
        vg_lite_kernel_hintmsg("call VG_LITE_RESERVE_ALLOCATOR!\n");

        page = pfn_to_page(physical >> PAGE_SHIFT);

        if (sg_alloc_table(sgt, 1, GFP_KERNEL))
        {
            vg_lite_kernel_error("sg_alloc_table!\n");
            return -1;
        }

        sg_set_page(sgt->sgl, page, PAGE_ALIGN(bytes), 0);

        *_sgt = (vg_lite_pointer)sgt;

        return 0;

    }
    else if (flags == VG_LITE_GFP_ALLOCATOR)
    {
        vg_lite_kernel_hintmsg("call VG_LITE_GFP_ALLOCATOR!\n");
        pages = kmalloc_array(num_pages, sizeof(struct page *), GFP_KERNEL | __GFP_NOWARN);
        if (!pages)
        {
            vg_lite_kernel_error("kmalloc_array failed!\n");
            return -1;
        }

        page = pfn_to_page(physical >> PAGE_SHIFT);

        for (i = 0; i < num_pages; ++i)
        {
            pages[i] = nth_page(page, i);
        }
    }
    else if (flags == VG_LITE_DMA_ALLOCATOR)
    {

    }
    else
    {
        vg_lite_kernel_hintmsg("allocate memory not support!\n");
        return -1;
    }

    /* Here we only need to get the sgt of GFP and DMA alloctor, reserved memory returned above. */
    if (sg_alloc_table_from_pages(sgt, pages, num_pages, 0, bytes, GFP_KERNEL) < 0)
    {
        vg_lite_kernel_error("sg_alloc_table_from_pages fail!\n");
        return -1;
    }

    *_sgt = (vg_lite_pointer)sgt;

    return 0;
}

/* called by dma_buf_attach() */
# if LINUX_VERSION_CODE > KERNEL_VERSION(4, 18, 20)
static vg_lite_int32_t dmabuf_attach(struct dma_buf *dmabuf, struct dma_buf_attachment *attachment)
# else
static vg_lite_int32_t dmabuf_attach(struct dma_buf *dmabuf, struct device *dev,
                                     struct dma_buf_attachment *attachment)
# endif
{
    return 0;
}

/* called by dma_buf_detach() */
static void dmabuf_detach(struct dma_buf *dmabuf, struct dma_buf_attachment *attachment)
{

}


/* called by dma_buf_map_attachment() */
static struct sg_table *dmabuf_map_dma_buf(struct dma_buf_attachment *attachment,
                                           enum dma_data_direction direction)
{
    struct sg_table *sgt = NULL;
    struct dma_buf *dmabuf = attachment->dmabuf;
    vg_lite_kernel_allocate_t *_memory_node = dmabuf->priv;
# if LINUX_VERSION_CODE < KERNEL_VERSION(4, 8, 0)
    DEFINE_DMA_ATTRS(attrs);
# else
    vg_lite_long_t attrs = 0;
# endif
    vg_lite_int32_t ret = 0;

    ret = vg_lite_kernel_get_sgt(_memory_node, (void **)&sgt);
    if (ret != 0)
    {
        vg_lite_kernel_error("vg_lite_kernel_get_sgt fail!\n");
        return NULL;
    }

# if LINUX_VERSION_CODE < KERNEL_VERSION(4, 8, 0)
    if (dma_map_sg_attrs(attachment->dev, sgt->sgl, sgt->nents, direction, &attrs) == 0)
# else
    if (dma_map_sg_attrs(attachment->dev, sgt->sgl, sgt->nents, direction, attrs) == 0)
# endif
    {
        sg_free_table(sgt);
        kfree(sgt);
        sgt = NULL;
        vg_lite_kernel_error("dma_map_sg_attrs fail!\n");
        return NULL;
    }

    return sgt;
}

/* called by dma_buf_unmap_attachment() */
static void dmabuf_unmap_dma_buf(struct dma_buf_attachment *attachment,
                                 struct sg_table *sgt, enum dma_data_direction direction)
{
# if LINUX_VERSION_CODE < KERNEL_VERSION(4, 8, 0)
    DEFINE_DMA_ATTRS(attrs);
# else
    vg_lite_long_t attrs = 0;
# endif

# if LINUX_VERSION_CODE < KERNEL_VERSION(4, 8, 0)
    dma_unmap_sg_attrs(attachment->dev, sgt->sgl, sgt->nents, direction, &attrs);
# else
    dma_unmap_sg_attrs(attachment->dev, sgt->sgl, sgt->nents, direction, attrs);
# endif

    sg_free_table(sgt);
    kfree(sgt);
}

/* called by user api mmap */
static vg_lite_int32_t dmabuf_mmap(struct dma_buf *dmabuf, struct vm_area_struct *vma)
{
    return 0;
}

/* called by dma_buf_put()  */
static void dmabuf_release(struct dma_buf *dmabuf)
{
    vg_lite_kernel_allocate_t *node = NULL;

    node = dmabuf->priv;

    kfree(node->kmemory);
    kfree(node);
}

static struct dma_buf_ops dmabuf_ops =
{
    .attach        = dmabuf_attach,
    .detach        = dmabuf_detach,
    .map_dma_buf   = dmabuf_map_dma_buf,
    .unmap_dma_buf = dmabuf_unmap_dma_buf,
    .mmap          = dmabuf_mmap,
    .release       = dmabuf_release,
};
#endif

vg_lite_error_t vg_lite_hal_memory_export(int32_t *fd)
{
#if defined(CONFIG_DMA_SHARED_BUFFER)
    vg_lite_kernel_allocate_t *memory_node = NULL;
    struct dma_buf *dmabuf = NULL;
    vg_lite_pointer addr = NULL;
    vg_lite_uint32_t offset = 0;
    vg_lite_int32_t _fd = -1;
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_uint32_t allocater_flags = VG_LITE_GFP_ALLOCATOR;

    memory_node = kmalloc(sizeof(memory_node), GFP_KERNEL);
    if (!memory_node)
    {
        vg_lite_kernel_error("kmalloc failed, ");
        ONERROR(VG_LITE_OUT_OF_RESOURCES);
    }
    memset(memory_node, 0, sizeof(*memory_node));

    if (allocater_flags == VG_LITE_GFP_ALLOCATOR)
    {
        addr = kmalloc(PAGE_SIZE, GFP_KERNEL);
        if (!addr)
        {
            vg_lite_kernel_error("kmalloc failed, ");
            ONERROR(VG_LITE_OUT_OF_RESOURCES);
        }
        memset(addr, 0, PAGE_SIZE);
        sprintf(addr, "dma_buf GFP test!");

        memory_node->flags = VG_LITE_GFP_ALLOCATOR;
        memory_node->bytes = PAGE_SIZE;
        memory_node->kmemory = addr;
        memory_node->memory_gpu = virt_to_phys(addr);

        offset = (vg_lite_uintptr_t)addr & (PAGE_SIZE - 1);

        vg_lite_kernel_print("memory_node.bytes = %d\n", memory_node->bytes);
        vg_lite_kernel_print("memory_node.bytes offset = %d\n", offset);
    }
    else if (allocater_flags == VG_LITE_RESERVED_ALLOCATOR)
    {
        memory_node->bytes = PAGE_SIZE;
        memory_node->flags = VG_LITE_RESERVED_ALLOCATOR;

        vg_lite_hal_allocate_contiguous(memory_node->bytes, &memory_node->memory, &memory_node->kmemory,
                                        &memory_node->memory_gpu, &memory_node->memory_handle);

        sprintf(memory_node->kmemory, "dma_buf reserve test!");
        offset = (vg_lite_uintptr_t)memory_node->kmemory & (PAGE_SIZE - 1);

        vg_lite_kernel_print("memory_node.bytes = %d\n", memory_node->bytes);
        vg_lite_kernel_print("memory_node.bytes offset = %d\n", offset);
    }
    else if (allocater_flags == VG_LITE_DMA_ALLOCATOR)
    {

    }
    else
    {
        vg_lite_kernel_hintmsg("dmabuf not support this allocater!\n");
        ONERROR(VG_LITE_NOT_SUPPORT);
    }

# if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 1, 0)
    DEFINE_DMA_BUF_EXPORT_INFO(exp_info);
    exp_info.ops = &dmabuf_ops;
    exp_info.size = memory_node->bytes;
    exp_info.flags = O_CLOEXEC;
    exp_info.priv = memory_node;
    dmabuf = dma_buf_export(&exp_info);
# elif LINUX_VERSION_CODE >= KERNEL_VERSION(3, 17, 0)
    dmabuf = dma_buf_export(memory_node, &dmabuf_ops, memory_node->bytes, O_CLOEXEC, NULL);
# else
    dmabuf = dma_buf_export(memory_node, &dmabuf_ops, memory_node->bytes, O_CLOEXEC);
# endif

    _fd = dma_buf_fd(dmabuf, O_CLOEXEC);

    *fd = _fd;

    return error;
on_error:
    return error;
#endif
}

void *vg_lite_hal_map(uint32_t flags, uint32_t bytes, void *logical, uint32_t physical,
                      int32_t dma_buf_fd, uint32_t *gpu)
{
    struct mapped_memory *mapped;
    vg_lite_int32_t ret = -1;

    mapped = kmalloc(sizeof(struct mapped_memory), GFP_KERNEL);
    if (mapped == NULL)
    {
        return NULL;
    }
    memset(mapped, 0, sizeof(struct mapped_memory));

    if (flags == VG_LITE_HAL_MAP_DMABUF)
    {
        /* TODO: complete dma_buf importer*/
        struct dma_buf *dmabuf = NULL;

        dmabuf = dma_buf_get(dma_buf_fd);
        if (IS_ERR(dmabuf))
        {
            vg_lite_kernel_error("dma_buf_get invalid parameter!\n");
            return NULL;
        }

        dma_buf_put(dmabuf);
        mapped->dmabuf_desc.usr_dmabuf = dmabuf;
        mapped->flags = VG_LITE_HAL_MAP_DMABUF;

        ret = import_dma_buf(&device->pdev->dev, mapped);

        vg_lite_kernel_print("mapped->dmabuf_desc.dma_address_array = %s\n",
                             (char *)phys_to_virt(mapped->dmabuf_desc.dma_address_array[0]));

    }
    else if (flags == VG_LITE_HAL_MAP_USER_MEMORY)
    {

        struct vm_area_struct *vma = NULL;
        vg_lite_int32_t page_count;
        vg_lite_flag_t vm_flags;
        vg_lite_uintptr_t memory = (vg_lite_uintptr_t)logical;
        vg_lite_uint32_t offset = memory & (PAGE_SIZE - 1);

        if (logical != NULL)
        {
            /* check argument */
            if ((memory + bytes) < memory)
            {
                vg_lite_kernel_error("vg_lite_hal_map argument invalid!\n");
                return NULL;
            }

            /* Get the number of required pages. */
            page_count = (bytes + offset + PAGE_SIZE - 1) / PAGE_SIZE;

            /* check all vma */
            if (memory)
            {
                vg_lite_uintptr_t vaddr = memory;

                down_read(&current_mm_mmap_sem);
                vma = find_vma(current->mm, memory);
                up_read(&current_mm_mmap_sem);

                /* No such memory, or across vmas. */
                if (!vma)
                {
                    vg_lite_kernel_error("vg_lite_hal_map argument invalid!\n");
                    return NULL;
                }

#ifdef CONFIG_ARM
                /* coherent cache in case vivt or vipt-aliasing cache. */
                __cpuc_flush_user_range(memory, memory + bytes, vma->vm_flags);
                vg_lite_kernel_hintmsg("__cpuc_flush_user_range!\n");
#endif
                vm_flags = vma->vm_flags;
                vaddr = vma->vm_end;

                down_read(&current_mm_mmap_sem);
                while (vaddr < memory + bytes)
                {
                    vg_lite_kernel_hintmsg("scan vma!\n");
                    vma = find_vma(current->mm, vaddr);

                    if (!vma)
                    {
                        /* No such memory. */
                        vg_lite_kernel_error("vg_lite_hal_map argument invalid!\n");
                        return NULL;
                    }

                    if ((vma->vm_flags & VM_PFNMAP) != (vm_flags & VM_PFNMAP))
                    {
                        /* Can not support different map type: both PFN and PAGE detected. */
                        up_read(&current_mm_mmap_sem);
                        vg_lite_kernel_error("Can not support different map type: both PFN and PAGE detected.!\n");
                        return NULL;
                    }

                    vaddr = vma->vm_end;
                }
                up_read(&current_mm_mmap_sem);
            } /* if memory */
        }
        else
        {
            page_count = 0;
        }

        mapped->um_desc.logical = logical;
        mapped->um_desc.physical = physical;
        mapped->um_desc.page_count = page_count;
        mapped->um_desc.alloc_from_res = 0;
        mapped->um_desc.user_vaddr = memory;
        mapped->um_desc.size = bytes;
        mapped->um_desc.vm_flags = vm_flags;
        mapped->flags = VG_LITE_HAL_MAP_USER_MEMORY;

        if (logical == NULL)
        {
            *gpu = physical;
        }
        else
        {
            if (vm_flags & VM_PFNMAP)
            {
                vg_lite_kernel_hintmsg("import_pfns: \n");
                ret = import_pfns(mapped, memory, page_count, offset);
            }
            else
            {
                vg_lite_kernel_hintmsg("import_pages: \n");
                ret = import_pages(mapped, memory, page_count, offset, bytes, vm_flags);
            }
            *gpu = mapped->um_desc.physical;
        }

        vg_lite_kernel_hintmsg("vg_lite_hal_map: memory   = %p\n", (vg_lite_uintptr_t *)memory);
        vg_lite_kernel_hintmsg("vg_lite_hal_map: offset   = %d\n", offset);
        vg_lite_kernel_hintmsg("vg_lite_hal_map: physical = %p\n",
                               (vg_lite_uintptr_t *)mapped->um_desc.physical);
    }
    else
    {
        vg_lite_kernel_hintmsg("vg_lite_hal_map: this map type not support!\n");
        return NULL;
    }

    return mapped;
}

void vg_lite_hal_unmap(void *handle)
{
    struct mapped_memory *mapped = handle;
    struct page **pages = mapped->um_desc.pages;
    vg_lite_int32_t i;

    if (mapped->flags == VG_LITE_HAL_MAP_USER_MEMORY)
    {
        if (mapped->um_desc.page_count)
        {
            switch (mapped->um_desc.type)
            {
            case UM_PAGE_MAP:
                for (i = 0; i < mapped->um_desc.page_count; i++)
                {
                    if (pages[i] != NULL)
                    {
                        if (!PageReserved(pages[i]))
                        {
                            SetPageDirty(pages[i]);
                        }
#if LINUX_VERSION_CODE > KERNEL_VERSION(5, 6, 0)
                        unpin_user_page(pages[i]);
#else
                        put_page(pages[i]);
#endif
                    }
                }
                kfree(pages);
                break;

            case UM_PFN_MAP:
                for (i = 0; i < mapped->um_desc.page_count; i++)
                {
                    if (pfn_valid(mapped->um_desc.pfns[i]))
                    {
                        struct page *page = pfn_to_page(mapped->um_desc.pfns[i]);

                        if (!PageReserved(page))
                        {
                            SetPageDirty(page);
                        }

                        if (mapped->um_desc.refs[i])
#if LINUX_VERSION_CODE > KERNEL_VERSION(5, 6, 0)
                            unpin_user_page(page);
#else
                            put_page(page);
#endif
                    }
                }

                kfree(mapped->um_desc.pfns);
                kfree(mapped->um_desc.refs);
                break;

            default:
                break;
            }
        }
    }
    else if (mapped->flags == VG_LITE_HAL_MAP_DMABUF)
    {
        dma_buf_unmap_attachment(mapped->dmabuf_desc.attachment, mapped->dmabuf_desc.sgt,
                                 DMA_BIDIRECTIONAL);

        dma_buf_detach(mapped->dmabuf_desc.dmabuf, mapped->dmabuf_desc.attachment);

        dma_buf_put(mapped->dmabuf_desc.dmabuf);
    }
    else
    {
        vg_lite_kernel_hintmsg("vg_lite_hal_map: this map type not support!\n");
    }

    kfree(mapped);
}

vg_lite_error_t vg_lite_hal_operation_cache(void *handle, vg_lite_cache_op_t cache_op)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    struct mapped_memory *mapped = (struct mapped_memory *)handle;
    struct sg_table *sgt = &mapped->um_desc.sgt;
    struct device *dev = &device->pdev->dev;

#ifdef CONFIG_ARM
    /* coherent cache in case vivt or vipt-aliasing cache. */
    __cpuc_flush_user_range(mapped->um_desc.user_vaddr,
                            mapped->um_desc.user_vaddr + mapped->um_desc.size, mapped->um_desc.vm_flags);
#endif

    switch (cache_op)
    {
    case VG_LITE_CACHE_CLEAN:
        dma_sync_sg_for_device(dev, sgt->sgl, sgt->nents, DMA_TO_DEVICE);
        break;

    case VG_LITE_CACHE_INVALIDATE:
        dma_sync_sg_for_cpu(dev, sgt->sgl, sgt->nents, DMA_FROM_DEVICE);
        break;

    case VG_LITE_CACHE_FLUSH:
        dma_sync_sg_for_device(dev, sgt->sgl, sgt->nents, DMA_TO_DEVICE);
        dma_sync_sg_for_cpu(dev, sgt->sgl, sgt->nents, DMA_FROM_DEVICE);
        break;

    default:
        break;
    }

    return error;
}

#if 0
static int32_t unmap_contiguous_memory_from_user(void)
{
    vg_lite_pointer  _logical;
    vg_lite_uint32_t bytes;
    vg_lite_uint32_t offset;

    if (unlikely(!current->mm))
    {
        return -1;
    }

    offset = (vg_lite_uintptr_t)private_data->contiguous_mapped & (PAGE_SIZE - 1);
    _logical = (vg_lite_pointer)((vg_lite_uint8_t *)private_data->contiguous_mapped - offset);
    bytes = private_data->device->size + offset;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0)
    if (vm_munmap((unsigned long)_logical, bytes) < 0)
    {
        vg_lite_kernel_error("vm_munmap failed\n");
    }
#else
    down_write(&current_mm_mmap_sem);
    if (do_munmap(current->mm, (unsigned long)_logical, bytes) < 0)
    {
        vg_lite_kernel_error("do_munmap failed\n");
    }

    up_write(&current_mm_mmap_sem);
#endif

    return 0;
}
#endif

#ifdef USE_RESERVE_MEMORY
static void *map_contiguous_memory_to_kernel(vg_lite_uintptr_t physical, vg_lite_uint32_t size)
{
    struct resource *region = NULL;
    void *contiguous_klogical = NULL;
    vg_lite_error_t error = VG_LITE_SUCCESS;

    region = request_mem_region(physical, size, "vglite contiguous memory");
    if (!region)
    {
        vg_lite_kernel_error("request mem %s(0x%llx - 0x%llx) failed, ",
                             "vglite contiguous memory", physical, physical + size - 1);
        ONERROR(VG_LITE_OUT_OF_RESOURCES);
    }

# if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
    contiguous_klogical = ioremap(physical, size);
# else
    contiguous_klogical = ioremap_nocache(physical, size);
# endif
    if (!contiguous_klogical)
    {
        release_mem_region(physical, size);
        vg_lite_kernel_error("map contiguous memory(physical:0x%llx size:0x%x) failed.\n", physical, size);
        ONERROR(VG_LITE_OUT_OF_RESOURCES);
    }
    vg_lite_kernel_hintmsg("contiguous memory map to kernel success!\n");

    return contiguous_klogical;
on_error:
    return NULL;
}

static void unmap_contiguous_memory_from_kernel(void *klogical, uint64_t physical, uint32_t size)
{
    if (klogical != NULL)
    {
        release_mem_region(physical, size);
        iounmap(klogical);
    }
}
#endif

int drv_open(struct inode *inode, struct file *file)
{
    struct client_data *data;

    data = kmalloc(sizeof(struct client_data), GFP_KERNEL);
    if (data == NULL)
    {
        return -1;
    }

    data->device = device;
    data->contiguous_mapped   = NULL;
    data->contiguous_klogical = NULL;

    file->private_data = data;

    return 0;
}

int drv_release(struct inode *inode, struct file *file)
{
    struct client_data *data = (struct client_data *) file->private_data;

    if (data != NULL)
    {
        if (data->contiguous_mapped != NULL)
        {

        }

        kfree(data);
        file->private_data = NULL;
    }

    return 0;
}

long drv_ioctl(struct file *file, unsigned int ioctl_code, unsigned long arg)
{
    struct ioctl_data arguments;
    void *data;

    private_data = (struct client_data *) file->private_data;
    if (private_data == NULL)
    {
        return -1;
    }

    if (ioctl_code != VG_LITE_IOCTL)
    {
        return -1;
    }

    if ((void *) arg == NULL)
    {
        return -1;
    }

    if (copy_from_user(&arguments, (void *) arg, sizeof(arguments)) != 0)
    {
        return -1;
    }

    data = kmalloc(arguments.bytes, GFP_KERNEL);
    if (data == NULL)
    {
        return -1;
    }

    if (copy_from_user(data, arguments.buffer, arguments.bytes) != 0)
    {
        goto error;
    }

    arguments.error = vg_lite_kernel(arguments.command, data);

    if (copy_to_user(arguments.buffer, data, arguments.bytes) != 0)
    {
        goto error;
    }

    kfree(data);

    if (copy_to_user((void *) arg, &arguments, sizeof(arguments)) != 0)
    {
        return -1;
    }

    return 0;

error:
    kfree(data);
    return -1;
}

ssize_t drv_read(struct file *file, char *buffer, size_t length, loff_t *offset)
{
    struct client_data *private = (struct client_data *) file->private_data;

    if (length != 4)
    {
        return 0;
    }

    if (copy_to_user((void __user *) buffer, (const void *) &private->device->size,
                     sizeof(private->device->size)) != 0)
    {
        return 0;
    }

    return 4;
}

int drv_mmap(struct file *file, struct vm_area_struct *vm)
{
    vg_lite_long_t size;
    struct client_data *private = (struct client_data *) file->private_data;
    vg_lite_uint32_t offset = private->device->physical & (PAGE_SIZE - 1);
    vg_lite_uint32_t num_pages = 0;

    if (!cached)
#if defined (__arm64__) || defined (__aarch64__)
        vm->vm_page_prot = pgprot_writecombine(vm->vm_page_prot);
#else
        vm->vm_page_prot = pgprot_noncached(vm->vm_page_prot);
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(3,7,0)
    vm->vm_flags |= VM_RESERVED;
#else
    vm->vm_flags |= (VM_DONTEXPAND | VM_DONTDUMP);
#endif
    vm->vm_pgoff = 0;

    size = vm->vm_end - vm->vm_start;
    if (size > (private->device->size + offset))
    {
        size = private->device->size + offset;
    }

    num_pages = (size + PAGE_SIZE - 1) / PAGE_SIZE;
    vg_lite_kernel_hintmsg("offset = %d, size = %ld, num_pages << PAGE_SHIFT = %d\n", offset, size,
                           num_pages << PAGE_SHIFT);

    if (remap_pfn_range(vm, vm->vm_start, private->device->physical >> PAGE_SHIFT,
                        num_pages << PAGE_SHIFT, vm->vm_page_prot) < 0)
    {
        vg_lite_kernel_error("remap_pfn_range failed\n");
        return -1;
    }

    private->vm = vm;
    private->contiguous_mapped   = (vg_lite_pointer)((vg_lite_uint8_t *)vm->vm_start + offset);
    private->contiguous_klogical = private->device->virtual;

    if (verbose)
    {
        vg_lite_kernel_hintmsg("mapped %scached contiguous memory to %p\n", cached ? "" : "non-",
                               private->contiguous_mapped);
    }

    return 0;
}

static struct file_operations file_operations =
{
    .owner          = THIS_MODULE,
    .open           = drv_open,
    .release        = drv_release,
    .read           = drv_read,
    .unlocked_ioctl = drv_ioctl,
#if LINUX_VERSION_CODE <= KERNEL_VERSION(5, 8, 18)
# ifdef HAVE_COMPAT_IOCTL
    .compat_ioctl   = drv_ioctl,
# endif
#else
    .compat_ioctl   = drv_ioctl,
#endif
    .mmap           = drv_mmap,
};

static void vg_lite_exit(void)
{
    struct heap_node *pos;
    struct heap_node *n;

    /* Check for valid device. */
    if (device != NULL)
    {
        if (device->register_base_mapped != NULL)
        {
            /* Unmap the GPU registers. */
            iounmap(device->register_base_mapped);
            device->register_base_mapped = NULL;
        }

        if (device->pages != NULL)
        {
            /* Free the contiguous memory. */
            __free_pages(device->pages, device->order);
        }

        if (device->irq_enabled)
        {
            /* Free the IRQ. */
            free_irq(device->irq_line/*GPU_IRQ*/, device);
        }

        /* Process each node. */
        list_for_each_entry_safe(pos, n, &device->heap.list, list)
        {
            /* Remove it from the linked list. */
            list_del(&pos->list);

            /* Free up the memory. */
            kfree(pos);
        }

        if (device->created)
        {
            /* Destroy the device. */
            device_destroy(device->class, MKDEV(device->major, 0));
        }

        if (device->class != NULL)
        {
            /* Destroy the class. */
            class_destroy(device->class);
        }

        if (device->registered)
        {
            /* Unregister the device. */
            unregister_chrdev(device->major, "vg_lite");
        }

#ifdef USE_RESERVE_MEMORY
        if (device->virtual != NULL)
        {
            unmap_contiguous_memory_from_kernel(device->virtual, device->physical, device->size);
        }
#endif

        /* Free up the device structure. */
        kfree(device);
    }
}

static irqreturn_t irq_hander(int irq, void *context)
{
    struct vg_lite_device *device = context;

    /* Read interrupt status. */
    vg_lite_uint32_t flags = *(vg_lite_uint32_t *)(vg_lite_uint8_t *)(device->register_base_mapped +
                                                                      VG_LITE_INTR_STATUS);
    if (flags)
    {
        /* Combine with current interrupt flags. */
        device->int_flags |= flags;

        /* Wake up any waiters. */
        wake_up_interruptible(&device->int_queue);

        /* We handled the IRQ. */
        return IRQ_HANDLED;
    }

    /* Not our IRQ. */
    return IRQ_NONE;
}

static vg_lite_error_t vg_lite_init(struct platform_device *pdev)
{
    struct heap_node *node;
    vg_lite_uint32_t size;
    vg_lite_int8_t ch;
    vg_lite_error_t error = VG_LITE_SUCCESS;

    device->pdev = pdev;

    /* Map the GPU registers. */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
    device->register_base_mapped = ioremap(device->register_mem_base/*GPU_REG_START*/,
                                           device->register_mem_size/* GPU_REG_SIZE*/);
#else
    device->register_base_mapped = ioremap_nocache(device->register_mem_base/*GPU_REG_START*/,
                                                   device->register_mem_size/*GPU_REG_SIZE*/);
#endif
    if (device->register_base_mapped == NULL)
    {
        vg_lite_exit();
        vg_lite_kernel_error("ioremap failed, ");
        ONERROR(VG_LITE_OUT_OF_RESOURCES);
    }

    /* Allocate the contiguous memory. */
#ifndef USE_RESERVE_MEMORY
    /* Allocate the contiguous memory. */
    for (device->order = get_order(heap_size); device->order > 0; device->order--)
    {
        /* Allocate with the current amount. */
        device->pages = alloc_pages(GFP_KERNEL, device->order);
        if (device->pages != NULL)
        {
            break;
        }
    }

    /* Check if we allocated any contiguous memory or not. */
    if (device->pages == NULL)
    {
        vg_lite_exit();
        vg_lite_kernel_error("alloc_pages failed, ");
        ONERROR(VG_LITE_OUT_OF_RESOURCES);
    }

    /* Save contiguous memory. */
    device->virtual = page_address(device->pages);
    device->physical = virt_to_phys(device->virtual);
    device->size = ((1 << (device->order + PAGE_SHIFT)) > MAX_CONTIGUOUS_SIZE) ? MAX_CONTIGUOUS_SIZE :
                   (1 << (device->order + PAGE_SHIFT));
#else
    device->physical = device->contiguous_base;
    device->size = (device->contiguous_size > MAX_CONTIGUOUS_SIZE) ? MAX_CONTIGUOUS_SIZE :
                   device->contiguous_size;
    device->virtual = map_contiguous_memory_to_kernel(device->physical, device->size);
#endif

    size = (device->size >> 20) ? (device->size >> 20) : (device->size >> 10);
    ch = (device->size >> 20) ? 'M' : 'k';
    vg_lite_kernel_hintmsg("allocated a %u%cB heap at 0x%08llx\n", size, ch, device->physical);

    /* Create the heap. */
    INIT_LIST_HEAD(&device->heap.list);
    device->heap.free = device->size;

    node = kmalloc(sizeof(struct heap_node), GFP_KERNEL);
    if (node == NULL)
    {
        vg_lite_kernel_error("kmalloc failed, ");
        vg_lite_exit();
        ONERROR(VG_LITE_OUT_OF_RESOURCES);
    }
    node->offset = 0;
    node->size = device->size;
    node->status = 0;
    list_add(&node->list, &device->heap.list);

    /* Initialize the wait queue. */
    init_waitqueue_head(&device->int_queue);

    /* Install IRQ. */
    if (request_irq(device->irq_line/*GPU_IRQ*/, irq_hander, gcdIRQF_FLAG, "vg_lite_irq", device))
    {
        vg_lite_kernel_error("request_irq failed, ");
        vg_lite_exit();
        ONERROR(VG_LITE_OUT_OF_RESOURCES);
    }
    device->irq_enabled = 1;
    printk("vg_lite: enabled ISR for interrupt %d\n", device->irq_line/*GPU_IRQ*/);

    /* Register device. */
    device->major = register_chrdev(0, "vg_lite", &file_operations);
    if (device->major < 0)
    {
        vg_lite_kernel_error("register_chrdev failed, ");
        vg_lite_exit();
        ONERROR(VG_LITE_OUT_OF_RESOURCES);
    }
    device->registered = 1;

    /* Create the graphics class. */
    device->class = class_create(THIS_MODULE, "vg_lite_class");
    if (device->class == NULL)
    {
        vg_lite_kernel_error("class_create failed, ");
        vg_lite_exit();
        ONERROR(VG_LITE_OUT_OF_RESOURCES);
    }

    /* Create the device. */
    device->dev = device_create(device->class, NULL, MKDEV(device->major, 0), NULL, VG_DEVICE_NAME);
    if (device->dev == NULL)
    {
        vg_lite_kernel_error("device_create failed, ");
        vg_lite_exit();
        ONERROR(VG_LITE_OUT_OF_RESOURCES);
    }
    device->created = 1;

    if (verbose)
    {
        vg_lite_kernel_hintmsg("vg_lite: created /dev/vg_lite device\n");
    }

    /* Success. */
    return VG_LITE_SUCCESS;
on_error:
    return error;
}

static vg_lite_int32_t vg_lite_get_power(void)
{
    vg_lite_int32_t ret = -1;

    if (platform->ops->get_power)
    {
        ret = platform->ops->get_power(platform);
    }

    return ret;
}

static vg_lite_int32_t vg_lite_put_power(void)
{
    vg_lite_int32_t ret = -1;

    if (platform->ops->put_power)
    {
        ret = platform->ops->put_power(platform);
    }

    return ret;
}

static vg_lite_int32_t vg_lite_set_power(vg_lite_bool_t enable)
{
    vg_lite_int32_t ret = -1;

    if (platform->ops->set_power)
    {
        ret = platform->ops->set_power(platform, enable);
    }

    return ret;
}

static vg_lite_int32_t vg_lite_set_clock(vg_lite_bool_t enable)
{
    vg_lite_int32_t ret = -1;

    if (platform->ops->set_clock)
    {
        ret = platform->ops->set_clock(platform, enable);
    }

    return ret;
}

static vg_lite_int32_t gpu_probe(struct platform_device *pdev)
{
    vg_lite_error_t error;

    vg_lite_kernel_hintmsg("GPU PROBE CALL SUCCESS!\n");

    platform->device = pdev;

    /* Create device structure. */
    device = kmalloc(sizeof(struct vg_lite_device), GFP_KERNEL);
    if (device == NULL)
    {
        vg_lite_kernel_error("kmalloc failed, ");
        ONERROR(VG_LITE_OUT_OF_RESOURCES);
    }

    /* Zero out the enture structure. */
    memset(device, 0, sizeof(struct vg_lite_device));

    device->pdev = pdev;
    platform->vg_device = device;

    ONERROR(init_param(&global_param));
    ONERROR(sync_input_param(&global_param));

    if (platform->ops->adjust_param)
    {
        platform->ops->adjust_param(platform, &global_param);
    }

    ONERROR(sync_param(&global_param));

    ONERROR(vg_lite_init(pdev));

    if (!device || !device->created)
    {
        ONERROR(VG_LITE_INVALID_ARGUMENT);
    }

    if (!vg_lite_get_power())
    {
        vg_lite_set_power(VG_TRUE);
        vg_lite_set_clock(VG_TRUE);
    }

    return 0;
on_error:
    return -1;
}

static vg_lite_int32_t gpu_remove(struct platform_device *pdev)
{
    if (!vg_lite_set_clock(VG_FALSE))
    {
        vg_lite_set_power(VG_FALSE);
        vg_lite_put_power();
    }

    vg_lite_exit();

    return 0;
}

#ifdef BACKUP_COMMAND
static vg_lite_int32_t gpu_suspend(struct platform_device *dev, pm_message_t state)
{
    /* wait gpu idle */
    while (VG_LITE_KERNEL_IS_GPU_IDLE() != 1)
    {
        vg_lite_hal_delay(2);
    }

    global_flags = *(vg_lite_uint32_t *)(vg_lite_uint8_t *)(device->register_base_mapped +
                                                            VG_LITE_INTR_STATUS);

    /* shutdown gpu */
    vg_lite_kernel(VG_LITE_CLOSE, NULL);

    /* shutdown power and clock  */
    if (!vg_lite_set_clock(VG_FALSE))
    {
        vg_lite_set_power(VG_FALSE);
        vg_lite_put_power();
    }

    vg_lite_kernel_hintmsg("gpu_suspend success!\n");

    return 0;
}

static vg_lite_int32_t gpu_resume(struct platform_device *dev)
{
    /* open power and clock */
    if (!vg_lite_get_power())
    {
        vg_lite_set_power(VG_TRUE);
        vg_lite_set_clock(VG_TRUE);
    }

    /* open gpu interrupt and recovery gpu register */
    vg_lite_kernel(VG_LITE_RESET, NULL);


    if (global_flags)
    {
        /* Combine with current interrupt flags. */
        device->int_flags |= global_flags;

        /* Wake up any waiters. */
        wake_up_interruptible(&device->int_queue);

        global_flags = 0;
        vg_lite_kernel_hintmsg("wake up waiters!\n");
    }

    vg_lite_kernel_hintmsg("gpu_resume success!\n");

    return 0;
}

# if defined(CONFIG_PM) && LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 30)
#  ifdef CONFIG_PM_SLEEP
static vg_lite_int32_t gpu_system_suspend(struct device *dev)
{
    pm_message_t state = { 0 };

    return gpu_suspend(to_platform_device(dev), state);
}

static vg_lite_int32_t gpu_system_resume(struct device *dev)
{
    return gpu_resume(to_platform_device(dev));
}

static const struct dev_pm_ops gpu_pm_ops =
{
    SET_SYSTEM_SLEEP_PM_OPS(gpu_system_suspend, gpu_system_resume)
};
#  endif
# endif
#endif



static struct platform_driver gpu_driver =
{
    .probe      = gpu_probe,
    .remove     = gpu_remove,
#ifdef BACKUP_COMMAND
    .suspend    = gpu_suspend,
    .resume     = gpu_resume,
#endif
    .driver     = {
        .owner  = THIS_MODULE,
        .name   = VG_DEVICE_NAME,
#if defined(CONFIG_PM) && LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 30) && defined(BACKUP_COMMAND)
        .pm = &gpu_pm_ops,
#endif

    }
};

static vg_lite_int32_t __init gpu_init(void)
{
    vg_lite_int32_t err;

    err = vg_kernel_platform_init(&gpu_driver, &platform);
    if (err || !platform)
    {
        vg_lite_kernel_error("Soc platform init failed.\n");
        return -ENODEV;
    }
    vg_lite_kernel_hintmsg("vg_kernel_platform_init success!\n");

    err = platform_driver_register(&gpu_driver);
    if (err)
    {
        return -ENODEV;
    }

    return 0;
}

static void __exit gpu_exit(void)
{
    platform_driver_unregister(&gpu_driver);
    vg_kernel_platform_terminate(platform);
    platform = NULL;
}

module_init(gpu_init)
module_exit(gpu_exit)
