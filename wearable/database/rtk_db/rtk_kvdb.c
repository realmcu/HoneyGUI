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
#include "rtk_kvdb.h"
#include "os_mem.h"
#include "string.h"


static T_RTK_KVDB_ITEM rtk_kvdb_list = {NULL, NULL};


T_RTK_KVDB *kvdb_seek(const char *kvdb_name)
{
    if (kvdb_name == NULL)
    {
        FDB_INFO("error kvdb_seek kvdb_name is NULL\n");
        return NULL;
    }

    char name[KVDB_NAME_MAX_LEN];
    memcpy(name, kvdb_name, KVDB_NAME_MAX_LEN);

    slist_t *node;
    for (node = slist_first(&(rtk_kvdb_list.slist)); node; node = slist_next(node))
    {
        T_RTK_KVDB_ITEM *p_item = container_of(node, T_RTK_KVDB_ITEM, slist);

        if (strcmp(name, p_item->rtk_kvdb->kvdb_name) == 0)
        {
            return p_item->rtk_kvdb;
        }
    }

    FDB_INFO("error kvdb_seek kvdb not found kvdb %s \n", name);
    return NULL;
}

bool rtk_kvdb_get(const char *kvdb_name, const char *kvdb_key, void *value, size_t len)
{
    T_RTK_KVDB *rtk_kvdb = kvdb_seek(kvdb_name);

    if (kvdb_key == NULL)
    {
        FDB_INFO("error rtk_kvdb_get kvdb_key is NULL\n");
        return false;
    }

    if (value == NULL || len == 0)
    {
        FDB_INFO("error rtk_kvdb_get value is NULL or len is 0\n");
        return false;
    }

    if (rtk_kvdb != NULL)
    {
        struct fdb_blob blob;
        char key[KEY_MAX_LEN];
        memcpy(key, kvdb_key, KEY_MAX_LEN);
        for (uint8_t i = 0; i < KVDB_RETRY_TIMES; i++)
        {
            fdb_kv_get_blob(rtk_kvdb->kvdb, key, fdb_blob_make(&blob, value, len));
            if (blob.saved.len > 0)
            {
                return true;
            }
        }

        FDB_INFO("error fdb_kv_get_blob not found key %s\n", key);
        return false;
    }

    FDB_INFO("error rtk_kvdb_get kvdb not found kvdb\n");
    return false;
}

bool rtk_kvdb_set(const char *kvdb_name, const char *kvdb_key, void *value, size_t len)
{
    T_RTK_KVDB *rtk_kvdb = kvdb_seek(kvdb_name);
    if (kvdb_key == NULL)
    {
        FDB_INFO("error rtk_kvdb_set kvdb_key is NULL\n");
        return false;
    }

    if (value == NULL || len == 0)
    {
        FDB_INFO("error rtk_kvdb_set value is NULL or len is 0\n");
        return false;
    }

    if (rtk_kvdb != NULL)
    {
        struct fdb_blob blob;
        char key[KEY_MAX_LEN];
        memcpy(key, kvdb_key, KEY_MAX_LEN);

        for (uint8_t i = 0; i < KVDB_RETRY_TIMES; i++)
        {
            if (fdb_kv_set_blob(rtk_kvdb->kvdb, key, fdb_blob_make(&blob, value, len)) == FDB_NO_ERR)
            {
                return true;
            }
        }

        FDB_INFO("error rtk_kvdb_set %s set %s fail\n", rtk_kvdb->kvdb_name, key);
        return false;
    }

    FDB_INFO("error rtk_kvdb_set kvdb not found kvdb\n");
    return false;
}

bool rtk_kvdb_add(T_RTK_KVDB *kvdb)
{
    T_RTK_KVDB_ITEM *p_item = malloc(sizeof(T_RTK_KVDB_ITEM));

    if (p_item == NULL)
    {
        FDB_INFO("error rtk_kvdb_add malloc fail\n");
        return false;
    }

    p_item->rtk_kvdb = kvdb;

    slist_append(&(rtk_kvdb_list.slist), &(p_item->slist));

    return true;
}

bool rtk_kvdb_delete(const char *kvdb_name)
{
    if (kvdb_name == NULL)
    {
        FDB_INFO("error rtk_kvdb_get kvdb_name is NULL\n");
        return false;
    }

    char name[KVDB_NAME_MAX_LEN];
    memcpy(name, kvdb_name, KVDB_NAME_MAX_LEN);

    slist_t *node;
    for (node = slist_first(&(rtk_kvdb_list.slist)); node; node = slist_next(node))
    {
        T_RTK_KVDB_ITEM *p_item = container_of(node, T_RTK_KVDB_ITEM, slist);

        if (strcmp(name, p_item->rtk_kvdb->kvdb_name) == 0)
        {
            slist_remove(&(rtk_kvdb_list.slist), &(p_item->slist));
            free(p_item);
            return true;
        }
    }

    FDB_INFO("error rtk_kvdb_get kvdb not found kvdb %s \n", name);
    return false;
}

bool rtk_kvdb_reset_default(const char *kvdb_name)
{
    T_RTK_KVDB *rtk_kvdb = kvdb_seek(kvdb_name);

    if (rtk_kvdb != NULL)
    {
        for (uint8_t i = 0; i < KVDB_RETRY_TIMES; i++)
        {
            if (fdb_kv_set_default(rtk_kvdb->kvdb) == FDB_NO_ERR)
            {
                return true;
            }
        }

        FDB_INFO("fdb_kv_set_default %s fail\n", rtk_kvdb->kvdb_name);
        return false;
    }

    FDB_INFO("rtk_kvdb_reset_default kvdb not found\n");
    return false;
}

bool rtk_kvdb_deinit(const char *kvdb_name)
{
    T_RTK_KVDB *rtk_kvdb = kvdb_seek(kvdb_name);

    if (rtk_kvdb != NULL)
    {
        for (uint8_t i = 0; i < KVDB_RETRY_TIMES; i++)
        {
            if (fdb_kvdb_deinit(rtk_kvdb->kvdb) == FDB_NO_ERR)
            {
                return true;
            }
        }

        FDB_INFO("rtk_kvdb_deinit %s fail\n", rtk_kvdb->kvdb_name);
        return false;
    }

    FDB_INFO("rtk_kvdb_deinit kvdb not found\n");
    return false;
}



