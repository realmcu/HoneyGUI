/*
* Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
*/

#ifndef _BT_LINKBACK_H_
#define _BT_LINKBACK_H_

#include <stdint.h>
#include <stdbool.h>
#include "btm.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @defgroup APP_LINKBACK App Linkback
  * @brief App Linkback
  * @{
  */

typedef union
{
    bool is_source;
    bool is_target;
} T_LINKBACK_SEARCH_PARAM;

typedef struct
{
    uint16_t protocol_version;
    uint8_t server_channel;
    uint8_t local_server_chann;
    bool feature;
} T_LINKBACK_CONN_PARAM;

typedef struct
{
    uint8_t bd_addr[6];
    uint32_t plan_profs;
    bool is_force;
    bool is_special;
    T_LINKBACK_SEARCH_PARAM search_param;
    bool check_bond_flag;
    uint16_t retry_timeout;
} T_LINKBACK_NODE;

typedef struct st_linkback_node_item
{
    struct st_linkback_node_item *next;
    bool is_used;
    T_LINKBACK_NODE linkback_node;
} T_LINKBACK_NODE_ITEM;

typedef struct
{
    T_LINKBACK_NODE_ITEM *head;
    T_LINKBACK_NODE_ITEM *tail;
} T_LINKBACK_TODO_QUEUE;

typedef struct
{
    T_LINKBACK_NODE linkback_node;
    bool is_valid;
    bool is_exit;
    uint16_t retry_timeout;
    uint32_t remain_profs;
    uint32_t doing_prof;
    bool is_sdp_ok;
    uint8_t prof_retry_cnt;
    T_LINKBACK_CONN_PARAM linkback_conn_param;
} T_LINKBACK_ACTIVE_NODE;


bool linkback_profile_search_start(uint8_t *bd_addr, uint32_t prof, bool is_special,
                                   T_LINKBACK_SEARCH_PARAM *param);
bool linkback_profile_connect_start(uint8_t *bd_addr, uint32_t prof, T_LINKBACK_CONN_PARAM *param);
void linkback_profile_disconnect_start(uint8_t *bd_addr, uint32_t profs);

void linkback_todo_queue_init(void);
T_LINKBACK_NODE_ITEM *linkback_todo_queue_malloc_node_item(void);
void linkback_todo_queue_free_node_item(T_LINKBACK_NODE_ITEM *p_item);
void linkback_todo_queue_insert_normal_node(uint8_t *bd_addr, uint32_t plan_profs,
                                            uint16_t retry_timeout);
void linkback_todo_queue_insert_force_node(uint8_t *bd_addr, uint32_t plan_profs,
                                           bool is_special, T_LINKBACK_SEARCH_PARAM *search_param, bool check_bond_flag,
                                           uint16_t retry_timeout);

bool linkback_todo_queue_take_first_node(T_LINKBACK_NODE *node);
void linkback_todo_queue_remove_plan_profs(uint8_t *bd_addr, uint32_t plan_profs);
void linkback_todo_queue_delete_all_node(void);
void linkback_active_node_init(void);
void linkback_active_node_load_doing_prof(void);
void linkback_active_node_load(T_LINKBACK_NODE *node);
void linkback_active_node_step_suc_adjust_remain_profs(void);
void linkback_active_node_step_fail_adjust_remain_profs(void);
void linkback_active_node_src_conn_fail_adjust_remain_profs(void);
void linkback_active_node_remain_profs_add(uint32_t profs, bool check_bond_flag);
void linkback_active_node_remain_profs_sub(uint32_t profs);
bool linkback_active_node_judge_cur_conn_addr(uint8_t *bd_addr);
bool linkback_active_node_judge_cur_conn_prof(uint8_t *bd_addr, uint32_t prof);

bool linkback_load_bond_list(uint16_t retry_timeout);
bool linkback_check_bond_flag(uint8_t *bd_addr, uint32_t prof);
bool check_bond_smart_phone(void);
bool check_bond_buds(void);

bool check_br_link_connected_profile(uint8_t *bd_addr, uint32_t *profs);
void linkback_handle_sdp_attr_info(T_BT_EVENT_PARAM *param);
void linkback_handle_sdp_discov_cmpl(T_BT_EVENT_PARAM *param);
void linkback_handle_profile_conn(uint8_t *bd_addr, uint32_t prof);
void linkback_run(void);
void app_create_connection(uint8_t *bd_addr, uint32_t profile_mask);
void linkback_todo_queue_all_node(void);

/** End of APP_LINKBACK
* @}
*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
