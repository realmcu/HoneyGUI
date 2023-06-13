/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file
   * @brief     This file handles BLE peripheral application routines.
   * @author    jane
   * @date      2017-06-06
   * @version   v1.0
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
   **************************************************************************************
  */

#ifndef _STRUCTURE_LIST__
#define _STRUCTURE_LIST__

#ifdef __cplusplus
extern "C" {
#endif

#include "app_msg.h"
#include "stdbool.h"
#include "stddef.h"

/**
 * Single List structure
 */
struct slist_node
{
    struct slist_node *next;                         /**< point to next node. */
};
typedef struct slist_node slist_t;                /**< Type for single list. */

static __inline void slist_append(slist_t *l, slist_t *n)
{
    struct slist_node *node;

    node = l;
    while (node->next) { node = node->next; }

    /* append the node to the tail */
    node->next = n;
    n->next = NULL;
}
static __inline slist_t *slist_remove(slist_t *l, slist_t *n)
{
    /* remove slist head */
    struct slist_node *node = l;
    while (node->next && node->next != n) { node = node->next; }

    /* remove node */
    if (node->next != (slist_t *)0) { node->next = node->next->next; }

    return l;
}
static __inline slist_t *slist_first(slist_t *l)
{
    return l->next;
}
static __inline slist_t *slist_tail(slist_t *l)
{
    while (l->next) { l = l->next; }

    return l;
}
static __inline slist_t *slist_next(slist_t *n)
{
    return n->next;
}

#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))


#ifdef __cplusplus
}
#endif

#endif
