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
#include "otm8009a_480800_mipi_cmd.h"
#include "rtl_lcdc.h"
#include "rtl_lcdc_dsi.h"
#include "rtl_lcdc_edpi.h"

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

__weak void DSI_IO_PixelWriteCmd(uint32_t NbrParams, uint8_t *pParams)
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
    lcdc_init.LCDC_PixelInputFarmat = LCDC_INPUT_ARGB8888;
    lcdc_init.LCDC_PixelOutpuFarmat = LCDC_OUTPUT_RGB888;
    lcdc_init.LCDC_PixelBitSwap = LCDC_SWAP_BYPASS; //lcdc_handler_cfg->LCDC_TeEn = LCDC_TE_DISABLE;

    lcdc_init.LCDC_DmaThreshold =
        8;    //only support threshold = 8 for DMA MSIZE = 8; the other threshold setting will be support later
    LCDC_Init(&lcdc_init);

    LCDC_eDPICfgTypeDef eDPICfg;//836 * 550, 42FPS == 19311600
    eDPICfg.eDPI_ClockDiv = 4;
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

    DSI_InitStruct.ColorCoding                  = 0;
    DSI_InitStruct.DEPolarity                   = 0;
    DSI_InitStruct.HSPolarity                   = 0;
    DSI_InitStruct.VSPolarity                   = 0;

    DSI_InitStruct.phy_mode = 0x0;  //fix
    DSI_InitStruct.NumberOfLanes = 2;
    DSI_InitStruct.StopWaitTime = 0;    //skip
    DSI_InitStruct.InterruptMask0 = 0x1FFFFF;
    DSI_InitStruct.InterruptMask1 = 0x1FFFFF;
    DSI_InitStruct.HS_TX_CNT =
        0xFFFF;                 //for trigger timeout error measured in TO_CLK_DIVISION
    DSI_InitStruct.LP_RX_CNT =
        0xFFFF;                 //for trigger timeout error measured in TO_CLK_DIVISION
    DSI_InitStruct.TOCkdiv = 0x4;
    DSI_InitStruct.TXEscapeCkdiv = 0x05;
    DSI_InitStruct.ModeCfg = 1;                   //config for video mode or cmd mode
    DSI_InitStruct.CommandSize                  = 0x256;
    DSI_InitStruct.PeripherCharacteristics = 0;
    DSI_InitStruct.GenericPacketVCID = 0;
    DSI_InitStruct.GenericTearVCID = 0;
    DSI_InitStruct.GenericReadBackVCID = 0;
    DSI_InitStruct.AutoHS2LPEnable = 1;
    DSI_InitStruct.TxRequestCLKEnable = 1;
    DSI_InitStruct.ClockLaneHS2LPTime = 35;
    DSI_InitStruct.ClockLaneLP2HSTime = 35;
    DSI_InitStruct.DataLaneHS2LPTime = 35;
    DSI_InitStruct.DataLaneLP2HSTime = 35;
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
    DSI_InitStruct.LPGenLongWrite               =   DSI_LP_GLW_ENABLE;
    DSI_InitStruct.LPDcsShortWriteNoP           = DSI_LP_DSW0P_ENABLE;
    DSI_InitStruct.LPDcsShortWriteOneP          = DSI_LP_DSW1P_ENABLE;
    DSI_InitStruct.LPDcsShortReadNoP            = DSI_LP_DSR0P_ENABLE;
    DSI_InitStruct.LPDcsLongWrite               =   DSI_LP_DLW_ENABLE;
    DSI_Init(DSI, &DSI_InitStruct);

    DSI_AdaptedCmdCfgTypeDef   AdaptedCmd_InitStruct;

    AdaptedCmd_InitStruct.TEAcknowledgeRequest         = DSI_TE_ACKNOWLEDGE_DISABLE;
    AdaptedCmd_InitStruct.hw_tear_effect_gen       = 0;
    AdaptedCmd_InitStruct.scan_line_parameter   = 0;
    AdaptedCmd_InitStruct.hw_set_scan_line    = 0;
    AdaptedCmd_InitStruct.hw_tear_effect_on   = 0;

    DSI_ConfigAdaptedCommandMode(DSI, &AdaptedCmd_InitStruct);


    DSI_PhyCfgTypeDef dsi_phy;
    dsi_phy.phy_forepll  = PHY_FORCEPLL_DISABLE;
    dsi_phy.phy_enableclk  = PHY_CLK_ENABLE;
    dsi_phy.phy_rstz   = PHY_RST_ENABLE;
    dsi_phy.phy_shutdownz   = PHY_SHUTDOWN_ENABLE;
    DSI_ConfigPhy(DSI, &dsi_phy);

    DSI_Start(DSI);
