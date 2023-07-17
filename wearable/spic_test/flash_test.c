


#include "trace.h"
#include "flash_nor_device.h"
#include "os_mem.h"
#include "app_section.h"
#include "flash_test.h"


void flash_test_erase(void)
{
    FLASH_NOR_RET_TYPE ret = FLASH_NOR_RET_SUCCESS;

    for (uint16_t i = 0; i < FLASH_BLOCK_NUM; i++)
    {
        ret = flash_nor_erase_locked(FLASH_TEST_ADDR + FLASH_BLOCK_SIZE * i, FLASH_NOR_ERASE_BLOCK);
        if (ret != FLASH_NOR_RET_SUCCESS)
        {
            DBG_DIRECT("flash erase i error %d!", i, ret);
            return;
        }
    }
}

void flash_test_write(void)
{
    uint8_t *p_buf = malloc(sizeof(uint8_t) * FLASH_TEST_BUF_SIZE);
    if (p_buf == NULL)
    {
        DBG_DIRECT("flash_write_test malloc fail");
        return;
    }

    for (uint16_t i = 0; i < FLASH_TEST_BUF_SIZE; i++)
    {
        *(uint8_t *)(p_buf + i) = i;
    }

    FLASH_NOR_RET_TYPE ret = FLASH_NOR_RET_SUCCESS;
    for (uint16_t i = 0; i < FLASH_TEST_BUF_NUM; i++)
    {
        ret = flash_nor_write_locked(FLASH_TEST_ADDR + FLASH_TEST_BUF_SIZE * i, (uint8_t *)p_buf,
                                     FLASH_TEST_BUF_SIZE);
        if (ret != FLASH_NOR_RET_SUCCESS)
        {
            DBG_DIRECT("flash_nor_write_locked i error %d", i, ret);
            break;
        }
    }

    if (p_buf != NULL)
    {
        free(p_buf);
        p_buf = NULL;
    }
}

void flash_test_read(void)
{
    uint8_t *p_buf = malloc(sizeof(uint8_t) * FLASH_TEST_BUF_SIZE);

    FLASH_NOR_RET_TYPE ret = FLASH_NOR_RET_SUCCESS;
    for (int i = 0; i < FLASH_TEST_BUF_NUM; i++)
    {
        ret =  flash_nor_read_locked(FLASH_TEST_ADDR + FLASH_TEST_BUF_SIZE * i, p_buf, FLASH_TEST_BUF_SIZE);
        if (ret != FLASH_NOR_RET_SUCCESS)
        {
            DBG_DIRECT("flash_nor_read_locked i error %d", i, ret);
            break;
        }
    }

    if (p_buf != NULL)
    {
        free(p_buf);
        p_buf = NULL;
    }
}

void cpu_clock_switch(void)
{
    uint32_t actual_mhz;
    int32_t ret;
    ret = pm_cpu_freq_set(200, &actual_mhz);
    DBG_DIRECT("cpu: ret %d, actual_mhz %d", ret, actual_mhz);
}

void flash_test_clock_switch(void)
{
    extern void pf_set_clock_gen_rate(uint8_t clk_load, CLK_FREQ_TYPE clk_rate,
                                      CLK_FREQ_TYPE clk_rate_slow);
    pf_set_clock_gen_rate((KM4_ACTIVE_CLK_TYPE)(CLK_SPIC0), CLK_320MHZ, (CLK_FREQ_TYPE)0);
}

void flash_test_bit_mode_switch(void)
{
    int32_t ret = flash_nor_try_high_speed_mode(FLASH_NOR_IDX_SPIC0, FLASH_NOR_4_BIT_MODE);
    DBG_DIRECT("SPIC0 4 bit mode switch ret %d", ret);
}

RAM_FUNCTION
void flash_test(void)
{
    drv_tim6_init();
    drv_tim6_start();
    uint32_t start_time, stop_time;

    cpu_clock_switch();
    flash_test_clock_switch();
    flash_test_bit_mode_switch();

    start_time = drv_tim6_read();
    flash_test_erase();
    stop_time = drv_tim6_read();
    DBG_DIRECT("flash_test erase tim %d us", stop_time - start_time);

    start_time = drv_tim6_read();
    flash_test_write();
    stop_time = drv_tim6_read();
    DBG_DIRECT("flash_test write tim %d us", stop_time - start_time);

    start_time = drv_tim6_read();
    flash_test_read();
    stop_time = drv_tim6_read();
    DBG_DIRECT("flash_test read tim %d us", stop_time - start_time);
}


