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
#ifndef _RTK_KVDB_H_
#define _RTK_KVDB_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*============================================================================*
 *                        Header Files
*============================================================================*/
#include "stdbool.h"
#include "stdint.h"
#include <flashdb.h>
#include "structure_list.h"


#define FDB_LOG_TAG ""

#define KVDB_RETRY_TIMES 3

#define KVDB_NAME_MAX_LEN     16

#define KEY_MAX_LEN     64


typedef struct
{
    fdb_kvdb_t kvdb;
    char kvdb_name[KVDB_NAME_MAX_LEN];
} T_RTK_KVDB;

typedef struct
{
    slist_t slist;
    T_RTK_KVDB *rtk_kvdb;
} T_RTK_KVDB_ITEM;


T_RTK_KVDB *kvdb_seek(const char *kvdb_name);
bool rtk_kvdb_get(const char *kvdb_name, const char *key, void *value, size_t len);
bool rtk_kvdb_set(const char *kvdb_name, const char *key, void *value, size_t len);
bool rtk_kvdb_add(T_RTK_KVDB *kvdb);
bool rtk_kvdb_delete(const char *kvdb_name);
bool rtk_kvdb_reset_default(const char *kvdb_name);
bool rtk_kvdb_deinit(const char *kvdb_name);


#ifdef __cplusplus
}
#endif

#endif /* _RTK_KVDB_H_ */
