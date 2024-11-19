/******************************************************************************
* Copyright (c) 2021 Shanghai QDay Technology Co., Ltd.
* All rights reserved.
*
* This file is part of the LiteGFX 0.0.1 distribution.
*
* This software is licensed under terms that can be found in the LICENSE file in
* the root directory of this software component.
* If no LICENSE file comes with this software, it is provided AS-IS.
*
* Author:LiteGFX Team
* Date:2021.12.05
*******************************************************************************/
#ifndef LX_PLATFORM_MEMORY_H
#define LX_PLATFORM_MEMORY_H

#if defined(__cplusplus)
extern "C" {
#endif


/*********************
*      INCLUDES
*********************/
#include <stdlib.h>
#include <string.h>

/*********************
*      DEFINES
*********************/
#define LX_INT_MEM_SIZE     (512)//这部分SRAM用于提升帧率，默认值为32K，越大帧率优化的效果越好，客户可以根据实际情况修改大小

/**********************
*      TYPEDEFS
**********************/


/**********************
* GLOBAL PROTOTYPES
**********************/
void lx_platform_init();

void *lx_platform_malloc(size_t size);

void *lx_platform_calloc(size_t num, size_t size);

void *lx_platform_realloc(void *ptr, size_t size);

void lx_platform_free(void *ptr);

void *lx_platform_get_cache(void);

size_t lx_platform_get_cache_size(void);

#if defined(__cplusplus)
}
#endif

#endif // LX_PLATFORM_MEMORY_H
