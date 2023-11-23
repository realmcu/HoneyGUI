/**
*********************************************************************************************************
*               Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* \file     rtl_ppe.h
* \brief    This file provides all the Pixel Process Engine firmware functions.
* \details
* \author   astor zhang
* \date     2023-10-17
* \version  v1.0
*********************************************************************************************************
*/
#if LV_USE_GPU_RTK_PPE
/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef RTL_PPE_H
#define RTL_PPE_H

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "rtl876x.h"
#include "stdbool.h"
#include "rtl_ppe_reg.h"

/*============================================================================*
 *                          Private Macros
 *============================================================================*/
#define PPE_LAYER               ((PPE_LAYER_TypeDef*)PPE_CFG_REG_BASE)
#define PPE                     ((PPE_TypeDef*)(PPE_CFG_REG_BASE + 0x400))

/** \defgroup 87X2G_PPE        PPE
  * \brief
  * \{
  */

/*============================================================================*
 *                         Constants
 *============================================================================*/
/** \defgroup PPE_Exported_Constants PPE Exported Constants
  * \brief
  * \{
  */
typedef struct
{
    uint32_t LAYER_ADDR;
    uint32_t LAYER_POS;
    uint32_t LAYER_WINSIZE;
    uint32_t LAYER_CONST_PIX;
} PPE_LLI_LAYER;

typedef enum
{
    PPE_ABGR8888 = 0x0,
    PPE_ARGB8888,
    PPE_XBGR8888,
    PPE_XRGB8888,
    PPE_BGRA8888,
    PPE_RGBA8888,
    PPE_BGRX8888,
    PPE_RGBX8888,
    PPE_ABGR4444,
    PPE_ARGB4444,
    PPE_XBGR4444,
    PPE_XRGB4444,
    PPE_BGRA4444,
    PPE_RGBA4444,
    PPE_BGRX4444,
    PPE_RGBX4444,
    PPE_ABGR2222,
    PPE_ARGB2222,
    PPE_XBGR2222,
    PPE_XRGB2222,
    PPE_BGRA2222,
    PPE_RGBA2222,
    PPE_BGRX2222,
    PPE_RGBX2222,
    PPE_ABGR8565,
    PPE_ARGB8565,
    PPE_XBGR8565,
    PPE_XRGB8565,
    PPE_BGRA5658,
    PPE_RGBA5658,
    PPE_BGRX5658,
    PPE_RGBX5658,
    PPE_ABGR1555,
    PPE_ARGB1555,
    PPE_XBGR1555,
    PPE_XRGB1555,
    PPE_BGRA5551,
    PPE_RGBA5551,
    PPE_BGRX5551,
    PPE_RGBX5551,
    PPE_BGR888,
    PPE_RGB888,
    PPE_BGR565,
    PPE_RGB565,
    PPE_A8,
    PPE_X8,
    PPE_ABGR8666 = 0x32,
    PPE_ARGB8666,
    PPE_XBGR8666,
    PPE_XRGB8666,
    PPE_BGRA6668,
    PPE_RGBA6668,
    PPE_BGRX6668,
    PPE_RGBX6668,
} PPE_PIXEL_FORMAT;

typedef enum
{
    PPE_LAYER_SRC_CONST,
    PPE_LAYER_SRC_FROM_DMA,
} PPE_PIXEL_SOURCE;

typedef enum
{
    PPE_DMA_HW_HANDSHAKE,
    PPE_DMA_SW_HANDSHAKE,
} PPE_DMA_HANDSHAKE;

typedef enum
{
    PPE_MSIZE_1,
    PPE_MSIZE_2,
    PPE_MSIZE_4,
    PPE_MSIZE_8,
    PPE_MSIZE_16,
    PPE_MSIZE_32,
    PPE_MSIZE_64,
    PPE_MSIZE_128,
    PPE_MSIZE_256,
    PPE_MSIZE_512,
    PPE_MSIZE_1024,
} PPE_MSIZE_LOG;

