/**
*********************************************************************************************************
*               Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file
* @brief
* @details
* @author   howie wang
* @date     2022-05-17
* @version  v0.1
*********************************************************************************************************
*/
#include "st7701s_480480_mipi_video.h"
#include "rtl_lcdc.h"
#include "rtl_lcdc_dsi.h"
#include "rtl_lcdc_edpi.h"
#include "rtl_nvic.h"
#include "utils.h"
#ifndef __RTTHREAD__
#include "trace.h"
#include "os_sched.h"
#endif

#define LCDC_DMA_CHANNEL_NUM              0
#define LCDC_DMA_CHANNEL_INDEX            LCDC_DMA_Channel0






/**
  * @brief  DSI IO write short/long command.
  * @note : Can be surcharged by application code implementation of the function.
  */
__weak void DSI_IO_WriteCmd(uint32_t NbrParams, uint8_t *pParams)
{
    /* NOTE : This function Should not be modified, when it is needed,
              the DSI_IO_WriteCmd could be implemented in the user file
     */
    DSI_TypeDef *DSIx = DSI;
    if (NbrParams <= 1)
    {
        DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, pParams[0], pParams[1]);
        //rt_kprintf("cmd[SHORT] = 0x%x \n", pParams[0]);
    }
    else
    {
        DSI_LongWrite(DSIx,  0, DSI_DCS_LONG_PKT_WRITE, NbrParams, pParams[NbrParams], pParams);
        //rt_kprintf("cmd[LONG] = 0x%x \n", pParams[NbrParams]);
    }
}


void ST7701S_IO_Delay(uint32_t Delay)
{
#ifdef __RTTHREAD__
    rt_thread_mdelay(Delay);
#else
    os_delay(Delay);
#endif
}


/**
  * @brief  Initializes the LCD KoD display part by communication in DSI mode in Video Mode
  *         with IC Display Driver OTM8009A (see IC Driver specification for more information).
  * @param  hdsi_eval : pointer on DSI configuration structure
  * @param  hdsivideo_handle : pointer on DSI video mode configuration structure
  * @retval Status
  */
