#include "rtl876x_dbic.h"
#include "rtl876x_lcdc.h"
#include "utils.h"
#include "ST77903_400400_RLSPI.h"
#include "rtl876x_pinmux.h"
#include "trace.h"
#include "rtl876x_ramless_qspi.h"
#include "rtl876x_nvic.h"

#define MAX_PARAM_COUNT     15

typedef struct
{
    uint8_t instruction;
    uint8_t index;
    uint16_t delay;
    uint16_t wordcount;
    uint8_t  payload[MAX_PARAM_COUNT];
} cmd_struct;

const cmd_struct cmd_table[] =
{
    {0xDE, 0xF0, 1, 1, {0xC3}},
    {0xDE, 0xF0, 1, 1, {0x96}},
    {0xDE, 0xF0, 1, 1, {0xA5}},
    {0xDE, 0xE9, 1, 1, {0x20}},
    {0xDE, 0xE7, 1, 4, {0x80, 0x77, 0x1F, 0xCC}},
    {0xDE, 0xC1, 1, 4, {0x77, 0x07, 0xCF, 0x16}},
    {0xDE, 0xC2, 1, 4, {0x77, 0x07, 0xCF, 0x16}},
    {0xDE, 0xC3, 1, 4, {0x22, 0x02, 0x22, 0x04}},
    {0xDE, 0xC3, 1, 4, {0x22, 0x02, 0x22, 0x04}},
    {0xDE, 0xC5, 1, 1, {0xED}},
    {0xDE, 0xE0, 1, 14, {0x87, 0x09, 0x0C, 0x06, 0x05, 0x03, 0x29, 0x32, 0x49, 0x0F, 0x1B, 0x17, 0x2A, 0x2F}},
    {0xDE, 0xE1, 1, 14, {0x87, 0x09, 0x0C, 0x06, 0x05, 0x03, 0x29, 0x32, 0x49, 0x0F, 0x1B, 0x17, 0x2A, 0x2F}},
    {0xDE, 0xE5, 1, 14, {0xBE, 0xF5, 0xB1, 0x22, 0x22, 0x25, 0x10, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22}},
    {0xDE, 0xE6, 1, 14, {0xBE, 0xF5, 0xB1, 0x22, 0x22, 0x25, 0x10, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22}},
    {0xDE, 0xEC, 1, 2, {0x40, 0x03}},
    {0xDE, 0x36, 1, 1, {0x0C}},
    {0xDE, 0x3A, 1, 1, {0x07}},
    {0xDE, 0xB2, 1, 1, {0x00}},
    {0xDE, 0xB3, 1, 1, {0x01}},
    {0xDE, 0xB4, 1, 1, {0x00}},
    {0xDE, 0xB5, 1, 4, {0x00, 0x08, 0x00, 0x08}},
    {0xDE, 0xA5, 1, 9, {0x00, 0x00, 0x00, 0x00, 0x00, 0x15, 0x2A, 0x8A, 0x02}},
    {0xDE, 0xA6, 1, 9, {0x00, 0x00, 0x00, 0x00, 0x00, 0x15, 0x2A, 0x8A, 0x02}},
    {0xDE, 0xBA, 1, 7, {0x0A, 0x5A, 0x23, 0x10, 0x25, 0x02, 0x00}},
    {0xDE, 0xBB, 1, 8, {0x00, 0x30, 0x00, 0x2C, 0x82, 0x87, 0x17, 0x00}},
    {0xDE, 0xBC, 1, 8, {0x00, 0x30, 0x00, 0x2C, 0x82, 0x87, 0x17, 0x00}},
    {0xDE, 0xBD, 1, 11, {0xA1, 0xB2, 0x2B, 0x1A, 0x56, 0x43, 0x34, 0x65, 0xFF, 0xFF, 0x0F}},
    {0xDE, 0x35, 1, 1, {0x00}},
    {0xDE, 0x21, 1, 0, {}},
    {0xDE, 0x11, 120, 0, {}},
    {0xDE, 0x29, 120, 0, {}},
    {0xDE, 0x2C, 1, 0, {}},
//    {0xDE, 0xB0, 1, 1, {0xA5}},
//    {0xDE, 0xCC, 1, 9, {0x34, 0x00, 0x3F, 0x00, 0x14, 0x14, 0x20, 0x20, 0x03}},
};

