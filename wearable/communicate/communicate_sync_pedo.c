enum { __FILE_NUM__ = 0 };

/*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file
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
#include "sync_data.h"
#include "communicate_parse.h"
#include "flash_device.h"
#include "os_sync.h"
#include "communicate_sync_pedo.h"
#include "hub_interaction.h"

void WristBandPedoDataBlockInit(void)
{
    RtkWristbandSys.sport_address.p_write_addr = PEDO_DATA_START_ADDRESS;
    RtkWristbandSys.sport_address.p_read_addr = PEDO_DATA_START_ADDRESS;
    uint8_t prev_bp_lv = 0;
    flash_sw_protect_unlock_by_addr_locked(PEDO_DATA_START_ADDRESS, &prev_bp_lv);
    uint32_t s;
    s = os_lock();
    flash_erase_locked(FLASH_ERASE_SECTOR, PEDO_DATA_START_ADDRESS);
    flash_erase_locked(FLASH_ERASE_SECTOR, PEDO_DATA_START_ADDRESS + FLASH_SECTOR_SIZE);
    os_unlock(s);
    flash_set_block_protect_locked(prev_bp_lv);
}


bool WristBandPedoDataStore(uint8_t *buf, uint16_t len)
{
    if ((RtkWristbandSys.sport_address.p_write_addr + len) > PEDO_DATA_START_ADDRESS + 2 *
        FLASH_SECTOR_SIZE)
    {
        APP_PRINT_ERROR0("Pedo data expire MAX flash size!");
        return false;
    }
    else
    {
        uint8_t prev_bp_lv = 0;
        flash_sw_protect_unlock_by_addr_locked(PEDO_DATA_START_ADDRESS, &prev_bp_lv);

        uint32_t data = 0;
        for (uint8_t i = 0; i < len; i = i + 4)
        {
            memcpy(&data, buf + i, 4);
            if (flash_auto_write_locked(RtkWristbandSys.sport_address.p_write_addr + i, data) == false)
            {
                APP_PRINT_ERROR0("Pedo data save error!");
            }
        }
        RtkWristbandSys.sport_address.p_write_addr = RtkWristbandSys.sport_address.p_write_addr + len;

        flash_set_block_protect_locked(prev_bp_lv);
        return true;
    }
}

static bool WristBandPedoDataRestore(uint8_t *buf, uint16_t len)
{
    if ((len % 4) != 0)
    {
        return false;
    }
    if ((RtkWristbandSys.sport_address.p_read_addr + len) > RtkWristbandSys.sport_address.p_write_addr)
    {
        return false;
    }
    if (RtkWristbandSys.sport_address.p_read_addr == RtkWristbandSys.sport_address.p_write_addr)
    {
        return false;
    }
    uint32_t data = 0;
    for (uint8_t i = 0; i < len; i = i + 4)
    {
        if (flash_auto_read_locked((RtkWristbandSys.sport_address.p_read_addr + i) | 0x4000000,
                                   &data) == false)
        {
            APP_PRINT_ERROR0("sport data read error!");
        }
        memcpy(buf + i, &data, 4);
    }
    RtkWristbandSys.sport_address.p_read_addr = RtkWristbandSys.sport_address.p_read_addr + len;
    /* we had better do garbage collection here */
    if ((RtkWristbandSys.sport_address.p_read_addr > PEDO_DATA_START_ADDRESS + FLASH_SECTOR_SIZE) && \
        (RtkWristbandSys.sport_address.p_write_addr > PEDO_DATA_START_ADDRESS + FLASH_SECTOR_SIZE) && \
        (RtkWristbandSys.sport_address.p_read_addr == RtkWristbandSys.sport_address.p_write_addr))
    {
        RtkWristbandSys.sport_address.p_write_addr = PEDO_DATA_START_ADDRESS;
        RtkWristbandSys.sport_address.p_read_addr = PEDO_DATA_START_ADDRESS;
        uint8_t prev_bp_lv = 0;
        flash_sw_protect_unlock_by_addr_locked(PEDO_DATA_START_ADDRESS, &prev_bp_lv);
        uint32_t s;
        s = os_lock();
        flash_erase_locked(FLASH_ERASE_SECTOR, PEDO_DATA_START_ADDRESS);
        flash_erase_locked(FLASH_ERASE_SECTOR, PEDO_DATA_START_ADDRESS + FLASH_SECTOR_SIZE);
        os_unlock(s);
        flash_set_block_protect_locked(prev_bp_lv);
        APP_PRINT_WARN0("Pedo garbage collection !");
    }
    else
    {
        APP_PRINT_INFO0("no need do garbage collection !");
    }
    return true;
}

