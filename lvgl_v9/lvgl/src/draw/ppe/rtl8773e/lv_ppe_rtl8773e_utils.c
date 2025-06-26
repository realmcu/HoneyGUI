/**
 * @file lv_ppe_utils.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../../core/lv_refr.h"
#include "../../lv_draw_image_private.h"
#if LV_USE_DRAW_PPE_RTL8773E
#include "lv_ppe_rtl8773e_utils.h"
#include "section.h"
#include "math.h"
#include "dma_channel.h"
#include "rtl876x_gdma.h"
#include "rtl_idu_int.h"
#include "string.h"

SHM_DATA_SECTION uint8_t cache_buffer[40 * 1024];
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
static uint8_t high_speed_channel = 0xA5;
static uint8_t low_speed_channel = 0xA5;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

uint32_t lv_ppe_get_color(lv_color_t color, uint8_t opa)
{
    lv_color32_t ABGR8888_color;

    ABGR8888_color.alpha = opa;
    ABGR8888_color.blue = color.red;
    ABGR8888_color.green = color.green;
    ABGR8888_color.red = color.blue;

    return *(uint32_t *)&ABGR8888_color;
}

PPE_PIXEL_FORMAT lv_ppe_get_format(lv_color_format_t cf, const uint8_t *content)
{
    switch (cf)
    {
    case LV_COLOR_FORMAT_RGB565:
        return PPE_RGB565;
    case LV_COLOR_FORMAT_ARGB8888:
        return PPE_ARGB8888;
    case LV_COLOR_FORMAT_RGB888:
        return PPE_RGB888;
    case LV_COLOR_FORMAT_XRGB8888:
        return PPE_XRGB8888;
    case LV_COLOR_FORMAT_ARGB8565:
        return PPE_ARGB8565;
    case LV_COLOR_FORMAT_ARGB1555:
        return PPE_ARGB1555;
    case LV_COLOR_FORMAT_ARGB4444:
        return PPE_ARGB4444;
    case LV_COLOR_FORMAT_ARGB2222:
        return PPE_ARGB2222;
    case LV_COLOR_FORMAT_A8:
        return PPE_A8;
    case LV_COLOR_FORMAT_RAW:
        {
            uint8_t type = *(content + 1);
            switch (type)
            {
            case 0:
                return PPE_RGB565;
            case 1:
                return PPE_ARGB8565;
            case 3:
                return PPE_RGB888;
            case 4:
                return PPE_ARGB8888;
            case 5:
                return PPE_XRGB8888;
            default:
                return PPE_FORMAT_NOT_SUPPORT;
            }
        }
    default:
        return PPE_FORMAT_NOT_SUPPORT;
    }
}

lv_area_t lv_ppe_get_matrix_area(ppe_matrix_t *matrix, const lv_area_t *coords,
                                 const lv_draw_image_dsc_t *draw_dsc)
{
    lv_area_t transform_area;
    ppe_get_identity(matrix);
    bool scale = (draw_dsc->scale_x != LV_SCALE_NONE || draw_dsc->scale_y != LV_SCALE_NONE);
    bool rotate = (draw_dsc->rotation != 0);
    bool skew = (draw_dsc->skew_x != 0 || draw_dsc->skew_y != 0);
    matrix->m[0][2] = coords->x1;
    matrix->m[1][2] = coords->y1;
    if (scale || rotate)
    {
        ppe_translate(draw_dsc->pivot.x, draw_dsc->pivot.y, matrix);
        if (rotate)
        {
            ppe_rotate(draw_dsc->rotation / 10.0f, matrix);    /* angle is 1/10 degree */
        }
        if (scale)
        {
            float scale_ratio_x = 1.0f * draw_dsc->scale_x / LV_SCALE_NONE;
            float scale_ratio_y = 1.0f * draw_dsc->scale_y / LV_SCALE_NONE;
            ppe_scale(scale_ratio_x, scale_ratio_y, matrix);
        }
        if (skew)
        {
            ppe_skew(draw_dsc->skew_x, draw_dsc->skew_y, matrix);
        }
        ppe_translate(-draw_dsc->pivot.x, -draw_dsc->pivot.y, matrix);
    }

    int32_t w = lv_area_get_width(coords);
    int32_t h = lv_area_get_height(coords);

    lv_image_buf_get_transformed_area(&transform_area, w, h, draw_dsc->rotation, draw_dsc->scale_x,
                                      draw_dsc->scale_y,
                                      &draw_dsc->pivot);

    transform_area.x1 += coords->x1;
    transform_area.y1 += coords->y1;
    transform_area.x2 += coords->x1;
    transform_area.y2 += coords->y1;
    return transform_area;
}

