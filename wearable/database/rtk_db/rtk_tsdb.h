/**
*********************************************************************************************************
*               Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      datebse.h
* @brief
* @details
* @author
* @date
* @version
* *********************************************************************************************************
*/

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef _RTK_TSDB_H_
#define _RTK_TSDB_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*============================================================================*
 *                        Header Files
*============================================================================*/
#include "stdbool.h"
#include "stdint.h"
#include "string.h"
#include <flashdb.h>
#include "structure_list.h"


#define FDB_LOG_TAG ""

#define TSDB_RETRY_TIMES 3

#define TSDB_NAME_MAX_LEN     16


typedef struct
{
    fdb_tsdb_t tsdb;
    char tsdb_name[TSDB_NAME_MAX_LEN];
    fdb_tsl_cb query_cb;
    fdb_tsl_cb query_by_time_cb;
    void *p_tsl_data;
    uint16_t tsl_data_len;
} T_RTK_TSDB;

typedef struct
{
    slist_t slist;
    T_RTK_TSDB *rtk_tsdb;
} T_RTK_TSDB_ITEM;


T_RTK_TSDB *tsdb_seek(const char *tsdb_name);
bool rtk_tsdb_iter(const char *tsdb_name);
bool rtk_tsdb_iter_one_by_time(const char *tsdb_name, struct tm tm, void *value);
bool rtk_tsdb_tsl_append(const char *tsdb_name, void *value, size_t len);
bool rtk_tsdb_add(T_RTK_TSDB *tsdb);
bool rtk_tsdb_delete(const char *tsdb_name);
bool rtk_tsdb_clean(const char *tsdb_name);
bool rtk_tsdb_deinit(const char *tsdb_name);
size_t rtk_tsdb_tsl_query_count(const char *tsdb_name, struct tm tm_from, struct tm tm_to);


#ifdef __cplusplus
}
#endif

#endif /* _DP_RTK_TSDB_H_ */
