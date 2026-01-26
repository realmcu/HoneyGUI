/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include "lite_geometry_math.h"
/**********************
 *  STATIC VARIABLES
 **********************/
static const uint16_t sin0_90_table[] =
{
    0,     572,   1144,  1715,  2286,  2856,  3425,  3993,  4560,  5126,  5690,  6252,  6813,  7371,  7927,  8481,
    9032,  9580,  10126, 10668, 11207, 11743, 12275, 12803, 13328, 13848, 14365, 14876, 15384, 15886, 16384, 16877,
    17364, 17847, 18324, 18795, 19261, 19720, 20174, 20622, 21063, 21498, 21926, 22348, 22763, 23170, 23571, 23965,
    24351, 24730, 25102, 25466, 25822, 26170, 26510, 26842, 27166, 27482, 27789, 28088, 28378, 28660, 28932, 29197,
    29452, 29698, 29935, 30163, 30382, 30592, 30792, 30983, 31164, 31336, 31499, 31651, 31795, 31928, 32052, 32166,
    32270, 32365, 32449, 32524, 32588, 32643, 32688, 32723, 32748, 32763, 32768
};

/**********************
 *      MACROS
 **********************/
#define LG_TRIGO_SHIFT 15
/**********************
 *   GLOBAL FUNCTIONS
 **********************/

inline int32_t lg_trigo_sin(int16_t angle)//sin0_90_table
{
    int32_t ret = 0;
    while (angle < 0) { angle += 360; }
    while (angle >= 360) { angle -= 360; }

    if (angle < 90)
    {
        ret = sin0_90_table[angle];
    }
    else if (angle >= 90 && angle < 180)
    {
        angle = 180 - angle;
        ret   = sin0_90_table[angle];
    }
    else if (angle >= 180 && angle < 270)
    {
        angle = angle - 180;
        ret   = -sin0_90_table[angle];
    }
    else     /*angle >=270*/
    {
        angle = 360 - angle;
        ret   = -sin0_90_table[angle];
    }

    if (ret == 32767) { return 32768; }
    else if (ret == -32767) { return -32768; }
    else { return ret; }
}
int32_t lg_trigo_cos(int16_t angle)
{
    return lg_trigo_sin(angle + 90);
}
void lg_sqrt(uint32_t x, lg_sqrt_res_t *q, uint32_t mask)
{
    x = x << 8; /*To get 4 bit precision. (sqrt(256) = 16 = 4 bit)*/

    uint32_t root = 0;
    uint32_t trial;
    /*http://ww1.microchip.com/...en/AppNotes/91040a.pdf*/
    do
    {
        trial = root + mask;
        if (trial * trial <= x) { root = trial; }
        mask = mask >> 1;
    }
    while (mask);

    q->i = root >> 4;
    q->f = (root & 0xf) << 4;
}