uint8_t ST7701S_Init(uint32_t ColorCoding, uint32_t orientation)
{

    LCDC_InitTypeDef lcdc_init = {0};
    lcdc_init.LCDC_Interface = LCDC_IF_eDPI;
    lcdc_init.LCDC_PixelInputFarmat = LCDC_INPUT_ARGB8888;
    lcdc_init.LCDC_PixelOutpuFarmat = LCDC_OUTPUT_RGB888;
    lcdc_init.LCDC_PixelBitSwap = LCDC_SWAP_BYPASS; //lcdc_handler_cfg->LCDC_TeEn = LCDC_TE_DISABLE;

    lcdc_init.LCDC_InfiniteModeEn = ENABLE;
    lcdc_init.LCDC_DmaThreshold =
        64;    //only support threshold = 8 for DMA MSIZE = 8; the other threshold setting will be support later
    LCDC_Init(&lcdc_init);

    LCDC_MaskINTConfig(LCDC_MASK_TX_AUTO_DONE_INT_MSK, DISABLE);
    LCDC_ClearINTPendingBit(LCDC_TX_AUTO_DONE_INT_CLR);

    NVIC_InitTypeDef NVIC_InitStruct;
//    NVIC_InitStruct.NVIC_IRQChannel = Display_IRQn;
//    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
//    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
//    extern void NVIC_Init_rom(NVIC_InitTypeDef * NVIC_InitStruct);
//    NVIC_Init_rom(&NVIC_InitStruct);

    NVIC_InitStruct.NVIC_IRQChannel = MIPI_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
    //extern void NVIC_Init_rom(NVIC_InitTypeDef * NVIC_InitStruct);
    //NVIC_Init_rom(&NVIC_InitStruct);

    LCDC_eDPICfgTypeDef eDPICfg;//836 * 550, 42FPS == 19311600
    eDPICfg.eDPI_ClockDiv = 10;
    uint32_t laneByteClk_kHz = 50000;
    uint32_t LcdClock_kHz  = (80000 / eDPICfg.eDPI_ClockDiv);

    eDPICfg.eDPI_HoriSyncWidth = ST7701S_LCD_HSYNC;
    eDPICfg.eDPI_VeriSyncHeight = ST7701S_LCD_VSYNC;
    eDPICfg.eDPI_AccumulatedHBP = ST7701S_LCD_HSYNC + ST7701S_LCD_HBP;
    eDPICfg.eDPI_AccumulatedVBP = ST7701S_LCD_VSYNC + ST7701S_LCD_VBP;
    eDPICfg.eDPI_AccumulatedActiveW = ST7701S_LCD_HSYNC + ST7701S_LCD_HBP + ST7701S_LCD_WIDTH;
    eDPICfg.eDPI_AccumulatedActiveH = ST7701S_LCD_VSYNC + ST7701S_LCD_VBP + ST7701S_LCD_HEIGHT;
    eDPICfg.eDPI_TotalWidth = ST7701S_LCD_WIDTH + ST7701S_LCD_HSYNC + ST7701S_LCD_HBP +
                              ST7701S_LCD_HFP;
    eDPICfg.eDPI_TotalHeight = ST7701S_LCD_HEIGHT + ST7701S_LCD_VSYNC + ST7701S_LCD_VBP +
                               ST7701S_LCD_VFP;
    eDPICfg.eDPI_HoriSyncPolarity = 0;
    eDPICfg.eDPI_VeriSyncPolarity = 0;
    eDPICfg.eDPI_DataEnPolarity = 0;
    eDPICfg.eDPI_LineIntMask = 1;
    //eDPICfg.eDPI_LineIntFlag = 0;
    //eDPICfg.eDPI_LineIntClr = 0;
    eDPICfg.eDPI_ColorMap = 0;//for RGB888
    eDPICfg.eDPI_OperateMode = EDPI_STANDARD_VIDEO_MODE;//for adpated VIDEO mode
    eDPICfg.eDPI_LcdArc = 0;
    eDPICfg.eDPI_ShutdnPolarity = 0;
    eDPICfg.eDPI_ColorModePolarity = 0;
    eDPICfg.eDPI_ShutdnEn = 0;
    eDPICfg.eDPI_ColorModeEn = 0;
    eDPICfg.eDPI_UpdateCfgEn = 0;
    eDPICfg.eDPI_TearReq = 0;
    eDPICfg.eDPI_Halt = 0;
    eDPICfg.eDPI_CmdMaxLatency = 0x39d23;//todo
    eDPICfg.eDPI_LineBufferPixelThreshold = eDPICfg.eDPI_TotalWidth / 2;
    EDPI_Init(&eDPICfg);


    uint32_t HSA = ST7701S_LCD_HSYNC, HFP = ST7701S_LCD_HFP, HBP = ST7701S_LCD_HBP,
             HACT = ST7701S_LCD_WIDTH;
    uint32_t VSA = ST7701S_LCD_VSYNC, VFP = ST7701S_LCD_VFP, VBP = ST7701S_LCD_VBP,
             VACT = ST7701S_LCD_HEIGHT;
#ifdef __RTTHREAD__
    LCDC_LCD_SET_RST(false);
    rt_thread_mdelay(100);
    LCDC_LCD_SET_RST(true);
    rt_thread_mdelay(50);
    LCDC_LCD_SET_RST(false);
    rt_thread_mdelay(50);//todo
#else
    LCDC_LCD_SET_RST(false);
    platform_delay_ms(100);
    LCDC_LCD_SET_RST(true);
    platform_delay_ms(50);
    LCDC_LCD_SET_RST(false);
    platform_delay_ms(50);//todo
#endif
    /***********for clock setting***********/
    //todo
    /***********for clock setting***********/

    /***********for irq setting***********/
    //todo
    /***********for irq setting***********/

    DSI_DeInit(DSI);
    DSI_InitTypeDef   DSI_InitStruct;
    DSI_InitStruct.VirtualChannelID             = 0;

    DSI_InitStruct.ColorCoding                  = 5;
    DSI_InitStruct.DEPolarity                   = 1;
    DSI_InitStruct.HSPolarity                   = 1;
    DSI_InitStruct.VSPolarity                   = 1;

    DSI_InitStruct.ShutDownPolarity                   = 0;
    DSI_InitStruct.ColorModePolarity                   = 0;

    DSI_InitStruct.phy_mode = 0x0;  //fix
    DSI_InitStruct.NumberOfLanes = 2;
    DSI_InitStruct.StopWaitTime = 0;    //skip
    DSI_InitStruct.InterruptMask0 = 0x1FFFFF;
    DSI_InitStruct.InterruptMask1 = 0x1FFFFF;
    DSI_InitStruct.HS_TX_CNT =
        0xFFFF;                 //for trigger timeout error measured in TO_CLK_DIVISION
    DSI_InitStruct.LP_RX_CNT =
        0xFFFF;                 //for trigger timeout error measured in TO_CLK_DIVISION
    DSI_InitStruct.TOCkdiv = 0x45;
    DSI_InitStruct.TXEscapeCkdiv = 0x04;
    DSI_InitStruct.ModeCfg = DSI_MODE_VIDEO;                   //config for video mode or cmd mode
    DSI_InitStruct.CommandSize                  = 598;
    DSI_InitStruct.PeripherCharacteristics = 0;
    DSI_InitStruct.GenericPacketVCID = 0;
    DSI_InitStruct.GenericTearVCID = 0;
    DSI_InitStruct.GenericReadBackVCID = 0;
    DSI_InitStruct.AutoHS2LPEnable = 0;
    DSI_InitStruct.TxRequestCLKEnable = 1;

    DSI_InitStruct.ClockLaneHS2LPTime = 0x24;
    DSI_InitStruct.ClockLaneLP2HSTime = 0x3d;
    DSI_InitStruct.DataLaneHS2LPTime = 0x26;
    DSI_InitStruct.DataLaneLP2HSTime = 0x14;

//    DSI_InitStruct.ClockLaneHS2LPTime = 0x0E;
//    DSI_InitStruct.ClockLaneLP2HSTime = 0x27;
//    DSI_InitStruct.DataLaneHS2LPTime = 0x10;
//    DSI_InitStruct.DataLaneLP2HSTime = 0x37;

    DSI_InitStruct.DataLaneMaxReadTime = 0;
    DSI_InitStruct.phy_shutdownz = 1;
//    DSI_InitStruct.phy_testclr = 0;
//    DSI_InitStruct.phy_testen = 0;
    DSI_InitStruct.phy_enableclk = 1;
//    DSI_InitStruct.phy_forcepll = 0;
    DSI_InitStruct.phy_rstz = 1;
//    DSI_InitStruct.phy_testclk = 0;
//    DSI_InitStruct.phy_testdout = 0;
//    DSI_InitStruct.phy_testin = 0;
//    DSI_InitStruct.phy_auto_ulps = 0;
//    DSI_InitStruct.phy_pll_off_ulps = 0;
//    DSI_InitStruct.phy_pre_pll_off_req = 0;
//    DSI_InitStruct.phy_auto_ulps_delay = 0;
//    DSI_InitStruct.phy_auto_ulps_time = 0;

    DSI_InitStruct.AcknowledgeRequest           = DSI_ACKNOWLEDGE_DISABLE;
    DSI_InitStruct.LPGenReadTime                = 0;
    DSI_InitStruct.HSGenReadTime                = 0;
    DSI_InitStruct.LPGenWriteTime               = 0;
    DSI_InitStruct.HSGenWriteTime               = 0;
    DSI_InitStruct.BTATime                      = 0;

    DSI_InitStruct.LPGenShortWriteNoP           = DSI_LP_GSW0P_ENABLE;
    DSI_InitStruct.LPGenShortWriteOneP          = DSI_LP_GSW1P_ENABLE;
    DSI_InitStruct.LPGenShortWriteTwoP          = DSI_LP_GSW2P_ENABLE;
    DSI_InitStruct.LPGenShortReadNoP            = DSI_LP_GSR0P_ENABLE;
    DSI_InitStruct.LPGenShortReadOneP           = DSI_LP_GSR1P_ENABLE;
    DSI_InitStruct.LPGenShortReadTwoP           = DSI_LP_GSR2P_ENABLE;
    DSI_InitStruct.LPGenLongWrite               = DSI_LP_GLW_ENABLE;
    DSI_InitStruct.LPDcsShortWriteNoP           = DSI_LP_DSW0P_ENABLE;
    DSI_InitStruct.LPDcsShortWriteOneP          = DSI_LP_DSW1P_ENABLE;
    DSI_InitStruct.LPDcsShortReadNoP            = DSI_LP_DSR0P_ENABLE;
    DSI_InitStruct.LPDcsLongWrite               = DSI_LP_DLW_ENABLE;
    DSI_Init(DSI, &DSI_InitStruct);

    DSI_VidCfgTypeDef   VideoCmd_InitStruct = {0};
    VideoCmd_InitStruct.Mode = VIDEO_BURST_MODE_2;
    VideoCmd_InitStruct.LPCommandEnable = ENABLE;
    VideoCmd_InitStruct.FrameBTAAcknowledgeEnable = DISABLE;
    VideoCmd_InitStruct.LPVerticalSyncActiveEnable      = ENABLE;
    VideoCmd_InitStruct.LPVerticalBackPorchEnable       = ENABLE;
    VideoCmd_InitStruct.LPVerticalFrontPorchEnable      = ENABLE;
    VideoCmd_InitStruct.LPVerticalActiveEnable          = ENABLE;
    VideoCmd_InitStruct.LPHorizontalBackPorchEnable     = ENABLE;
    VideoCmd_InitStruct.LPHorizontalFrontPorchEnable    = ENABLE;


    VideoCmd_InitStruct.invact_lpcmd_size = 0x66;//todo
    VideoCmd_InitStruct.outvact_lpcmd_size = 0x65;//todo

//    VideoCmd_InitStruct.NumberOfChunks = 0;//todo
//    VideoCmd_InitStruct.NullPacketSize = 0xFFF;//todo


    VideoCmd_InitStruct.PacketSize = HACT;//todo
    VideoCmd_InitStruct.HorizontalSyncActive = ST7701S_LCD_HSYNC * laneByteClk_kHz / LcdClock_kHz;
    VideoCmd_InitStruct.HorizontalBackPorch = ST7701S_LCD_HBP * laneByteClk_kHz / LcdClock_kHz;
    VideoCmd_InitStruct.HorizontalLine = (ST7701S_LCD_WIDTH + ST7701S_LCD_HSYNC + ST7701S_LCD_HBP +
                                          ST7701S_LCD_HFP) * laneByteClk_kHz / LcdClock_kHz;


    VideoCmd_InitStruct.VerticalSyncActive = ST7701S_LCD_VSYNC;
    VideoCmd_InitStruct.VerticalBackPorch = ST7701S_LCD_VBP;
    VideoCmd_InitStruct.VerticalFrontPorch = ST7701S_LCD_VFP;
    VideoCmd_InitStruct.VerticalActive = ST7701S_LCD_HEIGHT;

    VideoCmd_InitStruct.VPGMode = VIDEO_VPG_MODE_ONLY_V;
    VideoCmd_InitStruct.VPGOrientation = VIDEO_VPG_ORIENTATION_V;
    VideoCmd_InitStruct.VPGEnable = DISABLE;


    DSI_ConfigVideoMode(DSI, &VideoCmd_InitStruct);


    DSI_PhyCfgTypeDef dsi_phy;
    dsi_phy.phy_forepll  = PHY_FORCEPLL_DISABLE;
    dsi_phy.phy_enableclk  = PHY_CLK_ENABLE;
    dsi_phy.phy_rstz   = PHY_RST_ENABLE;
    dsi_phy.phy_shutdownz   = PHY_SHUTDOWN_ENABLE;
    DSI_ConfigPhy(DSI, &dsi_phy);

    DSI_ConfigPhy(DSI, &dsi_phy);


    DSI_Start(DSI);

    return 0;
}


