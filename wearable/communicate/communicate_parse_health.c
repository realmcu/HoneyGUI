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
#include "communicate_parse_health.h"
#include "string.h"
#include "ftl.h"
#include "trace.h"




/**
* @brief   resolve health data command received from remote APP
* @param   key: L2 key
* @param   pValue: received value pointer
* @param   length: value length
* @retval  error code
*/
void resolve_HealthData_command(uint8_t key, const uint8_t *pValue, uint16_t length)
{
    switch (key)
    {
    case KEY_REQUEST_DATA:
        {

        }
        break;
    case KEY_SET_STEPS_NOTIFY:
        {

        }
        break;

    case KEY_DAILY_DATA_SYNC:
        {

        }
        break;

    case KEY_LATEST_DATA_SYNC:
        {

        }
        break;
#if 0
    case KEY_REQUEST_HEART_DATA:
        {
            //start a single sample
            if (length == 1)
            {
                if (pValue[0] == 0x01)
                {
                    RtkWristbandSys.hrs_start_up_mode = HRS_MOBILE_START;
                    RtkWristbandSys.heart_sample_by_phone = true;
                    hal_hrs_timer_start();
                }
                else if (pValue[0] == 0x00)
                {
                    hal_hrs_timer_stop();
                }
            }
        }
        break;
    case KEY_HEART_DATA_SAMPLE_SETTING:
        {
            if (length == 2)
            {
                if (pValue[0] == 0x01)
                {
                    //to do get sample period
                    if (pValue[1] > 0 && pValue[1] <= 10)
                    {
                        hal_hrs_sample_continuously_start(pValue[1] * 60 * 1000);
                        RtkWristbandSys.hrs_detect_period = pValue[1];
                    }
                    else
                    {
                        hal_hrs_sample_continuously_start(300000);//5min
                        RtkWristbandSys.hrs_detect_period = 5;
                    }
                }
                else if (pValue[0] == 0x00)
                {
                    hal_hrs_sample_continuously_stop();
                    hal_hrs_timer_stop();
                    RtkWristbandSys.hrs_detect_period = 0;
                }
                uint32_t temp = RtkWristbandSys.hrs_detect_period;
                ftl_save(&temp, HRS_PERIOD_OFFSET, HRS_PERIOD_SIZE);
            }
        }
        break;
#endif
    case KEY_REQUEST_HEART_SAMPLE_SETTING:
        {

        }
        break;
    default:
        break;
    }
}





