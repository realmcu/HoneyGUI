/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-12-05     tyustli      the first version
 */
#ifndef _FAL_CFG_H_
#define _FAL_CFG_H_

#include <board.h>
#define FAL_PART_HAS_TABLE_CFG
extern const struct fal_flash_dev realtek_nor_flash;
extern struct fal_flash_dev sram0;

/* flash device table */
#define FAL_FLASH_DEV_TABLE                                          \
    {                                                                    \
        &realtek_nor_flash,                                           \
        /*&sram0,*/                                                          \
    }
/* ====================== Partition Configuration ========================== */
#ifdef FAL_PART_HAS_TABLE_CFG

#define FDB_START_ADDRESS 0

#define FDB_KVDB1_OFFSET FDB_START_ADDRESS
#define FDB_KVDB1_SIZE 8 * 1024

#define FDB_TSDB2_OFFSET (FDB_KVDB1_OFFSET + FDB_KVDB1_SIZE)
#define FDB_TSDB2_SIZE 8 * 1024

#define KVDB_TUYA_OFFSET (FDB_TSDB2_OFFSET + FDB_TSDB2_SIZE)
#define KVDB_TUYA_SIZE 8 * 1024
#define KVDB_TUYA_PART_NAME "kvdb_tuya"

#define TSDB_SPORT_MIN_OFFSET (KVDB_TUYA_OFFSET + KVDB_TUYA_SIZE)
#define TSDB_SPORT_MIN_SIZE 16 * 1024
#define TSDB_SPORT_MIN_PART_NAME "tsdb_sport_min"

#define KVDB_MCU_CFG_CONST_OFFSET (TSDB_SPORT_MIN_OFFSET + TSDB_SPORT_MIN_SIZE)
#define KVDB_MCU_CFG_CONST_SIZE 24 * 1024
#define KVDB_MCU_CFG_CONST_PART_NAME "kvdb_mcu_cfg_const"

#define KVDB_MCU_CFG_RW_OFFSET (KVDB_MCU_CFG_CONST_OFFSET + KVDB_MCU_CFG_CONST_SIZE)
#define KVDB_MCU_CFG_RW_SIZE 24 * 1024
#define KVDB_MCU_CFG_RW_PART_NAME "kvdb_mcu_cfg_rw"

/* partition table */
#define FAL_PART_TABLE                                                                                                    \
    {                                                                                                                         \
        {FAL_PART_MAGIC_WROD,        "fdb_kvdb1",    "nor_flash",                    FDB_KVDB1_OFFSET,      FDB_KVDB1_SIZE, 0},  \
        {FAL_PART_MAGIC_WROD,        "fdb_tsdb1",    "nor_flash",                    FDB_TSDB2_OFFSET,      FDB_TSDB2_SIZE, 0},  \
        {FAL_PART_MAGIC_WROD,        KVDB_TUYA_PART_NAME,    "nor_flash",                   KVDB_TUYA_OFFSET,      KVDB_TUYA_SIZE, 0},  \
        {FAL_PART_MAGIC_WROD,        TSDB_SPORT_MIN_PART_NAME,    "nor_flash",                    TSDB_SPORT_MIN_OFFSET,      TSDB_SPORT_MIN_SIZE, 0},  \
        {FAL_PART_MAGIC_WROD,        KVDB_MCU_CFG_CONST_PART_NAME,    "nor_flash",                    KVDB_MCU_CFG_CONST_OFFSET,      KVDB_MCU_CFG_CONST_SIZE, 0},  \
        {FAL_PART_MAGIC_WROD,        KVDB_MCU_CFG_RW_PART_NAME,    "nor_flash",                    KVDB_MCU_CFG_RW_OFFSET,      KVDB_MCU_CFG_RW_SIZE, 0},  \
    }
#endif /* FAL_PART_HAS_TABLE_CFG */
#endif /* _FAL_CFG_H_ */
