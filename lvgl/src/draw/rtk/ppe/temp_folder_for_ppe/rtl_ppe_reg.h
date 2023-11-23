#ifndef RTL_PPE_REG_H
#define RTL_PPE_REG_H

#include <stdint.h>
#include <stdbool.h>
#include "rtl876x.h"

#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ================================================================================ */
/* ============= Refence: PPE.xlsx =============== */
/* ================================================================================ */

typedef struct
{
    __IO uint32_t
    LAYER0_ADDR_L;                                                                                //0x00
    __IO uint32_t
    LAYER0_ADDR_H;                                                                                //0x04
    __IO uint32_t
    LAYER0_RSVD1;                                                                                 //0x08
    __IO uint32_t
    LAYER0_WIN_SIZE;                                                                              //0x0C
    __IO uint32_t
    LAYER0_RSVD2;                                                                                 //0x10
    __IO uint32_t
    LAYER0_PIC_CFG;                                                                               //0x14
    __IO uint32_t
    LAYER0_RSVD3;                                                                                 //0x18
    __IO uint32_t
    LAYER0_BUS_CFG;                                                                               //0x1C
    __IO uint32_t
    LAYER0_HS_CFG;                                                                                //0x20
    __I  uint32_t  RSVD4[7];
} PPE_RESULT_LAYER_TypeDef;

typedef struct
{
    __IO uint32_t
    LAYERx_ADDR_L;                                                                                //0x00 + x*0x40
    __IO uint32_t
    LAYERx_ADDR_H;                                                                                //0x04 + x*0x40
    __IO uint32_t
    LAYERx_POS;                                                                                   //0x08 + x*0x40
    __IO uint32_t
    LAYERx_WIN_SIZE;                                                                              //0x0c + x*0x40
    __IO uint32_t
    LAYERx_CONST_PIX;                                                                             //0x10 + x*0x40
    __IO uint32_t
    LAYERx_PIC_CFG;                                                                               //0x14 + x*0x40
    __IO uint32_t
    LAYERx_KEY_COLOR;                                                                             //0x18 + x*0x40
    __IO uint32_t
    LAYERx_BUS_CFG;                                                                               //0x1c + x*0x40
    __IO uint32_t
    LAYERx_HS_CFG;                                                                                //0x20 + x*0x40
    __I  uint32_t  RSVD2[7];
} PPE_INPUT_LAYER_TypeDef;

typedef struct
{
    PPE_RESULT_LAYER_TypeDef RESULT_LAYER;
    PPE_INPUT_LAYER_TypeDef INPUT_LAYER[15];
} PPE_LAYER_TypeDef;

typedef struct
{
    __IO uint32_t
    GLB_CTL;                                                                                      //0x400
    __IO uint32_t
    FUNC_CFG;                                                                                     //0x404
    __IO uint32_t
    RELOAD_CFG;                                                                                   //0x408
    __IO uint32_t
    LL_CFG;                                                                                       //0x40C
    __IO uint32_t
    LLP;                                                                                          //0x410
    __IO uint32_t
    RSVD1;                                                                                        //0x414
    __IO uint32_t
    SCA_RATIO_X;                                                                                  //0x418
    __IO uint32_t
    SCA_RATIO_Y;                                                                                  //0x41C
    __IO uint32_t
    MISC;                                                                                         //0x420
    __IO uint32_t
    LINE_INDEX;                                                                                   //0x424
    __I  uint32_t  RSVD[7];
    __IO uint32_t
    INTR_ST;                                                                                      //0x440
    __IO uint32_t
    INTR_RAW;                                                                                     //0x444
    __IO uint32_t
    INTR_CLR;                                                                                     //0x448
    __IO uint32_t
    INTR_MASK;                                                                                    //0x44C
} PPE_TypeDef;

