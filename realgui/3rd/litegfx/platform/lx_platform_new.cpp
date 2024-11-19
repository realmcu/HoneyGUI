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

#if 0//ndef _WIN32
#include "lx_platform_memory.h"
#include <gui_api.h>

//#define LITEGFX_MEMORY_DEBUG (1)

extern "C" void LxMemPrint();

#if defined(LITEGFX_MEMORY_DEBUG)
#include <etl/vector.h>

static bool memVectorInited = false;
static etl::vector<void *, 1024> memVector;

static void memVectorAdd(void *ptr)
{
    if (memVectorInited == false)
    {
        memVectorInited = true;
        memVector.clear();
    }

    memVector.push_back(ptr);
}

static void memVectorRemove(void *ptr)
{
    for (int i = 0; i < memVector.size(); i++)
    {
        if (memVector[i] == ptr)
        {
            memVector.erase(memVector.begin() + i);

            break;
        }
    }
}
#endif


void *operator new (size_t size)
{
    void *ptr = lx_platform_malloc(size);

#if defined(LITEGFX_MEMORY_DEBUG)
    memVectorAdd(ptr);
#endif

    return ptr;
}

void *operator new[](size_t size)
{
    void *ptr = lx_platform_malloc(size);

#if defined(LITEGFX_MEMORY_DEBUG)
    memVectorAdd(ptr);
#endif

    return ptr;
}

void operator delete (void *ptr)
{
#if defined(LITEGFX_MEMORY_DEBUG)
    memVectorRemove(ptr);
#endif

    lx_platform_free(ptr);
}

void operator delete[](void *ptr)
{
#if defined(LITEGFX_MEMORY_DEBUG)
    memVectorRemove(ptr);
#endif

    lx_platform_free(ptr);
}

void LxMemPrint()
{
#if defined(LITEGFX_MEMORY_DEBUG)
    for (int i = 0; i < memVector.size(); i++)
    {
        gui_log("Litegfx memVector[%d]:0x%x\r\n", i, memVector[i]);
    }
#else
    gui_log("Please enable LITEGFX_MEMORY_DEBUG");
#endif
}
#endif
