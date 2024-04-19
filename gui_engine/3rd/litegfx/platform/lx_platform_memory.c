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


/*********************
*      INCLUDES
*********************/
#include "lx_platform_memory.h"
#include "lx_platform_log.h"
#include <gui_api.h>
#include "tlsf.h"

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
static char lx_int_mem_buffer[LX_INT_MEM_SIZE];

static tlsf_t lx_int_mem_tlsf;
static bool lx_int_mem_inited = false;

/**********************
*  GLOBAL VARIABLES
**********************/


/**********************
*      MACROS
**********************/


/**********************
*   GLOBAL FUNCTIONS
**********************/
void lx_platform_init()
{

}

void *lx_platform_malloc(size_t size)
{
    void *ptr = NULL;

    if (size != 0)
    {
        ptr = gui_malloc(size);

        if (ptr != NULL)
        {
            memset(ptr, 0, size);
        }
        else
        {
            LX_LOG_ERROR("lx_platform_malloc failed!!!, size:%d", size);
        }
    }

    return (void *)ptr;
}

void *lx_platform_calloc(size_t num, size_t size)
{
    void *ptr = NULL;

    if (num != 0 && size != 0)
    {
        ptr = gui_malloc(size * num);
    }

    if (ptr != NULL)
    {
        memset(ptr, 0, size * num);
    }
    else
    {
        LX_LOG_ERROR("lx_platform_malloc failed!!!, size:%d", size * num);
    }

    return (void *)ptr;
}

void *lx_platform_realloc(void *ptr, size_t size)
{
    void *new_ptr = NULL;

    new_ptr = gui_realloc(ptr, size);

    return new_ptr;
}

void lx_platform_free(void *ptr)
{
    gui_free(ptr);
}

void *lx_platform_int_malloc(size_t size)
{
    void *ptr = NULL;

    if (lx_int_mem_inited == false)
    {
        lx_int_mem_inited = true;

        lx_int_mem_tlsf = tlsf_create_with_pool(&lx_int_mem_buffer[0], LX_INT_MEM_SIZE);
    }

    ptr = tlsf_malloc(lx_int_mem_tlsf, size);

    if (ptr != NULL)
    {
        memset(ptr, 0, size);
    }
    else
    {
        LX_LOG_WARN("lx_platform_int_malloc failed!!!, size:%d", size);
    }

    return (void *)ptr;
}

void lx_platform_int_free(void *ptr)
{
    tlsf_free(lx_int_mem_tlsf, ptr);
}

/**********************
*   STATIC FUNCTIONS
**********************/
