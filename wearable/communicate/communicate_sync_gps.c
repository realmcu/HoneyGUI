enum { __FILE_NUM__ = 0 };

/*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     app_flash.c
* @brief
* @details
* @author
* @date
* @version  v0.1
*********************************************************************************************************
*/

#include "board.h"
#include "trace.h"
#include <string.h>
#include "ftl.h"
#include "app_flash_config.h"
#include "os_sync.h"

void WristBandGPSDataStoreInit(void)
{

}


bool WristBandGPSDataStore(uint8_t *buf, uint16_t len)
{
    if ((RtkWristbandSys.gps_address.p_write_addr + len) > GPS_DATA_START_ADDRESS + 2 *
        FLASH_SECTOR_SIZE)
    {
        APP_PRINT_ERROR0("Pedo data expire MAX flash size!");
        return false;
    }
    else
    {
        uint8_t prev_bp_lv = 0;
        flash_sw_protect_unlock_by_addr_locked(GPS_DATA_START_ADDRESS, &prev_bp_lv);

        uint32_t data = 0;
        for (uint8_t i = 0; i < len; i = i + 4)
        {
            memcpy(&data, buf + i, 4);
            if (flash_auto_write_locked(RtkWristbandSys.gps_address.p_write_addr + i, data) == false)
            {
                APP_PRINT_ERROR0("GPS data save error!");
            }
        }
        RtkWristbandSys.gps_address.p_write_addr = RtkWristbandSys.gps_address.p_write_addr + len;

        flash_set_block_protect_locked(prev_bp_lv);
        return true;
    }
}

bool WristBandGPSDataRestore(uint8_t *buf, uint16_t len)
{
    if ((len % 4) != 0)
    {
        return false;
    }
    if ((RtkWristbandSys.gps_address.p_read_addr + len) > RtkWristbandSys.gps_address.p_write_addr)
    {
        return false;
    }
    uint32_t data = 0;
    for (uint8_t i = 0; i < len; i = i + 4)
    {
        if (flash_auto_read_locked((RtkWristbandSys.gps_address.p_read_addr + i) | 0x4000000,
                                   &data) == false)
        {
            APP_PRINT_ERROR0("gps data read error!");
        }
        memcpy(buf + i, &data, 4);
    }
    RtkWristbandSys.gps_address.p_read_addr = RtkWristbandSys.gps_address.p_read_addr + len;
    /* we had better do garbage collection here */
    if ((RtkWristbandSys.gps_address.p_read_addr > GPS_DATA_START_ADDRESS + FLASH_SECTOR_SIZE) && \
        (RtkWristbandSys.gps_address.p_write_addr > GPS_DATA_START_ADDRESS + FLASH_SECTOR_SIZE) && \
        (RtkWristbandSys.gps_address.p_read_addr == RtkWristbandSys.gps_address.p_write_addr))
    {
        RtkWristbandSys.gps_address.p_write_addr = GPS_DATA_START_ADDRESS;
        RtkWristbandSys.gps_address.p_read_addr = GPS_DATA_START_ADDRESS;
        uint8_t prev_bp_lv = 0;
        flash_sw_protect_unlock_by_addr_locked(GPS_DATA_START_ADDRESS, &prev_bp_lv);
        uint32_t s;
        s = os_lock();
        flash_erase_locked(FLASH_ERASE_SECTOR, GPS_DATA_START_ADDRESS);
        flash_erase_locked(FLASH_ERASE_SECTOR, GPS_DATA_START_ADDRESS + FLASH_SECTOR_SIZE);
        os_unlock(s);
        flash_set_block_protect_locked(prev_bp_lv);
        APP_PRINT_WARN0("GPS garbage collection !");
    }
    else
    {
        APP_PRINT_INFO0("no need do garbage collection !");
    }
    return true;
}


