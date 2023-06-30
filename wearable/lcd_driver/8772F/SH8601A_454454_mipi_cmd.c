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
#include "SH8601A_454454_mipi_cmd.h"
#include "rtl_lcdc.h"
#include "rtl_pinmux.h"
#include "rtl_rcc.h"
#include "rtl_lcdc_dsi.h"
#include "rtl_lcdc_dphy.h"
#include "rtl_lcdc_edpi.h"
#include "os_sched.h"
#include "utils.h"
#include "drv_gpio.h"


#define LCDC_DMA_CHANNEL_NUM              0
#define LCDC_DMA_CHANNEL_INDEX            LCDC_DMA_Channel0


__weak void DSI_IO_WriteCmd(uint32_t NbrParams, uint8_t *pParams)
{
    DSI_TypeDef *DSIx = DSI;
    if (NbrParams == 0)
    {
        DSI_ShortWrite(DSIx, 0, DSI_DCS_SHORT_PKT_WRITE_P1, pParams[0], pParams[1]);
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

uint32_t rtk_lcd_hal_get_width(void)
{
    return SH8601A_LCD_WIDTH;
}
uint32_t rtk_lcd_hal_get_height(void)
{
    return SH8601A_LCD_HEIGHT;
}

uint32_t rtk_lcd_hal_get_pixel_bits(void)
{
    return SH8601A_LCD_BITS_PIXEL;
}

void rtk_lcd_hal_rect_fill(uint16_t xStart, uint16_t yStart, uint16_t w, uint16_t h, uint32_t color)
{

    GDMA_InitTypeDef LCDC_DMA_InitStruct = {0};
    LCDC_DMA_StructInit(&LCDC_DMA_InitStruct);
    LCDC_DMA_InitStruct.GDMA_ChannelNum          = LCDC_DMA_CHANNEL_NUM;
    LCDC_DMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Fix;
    LCDC_DMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Fix;
    LCDC_DMA_InitStruct.GDMA_SourceDataSize      = GDMA_DataSize_Word;
    LCDC_DMA_InitStruct.GDMA_DestinationDataSize = GDMA_DataSize_Word;
    LCDC_DMA_InitStruct.GDMA_SourceMsize         = GDMA_Msize_8;
    LCDC_DMA_InitStruct.GDMA_DestinationMsize    = GDMA_Msize_8;
    LCDC_DMA_InitStruct.GDMA_SourceAddr          = (uint32_t)(&color);
    LCDC_DMA_InitStruct.GDMA_Multi_Block_En     = 0;
    LCDC_DMA_Init(LCDC_DMA_CHANNEL_INDEX, &LCDC_DMA_InitStruct);

    LCDC_ClearDmaFifo();
    LCDC_ClearTxPixelCnt();

    LCDC_SwitchMode(LCDC_AUTO_MODE);
    LCDC_SwitchDirect(LCDC_TX_MODE);

    LCDC_SetTxPixelLen(w * h);

    LCDC_Cmd(ENABLE);


    LCDC_DMA_SetSourceAddress(LCDC_DMA_CHANNEL_INDEX, (uint32_t)(&color));

    LCDC_DMAChannelCmd(LCDC_DMA_CHANNEL_NUM, ENABLE);
    LCDC_DmaCmd(ENABLE);

    TEAR_CTR_t handler_reg_0x10 = {.d32 = LCDC_HANDLER->TEAR_CTR};
    handler_reg_0x10.b.bypass_t2w_delay = 0;
    handler_reg_0x10.b.t2w_delay = 0xfff;
    LCDC_HANDLER->TEAR_CTR = handler_reg_0x10.d32;
    LCDC_TeCmd(ENABLE);

    uint32_t time_out_ms = 100;
    while (((LCDC_HANDLER->DMA_FIFO_CTRL & LCDC_DMA_ENABLE) != RESET) && (time_out_ms != 0))
    {
        platform_delay_ms(1);
        time_out_ms--;
    }
    if (time_out_ms != 0)
    {
        while (((LCDC_HANDLER->DMA_FIFO_OFFSET & LCDC_DMA_TX_FIFO_OFFSET) != RESET) &&
               (LCDC_HANDLER->TX_CNT == LCDC_HANDLER->TX_LEN));

        LCDC_TeCmd(DISABLE);                            // disable Tear trigger auto_write_start
    }
    else
    {
        LCDC_AutoWriteCmd(ENABLE);
        while ((LCDC_HANDLER->DMA_FIFO_CTRL & LCDC_DMA_ENABLE) != RESET);
        while (((LCDC_HANDLER->DMA_FIFO_OFFSET & LCDC_DMA_TX_FIFO_OFFSET) != RESET) &&
               (LCDC_HANDLER->TX_CNT == LCDC_HANDLER->TX_LEN));
    }

    LCDC_ClearDmaFifo();
    LCDC_ClearTxPixelCnt();

    LCDC_Cmd(DISABLE);

}

static void driver_ic_init(void)
{
    Pad_Config(P17_7, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Dedicated_Config(P17_7, ENABLE);
    LCDC_LCD_SET_RST(false);
    platform_delay_ms(50);
    LCDC_LCD_SET_RST(true);
    platform_delay_ms(200);
    LCDC_LCD_SET_RST(false);
    platform_delay_ms(50);//todo
    Pad_Config(P17_7, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Dedicated_Config(P17_7, DISABLE);

    uint8_t pass_word_0[] = {0x5a, 0x5a, 0xC0};
    DSI_IO_WriteCmd(2, (uint8_t *)pass_word_0);
    uint8_t pass_word_1[] = {0x5a, 0x5a, 0xC1};
    DSI_IO_WriteCmd(2, (uint8_t *)pass_word_1);
    platform_delay_ms(5);
    uint8_t ShortRegData36[] = {0x11, 0x00};
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData36);
    platform_delay_ms(5);

    /*set tear*/
    uint8_t lcdRegData_tear_on[] = {0x35, 0x00};
    DSI_IO_WriteCmd(0, (uint8_t *)lcdRegData_tear_on);
    platform_delay_ms(5);
    uint8_t lcdRegData_tear_scan_line[] = {0x00, 0x00, 0x44};
    DSI_IO_WriteCmd(2, (uint8_t *)lcdRegData_tear_scan_line);
    platform_delay_ms(5);

    // frame rate switch://
    uint8_t paramter_select[] = {0xB0, 0x16}; // Parameter selection, 22th parameter
    DSI_IO_WriteCmd(1, (uint8_t *)paramter_select);
    platform_delay_ms(5);
    uint8_t frame_rate[] = {0xB1, 0x00}; //0x01=45Hz, 0x00=60Hz
    DSI_IO_WriteCmd(1, (uint8_t *)frame_rate);
    platform_delay_ms(5);

    uint8_t lcdRegData27[] = {0, 0, 0x01, 0xC5, 0x2A};
    DSI_IO_WriteCmd(4, (uint8_t *)lcdRegData27);
    platform_delay_ms(5);

    uint8_t lcdRegData28[] = {0, 0, 0x01, 0xC5, 0x2B};
    DSI_IO_WriteCmd(4, (uint8_t *)lcdRegData28);
    platform_delay_ms(5);

    uint8_t ShortRegData41[] = {0x53, 0x20};//Write CTRL Display
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData41);


    platform_delay_ms(60);
    uint8_t ShortRegData44[] = {0x29, 0x00};
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData44);

}

void rtk_lcd_hal_init(void)
{

    RCC_PeriphClockCmd(APBPeriph_DISP, APBPeriph_DISP_CLOCK_CLOCK, ENABLE);
    RCC_PeriphClockCmd(APBPeriph_MIPI_HOST, APBPeriph_MIPI_HOST_CLOCK, ENABLE);

    Pad_Config(P17_2, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Dedicated_Config(P17_2, ENABLE);
    drv_pin_mode(P13_7, PIN_MODE_OUTPUT);
    drv_pin_write(P13_7, 0); //im0
    drv_pin_mode(P13_6, PIN_MODE_OUTPUT);
    drv_pin_write(P13_6, 1); //im1

    LCDC_InitTypeDef lcdc_init = {0};
    lcdc_init.LCDC_Interface = LCDC_IF_eDPI;
    lcdc_init.LCDC_PixelInputFarmat = LCDC_INPUT_ARGB8888;
    lcdc_init.LCDC_PixelOutpuFarmat = LCDC_OUTPUT_RGB888;
    lcdc_init.LCDC_PixelBitSwap = LCDC_SWAP_BYPASS; //lcdc_handler_cfg->LCDC_TeEn = DISABLE;

    lcdc_init.LCDC_TeEn = DISABLE;
    lcdc_init.LCDC_TePolarity = LCDC_TE_EDGE_RISING;
    lcdc_init.LCDC_TeInputMux = LCDC_TE_LCD_INPUT;

    lcdc_init.LCDC_DmaThreshold =
        8;    //only support threshold = 8 for DMA MSIZE = 8; the other threshold setting will be support later
    LCDC_Init(&lcdc_init);

    LCDC_eDPICfgTypeDef eDPICfg;//836 * 550, 42FPS == 19311600
    eDPICfg.eDPI_ClockDiv = 4;
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
    eDPICfg.eDPI_HoriSyncPolarity = 1;
    eDPICfg.eDPI_VeriSyncPolarity = 1;
    eDPICfg.eDPI_DataEnPolarity = 1;
    eDPICfg.eDPI_LineIntMask = 1;
    //eDPICfg.eDPI_LineIntFlag = 0;
    //eDPICfg.eDPI_LineIntClr = 0;
    eDPICfg.eDPI_ColorMap = 0;//for RGB888
    eDPICfg.eDPI_OperateMode = EDPI_ADAPTED_COMMAND_MODE;//for adpated cmd mode
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


#if 0

    DSI_DeInit(DSI);
    DSI_InitTypeDef   DSI_InitStruct = {0};
    DSI_InitStruct.VirtualChannelID             = 0;

    DSI_InitStruct.ColorCoding                  = 0;
    DSI_InitStruct.DEPolarity                   = 0;
    DSI_InitStruct.HSPolarity                   = 0;
    DSI_InitStruct.VSPolarity                   = 0;

    DSI_InitStruct.phy_mode = 0x0;  //fix
    DSI_InitStruct.NumberOfLanes = 0x01;
    DSI_InitStruct.StopWaitTime = 0;    //skip
    DSI_InitStruct.InterruptMask0 = 0x1FFFFF;
    DSI_InitStruct.InterruptMask1 = 0x1FFFFF;
    DSI_InitStruct.HS_TX_CNT =
        0xFFFF;                 //for trigger timeout error measured in TO_CLK_DIVISION
    DSI_InitStruct.LP_RX_CNT =
        0xFFFF;                 //for trigger timeout error measured in TO_CLK_DIVISION
    DSI_InitStruct.TOCkdiv = 0x4;
    DSI_InitStruct.TXEscapeCkdiv = 0x03;
    DSI_InitStruct.ModeCfg = 1;                   //config for video mode or cmd mode
    DSI_InitStruct.CommandSize                  = 598;
    DSI_InitStruct.PeripherCharacteristics = 0;
    DSI_InitStruct.GenericPacketVCID = 0;
    DSI_InitStruct.GenericTearVCID = 0;
    DSI_InitStruct.GenericReadBackVCID = 0;
    DSI_InitStruct.AutoHS2LPEnable = 1;
    DSI_InitStruct.TxRequestCLKEnable = 1;
    DSI_InitStruct.ClockLaneHS2LPTime = 0x24;
    DSI_InitStruct.ClockLaneLP2HSTime = 0x3d;
    DSI_InitStruct.DataLaneHS2LPTime = 0x26;
    DSI_InitStruct.DataLaneLP2HSTime = 0xA1;
    DSI_InitStruct.DataLaneMaxReadTime = 0;
    DSI_InitStruct.phy_shutdownz = 1;
//    DSI_InitStruct.phy_testclr = 0;
//    DSI_InitStruct.phy_testen = 0;
    DSI_InitStruct.phy_enableclk = 1;
//    DSI_InitStruct.phy_forcepll = 0;
    DSI_InitStruct.phy_rstz = 1;
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
    DSI_InitStruct.LPDcsLongWrite               = DSI_LP_DLW_DISABLE;
    DSI_Init(DSI, &DSI_InitStruct);

    DSI_AdaptedCmdCfgTypeDef   AdaptedCmd_InitStruct = {0};
    AdaptedCmd_InitStruct.TEAcknowledgeRequest         = DSI_TE_ACKNOWLEDGE_DISABLE;
    AdaptedCmd_InitStruct.hw_tear_effect_gen       = 0;
    AdaptedCmd_InitStruct.scan_line_parameter   = 0;
    AdaptedCmd_InitStruct.hw_set_scan_line    = 0;
    AdaptedCmd_InitStruct.hw_tear_effect_on   = 0;

    DSI_ConfigAdaptedCommandMode(DSI, &AdaptedCmd_InitStruct);


    DSI_PhyCfgTypeDef dsi_phy = {0};
    dsi_phy.phy_forepll  = PHY_FORCEPLL_DISABLE;
    dsi_phy.phy_enableclk  = PHY_CLK_ENABLE;
    dsi_phy.phy_rstz   = PHY_RST_ENABLE;
    dsi_phy.phy_shutdownz   = PHY_SHUTDOWN_ENABLE;
    DSI_ConfigPhy(DSI, &dsi_phy);

    DSI_ConfigPhy(DSI, &dsi_phy);

    DSI_Start(DSI);

//    uint8_t pass_word_0[] = {0x5a, 0x5a, 0xC0};
//    DSI_IO_WriteCmd(2, (uint8_t *)pass_word_0);
//    uint8_t pass_word_1[] = {0x5a, 0x5a, 0xC1};
//    DSI_IO_WriteCmd(2, (uint8_t *)pass_word_1);
//    uint8_t video[] = {0xB1, 0X00};
//    DSI_IO_WriteCmd(0, (uint8_t *)video);



    //for bist mode
//    uint8_t bist[] = {0xBA, 0X81};
//    DSI_IO_WriteCmd(0, (uint8_t *)bist);
#else
    DSI->PWR_UP                             = (DW_DSI_RESET <<
                                               DSI_SHUTDOWNZ_POS);                        //reset DW DSI

    DSI->PHY_MODE                           = (DW_DSI_DPHY <<
                                               DSI_PHY_MODE_POS);                          // D-PHY

    DSI->PHY_IF_CFG                     = (0      << DSI_PHY_STOP_WAIT_TIME_POS
                                           |   DW_DSI_LANE0    << DSI_N_LANES_POS                          // 1 LANE
                                          );

    DSI->INT_MSK0                           = (
                                                  DW_DSI_MASK << DSI_MASK_DPHY_ERRORS_4_POS
                                                  |   DW_DSI_MASK << DSI_MASK_DPHY_ERRORS_3_POS
                                                  |   DW_DSI_MASK << DSI_MASK_DPHY_ERRORS_2_POS
                                                  |   DW_DSI_MASK << DSI_MASK_DPHY_ERRORS_1_POS
                                                  |   DW_DSI_MASK << DSI_MASK_DPHY_ERRORS_0_POS
                                                  | DW_DSI_MASK << DSI_MASK_ACK_WITH_ERR_15_POS
                                                  | DW_DSI_MASK << DSI_MASK_ACK_WITH_ERR_14_POS
                                                  | DW_DSI_MASK << DSI_MASK_ACK_WITH_ERR_13_POS
                                                  | DW_DSI_MASK << DSI_MASK_ACK_WITH_ERR_12_POS
                                                  | DW_DSI_MASK << DSI_MASK_ACK_WITH_ERR_11_POS
                                                  | DW_DSI_MASK << DSI_MASK_ACK_WITH_ERR_10_POS
                                                  | DW_DSI_MASK << DSI_MASK_ACK_WITH_ERR_9_POS
                                                  | DW_DSI_MASK << DSI_MASK_ACK_WITH_ERR_8_POS
                                                  | DW_DSI_MASK << DSI_MASK_ACK_WITH_ERR_7_POS
                                                  | DW_DSI_MASK << DSI_MASK_ACK_WITH_ERR_6_POS
                                                  | DW_DSI_MASK << DSI_MASK_ACK_WITH_ERR_5_POS
                                                  | DW_DSI_MASK << DSI_MASK_ACK_WITH_ERR_4_POS
                                                  | DW_DSI_MASK << DSI_MASK_ACK_WITH_ERR_3_POS
                                                  | DW_DSI_MASK << DSI_MASK_ACK_WITH_ERR_2_POS
                                                  | DW_DSI_MASK << DSI_MASK_ACK_WITH_ERR_1_POS
                                                  | DW_DSI_MASK << DSI_MASK_ACK_WITH_ERR_0_POS
                                              );


    DSI->INT_MSK1                           = (
                                                  DW_DSI_MASK << DSI_MASK_TEAR_REQUEST_ERR_POS
                                                  | DW_DSI_MASK << DSI_MASK_DPI_BUFF_PLD_UNDER_POS
                                                  | DW_DSI_MASK << DSI_MASK_DBI_ILEGAL_COMM_ERR_POS
                                                  | DW_DSI_MASK << DSI_MASK_DBI_PLD_RECV_ERR_POS
                                                  | DW_DSI_MASK << DSI_MASK_DBI_PLD_RD_ERR_POS
                                                  | DW_DSI_MASK << DSI_MASK_DBI_PLD_WR_ERR_POS
                                                  | DW_DSI_MASK << DSI_MASK_DBI_CMD_WR_ERR_POS
                                                  | DW_DSI_MASK << DSI_MASK_GEN_PLD_RECEV_ERR_POS
                                                  | DW_DSI_MASK << DSI_MASK_GEN_PLD_RD_ERR_POS
                                                  | DW_DSI_MASK << DSI_MASK_GEN_PLD_SEND_ERR_POS
                                                  | DW_DSI_MASK << DSI_MASK_GEN_PLD_WR_ERR_POS
                                                  | DW_DSI_MASK << DSI_MASK_GEN_CMD_WR_ERR_POS
                                                  | DW_DSI_MASK << DSI_MASK_DPI_PLD_WR_ERR_POS
                                                  | DW_DSI_MASK << DSI_MASK_EOPT_ERR_POS
                                                  | DW_DSI_MASK << DSI_MASK_PKT_SIZE_ERR_POS
                                                  | DW_DSI_MASK << DSI_MASK_CRC_ERR_POS
                                                  | DW_DSI_MASK << DSI_MASK_ECC_MILTI_ERR_POS
                                                  | DW_DSI_MASK << DSI_MASK_ECC_SINGLE_ERR_POS
                                                  | DW_DSI_MASK << DSI_MASK_TO_LP_RX_POS
                                                  | DW_DSI_MASK << DSI_MASK_TO_HS_TX_POS
                                              );


    DSI->TO_CNT_CFG             = (
                                      0XFFFF << DSI_HSTX_TO_CNT_POS
                                      |   0XFFFF << DSI_LPRX_TO_CNT_POS
                                  );

    DSI->CLKMGR_CFG             = (
                                      0X14                                            <<  DSI_TO_CLK_DIVISION_POS
                                      |   TEST_LANEBYTECLK_DIV            <<
                                      DSI_TX_ESC_CLK_DIVISION_POS                 // tx_clk_esc   13.3MHz-20MHz
                                  );

    DSI->MODE_CFG                   = (
                                          1 << DSI_CMD_VIDEO_MODE_POS                                         // COMMAND MODE
                                      );

    DSI->DPI_VCID                   = (
                                          0   <<  DSI_DPI_VCID_POS                                                    // VIRTUAL CHANNEL ID 0
                                      );

    DSI->DPI_COLOR_CODING   = (
                                  5 << DSI_DPI_COLOR_CODING_POS                                       //RGB888_BYTE_NUM COLOR CODING
                              );

    DSI->DPI_CFG_POL            = (
                                      1 << DSI_COLORM_ACTIVE_LOW_POS
                                      |   1 << DSI_SHUTD_ACTIVE_LOW_POS
                                      | 0 << DSI_HSYNC_ACTIVE_LOW_POS
                                      | 0 << DSI_VSYNC_ACTIVE_LOW_POS
                                      | 0 << DSI_DATAEN_ACTIVE_LOW_POS
                                  );

    DSI->EDPI_CMD_SIZE      = (
                                  0x256 <<
                                  DSI_EDPI_ALLOWED_CMD_SIZE_POS                       //  pixel for one packet,don't set it too small
                              );

    DSI->CMD_MODE_CFG           = (
                                      TRANS_LP << DSI_MAX_RD_PKT_SIZE_POS
                                      | TRANS_LP << DSI_DCS_LW_TX_POS
                                      | TRANS_LP << DSI_DCS_SR_0P_TX_POS
                                      | TRANS_LP << DSI_DCS_SW_1P_TX_POS
                                      | TRANS_LP << DSI_DCS_SW_0P_TX_POS
                                      | TRANS_LP << DSI_GEN_LW_TX_POS
                                      | TRANS_LP << DSI_GEN_SR_2P_TX_POS
                                      | TRANS_LP << DSI_GEN_SR_1P_TX_POS
                                      | TRANS_LP << DSI_GEN_SR_0P_TX_POS
                                      | TRANS_LP << DSI_GEN_SW_2P_TX_POS
                                      | TRANS_LP << DSI_GEN_SW_1P_TX_POS
                                      | TRANS_LP << DSI_GEN_SW_0P_TX_POS
                                      | 0 << DSI_ACK_RQST_EN_POS
                                      | 0 << DSI_TEAR_FX_EN_POS
                                  );


    DSI->HS_RD_TO_CNT           = (
                                      0 << DSI_HS_RD_TO_CNT_POS
                                  );

    DSI->LP_RD_TO_CNT           = (
                                      0 << DSI_LP_RD_TO_CNT_POS
                                  );

    DSI->HS_WR_TO_CNT           = (
                                      0    << DSI_PRESP_TO_MODE_POS
                                      |   0 << DSI_HS_WR_TO_CNT_POS
                                  );

    DSI->LP_WR_TO_CNT           = (
                                      0 << DSI_LP_WR_TO_CNT_POS
                                  );


    DSI->BTA_TO_CNT             = (
                                      0 << DSI_BTA_TO_CNT_POS
                                  );

    DSI->EDPI_TE_HW_CFG     = (
                                  0 << SCAN_LINE_PARAMETER_POS
                                  | 0 << HW_SET_SCAN_LINE_POS
                                  | 0 << HW_TEAR_EFFECT_GEN_POS
                                  | 0 << HW_TEAR_EFFECT_ON_POS
                              );

    DSI->PCKHDL_CFG             = (
                                      0 << DSI_EOTP_TX_LP_EN_POS
                                      |   0 << DSI_CRC_RX_EN_POS
                                      |   0 << DSI_ECC_RX_EN_POS
                                      |   0 << DSI_BTA_EN_POS
                                      |   0 << DSI_EOTP_RX_EN_POS
                                      |   0 << DSI_EOTP_TX_EN_POS
                                  );

    DSI->GEN_VCID                   = (
                                          0 << DSI_GEN_VCID_TX_AUTO_POS
                                          | 0 << DSI_GEN_VCID_TEAR_AUTO_POS
                                          | 0 << DSI_GEN_VCID_RX_POS
                                      );

    DSI->LPCLK_CTRL             = (
                                      1 << DSI_AUTO_CLKLANE_CTRL_POS
                                      | 1 << DSI_PHY_TXREQUESTCLKHS_POS
                                  );

    DSI->PHY_TMR_RD_CFG     = (
                                  0   << DSI_MAX_RD_TIME_POS
                              );

    DSI->LPCLK_CTRL             = (
                                      1 << DSI_AUTO_CLKLANE_CTRL_POS
                                      | 1 << DSI_PHY_TXREQUESTCLKHS_POS
                                  );

    DSI->PHY_TMR_LPCLK_CFG  = (
                                  (CLK_POST_TIME + TX_TAIL_TIME + HS_EXIT_TIME + 3 + CLOCK_MARGIN) << DSI_PHY_CLKHS2LP_TIME_POS
                                  | (TX_LPX_TIME + CLK_PRPR_TIME + CLK_ZERO_TIME + CLK_PRE_TIME + 4 + CLOCK_MARGIN) <<
                                  DSI_PHY_CLKLP2HS_TIME_POS
                              );

    DSI->PHY_TMR_CFG            = (
                                      (TX_TAIL_TIME + HS_EXIT_TIME + 3 + DATA_MARGIN) << DSI_PHY_HS2LP_TIME_POS
                                      | (TX_LPX_TIME + HS_PRPR_TIME + HS_ZERO_TIME + 3 + DATA_MARGIN) << DSI_PHY_LP2HS_TIME_POS
                                  );


    DSI->AUTO_ULPS_MODE     = (
                                  0 << DSI_PRE_PLL_OFF_REQ_POS
                                  | 0 << DSI_PLL_OFF_ULPS_POS
                                  | 0 << DSI_AUTO_ULPS_POS
                              );

    DSI->AUTO_ULPS_ENTRY_DELAY  = (
                                      0 << DSI_ULPS_ENTRY_DELAY_POS
                                  );

    DSI->AUTO_ULPS_WAKEUP_TIME  = (
                                      0X0 << DSI_TWAKEUP_CNT_POS
                                      | 0 << DSI_TWAKEUP_CLK_DIV_POS
                                  );

    DSI->AUTO_ULPS_MIN_TIME         = (
                                          0X0 << DSI_ULPS_MIN_TIME_POS
                                      );



    /* MIPI D-PHY initialization */
    DSI->PHY_RSTZ                   = (                           // power up for D-PHY
                                          0 << DSI_PHY_FORCEPLL_POS
                                          | 0 << DSI_PHY_ENABLECLK_POS
                                          | 0 << DSI_PHY_RSTZ_POS
                                          | 1 << DSI_PHY_SHUTDOWNZ_POS
                                      );

    /*Set D-PHY bit clock*/
    /*source 800MHz*/
    DPHY->SSC3  = (
                      0x28    <<  REG_OC_STEP_SET_POS
                      | 17        <<  NCODE_T_SCPU_POS
                      | 17        <<  NCODE_SSC_POS
                  );
    DPHY->SSC2  = (
                      0           <<  FCODE_T_SCPU_POS
                      | 0         <<  FCODE_SSC_POS
                  );
    /*enable DIV2 400MHz*/
    DPHY->DF        = (DPHY->DF | REG_EN_ALL_DIV2_MSK);

    DPHY->ESCAPE_TX_CLK_0 &= REG_RQST_LP_MASK_ENABLE_CLR;

    DPHY->ESCAPE_TX_DATA_1 = (
                                 TX_LPX_TIME   << REG_TX_LPX_TIME_POS                  // reg_tx_lpx_time   65ns
                                 | HS_EXIT_TIME  << REG_HS_EXIT_TIME_POS                 // reg_hs_exit_time 100ns
                                 | 0x1100 << 0                                                                   // tx_init_time
                             );
    DPHY->ESCAPE_TX_DATA_2 = (
                                 CLK_PRE_TIME    << REG_CLK_PRE_TIME_POS                 // clk_pre_time 8UI
                                 | CLK_ZERO_TIME     << REG_CLK_ZERO_TIME_POS                // clk_zero_time  300ns
                                 | CLK_PRPR_TIME     << REG_CLK_PRPR_TIME_POS                // clk_prpr_time
                                 | CLK_POST_TIME     << REG_CLK_POST_TIME_POS                // clk_post_time
                             );
    DPHY->ESCAPE_TX_DATA_3 = (
                                 5  << REG_TX_VLD_TIME_POS                                               // tx_vld_time default
                                 | TX_TAIL_TIME      << REG_TX_TAIL_TIME_POS                 // tx_tail_time
                                 | HS_ZERO_TIME      << REG_HS_ZERO_TIME_POS                 // hs_zero_time
                                 | HS_PRPR_TIME      << REG_HS_PRPR_TIME_POS                 // hs_prpr_time
                             );
    DPHY->MPLL                              &=      REG_MPLL_PDIV_CLR;
    DPHY->WATCHDOG                      = (1 << REG_TIME0_CK_POS) | (DPHY->WATCHDOG & REG_TIME0_CK_CLR);

    /*toogle D-PHY*/
    DPHY->DF                        &= ~(REG_POSTDIV4_MSK | REG_POSTDIV3_MSK | REG_POSTDIV2_MSK);
    DPHY->CLOCK_GEN         &= ~(REG_CLK_LANE_EN_MSK | REG_LANE3_EN_MSK | REG_LANE2_EN_MSK |
                                 REG_LANE1_EN_MSK | REG_LANE0_EN_MSK | REG_RX_RST_N_MSK | REG_TX_RST_N_MSK);
    DPHY->DF                        &= ~(REG_PWCT_MSK | REG_MBIAS_EN_MSK);
    DPHY->WATCHDOG          &= ~RSTB_MSK;
    /*toogle to 1*/
    DPHY->DF                        |= (REG_POSTDIV4_MSK | REG_POSTDIV3_MSK | REG_POSTDIV2_MSK);
    DPHY->CLOCK_GEN         |= (REG_CLK_LANE_EN_MSK | REG_LANE3_EN_MSK | REG_LANE0_EN_MSK |
                                REG_RX_RST_N_MSK | REG_TX_RST_N_MSK);           /*only data lane 0 enable*/
    DPHY->DF                        |= (REG_PWCT_MSK | REG_MBIAS_EN_MSK);
    DPHY->WATCHDOG          |= (RSTB_MSK);
    DSI->PHY_RSTZ                   = (
                                          0 << DSI_PHY_FORCEPLL_POS
                                          | 1 << DSI_PHY_ENABLECLK_POS
                                          | 0 << DSI_PHY_RSTZ_POS
                                          | 0 << DSI_PHY_SHUTDOWNZ_POS
                                      );
    DSI->PHY_RSTZ                   = (
                                          0 << DSI_PHY_FORCEPLL_POS
                                          | 1 << DSI_PHY_ENABLECLK_POS
                                          | 1 << DSI_PHY_RSTZ_POS
                                          | 1 << DSI_PHY_SHUTDOWNZ_POS
                                      );
    while ((DSI->PHY_STATUS & DSI_PHY_STOPSTATECLKLANE_MSK) == 0U);
    while ((DSI->PHY_STATUS & DSI_PHY_STOPSTATE0LANE_MSK) == 0U);
    DSI->PWR_UP                     = (1 <<
                                       DSI_SHUTDOWNZ_POS);                                      //power up DW DSI



    DPHY->DF |= REG_PWCT_MSK;
    DPHY->DF |= REG_MBIAS_EN_MSK;

    DPHY->MPLL |= REG_EN_TX_LP_LDO_MSK;

    DPHY->TXF |= REG_EN_LV_LDO_MSK;

    DPHY->CLOCK_GEN |= REG_MTX_EN_MSK;

    DPHY->WATCHDOG |= RSTB_MSK;

    DPHY->CLOCK_GEN |= REG_TX_RST_N_MSK;
    DPHY->CLOCK_GEN |= REG_RX_RST_N_MSK;
    DPHY->CLOCK_GEN |= REG_CLK_LANE_EN_MSK;
    DPHY->CLOCK_GEN &= REG_LANE3_EN_CLR;
    DPHY->CLOCK_GEN &= REG_LANE2_EN_CLR;
    DPHY->CLOCK_GEN &= REG_LANE1_EN_CLR;
    DPHY->CLOCK_GEN |= REG_LANE0_EN_MSK;
#endif

    DSI->CMD_MODE_CFG           = (
                                      TRANS_HS << DSI_MAX_RD_PKT_SIZE_POS
                                      | TRANS_HS << DSI_DCS_LW_TX_POS
                                      | TRANS_HS << DSI_DCS_SR_0P_TX_POS
                                      | TRANS_HS << DSI_DCS_SW_1P_TX_POS
                                      | TRANS_HS << DSI_DCS_SW_0P_TX_POS
                                      | TRANS_HS << DSI_GEN_LW_TX_POS
                                      | TRANS_HS << DSI_GEN_SR_2P_TX_POS
                                      | TRANS_HS << DSI_GEN_SR_1P_TX_POS
                                      | TRANS_HS << DSI_GEN_SR_0P_TX_POS
                                      | TRANS_HS << DSI_GEN_SW_2P_TX_POS
                                      | TRANS_HS << DSI_GEN_SW_1P_TX_POS
                                      | TRANS_HS << DSI_GEN_SW_0P_TX_POS
                                      | 0 << DSI_ACK_RQST_EN_POS
                                      | 0 << DSI_TEAR_FX_EN_POS
                                  );
    driver_ic_init();
}





uint32_t rtk_lcd_hal_power_on(void)
{
    uint8_t ShortRegData44[] = {0x29, 0x00};
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData44);
    uint8_t ShortRegData36[] = {0x11, 0x00};
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData36);
    return 0;
}

