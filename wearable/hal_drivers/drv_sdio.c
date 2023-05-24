/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-06-22     tyx          first
 * 2018-12-12     balanceTWK   first version
 * 2019-06-11     WillianChan  Add SD card hot plug detection
 * 2020-11-09     whj4674672   fix sdio non-aligned access problem
 */

#include "board.h"
#include "drv_sdio.h"
//#include "drv_config.h"

#ifdef BSP_USING_SDIO


#define DBG_ENABLE
#define DBG_TAG     "drv.sdio"
#define DBG_LVL     DBG_LOG
#include <rtdbg.h>


static struct rt_mmcsd_host *host;

#define SDIO_TX_RX_COMPLETE_TIMEOUT_LOOPS    (100000)

#define RTHW_SDIO_LOCK(_sdio)   rt_mutex_take(&_sdio->mutex, RT_WAITING_FOREVER)
#define RTHW_SDIO_UNLOCK(_sdio) rt_mutex_release(&_sdio->mutex);

struct sdio_pkg
{
    struct rt_mmcsd_cmd *cmd;
    void *buff;
    rt_uint32_t flag;
};

struct rthw_sdio
{
    struct rt_mmcsd_host *host;
    struct rtk_sdio_des sdio_des;
    struct rt_event event;
    struct rt_mutex mutex;
    struct sdio_pkg *pkg;
};


__attribute__((aligned(SDIO_ALIGN_LEN))) static rt_uint8_t cache_buf[SDIO_BUFF_SIZE];


static rt_uint32_t rtk_sdio_clk_get(struct rtk_sdio *hw_sdio)
{
    return SDIO_CLOCK_FREQ;
}
/**
  * @brief  This function get order from sdio.
  * @param  data
  * @retval sdio  order
  */
static int get_order(rt_uint32_t data)
{
    int order = 0;

    switch (data)
    {
    case 1:
        order = 0;
        break;
    case 2:
        order = 1;
        break;
    case 4:
        order = 2;
        break;
    case 8:
        order = 3;
        break;
    case 16:
        order = 4;
        break;
    case 32:
        order = 5;
        break;
    case 64:
        order = 6;
        break;
    case 128:
        order = 7;
        break;
    case 256:
        order = 8;
        break;
    case 512:
        order = 9;
        break;
    case 1024:
        order = 10;
        break;
    case 2048:
        order = 11;
        break;
    case 4096:
        order = 12;
        break;
    case 8192:
        order = 13;
        break;
    case 16384:
        order = 14;
        break;
    default :
        order = 0;
        break;
    }

    return order;
}

/**
  * @brief  This function wait sdio completed.
  * @param  sdio  rthw_sdio
  * @retval None
  */
static void rthw_sdio_wait_completed(struct rthw_sdio *sdio)
{
    rt_uint32_t status;
    struct rt_mmcsd_cmd *cmd = sdio->pkg->cmd;
    struct rt_mmcsd_data *data = cmd->data;
    struct rtk_sdio *hw_sdio = sdio->sdio_des.hw_sdio;

    if (rt_event_recv(&sdio->event, 0xffffffff, RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                      rt_tick_from_millisecond(5000), &status) != RT_EOK)
    {
        LOG_E("wait completed timeout");
        cmd->err = -RT_ETIMEOUT;
        return;
    }

    if (sdio->pkg == RT_NULL)
    {
        return;
    }

    cmd->resp[0] = hw_sdio->RESP0;
    cmd->resp[1] = hw_sdio->RESP1;
    cmd->resp[2] = hw_sdio->RESP2;
    cmd->resp[3] = hw_sdio->RESP3;

    if (status)
    {
        LOG_E(" ERROR ");
    }
    else
    {
        cmd->err = RT_EOK;
        LOG_D("sta:0x%08X [%08X %08X %08X %08X]", status, cmd->resp[0], cmd->resp[1], cmd->resp[2],
              cmd->resp[3]);
    }
}

/**
  * @brief  This function transfer data by dma.
  * @param  sdio  rthw_sdio
  * @param  pkg   sdio package
  * @retval None
  */
