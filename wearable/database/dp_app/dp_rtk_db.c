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
#include "dp_rtk_db.h"


void test_temp_code(void)
{
    T_DP_TARGET_SET target_set = {100, 200, 300};

    rtk_kvdb_get(KVDB_SPORT_REFS_NAME, KEY_SPORTS_REFS_TARGET_SET, &target_set, sizeof(target_set));
}

void rtk_db_init(void)
{
    dp_mcu_cfg_const_flashDB_init();

    dp_mcu_cfg_rw_flashDB_init();

    test_temp_code();
}







