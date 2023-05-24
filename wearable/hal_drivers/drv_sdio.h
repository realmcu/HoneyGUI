/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-12-13     BalanceTWK   first version
 * 2019-06-11     WillianChan   Add SD card hot plug detection
 */

#ifndef _DRV_SDIO_H
#define _DRV_SDIO_H
#include <rtthread.h>
#include "rtdevice.h"
#include <rthw.h>
#include <drv_common.h>
#include <string.h>
#include <drivers/mmcsd_core.h>
#include <drivers/sdio.h>
#include "rtl876x.h"



#define SDIO_BUFF_SIZE       4096
#define SDIO_ALIGN_LEN       32

#ifndef SDIO_MAX_FREQ
#define SDIO_MAX_FREQ        (1000000)
#endif

#ifndef SDIO_CLOCK_FREQ
#define SDIO_CLOCK_FREQ      (40U * 1000 * 1000)
#endif



#define HW_SDIO_IT_CD                    (0x01U << 0)
#define HW_SDIO_IT_RE                    (0x01U << 1)
#define HW_SDIO_IT_CDDONE                    (0x01U << 2)
#define HW_SDIO_IT_DTO                    (0x01U << 3)
#define HW_SDIO_IT_TXDR                    (0x01U << 4)
#define HW_SDIO_IT_RXDR                     (0x01U << 5)
#define HW_SDIO_IT_RCRC                     (0x01U << 6)
#define HW_SDIO_IT_DCRC                     (0x01U << 7)
#define HW_SDIO_IT_RTO                     (0x01U << 8)
#define HW_SDIO_IT_DRTO                    (0x01U << 9)
#define HW_SDIO_IT_HTO                     (0x01U << 10)
#define HW_SDIO_IT_FRUN                      (0x01U << 11)
#define HW_SDIO_IT_HLE                       (0x01U << 12)
#define HW_SDIO_IT_BCI                       (0x01U << 13)
#define HW_SDIO_IT_ACD                    (0x01U << 14)
#define HW_SDIO_IT_EBE                    (0x01U << 15)


//#define HW_SDIO_ERRORS \
//    (HW_SDIO_IT_CCRCFAIL | HW_SDIO_IT_CTIMEOUT | \
//     HW_SDIO_IT_DCRCFAIL | HW_SDIO_IT_DTIMEOUT | \
//     HW_SDIO_IT_RXOVERR  | HW_SDIO_IT_TXUNDERR)

//#define HW_SDIO_POWER_OFF                      (0x00U)
//#define HW_SDIO_POWER_UP                       (0x02U)
//#define HW_SDIO_POWER_ON                       (0x03U)

//#define HW_SDIO_FLOW_ENABLE                    (0x01U << 14)
//#define HW_SDIO_BUSWIDE_1B                     (0x00U << 11)
//#define HW_SDIO_BUSWIDE_4B                     (0x01U << 11)
//#define HW_SDIO_BUSWIDE_8B                     (0x02U << 11)
//#define HW_SDIO_BYPASS_ENABLE                  (0x01U << 10)
//#define HW_SDIO_IDLE_ENABLE                    (0x01U << 9)
//#define HW_SDIO_CLK_ENABLE                     (0x01U << 8)

//#define HW_SDIO_SUSPEND_CMD                    (0x01U << 11)
//#define HW_SDIO_CPSM_ENABLE                    (0x01U << 10)
//#define HW_SDIO_WAIT_END                       (0x01U << 9)
//#define HW_SDIO_WAIT_INT                       (0x01U << 8)
#define HW_SDIO_RESPONSE_NO                    (0x00U << 6)
#define HW_SDIO_RESPONSE_SHORT                 (0x01U << 6)
#define HW_SDIO_RESPONSE_LONG                  (0x03U << 6)

//#define HW_SDIO_DATA_LEN_MASK                  (0x01FFFFFFU)

//#define HW_SDIO_IO_ENABLE                      (0x01U << 11)
//#define HW_SDIO_RWMOD_CK                       (0x01U << 10)
//#define HW_SDIO_RWSTOP_ENABLE                  (0x01U << 9)
//#define HW_SDIO_RWSTART_ENABLE                 (0x01U << 8)
//#define HW_SDIO_DBLOCKSIZE_1                   (0x00U << 4)
//#define HW_SDIO_DBLOCKSIZE_2                   (0x01U << 4)
//#define HW_SDIO_DBLOCKSIZE_4                   (0x02U << 4)
//#define HW_SDIO_DBLOCKSIZE_8                   (0x03U << 4)
//#define HW_SDIO_DBLOCKSIZE_16                  (0x04U << 4)
//#define HW_SDIO_DBLOCKSIZE_32                  (0x05U << 4)
//#define HW_SDIO_DBLOCKSIZE_64                  (0x06U << 4)
//#define HW_SDIO_DBLOCKSIZE_128                 (0x07U << 4)
//#define HW_SDIO_DBLOCKSIZE_256                 (0x08U << 4)
//#define HW_SDIO_DBLOCKSIZE_512                 (0x09U << 4)
//#define HW_SDIO_DBLOCKSIZE_1024                (0x0AU << 4)
//#define HW_SDIO_DBLOCKSIZE_2048                (0x0BU << 4)
//#define HW_SDIO_DBLOCKSIZE_4096                (0x0CU << 4)
//#define HW_SDIO_DBLOCKSIZE_8192                (0x0DU << 4)
//#define HW_SDIO_DBLOCKSIZE_16384               (0x0EU << 4)
//#define HW_SDIO_DMA_ENABLE                     (0x01U << 3)
//#define HW_SDIO_STREAM_ENABLE                  (0x01U << 2)
//#define HW_SDIO_TO_HOST                        (0x01U << 1)
//#define HW_SDIO_DPSM_ENABLE                    (0x01U << 0)

