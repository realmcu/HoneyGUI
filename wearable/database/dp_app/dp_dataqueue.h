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
#ifndef DP_DATAQUEUE_H
#define DP_DATAQUEUE_H

#ifdef __cplusplus
extern "C"
{
#endif

/*============================================================================*
 *                        Header Files
*============================================================================*/
#include "stdbool.h"
#include "stdint.h"
#include "dp_wearable.h"


typedef struct
{
    uint16_t len;
    uint8_t *buf;
} T_DATAQUEUE_DATA;

typedef struct data_node
{
    struct data_node *next;
    T_DATAQUEUE_DATA data;
    uint8_t valid_cur_cnt;
    uint8_t buf_num_max;
    uint16_t buf_len_max;
} T_DATAQUEUE_NODE;


void add_empty_node(T_DATAQUEUE_NODE *empty_queue_hdr, T_DATAQUEUE_NODE *node);
T_DATAQUEUE_NODE *get_empty_node(T_DATAQUEUE_NODE *empty_queue_hdr);
void valid_queue_init(T_DATAQUEUE_NODE *valid_queue_hdr, uint8_t buf_num_max, uint16_t buf_len_max);
void add_valid_node(T_DATAQUEUE_NODE *valid_queue_hdr, T_DATAQUEUE_NODE *node);
void delete_valid_node(T_DATAQUEUE_NODE *empty_queue_hdr, T_DATAQUEUE_NODE *valid_queue_hdr);
T_DATAQUEUE_NODE *get_valid_node(T_DATAQUEUE_NODE *valid_queue_hdr, uint8_t node_num);
uint8_t data_queue_depth_get(T_DATAQUEUE_NODE *valid_queue_hdr);
void empty_queue_init(T_DATAQUEUE_NODE *empty_queue_hdr, T_DATAQUEUE_NODE *data_node_array,
                      uint8_t *data_node_buf, uint8_t buf_num_max, uint16_t buf_len_max);

#ifdef __cplusplus
}
#endif

#endif /* DP_DATAQUEUE_H */
