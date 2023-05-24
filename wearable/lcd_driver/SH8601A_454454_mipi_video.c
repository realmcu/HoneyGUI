/**
*********************************************************************************************************
*               Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file
* @brief
* @details
* @author   howie wang
* @date     2022-02-16
* @version  v0.1
*********************************************************************************************************
*/
#include "SH8601A_454454_mipi_video.h"
#include "rtl_lcdc.h"
#include "rtl_lcdc_dsi.h"
#include "rtl_lcdc_edpi.h"
#include "rtl_nvic.h"


#define LCDC_DMA_CHANNEL_NUM              0
#define LCDC_DMA_CHANNEL_INDEX            LCDC_DMA_Channel0


__weak void DSI_IO_WriteCmd(uint32_t NbrParams, uint8_t *pParams)
{
    DSI_TypeDef *DSIx = DSI;
    if (NbrParams == 0)
    {
        DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P0, pParams[0], pParams[1]);
    }
    else if (NbrParams == 1)
    {
        DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, pParams[0], pParams[1]);
    }
    else
    {
        DSI_LongWrite(DSIx,  0, DSI_DCS_LONG_PKT_WRITE, NbrParams, pParams[NbrParams], pParams);
    }
}



uint8_t SH8601A_Init()
{
    LCDC_InitTypeDef lcdc_init = {0};
    lcdc_init.LCDC_Interface = LCDC_IF_eDPI;
    lcdc_init.LCDC_PixelInputFarmat = LCDC_INPUT_ARGB8888;
    //lcdc_init.LCDC_PixelInputFarmat = LCDC_INPUT_RGB565;
    lcdc_init.LCDC_PixelOutpuFarmat = LCDC_OUTPUT_RGB888;
    lcdc_init.LCDC_PixelBitSwap = LCDC_SWAP_BYPASS; //lcdc_handler_cfg->LCDC_TeEn = LCDC_TE_DISABLE;

    lcdc_init.LCDC_InfiniteModeEn = ENABLE;
    lcdc_init.LCDC_DmaThreshold =
        64;    //only support threshold = 8 for DMA MSIZE = 8; the other threshold setting will be support later
    LCDC_Init(&lcdc_init);

    LCDC_MaskINTConfig(LCDC_MASK_TX_AUTO_DONE_INT_MSK, DISABLE);
    LCDC_ClearINTPendingBit(LCDC_TX_AUTO_DONE_INT_CLR);
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = Display_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = DISABLE;
    NVIC_Init(&NVIC_InitStruct);

    LCDC_eDPICfgTypeDef eDPICfg = {0};//836 * 550, 42FPS == 19311600
    eDPICfg.eDPI_ClockDiv1 = 1;
    eDPICfg.eDPI_ClockDiv2 = 8;
    eDPICfg.eDPI_ClockDiv3 = 1;
    uint32_t laneByteClk_kHz = 50000;
    uint32_t LcdClock_kHz  = (80000 / 8);
    eDPICfg.eDPI_HoriSyncWidth = SH8601A_LCD_HSYNC;
    eDPICfg.eDPI_VeriSyncHeight = SH8601A_LCD_VSYNC;
    eDPICfg.eDPI_AccumulatedHBP = SH8601A_LCD_HSYNC + SH8601A_LCD_HBP;
    eDPICfg.eDPI_AccumulatedVBP = SH8601A_LCD_VSYNC + SH8601A_LCD_VBP;
    eDPICfg.eDPI_AccumulatedActiveW = SH8601A_LCD_HSYNC + SH8601A_LCD_HBP + SH8601A_LCD_WIDTH;
    eDPICfg.eDPI_AccumulatedActiveH = SH8601A_LCD_VSYNC + SH8601A_LCD_VBP + SH8601A_LCD_HEIGHT;
    eDPICfg.eDPI_TotalWidth = SH8601A_LCD_WIDTH + SH8601A_LCD_HSYNC + SH8601A_LCD_HBP +
                              SH8601A_LCD_HFP;
    eDPICfg.eDPI_TotalHeight = SH8601A_LCD_HEIGHT + SH8601A_LCD_VSYNC + SH8601A_LCD_VBP +
                               SH8601A_LCD_VFP;
    eDPICfg.eDPI_HoriSyncPolarity = 0;
    eDPICfg.eDPI_VeriSyncPolarity = 0;
    eDPICfg.eDPI_DataEnPolarity = 0;
    eDPICfg.eDPI_LineIntMask = 1;

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


    uint32_t HSA = SH8601A_LCD_HSYNC, HFP = SH8601A_LCD_HFP, HBP = SH8601A_LCD_HBP,
             HACT = SH8601A_LCD_WIDTH;
    uint32_t VSA = SH8601A_LCD_VSYNC, VFP = SH8601A_LCD_VFP, VBP = SH8601A_LCD_VBP,
             VACT = SH8601A_LCD_HEIGHT;

    LCDC_LCD_SET_RST(false);
    rt_thread_mdelay(100);
    LCDC_LCD_SET_RST(true);
    rt_thread_mdelay(50);
    LCDC_LCD_SET_RST(false);
    rt_thread_mdelay(50);//todo

    DSI_DeInit(DSI);
    DSI_InitTypeDef   DSI_InitStruct = {0};
    DSI_InitStruct.VirtualChannelID             = 0;

    DSI_InitStruct.ColorCoding                  = 5;
    DSI_InitStruct.DEPolarity                   = 1;
    DSI_InitStruct.HSPolarity                   = 1;
    DSI_InitStruct.VSPolarity                   = 1;

    DSI_InitStruct.phy_mode = 0x0;  //fix
    DSI_InitStruct.NumberOfLanes = 0x01;
    DSI_InitStruct.StopWaitTime = 0;    //skip
    DSI_InitStruct.InterruptMask0 = 0x1FFFFF;
    DSI_InitStruct.InterruptMask1 = 0x1FFFFF;
    DSI_InitStruct.HS_TX_CNT =
        0xFFFF;                 //for trigger timeout error measured in TO_CLK_DIVISION
    DSI_InitStruct.LP_RX_CNT =
        0xFFFF;                 //for trigger timeout error measured in TO_CLK_DIVISION
    DSI_InitStruct.TOCkdiv = 0x05;
    DSI_InitStruct.TXEscapeCkdiv = 0x05;
    DSI_InitStruct.ModeCfg = DSI_MODE_VIDEO;                   //config for video mode or cmd mode
    DSI_InitStruct.CommandSize                  = 598;
    DSI_InitStruct.PeripherCharacteristics = 0;
    DSI_InitStruct.GenericPacketVCID = 0;
    DSI_InitStruct.GenericTearVCID = 0;
    DSI_InitStruct.GenericReadBackVCID = 0;
    DSI_InitStruct.AutoHS2LPEnable = 0;
    DSI_InitStruct.TxRequestCLKEnable = 1;

    DSI_InitStruct.ClockLaneHS2LPTime = 0x0E;
    DSI_InitStruct.ClockLaneLP2HSTime = 0x27;
    DSI_InitStruct.DataLaneHS2LPTime = 0x10;
    DSI_InitStruct.DataLaneLP2HSTime = 0x37;

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
    DSI_Init(DSI, &DSI_InitStruct);


    DSI_VidCfgTypeDef   VideoCmd_InitStruct = {0};
    VideoCmd_InitStruct.Mode = VIDEO_BURST_MODE_3;
    VideoCmd_InitStruct.LPCommandEnable = ENABLE;
    VideoCmd_InitStruct.FrameBTAAcknowledgeEnable = DISABLE;
    VideoCmd_InitStruct.LPVerticalSyncActiveEnable = ENABLE;
    VideoCmd_InitStruct.LPVerticalBackPorchEnable = ENABLE;
    VideoCmd_InitStruct.LPVerticalFrontPorchEnable = ENABLE;
    VideoCmd_InitStruct.LPVerticalActiveEnable = ENABLE;
    VideoCmd_InitStruct.LPHorizontalBackPorchEnable = ENABLE;
    VideoCmd_InitStruct.LPHorizontalFrontPorchEnable = ENABLE;


    VideoCmd_InitStruct.invact_lpcmd_size = 0;//todo
    VideoCmd_InitStruct.outvact_lpcmd_size = 16;//todo
    VideoCmd_InitStruct.PacketSize = HACT;//todo
    VideoCmd_InitStruct.NumberOfChunks = 0;//todo
    VideoCmd_InitStruct.NullPacketSize = 0xFFF;//todo

    VideoCmd_InitStruct.HorizontalLine = (SH8601A_LCD_WIDTH + SH8601A_LCD_HSYNC + SH8601A_LCD_HBP +
                                          SH8601A_LCD_HFP) * laneByteClk_kHz / LcdClock_kHz;
    VideoCmd_InitStruct.HorizontalSyncActive = SH8601A_LCD_HSYNC * laneByteClk_kHz / LcdClock_kHz;
    VideoCmd_InitStruct.HorizontalBackPorch = SH8601A_LCD_HBP * laneByteClk_kHz / LcdClock_kHz;
    VideoCmd_InitStruct.VerticalSyncActive = SH8601A_LCD_VSYNC;
    VideoCmd_InitStruct.VerticalBackPorch = SH8601A_LCD_VBP;
    VideoCmd_InitStruct.VerticalFrontPorch = SH8601A_LCD_VFP;
    VideoCmd_InitStruct.VerticalActive = SH8601A_LCD_HEIGHT;

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

void sh8601a_video_mode_init(void)
{
    uint8_t pass_word_0[] = {0x5a, 0x5a, 0xC0};
    DSI_IO_WriteCmd(2, (uint8_t *)pass_word_0);
    uint8_t pass_word_1[] = {0x5a, 0x5a, 0xC1};
    DSI_IO_WriteCmd(2, (uint8_t *)pass_word_1);

    uint8_t ShortRegData36[] = {0x11, 0x00};
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData36);
    rt_thread_mdelay(5);
    uint8_t video[] = {0xB1, 0XF0};
    DSI_IO_WriteCmd(1, (uint8_t *)video);

    //DSI_ShortWrite(DSI, 0, DSI_GEN_SHORT_PKT_WRITE_P1, 0xB1, 0xF0);

    uint8_t lcdRegData27[] = {0, 0, 0x01, 0xC5, 0x2A};
    DSI_IO_WriteCmd(4, (uint8_t *)lcdRegData27);

    uint8_t lcdRegData28[] = {0, 0, 0x01, 0xC5, 0x2B};
    DSI_IO_WriteCmd(4, (uint8_t *)lcdRegData28);

    uint8_t ShortRegData41[] = {0x53, 0x20};
    DSI_IO_WriteCmd(1, (uint8_t *)ShortRegData41);

    rt_thread_mdelay(50);
    uint8_t ShortRegData44[] = {0x29, 0x00};
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData44);