typedef enum
{
    PPE_MAX_AXLEN_0,
    PPE_MAX_AXLEN_1,
    PPE_MAX_AXLEN_3,
    PPE_MAX_AXLEN_7,
    PPE_MAX_AXLEN_15,
    PPE_MAX_AXLEN_31,
    PPE_MAX_AXLEN_63,
    PPE_MAX_AXLEN_127,
} PPE_MAX_AXLEN;

typedef enum
{
    PPE_POLARITY_LOW,
    PPE_POLARITY_HIGH,
} PPE_ACTIVE_POLARITY;

typedef enum
{
    PPE_ARBURST_FIX,
    PPE_ARBURST_INCR,
} PPE_ARBURST;

typedef enum
{
    PPE_NO_SWAP,
    PPE_8BIT_SWAP,
    PPE_16BIT_SWAP,
    PPE_8_16BIT_SWAP,
    PPE_32BIT_SWAP,
    PPE_32_8BIT_SWAP,
    PPE_32_16BIT_SWAP,
    PPE_32_16_8BIT_SWAP,
} PPE_BYTE_SWAP;

typedef struct
{
    uint32_t function;
    uint32_t blend_layer_num;
} PPE_InitTypeDef;

typedef struct
{
    uint32_t src_addr;
    uint32_t start_x;
    uint32_t start_y;
    uint32_t width;
    uint32_t height;
    uint32_t const_ABGR8888_value;
    uint32_t key_color_value;
    uint32_t line_len;
    PPE_PIXEL_FORMAT format;
    PPE_PIXEL_SOURCE src;
    FunctionalState color_key_en;
    PPE_BYTE_SWAP byte_swap;
    PPE_DMA_HANDSHAKE handshake_mode;
    PPE_ACTIVE_POLARITY polarity;
    FunctionalState handshake_en;
    PPE_MAX_AXLEN MAX_AXLEN_LOG;
    uint32_t AXSIZE;
    PPE_ARBURST INCR;
    uint32_t AXCACHE;
    uint32_t PRIOR;
    PPE_MSIZE_LOG handshake_msize;
    uint32_t hw_index;
} PPE_InputLayer_InitTypeDef;

typedef struct
{
    uint32_t src_addr;
    uint32_t width;
    uint32_t height;
    uint32_t line_len;
    PPE_PIXEL_FORMAT format;
    PPE_BYTE_SWAP byte_swap;
    PPE_DMA_HANDSHAKE handshake_mode;
    PPE_ACTIVE_POLARITY polarity;
    FunctionalState handshake_en;
    PPE_MAX_AXLEN MAX_AXLEN_LOG;
    uint32_t AXSIZE;
    PPE_ARBURST INCR;
    uint32_t AXCACHE;
    uint32_t PRIOR;
    PPE_MSIZE_LOG handshake_msize;
    uint32_t hw_index;
} PPE_ResultLayer_InitTypeDef;

typedef enum
{
    PPE_SUCCESS,
    PPE_ERROR_ADDR_NOT_ALIGNED,
    PPE_ERROR_INVALID_PARAM,
    PPE_ERROR_UNKNOWN_FORMAT,
    PPE_ERROR_NULL_SOURCE,
    PPE_ERROR_NULL_TARGET,
    PPE_ERROR_OUT_OF_RANGE,
} PPE_ERR;


typedef struct
{
    int32_t left;
    int32_t top;
    int32_t right;
    int32_t bottom;
} PPE_rect_t;

typedef struct
{
    int32_t x;
    int32_t y;
} PPE_translate_t;

typedef struct
{
    uint32_t *memory;
    uint32_t address;
    PPE_PIXEL_FORMAT format;
    bool color_key_en;
    bool global_alpha_en;
    uint16_t width;
    uint16_t height;
    uint16_t stride;
    uint32_t color_key_value;
    uint32_t global_alpha;
} ppe_buffer_t;