void send_sport_data(void)
{
    uint8_t buffer[PEDO_ITEM_LENGTH + PEDO_HEAD_LENGTH];
    uint8_t psendbuf[0x20];
    psendbuf[8] = HEALTH_DATA_COMMAND_ID;                       /* command id*/
    psendbuf[9] = L2_HEADER_VERSION;                            /* L2 header version */
    psendbuf[10] = KEY_RETURN_SPORTS_DATA;                      /* first key, */
    psendbuf[11] = 0;                                           /* length high */
    psendbuf[12] = PEDO_HEAD_LENGTH + PEDO_ITEM_LENGTH;         /* length low */
    while (WristBandPedoDataRestore(buffer, PEDO_ITEM_LENGTH + PEDO_HEAD_LENGTH) == true)
    {
        memcpy(psendbuf + 13, buffer, PEDO_HEAD_LENGTH + PEDO_ITEM_LENGTH);
        if (L1_send(psendbuf, 13 + PEDO_HEAD_LENGTH + PEDO_ITEM_LENGTH) == false)
        {
            WristBandPedoDataStore(buffer, PEDO_ITEM_LENGTH + PEDO_HEAD_LENGTH);
            break;
        }
    }

    if (RtkWristbandSys.gPedoData.quarter_steps != 0)
    {
        SportsData_U    mSportsData;
        SportsHead_t    mSportHead;
        /*send the current 15 minutes sport data*/
        APP_PRINT_INFO0("sync sport history for RAM");
        mSportHead.Date.date.day = RtkWristbandSys.Global_Time.day;
        mSportHead.Date.date.month = RtkWristbandSys.Global_Time.month;
        mSportHead.Date.date.year = RtkWristbandSys.Global_Time.year - 2000;
        mSportHead.length = 1;

        mSportsData.bits.mode = 0; /* TODO */
        mSportsData.bits.steps = RtkWristbandSys.gPedoData.quarter_steps;
        mSportsData.bits.Distance = (uint16_t)(RtkWristbandSys.gPedoData.quarter_distance / 1600);
        mSportsData.bits.Calory = ((uint32_t)((RtkWristbandSys.gPedoData.quarter_calories) / 4) & 0x7ffff);
        mSportsData.bits.offset = ((RtkWristbandSys.SecondCountRTC / 60) % 1440) / 15;
        psendbuf[13] = mSportHead.Date.data >> 8;
        psendbuf[14] = mSportHead.Date.data;
        psendbuf[15] = 0;
        psendbuf[16] = 1;

        psendbuf[17] = mSportsData.data >> 56;
        psendbuf[18] = mSportsData.data >> 48;
        psendbuf[19] = mSportsData.data >> 40;
        psendbuf[20] = mSportsData.data >> 32;
        psendbuf[21] = mSportsData.data >> 24;
        psendbuf[22] = mSportsData.data >> 16;
        psendbuf[23] = mSportsData.data >> 8;
        psendbuf[24] = mSportsData.data;

        if (L1_send(psendbuf, 13 + 12) == false)
        {
            APP_PRINT_ERROR0("sync ram pedo data fail!");
        }
    }
}

