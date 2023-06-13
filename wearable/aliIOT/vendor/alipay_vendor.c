#include "alipay_vendor.h"

#include "stdio.h"
#include "upay_service.h"
#include "ftl.h"
#include "upay_linklist.h"
#include "stdarg.h"
#include "rtthread.h"

#define FTL_UPAY_ITEM_STATUS_OFFSET               (USER_DEFINE)
#define FTL_UPAY_ITEM_STATUS_SIZE                 4
#define FTL_UPAY_ITEM_PRIVATE_KEY_OFFSET          (FTL_UPAY_ITEM_STATUS_OFFSET + FTL_UPAY_ITEM_STATUS_SIZE)
#define FTL_UPAY_ITEM_PRIVATE_KEY_SIZE            512
#define FTL_UPAY_ITEM_SHARE_KEY_OFFSET            (FTL_UPAY_ITEM_PRIVATE_KEY_OFFSET + FTL_UPAY_ITEM_PRIVATE_KEY_SIZE)
#define FTL_UPAY_ITEM_SHARE_KEY_SIZE              512
#define FTL_UPAY_ITEM_SEED_OFFSET                 (FTL_UPAY_ITEM_SHARE_KEY_OFFSET + FTL_UPAY_ITEM_SHARE_KEY_SIZE)
#define FTL_UPAY_ITEM_SEED_SIZE                   512
#define FTL_UPAY_ITEM_TIMEDIFF_OFFSET             (FTL_UPAY_ITEM_SEED_OFFSET + FTL_UPAY_ITEM_SEED_SIZE)
#define FTL_UPAY_ITEM_TIMEDIFF_SIZE               8
#define FTL_UPAY_ITEM_NICKNAME_OFFSET             (FTL_UPAY_ITEM_TIMEDIFF_OFFSET + FTL_UPAY_ITEM_TIMEDIFF_SIZE)
#define FTL_UPAY_ITEM_NICKNAME_SIZE               128
#define FTL_UPAY_ITEM_LOGONID_OFFSET              (FTL_UPAY_ITEM_NICKNAME_OFFSET + FTL_UPAY_ITEM_NICKNAME_SIZE)
#define FTL_UPAY_ITEM_LOGONID_SIZE                128
#define FTL_UPAY_ITEM_BINDFLAG_OFFSET             (FTL_UPAY_ITEM_LOGONID_OFFSET + FTL_UPAY_ITEM_LOGONID_SIZE)
#define FTL_UPAY_ITEM_BINDFLAG_SIZE               4


retval_t UPAY_storage_read(uint32_t item, uint8_t *buf_content, uint32_t *len_content)
{
    RT_ASSERT(RT_NULL != RT_NULL);
    return RV_OK;
}

retval_t UPAY_storage_write(uint32_t item,
                            const uint8_t *buf_content, uint32_t len_content)
{
    RT_ASSERT(RT_NULL != RT_NULL);
    return RV_OK;
}

retval_t UPAY_storage_delete(uint32_t items)
{
    RT_ASSERT(RT_NULL != RT_NULL);
    return RV_OK;
}

retval_t UPAY_storage_exists(uint32_t item)
{
    RT_ASSERT(RT_NULL != RT_NULL);
    return RV_OK;
}


uint32_t UPAY_get_timestamp(void)
{
    time_t now;

    now = time(RT_NULL);
    //rt_kprintf("%s\n", ctime(&now));
    return now;
}

void UPAY_ble_write(uint8_t *data, uint16_t len)
{
    RT_ASSERT(RT_NULL != RT_NULL);
    return;
}

retval_t UPAY_get_deviceid(PARAM_OUT uint8_t *buf_did, PARAM_INOUT uint32_t *len_did)
{
    RT_ASSERT(RT_NULL != RT_NULL);
    return RV_OK;
}

retval_t UPAY_get_sn(PARAM_OUT uint8_t *buf_sn, PARAM_INOUT uint32_t *len_sn)
{
    RT_ASSERT(RT_NULL != RT_NULL);
    return RV_OK;
}

retval_t ALIPAY_get_imei(PARAM_OUT uint8_t *buf_imei, PARAM_INOUT uint32_t *len_imei)
{
    RT_ASSERT(RT_NULL != RT_NULL);
    return RV_OK;
}

alipay_product_type UPAY_get_productType(void)
{
    RT_ASSERT(RT_NULL != RT_NULL);
    return ALIPAY_WATCH;
}

uint32_t UPAY_get_protocType(void)
{
    RT_ASSERT(RT_NULL != RT_NULL);
    return 0;
}

void UPAY_get_companyName(PARAM_OUT uint8_t *buffer, PARAM_INOUT uint32_t *len)
{
    RT_ASSERT(RT_NULL != RT_NULL);
    return;
}

void UPAY_get_productModel(PARAM_OUT uint8_t *buffer, PARAM_INOUT uint32_t *len)
{
    RT_ASSERT(RT_NULL != RT_NULL);
    return;
}


extern struct rt_memheap psram_heap;
void *ALIPAY_malloc(uint32_t size)
{
    return rt_memheap_alloc(&psram_heap, size);
}

void *ALIPAY_calloc(uint32_t nblock, uint32_t size)
{
    void *ptr;
    ptr = rt_memheap_alloc(&psram_heap, nblock * size);
    if (ptr != RT_NULL)
    {
        /* clean memory */
        rt_memset(ptr, 0, nblock * size);
    }
    return ptr;
}

void ALIPAY_free(void *pt)
{
    rt_memheap_free(pt);
    return;
}

void *ALIPAY_realloc(void *pt, uint32_t size)
{
    rt_memheap_realloc(&psram_heap, pt, size);
    return NULL;
}


#define DBG_TAG     "ALIVENDOR"
#define DBG_LVL     DBG_LOG
#include <rtdbg.h>

void UPAY_log(UPAY_log_level level, const char *format, int32_t value)
{
    rt_kprintf(format, value);
    if (level == ALIPAY_LEVEL_FATAL)
    {
        LOG_E(format, value);
    }
    else if (level == ALIPAY_LEVEL_ERRO)
    {
        LOG_E(format, value);
    }
    else if (level == ALIPAY_LEVEL_WARN)
    {
        LOG_W(format, value);
    }
    else if (level == ALIPAY_LEVEL_INFO)
    {
        LOG_I(format, value);
    }
    else if (level == ALIPAY_LEVEL_DBG)
    {
        LOG_D(format, value);
    }
}
void UPAY_log_ext(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    rt_kprintf(format, args);
    va_end(args);
}
