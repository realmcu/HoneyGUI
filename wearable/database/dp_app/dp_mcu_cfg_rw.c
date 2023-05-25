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
#include "dp_mcu_cfg_rw.h"
#include <flashdb.h>
#include "rtk_kvdb.h"


static uint8_t first_kv = 0;

static struct fdb_default_kv_node mcu_cfg_rw_kv_table[] =
{
    {"first_kv", &first_kv, sizeof(first_kv)}, /* struct type KV */
};

static struct fdb_kvdb kvdb_mcu_cfg_rw_data = { 0 };
static T_RTK_KVDB rtk_kvdb;


int dp_mcu_cfg_rw_flashDB_init(void)
{
    FDB_INFO("dp_mcu_cfg_rw_flashDB_init\n");

    fdb_err_t result;

#ifdef FDB_USING_KVDB
    {
        struct fdb_default_kv default_kv;
        default_kv.kvs = mcu_cfg_rw_kv_table;
        default_kv.num = sizeof(mcu_cfg_rw_kv_table) / sizeof(mcu_cfg_rw_kv_table[0]);
#if 0
        /* set the lock and unlock function if you want */
        fdb_kvdb_control(&kvdb_wearable_setting_data, FDB_KVDB_CTRL_SET_LOCK, (void *)lock);
        fdb_kvdb_control(&kvdb_wearable_setting_data, FDB_KVDB_CTRL_SET_UNLOCK, (void *)unlock);
#endif
        /* Key-Value database initialization
         *
         *       &kvdb: database object
         *       "env": database name
         * "fdb_kvdb1": The flash partition name base on FAL. Please make sure it's in FAL partition table.
         *              Please change to YOUR partition name.
         * &default_kv: The default KV nodes. It will auto add to KVDB when first initialize successfully.
         *        NULL: The user data if you need, now is empty.
         */
        result = fdb_kvdb_init(&kvdb_mcu_cfg_rw_data, KVDB_MCU_CFG_RW_NAME,
                               KVDB_MCU_CFG_RW_PART_NAME, &default_kv, NULL);

        if (result != FDB_NO_ERR)
        {
            FDB_INFO("dp_mcu_cfg_rw_flashDB_init fail\n");
            return -1;
        }
    }
#endif /* FDB_USING_KVDB */

    if (sizeof(KVDB_MCU_CFG_RW_NAME) > KVDB_NAME_MAX_LEN)
    {
        FDB_INFO("dp_mcu_cfg_rw_flashDB_init kvdb name too long\n");
        return -2;
    }
    strcpy(rtk_kvdb.kvdb_name, KVDB_MCU_CFG_RW_NAME);
    rtk_kvdb.kvdb = &kvdb_mcu_cfg_rw_data;

    rtk_kvdb_add(&rtk_kvdb);

    return 0;
}