static void spic3_spi_write(uint8_t *buf, uint32_t len)
{
    DBIC->CTRLR0 &= ~(BIT_CMD_CH(3) | BIT_ADDR_CH(3) | BIT_DATA_CH(3));//SET CHANNEL NUM
    DBIC->CTRLR0 &= ~(BIT_TMOD(3)); //tx mode

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
}

static void st77903_init_cmds(void)
{
    uint16_t idx = 0;
    uint16_t table_size = sizeof(cmd_table) / sizeof(cmd_struct);
    while (idx < table_size)
    {
//        DBG_DIRECT("[SPIC]try tx cmd No %d with delay %d ms", idx, cmd_table[idx].delay);
        uint8_t sdat[cmd_table[idx].wordcount + 4];

        sdat[0] = cmd_table[idx].instruction;

        sdat[1] = 0;
        sdat[2] = cmd_table[idx].index;        // Set in the middle 8 bits ADDR[15:8] of the 24 bits ADDR[23:0]
        sdat[3] = 0;

        for (uint16_t i = 0; i < cmd_table[idx].wordcount; i++)
        {
            sdat[i + 4] = cmd_table[idx].payload[i];
        }

        spic3_spi_write(sdat, sizeof(sdat));
        if ((cmd_table[idx].delay != 1) && (cmd_table[idx].delay != 0))
        {
            platform_delay_ms(cmd_table[idx].delay);
        }

        idx++;
    }
}

static void ST77903_qspi_enter_data_output_mode(uint32_t
                                                len_byte) //total 4 byte, first byte is 0x32
{
    LCDC_AXIMUXMode(LCDC_FW_MODE);
    DBIC_SwitchMode(DBIC_USER_MODE);
    DBIC_SwitchDirect(DBIC_TMODE_TX);
    DBIC_CmdLength(1);
    DBIC_AddrLength(3);
    DBIC_TX_NDF(len_byte);

    DBIC->CTRLR0 &= ~(BIT_CMD_CH(3) | BIT_ADDR_CH(3) | BIT_DATA_CH(3));//SET CHANNEL NUM
    DBIC->CTRLR0 |= (BIT_CMD_CH(0) | BIT_ADDR_CH(0) | BIT_DATA_CH(2));

    DBIC->DMACR = 2;

    /* change this value can not influence the result. the wave is split into two periods. the first is 32 bytes. */
    DBIC->DMATDLR = 4; /* no any influence. */

    LCDC_AXIMUXMode(LCDC_HW_MODE);
}

static uint32_t framebuffer = 0;
void st77903_update_framebuffer(uint8_t *buf, uint32_t line_bytes)
{
    LCDC_DMA_Infinite_Buf_Update(buf, buf + ST77903_WIDTH * PIXEL_BYTES);
}

