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
#include "otm8009a_480800_mipi_video.h"
#include "rtl_lcdc.h"
#include "rtl_lcdc_dsi.h"
#include "rtl_lcdc_edpi.h"
#include "rtl_nvic.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Components
  * @{
  */

/** @defgroup OTM8009A OTM8009A
  * @brief     This file provides a set of functions needed to drive the
  *            otm8009a IC display driver.
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/** @defgroup OTM8009A_Private_Constants OTM8009A Private Constants
  * @{
  */
#define LCDC_DMA_CHANNEL_NUM              0
#define LCDC_DMA_CHANNEL_INDEX            LCDC_DMA_Channel0
/*
 * @brief Constant tables of register settings used to transmit DSI
 * command packets as power up initialization sequence of the KoD LCD (OTM8009A LCD Driver)
 */
const uint8_t lcdRegData1[]  = {0x80, 0x09, 0x01, 0xFF};
const uint8_t lcdRegData2[]  = {0x80, 0x09, 0xFF};
const uint8_t lcdRegData3[]  = {0x00, 0x09, 0x0F, 0x0E, 0x07, 0x10, 0x0B, 0x0A, 0x04, 0x07, 0x0B, 0x08, 0x0F, 0x10, 0x0A, 0x01, 0xE1};
const uint8_t lcdRegData4[]  = {0x00, 0x09, 0x0F, 0x0E, 0x07, 0x10, 0x0B, 0x0A, 0x04, 0x07, 0x0B, 0x08, 0x0F, 0x10, 0x0A, 0x01, 0xE2};
const uint8_t lcdRegData5[]  = {0x79, 0x79, 0xD8};
const uint8_t lcdRegData6[]  = {0x00, 0x01, 0xB3};
const uint8_t lcdRegData7[]  = {0x85, 0x01, 0x00, 0x84, 0x01, 0x00, 0xCE};
const uint8_t lcdRegData8[]  = {0x18, 0x04, 0x03, 0x39, 0x00, 0x00, 0x00, 0x18, 0x03, 0x03, 0x3A, 0x00, 0x00, 0x00, 0xCE};
const uint8_t lcdRegData9[]  = {0x18, 0x02, 0x03, 0x3B, 0x00, 0x00, 0x00, 0x18, 0x01, 0x03, 0x3C, 0x00, 0x00, 0x00, 0xCE};
const uint8_t lcdRegData10[] = {0x01, 0x01, 0x20, 0x20, 0x00, 0x00, 0x01, 0x02, 0x00, 0x00, 0xCF};
const uint8_t lcdRegData11[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xCB};
const uint8_t lcdRegData12[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xCB};
const uint8_t lcdRegData13[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xCB};
const uint8_t lcdRegData14[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xCB};
const uint8_t lcdRegData15[] = {0x00, 0x04, 0x04, 0x04, 0x04, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xCB};
const uint8_t lcdRegData16[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x04, 0x04, 0x04, 0x04, 0x00, 0x00, 0x00, 0x00, 0xCB};
const uint8_t lcdRegData17[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xCB};
const uint8_t lcdRegData18[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xCB};
const uint8_t lcdRegData19[] = {0x00, 0x26, 0x09, 0x0B, 0x01, 0x25, 0x00, 0x00, 0x00, 0x00, 0xCC};
const uint8_t lcdRegData20[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x26, 0x0A, 0x0C, 0x02, 0xCC};
const uint8_t lcdRegData21[] = {0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xCC};
const uint8_t lcdRegData22[] = {0x00, 0x25, 0x0C, 0x0A, 0x02, 0x26, 0x00, 0x00, 0x00, 0x00, 0xCC};
const uint8_t lcdRegData23[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x25, 0x0B, 0x09, 0x01, 0xCC};
const uint8_t lcdRegData24[] = {0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xCC};
const uint8_t lcdRegData25[] = {0xFF, 0xFF, 0xFF, 0xFF};
/*
  * CASET value (Column Address Set) : X direction LCD GRAM boundaries
  * depending on LCD orientation mode and PASET value (Page Address Set) : Y direction
  * LCD GRAM boundaries depending on LCD orientation mode
  * XS[15:0] = 0x000 = 0, XE[15:0] = 0x31F = 799 for landscape mode : apply to CASET
  * YS[15:0] = 0x000 = 0, YE[15:0] = 0x31F = 799 for portrait mode : : apply to PASET
  */
const uint8_t lcdRegData27[] = {0x00, 0x00, 0x03, 0x1F, OTM8009A_CMD_CASET};
/*
  * XS[15:0] = 0x000 = 0, XE[15:0] = 0x1DF = 479 for portrait mode : apply to CASET
  * YS[15:0] = 0x000 = 0, YE[15:0] = 0x1DF = 479 for landscape mode : apply to PASET
 */
const uint8_t lcdRegData28[] = {0x00, 0x00, 0x01, 0xDF, OTM8009A_CMD_PASET};


const uint8_t ShortRegData1[]  = {OTM8009A_CMD_NOP, 0x00};
const uint8_t ShortRegData2[]  = {OTM8009A_CMD_NOP, 0x80};
const uint8_t ShortRegData3[]  = {0xC4, 0x30};
const uint8_t ShortRegData4[]  = {OTM8009A_CMD_NOP, 0x8A};
const uint8_t ShortRegData5[]  = {0xC4, 0x40};
const uint8_t ShortRegData6[]  = {OTM8009A_CMD_NOP, 0xB1};
const uint8_t ShortRegData7[]  = {0xC5, 0xA9};
const uint8_t ShortRegData8[]  = {OTM8009A_CMD_NOP, 0x91};
const uint8_t ShortRegData9[]  = {0xC5, 0x34};
const uint8_t ShortRegData10[] = {OTM8009A_CMD_NOP, 0xB4};
const uint8_t ShortRegData11[] = {0xC0, 0x50};
const uint8_t ShortRegData12[] = {0xD9, 0x4E};
const uint8_t ShortRegData13[] = {OTM8009A_CMD_NOP, 0x81};
const uint8_t ShortRegData14[] = {0xC1, 0x66};
const uint8_t ShortRegData15[] = {OTM8009A_CMD_NOP, 0xA1};
const uint8_t ShortRegData16[] = {0xC1, 0x08};
const uint8_t ShortRegData17[] = {OTM8009A_CMD_NOP, 0x92};
const uint8_t ShortRegData18[] = {0xC5, 0x01};
const uint8_t ShortRegData19[] = {OTM8009A_CMD_NOP, 0x95};
const uint8_t ShortRegData20[] = {OTM8009A_CMD_NOP, 0x94};
const uint8_t ShortRegData21[] = {0xC5, 0x33};
const uint8_t ShortRegData22[] = {OTM8009A_CMD_NOP, 0xA3};
const uint8_t ShortRegData23[] = {0xC0, 0x1B};
const uint8_t ShortRegData24[] = {OTM8009A_CMD_NOP, 0x82};
const uint8_t ShortRegData25[] = {0xC5, 0x83};
const uint8_t ShortRegData26[] = {0xC4, 0x83};
const uint8_t ShortRegData27[] = {0xC1, 0x0E};
const uint8_t ShortRegData28[] = {OTM8009A_CMD_NOP, 0xA6};
const uint8_t ShortRegData29[] = {OTM8009A_CMD_NOP, 0xA0};
const uint8_t ShortRegData30[] = {OTM8009A_CMD_NOP, 0xB0};
const uint8_t ShortRegData31[] = {OTM8009A_CMD_NOP, 0xC0};
const uint8_t ShortRegData32[] = {OTM8009A_CMD_NOP, 0xD0};
const uint8_t ShortRegData33[] = {OTM8009A_CMD_NOP, 0x90};
const uint8_t ShortRegData34[] = {OTM8009A_CMD_NOP, 0xE0};
const uint8_t ShortRegData35[] = {OTM8009A_CMD_NOP, 0xF0};
const uint8_t ShortRegData36[] = {OTM8009A_CMD_SLPOUT, 0x00};
const uint8_t ShortRegData37[] = {OTM8009A_CMD_COLMOD, OTM8009A_COLMOD_RGB565};
const uint8_t ShortRegData38[] = {OTM8009A_CMD_COLMOD, OTM8009A_COLMOD_RGB888};
const uint8_t ShortRegData39[] = {OTM8009A_CMD_MADCTR, OTM8009A_MADCTR_MODE_LANDSCAPE};
const uint8_t ShortRegData40[] = {OTM8009A_CMD_WRDISBV, 0x7F};
const uint8_t ShortRegData41[] = {OTM8009A_CMD_WRCTRLD, 0x2C};
const uint8_t ShortRegData42[] = {OTM8009A_CMD_WRCABC, 0x02};
const uint8_t ShortRegData43[] = {OTM8009A_CMD_WRCABCMB, 0xFF};
const uint8_t ShortRegData44[] = {OTM8009A_CMD_DISPON, 0x00};
const uint8_t ShortRegData45[] = {OTM8009A_CMD_RAMWR, 0x00};
const uint8_t ShortRegData46[] = {0xCF, 0x00};
const uint8_t ShortRegData47[] = {0xC5, 0x66};
const uint8_t ShortRegData48[] = {OTM8009A_CMD_NOP, 0xB6};
const uint8_t ShortRegData49[] = {0xF5, 0x06};
const uint8_t ShortRegData50[] = {OTM8009A_CMD_NOP, 0xB1};
const uint8_t ShortRegData51[] = {0xC6, 0x05};
/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/** @defgroup OTM8009A_Exported_Variables
  * @{
  */

/**
  * @}
  */

/* Exported functions ---------------------------------------------------------*/
/** @defgroup OTM8009A_Exported_Functions OTM8009A Exported Functions
  * @{
  */

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


void OTM8009A_IO_Delay(uint32_t Delay)
{
    rt_thread_mdelay(Delay);
}


/**
  * @brief  Initializes the LCD KoD display part by communication in DSI mode in Video Mode
  *         with IC Display Driver OTM8009A (see IC Driver specification for more information).
  * @param  hdsi_eval : pointer on DSI configuration structure
  * @param  hdsivideo_handle : pointer on DSI video mode configuration structure
  * @retval Status
  */
uint8_t OTM8009A_Init(uint32_t ColorCoding, uint32_t orientation)
{

    LCDC_InitTypeDef lcdc_init = {0};
    lcdc_init.LCDC_Interface = LCDC_IF_eDPI;
    lcdc_init.LCDC_PixelInputFarmat = LCDC_INPUT_RGB565;
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
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    LCDC_eDPICfgTypeDef eDPICfg;//836 * 550, 42FPS == 19311600
    eDPICfg.eDPI_ClockDiv1 = 1;
    eDPICfg.eDPI_ClockDiv2 = 8;
    eDPICfg.eDPI_ClockDiv3 = 1;
    uint32_t laneByteClk_kHz = 50000;
    uint32_t LcdClock_kHz  = (80000 / 8);

    eDPICfg.eDPI_HoriSyncWidth = OTM8009A_LCD_HSYNC;
    eDPICfg.eDPI_VeriSyncHeight = OTM8009A_LCD_VSYNC;
    eDPICfg.eDPI_AccumulatedHBP = OTM8009A_LCD_HSYNC + OTM8009A_LCD_HBP;
    eDPICfg.eDPI_AccumulatedVBP = OTM8009A_LCD_VSYNC + OTM8009A_LCD_VBP;
    eDPICfg.eDPI_AccumulatedActiveW = OTM8009A_LCD_HSYNC + OTM8009A_LCD_HBP + OTM8009A_LCD_WIDTH;
    eDPICfg.eDPI_AccumulatedActiveH = OTM8009A_LCD_VSYNC + OTM8009A_LCD_VBP + OTM8009A_LCD_HEIGHT;
    eDPICfg.eDPI_TotalWidth = OTM8009A_LCD_WIDTH + OTM8009A_LCD_HSYNC + OTM8009A_LCD_HBP +
                              OTM8009A_LCD_HFP;
    eDPICfg.eDPI_TotalHeight = OTM8009A_LCD_HEIGHT + OTM8009A_LCD_VSYNC + OTM8009A_LCD_VBP +
                               OTM8009A_LCD_VFP;
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


    uint32_t HSA = OTM8009A_LCD_HSYNC, HFP = OTM8009A_LCD_HFP, HBP = OTM8009A_LCD_HBP,
             HACT = OTM8009A_LCD_WIDTH;
    uint32_t VSA = OTM8009A_LCD_VSYNC, VFP = OTM8009A_LCD_VFP, VBP = OTM8009A_LCD_VBP,
             VACT = OTM8009A_LCD_HEIGHT;

    LCDC_LCD_SET_RST(false);
    rt_thread_mdelay(100);
    LCDC_LCD_SET_RST(true);
    rt_thread_mdelay(50);
    LCDC_LCD_SET_RST(false);
    rt_thread_mdelay(50);//todo

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
    DSI_InitStruct.DataLaneLP2HSTime = 0xA1;

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
    VideoCmd_InitStruct.HorizontalSyncActive = OTM8009A_LCD_HSYNC * laneByteClk_kHz / LcdClock_kHz;
    VideoCmd_InitStruct.HorizontalBackPorch = OTM8009A_LCD_HBP * laneByteClk_kHz / LcdClock_kHz;
    VideoCmd_InitStruct.HorizontalLine = (OTM8009A_LCD_WIDTH + OTM8009A_LCD_HSYNC + OTM8009A_LCD_HBP +
                                          OTM8009A_LCD_HFP) * laneByteClk_kHz / LcdClock_kHz;


    VideoCmd_InitStruct.VerticalSyncActive = OTM8009A_LCD_VSYNC;
    VideoCmd_InitStruct.VerticalBackPorch = OTM8009A_LCD_VBP;
    VideoCmd_InitStruct.VerticalFrontPorch = OTM8009A_LCD_VFP;
    VideoCmd_InitStruct.VerticalActive = OTM8009A_LCD_HEIGHT;

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

void otm8009a_video_mode_init(uint32_t ColorCoding, uint32_t orientation)
{
    /* Enable CMD2 to access vendor specific commands                               */
    /* Enter in command 2 mode and set EXTC to enable address shift function (0x00) */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData1);
    DSI_IO_WriteCmd(3, (uint8_t *)lcdRegData1);

    /* Enter ORISE Command 2 */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData2); /* Shift address to 0x80 */
    DSI_IO_WriteCmd(2, (uint8_t *)lcdRegData2);

    /////////////////////////////////////////////////////////////////////
    /* SD_PCH_CTRL - 0xC480h - 129th parameter - Default 0x00          */
    /* Set SD_PT                                                       */
    /* -> Source output level during porch and non-display area to GND */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData2);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData3);
    OTM8009A_IO_Delay(10);
    /* Not documented */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData4);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData5);
    OTM8009A_IO_Delay(10);
    /////////////////////////////////////////////////////////////////////

    /* PWR_CTRL4 - 0xC4B0h - 178th parameter - Default 0xA8 */
    /* Set gvdd_en_test                                     */
    /* -> enable GVDD test mode !!!                         */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData6);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData7);

    /* PWR_CTRL2 - 0xC590h - 146th parameter - Default 0x79      */
    /* Set pump 4 vgh voltage                                    */
    /* -> from 15.0v down to 13.0v                               */
    /* Set pump 5 vgh voltage                                    */
    /* -> from -12.0v downto -9.0v                               */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData8);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData9);

    /* P_DRV_M - 0xC0B4h - 181th parameter - Default 0x00 */
    /* -> Column inversion                                */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData10);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData11);

    /* VCOMDC - 0xD900h - 1st parameter - Default 0x39h */
    /* VCOM Voltage settings                            */
    /* -> from -1.0000v downto -1.2625v                 */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData1);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData12);

    /* Oscillator adjustment for Idle/Normal mode (LPDT only) set to 65Hz (default is 60Hz) */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData13);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData14);

    /* Video mode internal */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData15);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData16);

    /* PWR_CTRL2 - 0xC590h - 147h parameter - Default 0x00 */
    /* Set pump 4&5 x6                                     */
    /* -> ONLY VALID when PUMP4_EN_ASDM_HV = "0"           */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData17);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData18);

    /* PWR_CTRL2 - 0xC590h - 150th parameter - Default 0x33h */
    /* Change pump4 clock ratio                              */
    /* -> from 1 line to 1/2 line                            */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData19);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData9);

    /* GVDD/NGVDD settings */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData1);
    DSI_IO_WriteCmd(2, (uint8_t *)lcdRegData5);

    /* PWR_CTRL2 - 0xC590h - 149th parameter - Default 0x33h */
    /* Rewrite the default value !                           */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData20);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData21);

    /* Panel display timing Setting 3 */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData22);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData23);

    /* Power control 1 */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData24);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData25);

    /* Source driver precharge */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData13);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData26);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData15);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData27);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData28);
    DSI_IO_WriteCmd(2, (uint8_t *)lcdRegData6);

    /* GOAVST */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData2);
    DSI_IO_WriteCmd(6, (uint8_t *)lcdRegData7);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData29);
    DSI_IO_WriteCmd(14, (uint8_t *)lcdRegData8);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData30);
    DSI_IO_WriteCmd(14, (uint8_t *)lcdRegData9);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData31);
    DSI_IO_WriteCmd(10, (uint8_t *)lcdRegData10);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData32);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData46);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData2);
    DSI_IO_WriteCmd(10, (uint8_t *)lcdRegData11);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData33);
    DSI_IO_WriteCmd(15, (uint8_t *)lcdRegData12);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData29);
    DSI_IO_WriteCmd(15, (uint8_t *)lcdRegData13);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData30);
    DSI_IO_WriteCmd(10, (uint8_t *)lcdRegData14);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData31);
    DSI_IO_WriteCmd(15, (uint8_t *)lcdRegData15);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData32);
    DSI_IO_WriteCmd(15, (uint8_t *)lcdRegData16);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData34);
    DSI_IO_WriteCmd(10, (uint8_t *)lcdRegData17);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData35);
    DSI_IO_WriteCmd(10, (uint8_t *)lcdRegData18);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData2);
    DSI_IO_WriteCmd(10, (uint8_t *)lcdRegData19);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData33);
    DSI_IO_WriteCmd(15, (uint8_t *)lcdRegData20);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData29);
    DSI_IO_WriteCmd(15, (uint8_t *)lcdRegData21);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData30);
    DSI_IO_WriteCmd(10, (uint8_t *)lcdRegData22);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData31);
    DSI_IO_WriteCmd(15, (uint8_t *)lcdRegData23);

    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData32);
    DSI_IO_WriteCmd(15, (uint8_t *)lcdRegData24);

    /////////////////////////////////////////////////////////////////////////////
    /* PWR_CTRL1 - 0xc580h - 130th parameter - default 0x00 */
    /* Pump 1 min and max DM                                */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData13);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData47);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData48);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData49);
    /////////////////////////////////////////////////////////////////////////////

    /* CABC LEDPWM frequency adjusted to 22,7kHz */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData50);
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData51);

    /* Exit CMD2 mode */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData1);
    DSI_IO_WriteCmd(3, (uint8_t *)lcdRegData25);

    /*************************************************************************** */
    /* Standard DCS Initialization TO KEEP CAN BE DONE IN HSDT                   */
    /*************************************************************************** */

    /* NOP - goes back to DCS std command ? */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData1);

    /* Gamma correction 2.2+ table (HSDT possible) */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData1);
    DSI_IO_WriteCmd(16, (uint8_t *)lcdRegData3);

    /* Gamma correction 2.2- table (HSDT possible) */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData1);
    DSI_IO_WriteCmd(16, (uint8_t *)lcdRegData4);

    /* Send Sleep Out command to display : no parameter */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData36);

    /* Wait for sleep out exit */
    OTM8009A_IO_Delay(120);

    switch (ColorCoding)
    {
    case OTM8009A_FORMAT_RBG565 :
        /* Set Pixel color format to RGB565 */
        rt_kprintf("line = %d\n", __LINE__);
        DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData37);
        break;
    case OTM8009A_FORMAT_RGB888 :
        /* Set Pixel color format to RGB888 */
        rt_kprintf("line = %d\n", __LINE__);
        DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData38);
        break;
    default :
        break;
    }

    /* Send command to configure display in landscape orientation mode. By default
        the orientation mode is portrait  */
    if (orientation == OTM8009A_ORIENTATION_LANDSCAPE)
    {
        rt_kprintf("line = %d\n", __LINE__);
        DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData39);
        DSI_IO_WriteCmd(4, (uint8_t *)lcdRegData27);
        DSI_IO_WriteCmd(4, (uint8_t *)lcdRegData28);
    }

    /** CABC : Content Adaptive Backlight Control section start >> */
    /* Note : defaut is 0 (lowest Brightness), 0xFF is highest Brightness, try 0x7F : intermediate value */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData40);

    /* defaut is 0, try 0x2C - Brightness Control Block, Display Dimming & BackLight on */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData41);

    /* defaut is 0, try 0x02 - image Content based Adaptive Brightness [Still Picture] */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData42);

    /* defaut is 0 (lowest Brightness), 0xFF is highest Brightness */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData43);

    /** CABC : Content Adaptive Backlight Control section end << */

    /* Send Command Display On */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData44);

    /* NOP command */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData1);

    /* Send Command GRAM memory write (no parameters) : this initiates frame write via other DSI commands sent by */
    /* DSI host from LTDC incoming pixels in video mode */
    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData45);
}

static uint32_t framebuffer = 0;
void lcd_otm8009a_mipi_video_update_framebuffer(uint8_t *buf, uint32_t len)
{
    framebuffer = (uint32_t)buf;
}
void lcd_otm8009a_mipi_video_init_framebuffer(uint8_t *buf, uint32_t len)
{
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

    LCDC_ClearDmaFifo();
    LCDC_ClearTxPixelCnt();

    LCDC_SwitchMode(LCDC_AUTO_MODE);
    LCDC_SwitchDirect(LCDC_TX_MODE);


    LCDC_SetTxPixelLen(len);

    LCDC_Cmd(ENABLE);


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
