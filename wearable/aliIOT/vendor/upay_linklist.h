/**
*********************************************************************************************************
*               Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     upay_linklist.h
* @brief
* @details
* @author
* @date     2022-01-19
* @version  v1.0
*********************************************************************************************************
*/

#ifndef _UPAY_LINKLIST_H_
#define _UPAY_LINKLIST_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "stdbool.h"
#include "string.h"



#define BUF_LEN_MAX 20//
#define BUF_NUM_MAX 16

typedef struct data_node
{
    struct data_node *next;
    uint8_t *buf;
    unsigned int len;
    bool bempty;
} DATA_NODE;



void empty_queue_init(void);
void add_empty_node(DATA_NODE *node);
DATA_NODE *get_empty_node(void);
void valid_queue_init(void);
void add_valid_node(DATA_NODE *node);
DATA_NODE *get_valid_node(void);





#ifdef __cplusplus
}
#endif

#endif /*_UPAY_LINKLIST_H_*/

/******************* (C) COPYRIGHT 2022 Realtek Semiconductor Corporation *****END OF FILE****/