/**
 * \defgroup    PPE_Interrupt PPE Interrupt
 * \{
 * \ingroup     PPE_Exported_Constants
 */
#define PPE_ALL_OVER_INT                                ((uint32_t)0x1 << PPE_INT_ALL_OVER_POS)
#define PPE_FR_OVER_INT                                 ((uint32_t)0x1 << PPE_INT_FR_OVER_POS)
#define PPE_LOAD_OVER_INT                               ((uint32_t)0x1 << PPE_INT_LOAD_OVER_POS)
#define PPE_LINE_WL_INT                                 ((uint32_t)0x1 << PPE_INT_LINE_WL_POS)
#define PPE_SUSP_INAC_INT                               ((uint32_t)0x1 << PPE_INT_SUSP_INAC_POS)
#define PPE_SECURE_ERR_INT                              ((uint32_t)0x1 << PPE_INT_SECURE_ERR_POS)
#define PPE_SET_ERR_INT                                 ((uint32_t)0x1 << PPE_INT_SET_ERR_POS)
#define PPE_BUS1_ERR_INT                                ((uint32_t)0x1 << PPE_INT_CHN_BUS_ERR_POS)
#define PPE_BUS2_ERR_INT                                ((uint32_t)0x2 << PPE_INT_CHN_BUS_ERR_POS)
#define PPE_BUS3_ERR_INT                                ((uint32_t)0x4 << PPE_INT_CHN_BUS_ERR_POS)
#define PPE_BUS4_ERR_INT                                ((uint32_t)0x8 << PPE_INT_CHN_BUS_ERR_POS)
#define PPE_BUS5_ERR_INT                                ((uint32_t)0x10 << PPE_INT_CHN_BUS_ERR_POS)
#define PPE_BUS6_ERR_INT                                ((uint32_t)0x20 << PPE_INT_CHN_BUS_ERR_POS)
#define PPE_BUS7_ERR_INT                                ((uint32_t)0x40 << PPE_INT_CHN_BUS_ERR_POS)
#define PPE_BUS8_ERR_INT                                ((uint32_t)0x80 << PPE_INT_CHN_BUS_ERR_POS)
#define PPE_BUS9_ERR_INT                                ((uint32_t)0x100 << PPE_INT_CHN_BUS_ERR_POS)
#define PPE_BUS10_ERR_INT                               ((uint32_t)0x200 << PPE_INT_CHN_BUS_ERR_POS)
#define PPE_BUS11_ERR_INT                               ((uint32_t)0x400 << PPE_INT_CHN_BUS_ERR_POS)
#define PPE_BUS12_ERR_INT                               ((uint32_t)0x800 << PPE_INT_CHN_BUS_ERR_POS)
#define PPE_BUS13_ERR_INT                               ((uint32_t)0x1000 << PPE_INT_CHN_BUS_ERR_POS)
#define PPE_BUS14_ERR_INT                               ((uint32_t)0x2000 << PPE_INT_CHN_BUS_ERR_POS)
#define PPE_BUS15_ERR_INT                               ((uint32_t)0x4000 << PPE_INT_CHN_BUS_ERR_POS)
#define PPE_BUS16_ERR_INT                               ((uint32_t)0x8000 << PPE_INT_CHN_BUS_ERR_POS)
#define PPE_BUS17_ERR_INT                               ((uint32_t)0x10000 << PPE_INT_CHN_BUS_ERR_POS)

/** End of PPE_Interrupt
  * \}
  */

#define IS_PPE_INT(interrupt)                           ((interrupt) == PPE_ALL_OVER_INT) || (interrupt) == PPE_INT_FR_OVER_POS) ||\
    (interrupt) == PPE_INT_LOAD_OVER_POS) || (interrupt) == PPE_INT_LINE_WL_POS) ||\
    (interrupt) == PPE_INT_SUSP_INAC_POS) || (interrupt) == PPE_INT_SECURE_ERR_POS) ||\
    (interrupt) == PPE_INT_SET_ERR_POS) || \
    ((interrupt) & PPE_INT_CHN_BUS_ERR_CLR) | PPE_INT_CHN_BUS_ERR_MSK))

