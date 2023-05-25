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
#include "dp_notification.h"
#include "dp_dataqueue.h"
#include "trace.h"

static uint8_t nc_node_buf[NC_DATA_NODE_BUF_NUM_MAX][NC_DATA_NODE_BUF_LEN_MAX];
static T_DATAQUEUE_NODE nc_data_node_array[NC_DATA_NODE_BUF_NUM_MAX];

static T_DATAQUEUE_NODE nc_empty_hdr_node;
static T_DATAQUEUE_NODE *nc_empty_queue_hdr = &nc_empty_hdr_node;
static T_DATAQUEUE_NODE nc_valid_hdr_node;
static T_DATAQUEUE_NODE *nc_valid_queue_hdr = &nc_valid_hdr_node;


void notification_data_set_test(uint8_t argc, char **argv)
{
    if (nc_valid_queue_hdr->valid_cur_cnt >= nc_valid_queue_hdr->buf_num_max)
    {
        delete_valid_node(nc_empty_queue_hdr, nc_valid_queue_hdr);
    }

    T_DATAQUEUE_NODE *nc_data_node = get_empty_node(nc_empty_queue_hdr);
    if (nc_data_node == NULL)
    {
#ifdef __RTTHREAD__
        rt_kprintf("No empty node\n");
#endif
        return;
    }
    nc_data_node->data.type = (T_NC_TYPE)atoi(argv[1]);
    nc_data_node->data.len = atoi(argv[2]);
    memcpy(nc_data_node->data.buf, argv[3], nc_data_node->data.len);
#ifdef __RTTHREAD__
    rt_kprintf("notification_data_add_test notification_type %d notification_len %d notification_buf %s\n",
               nc_data_node->data.type, nc_data_node->data.len, nc_data_node->data.buf);
#endif
    add_valid_node(nc_valid_queue_hdr, nc_data_node);
}

void set_notification(uint8_t len, void *p_value)
{
    if (nc_valid_queue_hdr->valid_cur_cnt >= NC_DATA_NODE_BUF_NUM_MAX)
    {
        delete_valid_node(nc_empty_queue_hdr, nc_valid_queue_hdr);
    }

    T_DATAQUEUE_NODE *nc_data_node = get_empty_node(nc_empty_queue_hdr);
    if (nc_data_node == NULL)
    {
#ifdef __RTTHREAD__
        rt_kprintf("No empty node\n");
#endif
        return;
    }
    nc_data_node->data.type = ((T_NC_TYPE *)p_value)[0];
    nc_data_node->data.len = len - 1;
    if (nc_data_node->data.len > NC_DATA_NODE_BUF_LEN_MAX)
    {
        nc_data_node->data.len = NC_DATA_NODE_BUF_LEN_MAX;
    }
#ifdef __RTTHREAD__
    rt_kprintf("nc_data_node->data.len is %d\n", nc_data_node->data.len);
#endif
    memcpy(nc_data_node->data.buf, (uint8_t *)p_value + 1, nc_data_node->data.len);

    add_valid_node(nc_valid_queue_hdr, nc_data_node);
    APP_PRINT_INFO2("set_notification len is %d,type is %d", nc_data_node->data.len,
                    nc_data_node->data.type);
    APP_PRINT_INFO1("SET NOTIFY p_value IS %b", TRACE_BINARY(nc_data_node->data.len,
                                                             nc_data_node->data.buf));
}

void notification_data_get_test(uint8_t argc, char **argv)
{
    T_DATAQUEUE_NODE *nc_data_node = get_valid_node(nc_valid_queue_hdr, atoi(argv[1]));
    if (nc_data_node == NULL)
    {
#ifdef __RTTHREAD__
        rt_kprintf("No valid node\n");
#endif
    }
    else
    {
#ifdef __RTTHREAD__
        rt_kprintf("notification_data_get_test notification_type %d notification_len %d notification_buf %s\n",
                   nc_data_node->data.type, nc_data_node->data.len, nc_data_node->data.buf);
#endif
    }
}

void get_notification(uint8_t len, void *p_value)
{
    T_DATAQUEUE_NODE *nc_data_node = get_valid_node(nc_valid_queue_hdr, ((uint8_t *)p_value)[0]);
    if (nc_data_node == NULL)
    {
#ifdef __RTTHREAD__
        rt_kprintf("No valid node\n");
#endif
    }
    else
    {
        ((uint8_t *)p_value)[1] = nc_data_node->data.type;
        if (nc_data_node->data.len > len)
        {
            memcpy((uint8_t *)p_value + 2, nc_data_node->data.buf, len);
            *(char *)p_value = 1;
        }
        else
        {
            memcpy((uint8_t *)p_value + 2, nc_data_node->data.buf, nc_data_node->data.len);
            *(char *)p_value = 0;
        }

    }


}

uint8_t notification_current_num_get(void)
{
    return data_queue_depth_get(nc_valid_queue_hdr);
}

void empty_queue_init(T_DATAQUEUE_NODE *empty_queue_hdr, T_DATAQUEUE_NODE *data_node_array,
                      uint8_t data_node_buf[][NC_DATA_NODE_BUF_LEN_MAX], uint8_t buf_num_max)
{
    T_DATAQUEUE_NODE *tmp_node;

    empty_queue_hdr->buf_num_max = buf_num_max;
    empty_queue_hdr->data.buf = NULL;
    tmp_node = empty_queue_hdr;

    for (uint8_t i = 0; i < empty_queue_hdr->buf_num_max; i++)
    {
        tmp_node->next = &data_node_array[i];
        tmp_node = tmp_node->next;
        tmp_node->data.buf = data_node_buf[i];
    }

    tmp_node->next = NULL;
}

void notification_data_manage_init(void)
{
    empty_queue_init(nc_empty_queue_hdr, nc_data_node_array, nc_node_buf, NC_DATA_NODE_BUF_NUM_MAX);
    valid_queue_init(nc_valid_queue_hdr, NC_DATA_NODE_BUF_NUM_MAX, NC_DATA_NODE_BUF_LEN_MAX);
}


