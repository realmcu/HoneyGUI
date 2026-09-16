/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include "lite_geometry_math.h"

uint16_t lg_atan2(int x, int y)
{
    uint8_t flags = 0;
    uint8_t compensation = 0;
    uint8_t degree8;
    unsigned int degree;
    unsigned int ux;
    unsigned int uy;

    if (x < 0)
    {
        flags |= 0x01u;
        x = -x;
    }
    ux = (unsigned int)x;

    if (y < 0)
    {
        flags |= 0x02u;
        y = -y;
    }
    uy = (unsigned int)y;

    if (ux > uy)
    {
        degree = (uy * 45u) / ux;
        flags |= 0x10u;
    }
    else
    {
        degree = (ux * 45u) / uy;
    }

    degree8 = (uint8_t)degree;
    if (degree8 > 22u)
    {
        if (degree8 <= 44u) { compensation++; }
        if (degree8 <= 41u) { compensation++; }
        if (degree8 <= 37u) { compensation++; }
        if (degree8 <= 32u) { compensation++; }
    }
    else
    {
        if (degree8 >= 2u) { compensation++; }
        if (degree8 >= 6u) { compensation++; }
        if (degree8 >= 10u) { compensation++; }
        if (degree8 >= 15u) { compensation++; }
    }
    degree += compensation;

    if (flags & 0x10u) { degree = 90u - degree; }

    if (flags & 0x02u)
    {
        degree = (flags & 0x01u) ? 180u + degree : 180u - degree;
    }
    else if (flags & 0x01u)
    {
        degree = 360u - degree;
    }

    return (uint16_t)degree;
}
