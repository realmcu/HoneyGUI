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


#define NC_DATA_NODE_BUF_NUM_MAX 3
#define NC_DATA_NODE_BUF_LEN_MAX 100


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
        APP_PRINT_INFO0("No empty node\n");
        return;
    }
    nc_data_node->data.len = atoi(argv[1]);
    memcpy(nc_data_node->data.buf, argv[2], nc_data_node->data.len);

    APP_PRINT_INFO3("notification_data_add_test notification_type %d notification_len %d notification_buf %s\n",
                    nc_data_node->data.buf, nc_data_node->data.len, nc_data_node->data.buf + 1);

    add_valid_node(nc_valid_queue_hdr, nc_data_node);
}

void set_notification(uint8_t len, void *p_value)
{
    if (data_queue_depth_get(nc_valid_queue_hdr) >= NC_DATA_NODE_BUF_NUM_MAX)
    {
        delete_valid_node(nc_empty_queue_hdr, nc_valid_queue_hdr);
    }

    T_DATAQUEUE_NODE *nc_data_node = get_empty_node(nc_empty_queue_hdr);
    if (nc_data_node == NULL)
    {
        APP_PRINT_INFO0("No empty node\n");
        return;
    }

    nc_data_node->data.len = len;
    if (nc_data_node->data.len > NC_DATA_NODE_BUF_LEN_MAX)
    {
        nc_data_node->data.len = NC_DATA_NODE_BUF_LEN_MAX;
    }

    APP_PRINT_INFO1("nc_data_node->data.len is %d\n", nc_data_node->data.len);

    memcpy(nc_data_node->data.buf, (uint8_t *)p_value, nc_data_node->data.len);

    add_valid_node(nc_valid_queue_hdr, nc_data_node);
    APP_PRINT_INFO2("set_notification len is %d,type is %d", nc_data_node->data.len,
                    nc_data_node->data.buf);
    APP_PRINT_INFO1("SET NOTIFY p_value IS %s", TRACE_BINARY(nc_data_node->data.len,
                                                             nc_data_node->data.buf + 1));
}

void notification_data_get_test(uint8_t argc, char **argv)
{
    T_DATAQUEUE_NODE *nc_data_node = get_valid_node(nc_valid_queue_hdr, atoi(argv[1]));
    if (nc_data_node == NULL)
    {
        APP_PRINT_INFO0("No valid node\n");
    }
    else
    {
        APP_PRINT_INFO3("notification_data_get_test notification_type %d notification_len %d notification_buf %s\n",
                        nc_data_node->data.buf, nc_data_node->data.len, nc_data_node->data.buf + 1);
    }
}

void get_notification(uint8_t len, void *p_value, uint8_t msg_num)
{
    T_DATAQUEUE_NODE *nc_data_node = get_valid_node(nc_valid_queue_hdr, msg_num);
    if (nc_data_node == NULL)
    {
        APP_PRINT_INFO0("No valid node\n");
    }
    else
    {
        memcpy((uint8_t *)p_value, nc_data_node->data.buf, nc_data_node->data.len);
    }
}

uint8_t notification_current_num_get(void)
{
    return data_queue_depth_get(nc_valid_queue_hdr);
}

void notification_data_manage_init(void)
{
    empty_queue_init(nc_empty_queue_hdr, nc_data_node_array, *nc_node_buf, NC_DATA_NODE_BUF_NUM_MAX,
                     NC_DATA_NODE_BUF_LEN_MAX);
    valid_queue_init(nc_valid_queue_hdr, NC_DATA_NODE_BUF_NUM_MAX, NC_DATA_NODE_BUF_LEN_MAX);
}


