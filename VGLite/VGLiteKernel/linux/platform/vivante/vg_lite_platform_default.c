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
#include "vg_lite_kernel.h"
#include "vg_lite_hal.h"

int _adjust_param(vg_platform_t *platform, vg_module_parameters_t *args);

static vg_linux_operations_t default_ops =
{
    .adjust_param        = _adjust_param,
};

#ifdef ENABLE_PCIE
struct vg_pcie_info
{
    uint irq_line;
    int pci_registered;
    struct pci_dev *p_dev;
};

struct vg_platform_pcie
{
    vg_platform_t base;
    struct vg_pcie_info pcie_info;
};

static struct vg_platform_pcie default_platform =
{
    .base = {
        .name = __FILE__,
        .ops  = &default_ops,
    },
};
#else
static vg_platform_t default_platform =
{
    .name = __FILE__,
    .ops  = &default_ops,
};
#endif

int _adjust_param(vg_platform_t *platform, vg_module_parameters_t *args)
{
#ifdef ENABLE_PCIE
    struct vg_platform_pcie *pcie_platform = (struct vg_platform_pcie *)platform;

    if (0 == args->irq_line)
    {
        args->irq_line = pcie_platform->pcie_info.irq_line;
    }

    pcie_platform->base.vg_device->p_dev = pcie_platform->pcie_info.p_dev;
    pcie_platform->base.vg_device->pci_registered = pcie_platform->pcie_info.pci_registered;
#endif
    return 0;
}

#ifdef ENABLE_PCIE
static int drv_probe(struct pci_dev *p_dev, const struct pci_device_id *ent)
{
    int ret = 0;
    uint irq_line = 0;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 24)
    static u64 dma_mask = DMA_BIT_MASK(40);
#else
    static u64 dma_mask = DMA_40BIT_MASK;
#endif

    vg_lite_kernel_hintmsg("PCIE DRIVER PROBE!\n");

    ret = pci_enable_device(p_dev);
    if (ret)
    {
        vg_lite_kernel_error("pci_enable_device call failed.\n");
        return ret;
    }

    ret = pci_set_dma_mask(p_dev, dma_mask);
    if (ret)
    {
        vg_lite_kernel_error("Failed to set DMA mask.\n");
        goto err0;
    }

    pci_set_master(p_dev);

    ret = pci_request_regions(p_dev, "vg_lite");
    if (ret)
    {
        vg_lite_kernel_error("Failed to get ownership of BAR region.\n");
        goto err1;
    }

    ret = pci_alloc_irq_vectors(p_dev, 1, 32, PCI_IRQ_LEGACY);
    if (ret < 1)
    {
        vg_lite_kernel_error("Failed to allocate irq vectors.\n");
        goto err2;
    }

    irq_line = pci_irq_vector(p_dev, 0);
    if (irq_line < 1)
    {
        vg_lite_kernel_error("Failed to pci_irq_vector.\n");
        goto err3;
    }

    default_platform.pcie_info.p_dev    = p_dev;
    default_platform.pcie_info.irq_line = irq_line;

    return 0;

err3:
    pci_free_irq_vectors(p_dev);
err2:
    pci_release_regions(p_dev);
err1:
    pci_clear_master(p_dev);
err0:
    pci_disable_device(p_dev);

    return ret;
}

static void drv_remove(struct pci_dev *p_dev)
{
    pci_set_drvdata(p_dev, NULL);
    pci_free_irq_vectors(p_dev);
    pci_clear_master(p_dev);
    pci_release_regions(p_dev);
    pci_disable_device(p_dev);

    return;
}

static const struct pci_device_id vg_lite_ids[] =
{
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

static struct pci_driver vg_lite_sub_driver =
{
    .name       = "vg_lite",
    .id_table   = vg_lite_ids,
    .probe      = drv_probe,
    .remove     = drv_remove,
};
#endif


struct platform_device *default_dev = NULL;

int vg_kernel_platform_init(struct platform_driver *pdrv, vg_platform_t **platform)
{
    int ret;

    default_dev = platform_device_alloc(pdrv->driver.name, -1);
    if (!default_dev)
    {
        vg_lite_kernel_error("platform_device_alloc failed.\n");
        return -ENOMEM;
    }

    /* Add device */
    ret = platform_device_add(default_dev);
    if (ret)
    {
        vg_lite_kernel_error("platform_device_add failed.\n");
        goto on_error;
    }

#ifdef ENABLE_PCIE
    ret = pci_register_driver(&vg_lite_sub_driver);
    default_platform.pcie_info.pci_registered = 1;
#endif

    *platform = (vg_platform_t *)&default_platform;

    return 0;

on_error:

    platform_device_put(default_dev);

    return ret;
}

int vg_kernel_platform_terminate(vg_platform_t *platform)
{
    if (default_dev)
    {
        platform_device_unregister(default_dev);
        default_dev = NULL;
    }

#ifdef ENABLE_PCIE
    if (default_platform.pcie_info.pci_registered == 1)
    {
        /* Unregister the pcie driver. */
        pci_unregister_driver(&vg_lite_sub_driver);
    }
#endif

    return 0;
}