uint8_t *lv_ppe_get_buffer(uint32_t size)
{
    return cache_buffer;
}

typedef struct
{
    float p[3];
} pox_t;

static void pos_transfer(ppe_matrix_t *matrix, pox_t *pox)
{
    float m_row0, m_row1, m_row2;

    float a = pox->p[0];
    float b = pox->p[1];
    float c = pox->p[2];

    /* Process all rows. */
    m_row0 = matrix->m[0][0];
    m_row1 = matrix->m[0][1];
    m_row2 = matrix->m[0][2];
    pox->p[0] = (m_row0 * a) + (m_row1 * b) + (m_row2 * c);

    m_row0 = matrix->m[1][0];
    m_row1 = matrix->m[1][1];
    m_row2 = matrix->m[1][2];
    pox->p[1] = (m_row0 * a) + (m_row1 * b) + (m_row2 * c);

    m_row0 = matrix->m[2][0];
    m_row1 = matrix->m[2][1];
    m_row2 = matrix->m[2][2];
    pox->p[2] = (m_row0 * a) + (m_row1 * b) + (m_row2 * c);

    pox->p[0] = pox->p[0] / pox->p[2];
    pox->p[1] = pox->p[1] / pox->p[2];
    pox->p[2] = 1;
}


bool lv_ppe_get_area(ppe_rect_t *result_rect, ppe_rect_t *source_rect, ppe_matrix_t *matrix)
{
    pox_t pox = {0.0f};
    float x_min = 0.0f;
    float x_max = 0.0f;
    float y_min = 0.0f;
    float y_max = 0.0f;

    pox.p[0] = source_rect->x * 1.0f;
    pox.p[1] = source_rect->y * 1.0f;
    pox.p[2] = 1.0f;
    pos_transfer(matrix, &pox);
    x_min = pox.p[0];
    x_max = pox.p[0];
    y_min = pox.p[1];
    y_max = pox.p[1];

    pox.p[0] = (source_rect->x + source_rect->w - 1) * 1.0f;
    pox.p[1] = source_rect->y * 1.0f;
    pox.p[2] = 1.0f;
    pos_transfer(matrix, &pox);
    if (x_min > pox.p[0])
    {
        x_min = pox.p[0];
    }
    if (x_max < pox.p[0])
    {
        x_max = pox.p[0];
    }
    if (y_min > pox.p[1])
    {
        y_min = pox.p[1];
    }
    if (y_max < pox.p[1])
    {
        y_max = pox.p[1];
    }

    pox.p[0] = source_rect->x * 1.0f;
    pox.p[1] = (source_rect->y + source_rect->h - 1) * 1.0f;
    pox.p[2] = 1.0f;
    pos_transfer(matrix, &pox);
    if (x_min > pox.p[0])
    {
        x_min = pox.p[0];
    }
    if (x_max < pox.p[0])
    {
        x_max = pox.p[0];
    }
    if (y_min > pox.p[1])
    {
        y_min = pox.p[1];
    }
    if (y_max < pox.p[1])
    {
        y_max = pox.p[1];
    }

    pox.p[0] = (source_rect->x + source_rect->w - 1) * 1.0f;
    pox.p[1] = (source_rect->y + source_rect->h - 1) * 1.0f;
    pox.p[2] = 1.0f;
    pos_transfer(matrix, &pox);
    if (x_min > pox.p[0])
    {
        x_min = pox.p[0];
    }
    if (x_max < pox.p[0])
    {
        x_max = pox.p[0];
    }
    if (y_min > pox.p[1])
    {
        y_min = pox.p[1];
    }
    if (y_max < pox.p[1])
    {
        y_max = pox.p[1];
    }

    result_rect->x = (int16_t)x_min;
    result_rect->y = (int16_t)y_min;
    result_rect->w = ceil(x_max) - result_rect->x + 1;
    result_rect->h = ceil(y_max) - result_rect->y + 1;

    if (isnan(y_min) || isnan(y_max) || isnan(x_min) || isnan(x_max) || result_rect->h == 0 ||
        result_rect->w == 0)
    {
        return false;
    }
    return true;
}

