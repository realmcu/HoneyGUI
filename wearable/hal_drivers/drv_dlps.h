/*
 * Copyright (c) 2022
 *
 *
 *
 *
 *
 *
 */

#ifndef __DRV_DLPS_H__
#define __DRV_DLPS_H__


#include "string.h"
#include "rtl_hal_peripheral.h"


/**
 * Single List structure for dlps
 */
struct dlps_slist_node
{
    struct dlps_slist_node *next;
};
typedef struct dlps_slist_node dlps_slist_t;

static __inline void dlps_slist_append(dlps_slist_t *l, dlps_slist_t *n)
{
    struct dlps_slist_node *node;

    node = l;
    while (node->next) { node = node->next; }

    /* append the node to the tail */
    node->next = n;
    n->next = NULL;
}
static __inline dlps_slist_t *dlps_slist_remove(dlps_slist_t *l, dlps_slist_t *n)
{
    /* remove slist head */
    struct dlps_slist_node *node = l;
    while (node->next && node->next != n) { node = node->next; }

    /* remove node */
    if (node->next != (dlps_slist_t *)0) { node->next = node->next->next; }

    return l;
}
static __inline dlps_slist_t *dlps_slist_first(dlps_slist_t *l)
{
    return l->next;
}
static __inline dlps_slist_t *dlps_slist_tail(dlps_slist_t *l)
{
    while (l->next) { l = l->next; }

    return l;
}
static __inline dlps_slist_t *dlps_slist_next(dlps_slist_t *n)
{
    return n->next;
}

#define dlps_container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))



typedef struct drv_dlps_cb_item
{
    dlps_slist_t slist;
    bool (*dlps_cb)(void);
    const char *name;

} drv_dlps_cb_item_t;


void drv_dlps_exit_cbacks_register(const char *name, bool (*cbacks)(void));
void drv_dlps_enter_cbacks_register(const char *name, bool (*cbacks)(void));
void drv_dlps_check_cbacks_register(const char *name, bool (*cbacks)(void));
void drv_dlps_wakeup_cbacks_register(const char *name, bool (*cbacks)(void));


#endif  /* __DRV_DLPS_H__ */