static void rthw_sdio_transfer_by_dma(struct rthw_sdio *sdio, struct sdio_pkg *pkg)
{
    struct rt_mmcsd_data *data;
    int size;
    void *buff;
    struct rtk_sdio *hw_sdio;

    if ((RT_NULL == pkg) || (RT_NULL == sdio))
    {
        LOG_E("rthw_sdio_transfer_by_dma invalid args");
        return;
    }

    data = pkg->cmd->data;
    if (RT_NULL == data)
    {
        LOG_E("rthw_sdio_transfer_by_dma invalid args");
        return;
    }

    buff = pkg->buff;
    if (RT_NULL == buff)
    {
        LOG_E("rthw_sdio_transfer_by_dma invalid args");
        return;
    }
    hw_sdio = sdio->sdio_des.hw_sdio;
    size = data->blks * data->blksize;

    if (data->flags & DATA_DIR_WRITE)
    {
        sdio->sdio_des.txconfig((rt_uint32_t *)buff, (rt_uint32_t *)&hw_sdio->DATA, size);
        //hw_sdio->dctrl |= HW_SDIO_DMA_ENABLE;
    }
    else if (data->flags & DATA_DIR_READ)
    {
        sdio->sdio_des.rxconfig((rt_uint32_t *)&hw_sdio->DATA, (rt_uint32_t *)buff, size);
        //hw_sdio->dctrl |= HW_SDIO_DMA_ENABLE | HW_SDIO_DPSM_ENABLE;
    }
}

/**
  * @brief  This function send command.
  * @param  sdio  rthw_sdio
  * @param  pkg   sdio package
  * @retval None
  */
static void rthw_sdio_send_command(struct rthw_sdio *sdio, struct sdio_pkg *pkg)
{
    struct rt_mmcsd_cmd *cmd = pkg->cmd;
    struct rt_mmcsd_data *data = cmd->data;
    struct rtk_sdio *hw_sdio = sdio->sdio_des.hw_sdio;
    rt_uint32_t reg_cmd = 0;

    /* save pkg */
    sdio->pkg = pkg;

    LOG_D("CMD:%d ARG:0x%08x RES:%s%s%s%s%s%s%s%s%s rw:%c len:%d blksize:%d",
          cmd->cmd_code,
          cmd->arg,
          resp_type(cmd) == RESP_NONE ? "NONE"  : "",
          resp_type(cmd) == RESP_R1  ? "R1"  : "",
          resp_type(cmd) == RESP_R1B ? "R1B"  : "",
          resp_type(cmd) == RESP_R2  ? "R2"  : "",
          resp_type(cmd) == RESP_R3  ? "R3"  : "",
          resp_type(cmd) == RESP_R4  ? "R4"  : "",
          resp_type(cmd) == RESP_R5  ? "R5"  : "",
          resp_type(cmd) == RESP_R6  ? "R6"  : "",
          resp_type(cmd) == RESP_R7  ? "R7"  : "",
          data ? (data->flags & DATA_DIR_WRITE ?  'w' : 'r') : '-',
          data ? data->blks * data->blksize : 0,
          data ? data->blksize : 0
         );

    /* config cmd reg */


    //reg_cmd = cmd->cmd_code | HW_SDIO_CPSM_ENABLE;
    if (resp_type(cmd) == RESP_NONE)
    {
        reg_cmd |= HW_SDIO_RESPONSE_NO;
    }
    else if (resp_type(cmd) == RESP_R2)
    {
        reg_cmd |= HW_SDIO_RESPONSE_LONG;
    }
    else
    {
        reg_cmd |= HW_SDIO_RESPONSE_SHORT;
    }

    /* config data reg */
    if (data != RT_NULL)
    {
        rt_uint32_t dir = 0;
        rt_uint32_t size = data->blks * data->blksize;
        int order;

        //hw_sdio->dctrl = 0;
        hw_sdio->TMOUT = HW_SDIO_DATATIMEOUT;
        hw_sdio->BYTCNT = size;
        order = get_order(data->blksize);
        hw_sdio->BLKSIZ = order;
        //dir = (data->flags & DATA_DIR_READ) ? HW_SDIO_TO_HOST : 0;
        //hw_sdio->dctrl = HW_SDIO_IO_ENABLE | dir;
    }

    /* transfer config */
    if (data != RT_NULL)
    {
        rthw_sdio_transfer_by_dma(sdio, pkg);
    }

    /* open irq */
//    hw_sdio->INTMASK |= HW_SDIO_IT_CMDSENT | HW_SDIO_IT_CMDREND | HW_SDIO_ERRORS;
//    if (data != RT_NULL)
//    {
//        hw_sdio->INTMASK |= HW_SDIO_IT_DATAEND;
//    }

    /* send cmd */
    hw_sdio->CMDARG = cmd->arg;
    hw_sdio->CMD = reg_cmd | BIT31;


    /* wait completed */
    rthw_sdio_wait_completed(sdio);

    /* Waiting for data to be sent to completion */
    if (data != RT_NULL)
    {
        volatile rt_uint32_t count = SDIO_TX_RX_COMPLETE_TIMEOUT_LOOPS;

        while (count && (hw_sdio->STATUS & (BIT10)))
        {
            count--;
        }

        if ((count == 0) || (hw_sdio->STATUS & BIT10))
        {
            cmd->err = -RT_ERROR;
        }
    }

    /* close irq, keep sdio irq */
    //hw_sdio->INTMASK = hw_sdio->INTMASK & HW_SDIO_IT_SDIOIT ? HW_SDIO_IT_SDIOIT : 0x00;

    /* clear pkg */
    sdio->pkg = RT_NULL;
}