const uint8_t ShortRegData1[]  = {0x11, 0x00};
//rt_thread_mdelay(120);
const uint8_t ShortRegData2[]  = {0xFF, 0x77, 0x01, 0x00, 0x00, 0x10};

const uint8_t ShortRegData3[]  = {0xC0, 0x3B, 0x00};
const uint8_t ShortRegData4[]  = {0xC1, 0x0D, 0x02};
const uint8_t ShortRegData5[]  = {0xC2, 0x37, 0x05, 0x02, 0x00, 0x00};

const uint8_t ShortRegData6[]  = {0xB0, 0x00, 0x11, 0x18, 0x0E, 0x11, 0x06, 0x07, 0x08, 0x07, 0x22, 0x04, 0x12, 0x0F, 0xAA, 0x31, 0x18};
const uint8_t ShortRegData7[]  = {0xB1, 0x00, 0x11, 0x19, 0x0E, 0x12, 0x07, 0x08, 0x08, 0x08, 0x22, 0x04, 0x11, 0x11, 0xA9, 0x32, 0x18};

const uint8_t ShortRegData8[]  = {0xff, 0x77, 0x01, 0x00, 0x00, 0x11};

const uint8_t ShortRegData9[]  = {0xB0, 0x60};

const uint8_t ShortRegData10[]  = {0xB1, 0x26};

