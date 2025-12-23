/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_stb.c
  * @brief STB library implementation file
  * @details This file is used to compile STB library APIs. Since STB is a header-only
  *          library, the implementation macros (e.g., STB_IMAGE_IMPLEMENTATION) can only
  *          be defined once in the entire project. This file serves as the single
  *          compilation unit for STB library, allowing other files to use STB APIs
  *          by simply including the header files without the implementation macros.
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "gui_api.h"

/*============================================================================*
 *                        STB Image Configuration
 *============================================================================*/

/* Define STB_IMAGE_IMPLEMENTATION to include the implementation */
#define STB_IMAGE_IMPLEMENTATION

/* Only enable the image formats we need */
#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#define STBI_ONLY_BMP

/* Disable features we don't need */
#define STBI_NO_HDR
#define STBI_NO_LINEAR
#define STBI_NO_GIF
#define STBI_NO_PIC
#define STBI_NO_PSD
#define STBI_NO_TGA
#define STBI_NO_PNM
#define STBI_NO_THREAD_LOCALS

/* Use GUI memory allocation functions */
#include "gui_api.h"
#define STBI_MALLOC(sz)           gui_lower_malloc(sz)
#define STBI_REALLOC(p,newsz)     gui_lower_realloc(p, newsz)
#define STBI_FREE(p)              gui_lower_free(p)

/* Include STB image header */
#include "stb_image.h"



/************************ (C) COPYRIGHT Realtek Semiconductor Corporation *****END OF FILE****/
