enum { __FILE_NUM__ = 0 };

/*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     app_flash.c
* @brief
* @details
* @author
* @date     2015-04-29
* @version  v0.1
*********************************************************************************************************
*/

#include "board.h"
#include "trace.h"
#include <string.h>
#include "ftl.h"
#include "app_flash_config.h"
#include "sync_data.h"
#include "communicate_parse.h"
#include "os_sync.h"
#include "communicate_sync_sleep.h"

void WristBandSleepDataBlockInit(void)
{
    RtkWristbandSys.sleep_address.p_write_addr = SLEEP_DATA_START_ADDRESS;
    RtkWristbandSys.sleep_address.p_read_addr = SLEEP_DATA_START_ADDRESS;
    uint8_t prev_bp_lv = 0;
    uint32_t s;
    s = os_lock();
    flash_sw_protect_unlock_by_addr_locked(SLEEP_DATA_START_ADDRESS, &prev_bp_lv);
    flash_erase_locked(FLASH_ERASE_SECTOR, SLEEP_DATA_START_ADDRESS);
    flash_erase_locked(FLASH_ERASE_SECTOR, SLEEP_DATA_START_ADDRESS + FLASH_SECTOR_SIZE);
    os_unlock(s);
    flash_set_block_protect_locked(prev_bp_lv);
}



bool WristBandSleepDataStore(uint8_t *buf, uint16_t len)
{
    if ((RtkWristbandSys.sleep_address.p_write_addr + len) > SLEEP_DATA_START_ADDRESS + 2 *
        FLASH_SECTOR_SIZE)
    {
        APP_PRINT_ERROR0("Sleep data expire MAX flash size!");
        return false;
    }
    else
    {
        uint8_t prev_bp_lv = 0;
        flash_sw_protect_unlock_by_addr_locked(SLEEP_DATA_START_ADDRESS, &prev_bp_lv);

        uint32_t data = 0;
        for (uint8_t i = 0; i < len; i = i + 4)
        {
            memcpy(&data, buf + i, 4);
            if (flash_auto_write_locked(RtkWristbandSys.sleep_address.p_write_addr + i, data) == false)
            {
                APP_PRINT_ERROR0("Sleep data save error!");
            }
        }
        RtkWristbandSys.sleep_address.p_write_addr = RtkWristbandSys.sleep_address.p_write_addr + len;

        flash_set_block_protect_locked(prev_bp_lv);
        return true;
    }
}

static bool WristBandSleepDataRestore(uint8_t *buf, uint16_t len)
{
    if ((len % 4) != 0)
    {
        return false;
    }
    if ((RtkWristbandSys.sleep_address.p_read_addr + len) > RtkWristbandSys.sleep_address.p_write_addr)
    {
        return false;
    }
    if (RtkWristbandSys.sleep_address.p_read_addr == RtkWristbandSys.sleep_address.p_write_addr)
    {
        return false;
    }
    uint32_t data = 0;
    for (uint8_t i = 0; i < len; i = i + 4)
    {
        if (flash_auto_read_locked((RtkWristbandSys.sleep_address.p_read_addr + i) | 0x4000000,
                                   &data) == false)
        {
            APP_PRINT_ERROR0("sleep data read error!");
        }
        memcpy(buf + i, &data, 4);
    }
    RtkWristbandSys.sleep_address.p_read_addr = RtkWristbandSys.sleep_address.p_read_addr + len;
    /* we had better do garbage collection here */
    if ((RtkWristbandSys.sleep_address.p_read_addr > SLEEP_DATA_START_ADDRESS + FLASH_SECTOR_SIZE) && \
        (RtkWristbandSys.sleep_address.p_write_addr > SLEEP_DATA_START_ADDRESS + FLASH_SECTOR_SIZE) && \
        (RtkWristbandSys.sleep_address.p_read_addr == RtkWristbandSys.sleep_address.p_write_addr))
    {
        RtkWristbandSys.sleep_address.p_write_addr = SLEEP_DATA_START_ADDRESS;
        RtkWristbandSys.sleep_address.p_read_addr = SLEEP_DATA_START_ADDRESS;
        uint8_t prev_bp_lv = 0;
        flash_sw_protect_unlock_by_addr_locked(SLEEP_DATA_START_ADDRESS, &prev_bp_lv);
        uint32_t s;
        s = os_lock();
        flash_erase_locked(FLASH_ERASE_SECTOR, SLEEP_DATA_START_ADDRESS);
        flash_erase_locked(FLASH_ERASE_SECTOR, SLEEP_DATA_START_ADDRESS + FLASH_SECTOR_SIZE);
        os_unlock(s);
        flash_set_block_protect_locked(prev_bp_lv);
        APP_PRINT_WARN0("Sleep garbage collection !");
    }
    else
    {
        APP_PRINT_INFO0("no need do garbage collection !");
    }
    return true;
}

void send_sleep_data(void)
{
    uint8_t buffer[SLEEP_ITEM_LENGTH + SLEEP_HEAD_LENGTH];
    uint8_t psendbuf[0x20];
    psendbuf[8] = HEALTH_DATA_COMMAND_ID;                       /* command id*/
    psendbuf[9] = L2_HEADER_VERSION;                            /* L2 header version */
    psendbuf[10] = KEY_RETURN_SLEEP_DATA;                      /* first key, */
    psendbuf[11] = 0;                                           /* length high */
    psendbuf[12] = SLEEP_HEAD_LENGTH + SLEEP_ITEM_LENGTH;         /* length low */
    while (WristBandSleepDataRestore(buffer, SLEEP_ITEM_LENGTH + SLEEP_HEAD_LENGTH) == true)
    {
        memcpy(psendbuf + 13, buffer, SLEEP_HEAD_LENGTH + SLEEP_ITEM_LENGTH);
        if (L1_send(psendbuf, 13 + SLEEP_ITEM_LENGTH + SLEEP_HEAD_LENGTH) == false)
        {
            WristBandSleepDataStore(buffer, SLEEP_HEAD_LENGTH + SLEEP_ITEM_LENGTH);
            break;
        }
    }
}