const uint8_t ShortRegData11[]  = {0xB2, 0x07};

const uint8_t ShortRegData12[]  = {0xB3, 0x80};

const uint8_t ShortRegData13[]  = {0xB5, 0x49};

const uint8_t ShortRegData14[]  = {0xB7, 0x85};

const uint8_t ShortRegData15[]  = {0xB8, 0x21};

const uint8_t ShortRegData16[]  = {0xc1, 0x78};

const uint8_t ShortRegData17[]  = {0xC2, 0x78};

const uint8_t ShortRegData18[]  = {0xE0, 0x00, 0x1B, 0x02};

const uint8_t ShortRegData19[]  = {0xE1, 0x08, 0xA0, 0x00, 0x00, 0x07, 0xA0, 0x00, 0x00, 0x00, 0x44, 0x44};

const uint8_t ShortRegData20[]  = {0xE2, 0x11, 0x11, 0x44, 0x44, 0xED, 0xA0, 0x00, 0x00, 0xEC, 0xA0, 0x00, 0x00};

const uint8_t ShortRegData21[]  = {0xE3, 0x00, 0x00, 0x11, 0x11};

const uint8_t ShortRegData22[]  = {0xE4, 0x44, 0x44};

const uint8_t ShortRegData23[]  = {0xE5, 0x0A, 0xE9, 0xD8, 0xA0, 0x0C, 0xEB, 0xD8, 0xA0, 0x0E, 0xED, 0xD8, 0xA0, 0x10, 0xEF, 0xD8, 0xA0};


