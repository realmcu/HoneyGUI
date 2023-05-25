#include "trace.h"
#include "pmu_sample_code.h"
//#include "data_uart.h"
/* DEFINED MACRO */
/* PMU REG*/
#define PMU_BASE_PHY           0xE0003000
#define DEMCR_ADDR             0xE000EDFC
#define PMU_CTRL_OFFSET        0xE04          /* offset */
#define PMU_CNTENCLR_OFFSET    0xC20          /* offset */
#define PMU_INTENCLR_OFFSET    0xC60          /* offset */
#define PMU_EVTYPER0_OFFSET    0x400          /* offset */
#define PMU_EVTYPER1_OFFSET    0x404          /* offset */
#define PMU_EVTYPER2_OFFSET    0x408          /* offset */
#define PMU_EVTYPER3_OFFSET    0x40C          /* offset */
#define PMU_EVTYPER4_OFFSET    0x410          /* offset */
#define PMU_EVTYPER5_OFFSET    0x414          /* offset */
#define PMU_EVTYPER6_OFFSET    0x418          /* offset */
#define PMU_EVTYPER7_OFFSET    0x41C          /* offset */
#define PMU_CNTENSET_OFFSET    0xC00          /* offset */
#define PMU_OVSCLR_OFFSET      0xC80          /* offset */
#define PMU_EVCNTR0_OFFSET     0x000          /* offset */
#define PMU_EVCNTR1_OFFSET     0x004          /* offset */
#define PMU_EVCNTR2_OFFSET     0x008          /* offset */
#define PMU_EVCNTR3_OFFSET     0x00C          /* offset */
#define PMU_EVCNTR4_OFFSET     0x010          /* offset */
#define PMU_EVCNTR5_OFFSET     0x014          /* offset */
#define PMU_EVCNTR6_OFFSET     0x018          /* offset */
#define PMU_EVCNTR7_OFFSET     0x01C          /* offset */

/* Reg R/W*/
void valWritePmuReg(uint32_t regOffset, uint32_t value)
{
    *((uint32_t volatile *)(PMU_BASE_PHY + regOffset)) = (uint32_t)value;
}

void valWriteMem(uint32_t addr, uint32_t value)
{
    *((uint32_t volatile *)(addr)) = (uint32_t)value;
}

uint32_t valReadPmuReg(uint32_t regOffset)
{
    uint32_t ret_val = 0;
    ret_val = ((uint32_t)(*((uint32_t volatile *)(PMU_BASE_PHY + regOffset))));
    return ret_val;
}

/* PMU init */
void init_pmu_counter(void)
{
    // Set DEMCR.TRCENA and DEMCR.SDME for cycle counter counting
    valWriteMem(DEMCR_ADDR, 0x01100000);
    // Disable all counters
    valWritePmuReg(PMU_CTRL_OFFSET, 0x0000000F); // reset and E=1
    valWritePmuReg(PMU_CNTENCLR_OFFSET, 0xFFFFFFFF);
    valWritePmuReg(PMU_INTENCLR_OFFSET, 0xFFFFFFFF);

    /*Part I: Check chained 32-bit event counter value
     Reset counter
    */
    valWritePmuReg(PMU_CTRL_OFFSET, 0x0000000F); // reset and E=1
    // Clear overflow flags
    valWritePmuReg(PMU_OVSCLR_OFFSET, 0xffffffff);
    // Set the types of event counters

    valWritePmuReg(PMU_EVTYPER0_OFFSET, 0x1036); // D-CACHE_HIT
    valWritePmuReg(PMU_EVTYPER1_OFFSET, 0x1E);   // Chain
    valWritePmuReg(PMU_EVTYPER2_OFFSET, 0x3);    // D-CACHE_REFILL
    valWritePmuReg(PMU_EVTYPER3_OFFSET, 0x1E);   // Chain
    valWritePmuReg(PMU_EVTYPER4_OFFSET, 0x1030); // I-CACHE_HIT
    valWritePmuReg(PMU_EVTYPER5_OFFSET, 0x1E);   // Chain
    valWritePmuReg(PMU_EVTYPER6_OFFSET, 0x1031); // I-CACHE_MISS
    valWritePmuReg(PMU_EVTYPER7_OFFSET, 0x1E);   // Chain


    // Assign initial value to event counter 0
    valWritePmuReg(PMU_EVCNTR0_OFFSET, 0x0000);
    valWritePmuReg(PMU_EVCNTR1_OFFSET, 0x0000);
    valWritePmuReg(PMU_EVCNTR2_OFFSET, 0x0000);
    valWritePmuReg(PMU_EVCNTR3_OFFSET, 0x0000);
    valWritePmuReg(PMU_EVCNTR4_OFFSET, 0x0000);
    valWritePmuReg(PMU_EVCNTR5_OFFSET, 0x0000);
    valWritePmuReg(PMU_EVCNTR6_OFFSET, 0x0000);
    valWritePmuReg(PMU_EVCNTR7_OFFSET, 0x0000);

    // Turn on event counter 0 ~ 7
    valWritePmuReg(PMU_CNTENSET_OFFSET, 0xFF);

}

/* Turn off PMU and get PMU counter val*/
void turnoff_pmu_print_counter(void)
{
    // Turn off all counters
    valWritePmuReg(PMU_CNTENCLR_OFFSET, 0xFFFFFFFF);
}

void pmu_print_counter(void)
{
    uint32_t dcache_hit_cnt = (valReadPmuReg(PMU_EVCNTR1_OFFSET) << 16) | valReadPmuReg(
                                  PMU_EVCNTR0_OFFSET);
    uint32_t dcache_miss_cnt = (valReadPmuReg(PMU_EVCNTR3_OFFSET) << 16) | valReadPmuReg(
                                   PMU_EVCNTR2_OFFSET);

    uint16_t icache_hit_cnt = (valReadPmuReg(PMU_EVCNTR5_OFFSET) << 16) | valReadPmuReg(
                                  PMU_EVCNTR4_OFFSET);
    uint16_t icache_miss_cnt = (valReadPmuReg(PMU_EVCNTR7_OFFSET) << 16) | valReadPmuReg(
                                   PMU_EVCNTR6_OFFSET);

    DBG_DIRECT("D-cache HIT cnt      %d", dcache_hit_cnt);
    DBG_DIRECT("D-cache MISS cnt     %d", dcache_miss_cnt);
    DBG_DIRECT("I-cache HIT cnt      %d", icache_hit_cnt);
    DBG_DIRECT("I-cache MISS cnt     %d", icache_miss_cnt);

    DBG_DIRECT("D cache hit: (*100)%d",
               (uint32_t)(dcache_hit_cnt * 100) / (dcache_hit_cnt + dcache_miss_cnt));

    DBG_DIRECT("I cache hit: (*100)%d",
               (uint32_t)(icache_hit_cnt * 100) / (icache_hit_cnt + icache_miss_cnt));
}
