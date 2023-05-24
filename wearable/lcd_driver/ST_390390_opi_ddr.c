#include "board.h"
#include "ST_390390_opi_ddr.h"
#include "rtl_lcdc_dbic.h"
#include "rtthread.h"

#define LCDC_DMA_CHANNEL_NUM              0
#define LCDC_DMA_CHANNEL_INDEX            LCDC_DMA_Channel0

#define BIT_CMD_CH(x)           (((x) & 0x00000003) << 20)
#define BIT_DATA_CH(x)          (((x) & 0x00000003) << 18)
#define BIT_ADDR_CH(x)          (((x) & 0x00000003) << 16)
#define BIT_TMOD(x)             (((x) & 0x00000003) << 8)
#define BIT_DDR              ((0x00000002) << 13)
#define BIT_TXSIM               (0x00000001 << 9)
#define BIT_SEQ_EN              (0x00000001 << 3)





static void spic3_spi_write(uint8_t *buf, uint32_t len)
{

    DBIC->CTRLR0 &= ~(BIT_CMD_CH(3) | BIT_ADDR_CH(3) | BIT_DATA_CH(3));//SET CHANNEL NUM
    DBIC->CTRLR0 &= ~(BIT_TMOD(3)); //tx mode
    DBIC->CTRLR0 = 0x8100001F;
    DBIC_CmdLength(1);
    DBIC_AddrLength(3);
    DBIC_TX_NDF(len - 4);
    DBIC_Cmd(ENABLE);
    for (uint32_t i = 0; i < len; i++)
    {
        DBIC->DR[0].byte = buf[i];
    }
    while (DBIC->SR & BIT0); // wait bus busy
    DBIC_Cmd(DISABLE);;//disable DBIC
};

static void st_opi_Reg_Write(const uint8_t *data, uint32_t length, uint32_t mdelay)
{
    spic3_spi_write((uint8_t *)data, length);
    rt_thread_mdelay(mdelay);
}

static void rtl_st_opi_write_cmd(uint8_t cmd)//total 4 byte, first byte is 0x02
{
    uint8_t sdat[4] = {0x02, 0x00, cmd, 0x00};
    spic3_spi_write(sdat, 4);
}
static void rtl_st_opi_cmd_param(uint8_t cmd, uint8_t data) //total 5 byte, first byte is 0x02
{
    uint8_t sdat[5] = {0x02, 0x00, cmd, 0x00, data};
    spic3_spi_write(sdat, 5);
}
static void rtl_st_opi_cmd_param2(uint8_t cmd, uint8_t *data) //total 6 byte, first byte is 0x02
{
    uint8_t sdat[6] = {0x02, 0x00, cmd, 0x00, data[0], data[1]};
    spic3_spi_write(sdat, 6);
}
static void rtl_st_opi_cmd_param4(uint8_t cmd, uint8_t *data) //total 8 byte, first byte is 0x02
{
    uint8_t sdat[8] = {0x02, 0x00, cmd, 0x00, data[0], data[1], data[2], data[3]};
    spic3_spi_write(sdat, 8);
}
void rtl_st_opi_enter_data_output_mode(uint32_t len_byte)
{
    LCDC_AXIMUXMode(LCDC_FW_MODE);
    DBIC_SwitchMode(DBIC_USER_MODE);
    DBIC_SwitchDirect(DBIC_TMODE_TX);
    DBIC_CmdLength(1);
    DBIC_AddrLength(3);
    DBIC_TX_NDF(len_byte);

    DBIC->CTRLR0 = 0x810C001F;//0x8108001F 114;0x810A001F 144;0x810C001F 118;0x810F001F 188
    DBIC->CTRLR0 |= BIT_DDR;//set data phase DDR
    LCDC_SPICCmd(0xB2);//0x32 114;0x12 144;0xB2 118;0x82 188
    LCDC_SPICAddr(0x002c00);
    DBIC->DMACR = 2;

    /* change this value can not influence the result. the wave is split into two periods. the first is 32 bytes. */
    DBIC->DMATDLR = 4; /* no any influence. */

    LCDC_AXIMUXMode(LCDC_HW_MODE);
}