const uint8_t ShortRegData24[]  = {0xE6, 0x00, 0x00, 0x11, 0x11};

const uint8_t ShortRegData25[]  = {0xE7, 0x44, 0x44};

const uint8_t ShortRegData26[]  = {0xE8, 0x09, 0xE8, 0xD8, 0xA0, 0x0B, 0xEA, 0xD8, 0xA0, 0x0D, 0xEC, 0xD8, 0xA0, 0x0F, 0xEE, 0xD8, 0xA0};

const uint8_t ShortRegData27[]  = {0xEB, 0x02, 0x00, 0xE4, 0xE4, 0x88, 0x00, 0x40};

const uint8_t ShortRegData28[]  = {0xEC, 0x3C, 0x00};

const uint8_t ShortRegData29[]  = {0xED, 0xAB, 0x89, 0x76, 0x54, 0x02, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x20, 0x45, 0x67, 0x98, 0xBA};


const uint8_t ShortRegData30[]  = {0xFF, 0x77, 0x01, 0x00, 0x00, 0x13};

const uint8_t ShortRegData31[]  = {0xE5, 0xE4};

const uint8_t ShortRegData32[]  = {0xFF, 0x77, 0x01, 0x00, 0x00, 0x00};

const uint8_t ShortRegData33[]  = {0x36, 0x00};


const uint8_t ShortRegData34[]  = {0x3A, 0x70};

const uint8_t ShortRegData35[]  = {0x29, 0x00};

const uint8_t ShortRegData36[]  = {0x28, 0x00};

const uint8_t ShortRegData37[]  = {0x10, 0x00};

static void DSI_IO_PixelWriteCmd(uint32_t NbrParams, uint8_t *pParams)
{
    DSI_TypeDef *DSIx = DSI;
    NbrParams = NbrParams - 1;
    if (NbrParams <= 1)
    {
        DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, pParams[0], pParams[1]);
        //rt_kprintf("cmd[SHORT] = 0x%x \n", pParams[0]);
    }
    else
    {
        DSI_LongWrite(DSIx,  0, DSI_DCS_LONG_PKT_WRITE, NbrParams, pParams[0], pParams + 1);
        //rt_kprintf("cmd[LONG] = 0x%x \n", pParams[NbrParams]);
    }
}

