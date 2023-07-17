/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author        Notes
 * 2020-08-04     tyustli  first version
 */

#include "board.h"
#include "drv_flash.h"
#include "trace.h"
#include "string.h"
#ifdef RTK_MODULE_USING_DATABASE
#include "fal.h"
#endif
/**
 * Read data from flash.
 * @note This operation's units is word.
 *
 * @param addr flash address
 * @param buf buffer to store read data
 * @param size read bytes size
 *
 * @return result
 */

int realtek_flash_read(uint32_t addr, uint8_t *buf, uint32_t size)
{
    if ((addr + size) > REALTEK_FLASH_END_ADDRESS)
    {
        DBG_DIRECT("read outrange flash size! addr is (0x%p)", (void *)(addr + size));
        return -1;
    }

#if defined RTL8772F || defined RTL8762G
    flash_nor_read_locked(addr, buf, size);
#elif defined RTL8762D
    flash_read_locked(addr, size, buf);
#endif
    return size;
}

/**
 * Write data to flash.
 * @note This operation's units is word.
 * @note This operation must after erase. @see flash_erase.
 *
 * @param addr flash address
 * @param buf the write data buffer
 * @param size write bytes size
 *
 * @return result
 */

int realtek_flash_write(uint32_t addr, const uint8_t *buf, uint32_t size)
{

#if defined RTL8772F || defined RTL8762G
    FLASH_NOR_RET_TYPE ret = flash_nor_write_locked(addr, (uint8_t *)buf, size);
    if (ret != FLASH_NOR_RET_SUCCESS)
    {
        DBG_DIRECT("realtek data write error %d", ret);
        return 0;
    }
#elif defined RTL8762D
    if (flash_write_locked(addr, size, (uint8_t *)buf) == false)

    {
        DBG_DIRECT("realtek data write error");
        return 0;
    }
#endif
    return size;
}

/**
 * Erase data on flash.
 * @note This operation is irreversible.
 * @note This operation's units is different which on many chips.
 *
 * @param addr flash address
 * @param size erase bytes size
 *
 * @return result
 */

int realtek_flash_erase(uint32_t addr, size_t size)
{
    uint32_t div_size;
    uint32_t yu_size;

    div_size = size / FLASH_SECTOR_SIZE;
    yu_size = size % FLASH_SECTOR_SIZE;

    uint32_t count_sector; /* Number of sector to erase */
    count_sector = div_size;

    if (count_sector == 0)
    {
        if (yu_size == 0)
        {
            count_sector = 0;
        }
        else
        {
            count_sector = 1;
        }
    }
    else
    {
        if (yu_size != 0)
        {
            count_sector++;
        }
    }

    uint32_t start_sector; /* Start sector in LBA */
    start_sector = addr - REALTK_FLASH_START_ADDRESS;
    div_size = start_sector / FLASH_SECTOR_SIZE;
    yu_size = start_sector % FLASH_SECTOR_SIZE;

    if (div_size == 0)
    {
        start_sector = 0;
    }
    else
    {
        if (yu_size == 0)
        {
            start_sector = div_size;
        }
        else
        {
            start_sector = div_size + 1;
        }
    }

    for (uint32_t i = 0; i < count_sector; i++)
    {

#if defined RTL8772F || defined RTL8762G
        FLASH_NOR_RET_TYPE ret = flash_nor_erase_locked(REALTK_FLASH_START_ADDRESS + start_sector *
                                                        FLASH_SECTOR_SIZE +
                                                        FLASH_SECTOR_SIZE * i, FLASH_NOR_ERASE_SECTOR);
        if (ret != FLASH_NOR_RET_SUCCESS)
        {
            DBG_DIRECT("fs data erase error!");
            return 0;
        }
#elif defined RTL8762D
        if (flash_erase_locked(FLASH_ERASE_SECTOR,
                               REALTK_FLASH_START_ADDRESS + start_sector * FLASH_SECTOR_SIZE +
                               FLASH_SECTOR_SIZE * i) == false)

        {
            DBG_DIRECT("fs data erase error!");
            return 0;
        }
#endif
    }

    return size;
}


#ifdef RTK_MODULE_USING_DATABASE
static int fal_flash_read(long offset, uint8_t *buf, size_t size);
static int fal_flash_write(long offset, const uint8_t *buf, size_t size);
static int fal_flash_erase(long offset, size_t size);

const struct fal_flash_dev realtek_nor_flash = { "nor_flash", REALTK_FLASH_START_ADDRESS, REALTEK_FLASH_SIZE,
    FLASH_SECTOR_SIZE, {NULL, fal_flash_read, fal_flash_write, fal_flash_erase}
};

static int fal_flash_read(long offset, uint8_t *buf, size_t size)
{
    return realtek_flash_read(realtek_nor_flash.addr + offset, buf, size);
}

static int fal_flash_write(long offset, const uint8_t *buf, size_t size)
{
    return realtek_flash_write(realtek_nor_flash.addr + offset, buf, size);
}

static int fal_flash_erase(long offset, size_t size)
{
    return realtek_flash_erase(realtek_nor_flash.addr + offset, size);
}
#endif

/************************** end of file ******************************/

