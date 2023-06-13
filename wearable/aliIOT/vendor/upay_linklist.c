/**
*********************************************************************************************************
*               Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     upay_linklist.c
* @brief
* @details
* @author
* @date     2022-01-19
* @version  v1.0
*********************************************************************************************************
*/


/* Includes ------------------------------------------------------------------*/

#include "upay_linklist.h"
#include "os_sync.h"


unsigned char UART_BUF[BUF_NUM_MAX][BUF_LEN_MAX];
DATA_NODE data_node_array[BUF_NUM_MAX];
DATA_NODE *empty_queue_hdr = NULL;
DATA_NODE empty_hdr_node;
DATA_NODE *valid_queue_hdr = NULL;
DATA_NODE valid_hdr_node;


void empty_queue_init()
{
    int i;
    DATA_NODE *tmp_node;

    empty_queue_hdr = &empty_hdr_node;
    empty_queue_hdr->bempty = true;
    empty_queue_hdr->len = 0;
    empty_queue_hdr->buf = NULL;
    tmp_node = empty_queue_hdr;

    for (i = 0; i < BUF_NUM_MAX; i++)
    {
        tmp_node->next = &data_node_array[i];
        tmp_node = tmp_node->next;
        tmp_node->bempty = true;
        tmp_node->len = 0;
        tmp_node->buf = UART_BUF[i];
    }

    tmp_node->next = NULL;
}

void add_empty_node(DATA_NODE *node)
{
    uint32_t s = os_lock();
    DATA_NODE *tmp_node = empty_queue_hdr;

    while (tmp_node->next != NULL)
    {
        tmp_node = tmp_node->next;
    }

    tmp_node->next = node;
    node->next = NULL;

    os_unlock(s);
}

DATA_NODE *get_empty_node()
{
    DATA_NODE *tmp_node = NULL;

    uint32_t s = os_lock();

    if (empty_queue_hdr->next != NULL)
    {
        tmp_node = empty_queue_hdr->next;
        empty_queue_hdr->next = empty_queue_hdr->next->next;
    }
    os_unlock(s);
    return tmp_node;
}

void valid_queue_init()
{
    valid_queue_hdr = &valid_hdr_node;
    valid_queue_hdr->bempty = true;
    valid_queue_hdr->len = 0;
    valid_queue_hdr->next = NULL;
    valid_queue_hdr->buf = NULL;
    return;
}

void add_valid_node(DATA_NODE *node)
{
    uint32_t s = os_lock();

    DATA_NODE *tmp_node = valid_queue_hdr;

    while (tmp_node->next != NULL)
    {
        tmp_node = tmp_node->next;
    }

    tmp_node->next = node;
    node->next = NULL;
    os_unlock(s);
}

DATA_NODE *get_valid_node()
{
    DATA_NODE *tmp_node = NULL;

    uint32_t s = os_lock();

    if (valid_queue_hdr->next != NULL)
    {
        tmp_node = valid_queue_hdr->next;
        valid_queue_hdr->next = valid_queue_hdr->next->next;
    }

    os_unlock(s);
    return tmp_node;
}