void st7701s_video_mode_power_on()
{
    DSI_IO_PixelWriteCmd(sizeof(ShortRegData1), (uint8_t *)ShortRegData1);
    DSI_IO_PixelWriteCmd(sizeof(ShortRegData35), (uint8_t *)ShortRegData35);
}

void st7701s_video_mode_power_off()
{
    DSI_IO_PixelWriteCmd(sizeof(ShortRegData36), (uint8_t *)ShortRegData36);
    DSI_IO_PixelWriteCmd(sizeof(ShortRegData37), (uint8_t *)ShortRegData37);
}

void st7701s_video_mode_init(uint32_t ColorCoding, uint32_t orientation)
{

    DSI_IO_PixelWriteCmd(sizeof(ShortRegData1), (uint8_t *)ShortRegData1);
    DSI_IO_PixelWriteCmd(sizeof(ShortRegData2), (uint8_t *)ShortRegData2);
    DSI_IO_PixelWriteCmd(sizeof(ShortRegData3), (uint8_t *)ShortRegData3);
    DSI_IO_PixelWriteCmd(sizeof(ShortRegData4), (uint8_t *)ShortRegData4);
    DSI_IO_PixelWriteCmd(sizeof(ShortRegData5), (uint8_t *)ShortRegData5);
    DSI_IO_PixelWriteCmd(sizeof(ShortRegData6), (uint8_t *)ShortRegData6);
    DSI_IO_PixelWriteCmd(sizeof(ShortRegData7), (uint8_t *)ShortRegData7);
    DSI_IO_PixelWriteCmd(sizeof(ShortRegData8), (uint8_t *)ShortRegData8);
    DSI_IO_PixelWriteCmd(sizeof(ShortRegData9), (uint8_t *)ShortRegData9);
    DSI_IO_PixelWriteCmd(sizeof(ShortRegData10), (uint8_t *)ShortRegData10);

    DSI_IO_PixelWriteCmd(sizeof(ShortRegData11), (uint8_t *)ShortRegData11);
    DSI_IO_PixelWriteCmd(sizeof(ShortRegData12), (uint8_t *)ShortRegData12);
    DSI_IO_PixelWriteCmd(sizeof(ShortRegData13), (uint8_t *)ShortRegData13);
    DSI_IO_PixelWriteCmd(sizeof(ShortRegData14), (uint8_t *)ShortRegData14);
    DSI_IO_PixelWriteCmd(sizeof(ShortRegData15), (uint8_t *)ShortRegData15);
    DSI_IO_PixelWriteCmd(sizeof(ShortRegData16), (uint8_t *)ShortRegData16);
    DSI_IO_PixelWriteCmd(sizeof(ShortRegData17), (uint8_t *)ShortRegData17);
    DSI_IO_PixelWriteCmd(sizeof(ShortRegData18), (uint8_t *)ShortRegData18);
    DSI_IO_PixelWriteCmd(sizeof(ShortRegData19), (uint8_t *)ShortRegData19);
    DSI_IO_PixelWriteCmd(sizeof(ShortRegData20), (uint8_t *)ShortRegData20);

    DSI_IO_PixelWriteCmd(sizeof(ShortRegData21), (uint8_t *)ShortRegData21);
    DSI_IO_PixelWriteCmd(sizeof(ShortRegData22), (uint8_t *)ShortRegData22);
    DSI_IO_PixelWriteCmd(sizeof(ShortRegData23), (uint8_t *)ShortRegData23);
    DSI_IO_PixelWriteCmd(sizeof(ShortRegData24), (uint8_t *)ShortRegData24);
    DSI_IO_PixelWriteCmd(sizeof(ShortRegData25), (uint8_t *)ShortRegData25);
    DSI_IO_PixelWriteCmd(sizeof(ShortRegData26), (uint8_t *)ShortRegData26);
    DSI_IO_PixelWriteCmd(sizeof(ShortRegData27), (uint8_t *)ShortRegData27);
    DSI_IO_PixelWriteCmd(sizeof(ShortRegData28), (uint8_t *)ShortRegData28);
    DSI_IO_PixelWriteCmd(sizeof(ShortRegData29), (uint8_t *)ShortRegData29);
    DSI_IO_PixelWriteCmd(sizeof(ShortRegData30), (uint8_t *)ShortRegData30);

    DSI_IO_PixelWriteCmd(sizeof(ShortRegData31), (uint8_t *)ShortRegData31);
    DSI_IO_PixelWriteCmd(sizeof(ShortRegData32), (uint8_t *)ShortRegData32);
    DSI_IO_PixelWriteCmd(sizeof(ShortRegData33), (uint8_t *)ShortRegData33);
    DSI_IO_PixelWriteCmd(sizeof(ShortRegData34), (uint8_t *)ShortRegData34);
    DSI_IO_PixelWriteCmd(sizeof(ShortRegData35), (uint8_t *)ShortRegData35);
#ifdef __RTTHREAD__
    rt_thread_mdelay(25);
#else
    platform_delay_ms(25);
#endif
}


