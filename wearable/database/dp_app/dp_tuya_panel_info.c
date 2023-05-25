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
#include "dp_tuya_panel_info.h"
#include <flashdb.h>
#include "rtk_kvdb.h"
#include "tuya_ble_log.h"


#define FDB_LOG_TAG ""


static uint8_t first_kv = 0;

static struct fdb_default_kv_node panelinfo_setting_kv_table[] =
{
    {"test_kv", &first_kv, sizeof(first_kv)}, /* struct type KV */
};

static struct fdb_kvdb kvdb_panelinfo_setting_data = { 0 };
static T_RTK_KVDB rtk_kvdb;


int dp_tuya_panelinfo_flashDB_init(void)
{
    FDB_INFO("dp_panelinfo_flashDB_init\n");

    fdb_err_t result;

#ifdef FDB_USING_KVDB
    {
        /* KVDB Sample */
        struct fdb_default_kv default_kv;

        default_kv.kvs = panelinfo_setting_kv_table;
        default_kv.num = sizeof(panelinfo_setting_kv_table) / sizeof(panelinfo_setting_kv_table[0]);

        /* Key-Value database initialization
         *
         *       &kvdb: database object
         *       "env": database name
         * "fdb_kvdb1": The flash partition name base on FAL. Please make sure it's in FAL partition table.
         *              Please change to YOUR partition name.
         * &default_kv: The default KV nodes. It will auto add to KVDB when first initialize successfully.
         *        NULL: The user data if you need, now is empty.
         */
        result = fdb_kvdb_init(&kvdb_panelinfo_setting_data, KVDB_TUYA_PANEL_INFO_NAME,
                               KVDB_TUYA_PART_NAME, &default_kv, NULL);

        if (result != FDB_NO_ERR)
        {
            FDB_INFO("dp_panelinfo_flashDB_init fail %d\n", result);
            return -1;
        }
    }
#endif /* FDB_USING_KVDB */
    if (sizeof(KVDB_TUYA_PANEL_INFO_NAME) > KVDB_NAME_MAX_LEN)
    {
        FDB_INFO("dp_mcu_cfg_const_flashDB_init kvdb name too long\n");
        return -2;
    }

    strcpy(rtk_kvdb.kvdb_name, KVDB_TUYA_PANEL_INFO_NAME);
    rtk_kvdb.kvdb = &kvdb_panelinfo_setting_data;

    rtk_kvdb_add(&rtk_kvdb);
    return 0;
}