void st77903_framebuffer_init(uint8_t *buf, uint32_t len)
{
    ST77903_qspi_enter_data_output_mode(len * 3);
#if DMA_MULTIBLOCK
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

    LCDC_SET_GROUP1_BLOCKSIZE(ST77903_WIDTH * PIXEL_BYTES);
    LCDC_SET_GROUP2_BLOCKSIZE(ST77903_WIDTH * PIXEL_BYTES);

    LCDC_DMALLI_InitTypeDef LCDC_DMA_LLI_Init = {0};
    LCDC_DMA_LLI_Init.g1_source_addr = (uint32_t)buf;
    LCDC_DMA_LLI_Init.g1_source_addr_infinite_mode = (uint32_t)buf;
    LCDC_DMA_LLI_Init.g1_sar_offset = ST77903_WIDTH * PIXEL_BYTES * 2;

    LCDC_DMA_LLI_Init.g2_source_addr = (uint32_t)(buf + ST77903_WIDTH * PIXEL_BYTES);
    LCDC_DMA_LLI_Init.g2_source_addr_infinite_mode = (uint32_t)(buf + ST77903_WIDTH * PIXEL_BYTES);
    LCDC_DMA_LLI_Init.g2_sar_offset = ST77903_WIDTH * PIXEL_BYTES * 2;
    LCDC_DMA_LinkList_Init(&LCDC_DMA_LLI_Init,
                           &LCDC_DMA_InitStruct);//LLI_TRANSFER or LLI_WITH_CONTIGUOUS_SAR
#else
    LCDC_DMA_InitTypeDef LCDC_DMA_InitStruct = {0};
    LCDC_DMA_StructInit(&LCDC_DMA_InitStruct);
    LCDC_DMA_InitStruct.LCDC_DMA_ChannelNum          = LCDC_DMA_CHANNEL_NUM;
    LCDC_DMA_InitStruct.LCDC_DMA_DIR                 = DMA_DIR_PeripheralToMemory;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceInc           = DMA_SourceInc_Fix;
    LCDC_DMA_InitStruct.LCDC_DMA_DestinationInc      = DMA_DestinationInc_Fix;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceDataSize      = DMA_DataSize_Word;
    LCDC_DMA_InitStruct.LCDC_DMA_DestinationDataSize = DMA_DataSize_Word;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceMsize         = DMA_Msize_64;
    LCDC_DMA_InitStruct.LCDC_DMA_DestinationMsize    = DMA_Msize_64;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceAddr          = 0x00500000;
    LCDC_DMA_InitStruct.LCDC_DMA_Multi_Block_En      = 0;
    LCDC_DMA_Init(LCDC_DMA_CHANNEL_INDEX, &LCDC_DMA_InitStruct);
#endif
    LCDC_ClearDmaFifo();
    LCDC_ClearTxPixelCnt();

    LCDC_SwitchMode(LCDC_AUTO_MODE);
    LCDC_SwitchDirect(LCDC_TX_MODE);


    LCDC_SetTxPixelLen(len);

    LCDC_Cmd(ENABLE);


    LCDC_DMA_SetSourceAddress(LCDC_DMA_CHANNEL_INDEX, (uint32_t)buf);

#if DMA_MULTIBLOCK
    LCDC_DMA_MultiBlockCmd(ENABLE);
#endif
    LCDC_DMAChannelCmd(LCDC_DMA_CHANNEL_NUM, ENABLE);
    LCDC_DmaCmd(ENABLE);
    LCDC_AutoWriteCmd(ENABLE);
}

