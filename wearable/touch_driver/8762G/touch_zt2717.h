/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author        Notes
 * 2020-08-04     HOWIE  first version
 */

#ifndef __TOUCH_ZT2717_H
#define __TOUCH_ZT2717_H

#include "stdint.h"
#include "stdbool.h"
#include "board.h"


#ifdef __cplusplus
extern "C" {
#endif

#ifdef TPD_HIGH_SPEED_DMA
#define TOUCH_POINT_MODE    0
#else
#define TOUCH_POINT_MODE    1
#endif

/* max 10 */
#define MAX_SUPPORTED_FINGER_NUM    2
#define TPD_RES_MAX_X   280
#define TPD_RES_MAX_Y   456

/* Register Map*/
#define ZINITIX_SWRESET_CMD     0x0000
#define ZINITIX_WAKEUP_CMD      0x0001

#define ZINITIX_IDLE_CMD        0x0004
#define ZINITIX_SLEEP_CMD       0x0005

#define ZINITIX_CLEAR_INT_STATUS_CMD    0x0003
#define ZINITIX_CALIBRATE_CMD       0x0006
#define ZINITIX_SAVE_STATUS_CMD     0x0007
#define ZINITIX_SAVE_CALIBRATION_CMD    0x0008
#define ZINITIX_RECALL_FACTORY_CMD  0x000f

#define ZINITIX_SENSITIVITY 0x0020

#define ZINITIX_I2C_CHECKSUM_WCNT   0x016a
#define ZINITIX_I2C_CHECKSUM_RESULT 0x016c


#define ZINITIX_DEBUG_REG       0x0115  //0~7

#define ZINITIX_TOUCH_MODE      0x0010
#define ZINITIX_CHIP_REVISION       0x0011
#define ZINITIX_FIRMWARE_VERSION    0x0012

#define ZINITIX_MINOR_FW_VERSION    0x0121

#define ZINITIX_DATA_VERSION_REG    0x0013
#define ZINITIX_HW_ID           0x0014
#define ZINITIX_SUPPORTED_FINGER_NUM    0x0015
#define ZINITIX_EEPROM_INFO     0x0018
#define ZINITIX_INITIAL_TOUCH_MODE      0x0019


#define ZINITIX_TOTAL_NUMBER_OF_X   0x0060
#define ZINITIX_TOTAL_NUMBER_OF_Y   0x0061

#define ZINITIX_DELAY_RAW_FOR_HOST  0x007f

#define ZINITIX_BUTTON_SUPPORTED_NUM    0x00B0
#define ZINITIX_BUTTON_SENSITIVITY  0x00B2


#define ZINITIX_X_RESOLUTION        0x00C0
#define ZINITIX_Y_RESOLUTION        0x00C1


#define ZINITIX_POINT_STATUS_REG    0x0080
#define ZINITIX_ICON_STATUS_REG     0x00AA

#define ZINITIX_AFE_FREQUENCY       0x0100
#define ZINITIX_DND_N_COUNT         0x0122

#define ZINITIX_RAWDATA_REG     0x0200

#define ZINITIX_EEPROM_INFO_REG     0x0018

#define ZINITIX_INT_ENABLE_FLAG     0x00f0
#define ZINITIX_PERIODICAL_INTERRUPT_INTERVAL   0x00f1

#define ZINITIX_BTN_WIDTH       0x016d

#define ZINITIX_CHECKSUM_RESULT 0x012c

#define ZINITIX_INIT_FLASH      0x01d0
#define ZINITIX_WRITE_FLASH     0x01d1
#define ZINITIX_READ_FLASH      0x01d2

#define ZINITIX_INTERNAL_FLAG_02 0x011e


#define TOUCH_ZT2717_ADDR                                      0x20
#define TOUCH_ZT2717_CHIP_ID                                   0x15
#if 1 // 1 QFN68 -  0 QFN88
#define TOUCH_ZT2717_RST                                          P2_3
#define TOUCH_ZT2717_INT                                          P2_4
#define TOUCH_ZT2717_SCL                                          P5_7
#define TOUCH_ZT2717_SDA                                          P1_3
#else
#define TOUCH_ZT2717_RST                                          P5_0
#define TOUCH_ZT2717_INT                                          P2_2
#define TOUCH_ZT2717_SCL                                          P2_3
#define TOUCH_ZT2717_SDA                                          P2_4
#endif
struct _ts_zinitix_coord
{
    uint16_t    x;
    uint16_t    y;
    uint8_t    width;
    uint8_t    sub_status;
};

struct _ts_zinitix_point_info
{
    uint16_t    status;
#if TOUCH_POINT_MODE
    uint16_t event_flag;
#else
    uint8_t    finger_cnt;
    uint8_t    time_stamp;
#endif
    struct _ts_zinitix_coord    coord[MAX_SUPPORTED_FINGER_NUM];
};


bool rtk_touch_hal_read_all(uint16_t *x, uint16_t *y, bool *pressing);

#ifdef __cplusplus
}
#endif

#endif /* __TOUCH_ZT2717_H */