/**
  * @brief  This function send sdio request.
  * @param  host  rt_mmcsd_host
  * @param  req   request
  * @retval None
  */
static void rthw_sdio_request(struct rt_mmcsd_host *host, struct rt_mmcsd_req *req)
{
    struct sdio_pkg pkg;
    struct rthw_sdio *sdio = host->private_data;
    struct rt_mmcsd_data *data;

    RTHW_SDIO_LOCK(sdio);

    if (req->cmd != RT_NULL)
    {
        rt_memset(&pkg, 0, sizeof(pkg));
        data = req->cmd->data;
        pkg.cmd = req->cmd;

        if (data != RT_NULL)
        {
            rt_uint32_t size = data->blks * data->blksize;

            RT_ASSERT(size <= SDIO_BUFF_SIZE);

            pkg.buff = data->buf;
            if ((rt_uint32_t)data->buf & (SDIO_ALIGN_LEN - 1))
            {
                pkg.buff = cache_buf;
                if (data->flags & DATA_DIR_WRITE)
                {
                    rt_memcpy(cache_buf, data->buf, size);
                }
            }
        }

        rthw_sdio_send_command(sdio, &pkg);

        if ((data != RT_NULL) && (data->flags & DATA_DIR_READ) &&
            ((rt_uint32_t)data->buf & (SDIO_ALIGN_LEN - 1)))
        {
            rt_memcpy(data->buf, cache_buf, data->blksize * data->blks);
        }
    }

    if (req->stop != RT_NULL)
    {
        rt_memset(&pkg, 0, sizeof(pkg));
        pkg.cmd = req->stop;
        rthw_sdio_send_command(sdio, &pkg);
    }

    RTHW_SDIO_UNLOCK(sdio);

    mmcsd_req_complete(sdio->host);
}

/**
  * @brief  This function config sdio.
  * @param  host    rt_mmcsd_host
  * @param  io_cfg  rt_mmcsd_io_cfg
  * @retval None
  */
