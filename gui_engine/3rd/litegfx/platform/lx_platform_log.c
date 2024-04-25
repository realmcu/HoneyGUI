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
#include "lx_platform_log.h"
#include <stdio.h>
#include <assert.h>

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
*  GLOBAL VARIABLES
**********************/


/**********************
*      MACROS
**********************/


/**********************
*   GLOBAL FUNCTIONS
**********************/
void lx_platform_trace(const char *buf)
{
    LX_PRINTF(buf);
}

void lx_platform_assert(const char *file, uint32_t line)
{
    LX_PRINTF("Assert file:%s, line:%d", file, line);

    //assert(0);
}
/**********************
*   STATIC FUNCTIONS
**********************/
