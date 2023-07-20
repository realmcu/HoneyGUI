/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file      communicate_task.c
   * @brief     Routines to create App task and handle events & messages
   * @author    howie
   * @date      2019-12-26
   * @version   v1.0
*****************************************************************************************
  */
#include <os_msg.h>
#include <os_task.h>
#include <communicate_task.h>
#include <communicate_protocol.h>
#include <communicate_parse.h>
#include <app_msg.h>
#include <os_task.h>
#include "trace.h"
#include "version.h"
#include <stdlib.h>
#include "string.h"

#include <os_msg.h>
#include <os_task.h>
#include <gap.h>
#include <gap_le.h>
#include <app_msg.h>
#include "trace.h"

void *l1send_task_handle;
void *l1reveive_task_handle;

void *l1send_queue_handle;
void *l1recv_queue_handle;
void *raw_data_receive_queue_handle;

void l1send_task(void *p_param);
void l1receive_task(void *p_param);

int communicate_task_init(void)
{
    os_task_create(&l1send_task_handle, "l1send", l1send_task, 0, 256 * 4, 1);
    os_task_create(&l1reveive_task_handle, "l1receive", l1receive_task, 0, 256 * 4, 1);
    return 0;
}

bool cb_send_to_queue(pfunc func)
{
    if (os_msg_send(l1send_queue_handle, &func, 0) == false)
    {
        return false;
    }
    return true;
}


void l1send_task(void *pvParameters)
{
    os_alloc_secure_ctx(1024);
    pfunc p_func = NULL;
    os_msg_queue_create(&l1send_queue_handle, "l1send", 0x10, sizeof(void *));
    os_msg_queue_create(&l1recv_queue_handle, "l1recv", 0x10, sizeof(uint8_t));

    while (true)
    {
        if (os_msg_recv(l1send_queue_handle, &p_func, 0xFFFFFFFF) == true)
        {
            if (p_func != NULL)
            {
                p_func();
            }
        }
    }
}
#if USE_HRS_MASTER
bool cb_l1receive_to_queue(void *buf)
{
    if (os_msg_send(raw_data_receive_queue_handle, &buf, 0) == false)
    {
        return false;
    }
    return true;
}
#include "gcs_client.h"
void l1receive_task(void *pvParameters)
{
    uint8_t buf[256] = {0};
    memset(buf, 0x0, 256);
    os_msg_queue_create(&raw_data_receive_queue_handle, 0x10, sizeof(void *));
    while (true)
    {
        if (os_msg_recv(raw_data_receive_queue_handle, &buf, 0xFFFFFFFF) == true)
        {
            APP_PRINT_INFO0("os_msg_recv raw_data_receive_queue_handle");
            uint16 length = buf[5] * 128 + buf[4];
            APP_PRINT_INFO1("length is %d", length);

            resolve_remote_data(((T_GCS_NOTIF_IND *)buf)->p_value, length);
//            free(buf);
        }
    }
}
#else
bool cb_l1receive_to_queue(pfunc func)
{
    if (os_msg_send(raw_data_receive_queue_handle, &func, 0) == false)
    {
        return false;
    }
    return true;
}

void l1receive_task(void *pvParameters)
{
    uint8_t *buf = NULL;
    os_alloc_secure_ctx(1024);
    os_msg_queue_create(&raw_data_receive_queue_handle, "rec_raw_data", 0x10, sizeof(void *));

    while (true)
    {
        if (os_msg_recv(raw_data_receive_queue_handle, &buf, 0xFFFFFFFF) == true)
        {
            //rt_kprintf("buf2 = 0x%x \n", buf);
            resolve_remote_data(buf + 1, buf[0]);
            free(buf);
        }
    }
}
#endif
//#include "rtthread.h"
//INIT_APP_EXPORT(communicate_task_init);