static void rthw_sdio_iocfg(struct rt_mmcsd_host *host, struct rt_mmcsd_io_cfg *io_cfg)
{
    rt_uint32_t clkcr, div;
    rt_uint32_t clk_src;
    rt_uint32_t clk = io_cfg->clock;
    struct rthw_sdio *sdio = host->private_data;
    struct rtk_sdio *hw_sdio = sdio->sdio_des.hw_sdio;

    clk_src = 4000000;
    if (clk_src < 400 * 1000)
    {
        LOG_E("The clock rate is too low! rata:%d", clk_src);
        return;
    }

    if (clk > host->freq_max) { clk = host->freq_max; }

    if (clk > clk_src)
    {
        LOG_W("Setting rate is greater than clock source rate.");
        clk = clk_src;
    }

    LOG_D("clk:%d width:%s%s%s power:%s%s%s",
          clk,
          io_cfg->bus_width == MMCSD_BUS_WIDTH_8 ? "8" : "",
          io_cfg->bus_width == MMCSD_BUS_WIDTH_4 ? "4" : "",
          io_cfg->bus_width == MMCSD_BUS_WIDTH_1 ? "1" : "",
          io_cfg->power_mode == MMCSD_POWER_OFF ? "OFF" : "",
          io_cfg->power_mode == MMCSD_POWER_UP ? "UP" : "",
          io_cfg->power_mode == MMCSD_POWER_ON ? "ON" : ""
         );

    RTHW_SDIO_LOCK(sdio);

    div = clk_src / clk;
    if ((clk == 0) || (div == 0))
    {
        clkcr = 0;
    }
    else
    {
        if (div < 2)
        {
            div = 2;
        }
        else if (div > 0xFF)
        {
            div = 0xFF;
        }
        div -= 2;
        //clkcr = div | HW_SDIO_CLK_ENABLE;
    }

    if (io_cfg->bus_width == MMCSD_BUS_WIDTH_8)
    {
        //clkcr |= HW_SDIO_BUSWIDE_8B;
    }
    else if (io_cfg->bus_width == MMCSD_BUS_WIDTH_4)
    {
        //clkcr |= HW_SDIO_BUSWIDE_4B;
    }
    else
    {
        //clkcr |= HW_SDIO_BUSWIDE_1B;
    }

    if (io_cfg->bus_mode == MMCSD_BUSMODE_OPENDRAIN)
    {
        hw_sdio->CTRL |= BIT24;
    }
    else if (io_cfg->bus_mode == MMCSD_BUSMODE_PUSHPULL)
    {
        hw_sdio->CTRL &= ~BIT24;
    }

    ///hw_sdio->clkcr = clkcr;

    switch (io_cfg->power_mode)
    {
    case MMCSD_POWER_OFF:
        hw_sdio->PWREN = 0;
        break;
    case MMCSD_POWER_UP:
        hw_sdio->PWREN = 1;
        break;
    case MMCSD_POWER_ON:
        hw_sdio->PWREN = 1;
        break;
    default:
        LOG_W("unknown power_mode %d", io_cfg->power_mode);
        break;
    }

    RTHW_SDIO_UNLOCK(sdio);
}

/**
  * @brief  This function update sdio interrupt.
  * @param  host    rt_mmcsd_host
  * @param  enable
  * @retval None
  */
void rthw_sdio_irq_update(struct rt_mmcsd_host *host, rt_int32_t enable)
{
    struct rthw_sdio *sdio = host->private_data;
    struct rtk_sdio *hw_sdio = sdio->sdio_des.hw_sdio;

    if (enable)
    {
        LOG_D("enable sdio irq");
        //hw_sdio->mask |= HW_SDIO_IT_SDIOIT;
    }
    else
    {
        LOG_D("disable sdio irq");
        //hw_sdio->mask &= ~HW_SDIO_IT_SDIOIT;
    }
}

/**
  * @brief  This function detect sdcard.
  * @param  host    rt_mmcsd_host
  * @retval 0x01
  */
static rt_int32_t rthw_sd_detect(struct rt_mmcsd_host *host)
{
    LOG_D("try to detect device");
    return 0x01;
}

/**
  * @brief  This function interrupt process function.
  * @param  host  rt_mmcsd_host
  * @retval None
  */