void minute_sport_handler(void)
{
    static uint32_t recored_last_step = 0;
    static uint32_t recored_current_step = 0;
    recored_last_step = recored_current_step;
    recored_current_step = RtkWristbandSys.gPedoData.global_steps;
    uint8_t buffer[12] = {0};
    uint32_t detal = 0;
    SportsData_U    mSportsData;
    SportsHead_t    mSportHead;

    if ((RtkWristbandSys.Global_Time.minutes % 15 == 0) &&
        (RtkWristbandSys.gPedoData.quarter_steps != 0))
    {
        /* the pedo data not belong this offset */
        uint32_t pedoDataSecond = RtkWristbandSys.SecondCountRTC - 60;
        convert_to_utc_time(pedoDataSecond);
        mSportHead.Date.date.day = RtkWristbandSys.Global_Time.day;
        mSportHead.Date.date.month = RtkWristbandSys.Global_Time.month;
        mSportHead.Date.date.year = RtkWristbandSys.Global_Time.year - 2000;
        mSportHead.length = 1;

        mSportsData.bits.mode = 0; /* TODO */
        mSportsData.bits.steps = RtkWristbandSys.gPedoData.quarter_steps;
        mSportsData.bits.Distance = (uint16_t)(RtkWristbandSys.gPedoData.quarter_distance / 1600);
        detal = RtkWristbandSys.gPedoData.quarter_distance - mSportsData.bits.Distance * 1600;
        RtkWristbandSys.gPedoData.global_distance = RtkWristbandSys.gPedoData.global_distance - detal;
        mSportsData.bits.Calory = ((uint32_t)((RtkWristbandSys.gPedoData.quarter_calories) / 4) & 0x7ffff);
        detal = RtkWristbandSys.gPedoData.quarter_calories - mSportsData.bits.Calory * 4;
        RtkWristbandSys.gPedoData.global_calorys = RtkWristbandSys.gPedoData.global_calorys - detal;
        mSportsData.bits.offset = ((pedoDataSecond / 60) % 1440) / 15;
        convert_to_utc_time(RtkWristbandSys.SecondCountRTC);

        buffer[0] = mSportHead.Date.data >> 8;
        buffer[1] = mSportHead.Date.data;
        buffer[2] = 0;
        buffer[3] = mSportHead.length;
        buffer[4] = mSportsData.data >> 56;
        buffer[5] = mSportsData.data >> 48;
        buffer[6] = mSportsData.data >> 40;
        buffer[7] = mSportsData.data >> 32;
        buffer[8] = mSportsData.data >> 24;
        buffer[9] = mSportsData.data >> 16;
        buffer[10] = mSportsData.data >> 8;
        buffer[11] = mSportsData.data;

        APP_PRINT_INFO0("save pedo quarter data\n\n");
        WristBandPedoDataStore(buffer, PEDO_ITEM_LENGTH + PEDO_HEAD_LENGTH);

        RtkWristbandSys.gPedoData.quarter_steps = RtkWristbandSys.gPedoData.quarter_distance =
                                                      RtkWristbandSys.gPedoData.quarter_calories =
                                                          0;
    }

    if (RtkWristbandSys.Global_Time.hour == 0 && RtkWristbandSys.Global_Time.minutes == 0)
    {
        APP_PRINT_INFO0("coming into new day");
        RtkWristbandSys.flag_field.daily_target_achieved = false;
        RtkWristbandSys.gPedoData.global_steps = 0;
        RtkWristbandSys.gPedoData.global_calorys = 0;
        RtkWristbandSys.gPedoData.global_distance = 0;
    }

    if ((recored_current_step == recored_last_step) &&
        (RtkWristbandSys.sit_alert_data.sit_alert.on_off))
    {
        RtkWristbandSys.current_stationary_time++;
        APP_PRINT_INFO2("DO long time sit alarm: current_stationary_time: %d,  sit_min: %d",
                        RtkWristbandSys.current_stationary_time, RtkWristbandSys.sit_alert_data.sit_alert.sit_min);
        if (RtkWristbandSys.current_stationary_time > RtkWristbandSys.sit_alert_data.sit_alert.sit_min)
        {
            /* long time no movement alert */
            wristband_system_interact(INTERACT_NO_MOVEMENT, 0, NULL);
            RtkWristbandSys.current_stationary_time = 0;
        }
        if (RtkWristbandSys.wristband_sleep_status != TGET_UP)
        {
            RtkWristbandSys.current_stationary_time = 0;
        }
    }
    else
    {
        RtkWristbandSys.current_stationary_time = 0;
    }
}