#define HW_SDIO_DATATIMEOUT                    (0xF0000000U)


struct rtk_sdio
{
    __IO uint32_t CTRL;                   /*!< 0x00 */
    __IO uint32_t PWREN;                  /*!< 0x04 */
    __IO uint32_t CLKDIV;                 /*!< 0x08 */
    __IO uint32_t CLKSRC;                 /*!< 0x0C */
    __IO uint32_t CLKENA;                 /*!< 0x10 */
    __IO uint32_t TMOUT;                  /*!< 0x14 */
    __IO uint32_t CTYPE;                  /*!< 0x18 */
    __IO uint32_t BLKSIZ;                 /*!< 0x1C */
    __IO uint32_t BYTCNT;                 /*!< 0x20 */
    __IO uint32_t INTMASK;                /*!< 0x24 */
    __IO uint32_t CMDARG;                 /*!< 0x28 */
    __IO uint32_t CMD;                    /*!< 0x2C */
    __I  uint32_t RESP0;                  /*!< 0x30 */
    __I  uint32_t RESP1;                  /*!< 0x34 */
    __I  uint32_t RESP2;                  /*!< 0x38 */
    __I  uint32_t RESP3;                  /*!< 0x3C */
    __I  uint32_t MINTSTS;                /*!< 0x40 */
    __IO uint32_t RINTSTS;                /*!< 0x44 */
    __I  uint32_t STATUS;                 /*!< 0x48 */
    __IO uint32_t FIFOTH;                 /*!< 0x4C */
    __I  uint32_t CDETECT;                /*!< 0x50 */
    __I  uint32_t WRTPRT;                 /*!< 0x54 */
    __IO uint32_t GPIO;                   /*!< 0x58 */
    __I  uint32_t TCBCNT;                 /*!< 0x5C */
    __I  uint32_t TBBCNT;                 /*!< 0x60 */
    __IO uint32_t DEBNCE;                 /*!< 0x64 */
    __IO uint32_t USRID;                  /*!< 0x68 */
    __I  uint32_t VERID;                  /*!< 0x6C */
    __I  uint32_t HCON;                   /*!< 0x70 */
    __IO uint32_t UHS_REG;                /*!< 0x74 */
    __IO uint32_t RST_n;                  /*!< 0x78 */
    __I  uint32_t Reserved0;              /*!< 0x7C */
    __IO uint32_t BMOD;                   /*!< 0x80 */
    __O  uint32_t PLDMND;                 /*!< 0x84 */
    __IO uint32_t DBADDR;                 /*!< 0x88 */
    __IO uint32_t IDSTS;                  /*!< 0x8C */
    __IO uint32_t IDINTEN;                /*!< 0x90 */
    __I  uint32_t DSCADDR;                /*!< 0x94 */
    __I  uint32_t BUFADDR;                /*!< 0x98 */
    __I  uint8_t Reserved1[0x100 - 0x9C];            /*!< 0x9C */
    __IO uint32_t CardThrCtl;             /*!< 0x100 */
    __IO uint32_t Back_end_power;         /*!< 0x104 */
    __IO uint32_t UHS_REG_EXT;            /*!< 0x108 */
    __IO uint32_t EMMC_DDR_REG;           /*!< 0x10C */
    __IO uint32_t ENABLE_SHIFT;           /*!< 0x110 */
    __IO uint8_t Reserved2[0x100 - 0x14];        /*!< 0x114 */
    __IO uint32_t DATA;                   /*!< >=0x200 */
};

typedef rt_err_t (*dma_txconfig)(rt_uint32_t *src, rt_uint32_t *dst, int size);
typedef rt_err_t (*dma_rxconfig)(rt_uint32_t *src, rt_uint32_t *dst, int size);
typedef rt_uint32_t (*sdio_clk_get)(struct rtk_sdio *hw_sdio);

struct rtk_sdio_des
{
    struct rtk_sdio *hw_sdio;
    dma_txconfig txconfig;
    dma_rxconfig rxconfig;
    sdio_clk_get clk_get;
};

extern void stm32_mmcsd_change(void);

#endif