void rthw_sdio_irq_process(struct rt_mmcsd_host *host)
{
//    int complete = 0;
//    struct rthw_sdio *sdio = host->private_data;
//    struct rtk_sdio *hw_sdio = sdio->sdio_des.hw_sdio;
//    rt_uint32_t intstatus = hw_sdio->STATUS;

//    if (intstatus & HW_SDIO_ERRORS)
//    {
//        hw_sdio->icr = HW_SDIO_ERRORS;
//        complete = 1;
//    }
//    else
//    {
//        if (intstatus & HW_SDIO_IT_CMDREND)
//        {
//            hw_sdio->icr = HW_SDIO_IT_CMDREND;

//            if (sdio->pkg != RT_NULL)
//            {
//                if (!sdio->pkg->cmd->data)
//                {
//                    complete = 1;
//                }
//                else if ((sdio->pkg->cmd->data->flags & DATA_DIR_WRITE))
//                {
//                    hw_sdio->dctrl |= HW_SDIO_DPSM_ENABLE;
//                }
//            }
//        }

//        if (intstatus & HW_SDIO_IT_CMDSENT)
//        {
//            hw_sdio->icr = HW_SDIO_IT_CMDSENT;

//            if (resp_type(sdio->pkg->cmd) == RESP_NONE)
//            {
//                complete = 1;
//            }
//        }

//        if (intstatus & HW_SDIO_IT_DATAEND)
//        {
//            hw_sdio->icr = HW_SDIO_IT_DATAEND;
//            complete = 1;
//        }
//    }

//    if ((intstatus & HW_SDIO_IT_SDIOIT) && (hw_sdio->mask & HW_SDIO_IT_SDIOIT))
//    {
//        hw_sdio->icr = HW_SDIO_IT_SDIOIT;
//        sdio_irq_wakeup(host);
//    }

//    if (complete)
//    {
//        hw_sdio->mask &= ~HW_SDIO_ERRORS;
//        rt_event_send(&sdio->event, intstatus);
//    }
}

static const struct rt_mmcsd_host_ops ops =
{
    rthw_sdio_request,
    rthw_sdio_iocfg,
    rthw_sd_detect,
    rthw_sdio_irq_update,
};


/**
  * @brief  This function get stm32 sdio clock.
  * @param  hw_sdio: rtk_sdio
  * @retval PCLK2Freq
  */
static rt_uint32_t rtk_sdio_clock_get(struct rtk_sdio *hw_sdio)
{
    //return HAL_RCC_GetPCLK2Freq();
    return 0;
}

static rt_err_t DMA_TxConfig(rt_uint32_t *src, rt_uint32_t *dst, int Size)
{
    //SD_LowLevel_DMA_TxConfig((uint32_t *)src, (uint32_t *)dst, Size / 4);
    return RT_EOK;
}

static rt_err_t DMA_RxConfig(rt_uint32_t *src, rt_uint32_t *dst, int Size)
{
    //SD_LowLevel_DMA_RxConfig((uint32_t *)src, (uint32_t *)dst, Size / 4);
    return RT_EOK;
}

/**
  * @brief  This function create mmcsd host.
  * @param  sdio_des  rtk_sdio_des
  * @retval rt_mmcsd_host
  */
struct rt_mmcsd_host *sdio_host_create(struct rtk_sdio_des *sdio_des)
{
    struct rt_mmcsd_host *host;
    struct rthw_sdio *sdio = RT_NULL;

    if ((sdio_des == RT_NULL) || (sdio_des->txconfig == RT_NULL) || (sdio_des->rxconfig == RT_NULL))
    {
        LOG_E("L:%d F:%s %s %s %s",
              (sdio_des == RT_NULL ? "sdio_des is NULL" : ""),
              (sdio_des ? (sdio_des->txconfig ? "txconfig is NULL" : "") : ""),
              (sdio_des ? (sdio_des->rxconfig ? "rxconfig is NULL" : "") : "")
             );
        return RT_NULL;
    }

    sdio = rt_malloc(sizeof(struct rthw_sdio));
    if (sdio == RT_NULL)
    {
        LOG_E("L:%d F:%s malloc rthw_sdio fail");
        return RT_NULL;
    }
    rt_memset(sdio, 0, sizeof(struct rthw_sdio));

