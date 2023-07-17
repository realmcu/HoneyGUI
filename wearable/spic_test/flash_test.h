
#ifndef _FLASH_TEST_H_
#define _FLASH_TEST_H_

#include <flash_map.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FLASH_SECTOR_SIZE                           (4 * 1024)
#define FLASH_SECTOR_NUM                            1024
#define FLASH_BLOCK_SIZE                            (64 * 1024)
#define FLASH_BLOCK_NUM                             16 //1MBytes
#define FLASH_TEST_ADDR                             OTA_TMP_ADDR
#define FLASH_TEST_SIZE                             (FLASH_BLOCK_SIZE * FLASH_BLOCK_NUM)
#define FLASH_TEST_BUF_SIZE                         1024
#define FLASH_TEST_BUF_NUM                          1024


typedef enum _KM4_ACTIVE_CLK_TYPE
{
    /* set_clock_gen_rate support */
    CLK_KM4                 = 0,
    CLK_DSP                 = 1,
    CLK_SPIC0               = 2,
    CLK_SPIC1               = 3,
    CLK_SPIC2               = 4,
    CLK_SPIC3               = 5,
    /* ======================= */
    CLK_RTK_TIMER           = 6,
    CLK_SDIO0               = 7,
    CLK_SDIO1               = 8,
    CLK_DISPLAY_CTRL        = 9,
    CLK_USB                 = 10,
    CLK_LOW                 = 11,
    CLK_S5                  = 12,
    CLK_S7                  = 13,
    CLK_S11                 = 14,
    KM4_DVFS_LIMIT_CLK_MAX,
    KM4_CLK_MAX = KM4_DVFS_LIMIT_CLK_MAX,
} KM4_ACTIVE_CLK_TYPE;

typedef enum _CLK_FREQ_TYPE
{
    CLK_625KHZ,    //XTAL div64
    CLK_1P25MHZ,   //XTAL div32
    CLK_2P5MHZ,    //XTAL div16
    CLK_3P3MHZ,    //XTAL div12
    CLK_4MHZ,      // XTAL div10
    CLK_5MHZ,      // XTAL div8
    CLK_8MHZ,      // XTAL div5
    CLK_10MHZ,     // XTAL div4
    CLK_20MHZ,     // XTAL div2
    CLK_30MHZ,     // clock gen
    CLK_40MHZ,     // XTAL max freq
    CLK_50MHZ,
    CLK_53P3MHZ,   // clock gen
    CLK_60MHZ,     // PLL1,2,3 120MHz & clock gen div2
    CLK_70MHZ,     // clock gen
    CLK_80MHZ,     // PLL1,2 DIVN_SDM[6:0] = 18 div2, PLL3 DIVN_SDM[6:0] = 2 div2
    CLK_90MHZ,     // clock gen
    CLK_98P304MHZ, // PLL2 MCLK output
    CLK_100MHZ,    // PLL1, PLL2 DIVN_SDM[6:0] = 23 div2
    CLK_120MHZ,    // PLL1,2,DIVN_SDM[6:0] = 28 div2, PLL3, DIVN_SDM[6:0] = 4 div2
    CLK_140MHZ,    // clock gen
    CLK_160MHZ,    // PLL1,2,3 DIVN_SDM[6:0] = 18
    CLK_180MHZ,    // clock gen
    CLK_200MHZ,    // PLL1, PLL2 DIVN_SDM[6:0] = 23, PLL3 DIVN_SDM[6:0] = 3
    CLK_240MHZ,    // PLL2, PLL3 max freq DIVN_SDM[6:0] = 28
    CLK_280MHZ,    // PLL1, DIVN_SDM[6:0] = 33
    CLK_320MHZ,    // PLL1, DIVN_SDM[6:0] = 38
    CLK_360MHZ,    // PLL1, DIVN_SDM[6:0] = 43
    CLK_400MHZ,    // PLL1, DIVN_SDM[6:0] = 48
    CLK_480MHZ,    // PLL1 max freq, DIVN_SDM[6:0] = 58
    CLK_MAX_FREQ,
} CLK_FREQ_TYPE;


#ifdef __cplusplus
}
#endif

#endif /* _FLASH_TEST_H_ */

/************************** end of file ******************************/