void lcd_st_opi_390_power_on(void)
{
    rtl_st_opi_write_cmd(0x11);
    rtl_st_opi_write_cmd(0x29);
    rtl_st_opi_cmd_param(0x3A, 0x77);
#if 1 //for DDR
    rtl_st_opi_cmd_param(0x40, 0x02);
    uint8_t data[2] = {0x07, 0x04};
    rtl_st_opi_cmd_param2(0x62, data);
#endif
}
void lcd_st_opi_390_power_off(void)
{
    rtl_st_opi_write_cmd(0x28);
    rtl_st_opi_write_cmd(0x10);
}
void lcd_st_opi_390_set_window(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd)
{
    uint8_t data[4];
    xStart = xStart + 0;
    xEnd = xEnd + 0;
    yStart = yStart + 0;
    yEnd = yEnd + 0;
    data[0] = xStart >> 8;
    data[1] = xStart & 0xff;
    data[2] = xEnd >> 8;
    data[3] = xEnd & 0xff;
    rtl_st_opi_cmd_param4(0x2A, data);
    data[0] = yStart >> 8;
    data[1] = yStart & 0xff;
    data[2] = yEnd >> 8;
    data[3] = yEnd & 0xff;
    rtl_st_opi_cmd_param4(0x2B, data);
    uint32_t len_byte = (xEnd - xStart + 1) * (yEnd - yStart + 1) * OUTPUT_PIXEL_BYTES;
    rtl_st_opi_enter_data_output_mode(len_byte);
}
void lcd_st_opi_update_framebuffer(uint8_t *buf, uint32_t len)
{
#if (DMA_LINKLIST == 0)
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
    LCDC_DMA_InitStruct.LCDC_DMA_SourceAddr          = (uint32_t)buf;
    LCDC_DMA_InitStruct.LCDC_DMA_Multi_Block_En     = 0;
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
    LCDC_DMA_InitStruct.LCDC_DMA_SourceMsize         = DMA_Msize_8;
    LCDC_DMA_InitStruct.LCDC_DMA_DestinationMsize    = DMA_Msize_8;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceAddr          = 0;

    LCDC_DMA_InitStruct.LCDC_DMA_Multi_Block_Mode   =
        LLI_TRANSFER;//LLI_TRANSFER or LLI_WITH_CONTIGUOUS_SAR
    LCDC_DMA_InitStruct.LCDC_DMA_Multi_Block_En     = 1;
    LCDC_DMA_InitStruct.LCDC_DMA_Multi_Block_Struct  = LCDC_DMA_LINKLIST_REG_BASE + 0x50;
    LCDC_DMA_Init(LCDC_DMA_CHANNEL_INDEX, &LCDC_DMA_InitStruct);

    LCDC_SET_GROUP1_BLOCKSIZE(STOPI_LCD_WIDTH * INPUT_PIXEL_BYTES);
    LCDC_SET_GROUP2_BLOCKSIZE(STOPI_LCD_WIDTH * INPUT_PIXEL_BYTES);

    /*16 pixel aligned for GPU*/
    uint32_t gpu_width = ((STOPI_LCD_WIDTH + 15) >> 4) << 4;
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

    while ((LCDC_HANDLER->DMA_FIFO_CTRL & LCDC_DMA_ENABLE_MSK) != RESET);
    while ((LCDC_HANDLER->OPERATE_CTR & LCDC_AUTO_WRITE_START_MSK) != RESET);

    LCDC_ClearDmaFifo();
    LCDC_ClearTxPixelCnt();
    LCDC_AXIMUXMode(LCDC_FW_MODE);
    LCDC_Cmd(DISABLE);

}
void lcd_st_opi_390_init(void)
{
    LCDC_InitTypeDef lcdc_init = {0};
    lcdc_init.LCDC_Interface = LCDC_IF_DBIC;
    lcdc_init.LCDC_PixelInputFarmat = LCDC_INPUT_ABGR888;
    lcdc_init.LCDC_PixelOutpuFarmat = LCDC_OUTPUT_RGB888;
    lcdc_init.LCDC_PixelBitSwap = LCDC_SWAP_BYPASS; //lcdc_handler_cfg->LCDC_TeEn = LCDC_TE_DISABLE;
#if TE_VALID
    lcdc_init.LCDC_TeEn = LCDC_TE_DISABLE;
    lcdc_init.LCDC_TePolarity = LCDC_TE_EDGE_FALLING;
    lcdc_init.LCDC_TeInputMux = LCDC_TE_LCD_INPUT;
#endif
    lcdc_init.LCDC_DmaThreshold =
        8;    //only support threshold = 8 for DMA MSIZE = 8; the other threshold setting will be support later
    LCDC_Init(&lcdc_init);


    LCDC_DBICCfgTypeDef dbic_init = {0};
    dbic_init.DBIC_SPEED_SEL         = 16;
    dbic_init.DBIC_TxThr             = 0;//0 or 4
    dbic_init.DBIC_RxThr             = 0;
    dbic_init.SCPOL                  = DBIC_SCPOL_LOW;
    dbic_init.SCPH                   = DBIC_SCPH_1Edge;
    DBIC_Init(&dbic_init);

    LCDC_SwitchMode(LCDC_MANUAL_MODE);
    LCDC_SwitchDirect(LCDC_TX_MODE);
    LCDC_Cmd(ENABLE);

    LCDC_LCD_SET_RST(false);
    rt_thread_mdelay(100);
    LCDC_LCD_SET_RST(true);
    rt_thread_mdelay(50);
    LCDC_LCD_SET_RST(false);
    rt_thread_mdelay(50);

    LCDC_AXIMUXMode(LCDC_FW_MODE);
    DBIC_SwitchMode(DBIC_USER_MODE);
    DBIC_SwitchDirect(DBIC_TMODE_TX);
    lcd_st_opi_390_power_on();
}