void st77903_screen_init(void)
{
    LCDC_RLSPI_initTypeDef rlspi_init_struct;
    rlspi_init_struct.VSA = ST77903_VSYNC;
    rlspi_init_struct.VBP = ST77903_VBP;
    rlspi_init_struct.VFP = ST77903_VFP;
    rlspi_init_struct.VSYNC_CMD = ST77903_VSYNC_CMD;
    rlspi_init_struct.VSYNC_CMD_ADDR = ST77903_VSYNC_ADDR;
    rlspi_init_struct.HSYNC_CMD_VBP = ST77903_VBP_CMD;
    rlspi_init_struct.HSYNC_CMD_VBP_ADDR = ST77903_VBP_ADDR;
    rlspi_init_struct.HSYNC_CMD_VFP = ST77903_VFP_CMD;
    rlspi_init_struct.HSYNC_CMD_VFP_ADDR = ST77903_VFP_ADDR;
    rlspi_init_struct.HSYNC_CMD_VACTIVE = ST77903_VACTIVE_CMD_SPI;
    rlspi_init_struct.HSYNC_CMD_VACTIVE_ADDR = ST77903_VACTIVE_ADDR;
    rlspi_init_struct.DUMMY_CMD = ST77903_DUMMY_CMD;
    rlspi_init_struct.height = ST77903_HEIGHT;
    rlspi_init_struct.width = ST77903_WIDTH;
    RLSPI_Init(&rlspi_init_struct);
    RAMLESS_QSPI->RLSPI_LINE_DELAY_IN_VACTIVE = 0xC80;// 40us for line delay
    RAMLESS_QSPI->RLSPI_LINE_DELAY_OUT_VACTIVE = 0xC80;// 40us for line delay
    RAMLESS_QSPI->RLSPI_FRAME_DELAY_INFINITE = 0x3D0900;    // 50ms for frame delay

    LCDC_InitTypeDef lcdc_init = {0};
    lcdc_init.LCDC_Interface = LCDC_IF_RAMLESS_QSPI;
    lcdc_init.LCDC_PixelInputFarmat = LCDC_INPUT_ARGB8888;
    lcdc_init.LCDC_PixelOutpuFarmat = LCDC_OUTPUT_RGB888;
    lcdc_init.LCDC_PixelBitSwap = LCDC_SWAP_BYPASS; //lcdc_handler_cfg->LCDC_TeEn = LCDC_TE_DISABLE;
#if TE_VALID
    lcdc_init.LCDC_TeEn = LCDC_TE_DISABLE;
    lcdc_init.LCDC_TePolarity = LCDC_TE_EDGE_FALLING;
    lcdc_init.LCDC_TeInputMux = LCDC_TE_LCD_INPUT;
#endif
    lcdc_init.LCDC_DmaThreshold =
        8;    //only support threshold = 8 for DMA MSIZE = 8; the other threshold setting will be support later
    lcdc_init.LCDC_InfiniteModeEn = 1;
    LCDC_Init(&lcdc_init);
//    LCDC_HANDLER->INFINITE_MODE_CTR |= BIT31;
    LCDC_MaskINTConfig(LCDC_MASK_TX_AUTO_DONE_INT_MSK, ENABLE);
    LCDC_ClearINTPendingBit(LCDC_TX_AUTO_DONE_INT_CLR);
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = Display_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
    DBG_DIRECT("LCDC_HANDLER->DMA_FIFO_IMR = 0x%x\n", LCDC_HANDLER->DMA_FIFO_IMR);
    DBG_DIRECT("LCDC_HANDLER->DMA_FIFO_SR = 0x%x\n", LCDC_HANDLER->DMA_FIFO_SR);
    DBG_DIRECT("LCDC_HANDLER->DMA_FIFO_ICR = 0x%x\n", LCDC_HANDLER->DMA_FIFO_ICR);

    LCDC_DBICCfgTypeDef dbic_init = {0};
    dbic_init.DBIC_SPEED_SEL         = 1;
    dbic_init.DBIC_TxThr             = 0;//0 or 4
    dbic_init.DBIC_RxThr             = 0;
    dbic_init.SCPOL                  = DBIC_SCPOL_LOW;
    dbic_init.SCPH                   = DBIC_SCPH_1Edge;
    DBIC_Init(&dbic_init);
    DBIC->TXFTLR        =       ST77903_WIDTH;

    LCDC_SwitchMode(LCDC_MANUAL_MODE);
    LCDC_SwitchDirect(LCDC_TX_MODE);
    LCDC_Cmd(ENABLE);

    LCDC_LCD_SET_RST(false);
    platform_delay_ms(100);
    LCDC_LCD_SET_RST(true);
    platform_delay_ms(20);
    LCDC_LCD_SET_RST(false);
    platform_delay_ms(120);

    LCDC_AXIMUXMode(LCDC_FW_MODE);
    DBIC_SwitchMode(DBIC_USER_MODE);
    DBIC_SwitchDirect(DBIC_TMODE_TX);

    st77903_init_cmds();
}

