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
#include "dp_wearable.h"
#include "dp_sports_refs.h"
#include <flashdb.h>
#include "rtk_kvdb.h"
#include "rtk_tsdb.h"
#include "watch_clock.h"


#define FDB_LOG_TAG ""


static uint8_t first_kv = 0;
static struct fdb_default_kv_node sports_refs_kv_table[] =
{
    {"first_kv", &first_kv, sizeof(first_kv)}, /* struct type KV */
};

static struct fdb_kvdb kvdb_sports_refs_data = { 0 };
static T_RTK_KVDB rtk_kvdb;

/* TSDB object */
struct fdb_tsdb tsdb_sport_min_data = { 0 };
static T_RTK_TSDB rtk_tsdb;
/* counts for simulated timestamp */
static bool query_by_time_cb(fdb_tsl_t tsl, void *arg);

static T_SPORT_MIN_DATA sport_min_data;


static bool query_by_time_cb(fdb_tsl_t tsl, void *arg)
{
    struct fdb_blob blob;

    fdb_tsdb_t db = arg;

    fdb_blob_read((fdb_db_t) db, fdb_tsl_to_blob(tsl, fdb_blob_make(&blob, &sport_min_data,
                                                                    sizeof(sport_min_data))));
    FDB_INFO("[query_by_time_cb] queried a TSL: walk_min %d calorie_min %d mileage_min %d\n",
             sport_min_data.walk_min, sport_min_data.calorie_min, sport_min_data.mileage_min);

    return false;
}

static fdb_time_t get_time(void)
{
    /* Using the counts instead of timestamp.
     * Please change this function to return RTC time.
     */
    struct tm tm_time = { .tm_year = 2022 - 1900, .tm_mon = 4, .tm_mday = 5, .tm_hour = 0, .tm_min = 0, .tm_sec = 0 };
#if 0
    struct tm sport_data_time = watch_clock_get();
#else
    struct tm sport_data_time = {0};
#endif
    sport_data_time.tm_sec = 0;
    FDB_INFO("mean : query from %d %d %d %d %d\n", sport_data_time.tm_year,
             sport_data_time.tm_mon,
             sport_data_time.tm_mday, sport_data_time.tm_hour, sport_data_time.tm_min);

    time_t time = mktime(&sport_data_time);

    FDB_INFO("get_time time %d", time);

    return time;
}

int sport_min_data_flashDB_init(void)
{
    fdb_err_t result;

#ifdef FDB_USING_TSDB
    {
        /* Time series database initialization
         *
         *       &tsdb: database object
         *       "log": database name
         * "fdb_tsdb1": The flash partition name base on FAL. Please make sure it's in FAL partition table.
         *              Please change to YOUR partition name.
         *    get_time: The get current timestamp function.
         *         128: maximum length of each log
         *        NULL: The user data if you need, now is empty.
         */
        result = fdb_tsdb_init(&tsdb_sport_min_data, TSDB_SPORT_MIN_DATA_NAME, TSDB_SPORT_MIN_PART_NAME,
                               get_time, TSDB_SPORT_MIN_DATA_TSL_MAX_LEN, NULL);
        /* read last saved time for simulated timestamp */
        //fdb_tsdb_control(&tsdb_sport_min_data, FDB_TSDB_CTRL_GET_LAST_TIME, &counts);

        if (result != FDB_NO_ERR)
        {
            FDB_INFO("sport_min_data_flashDB_init fail %d\n", result);
            return -1;
        }
    }
#endif /* FDB_USING_TSDB */

    if (sizeof(TSDB_SPORT_MIN_DATA_NAME) > TSDB_NAME_MAX_LEN)
    {
        FDB_INFO("sport_min_data_flashDB_init tsdb name too long\n");
        return -2;
    }
    strcpy(rtk_tsdb.tsdb_name, TSDB_SPORT_MIN_DATA_NAME);
    rtk_tsdb.tsdb = &tsdb_sport_min_data;
    rtk_tsdb.query_cb = NULL;
    rtk_tsdb.query_by_time_cb = query_by_time_cb;
    rtk_tsdb.p_tsl_data = &sport_min_data;
    rtk_tsdb.tsl_data_len = sizeof(sport_min_data);
    rtk_tsdb_add(&rtk_tsdb);

    return 0;
}

//kvdb config
int dp_sports_refs_flashDB_init(void)
{
    fdb_err_t result;

#ifdef FDB_USING_KVDB
    {
        /* KVDB Sample */
        struct fdb_default_kv default_kv;

        default_kv.kvs = sports_refs_kv_table;
        default_kv.num = sizeof(sports_refs_kv_table) / sizeof(sports_refs_kv_table[0]);

        /* Key-Value database initialization
         *
         *       &kvdb: database object
         *       "env": database name
         * "fdb_kvdb1": The flash partition name base on FAL. Please make sure it's in FAL partition table.
         *              Please change to YOUR partition name.
         * &default_kv: The default KV nodes. It will auto add to KVDB when first initialize successfully.
         *        NULL: The user data if you need, now is empty.
         */
        result = fdb_kvdb_init(&kvdb_sports_refs_data, KVDB_SPORT_REFS_NAME,
                               KVDB_TUYA_PART_NAME, &default_kv, NULL);

        if (result != FDB_NO_ERR)
        {
            FDB_INFO("dp_sports_refs_flashDB_init fail %d", result);
            return -1;
        }
    }
#endif /* FDB_USING_KVDB */

    if (sizeof(KVDB_SPORT_REFS_NAME) > KVDB_NAME_MAX_LEN)
    {
        FDB_INFO("dp_sports_refs_flashDB_init kvdb name too long\n");
        return -2;
    }
    strcpy(rtk_kvdb.kvdb_name, KVDB_SPORT_REFS_NAME);
    rtk_kvdb.kvdb = &kvdb_sports_refs_data;

    rtk_kvdb_add(&rtk_kvdb);

    return 0;
}