/* 0x00
    31:0    R/W    addr                undefined
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t addr: 32;
    } b;
} PPE_LAYER0_ADDR_L_t;



/* 0x04
    31:0    R      reserved            0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __I uint32_t reserved_0: 32;
    } b;
} PPE_LAYER0_ADDR_H_t;



/* 0x08
    31:0    R      reserved            0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __I uint32_t reserved_0: 32;
    } b;
} PPE_LAYER0_RSVD1_t;



/* 0x0C
    15:0    R/W    width               0x1
    31:16   R/W    height              0x1
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t width: 16;
        __IO uint32_t height: 16;
    } b;
} PPE_LAYER0_WIN_SIZE_t;



/* 0x10
    31:0    R      reserved            0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __I uint32_t reserved_0: 32;
    } b;
} PPE_LAYER0_RSVD2_t;



/* 0x14
    7:0     R/W    format              0x0
    15:8    R      reserved            0x1
    31:16   R/W    line_len            0x1
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t format: 8;
        __I uint32_t reserved_0: 8;
        __IO uint32_t line_len: 16;
    } b;
} PPE_LAYER0_PIC_CFG_t;



/* 0x18
    31:0    R/W    reserved            0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t reserved_0: 32;
    } b;
} PPE_LAYER0_RSVD3_t;



/* 0x1C
    2:0     R/W    axsize              0x2
    3       R/W    incr                0x1
    7:4     R/W    axcache             0x0
    10:8    R/W    max_axlen_log       0x7
    11      R      reserved2           0x0
    15:12   R/W    prior               0x0
    23:16   R      reserved1           0x0
    31:24   R      byte_swap           0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t axsize: 3;
        __IO uint32_t incr: 1;
        __IO uint32_t axcache: 4;
        __IO uint32_t max_axlen_log: 3;
        __I uint32_t reserved_1: 1;
        __IO uint32_t prior: 4;
        __I uint32_t reserved_0: 8;
        __IO uint32_t byte_swap: 8;
    } b;
} PPE_LAYER0_BUS_CFG_t;



/* 0x20
    0       R      hw_hs               0x0
    1       R/W    polar               0x0
    2       R/W    hs_en               0x0
    3       R      reserved2           0x0
    7:4     R/W    msize_log           0x0
    15:8    R/W    hw_index            0x0
    31:16   R      reserved1           0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t hw_hs: 1;
        __IO uint32_t polar: 1;
        __IO uint32_t hs_en: 1;
        __I uint32_t reserved_1: 1;
        __IO uint32_t msize_log: 4;
        __IO uint32_t hw_index: 8;
        __I uint32_t reserved_0: 16;
    } b;
} PPE_LAYER0_HS_CFG_t;

//__IO uint32_t RSVD_0X24[7];



/* 0x40
    31:0    R/W    addr                undefined
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t addr: 32;
    } b;
} PPE_LAYERX_ADDR_L_t;



/* 0x44
    31:0    R      reserved            0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __I uint32_t reserved_0: 32;
    } b;
} PPE_LAYERX_ADDR_H_t;



/* 0x48
    15:0    R/W    start_x             0x0
    31:16   R/W    start_y             0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t start_x: 16;
        __IO uint32_t start_y: 16;
    } b;
} PPE_LAYERX_POS_t;



/* 0x4C
    15:0    R/W    width               0x1
    31:16   R/W    height              0x1
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t width: 16;
        __IO uint32_t height: 16;
    } b;
} PPE_LAYERX_WIN_SIZE_t;



/* 0x50
    31:0    R/W    const_pix           0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t const_pix: 32;
    } b;
} PPE_LAYERX_CONST_PIX_t;



/* 0x54
    7:0     R/W    format              0x0
    8       R/W    pix_src             0x0
    11:9    R      reserved2           0x0
    12      R/W    key_en              0x0
    15:13   R      reserved1           0x0
    31:16   R/W    line_len            0x1
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t format: 8;
        __IO uint32_t pix_src: 1;
        __I uint32_t reserved_1: 3;
        __IO uint32_t key_en: 1;
        __I uint32_t reserved_0: 3;
        __IO uint32_t line_len: 16;
    } b;
} PPE_LAYERX_PIC_CFG_t;



/* 0x58
    31:0    R/W    key_color           undefined
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t key_color: 32;
    } b;
} PPE_LAYERX_KEY_COLOR_t;



/* 0x5C
    2:0     R/W    axsize              0x2
    3       R/W    incr                0x1
    7:4     R/W    axcache             0x0
    10:8    R/W    max_axlen_log       0x7
    11      R      reserved2           0x0
    15:12   R/W    prior               0x0
    31:16   R      reserved1           0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t axsize: 3;
        __IO uint32_t incr: 1;
        __IO uint32_t axcache: 4;
        __IO uint32_t max_axlen_log: 3;
        __I uint32_t reserved_1: 1;
        __IO uint32_t prior: 4;
        __I uint32_t reserved_0: 16;
    } b;
} PPE_LAYERX_BUS_CFG_t;



/* 0x60
    0       R/W    hwhs                0x0
    1       R/W    polar               0x0
    2       R/W    hs_en               0x0
    3       R      reserved2           0x0
    7:4     R/W    msize_log           0x0
    15:8    R/W    hw_index            0x0
    31:16   R      reserved1           0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t hwhs: 1;
        __IO uint32_t polar: 1;
        __IO uint32_t hs_en: 1;
        __I uint32_t reserved_1: 1;
        __IO uint32_t msize_log: 4;
        __IO uint32_t hw_index: 8;
        __I uint32_t reserved_0: 16;
    } b;
} PPE_LAYERX_HS_CFG_t;

//__IO uint32_t RSVD_0X64[231];



/* 0x400
    0       R/W1C  glb_en              0x0
    1       W1C    abort               0x0
    2       R/W1C  susp                0x0
    3       W1C    resume              0x0
    4       R      all_inact           0x1
    14:5    R      reserved            0x0
    31:15   R      chn_inact           0x1FFFF
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t glb_en: 1;
        __IO uint32_t abort: 1;
        __IO uint32_t susp: 1;
        __IO uint32_t resume: 1;
        __I uint32_t all_inact: 1;
        __I uint32_t reserved_0: 10;
        __I uint32_t chn_inact: 17;
    } b;
} PPE_GLB_CTL_t;



/* 0x404
    3:0     R/W    func_sel            0x0
    7:4     R/W    blend_lay           0x1
    31:8    R      reserved            0x1
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t func_sel: 4;
        __IO uint32_t blend_lay: 4;
        __I uint32_t reserved_0: 24;
    } b;
} PPE_FUNC_CFG_t;



/* 0x408
    15:0    R/W    layer_reload_en     0x0
    16      R/W1C  set_valid           0x0
    17      R/W    auto_clr            0x1
    31:18   R      reserved            0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t layer_reload_en: 16;
        __IO uint32_t set_valid: 1;
        __IO uint32_t auto_clr: 1;
        __I uint32_t reserved_0: 14;
    } b;
} PPE_RELOAD_CFG_t;



/* 0x40C
    15:0    R/W    layer_ll_en         0x0
    18:16   R      ll_arsize           0x2
    19      R      ll_incr             0x1
    23:20   R      ll_arcache          0x2
    26:24   R      ll_max_arlen_log    0x7
    27      R      reserved            0x0
    31:28   R      ll_prior            0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t layer_ll_en: 16;
        __I uint32_t ll_arsize: 3;
        __I uint32_t ll_incr: 1;
        __I uint32_t ll_arcache: 4;
        __I uint32_t ll_max_arlen_log: 3;
        __I uint32_t reserved_0: 1;
        __I uint32_t ll_prior: 4;
    } b;
} PPE_LL_CFG_t;



/* 0x410
    31:0    R/W    llp                 undefined
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t llp: 32;
    } b;
} PPE_LLP_t;



/* 0x414
    31:0    R      reserved            0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __I uint32_t reserved_0: 32;
    } b;
} PPE_RSVD1_t;



/* 0x418
    31:0    R/W    sca_ratio_x         0x10000
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t sca_ratio_x: 32;
    } b;
} PPE_SCA_RATIO_X_t;



/* 0x41C
    31:0    R/W    sca_ratio_y         0x10000
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t sca_ratio_y: 32;
    } b;
} PPE_SCA_RATIO_Y_t;



/* 0x420
    0       R/W    secure              0x0
    31:1    R      reserved            0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t secure: 1;
        __I uint32_t reserved_0: 31;
    } b;
} PPE_MISC_t;



/* 0x424
    15:0    R/W    line_index          0xFFF
    31:16   R      reserved            0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t line_index: 16;
        __I uint32_t reserved_0: 16;
    } b;
} PPE_LINE_INDEX_t;

//__IO uint32_t RSVD_0X428[6];



/* 0x440
    0       R      all_over            0x0
    1       R      fr_over             0x0
    2       R      load_over           0x0
    3       R      line_over           0x0
    4       R      susp_inac           0x0
    5       R      secure_err          0x0
    6       R      set_err             0x0
    14:7    R      reserved            0x0
    31:15   R      chn_bus_err         0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __I uint32_t all_over: 1;
        __I uint32_t fr_over: 1;
        __I uint32_t load_over: 1;
        __I uint32_t line_over: 1;
        __I uint32_t susp_inac: 1;
        __I uint32_t secure_err: 1;
        __I uint32_t set_err: 1;
        __I uint32_t reserved_0: 8;
        __I uint32_t chn_bus_err: 17;
    } b;
} PPE_INTR_ST_t;



/* 0x444
    0       R      all_over            0x0
    1       R      fr_over             0x0
    2       R      load_over           0x0
    3       R      line_over           0x0
    4       R      susp_inac           0x0
    5       R      secure_err          0x0
    6       R      set_err             0x0
    14:7    R      reserved            0x0
    31:15   R      chn_bus_err         0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __I uint32_t all_over: 1;
        __I uint32_t fr_over: 1;
        __I uint32_t load_over: 1;
        __I uint32_t line_over: 1;
        __I uint32_t susp_inac: 1;
        __I uint32_t secure_err: 1;
        __I uint32_t set_err: 1;
        __I uint32_t reserved_0: 8;
        __I uint32_t chn_bus_err: 17;
    } b;
} PPE_INTR_RAW_t;



/* 0x448
    0       W1C    all_over            0x0
    1       W1C    fr_over             0x0
    2       W1C    load_over           0x0
    3       W1C    line_over           0x0
    4       W1C    susp_inac           0x0
    5       W1C    secure_err          0x0
    6       W1C    set_err             0x0
    14:7    R      reserved            0x0
    31:15   W1C    chn_bus_err         0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t all_over: 1;
        __IO uint32_t fr_over: 1;
        __IO uint32_t load_over: 1;
        __IO uint32_t line_over: 1;
        __IO uint32_t susp_inac: 1;
        __IO uint32_t secure_err: 1;
        __IO uint32_t set_err: 1;
        __I uint32_t reserved_0: 8;
        __IO uint32_t chn_bus_err: 17;
    } b;
} PPE_INTR_CLR_t;



/* 0x44C
    0       R/W    all_over            0x0
    1       R/W    fr_over             0x0
    2       R/W    load_over           0x0
    3       R/W    line_over           0x0
    4       R/W    susp_inac           0x0
    5       R/W    secure_err          0x0
    6       R/W    set_err             0x0
    14:7    R      reserved            0x0
    31:15   R/W    chn_bus_err         0x0
*/
typedef union
{
    uint32_t d32;
    uint8_t d8[4];
    struct
    {
        __IO uint32_t all_over: 1;
        __IO uint32_t fr_over: 1;
        __IO uint32_t load_over: 1;
        __IO uint32_t line_over: 1;
        __IO uint32_t susp_inac: 1;
        __IO uint32_t secure_err: 1;
        __IO uint32_t set_err: 1;
        __I uint32_t reserved_0: 8;
        __IO uint32_t chn_bus_err: 17;
    } b;
} PPE_INTR_MASK_t;


#ifdef  __cplusplus
}
#endif /* __cplusplus */
#endif /* RTL_PPE_REG_H */