#define IS_PPE_CHN_INDEX(chn)                               (((chn) < 17) && ((chn) >= 0))

#define PPE_FUNCTION_ALPHA_BLEND                        ((uint32_t)0x0)
#define PPE_FUNCTION_SCALE                              ((uint32_t)0x1)
#define IS_PPE_FUNCTION(func)                           (((func) == PPE_FUNCTION_ALPHA_BLEND) || ((func) == PPE_FUNCTION_SCALE))

#define IS_PPE_ALPHA_BLEND_LAYER_NUM(layer_num)         ((layer_num) < 16)
#define IS_PPE_SCALE_LINE_NUM(line_num)                 ((line_num) < 16)
#define IS_PPE_SCALE_COLUMN_NUM(colunm_num)             ((colunm_num) < 16)

/** End of PPE_Exported_Constants
  * \}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/
/** \defgroup PPE_Exported_Functions PPE Exported Functions
  * \brief
  * \{
  */
void PPE_Cmd(FunctionalState state);
void PPE_Suspend(void);
void PPE_Resume(void);
void PPE_Abort(void);
ITStatus PPE_ChannelInactiveCheck(uint8_t chn_id);
void PPE_Secure(FunctionalState state);
ITStatus PPE_GetINTStatus(uint32_t PPE_INT);
ITStatus PPE_GetINTStatusRaw(uint32_t PPE_INT);
void PPE_ClearINTPendingBit(uint32_t PPE_INT);
void PPE_MaskINTConfig(uint32_t PPE_INT_MSK, FunctionalState NewState);
void PPE_AutoReloadCmd(FunctionalState NewState);
void PPE_AutoClearCmd(FunctionalState NewState);
void PPE_SetValid(FunctionalState NewState);
void PPE_structInit(PPE_InitTypeDef *PPE_init_struct);
void PPE_InputLayer_StructInit(PPE_InputLayer_InitTypeDef *layer_init_struct);
void PPE_ResultLayer_StructInit(PPE_ResultLayer_InitTypeDef *layer_init_struct);
void PPE_Init(PPE_InitTypeDef *PPE_init_struct);
void PPE_InitInputLayer(uint8_t id, PPE_InputLayer_InitTypeDef *layer_init_struct);
void PPE_InitResultLayer(PPE_ResultLayer_InitTypeDef *layer_init_struct);

/**
 * \brief  Scale the input image and save in output image buffer
 * \param[in] image         input image.
 * \param[in] buffer        output image buffer.
 * \param[in] x_ratio       scale ration on x-axis.
 * \param[in] y_ratio       scale ration on y-axis.
 * \return operation result
 * \retval PPE_SUCCESS  Operation success.
 * \retval others       Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test_code(void){
        RCC_PeriphClockCmd(APBPeriph_PPE, APBPeriph_PPE_CLOCK, ENABLE);
        ppe_buffer_t image, buffer;
        image.width = 25;
        image.height = 25;
        image.format = PPE_RGB888;
        image.memory = (uint32_t*)SOURCE_PIC_1;
        image.address = (uint32_t)image.memory;

        buffer.format = PPE_RGB565;
        buffer.memory = (uint32_t*)PIC_OUTPUT_LAYER;
        buffer.address = (uint32_t)PIC_OUTPUT_LAYER;
        memset(PIC_OUTPUT_LAYER, 0, sizeof(PIC_OUTPUT_LAYER));
        PPE_ERR err = PPE_Scale(&image, &buffer, 3, 0.8);
    }
 * \endcode
 */
PPE_ERR PPE_Scale(ppe_buffer_t *image, ppe_buffer_t *buffer, float x_ratio, float y_ratio);

