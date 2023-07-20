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
#include "communicate_protocol.h"
#include "communicate_task.h"
#include "communicate_parse_bond.h"
#include "string.h"
#include "os_timer.h"
#include "trace.h"


/* disconnect timer */
static void *unbond_disc_timer;

void unbond_timerout_callback(void *pxTimer)
{
    APP_PRINT_INFO0("unbond timeout disconnect!");
    //le_disconnect(RtkWristbandSys.wristband_conn_id);
}
void communicate_parse_init(void)
{
    os_timer_create(&unbond_disc_timer, "unbond disc_timer", 1, 2000, false, unbond_timerout_callback);
}

static void bond_success(void)
{
    //package_prepare_send(package);
}

static void login_request(void)
{
    //rt_kprintf("current thread name = %s \n",rt_thread_self()->name);
    struct protocol_pack p = {0};
    p.l2_cmd_id = BOND_COMMAND_ID;
    p.l2_key = 0x04;
    p.l2_lenght = 1;
    p.l2_payload[0] = 1;
    package_prepare_send(&p);

}
void resolve_private_bond_command(uint8_t key, const uint8_t *pValue, uint16_t length)
{
    switch (key)
    {
    case KEY_BOND_REQUEST:
        {
            cb_send_to_queue(bond_success);
        }
        break;
    case KEY_LOGIN_REQUEST:
        {
            cb_send_to_queue(login_request);
        }
        break;
    case KEY_UNBOND:
        {
            APP_PRINT_INFO0("Unbond !!! ");
            if (unbond_disc_timer != NULL)
            {
                os_timer_start(&unbond_disc_timer);
            }
        }
        break;
    default:
        break;
    }
}