    host = mmcsd_alloc_host();
    if (host == RT_NULL)
    {
        LOG_E("L:%d F:%s mmcsd alloc host fail");
        rt_free(sdio);
        return RT_NULL;
    }

    rt_memcpy(&sdio->sdio_des, sdio_des, sizeof(struct rtk_sdio_des));
    sdio->sdio_des.hw_sdio = (sdio_des->hw_sdio == RT_NULL ? (struct rtk_sdio *)
                              SDIO_HOST1_CFG_REG_BASE : sdio_des->hw_sdio);
    sdio->sdio_des.clk_get = (sdio_des->clk_get == RT_NULL ? rtk_sdio_clk_get : sdio_des->clk_get);

    rt_event_init(&sdio->event, "sdio", RT_IPC_FLAG_FIFO);
    rt_mutex_init(&sdio->mutex, "sdio", RT_IPC_FLAG_PRIO);

    /* set host defautl attributes */
    host->ops = &ops;
    host->freq_min = 400 * 1000;
    host->freq_max = SDIO_MAX_FREQ;
    host->valid_ocr = 0X00FFFF80;/* The voltage range supported is 1.65v-3.6v */
#ifndef SDIO_USING_1_BIT
    host->flags = MMCSD_BUSWIDTH_4 | MMCSD_MUTBLKWRITE | MMCSD_SUP_SDIO_IRQ;
#else
    host->flags = MMCSD_MUTBLKWRITE | MMCSD_SUP_SDIO_IRQ;
#endif
    host->max_seg_size = SDIO_BUFF_SIZE;
    host->max_dma_segs = 1;
    host->max_blk_size = 512;
    host->max_blk_count = 512;

    /* link up host and sdio */
    sdio->host = host;
    host->private_data = sdio;

    rthw_sdio_irq_update(host, 1);

    /* ready to change */
    mmcsd_change(host);

    return host;
}



void SDIO_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();
    /* Process All SDIO Interrupt Sources */
    rthw_sdio_irq_process(host);

    /* leave interrupt */
    rt_interrupt_leave();
}

#if 0
#include "rtl_pinmux.h"
#include "rtl876x_sdio.h"
#define CLK_40M_FREQ_kHz        (20*1000)//40M
#define ROUND_UP_DIVIDE(a, b)   (((a)+(b)-1)/(b))

static void Sdio_InitClk(SDIO_TypeDef *Sdio)
{

    Sdh0Phy0_t sdh0phy0 = {.d32 = SDH0_PHY0_REG};
    sdh0phy0.b.r_sdh0_clk_sel_drv = 1;
    sdh0phy0.b.r_sdh0_clk_sel_sample = 1;
    SDH0_PHY0_REG = sdh0phy0.d32;

    SdhCtl_t sdhctl = {.d32 = SDH_CTL_REG};
    sdhctl.b.sdio0_ck_en = 1;
    sdhctl.b.sdio0_func_en = 1;
    sdhctl.b.r_sdio0_clk_src_en = 1;
    sdhctl.b.r_sdio0_div_sel = 0;
    sdhctl.b.r_sdio0_div_en = 0;
    sdhctl.b.r_sdio0_mux_clk_cg_en = 1;
    sdhctl.b.r_sdio0_clk_src_sel = 0;
    SDH_CTL_REG = sdhctl.d32;

    rt_kprintf("InitClk, 0x400E_21E8: 0x%x, 0x400E_21EC: 0x%x", SDH_CTL_REG, SDH0_PHY0_REG);
}

static void Sdio_ResetAll(SDIO_TypeDef *Sdio)
{
    CTRL_t ctrl = {.d32 = Sdio->CTRL};
    ctrl.b.controller_reset = 1;
    ctrl.b.fifo_reset = 1;
    ctrl.b.dma_reset = 1;
    Sdio->CTRL = ctrl.d32;
    do
    {
        ctrl.d32 = Sdio->CTRL;
    }
    while (ctrl.b.controller_reset == 1 || ctrl.b.fifo_reset == 1 || ctrl.b.dma_reset == 1);
}