uint32_t rtk_lcd_hal_power_off(void)
{
    uint8_t ShortRegData35[] = {0x10, 0x00};
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData35);
    uint8_t ShortRegData43[] = {0x28, 0x00};
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData43);
    return 0;
}

uint32_t rtk_lcd_hal_dlps_restore(void)
{
    RCC_PeriphClockCmd(APBPeriph_DISP, APBPeriph_DISP_CLOCK_CLOCK, ENABLE);
    RCC_PeriphClockCmd(APBPeriph_MIPI_HOST, APBPeriph_MIPI_HOST_CLOCK, ENABLE);

    Pad_Config(P17_2, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Dedicated_Config(P17_2, ENABLE);
    drv_pin_mode(P13_7, PIN_MODE_OUTPUT);
    drv_pin_write(P13_7, 0); //im0
    drv_pin_mode(P13_6, PIN_MODE_OUTPUT);
    drv_pin_write(P13_6, 1); //im1

    LCDC_InitTypeDef lcdc_init = {0};
    lcdc_init.LCDC_Interface = LCDC_IF_eDPI;
    lcdc_init.LCDC_PixelInputFarmat = LCDC_INPUT_ARGB8888;
    lcdc_init.LCDC_PixelOutpuFarmat = LCDC_OUTPUT_RGB888;
    lcdc_init.LCDC_PixelBitSwap = LCDC_SWAP_BYPASS; //lcdc_handler_cfg->LCDC_TeEn = DISABLE;

    lcdc_init.LCDC_TeEn = DISABLE;
    lcdc_init.LCDC_TePolarity = LCDC_TE_EDGE_RISING;
    lcdc_init.LCDC_TeInputMux = LCDC_TE_LCD_INPUT;

    lcdc_init.LCDC_DmaThreshold =
        8;    //only support threshold = 8 for DMA MSIZE = 8; the other threshold setting will be support later
    LCDC_Init(&lcdc_init);

    LCDC_eDPICfgTypeDef eDPICfg;//836 * 550, 42FPS == 19311600
    eDPICfg.eDPI_ClockDiv = 4;
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
    eDPICfg.eDPI_HoriSyncPolarity = 1;
    eDPICfg.eDPI_VeriSyncPolarity = 1;
    eDPICfg.eDPI_DataEnPolarity = 1;
    eDPICfg.eDPI_LineIntMask = 1;
    //eDPICfg.eDPI_LineIntFlag = 0;
    //eDPICfg.eDPI_LineIntClr = 0;
    eDPICfg.eDPI_ColorMap = 0;//for RGB888
    eDPICfg.eDPI_OperateMode = EDPI_ADAPTED_COMMAND_MODE;//for adpated cmd mode
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


#if 0

    DSI_DeInit(DSI);
    DSI_InitTypeDef   DSI_InitStruct = {0};
    DSI_InitStruct.VirtualChannelID             = 0;

    DSI_InitStruct.ColorCoding                  = 0;
    DSI_InitStruct.DEPolarity                   = 0;
    DSI_InitStruct.HSPolarity                   = 0;
    DSI_InitStruct.VSPolarity                   = 0;

    DSI_InitStruct.phy_mode = 0x0;  //fix
    DSI_InitStruct.NumberOfLanes = 0x01;
    DSI_InitStruct.StopWaitTime = 0;    //skip
    DSI_InitStruct.InterruptMask0 = 0x1FFFFF;
    DSI_InitStruct.InterruptMask1 = 0x1FFFFF;
    DSI_InitStruct.HS_TX_CNT =
        0xFFFF;                 //for trigger timeout error measured in TO_CLK_DIVISION
    DSI_InitStruct.LP_RX_CNT =
        0xFFFF;                 //for trigger timeout error measured in TO_CLK_DIVISION
    DSI_InitStruct.TOCkdiv = 0x4;
    DSI_InitStruct.TXEscapeCkdiv = 0x03;
    DSI_InitStruct.ModeCfg = 1;                   //config for video mode or cmd mode
    DSI_InitStruct.CommandSize                  = 598;
    DSI_InitStruct.PeripherCharacteristics = 0;
    DSI_InitStruct.GenericPacketVCID = 0;
    DSI_InitStruct.GenericTearVCID = 0;
    DSI_InitStruct.GenericReadBackVCID = 0;
    DSI_InitStruct.AutoHS2LPEnable = 1;
    DSI_InitStruct.TxRequestCLKEnable = 1;
    DSI_InitStruct.ClockLaneHS2LPTime = 0x24;
    DSI_InitStruct.ClockLaneLP2HSTime = 0x3d;
    DSI_InitStruct.DataLaneHS2LPTime = 0x26;
    DSI_InitStruct.DataLaneLP2HSTime = 0xA1;
    DSI_InitStruct.DataLaneMaxReadTime = 0;
    DSI_InitStruct.phy_shutdownz = 1;
//    DSI_InitStruct.phy_testclr = 0;
//    DSI_InitStruct.phy_testen = 0;
    DSI_InitStruct.phy_enableclk = 1;
//    DSI_InitStruct.phy_forcepll = 0;
    DSI_InitStruct.phy_rstz = 1;
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
    DSI_InitStruct.LPDcsLongWrite               = DSI_LP_DLW_DISABLE;
    DSI_Init(DSI, &DSI_InitStruct);

    DSI_AdaptedCmdCfgTypeDef   AdaptedCmd_InitStruct = {0};
    AdaptedCmd_InitStruct.TEAcknowledgeRequest         = DSI_TE_ACKNOWLEDGE_DISABLE;
    AdaptedCmd_InitStruct.hw_tear_effect_gen       = 0;
    AdaptedCmd_InitStruct.scan_line_parameter   = 0;
    AdaptedCmd_InitStruct.hw_set_scan_line    = 0;
    AdaptedCmd_InitStruct.hw_tear_effect_on   = 0;

    DSI_ConfigAdaptedCommandMode(DSI, &AdaptedCmd_InitStruct);


    DSI_PhyCfgTypeDef dsi_phy = {0};
    dsi_phy.phy_forepll  = PHY_FORCEPLL_DISABLE;
    dsi_phy.phy_enableclk  = PHY_CLK_ENABLE;
    dsi_phy.phy_rstz   = PHY_RST_ENABLE;
    dsi_phy.phy_shutdownz   = PHY_SHUTDOWN_ENABLE;
    DSI_ConfigPhy(DSI, &dsi_phy);

    DSI_ConfigPhy(DSI, &dsi_phy);

    DSI_Start(DSI);

//    uint8_t pass_word_0[] = {0x5a, 0x5a, 0xC0};
//    DSI_IO_WriteCmd(2, (uint8_t *)pass_word_0);
//    uint8_t pass_word_1[] = {0x5a, 0x5a, 0xC1};
//    DSI_IO_WriteCmd(2, (uint8_t *)pass_word_1);
//    uint8_t video[] = {0xB1, 0X00};
//    DSI_IO_WriteCmd(0, (uint8_t *)video);



    //for bist mode
//    uint8_t bist[] = {0xBA, 0X81};
//    DSI_IO_WriteCmd(0, (uint8_t *)bist);
#else
    DSI->PWR_UP                             = (DW_DSI_RESET <<
                                               DSI_SHUTDOWNZ_POS);                        //reset DW DSI

    DSI->PHY_MODE                           = (DW_DSI_DPHY <<
                                               DSI_PHY_MODE_POS);                          // D-PHY

    DSI->PHY_IF_CFG                     = (0      << DSI_PHY_STOP_WAIT_TIME_POS
                                           |   DW_DSI_LANE0    << DSI_N_LANES_POS                          // 1 LANE
                                          );

    DSI->INT_MSK0                           = (
                                                  DW_DSI_MASK << DSI_MASK_DPHY_ERRORS_4_POS
                                                  |   DW_DSI_MASK << DSI_MASK_DPHY_ERRORS_3_POS
                                                  |   DW_DSI_MASK << DSI_MASK_DPHY_ERRORS_2_POS
                                                  |   DW_DSI_MASK << DSI_MASK_DPHY_ERRORS_1_POS
                                                  |   DW_DSI_MASK << DSI_MASK_DPHY_ERRORS_0_POS
                                                  | DW_DSI_MASK << DSI_MASK_ACK_WITH_ERR_15_POS
                                                  | DW_DSI_MASK << DSI_MASK_ACK_WITH_ERR_14_POS
                                                  | DW_DSI_MASK << DSI_MASK_ACK_WITH_ERR_13_POS
                                                  | DW_DSI_MASK << DSI_MASK_ACK_WITH_ERR_12_POS
                                                  | DW_DSI_MASK << DSI_MASK_ACK_WITH_ERR_11_POS
                                                  | DW_DSI_MASK << DSI_MASK_ACK_WITH_ERR_10_POS
                                                  | DW_DSI_MASK << DSI_MASK_ACK_WITH_ERR_9_POS
                                                  | DW_DSI_MASK << DSI_MASK_ACK_WITH_ERR_8_POS
                                                  | DW_DSI_MASK << DSI_MASK_ACK_WITH_ERR_7_POS
                                                  | DW_DSI_MASK << DSI_MASK_ACK_WITH_ERR_6_POS
                                                  | DW_DSI_MASK << DSI_MASK_ACK_WITH_ERR_5_POS
                                                  | DW_DSI_MASK << DSI_MASK_ACK_WITH_ERR_4_POS
                                                  | DW_DSI_MASK << DSI_MASK_ACK_WITH_ERR_3_POS
                                                  | DW_DSI_MASK << DSI_MASK_ACK_WITH_ERR_2_POS
                                                  | DW_DSI_MASK << DSI_MASK_ACK_WITH_ERR_1_POS
                                                  | DW_DSI_MASK << DSI_MASK_ACK_WITH_ERR_0_POS
                                              );


    DSI->INT_MSK1                           = (
                                                  DW_DSI_MASK << DSI_MASK_TEAR_REQUEST_ERR_POS
                                                  | DW_DSI_MASK << DSI_MASK_DPI_BUFF_PLD_UNDER_POS
                                                  | DW_DSI_MASK << DSI_MASK_DBI_ILEGAL_COMM_ERR_POS
                                                  | DW_DSI_MASK << DSI_MASK_DBI_PLD_RECV_ERR_POS
                                                  | DW_DSI_MASK << DSI_MASK_DBI_PLD_RD_ERR_POS
                                                  | DW_DSI_MASK << DSI_MASK_DBI_PLD_WR_ERR_POS
                                                  | DW_DSI_MASK << DSI_MASK_DBI_CMD_WR_ERR_POS
                                                  | DW_DSI_MASK << DSI_MASK_GEN_PLD_RECEV_ERR_POS
                                                  | DW_DSI_MASK << DSI_MASK_GEN_PLD_RD_ERR_POS
                                                  | DW_DSI_MASK << DSI_MASK_GEN_PLD_SEND_ERR_POS
                                                  | DW_DSI_MASK << DSI_MASK_GEN_PLD_WR_ERR_POS
                                                  | DW_DSI_MASK << DSI_MASK_GEN_CMD_WR_ERR_POS
                                                  | DW_DSI_MASK << DSI_MASK_DPI_PLD_WR_ERR_POS
                                                  | DW_DSI_MASK << DSI_MASK_EOPT_ERR_POS
                                                  | DW_DSI_MASK << DSI_MASK_PKT_SIZE_ERR_POS
                                                  | DW_DSI_MASK << DSI_MASK_CRC_ERR_POS
                                                  | DW_DSI_MASK << DSI_MASK_ECC_MILTI_ERR_POS
                                                  | DW_DSI_MASK << DSI_MASK_ECC_SINGLE_ERR_POS
                                                  | DW_DSI_MASK << DSI_MASK_TO_LP_RX_POS
                                                  | DW_DSI_MASK << DSI_MASK_TO_HS_TX_POS
                                              );


    DSI->TO_CNT_CFG             = (
                                      0XFFFF << DSI_HSTX_TO_CNT_POS
                                      |   0XFFFF << DSI_LPRX_TO_CNT_POS
                                  );

    DSI->CLKMGR_CFG             = (
                                      0X14                                            <<  DSI_TO_CLK_DIVISION_POS
                                      |   TEST_LANEBYTECLK_DIV            <<
                                      DSI_TX_ESC_CLK_DIVISION_POS                 // tx_clk_esc   13.3MHz-20MHz
                                  );

    DSI->MODE_CFG                   = (
                                          1 << DSI_CMD_VIDEO_MODE_POS                                         // COMMAND MODE
                                      );

    DSI->DPI_VCID                   = (
                                          0   <<  DSI_DPI_VCID_POS                                                    // VIRTUAL CHANNEL ID 0
                                      );

    DSI->DPI_COLOR_CODING   = (
                                  5 << DSI_DPI_COLOR_CODING_POS                                       //RGB888_BYTE_NUM COLOR CODING
                              );

    DSI->DPI_CFG_POL            = (
                                      1 << DSI_COLORM_ACTIVE_LOW_POS
                                      |   1 << DSI_SHUTD_ACTIVE_LOW_POS
                                      | 0 << DSI_HSYNC_ACTIVE_LOW_POS
                                      | 0 << DSI_VSYNC_ACTIVE_LOW_POS
                                      | 0 << DSI_DATAEN_ACTIVE_LOW_POS
                                  );

    DSI->EDPI_CMD_SIZE      = (
                                  0x256 <<
                                  DSI_EDPI_ALLOWED_CMD_SIZE_POS                       //  pixel for one packet,don't set it too small
                              );

    DSI->CMD_MODE_CFG           = (
                                      TRANS_LP << DSI_MAX_RD_PKT_SIZE_POS
                                      | TRANS_LP << DSI_DCS_LW_TX_POS
                                      | TRANS_LP << DSI_DCS_SR_0P_TX_POS
                                      | TRANS_LP << DSI_DCS_SW_1P_TX_POS
                                      | TRANS_LP << DSI_DCS_SW_0P_TX_POS
                                      | TRANS_LP << DSI_GEN_LW_TX_POS
                                      | TRANS_LP << DSI_GEN_SR_2P_TX_POS
                                      | TRANS_LP << DSI_GEN_SR_1P_TX_POS
                                      | TRANS_LP << DSI_GEN_SR_0P_TX_POS
                                      | TRANS_LP << DSI_GEN_SW_2P_TX_POS
                                      | TRANS_LP << DSI_GEN_SW_1P_TX_POS
                                      | TRANS_LP << DSI_GEN_SW_0P_TX_POS
                                      | 0 << DSI_ACK_RQST_EN_POS
                                      | 0 << DSI_TEAR_FX_EN_POS
                                  );


    DSI->HS_RD_TO_CNT           = (
                                      0 << DSI_HS_RD_TO_CNT_POS
                                  );

    DSI->LP_RD_TO_CNT           = (
                                      0 << DSI_LP_RD_TO_CNT_POS
                                  );

    DSI->HS_WR_TO_CNT           = (
                                      0    << DSI_PRESP_TO_MODE_POS
                                      |   0 << DSI_HS_WR_TO_CNT_POS
                                  );

    DSI->LP_WR_TO_CNT           = (
                                      0 << DSI_LP_WR_TO_CNT_POS
                                  );


    DSI->BTA_TO_CNT             = (
                                      0 << DSI_BTA_TO_CNT_POS
                                  );

    DSI->EDPI_TE_HW_CFG     = (
                                  0 << SCAN_LINE_PARAMETER_POS
                                  | 0 << HW_SET_SCAN_LINE_POS
                                  | 0 << HW_TEAR_EFFECT_GEN_POS
                                  | 0 << HW_TEAR_EFFECT_ON_POS
                              );

    DSI->PCKHDL_CFG             = (
                                      0 << DSI_EOTP_TX_LP_EN_POS
                                      |   0 << DSI_CRC_RX_EN_POS
                                      |   0 << DSI_ECC_RX_EN_POS
                                      |   0 << DSI_BTA_EN_POS
                                      |   0 << DSI_EOTP_RX_EN_POS
                                      |   0 << DSI_EOTP_TX_EN_POS
                                  );

    DSI->GEN_VCID                   = (
                                          0 << DSI_GEN_VCID_TX_AUTO_POS
                                          | 0 << DSI_GEN_VCID_TEAR_AUTO_POS
                                          | 0 << DSI_GEN_VCID_RX_POS
                                      );

    DSI->LPCLK_CTRL             = (
                                      1 << DSI_AUTO_CLKLANE_CTRL_POS
                                      | 1 << DSI_PHY_TXREQUESTCLKHS_POS
                                  );

    DSI->PHY_TMR_RD_CFG     = (
                                  0   << DSI_MAX_RD_TIME_POS
                              );

    DSI->LPCLK_CTRL             = (
                                      1 << DSI_AUTO_CLKLANE_CTRL_POS
                                      | 1 << DSI_PHY_TXREQUESTCLKHS_POS
                                  );

    DSI->PHY_TMR_LPCLK_CFG  = (
                                  (CLK_POST_TIME + TX_TAIL_TIME + HS_EXIT_TIME + 3 + CLOCK_MARGIN) << DSI_PHY_CLKHS2LP_TIME_POS
                                  | (TX_LPX_TIME + CLK_PRPR_TIME + CLK_ZERO_TIME + CLK_PRE_TIME + 4 + CLOCK_MARGIN) <<
                                  DSI_PHY_CLKLP2HS_TIME_POS
                              );

    DSI->PHY_TMR_CFG            = (
                                      (TX_TAIL_TIME + HS_EXIT_TIME + 3 + DATA_MARGIN) << DSI_PHY_HS2LP_TIME_POS
                                      | (TX_LPX_TIME + HS_PRPR_TIME + HS_ZERO_TIME + 3 + DATA_MARGIN) << DSI_PHY_LP2HS_TIME_POS
                                  );


    DSI->AUTO_ULPS_MODE     = (
                                  0 << DSI_PRE_PLL_OFF_REQ_POS
                                  | 0 << DSI_PLL_OFF_ULPS_POS
                                  | 0 << DSI_AUTO_ULPS_POS
                              );

    DSI->AUTO_ULPS_ENTRY_DELAY  = (
                                      0 << DSI_ULPS_ENTRY_DELAY_POS
                                  );

    DSI->AUTO_ULPS_WAKEUP_TIME  = (
                                      0X0 << DSI_TWAKEUP_CNT_POS
                                      | 0 << DSI_TWAKEUP_CLK_DIV_POS
                                  );

    DSI->AUTO_ULPS_MIN_TIME         = (
                                          0X0 << DSI_ULPS_MIN_TIME_POS
                                      );



    /* MIPI D-PHY initialization */
    DSI->PHY_RSTZ                   = (                           // power up for D-PHY
                                          0 << DSI_PHY_FORCEPLL_POS
                                          | 0 << DSI_PHY_ENABLECLK_POS
                                          | 0 << DSI_PHY_RSTZ_POS
                                          | 1 << DSI_PHY_SHUTDOWNZ_POS
                                      );

    /*Set D-PHY bit clock*/
    /*source 800MHz*/
    DPHY->SSC3  = (
                      0x28    <<  REG_OC_STEP_SET_POS
                      | 17        <<  NCODE_T_SCPU_POS
                      | 17        <<  NCODE_SSC_POS
                  );
    DPHY->SSC2  = (
                      0           <<  FCODE_T_SCPU_POS
                      | 0         <<  FCODE_SSC_POS
                  );
    /*enable DIV2 400MHz*/
    DPHY->DF        = (DPHY->DF | REG_EN_ALL_DIV2_MSK);

    DPHY->ESCAPE_TX_CLK_0 &= REG_RQST_LP_MASK_ENABLE_CLR;

    DPHY->ESCAPE_TX_DATA_1 = (
                                 TX_LPX_TIME   << REG_TX_LPX_TIME_POS                  // reg_tx_lpx_time   65ns
                                 | HS_EXIT_TIME  << REG_HS_EXIT_TIME_POS                 // reg_hs_exit_time 100ns
                                 | 0x1100 << 0                                                                   // tx_init_time
                             );
    DPHY->ESCAPE_TX_DATA_2 = (
                                 CLK_PRE_TIME    << REG_CLK_PRE_TIME_POS                 // clk_pre_time 8UI
                                 | CLK_ZERO_TIME     << REG_CLK_ZERO_TIME_POS                // clk_zero_time  300ns
                                 | CLK_PRPR_TIME     << REG_CLK_PRPR_TIME_POS                // clk_prpr_time
                                 | CLK_POST_TIME     << REG_CLK_POST_TIME_POS                // clk_post_time
                             );
    DPHY->ESCAPE_TX_DATA_3 = (
                                 5  << REG_TX_VLD_TIME_POS                                               // tx_vld_time default
                                 | TX_TAIL_TIME      << REG_TX_TAIL_TIME_POS                 // tx_tail_time
                                 | HS_ZERO_TIME      << REG_HS_ZERO_TIME_POS                 // hs_zero_time
                                 | HS_PRPR_TIME      << REG_HS_PRPR_TIME_POS                 // hs_prpr_time
                             );
    DPHY->MPLL                              &=      REG_MPLL_PDIV_CLR;
    DPHY->WATCHDOG                      = (1 << REG_TIME0_CK_POS) | (DPHY->WATCHDOG & REG_TIME0_CK_CLR);

    /*toogle D-PHY*/
    DPHY->DF                        &= ~(REG_POSTDIV4_MSK | REG_POSTDIV3_MSK | REG_POSTDIV2_MSK);
    DPHY->CLOCK_GEN         &= ~(REG_CLK_LANE_EN_MSK | REG_LANE3_EN_MSK | REG_LANE2_EN_MSK |
                                 REG_LANE1_EN_MSK | REG_LANE0_EN_MSK | REG_RX_RST_N_MSK | REG_TX_RST_N_MSK);
    DPHY->DF                        &= ~(REG_PWCT_MSK | REG_MBIAS_EN_MSK);
    DPHY->WATCHDOG          &= ~RSTB_MSK;
    /*toogle to 1*/
    DPHY->DF                        |= (REG_POSTDIV4_MSK | REG_POSTDIV3_MSK | REG_POSTDIV2_MSK);
    DPHY->CLOCK_GEN         |= (REG_CLK_LANE_EN_MSK | REG_LANE3_EN_MSK | REG_LANE0_EN_MSK |
                                REG_RX_RST_N_MSK | REG_TX_RST_N_MSK);           /*only data lane 0 enable*/
    DPHY->DF                        |= (REG_PWCT_MSK | REG_MBIAS_EN_MSK);
    DPHY->WATCHDOG          |= (RSTB_MSK);
    DSI->PHY_RSTZ                   = (
                                          0 << DSI_PHY_FORCEPLL_POS
                                          | 1 << DSI_PHY_ENABLECLK_POS
                                          | 0 << DSI_PHY_RSTZ_POS
                                          | 0 << DSI_PHY_SHUTDOWNZ_POS
                                      );
    DSI->PHY_RSTZ                   = (
                                          0 << DSI_PHY_FORCEPLL_POS
                                          | 1 << DSI_PHY_ENABLECLK_POS
                                          | 1 << DSI_PHY_RSTZ_POS
                                          | 1 << DSI_PHY_SHUTDOWNZ_POS
                                      );
    while ((DSI->PHY_STATUS & DSI_PHY_STOPSTATECLKLANE_MSK) == 0U);
    while ((DSI->PHY_STATUS & DSI_PHY_STOPSTATE0LANE_MSK) == 0U);
    DSI->PWR_UP                     = (1 <<
                                       DSI_SHUTDOWNZ_POS);                                      //power up DW DSI



    DPHY->DF |= REG_PWCT_MSK;
    DPHY->DF |= REG_MBIAS_EN_MSK;

    DPHY->MPLL |= REG_EN_TX_LP_LDO_MSK;

    DPHY->TXF |= REG_EN_LV_LDO_MSK;

    DPHY->CLOCK_GEN |= REG_MTX_EN_MSK;

    DPHY->WATCHDOG |= RSTB_MSK;

    DPHY->CLOCK_GEN |= REG_TX_RST_N_MSK;
    DPHY->CLOCK_GEN |= REG_RX_RST_N_MSK;
    DPHY->CLOCK_GEN |= REG_CLK_LANE_EN_MSK;
    DPHY->CLOCK_GEN &= REG_LANE3_EN_CLR;
    DPHY->CLOCK_GEN &= REG_LANE2_EN_CLR;
    DPHY->CLOCK_GEN &= REG_LANE1_EN_CLR;
    DPHY->CLOCK_GEN |= REG_LANE0_EN_MSK;
#endif

    DSI->CMD_MODE_CFG           = (
                                      TRANS_HS << DSI_MAX_RD_PKT_SIZE_POS
                                      | TRANS_HS << DSI_DCS_LW_TX_POS
                                      | TRANS_HS << DSI_DCS_SR_0P_TX_POS
                                      | TRANS_HS << DSI_DCS_SW_1P_TX_POS
                                      | TRANS_HS << DSI_DCS_SW_0P_TX_POS
                                      | TRANS_HS << DSI_GEN_LW_TX_POS
                                      | TRANS_HS << DSI_GEN_SR_2P_TX_POS
                                      | TRANS_HS << DSI_GEN_SR_1P_TX_POS
                                      | TRANS_HS << DSI_GEN_SR_0P_TX_POS
                                      | TRANS_HS << DSI_GEN_SW_2P_TX_POS
                                      | TRANS_HS << DSI_GEN_SW_1P_TX_POS
                                      | TRANS_HS << DSI_GEN_SW_0P_TX_POS
                                      | 0 << DSI_ACK_RQST_EN_POS
                                      | 0 << DSI_TEAR_FX_EN_POS
                                  );
    return 0;
}


void rtk_lcd_hal_set_window(uint16_t xStart, uint16_t yStart, uint16_t w, uint16_t h)
{
    uint16_t xEnd = xStart + w - 1;
    uint16_t yEnd = yStart + h - 1;
    uint8_t lcdRegData27[] = {xStart >> 8, xStart & 0xff, xEnd >> 8, xEnd & 0xff, 0x2A};
    uint8_t lcdRegData28[] = {yStart >> 8, yStart & 0xff, yEnd >> 8, yEnd & 0xff, 0x2B};

    DSI_IO_WriteCmd(4, (uint8_t *)lcdRegData27);
    DSI_IO_WriteCmd(4, (uint8_t *)lcdRegData28);
    //HAL_WRITE32(0x40602000, 0x64, 8);
}
void rtk_lcd_hal_update_framebuffer(uint8_t *buf, uint32_t len)
{
#define DMA_LINKLIST 1
#if (DMA_LINKLIST == 0)
    GDMA_InitTypeDef LCDC_DMA_InitStruct = {0};
    LCDC_DMA_StructInit(&LCDC_DMA_InitStruct);
    LCDC_DMA_InitStruct.GDMA_ChannelNum          = LCDC_DMA_CHANNEL_NUM;
    LCDC_DMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Inc;
    LCDC_DMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Fix;
    LCDC_DMA_InitStruct.GDMA_SourceDataSize      = GDMA_DataSize_Word;
    LCDC_DMA_InitStruct.GDMA_DestinationDataSize = GDMA_DataSize_Word;
    LCDC_DMA_InitStruct.GDMA_SourceMsize         = GDMA_Msize_8;
    LCDC_DMA_InitStruct.GDMA_DestinationMsize    = GDMA_Msize_8;
    LCDC_DMA_InitStruct.GDMA_SourceAddr          = (uint32_t)buf;
    LCDC_DMA_InitStruct.GDMA_Multi_Block_En     = 0;
    LCDC_DMA_Init(LCDC_DMA_CHANNEL_INDEX, &LCDC_DMA_InitStruct);
#else
    GDMA_InitTypeDef LCDC_DMA_InitStruct = {0};
    LCDC_DMA_StructInit(&LCDC_DMA_InitStruct);
    LCDC_DMA_InitStruct.GDMA_ChannelNum          = LCDC_DMA_CHANNEL_NUM;
    LCDC_DMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Inc;
    LCDC_DMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Fix;
    LCDC_DMA_InitStruct.GDMA_SourceDataSize      = GDMA_DataSize_Word;
    LCDC_DMA_InitStruct.GDMA_DestinationDataSize = GDMA_DataSize_Word;
    LCDC_DMA_InitStruct.GDMA_SourceMsize         = GDMA_Msize_8;
    LCDC_DMA_InitStruct.GDMA_DestinationMsize    = GDMA_Msize_8;
    LCDC_DMA_InitStruct.GDMA_SourceAddr          = 0;

    LCDC_DMA_InitStruct.GDMA_Multi_Block_Mode   =
        LLI_TRANSFER;//LLI_TRANSFER or LLI_WITH_CONTIGUOUS_SAR
    LCDC_DMA_InitStruct.GDMA_Multi_Block_En     = 1;
    LCDC_DMA_InitStruct.GDMA_Multi_Block_Struct  = LCDC_DMA_LINKLIST_REG_BASE + 0x50;
    LCDC_DMA_Init(LCDC_DMA_CHANNEL_INDEX, &LCDC_DMA_InitStruct);

    LCDC_SET_GROUP1_BLOCKSIZE(SH8601A_LCD_WIDTH * INPUT_PIXEL_BYTES);
    LCDC_SET_GROUP2_BLOCKSIZE(SH8601A_LCD_WIDTH * INPUT_PIXEL_BYTES);

    /*16 pixel aligned for GPU*/
    //uint32_t gpu_width = SH8601A_LCD_WIDTH;
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

    TEAR_CTR_t handler_reg_0x10 = {.d32 = LCDC_HANDLER->TEAR_CTR};
    handler_reg_0x10.b.bypass_t2w_delay = 0;
    handler_reg_0x10.b.t2w_delay = 0xfff;
    LCDC_HANDLER->TEAR_CTR = handler_reg_0x10.d32;
    LCDC_TeCmd(ENABLE);

    uint32_t time_out_ms = 100;
    while (((LCDC_HANDLER->DMA_FIFO_CTRL & LCDC_DMA_ENABLE) != RESET) && (time_out_ms != 0))
    {
        platform_delay_ms(1);
        time_out_ms--;
    }
    if (time_out_ms != 0)
    {
        while (((LCDC_HANDLER->DMA_FIFO_OFFSET & LCDC_DMA_TX_FIFO_OFFSET) != RESET) &&
               (LCDC_HANDLER->TX_CNT == LCDC_HANDLER->TX_LEN));

        LCDC_TeCmd(DISABLE);                            // disable Tear trigger auto_write_start
    }
    else
    {
        LCDC_AutoWriteCmd(ENABLE);
        while ((LCDC_HANDLER->DMA_FIFO_CTRL & LCDC_DMA_ENABLE) != RESET);
        while (((LCDC_HANDLER->DMA_FIFO_OFFSET & LCDC_DMA_TX_FIFO_OFFSET) != RESET) &&
               (LCDC_HANDLER->TX_CNT == LCDC_HANDLER->TX_LEN));
    }

    LCDC_ClearDmaFifo();
    LCDC_ClearTxPixelCnt();

    LCDC_Cmd(DISABLE);

}



void rtk_lcd_hal_start_transfer(uint8_t *buf, uint32_t len)
{
    //todo
}
void rtk_lcd_hal_transfer_done(void)
{
    //todo
}