//      //for bist mode
//      uint8_t bist[] = {0xBA, 0X81};
//      DSI_IO_WriteCmd(1, (uint8_t *)bist);


}

static uint32_t framebuffer = 0;

void lcd_sh8601a_mipi_video_update_framebuffer(uint8_t *buf, uint32_t len)
{
    framebuffer = (uint32_t)buf;
}

void lcd_sh8601a_mipi_video_init_framebuffer(uint8_t *buf, uint32_t len)
{
#define DMA_LINKLIST 0
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
    LCDC_DMA_InitStruct.LCDC_DMA_SourceAddr          = (uint32_t)buf;
    LCDC_DMA_InitStruct.LCDC_DMA_Multi_Block_En     = 0;
    LCDC_DMA_Init(LCDC_DMA_CHANNEL_INDEX, &LCDC_DMA_InitStruct);

//      Test_SH8601A_LCD_DPI_OUTPUT_RTKDMAC_SETUP();
//      LCDC_DMA_Channel0 ->SAR                     =       (uint32_t)buf;

#else
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
    LCDC_DMA_InitStruct.LCDC_DMA_SourceAddr          = 0;

    LCDC_DMA_InitStruct.LCDC_DMA_Multi_Block_Mode   =
        LLI_TRANSFER;//LLI_TRANSFER or LLI_WITH_CONTIGUOUS_SAR
    LCDC_DMA_InitStruct.LCDC_DMA_Multi_Block_En     = 1;
    LCDC_DMA_InitStruct.LCDC_DMA_Multi_Block_Struct  = LCDC_DMA_LINKLIST_REG_BASE + 0x50;
    LCDC_DMA_Init(LCDC_DMA_CHANNEL_INDEX, &LCDC_DMA_InitStruct);

    LCDC_SET_GROUP1_BLOCKSIZE(SH8601A_LCD_WIDTH * INPUT_PIXEL_BYTES);
    LCDC_SET_GROUP2_BLOCKSIZE(SH8601A_LCD_WIDTH * INPUT_PIXEL_BYTES);

    /*16 pixel aligned for GPU*/
    uint32_t gpu_width = ((SH8601A_LCD_WIDTH + 15) >> 4) << 4;
    /*16 pixel aligned for GPU*/
    LCDC_DMALLI_InitTypeDef LCDC_DMA_LLI_Init = {0};
    LCDC_DMA_LLI_Init.g1_source_addr = (uint32_t)buf;
    LCDC_DMA_LLI_Init.g1_sar_offset = gpu_width * INPUT_PIXEL_BYTES * 2;

    LCDC_DMA_LLI_Init.g2_source_addr = (uint32_t)(buf + gpu_width * INPUT_PIXEL_BYTES);
    LCDC_DMA_LLI_Init.g2_sar_offset = gpu_width * INPUT_PIXEL_BYTES * 2;
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
    LCDC_DMA_MultiBlockCmd(ENABLE);
#endif

    LCDC_DMA_SetSourceAddress(LCDC_DMA_CHANNEL_INDEX, (uint32_t)buf);

    LCDC_DMAChannelCmd(LCDC_DMA_CHANNEL_NUM, ENABLE);
    LCDC_DmaCmd(ENABLE);
    LCDC_AutoWriteCmd(ENABLE);
}

void LCDC_IRQHandler(void)
{
    if (LCDC_GetINTStatus(LCDC_TX_AUTO_DONE_INT_MSK) == SET)
    {
        LCDC_ClearINTPendingBit(LCDC_TX_AUTO_DONE_INT_MSK);
        //rt_kprintf("LCDC_IRQHandler line = %d\n",__LINE__);
        LCDC_ClearDmaFifo();
        LCDC_ClearTxPixelCnt();

        LCDC_DMA_SetSourceAddress(LCDC_DMA_CHANNEL_INDEX, (uint32_t)framebuffer);
        LCDC_DMAChannelCmd(LCDC_DMA_CHANNEL_NUM, ENABLE);
        LCDC_DmaCmd(ENABLE);
        LCDC_AutoWriteCmd(ENABLE);
    }

}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
