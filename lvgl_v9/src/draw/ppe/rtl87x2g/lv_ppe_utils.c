/**
 * @file lv_ppe_utils.c
 *
 */

/*********************
 *      INCLUDES
 *********************/


#if 1
#include "lv_ppe_utils.h"
#include "../../../core/lv_refr.h"
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

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

PPE_ERR lv_ppe_recolor(ppe_buffer_t *image, ppe_buffer_t *buffer, ppe_rect_t *rect, uint32_t color)
{
    if (buffer == NULL)
    {
        return PPE_ERROR_NULL_TARGET;
    }
    if (image == NULL)
    {
        return PPE_ERROR_NULL_SOURCE;
    }
    uint8_t format_len = ppe_get_format_data_len(image->format);
    if (format_len == 0)
    {
        return PPE_ERROR_UNKNOWN_FORMAT;
    }

    RCC_PeriphClockCmd(APBPeriph_PPE, APBPeriph_PPE_CLOCK, ENABLE);
    /*declaration of input/result layer initialization struct*/
    PPE_InputLayer_InitTypeDef      PPE_Input_Layer1;
    PPE_InputLayer_InitTypeDef      PPE_Input_Layer2;
    PPE_ResultLayer_InitTypeDef     PPE_Result_Layer;
    PPE_InitTypeDef                 PPE_Init_User;

    PPE_InputLayer_StructInit(&PPE_Input_Layer1);
    PPE_InputLayer_StructInit(&PPE_Input_Layer2);
    PPE_ResultLayer_StructInit(&PPE_Result_Layer);
    PPE_structInit(&PPE_Init_User);

    /*initial input layer2*/
    PPE_Input_Layer2.start_x = 0;
    PPE_Input_Layer2.start_y = 0;
    PPE_Input_Layer2.src_addr                       = NULL;
    PPE_Input_Layer2.width                          = rect->right - rect->left + 1;
    PPE_Input_Layer2.height                         = rect->bottom - rect->top + 1;
    PPE_Input_Layer2.const_ABGR8888_value           = color;
    PPE_Input_Layer2.format                         = PPE_ABGR8888;
    PPE_Input_Layer2.src                            = PPE_LAYER_SRC_CONST;
    PPE_Input_Layer2.line_len                       = image->stride;
    PPE_Input_Layer2.AXSIZE                         = 2;// 32bit bandwidth;
    PPE_Input_Layer2.INCR                           = PPE_ARBURST_INCR;
    PPE_Input_Layer2.AXCACHE                        = 1;
    PPE_Input_Layer2.MAX_AXLEN_LOG                  = PPE_MAX_AXLEN_127;
    PPE_Input_Layer2.PRIOR                          = 0;
    PPE_Input_Layer2.byte_swap                      = PPE_NO_SWAP;
    PPE_Input_Layer2.handshake_mode                 = PPE_DMA_SW_HANDSHAKE;
    PPE_Input_Layer2.polarity                       = PPE_POLARITY_HIGH;
    PPE_Input_Layer2.handshake_en                   = DISABLE;
    PPE_Input_Layer2.handshake_msize                = PPE_MSIZE_1024;
    PPE_Input_Layer2.hw_index                       = 0;
    PPE_InitInputLayer(2, &PPE_Input_Layer2);

    /*initial input layer1*/
    PPE_Input_Layer1.start_x                        = 0;
    PPE_Input_Layer1.start_y                        = 0;
    PPE_Input_Layer1.src_addr                       = (uint32_t)image->memory +
                                                      (rect->top * image->stride + rect->left) * ppe_get_format_data_len(image->format);
    PPE_Input_Layer1.width                          = rect->right - rect->left + 1;
    PPE_Input_Layer1.height                         = rect->bottom - rect->top + 1;
    PPE_Input_Layer1.const_ABGR8888_value           = 0XFFAABBCC;
    PPE_Input_Layer1.format                         = image->format;
    PPE_Input_Layer1.src                            = PPE_LAYER_SRC_FROM_DMA;
    if (image->color_key_en)
    {
        PPE_Input_Layer1.color_key_en                   = ENABLE;
        PPE_Input_Layer1.key_color_value                = image->color_key_value;
    }
    else
    {
        PPE_Input_Layer1.color_key_en                   = DISABLE;
        PPE_Input_Layer1.key_color_value                = 0;
    }
    PPE_Input_Layer1.line_len                       = image->stride;
    PPE_Input_Layer1.AXSIZE                         = 2;// 32bit bandwidth;
    PPE_Input_Layer1.INCR                           = PPE_ARBURST_INCR;
    PPE_Input_Layer1.AXCACHE                        = 1;
    PPE_Input_Layer1.MAX_AXLEN_LOG                  = PPE_MAX_AXLEN_127;
    PPE_Input_Layer1.PRIOR                          = 0;
    PPE_Input_Layer1.byte_swap                      = PPE_NO_SWAP;
    PPE_Input_Layer1.handshake_mode                 = PPE_DMA_SW_HANDSHAKE;
    PPE_Input_Layer1.polarity                       = PPE_POLARITY_HIGH;
    PPE_Input_Layer1.handshake_en                   = DISABLE;
    PPE_Input_Layer1.handshake_msize                = PPE_MSIZE_1024;
    PPE_Input_Layer1.hw_index                       = 0;
    PPE_InitInputLayer(1, &PPE_Input_Layer1);

    /*initial result layer*/
    PPE_Result_Layer.src_addr                       = (uint32_t)buffer->memory;
    PPE_Result_Layer.width                          = PPE_Input_Layer1.width;
    PPE_Result_Layer.height                         = PPE_Input_Layer1.height;
    PPE_Result_Layer.format                         = buffer->format;
    PPE_Result_Layer.line_len                       = PPE_Input_Layer1.width;
    PPE_Result_Layer.AXSIZE                         = 2;// 32bit bandwidth
    PPE_Result_Layer.INCR                           = PPE_ARBURST_INCR;
    PPE_Result_Layer.AXCACHE                        = 1;
    PPE_Result_Layer.MAX_AXLEN_LOG                  = PPE_MAX_AXLEN_127;
    PPE_Result_Layer.PRIOR                          = 0;
    PPE_Result_Layer.byte_swap                      = PPE_NO_SWAP;
    PPE_Result_Layer.handshake_mode                 = PPE_DMA_SW_HANDSHAKE;
    PPE_Result_Layer.polarity                       = PPE_POLARITY_HIGH;
    PPE_Result_Layer.handshake_en                   = DISABLE;
    PPE_Result_Layer.handshake_msize                = PPE_MSIZE_1024;
    PPE_Result_Layer.hw_index                       = 0;
    PPE_InitResultLayer(&PPE_Result_Layer);

    /*initial PPE global*/
    PPE_Init_User.function                          =   PPE_FUNCTION_ALPHA_BLEND;
    PPE_Init_User.blend_layer_num                   = 2;
    PPE_Init(&PPE_Init_User);
    PPE_Secure(ENABLE);  /*secure for all channel*/
    /*enable PPE*/
    PPE_Cmd(ENABLE);
    while (PPE->GLB_CTL & BIT0);
    buffer->stride = buffer->width;
    return PPE_SUCCESS;
}

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

uint8_t *lv_ppe_get_buffer(uint32_t size)
{
    uint8_t *buffer = NULL;
    if (size == 0)
    {
        return NULL;
    }
    if (size > LV_PPE_MAX_BUFFER_SIZE)
    {
        buffer = lv_malloc(LV_PPE_MAX_BUFFER_SIZE);
    }
    else
    {
        buffer = lv_malloc(size);
    }
    return buffer;
}
#endif /*LV_USE_PPE*/
