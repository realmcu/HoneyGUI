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
#include "dp_dataqueue.h"
#include "trace.h"


void add_empty_node(T_DATAQUEUE_NODE *empty_queue_hdr, T_DATAQUEUE_NODE *node)
{
    T_DATAQUEUE_NODE *tmp_node = empty_queue_hdr;

    while (tmp_node->next != NULL)
    {
        tmp_node = tmp_node->next;
    }

    tmp_node->next = node;
    if (node != NULL)
    {
        node->next = NULL;
    }
    else
    {
        APP_PRINT_INFO0("error node is NULL\n");
    }
}

T_DATAQUEUE_NODE *get_empty_node(T_DATAQUEUE_NODE *empty_queue_hdr)
{
    T_DATAQUEUE_NODE *tmp_node = NULL;

    if (empty_queue_hdr->next != NULL)
    {
        tmp_node = empty_queue_hdr->next;
        empty_queue_hdr->next = empty_queue_hdr->next->next;
    }

    return tmp_node;
}

void empty_queue_init(T_DATAQUEUE_NODE *empty_queue_hdr, T_DATAQUEUE_NODE *data_node_array,
                      uint8_t *data_node_buf, uint8_t buf_num_max, uint16_t buf_len_max)
{
    T_DATAQUEUE_NODE *tmp_node;

    empty_queue_hdr->buf_num_max = buf_num_max;
    empty_queue_hdr->data.buf = NULL;
    tmp_node = empty_queue_hdr;

    for (uint8_t i = 0; i < empty_queue_hdr->buf_num_max; i++)
    {
        tmp_node->next = &data_node_array[i];
        tmp_node = tmp_node->next;
        tmp_node->data.buf = data_node_buf + i * buf_len_max;
    }

    tmp_node->next = NULL;
}

void add_valid_node(T_DATAQUEUE_NODE *valid_queue_hdr, T_DATAQUEUE_NODE *node)
{
    T_DATAQUEUE_NODE *tmp_node = valid_queue_hdr;

    while (tmp_node->next != NULL)
    {
        tmp_node = tmp_node->next;
    }

    valid_queue_hdr->valid_cur_cnt++;
    tmp_node->next = node;
    node->next = NULL;
}

void delete_valid_node(T_DATAQUEUE_NODE *empty_queue_hdr, T_DATAQUEUE_NODE *valid_queue_hdr)
{
    T_DATAQUEUE_NODE *tmp_node = NULL;

    if (valid_queue_hdr->next != NULL)
    {
        tmp_node = valid_queue_hdr->next;
        valid_queue_hdr->next = valid_queue_hdr->next->next;
    }

    add_empty_node(empty_queue_hdr, tmp_node);
    valid_queue_hdr->valid_cur_cnt--;
}

T_DATAQUEUE_NODE *get_valid_node(T_DATAQUEUE_NODE *valid_queue_hdr, uint8_t node_num)
{
    if (node_num > valid_queue_hdr->valid_cur_cnt || node_num > valid_queue_hdr->buf_num_max)
    {
        return NULL;
    }

    T_DATAQUEUE_NODE *tmp_node = valid_queue_hdr;

    while (node_num--)
    {
        tmp_node = tmp_node->next;
    }

    return tmp_node;
}

void valid_queue_init(T_DATAQUEUE_NODE *valid_queue_hdr, uint8_t buf_num_max, uint16_t buf_len_max)
{
    valid_queue_hdr->buf_len_max = buf_len_max;
    valid_queue_hdr->buf_num_max = buf_num_max;
    valid_queue_hdr->valid_cur_cnt = 0;
    valid_queue_hdr->next = NULL;
    valid_queue_hdr->data.buf = NULL;
    return;
}

uint8_t data_queue_depth_get(T_DATAQUEUE_NODE *valid_queue_hdr)
{
    return valid_queue_hdr->valid_cur_cnt;
}