/*
// Alternative Integer Square Root function
// Contributors include Arne Steinarson for the basic approximation idea,
// Dann Corbit and Mathew Hendry for the first cut at the algorithm,
// Lawrence Kirby for the rearrangement, improvements and range optimization
// and Paul Hsieh for the round-then-adjust idea.
*/
int32_t lg_sqrt32(uint32_t x)
{
    static const unsigned char sqq_table[] =
    {
        0,  16,  22,  27,  32,  35,  39,  42,  45,  48,  50,  53,  55,  57,
        59,  61,  64,  65,  67,  69,  71,  73,  75,  76,  78,  80,  81,  83,
        84,  86,  87,  89,  90,  91,  93,  94,  96,  97,  98,  99, 101, 102,
        103, 104, 106, 107, 108, 109, 110, 112, 113, 114, 115, 116, 117, 118,
        119, 120, 121, 122, 123, 124, 125, 126, 128, 128, 129, 130, 131, 132,
        133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 144, 145,
        146, 147, 148, 149, 150, 150, 151, 152, 153, 154, 155, 155, 156, 157,
        158, 159, 160, 160, 161, 162, 163, 163, 164, 165, 166, 167, 167, 168,
        169, 170, 170, 171, 172, 173, 173, 174, 175, 176, 176, 177, 178, 178,
        179, 180, 181, 181, 182, 183, 183, 184, 185, 185, 186, 187, 187, 188,
        189, 189, 190, 191, 192, 192, 193, 193, 194, 195, 195, 196, 197, 197,
        198, 199, 199, 200, 201, 201, 202, 203, 203, 204, 204, 205, 206, 206,
        207, 208, 208, 209, 209, 210, 211, 211, 212, 212, 213, 214, 214, 215,
        215, 216, 217, 217, 218, 218, 219, 219, 220, 221, 221, 222, 222, 223,
        224, 224, 225, 225, 226, 226, 227, 227, 228, 229, 229, 230, 230, 231,
        231, 232, 232, 233, 234, 234, 235, 235, 236, 236, 237, 237, 238, 238,
        239, 240, 240, 241, 241, 242, 242, 243, 243, 244, 244, 245, 245, 246,
        246, 247, 247, 248, 248, 249, 249, 250, 250, 251, 251, 252, 252, 253,
        253, 254, 254, 255
    };

    int32_t xn;

    if (x >= 0x10000)
        if (x >= 0x1000000)
            if (x >= 0x10000000)
                if (x >= 0x40000000)
                {
                    if (x >= 65535UL * 65535UL)
                    {
                        return 65535;
                    }
                    xn = sqq_table[x >> 24] << 8;
                }
                else
                {
                    xn = sqq_table[x >> 22] << 7;
                }
            else if (x >= 0x4000000)
            {
                xn = sqq_table[x >> 20] << 6;
            }
            else
            {
                xn = sqq_table[x >> 18] << 5;
            }
        else
        {
            if (x >= 0x100000)
                if (x >= 0x400000)
                {
                    xn = sqq_table[x >> 16] << 4;
                }
                else
                {
                    xn = sqq_table[x >> 14] << 3;
                }
            else if (x >= 0x40000)
            {
                xn = sqq_table[x >> 12] << 2;
            }
            else
            {
                xn = sqq_table[x >> 10] << 1;
            }

            goto nr1;
        }
    else if (x >= 0x100)
    {
        if (x >= 0x1000)
            if (x >= 0x4000)
            {
                xn = (sqq_table[x >> 8] >> 0) + 1;
            }
            else
            {
                xn = (sqq_table[x >> 6] >> 1) + 1;
            }
        else if (x >= 0x400)
        {
            xn = (sqq_table[x >> 4] >> 2) + 1;
        }
        else
        {
            xn = (sqq_table[x >> 2] >> 3) + 1;
        }

        goto adj;
    }
    else
    {
        return sqq_table[x] >> 4;
    }

    /* Run two iterations of the standard convergence formula */

    xn = (xn + 1 + x / xn) / 2;
nr1:
    xn = (xn + 1 + x / xn) / 2;
adj:

    if (xn * xn > (int32_t)x)  /* Correct rounding if necessary */
    {
        xn--;
    }

    return xn;
}
uint16_t lg_atan2(int x, int y)
{
    /**
     * Fast XY vector to integer degree algorithm - Jan 2011 www.RomanBlack.com
     * Converts any XY values including 0 to a degree value that should be
     * within +/- 1 degree of the accurate value without needing
     * large slow trig functions like ArcTan() or ArcCos().
     * NOTE! at least one of the X or Y values must be non-zero!
     * This is the full version, for all 4 quadrants and will generate
     * the angle in integer degrees from 0-360.
     * Any values of X and Y are usable including negative values provided
     * they are between -1456 and 1456 so the 16bit multiply does not overflow.
     */
    unsigned char negflag;
    unsigned char tempdegree;
    unsigned char comp;
    unsigned int degree;     /*this will hold the result*/
    unsigned int ux;
    unsigned int uy;

    /*Save the sign flags then remove signs and get XY as unsigned ints*/
    // printf("x=%d, y=%d, \n", x, y);
    negflag = 0;
    if (x < 0)
    {
        negflag += 0x01;    /*x flag bit*/
        x = (0 - x);        /*is now +*/
    }
    ux = x;                /*copy to unsigned var before multiply*/
    if (y < 0)
    {
        negflag += 0x02;    /*y flag bit*/
        y = (0 - y);        /*is now +*/
    }
    uy = y;                /*copy to unsigned var before multiply*/

    /*1. Calc the scaled "degrees"*/
    if (ux > uy)
    {
        degree = (uy * 45) / ux;   /*degree result will be 0-45 range*/
        negflag += 0x10;    /*octant flag bit*/
    }
    else
    {
        degree = (ux * 45) / uy;   /*degree result will be 0-45 range*/
    }

    /*2. Compensate for the 4 degree error curve*/
    comp = 0;
    tempdegree = degree;    /*use an unsigned char for speed!*/
    if (tempdegree > 22)     /*if top half of range*/
    {
        if (tempdegree <= 44) { comp++; }
        if (tempdegree <= 41) { comp++; }
        if (tempdegree <= 37) { comp++; }
        if (tempdegree <= 32) { comp++; } /*max is 4 degrees compensated*/
    }
    else     /*else is lower half of range*/
    {
        if (tempdegree >= 2) { comp++; }
        if (tempdegree >= 6) { comp++; }
        if (tempdegree >= 10) { comp++; }
        if (tempdegree >= 15) { comp++; } /*max is 4 degrees compensated*/
    }
    degree += comp;   /*degree is now accurate to +/- 1 degree!*/

    /*Invert degree if it was X>Y octant, makes 0-45 into 90-45*/
    if (negflag & 0x10) { degree = (90 - degree); }

    /*3. Degree is now 0-90 range for this quadrant,*/
    /*need to invert it for whichever quadrant it was in*/
    if (negflag & 0x02)  /*if -Y*/
    {
        if (negflag & 0x01)  /*if -Y -X*/
        {
            degree = (180 + degree);
        }
        else        /*else is -Y +X*/
        {
            degree = (180 - degree);
        }
    }
    else     /*else is +Y*/
    {
        if (negflag & 0x01)  /*if +Y -X*/
        {
            degree = (360 - degree);
        }
    }
    return degree;
}
int64_t lg_pow(int64_t base, int8_t exp)
{
    int64_t result = 1;
    while (exp)
    {
        if (exp & 1)
        {
            result *= base;
        }
        exp >>= 1;
        base *= base;
    }

    return result;
}
