/**
 * @file def_list.h
 * @author wenjing_jiang(wenjing_jiang@realsil.com.cn)
 * @brief
 * @version 0.1
 * @date 2024-06-14
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef __DEF_LIST_H__
#define __DEF_LIST_H__


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/*****Compiler Related*****/
#if defined(__CC_ARM) || defined(__CLANG_ARM)           /* ARM Compiler */
#include <stdarg.h>
#define gui_inline                   static __inline
#elif defined (__IAR_SYSTEMS_ICC__)     /* for IAR Compiler */
#include <stdarg.h>
#define gui_inline                   static inline
#elif defined (__GNUC__)                /* GNU GCC Compiler */
#ifdef RT_USING_NEWLIB
#include <stdarg.h>
#else
/* the version of GNU GCC must be greater than 4.x */
#endif
#define gui_inline                   static __inline
#elif defined _WIN32
#define gui_inline                   __inline
#else
#error not supported tool chain
#endif

struct gui_node_list
{
    struct gui_node_list *next;                          /**< point to next node. */
    struct gui_node_list *prev;                          /**< point to prev node. */
};
typedef struct gui_node_list gui_node_list_t;                 /**< Type for lists. */

gui_inline void gui_list_init(gui_node_list_t *l)
{
    l->next = l->prev = l;
}

gui_inline void gui_list_append(gui_node_list_t *l, gui_node_list_t *n)
{
    gui_node_list_t *node = l;

    while (node->next != l)
    {
        node = node->next;
    }

    /* Appends the node to the tail */
    node->next = n;
    n->next = l;
}

/**
 * @brief insert a node before a list
 *
 * @param n new node to be inserted
 * @param l list to insert it
 */
gui_inline void gui_list_insert_before(gui_node_list_t *l, gui_node_list_t *n)
{
    l->prev->next = n;
    n->prev = l->prev;

    l->prev = n;
    n->next = l;
}

gui_inline void gui_list_insert(gui_node_list_t *l, gui_node_list_t *n)
{
    n->next = l->next;
    n->prev = l;
    l->next->prev = n;
    l->next = n;
}

/**
 * @brief remove node from list.
 * @param n the node to remove from the list.
 */
gui_inline void gui_list_remove(gui_node_list_t *n)
{
    n->next->prev = n->prev;
    n->prev->next = n->next;

    n->next = n->prev = n;
}

/**
 * gui_list_entry - return the member address of ptr, if the type of ptr is the
 * struct type.
 */
#define gui_list_entry(node, type, member) \
    ((type *)((char *)(node) - (unsigned long)(uintptr_t)(&((type *)0)->member)))

/**
 * gui_list_for_each - iterate over a list
 * @pos:    the rt_list_t * to use as a loop cursor.
 * @head:   the head for your list.
 */
#define gui_list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * gui_list_for_each_safe - iterate over a list safe against removal of list entry
 * @pos:    the rt_list_t * to use as a loop cursor.
 * @n:      another rt_list_t * to use as temporary storage
 * @head:   the head for your list.
 */
#define gui_list_for_each_safe(pos, n, head) \
    for (pos = (head)->next, n = pos->next; pos != (head); \
         pos = n, n = pos->next)

#ifdef __cplusplus
}
#endif

#endif

