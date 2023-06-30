/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-03-02     wanghao      first version
 */

#include "drv_gpu.h"
#include "drv_lcd.h"
#include "os_mem.h"
#include "trace.h"
#include "vg_lite_platform.h"
#include "vg_lite.h"


static uint8_t *contiguous_mem = NULL;



void drv_gpu_init(void)
{
    vg_lite_init_mem(0x40140000, 0x0, contiguous_mem, 0x50000);
    DBG_DIRECT("contiguous_mem addr = 0x%x\r\n", contiguous_mem);

    vg_lite_set_command_buffer_size((32 << 10));//cmd buffer = value * 2
    vg_lite_init(drv_lcd_get_fb_width(), drv_lcd_get_fb_height());
}

void drv_gpu_deinit(void)
{
    vg_lite_close();
}

void hw_gpu_init(void)
{
    contiguous_mem = os_mem_alloc(RAM_TYPE_EXT_DATA_SRAM, 0x50000);

    if (contiguous_mem == NULL)
    {
        DBG_DIRECT("!!!GPU BUFFER INIT FAIL");
    }

    drv_gpu_init();

    uint32_t chip_id = 0, chip_rev = 0, cid = 0;
    vg_lite_get_product_info(NULL, &chip_id, &chip_rev);
    vg_lite_get_register(0x30, &cid);
    DBG_DIRECT("chip id information, chid_id = 0x%x; chip_rev = 0x%x; cid = 0x%x\n", chip_id, chip_rev,
               cid);
}


/************** end of file ********************/
