/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_stb.h
  * @brief STB library API header file
  * @details This header provides STB library API declarations and constants.
  *          Include this header to use STB image loading and resizing functions.
  *          The implementation is in gui_stb.c - do NOT define STB_IMAGE_IMPLEMENTATION
  *          in any other file.
  * @author howie_wang@realsil.com.cn
  * @date 2024/12/22
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __GUI_STB_H__
#define __GUI_STB_H__

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <stddef.h>

/*============================================================================*
 *                        Constants
 *============================================================================*/

/** @brief Request auto-detect number of components */
#define STBI_DEFAULT        0

/** @brief Request grey component only */
#define STBI_GREY           1

/** @brief Request grey + alpha components */
#define STBI_GREY_ALPHA     2

/** @brief Request RGB components */
#define STBI_RGB            3

/** @brief Request RGBA components */
#define STBI_RGBA           4

/*============================================================================*
 *                        STB Image API Declarations
 *============================================================================*/

/**
 * @brief Load image from file
 */
unsigned char *stbi_load(char const *filename, int *x, int *y, int *channels_in_file,
                         int desired_channels);

/**
 * @brief Load image from memory buffer
 */
unsigned char *stbi_load_from_memory(unsigned char const *buffer, int len, int *x, int *y,
                                     int *channels_in_file, int desired_channels);

/**
 * @brief Free image data
 */
void stbi_image_free(void *retval_from_stbi_load);

/**
 * @brief Get error message for last failed operation
 */
const char *stbi_failure_reason(void);

/**
 * @brief Get image info from file without loading pixel data
 */
int stbi_info(char const *filename, int *x, int *y, int *comp);

/**
 * @brief Get image info from memory without loading pixel data
 */
int stbi_info_from_memory(unsigned char const *buffer, int len, int *x, int *y, int *comp);



#ifdef __cplusplus
}
#endif

#endif /* __GUI_STB_H__ */

/************************ (C) COPYRIGHT Realtek Semiconductor Corporation *****END OF FILE****/