/**
 * \brief  Scale a part of input image and save in output image buffer
 * \param[in] image         input image.
 * \param[in] buffer        output image buffer.
 * \param[in] x_ratio       scale ration on x-axis.
 * \param[in] y_ratio       scale ration on y-axis.
 * \param[in] rect          the boundary of the part to be scaled.
 * \return operation result
 * \retval PPE_SUCCESS  Operation success.
 * \retval others       Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test_code(void){
        RCC_PeriphClockCmd(APBPeriph_PPE, APBPeriph_PPE_CLOCK, ENABLE);
        ppe_buffer_t image, buffer;
        image.width = 25;
        image.height = 25;
        image.format = PPE_RGB888;
        image.memory = (uint32_t*)SOURCE_PIC_1;
        image.address = (uint32_t)image.memory;

        buffer.format = PPE_RGB565;
        buffer.memory = (uint32_t*)PIC_OUTPUT_LAYER;
        buffer.address = (uint32_t)PIC_OUTPUT_LAYER;
        memset(PIC_OUTPUT_LAYER, 0, sizeof(PIC_OUTPUT_LAYER));
        PPE_rect_t rect = {6, 4, 20, 24};
        PPE_ERR err = PPE_Scale_Rect(&image, &buffer, 3, 3, &rect);
    }
 * \endcode
 */
PPE_ERR PPE_Scale_Rect(ppe_buffer_t *image, ppe_buffer_t *buffer, float x_ratio, float y_ratio,
                       PPE_rect_t *rect);

/**
 * \brief  Clear the image buffer with certain color in ABGR8888 format
 * \param[in] buffer        input image buffer.
 * \param[in] color         specified color in ABGR8888 format
 * \return operation result
 * \retval PPE_SUCCESS  Operation success.
 * \retval others       Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test_code(void){
        RCC_PeriphClockCmd(APBPeriph_PPE, APBPeriph_PPE_CLOCK, ENABLE);
        ppe_buffer_t buffer;

        buffer.format = PPE_RGB565;
        buffer.memory = (uint32_t*)PIC_OUTPUT_LAYER;
        buffer.address = (uint32_t)PIC_OUTPUT_LAYER;
        buffer.width = 64;
        buffer.height = 64;

        PPE_ERR PPE_Clear(&buffer, 0xFFFF00FF);
    }
 * \endcode
 */
PPE_ERR PPE_Clear(ppe_buffer_t *buffer, uint32_t color);

PPE_ERR PPE_Clear_Rect(ppe_buffer_t *buffer, PPE_rect_t *rect, uint32_t color);

/**
 * \brief  blend source image to target buffer
 * \param[in] image         source image.
 * \param[in] buffer        target image buffer.
 * \param[in] trans         translate information of source image.
 * \return operation result
 * \retval PPE_SUCCESS  Operation success.
 * \retval others       Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test_code(void){
        RCC_PeriphClockCmd(APBPeriph_PPE, APBPeriph_PPE_CLOCK, ENABLE);
        ppe_buffer_t image, buffer;
        image.width = 25;
        image.height = 25;
        image.format = PPE_RGB888;
        image.memory = (uint32_t*)SOURCE_PIC_1;
        image.address = (uint32_t)image.memory;

        buffer.format = PPE_RGB565;
        buffer.memory = (uint32_t*)PIC_OUTPUT_LAYER;
        buffer.address = (uint32_t)PIC_OUTPUT_LAYER;
        buffer.width = 64;
        buffer.height = 64;
        memset(PIC_OUTPUT_LAYER, 0, sizeof(PIC_OUTPUT_LAYER));

        PPE_translate_t trans = {20, 20};
        PPE_blend(&image, &buffer, &trans);
    }
 * \endcode
 */
PPE_ERR PPE_blend(ppe_buffer_t *image, ppe_buffer_t *buffer, PPE_translate_t *trans);

