/*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     communicate_parse.c
* @brief
* @details
* @author
* @date
* @version  v0.1
*********************************************************************************************************
*/

#include "board.h"
#include "communicate_parse.h"
#include "communicate_parse_bond.h"
#include "string.h"
#include "os_timer.h"
#include "trace.h"


/* disconnect timer */
void *unbond_disc_timer;

void unbond_timerout_callback(void *pxTimer)
{

}
void communicate_parse_init(void)
{
    os_timer_create(&unbond_disc_timer, "unbond disc_timer", 1, 2000, false, unbond_timerout_callback);
}

/**
* @brief   resolve private bond command received from remote APP
* @param   key: L2 key
* @param   pValue: received value pointer
* @param   length: value length
* @retval  error code
*/
void resolve_private_bond_command(uint8_t key, const uint8_t *pValue, uint16_t length)
{
    switch (key)
    {
    case KEY_BOND_REQUEST:
        {

        }
        break;
    case KEY_LOGIN_REQUEST:
        {
        }
        break;
    case KEY_UNBOND:
        {

        }
        break;
    default:
        break;
    }
}





