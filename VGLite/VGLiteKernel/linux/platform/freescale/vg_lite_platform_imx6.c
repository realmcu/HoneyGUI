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

struct imx_priv
{

    struct clk           *clk_2d_core;
    struct clk           *clk_vg_axi;
    struct regulator     *gpu_regulator;
};
struct imx_priv _imx_priv;

static const struct of_device_id mxs_gpu_dt_ids[] =
{
    { .compatible = "fsl,imx6q-gpu", },
    {}
};

int _adjust_driver(vg_platform_t *platform)
{
    struct platform_driver *pdrv = platform->driver;

    pdrv->driver.of_match_table = mxs_gpu_dt_ids;

    return 0;
}


int _adjust_param(vg_platform_t *platform, vg_module_parameters_t *args)
{
    struct resource *res;
    struct platform_device *pdev = platform->device;

    res = platform_get_resource_byname(pdev, IORESOURCE_IRQ, "irq_vg");
    if (res)
    {
        args->irq_line =  res->start;
    }
    res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "contiguous_mem");
    if (res)
    {
        args->contiguous_base = res->start;
        args->contiguous_size = res->end - res->start + 1;
    }

    res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "iobase_vg");
    if (res)
    {
        args->register_mem_base = res->start;
        args->register_mem_size = res->end - res->start + 1;
    }

    return 0;
}

int _get_power(vg_platform_t *platform)
{
    struct device *dev = &platform->device->dev;

    /* get clock. */
    _imx_priv.clk_2d_core = clk_get(dev, "gpu2d_clk");
    if (IS_ERR(_imx_priv.clk_2d_core))
    {
        _imx_priv.clk_2d_core = NULL;
        clk_put(_imx_priv.clk_2d_core);
        vg_lite_kernel_error("clk_get gpu2d_clk error!\n");
        return -1;
    }

    _imx_priv.clk_vg_axi = clk_get(dev, "openvg_axi_clk");
    if (IS_ERR(_imx_priv.clk_vg_axi))
    {
        _imx_priv.clk_vg_axi  = NULL;
        clk_put(_imx_priv.clk_vg_axi);
        vg_lite_kernel_error("clk_get openvg_axi_clk error!\n");
        return -1;
    }

#ifdef CONFIG_PM
    /* Init runtime pm for gpu. */
    pm_runtime_enable(dev);
#endif

    return 0;
}

int _set_power(vg_platform_t *platform, vg_lite_bool_t enable)
{
    struct device *dev = &platform->device->dev;

    if (enable)
    {
#if 0
        /* enable power. */
        if (_imx_priv.gpu_regulator)
        {
            int err = regulator_enable(_imx_priv.gpu_regulator);

            if (err)
            {
                BUG_ON(1);
            }
        }
#endif

#ifdef CONFIG_PM
        pm_runtime_get_sync(dev);
#endif
    }
    else
    {
#if 0
        if (_imx_priv.gpu_regulator)
        {
            regulator_disable(_imx_priv.gpu_regulator);
        }
#endif

#ifdef CONFIG_PM
        pm_runtime_put_sync(dev);
#endif
    }

    return 0;
}

int _put_power(vg_platform_t *platform)
{

    struct device *dev = &platform->device->dev;

    /* put clock. */
    clk_put(_imx_priv.clk_2d_core);
    clk_put(_imx_priv.clk_vg_axi);

#ifdef CONFIG_PM
    pm_runtime_disable(dev);
#endif

    return 0;
}


int _set_clock(vg_platform_t *platform, vg_lite_bool_t enable)
{
    if (enable)
    {
        /* enable clock. */
        if (_imx_priv.clk_2d_core)
        {
            clk_prepare(_imx_priv.clk_2d_core);
        }
        if (_imx_priv.clk_2d_core)
        {
            clk_enable(_imx_priv.clk_2d_core);
        }
        if (_imx_priv.clk_vg_axi)
        {
            clk_prepare(_imx_priv.clk_vg_axi);
        }
        if (_imx_priv.clk_vg_axi)
        {
            clk_enable(_imx_priv.clk_vg_axi);
        }
    }
    else
    {
        if (_imx_priv.clk_2d_core)
        {
            clk_disable(_imx_priv.clk_2d_core);
        }
        if (_imx_priv.clk_vg_axi)
        {
            clk_disable(_imx_priv.clk_vg_axi);
        }
    }

    return 0;
}

static vg_linux_operations_t operations =
{
    .adjust_param        = _adjust_param,
    .get_power           = _get_power,
    .set_power           = _set_power,
    .put_power           = _put_power,
    .set_clock           = _set_clock,
    .adjust_driver       = _adjust_driver,
};

static vg_platform_t imx6_platform =
{
    .name = __FILE__,
    .ops  = &operations,
};

int vg_kernel_platform_init(struct platform_driver *pdrv, vg_platform_t **platform)
{
    *platform = &imx6_platform;

    (*platform)->driver = pdrv;

    if ((*platform)->ops->adjust_driver)
    {
        (*platform)->ops->adjust_driver(*platform);
    }

    return 0;
}

int vg_kernel_platform_terminate(vg_platform_t *platform)
{
    return 0;
}
