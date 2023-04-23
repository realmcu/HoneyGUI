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
#include "../vg_lite_kernel.h"
#include "../vg_lite_hal.h"
#include "vg_lite_ioctl.h"
#include "../vg_lite_hw.h"
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/list.h>
#include <linux/wait.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/pagemap.h>
#include <linux/version.h>
#include <linux/vmalloc.h>
#include <linux/mman.h>
#include <linux/mm_types.h>
#include <asm/io.h>
#ifdef ENABLE_PCIE
#include <linux/pci.h>
#endif

MODULE_LICENSE("Dual MIT/GPL");

static int  vg_lite_init(void);
static void vg_lite_exit(void);

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

/* Struct definitions. */
struct heap_node {
    struct list_head list;
    uint32_t offset;
    unsigned long size;
    int32_t status;
};

struct memory_heap {
    uint32_t free;
    struct list_head list;
};

struct mapped_memory {
    void * logical;
    uint32_t physical;
    int page_count;
    struct page ** pages;
};

struct vg_lite_device {
    void * gpu;             /* Register memory base */
    struct page * pages;
    unsigned int order;
    void * virtual;
    uint64_t physical;
    uint32_t size;
    struct memory_heap heap;
    int irq_enabled;
    volatile uint32_t int_flags;
    wait_queue_head_t int_queue;
    void * device;
    int registered;
    int major;
    struct class * class;
    int created;
#ifdef ENABLE_PCIE
    struct pci_dev *pdev;
    int pci_registered;
#endif
};

struct client_data {
    struct vg_lite_device * device;
    struct vm_area_struct * vm;
    void * contiguous_mapped;
};

/* Data and objects declarations. */
static int heap_size = 8 << 20;     /* Default heap size is 16MB. */
static int verbose = 0;
static int cached = 0;