void lcd_st7701s_mipi_video_update_framebuffer(uint8_t *buf, uint32_t len)
{
    //updata todo
    LCDC_SET_INFINITE_ADDR((uint32_t)buf, (uint32_t)(buf + ST7701S_LCD_WIDTH * INPUT_PIXEL_BYTES));
}
void lcd_st7701s_mipi_video_init_framebuffer(uint8_t *buf, uint32_t len)
{
#define DMA_LINKLIST 1
#if (DMA_LINKLIST == 0)
    LCDC_DMA_InitTypeDef LCDC_DMA_InitStruct = {0};
    LCDC_DMA_StructInit(&LCDC_DMA_InitStruct);
    LCDC_DMA_InitStruct.LCDC_DMA_ChannelNum          = LCDC_DMA_CHANNEL_NUM;
    LCDC_DMA_InitStruct.LCDC_DMA_DIR                 = DMA_DIR_PeripheralToMemory;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceInc           = DMA_SourceInc_Inc;
    LCDC_DMA_InitStruct.LCDC_DMA_DestinationInc      = DMA_DestinationInc_Fix;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceDataSize      = DMA_DataSize_Word;
    LCDC_DMA_InitStruct.LCDC_DMA_DestinationDataSize = DMA_DataSize_Word;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceMsize         = DMA_Msize_64;
    LCDC_DMA_InitStruct.LCDC_DMA_DestinationMsize    = DMA_Msize_64;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceAddr          = 0x00500000;
    LCDC_DMA_InitStruct.LCDC_DMA_Multi_Block_En      = 0;
    LCDC_DMA_Init(LCDC_DMA_CHANNEL_INDEX, &LCDC_DMA_InitStruct);
#else
    LCDC_DMA_InitTypeDef LCDC_DMA_InitStruct = {0};
    LCDC_DMA_StructInit(&LCDC_DMA_InitStruct);
    LCDC_DMA_InitStruct.LCDC_DMA_ChannelNum          = LCDC_DMA_CHANNEL_NUM;
    LCDC_DMA_InitStruct.LCDC_DMA_DIR                 = DMA_DIR_PeripheralToMemory;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceInc           = DMA_SourceInc_Inc;
    LCDC_DMA_InitStruct.LCDC_DMA_DestinationInc      = DMA_DestinationInc_Fix;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceDataSize      = DMA_DataSize_Word;
    LCDC_DMA_InitStruct.LCDC_DMA_DestinationDataSize = DMA_DataSize_Word;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceMsize         = DMA_Msize_32;
    LCDC_DMA_InitStruct.LCDC_DMA_DestinationMsize    = DMA_Msize_32;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceAddr          = 0x00500000;
    LCDC_DMA_InitStruct.LCDC_DMA_Multi_Block_En      = 1;

    LCDC_DMA_InitStruct.LCDC_DMA_Multi_Block_Mode   =
        LLI_TRANSFER;//LLI_TRANSFER or LLI_WITH_CONTIGUOUS_SAR
    LCDC_DMA_InitStruct.LCDC_DMA_Multi_Block_Struct  = LCDC_DMA_LINKLIST_REG_BASE + 0x50;
    LCDC_DMA_Init(LCDC_DMA_CHANNEL_INDEX, &LCDC_DMA_InitStruct);





    LCDC_DMALLI_InitTypeDef LCDC_DMA_LLI_Init = {0};
    LCDC_SET_GROUP1_BLOCKSIZE(ST7701S_LCD_WIDTH * INPUT_PIXEL_BYTES);
    LCDC_DMA_LLI_Init.g1_source_addr = (uint32_t)buf;
    LCDC_DMA_LLI_Init.g1_sar_offset = ST7701S_LCD_WIDTH * INPUT_PIXEL_BYTES * 2;

    LCDC_SET_GROUP2_BLOCKSIZE(ST7701S_LCD_WIDTH * INPUT_PIXEL_BYTES);
    LCDC_DMA_LLI_Init.g2_source_addr = (uint32_t)(buf + ST7701S_LCD_WIDTH * INPUT_PIXEL_BYTES);
    LCDC_DMA_LLI_Init.g2_sar_offset = ST7701S_LCD_WIDTH * INPUT_PIXEL_BYTES * 2;
    LCDC_DMA_LinkList_Init(&LCDC_DMA_LLI_Init,
                           &LCDC_DMA_InitStruct);//LLI_TRANSFER or LLI_WITH_CONTIGUOUS_SAR
#endif

    LCDC_ClearDmaFifo();
    LCDC_ClearTxPixelCnt();

    LCDC_SwitchMode(LCDC_AUTO_MODE);
    LCDC_SwitchDirect(LCDC_TX_MODE);


    LCDC_SetTxPixelLen(len);

    LCDC_Cmd(ENABLE);

#if DMA_LINKLIST
    LCDC_SET_INFINITE_ADDR(LCDC_DMA_LLI_Init.g1_source_addr, LCDC_DMA_LLI_Init.g2_source_addr);
    LCDC_DMA_MultiBlockCmd(ENABLE);
#else
    LCDC_DMA_SetSourceAddress(LCDC_DMA_CHANNEL_INDEX, (uint32_t)buf);
#endif


    LCDC_DMAChannelCmd(LCDC_DMA_CHANNEL_NUM, ENABLE);
    LCDC_DmaCmd(ENABLE);
    LCDC_AutoWriteCmd(ENABLE);

}