void lv_acc_dma_channel_init(void)
{
    GDMA_channel_request(&high_speed_channel, NULL, true);
    GDMA_channel_request(&low_speed_channel, NULL, false);
    if (high_speed_channel == 0xA5 || low_speed_channel == 0xA5)
    {
        LV_LOG_ERROR("No dma channel valid");
        LV_ASSERT(NULL != NULL);
    }
}

uint8_t lv_acc_get_high_speed_channel(void)
{
    return high_speed_channel;
}

uint8_t lv_acc_get_low_speed_channel(void)
{
    return low_speed_channel;
}

void lv_acc_dma_copy(uint32_t length, uint32_t height, uint32_t src_stride,
                     uint32_t dst_stride, uint8_t *src, uint8_t *dst)
{
    bool use_LLI = true;
    if ((length == src_stride && length == dst_stride) || height == 1)
    {
        use_LLI = false;
    }
    uint32_t dma_height = height;
    uint32_t buffer_size = 0;
    uint32_t total_size_in_byte = length * height;
    uint32_t total_size = total_size_in_byte / 4;
    uint8_t m_size = 0, data_size = 0;
    uint8_t dma_depth = rtl_idu_get_dma_depth(high_speed_channel);
    if (length % 4 == 0)
    {
        data_size = GDMA_DataSize_Word;
        if (dma_depth == 4)
        {
            m_size = GDMA_Msize_4;
        }
        else if (dma_depth == 8)
        {
            m_size = GDMA_Msize_8;
        }
        else if (dma_depth == 16)
        {
            m_size = GDMA_Msize_16;
        }
        else
        {
            return;
        }
        buffer_size = length / 4;
        if (!use_LLI)
        {
            if (buffer_size > 65535)
            {
                use_LLI = true;
                dma_height = buffer_size / 65535;
                if (buffer_size % 65535)
                {
                    dma_height += 1;
                }
                buffer_size = 65535;
            }
            else
            {
                buffer_size = total_size;
            }
        }
    }
    else if (length % 2 == 0)
    {
        if (dma_depth == 4)
        {
            m_size = GDMA_Msize_8;
        }
        else if (dma_depth == 8)
        {
            m_size = GDMA_Msize_16;
        }
        else if (dma_depth == 16)
        {
            m_size = GDMA_Msize_32;
        }
        else
        {
            return;
        }
        data_size = GDMA_DataSize_HalfWord;
        buffer_size = length / 2;
        total_size = 0;
    }
    else
    {
        data_size = GDMA_DataSize_Byte;
        if (dma_depth == 4)
        {
            m_size = GDMA_Msize_16;
        }
        else if (dma_depth == 8)
        {
            m_size = GDMA_Msize_32;
        }
        else if (dma_depth == 16)
        {
            m_size = GDMA_Msize_64;
        }
        else
        {
            return;
        }
        buffer_size = length;
        total_size = 0;
    }

    GDMA_LLIDef *GDMA_LLIStruct;
    if (use_LLI)
    {
        GDMA_LLIStruct = (GDMA_LLIDef *)lv_malloc(dma_height * sizeof(GDMA_LLIDef));
        if (GDMA_LLIStruct == NULL)
        {
            assert_param(GDMA_LLIStruct != NULL);
        }
        else
        {
            memset(GDMA_LLIStruct, 0, dma_height * sizeof(GDMA_LLIDef));
        }
    }
    uint32_t start_address = (uint32_t)src;
    uint32_t dest_address = (uint32_t)dst;
    RCC_PeriphClockCmd(APBPeriph_GDMA, APBPeriph_GDMA_CLOCK, ENABLE);
    GDMA_ChannelTypeDef *dma_channel = rtl_idu_get_dma_channel_int(high_speed_channel);
    GDMA_InitTypeDef RX_GDMA_InitStruct;
    /*--------------GDMA init-----------------------------*/
    GDMA_StructInit(&RX_GDMA_InitStruct);
    RX_GDMA_InitStruct.GDMA_ChannelNum          = high_speed_channel;
    RX_GDMA_InitStruct.GDMA_BufferSize          = buffer_size;
    RX_GDMA_InitStruct.GDMA_DIR                 = GDMA_DIR_MemoryToMemory;
    RX_GDMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Inc;
    RX_GDMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Inc;
    RX_GDMA_InitStruct.GDMA_SourceMsize         =
        m_size;                         // 8 msize for source msize
    RX_GDMA_InitStruct.GDMA_DestinationMsize    =
        m_size;                         // 8 msize for destiantion msize
    RX_GDMA_InitStruct.GDMA_DestinationDataSize =
        data_size;                   // 32 bit width for destination transaction
    RX_GDMA_InitStruct.GDMA_SourceDataSize      =
        data_size;                   // 32 bit width for source transaction
    RX_GDMA_InitStruct.GDMA_SourceAddr          = (uint32_t)start_address;
    RX_GDMA_InitStruct.GDMA_DestinationAddr     = (uint32_t)dest_address;

    if (use_LLI)
    {
        RX_GDMA_InitStruct.GDMA_Multi_Block_Mode = LLI_TRANSFER;
        RX_GDMA_InitStruct.GDMA_Multi_Block_En = 1;
        RX_GDMA_InitStruct.GDMA_Multi_Block_Struct = (uint32_t)GDMA_LLIStruct;
    }

    GDMA_Init(dma_channel, &RX_GDMA_InitStruct);
    if (use_LLI && GDMA_LLIStruct != NULL)
    {
        for (int i = 0; i < dma_height; i++)
        {
            if (i == dma_height - 1)
            {
                GDMA_LLIStruct[i].SAR = start_address + src_stride * i;
                GDMA_LLIStruct[i].DAR = (uint32_t)dest_address + dst_stride * i;
                GDMA_LLIStruct[i].LLP = 0;
                /* configure low 32 bit of CTL register */
                GDMA_LLIStruct[i].CTL_LOW = (BIT(0)
                                             | (RX_GDMA_InitStruct.GDMA_DestinationDataSize << 1)
                                             | (data_size << 4)
                                             | (RX_GDMA_InitStruct.GDMA_DestinationInc << 7)
                                             | (RX_GDMA_InitStruct.GDMA_SourceInc << 9)
                                             | (RX_GDMA_InitStruct.GDMA_DestinationMsize << 11)
                                             | (RX_GDMA_InitStruct.GDMA_SourceMsize << 14)
                                             | (RX_GDMA_InitStruct.GDMA_DIR << 20));
                /* configure high 32 bit of CTL register */
                if (total_size == 0)
                {
                    GDMA_LLIStruct[i].CTL_HIGH = buffer_size;
                }
                else
                {
                    GDMA_LLIStruct[i].CTL_HIGH = total_size - i * buffer_size;
                }
            }
            else
            {
                GDMA_LLIStruct[i].SAR = start_address + src_stride * i;
                GDMA_LLIStruct[i].DAR = (uint32_t)dest_address + dst_stride * i;
                GDMA_LLIStruct[i].LLP = (uint32_t)&GDMA_LLIStruct[i + 1];
                /* configure low 32 bit of CTL register */
                GDMA_LLIStruct[i].CTL_LOW = rtl_idu_get_dma_ctl_low_int(dma_channel);
                /* configure high 32 bit of CTL register */
                GDMA_LLIStruct[i].CTL_HIGH = buffer_size;
            }
        }
    }
    GDMA_INTConfig(high_speed_channel, GDMA_INT_Transfer, ENABLE);
    GDMA_Cmd(high_speed_channel, ENABLE);
    while (GDMA_GetTransferINTStatus(high_speed_channel) != SET);
    GDMA_ClearINTPendingBit(high_speed_channel, GDMA_INT_Transfer);
    if (use_LLI && GDMA_LLIStruct != NULL)
    {
        lv_free(GDMA_LLIStruct);
    }
}

#endif /*LV_USE_PPE*/
