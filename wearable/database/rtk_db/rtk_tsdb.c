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


/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <stdlib.h>
#include "rtk_tsdb.h"
#include "os_mem.h"
#include "time.h"

typedef struct
{
    uint16_t walk_min;
    uint16_t calorie_min;
    uint16_t mileage_min;
} T_SPORT_MIN_DATA;

static T_RTK_TSDB_ITEM rtk_tsdb_list = {NULL, NULL};


T_RTK_TSDB *tsdb_seek(const char *tsdb_name)
{
    if (tsdb_name == NULL)
    {
        FDB_INFO("error tsdb_seek tsdb_name is NULL\n");
        return NULL;
    }

    char name[TSDB_NAME_MAX_LEN];
    memcpy(name, tsdb_name, TSDB_NAME_MAX_LEN);

    slist_t *node;
    for (node = slist_first(&(rtk_tsdb_list.slist)); node; node = slist_next(node))
    {
        T_RTK_TSDB_ITEM *p_item = container_of(node, T_RTK_TSDB_ITEM, slist);

        if (strcmp(name, p_item->rtk_tsdb->tsdb_name) == 0)
        {
            return p_item->rtk_tsdb;
        }
    }

    FDB_INFO("error tsdb_seek tsdb not found tsdb %s \n", name);
    return NULL;
}

bool rtk_tsdb_iter(const char *tsdb_name)
{
    T_RTK_TSDB *rtk_tsdb = tsdb_seek(tsdb_name);

    if (rtk_tsdb != NULL)
    {
        fdb_tsl_iter(rtk_tsdb->tsdb, rtk_tsdb->query_cb, rtk_tsdb->tsdb);
        return true;
    }

    FDB_INFO("error rtk_tsdb_iter fail\n");
    return false;
}

bool rtk_tsdb_iter_one_by_time(const char *tsdb_name, struct tm tm, void *p_value)
{
    T_RTK_TSDB *rtk_tsdb = tsdb_seek(tsdb_name);

    if (rtk_tsdb != NULL)
    {
        time_t tm_time = mktime(&tm);
        FDB_INFO("rtk_tsdb_iter_one_by_time tm_time %d\n", tm_time);
        fdb_tsl_iter_by_time(rtk_tsdb->tsdb, tm_time, tm_time, rtk_tsdb->query_by_time_cb, rtk_tsdb->tsdb);
        memcpy(p_value, rtk_tsdb->p_tsl_data, rtk_tsdb->tsl_data_len);
        memset(rtk_tsdb->p_tsl_data, 0, rtk_tsdb->tsl_data_len);
        return true;
    }

    FDB_INFO("error rtk_tsdb_iter_by_time fail\n");
    return false;
}

bool rtk_tsdb_tsl_append(const char *tsdb_name, void *value, size_t len)
{
    if (value == NULL || len == 0)
    {
        FDB_INFO("error rtk_tsdb_append value is NULL or len is 0\n");
        return false;
    }

    T_RTK_TSDB *rtk_tsdb = tsdb_seek(tsdb_name);

    if (rtk_tsdb != NULL)
    {
        for (uint8_t i = 0; i < TSDB_RETRY_TIMES; i++)
        {
            struct fdb_blob blob;
            if (fdb_tsl_append(rtk_tsdb->tsdb, fdb_blob_make(&blob, value, len)) == FDB_NO_ERR)
            {
                return true;
            }
        }

        FDB_INFO("error fdb_tsl_append %s append fail\n", rtk_tsdb->tsdb_name);
        return false;
    }

    FDB_INFO("error rtk_tsdb_append tsdb not found tsdb\n");
    return false;
}

bool rtk_tsdb_add(T_RTK_TSDB *tsdb)
{
    T_RTK_TSDB_ITEM *p_item = malloc(sizeof(T_RTK_TSDB_ITEM));
    if (p_item == NULL)
    {
        FDB_INFO("error rtk_tsdb_add malloc fail\n");
        return false;
    }

    p_item->rtk_tsdb = tsdb;

    slist_append(&(rtk_tsdb_list.slist), &(p_item->slist));

    return true;
}

bool rtk_tsdb_delete(const char *tsdb_name)
{
    T_RTK_TSDB *rtk_tsdb = tsdb_seek(tsdb_name);

    if (rtk_tsdb != NULL)
    {
        T_RTK_TSDB_ITEM *p_item = container_of(rtk_tsdb, T_RTK_TSDB_ITEM, rtk_tsdb);
        slist_remove(&(rtk_tsdb_list.slist), &(p_item->slist));
        free(p_item);
        return true;
    }

    FDB_INFO("rtk_tsdb_delete tsdb not found\n");
    return false;
}

bool rtk_tsdb_clean(const char *tsdb_name)
{
    T_RTK_TSDB *rtk_tsdb = tsdb_seek(tsdb_name);

    if (rtk_tsdb != NULL)
    {
        fdb_tsl_clean(rtk_tsdb->tsdb);
        return true;
    }

    FDB_INFO("rtk_tsdb_clean tsdb not found\n");
    return false;
}

bool rtk_tsdb_deinit(const char *tsdb_name)
{
    T_RTK_TSDB *rtk_tsdb = tsdb_seek(tsdb_name);

    if (rtk_tsdb != NULL)
    {
        if (fdb_tsdb_deinit(rtk_tsdb->tsdb) == FDB_NO_ERR)
        {
            return true;
        }
        else
        {
            FDB_INFO("rtk_tsdb_deinit fail\n");
            return false;
        }
    }

    FDB_INFO("rtk_tsdb_deinit tsdb not found\n");
    return false;
}

size_t rtk_tsdb_tsl_query_count(const char *tsdb_name, struct tm tm_from, struct tm tm_to)
{
    T_RTK_TSDB *rtk_tsdb = tsdb_seek(tsdb_name);

    if (rtk_tsdb != NULL)
    {
        time_t from_time = mktime(&tm_from), to_time = mktime(&tm_to);
        return fdb_tsl_query_count(rtk_tsdb->tsdb, from_time, to_time, FDB_TSL_WRITE);
    }

    return 0;
}