void doNewTimeSettingForPedoData(uint32_t old_sec, uint32_t new_sec)
{
    APP_PRINT_INFO0("do New Time Setting For PedoData!");
    uint8_t buffer[12] = {0};
    uint32_t detal = 0;
    SportsData_U    mSportsData;
    SportsHead_t    mSportHead;

    if (((old_sec / 60) / 1440) != ((new_sec / 60) / 1440))
    {
        APP_PRINT_INFO0("offset NOT equal, new day clear all sport data!");
        RtkWristbandSys.gPedoData.quarter_steps = 0;
        RtkWristbandSys.gPedoData.quarter_distance = 0;
        RtkWristbandSys.gPedoData.quarter_calories = 0;
        RtkWristbandSys.gPedoData.global_steps = 0;
        RtkWristbandSys.gPedoData.global_calorys = 0;
        RtkWristbandSys.gPedoData.global_distance = 0;
    }

    if (((old_sec / 60) % 1440) / 15 != ((new_sec / 60) % 1440) / 15)
    {
        APP_PRINT_INFO0("offset NOT equal, MODIFY TIME AFTER/BEFORE CURRENT TIME!");
        if (RtkWristbandSys.gPedoData.quarter_steps != 0)
        {
            /* the pedo data not belong this offset */
            uint32_t pedoDataSecond = old_sec;
            convert_to_utc_time(pedoDataSecond);
            mSportHead.Date.date.day = RtkWristbandSys.Global_Time.day;
            mSportHead.Date.date.month = RtkWristbandSys.Global_Time.month;
            mSportHead.Date.date.year = RtkWristbandSys.Global_Time.year - 2000;
            mSportHead.length = 1;

            mSportsData.bits.mode = 0; /* TODO */
            mSportsData.bits.steps = RtkWristbandSys.gPedoData.quarter_steps;
            mSportsData.bits.Distance = (uint16_t)(RtkWristbandSys.gPedoData.quarter_distance / 1600);
            detal = RtkWristbandSys.gPedoData.quarter_distance - mSportsData.bits.Distance * 1600;
            RtkWristbandSys.gPedoData.global_distance = RtkWristbandSys.gPedoData.global_distance - detal;
            mSportsData.bits.Calory = ((uint32_t)((RtkWristbandSys.gPedoData.quarter_calories) / 4) & 0x7ffff);
            detal = RtkWristbandSys.gPedoData.quarter_calories - mSportsData.bits.Calory * 4;
            RtkWristbandSys.gPedoData.global_calorys = RtkWristbandSys.gPedoData.global_calorys - detal;
            mSportsData.bits.offset = ((pedoDataSecond / 60) % 1440) / 15;
            convert_to_utc_time(new_sec);

            buffer[0] = mSportHead.Date.data >> 8;
            buffer[1] = mSportHead.Date.data;
            buffer[2] = 0;
            buffer[3] = mSportHead.length;
            buffer[4] = mSportsData.data >> 56;
            buffer[5] = mSportsData.data >> 48;
            buffer[6] = mSportsData.data >> 40;
            buffer[7] = mSportsData.data >> 32;
            buffer[8] = mSportsData.data >> 24;
            buffer[9] = mSportsData.data >> 16;
            buffer[10] = mSportsData.data >> 8;
            buffer[11] = mSportsData.data;

            APP_PRINT_INFO0("save pedo quarter data because change system time \n");
            WristBandPedoDataStore(buffer, PEDO_ITEM_LENGTH + PEDO_HEAD_LENGTH);

            RtkWristbandSys.gPedoData.quarter_steps = RtkWristbandSys.gPedoData.quarter_distance =
                                                          RtkWristbandSys.gPedoData.quarter_calories =
                                                              0;
        }
    }
}