#include "otm8009a_init_cmd.txt"

#if 0
    DSI_Init(DSI, &DSI_InitStruct);
    AdaptedCmd_InitStruct.LPGenShortWriteNoP           = DSI_LP_GSW0P_ENABLE;
    AdaptedCmd_InitStruct.LPGenShortWriteOneP          = DSI_LP_GSW1P_ENABLE;
    AdaptedCmd_InitStruct.LPGenShortWriteTwoP          = DSI_LP_GSW2P_ENABLE;
    AdaptedCmd_InitStruct.LPGenShortReadNoP            = DSI_LP_GSR0P_ENABLE;
    AdaptedCmd_InitStruct.LPGenShortReadOneP           = DSI_LP_GSR1P_ENABLE;
    AdaptedCmd_InitStruct.LPGenShortReadTwoP           = DSI_LP_GSR2P_ENABLE;
    AdaptedCmd_InitStruct.LPGenLongWrite               =   DSI_LP_GLW_ENABLE;
    AdaptedCmd_InitStruct.LPDcsShortWriteNoP           = DSI_LP_DSW0P_ENABLE;
    AdaptedCmd_InitStruct.LPDcsShortWriteOneP          = DSI_LP_DSW1P_ENABLE;
    AdaptedCmd_InitStruct.LPDcsShortReadNoP            = DSI_LP_DSR0P_ENABLE;
    AdaptedCmd_InitStruct.LPDcsLongWrite               =   DSI_LP_DLW_ENABLE;
    DSI_ConfigAdaptedCommandMode(DSI, &AdaptedCmd_InitStruct);
    DSI_ConfigPhy(DSI, &dsi_phy);

    DSI_Start(DSI);
#else
    *(uint32_t *)(0x40602000 + 0x68) &= ~BIT19;
#endif
    return 0;
}


void lcd_ota8009a_mipi_cmd_set_window(uint16_t xStart, uint16_t yStart, uint16_t xEnd,
                                      uint16_t yEnd)
{
//    uint8_t lcdRegData27[] = {xStart >> 8, xStart & 0xff, xEnd >> 8, xEnd & 0xff, 0x2A};
//    uint8_t lcdRegData28[] = {yStart >> 8, yStart & 0xff, yEnd >> 8, yEnd & 0xff, 0x2B};
//    uint8_t ShortRegData45[] = {0x2C, 0x00};
//    DSI_IO_WriteCmd(4, (uint8_t *)lcdRegData27);
//    DSI_IO_WriteCmd(4, (uint8_t *)lcdRegData28);
//    DSI_IO_WriteCmd(0, (uint8_t *)ShortRegData45);
}
void lcd_ota8009a_update_framebuffer(uint8_t *buf, uint32_t len)
{
    LCDC_DMA_InitTypeDef LCDC_DMA_InitStruct = {0};
    LCDC_DMA_StructInit(&LCDC_DMA_InitStruct);
    LCDC_DMA_InitStruct.LCDC_DMA_ChannelNum          = LCDC_DMA_CHANNEL_NUM;
    LCDC_DMA_InitStruct.LCDC_DMA_DIR                 = DMA_DIR_PeripheralToMemory;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceInc           = DMA_SourceInc_Inc;
    LCDC_DMA_InitStruct.LCDC_DMA_DestinationInc      = DMA_DestinationInc_Fix;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceDataSize      = DMA_DataSize_Word;
    LCDC_DMA_InitStruct.LCDC_DMA_DestinationDataSize = DMA_DataSize_Word;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceMsize         = DMA_Msize_8;
    LCDC_DMA_InitStruct.LCDC_DMA_DestinationMsize    = DMA_Msize_8;
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

    while ((LCDC_HANDLER->DMA_FIFO_CTRL & LCDC_DMA_ENABLE_MSK) != RESET);
    while ((LCDC_HANDLER->OPERATE_CTR & LCDC_AUTO_WRITE_START_MSK) != RESET);


    LCDC_ClearDmaFifo();
    LCDC_ClearTxPixelCnt();

    LCDC_Cmd(DISABLE);

}