/**
 * \brief  blend a part of source image to target buffer
 * \param[in] image         source image.
 * \param[in] buffer        target image buffer.
 * \param[in] trans         translate information of source image.
 * \param[in] rect          the boundary of the part to be scaled.
 * \return operation result
 * \retval PPE_SUCCESS  Operation success.
 * \retval others       Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test_code(void){
        RCC_PeriphClockCmd(APBPeriph_PPE, APBPeriph_PPE_CLOCK, ENABLE);
        ppe_buffer_t image, buffer;
        image.width = 25;
        image.height = 25;
        image.format = PPE_RGB888;
        image.memory = (uint32_t*)SOURCE_PIC_1;
        image.address = (uint32_t)image.memory;

        buffer.format = PPE_RGB565;
        buffer.memory = (uint32_t*)PIC_OUTPUT_LAYER;
        buffer.address = (uint32_t)PIC_OUTPUT_LAYER;
        buffer.width = 64;
        buffer.height = 64;
        memset(PIC_OUTPUT_LAYER, 0, sizeof(PIC_OUTPUT_LAYER));

        PPE_translate_t trans = {20, 20};
        PPE_rect_t rect = {6, 4, 20, 24};
        PPE_blend_rect(&image, &buffer, &trans, &rect);
    }
 * \endcode
 */
PPE_ERR PPE_blend_rect(ppe_buffer_t *image, ppe_buffer_t *buffer, PPE_translate_t *trans,
                       PPE_rect_t *rect);

/**
 * \brief  blend multiple images to target buffer
 * \param[in] image1        source image, must not be NULL.
 * \param[in] image2        another source image.
 * \param[in] image3        another source image.
 * \param[in] buffer        target image buffer.
 * \param[in] trans         translate information of source image.
 * \return operation result
 * \retval PPE_SUCCESS  Operation success.
 * \retval others       Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test_code(void){
        RCC_PeriphClockCmd(APBPeriph_PPE, APBPeriph_PPE_CLOCK, ENABLE);
        ppe_buffer_t image1, image2, image3, buffer;
        image1.width = 25;
        image1.height = 25;
        image1.format = PPE_RGB888;
        image1.memory = (uint32_t*)SOURCE_PIC_1;
        image1.address = (uint32_t)image.memory;

        image2.width = 25;
        image2.height = 25;
        image2.format = PPE_RGB888;
        image2.memory = (uint32_t*)SOURCE_PIC_2;
        image2.address = (uint32_t)image.memory;

        image3.width = 25;
        image3.height = 25;
        image3.format = PPE_RGB888;
        image3.memory = (uint32_t*)SOURCE_PIC_3;
        image3.address = (uint32_t)image.memory;

        buffer.format = PPE_RGB565;
        buffer.memory = (uint32_t*)PIC_OUTPUT_LAYER;
        buffer.address = (uint32_t)PIC_OUTPUT_LAYER;
        buffer.width = 64;
        buffer.height = 64;
        memset(PIC_OUTPUT_LAYER, 0, sizeof(PIC_OUTPUT_LAYER));

        PPE_translate_t trans = {20, 20};
        PPE_blend_multi(&image1, &image2, &image3, &buffer);
    }
 * \endcode
 */
PPE_ERR PPE_blend_multi(ppe_buffer_t *image1, ppe_buffer_t *image2,
                        ppe_buffer_t *buffer);

PPE_ERR PPE_recolor(ppe_buffer_t *image, ppe_buffer_t *buffer, uint32_t color);
PPE_ERR PPE_combine_image(ppe_buffer_t *image1, ppe_buffer_t *image2, ppe_buffer_t *buffer);
/** End of PPE_Exported_Functions
  * \}
  */

/** End of PPE
  * \}
  */

#ifdef __cplusplus
}
#endif

#endif /* RTL_PPE_H */
#endif
/******************* (C) COPYRIGHT 2023 Realtek Semiconductor Corporation *****END OF FILE****/