void Sdio_SetClk(SDIO_TypeDef *Sdio, uint32_t Freq_kHz)
{
    uint32_t Divider = ROUND_UP_DIVIDE(CLK_40M_FREQ_kHz, Freq_kHz);
    Divider = ROUND_UP_DIVIDE(Divider, 2);

    Sdio->CLKDIV = Divider;

    Sdio->CLKSRC = 0;

    static const CLKENA_t clkena = {.b = {.cclk_enable = 1, .cclk_low_power = 1}};
    Sdio->CLKENA = clkena.d32;

    CMD_t cmd = {.d32 = Sdio->CMD};
    cmd.b.start_cmd = 1;
    cmd.b.update_clock_registers_only = 1;
    cmd.b.wait_prvdata_complete = 1;
    Sdio->CMD = cmd.d32;
    do
    {
        cmd.d32 = Sdio->CMD;
        // TODO: Support HLE handling. P219
    }
    while (cmd.b.start_cmd == 1);
}

int rt_hw_sdio_init(void)
{
    /**********for test**************/
#define DRV_SDIO_CMD  P3_6
#define DRV_SDIO_CLK  P3_7
#define DRV_SDIO_D0  P4_0
#define DRV_SDIO_D1  P4_1
#define DRV_SDIO_D2  P4_2
#define DRV_SDIO_D3  P4_3

    Pad_Config(DRV_SDIO_CMD, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE,
               PAD_OUT_HIGH);
    Pad_Config(DRV_SDIO_CLK, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE,
               PAD_OUT_HIGH);
    Pad_Config(DRV_SDIO_D0, PAD_PINMUX_MODE, PAD_IS_PWRON,  PAD_PULL_UP, PAD_OUT_DISABLE,
               PAD_OUT_HIGH);
    Pad_Config(DRV_SDIO_D1, PAD_PINMUX_MODE, PAD_IS_PWRON,  PAD_PULL_UP, PAD_OUT_DISABLE,
               PAD_OUT_HIGH);
    Pad_Config(DRV_SDIO_D2, PAD_PINMUX_MODE, PAD_IS_PWRON,  PAD_PULL_UP, PAD_OUT_DISABLE,
               PAD_OUT_HIGH);
    Pad_Config(DRV_SDIO_D3, PAD_PINMUX_MODE, PAD_IS_PWRON,  PAD_PULL_UP, PAD_OUT_DISABLE,
               PAD_OUT_HIGH);

    Pad_Dedicated_Config(DRV_SDIO_CMD, ENABLE);
    Pad_Dedicated_Config(DRV_SDIO_CLK, ENABLE);
    Pad_Dedicated_Config(DRV_SDIO_D0, ENABLE);
    Pad_Dedicated_Config(DRV_SDIO_D1, ENABLE);
    Pad_Dedicated_Config(DRV_SDIO_D2, ENABLE);
    Pad_Dedicated_Config(DRV_SDIO_D3, ENABLE);

    struct rtk_sdio_des sdio_des;
    sdio_des.clk_get = rtk_sdio_clock_get;
    sdio_des.hw_sdio = (struct rtk_sdio *)SDIO_HOST0_CFG_REG_BASE;
    sdio_des.rxconfig = DMA_RxConfig;
    sdio_des.txconfig = DMA_TxConfig;

    struct rtk_sdio *hw_sdio = sdio_des.hw_sdio;

    Sdio_InitClk(NULL);

    Sdio_ResetAll((SDIO_TypeDef *)hw_sdio);

    hw_sdio->RINTSTS = 0xffffffff;
    hw_sdio->INTMASK = 0;

    Sdio_SetClk((SDIO_TypeDef *)hw_sdio, 400);

    /* cmd0 with send_initialization */
    hw_sdio->CMDARG = 0;
    hw_sdio->CMD = (BIT13 | BIT15 | BIT29 | BIT31);

    /**********for test**************/


    host = sdio_host_create(&sdio_des);
    if (host == RT_NULL)
    {
        LOG_E("host create fail");
        return -1;
    }

    return 0;
}
INIT_DEVICE_EXPORT(rt_hw_sdio_init);
#endif



#endif