void MIPI_Handler(void)
{
#ifdef __RTTHREAD__
    rt_kprintf("MIPI_Handler INT_ST0 = 0x%x\n", DSI->INT_ST0);
    rt_kprintf("MIPI_Handler INT_ST1 = 0x%x\n", DSI->INT_ST1);
    rt_kprintf("MIPI_Handler INT_MSK0 = 0x%x\n", DSI->INT_MSK0);
    rt_kprintf("MIPI_Handler INT_MSK1 = 0x%x\n", DSI->INT_MSK1);
    rt_kprintf("MIPI_Handler INT_FORCE0 = 0x%x\n", DSI->INT_FORCE0);
    rt_kprintf("MIPI_Handler INT_FORCE1 = 0x%x\n", DSI->INT_FORCE1);
#else
    DBG_DIRECT("MIPI_Handler INT_ST1 = 0x%x\n", DSI->INT_ST1);
#endif
    //while(1);
}

void LCDC_IRQHandler(void)
{
    if (LCDC_GetINTStatus(LCDC_TX_AUTO_DONE_INT_MSK) == SET)
    {
//        LCDC_ClearINTPendingBit(LCDC_TX_AUTO_DONE_INT_MSK);
#ifdef __RTTHREAD__
        rt_kprintf("LCDC_IRQHandler line = %d\n", __LINE__);
#else
        DBG_DIRECT("LCDC_IRQHandler line = %d\n", __LINE__);
#endif
//        LCDC_ClearDmaFifo();
//        LCDC_ClearTxPixelCnt();

//        LCDC_DMA_SetSourceAddress(LCDC_DMA_CHANNEL_INDEX, (uint32_t)framebuffer);

//        LCDC_DMAChannelCmd(LCDC_DMA_CHANNEL_NUM, ENABLE);
//        LCDC_DmaCmd(ENABLE);
//        LCDC_AutoWriteCmd(ENABLE);
    }

}