static struct vg_lite_device * device = NULL;
static struct client_data * private_data = NULL;

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
    if (cached) {
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

static int split_node(struct heap_node * node, unsigned long size)
{
    struct heap_node * split;

    /* Allocate a new node. */
    split = kmalloc(sizeof(struct heap_node), GFP_KERNEL);
    if (split == NULL)
        return -1;

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

vg_lite_error_t vg_lite_hal_allocate_contiguous(unsigned long size, void ** logical, uint32_t * physical,void ** node)
{
    unsigned long aligned_size;
    struct heap_node * pos;

    /* Align the size to 64 bytes. */
    aligned_size = VG_LITE_ALIGN(size, VGLITE_MEM_ALIGNMENT);

    /* Check if there is enough free memory available. */
    if (aligned_size > device->heap.free) {
        return VG_LITE_OUT_OF_MEMORY;
    }

    /* Walk the heap backwards. */
    list_for_each_entry_reverse(pos, &device->heap.list, list) {
        /* Check if the current node is free and is big enough. */
        if (pos->status == 0 && pos->size >= aligned_size) {
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
            *logical = (uint8_t *) private_data->contiguous_mapped + pos->offset;
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

void vg_lite_hal_free_contiguous(void * memory_handle)
{
    struct heap_node * pos, * node;

    /* Get pointer to node. */
    node = memory_handle;

    if (node->status != HEAP_NODE_USED) {
        if (verbose)
            printk("vg_lite: ignoring corrupted memory handle %p\n", memory_handle);
        return;
    }

    /* Mark node as free. */
    node->status = 0;

    /* Add node size to free count. */
    device->heap.free += node->size;

    /* Check if next node is free. */
    pos = node;
    list_for_each_entry_continue(pos, &device->heap.list, list) {
        if (pos->status == 0) {
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
    list_for_each_entry_continue_reverse(pos, &device->heap.list, list) {
        if (pos->status == 0) {
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
    return *(uint32_t *) (uint8_t *) (device->gpu + address);
}

void vg_lite_hal_poke(uint32_t address, uint32_t data)
{
    /* Write data to the GPU register. */
    *(uint32_t *) (uint8_t *) (device->gpu + address) = data;
}

vg_lite_error_t vg_lite_hal_query_mem(vg_lite_kernel_mem_t *mem)
{
    if(device != NULL){
        mem->bytes = device->heap.free;
        return VG_LITE_SUCCESS;
    }
    mem->bytes = 0;
    return VG_LITE_NO_CONTEXT;
}

vg_lite_error_t vg_lite_hal_map_memory(vg_lite_kernel_map_memory_t *node)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    void* _logical = NULL;
    uint64_t physical = node->physical;
    uint32_t offset = physical & (PAGE_SIZE - 1);
    uint64_t bytes = node->bytes + offset;
    uint32_t num_pages, pfn = 0;
    vg_lite_kernel_unmap_memory_t unmap_node;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 4, 0)
    _logical = (void*)vm_mmap(NULL, 0L, bytes, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_NORESERVE, 0);
#else
    down_write(&current_mm_mmap_sem);
    _logical = (void*)do_mmap_pgoff(NULL, 0L, bytes,
                PROT_READ | PROT_WRITE, MAP_SHARED, 0);
    up_write(&current_mm_mmap_sem);
#endif

    if (!_logical) {
        node->logical = NULL;
        return VG_LITE_OUT_OF_MEMORY;
    }

    down_write(&current_mm_mmap_sem);

    struct vm_area_struct *vma = find_vma(current->mm, (unsigned long)_logical);
    if (vma == NULL) {
        return VG_LITE_OUT_OF_RESOURCES;
    }

    pfn = (physical >> PAGE_SHIFT);
    num_pages = GET_PAGE_COUNT(bytes, PAGE_SIZE);

    /* Make this mapping non-cached. */
    vma->vm_flags |= VM_FLAGS;

    vma->vm_page_prot = pgprot_writecombine(vma->vm_page_prot);

    if (remap_pfn_range(vma, vma->vm_start, pfn, num_pages << PAGE_SHIFT, vma->vm_page_prot) < 0) {
        error = VG_LITE_OUT_OF_MEMORY;
    }

    node->logical = (void *)((uint8_t*)_logical + offset);

    up_write(&current_mm_mmap_sem);

    if(error)
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
    void * _logical;
    uint32_t bytes;
    uint32_t offset = (uint64_t)node->logical & (PAGE_SIZE - 1);

    if (unlikely(!current->mm))
        return error;

    _logical = (void *)((uint8_t*)node->logical - offset);
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

int32_t vg_lite_hal_wait_interrupt(uint32_t timeout, uint32_t mask, uint32_t * value)
{
    struct timeval tv;
    unsigned long jiffies;
    unsigned long result;

    if (timeout == VG_LITE_INFINITE) {
        /* Set 1 second timeout. */
        tv.tv_sec = 1;
        tv.tv_usec = 0;
    } else {
        /* Convert timeout in ms to timeval. */
        tv.tv_sec = timeout / 1000;
        tv.tv_usec = (timeout % 1000) * 1000;
    }

    /* Convert timeval to jiffies. */
    jiffies = timeval_to_jiffies(&tv);

    /* Wait for interrupt, ignoring timeout. */
    do {
        result = wait_event_interruptible_timeout(device->int_queue, device->int_flags & mask, jiffies);
    } while (timeout == VG_LITE_INFINITE && result == 0);

    /* Report the event(s) got. */
    if (value != NULL) {
        *value = device->int_flags & mask;
    }

    device->int_flags = 0;
    return (result != 0);
}

void * vg_lite_hal_map(unsigned long bytes, void * logical, uint32_t physical, uint32_t * gpu)
{
    struct mapped_memory * mapped;
    int page_count;
    unsigned long start, end;

    if (logical != NULL) {
        start = (unsigned long) logical >> PAGE_SHIFT;
        end = ((unsigned long) logical + bytes + PAGE_SIZE - 1) >> PAGE_SHIFT;
        page_count = end - start;
    } else {
        page_count = 0;
    }

    mapped = kmalloc(sizeof(struct mapped_memory) + page_count * sizeof(struct page *), GFP_KERNEL);
    if (mapped == NULL) {
        return NULL;
    }

    mapped->logical = logical;
    mapped->physical = physical;
    mapped->page_count = page_count;
    mapped->pages = page_count ? (struct page **) (mapped + 1) : NULL;

    if (logical == NULL) {
        *gpu = physical;
    } else {
        down_read(&current->mm->mmap_sem);
        get_user_pages(
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 6, 0)
            current,
            current->mm,
#endif
            (unsigned long) logical & PAGE_MASK,
            page_count,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 9, 0)
            FOLL_WRITE,
#else
            1,
            0,
#endif
            mapped->pages,
            NULL);

        up_read(&current->mm->mmap_sem);
    }

    return mapped;
}

void vg_lite_hal_unmap(void * handle)
{
    struct mapped_memory * mapped = handle;
    int i;

    if (mapped->page_count) {
        for (i = 0; i < mapped->page_count; i++) {
            if (mapped->pages[i] != NULL) {
                put_page(mapped->pages[i]);
            }
        }
    }

    kfree(mapped);
}

int drv_open(struct inode * inode, struct file * file)
{
    struct client_data * data;

    data = kmalloc(sizeof(struct client_data), GFP_KERNEL);
    if (data == NULL)
        return -1;

    data->device = device;
    data->contiguous_mapped = NULL;

    file->private_data = data;

    return 0;
}

int drv_release(struct inode * inode, struct file * file)
{
    struct client_data * data = (struct client_data *) file->private_data;

    if (data != NULL) {
        if (data->contiguous_mapped != NULL) {
        }

        kfree(data);
        file->private_data = NULL;
    }

    return 0;
}

#ifdef HAVE_UNLOCKED_IOCTL
long drv_ioctl(struct file * file, unsigned int ioctl_code, unsigned long arg)
#else
long drv_ioctl(struct inode *inode, struct file * file, unsigned int ioctl_code, unsigned long arg)
#endif
{
    struct ioctl_data arguments;
    void * data;

#ifndef HAVE_UNLOCKED_IOCTL
    /* inode will be not used */
    (void)inode;
#endif
    private_data = (struct client_data *) file->private_data;
    if (private_data == NULL)
    {
        return -1;
    }

    if (ioctl_code != VG_LITE_IOCTL) {
        return -1;
    }

    if ((void *) arg == NULL)
    {
        return -1;
    }

    if (copy_from_user(&arguments, (void *) arg, sizeof(arguments)) != 0) {
        return -1;
    }

    data = kmalloc(arguments.bytes, GFP_KERNEL);
    if (data == NULL)
        return -1;

    if (copy_from_user(data, arguments.buffer, arguments.bytes) != 0)
        goto error;

    arguments.error = vg_lite_kernel(arguments.command, data);

    if (copy_to_user(arguments.buffer, data, arguments.bytes) != 0)
        goto error;

    kfree(data);

    if (copy_to_user((void *) arg, &arguments, sizeof(arguments)) != 0)
        return -1;

    return 0;

error:
    kfree(data);
    return -1;
}

ssize_t drv_read(struct file * file, char * buffer, size_t length, loff_t * offset)
{
    struct client_data * private = (struct client_data *) file->private_data;

    if (length != 4) {
        return 0;
    }

    if (copy_to_user((void __user *) buffer, (const void *) &private->device->size, sizeof(private->device->size)) != 0)
    {
        return 0;
    }

    memcpy(buffer, &private->device->size, 4);
    return 4;
}

int drv_mmap(struct file * file, struct vm_area_struct * vm)
{
    unsigned long size;
    struct client_data * private = (struct client_data *) file->private_data;

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
    if (size > private->device->size)
        size = private->device->size;

    if (remap_pfn_range(vm, vm->vm_start, private->device->physical >> PAGE_SHIFT, size, vm->vm_page_prot) < 0) {
        printk("vg_lite: remap_pfn_range failed\n");
        return -1;
    }

    private->vm = vm;
    private->contiguous_mapped = (void *) vm->vm_start;

    if (verbose)
        printk("vg_lite: mapped %scached contiguous memory to %p\n", cached ? "" : "non-", private->contiguous_mapped);

    return 0;
}

#ifdef ENABLE_PCIE
static int drv_probe(struct pci_dev *pdev, const struct pci_device_id *ent)
{
    int ret = 0;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 24)
    static u64 dma_mask = DMA_BIT_MASK(40);
#else
    static u64 dma_mask = DMA_40BIT_MASK;
#endif

    printk("PCIE DRIVER PROBED");

    ret = pci_enable_device(pdev);
    if (ret) {
        printk(KERN_ERR "vg_lite: pci_enable_device() failed.\n");
        return ret;
    }

    ret = pci_set_dma_mask(pdev, dma_mask);
    if (ret) {
        printk(KERN_ERR "vg_lite: Failed to set DMA mask.\n");
        goto err0;
    }

    pci_set_master(pdev);

    ret = pci_request_regions(pdev, "vg_lite");
    if (ret) {
        printk(KERN_ERR "vg_lite: Failed to get ownership of BAR region.\n");
        goto err1;
    }

    ret = pci_alloc_irq_vectors(pdev, 1, 32, PCI_IRQ_LEGACY);
    if (ret < 1) {
        printk("vg_lite: Failed to allocate irq vectors.\n");
        goto err2;
    }

    if (irqLine == 0) {
        irqLine = pci_irq_vector(pdev, 0);
        if (irqLine < 1)
        {
            printk("vg_lite: Failed to pci_irq_vector.\n");
            goto err3;
        }
    }
    device->pdev = pdev;

    return 0;
err3:
    pci_free_irq_vectors(pdev);
err2:
    pci_release_regions(pdev);
err1:
    pci_clear_master(pdev);
err0:
    pci_disable_device(pdev);

    return ret;
}

static void drv_remove(struct pci_dev *pdev)
{
    pci_set_drvdata(pdev, NULL);
    pci_free_irq_vectors(pdev);
    pci_clear_master(pdev);
    pci_release_regions(pdev);
    pci_disable_device(pdev);

    return;
}

static const struct pci_device_id vg_lite_ids[] = {
    {
        .class = 0,
        .class_mask = 0,
        .vendor = 0x10ee,
        .device = 0x7012,
        .subvendor = PCI_ANY_ID,
        .subdevice = PCI_ANY_ID,
        .driver_data = 0
    },
    {0 /* End: all zeroes */ }
};

MODULE_DEVICE_TABLE(pci, vg_lite_ids);

static struct pci_driver vg_lite_sub_driver = {
    .name       = "vg_lite",
    .id_table   = vg_lite_ids,
    .probe      = drv_probe,
    .remove     = drv_remove,
};
#endif

static struct file_operations file_operations =
{
    .owner          = THIS_MODULE,
    .open           = drv_open,
    .release        = drv_release,
    .read           = drv_read,
#ifdef HAVE_UNLOCKED_IOCTL
    .unlocked_ioctl = drv_ioctl,
#endif
#ifdef HAVE_COMPAT_IOCTL
    .compat_ioctl   = drv_ioctl,
#endif
    .mmap           = drv_mmap,
};

static void vg_lite_exit(void)
{
    struct heap_node * pos;
    struct heap_node * n;

    /* Check for valid device. */
    if (device != NULL) {
        if (device->gpu != NULL) {
            /* Unmap the GPU registers. */
            iounmap(device->gpu);
            device->gpu = NULL;
        }

        if (device->pages != NULL) {
            /* Free the contiguous memory. */
            __free_pages(device->pages, device->order);
        }

        if (device->irq_enabled) {
            /* Free the IRQ. */
            free_irq(irqLine/*GPU_IRQ*/, device);
        }

        /* Process each node. */
        list_for_each_entry_safe(pos, n, &device->heap.list, list) {
            /* Remove it from the linked list. */
            list_del(&pos->list);

            /* Free up the memory. */
            kfree(pos);
        }

        if (device->created) {
            /* Destroy the device. */
            device_destroy(device->class, MKDEV(device->major, 0));
        }

        if (device->class != NULL) {
            /* Destroy the class. */
            class_destroy(device->class);
        }

        if (device->registered) {
            /* Unregister the device. */
            unregister_chrdev(device->major, "vg_lite");
        }

#ifdef ENABLE_PCIE
        if (device->pci_registered) {
            /* Unregister the pcie driver. */
            pci_unregister_driver(&vg_lite_sub_driver);
        }
#endif

        /* Free up the device structure. */
        kfree(device);
    }
}

static irqreturn_t irq_hander(int irq, void * context)
{
    struct vg_lite_device * device = context;

    /* Read interrupt status. */
    uint32_t flags = *(uint32_t *) (uint8_t *) (device->gpu + VG_LITE_INTR_STATUS);
    if (flags) {
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

static int vg_lite_init(void)
{
    struct heap_node * node;

    /* Create device structure. */
    device = kmalloc(sizeof(struct vg_lite_device), GFP_KERNEL);
    if (device == NULL) {
        printk("vg_lite: kmalloc failed\n");
        return -1;
    }

    /* Zero out the enture structure. */
    memset(device, 0, sizeof(struct vg_lite_device));

#ifdef ENABLE_PCIE
    if (pci_register_driver(&vg_lite_sub_driver))
    {
        printk("vg_lite: pci_register_driver failed\n");
        vg_lite_exit();
        return -1;
    }
    device->pci_registered = 1;
#endif

    /* Map the GPU registers. */
    device->gpu = ioremap_nocache(registerMemBase/*GPU_REG_START*/, registerMemSize/*GPU_REG_SIZE*/);
    if (device->gpu == NULL) {
        vg_lite_exit();
        printk("vg_lite: ioremap failed\n");
        return -1;
    }

    /* Allocate the contiguous memory. */
#if 0
    /* Allocate the contiguous memory. */
    for (device->order = get_order(heap_size); device->order > 0; device->order--) {
        /* Allocate with the current amount. */
        device->pages = alloc_pages(GFP_KERNEL, device->order);
        if (device->pages != NULL)
            break;
    }

    /* Check if we allocated any contiguous memory or not. */
    if (device->pages == NULL) {
        printk("vg_lite: alloc_pages failed\n");
        vg_lite_exit();
        return -1;
    }

    /* Save contiguous memory. */
    device->virtual = page_address(device->pages);
    device->physical = virt_to_phys(device->virtual);
    device->size = 1 << (device->order + PAGE_SHIFT);
#else
    device->virtual = 0;
    device->physical = contiguousBase;
    device->size = contiguousSize;
#endif
    if (verbose) {
        uint32_t size = (device->size >> 20) ? (device->size >> 20) : (device->size >> 10);
        char c = (device->size >> 20) ? 'M' : 'k';
        printk("vg_lite: allocated a %u%cB heap at 0x%08llx\n", size, c, device->physical);
    }
    
    /* Create the heap. */
    INIT_LIST_HEAD(&device->heap.list);
    device->heap.free = device->size;

    node = kmalloc(sizeof(struct heap_node), GFP_KERNEL);
    if (node == NULL) {
        printk("vg_lite: kmalloc failed\n");
        vg_lite_exit();
        return -1;
    }
    node->offset = 0;
    node->size = device->size;
    node->status = 0;
    list_add(&node->list, &device->heap.list);

    /* Initialize the wait queue. */
    init_waitqueue_head(&device->int_queue);

    /* Install IRQ. */
    if (request_irq(irqLine/*GPU_IRQ*/, irq_hander, 0, "vg_lite_irq", device)) {
        printk("vg_lite: request_irq failed\n");
        vg_lite_exit();
        return -1;
    }
    device->irq_enabled = 1;
    if (verbose)
        printk("vg_lite: enabled ISR for interrupt %d\n", irqLine/*GPU_IRQ*/);

    /* Register device. */
    device->major = register_chrdev(0, "vg_lite", &file_operations);
    if (device->major < 0) {
        printk("vg_lite: register_chrdev failed\n");
        vg_lite_exit();
        return -1;
    }
    device->registered = 1;

    /* Create the graphics class. */
    device->class = class_create(THIS_MODULE, "vg_lite_class");
    if (device->class == NULL) {
        printk("vg_lite: class_create failed\n");
        vg_lite_exit();
        return -1;
    }

    /* Create the device. */
    if (device_create(device->class, NULL, MKDEV(device->major, 0), NULL, "vg_lite") == NULL) {
        printk("vg_lite: device_create failed\n");
        vg_lite_exit();
        return -1;
    }
    device->created = 1;

    if (verbose)
        printk("vg_lite: created /dev/vg_lite device\n");

    /* Success. */
    return 0;
}


module_param(heap_size, int, S_IRUGO);
module_param(verbose, int, S_IRUGO);
module_param(cached, int, S_IRUGO);
module_init(vg_lite_init)
module_exit(vg_lite_exit)
