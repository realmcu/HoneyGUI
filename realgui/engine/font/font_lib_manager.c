/**
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file font_lib_manager.c
  * @brief Unified font library manager implementation
  * @details Dynamic font library with reference counting for BMP and TTF fonts
  * @author luke_sun
  * @date 2025/01/07
  * @version v1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2025 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

#include <string.h>
#include "font_lib_manager.h"

/*============================================================================*
 *                            Variables
 *============================================================================*/

/** @brief Head of font library linked list */
static FONT_LIB_NODE *font_lib_head = NULL;

/** @brief Total count of loaded fonts */
static uint32_t font_lib_count = 0;

/*============================================================================*
 *                           Public Functions
 *============================================================================*/

FONT_LIB_NODE *gui_font_lib_find(uint8_t *font_file)
{
    FONT_LIB_NODE *node = font_lib_head;
    while (node != NULL)
    {
        if (node->font_file == font_file)
        {
            return node;
        }
        node = node->next;
    }
    return NULL;
}

FONT_LIB_NODE *gui_font_lib_find_by_size(uint8_t font_size, FONT_SRC_TYPE font_type)
{
    FONT_LIB_NODE *node = font_lib_head;
    while (node != NULL)
    {
        if (node->font_size == font_size && node->font_type == font_type)
        {
            return node;
        }
        node = node->next;
    }
    return NULL;
}

FONT_LIB_NODE *gui_font_lib_register(uint8_t *font_file,
                                     uint8_t font_size,
                                     FONT_SRC_MODE src_mode,
                                     FONT_SRC_TYPE font_type,
                                     uint8_t *cached_data,
                                     uint32_t cached_size)
{
    /* Check if already registered */
    FONT_LIB_NODE *node = gui_font_lib_find(font_file);
    if (node != NULL)
    {
        node->ref_count++;
        // gui_log("font_lib: addref %p, type=%d, ref_count=%d\n",
        //         font_file, font_type, node->ref_count);
        return node;
    }

    /* Create new node */
    node = gui_malloc(sizeof(FONT_LIB_NODE));
    if (node == NULL)
    {
        gui_log("font_lib: failed to allocate node\n");
        return NULL;
    }

    node->font_file = font_file;
    node->font_size = font_size;
    node->src_mode = src_mode;
    node->font_type = font_type;
    node->cached_data = cached_data;
    node->cached_size = cached_size;
    node->ref_count = 1;

    /* Insert at head */
    node->next = font_lib_head;
    font_lib_head = node;
    font_lib_count++;

    // gui_log("font_lib: registered %p, type=%d, size=%d, src=%d, count=%d\n",
    //         font_file, font_type, font_size, src_mode, font_lib_count);

    return node;
}

uint32_t gui_font_lib_addref(FONT_LIB_NODE *node)
{
    if (node == NULL)
    {
        return 0;
    }
    node->ref_count++;
    // gui_log("font_lib: addref %p, ref_count=%d\n", node->font_file, node->ref_count);
    return node->ref_count;
}

/**
 * @brief Internal: Remove and free a font node
 */
static void font_lib_node_remove(FONT_LIB_NODE *target)
{
    if (target == NULL)
    {
        return;
    }

    /* Remove from linked list */
    if (font_lib_head == target)
    {
        font_lib_head = target->next;
    }
    else
    {
        FONT_LIB_NODE *prev = font_lib_head;
        while (prev != NULL && prev->next != target)
        {
            prev = prev->next;
        }
        if (prev != NULL)
        {
            prev->next = target->next;
        }
    }

    /* Free cached data (not for MEM mode) */
    if (target->cached_data != NULL && target->src_mode != FONT_SRC_MEMADDR)
    {
        gui_free(target->cached_data);
    }

    // gui_log("font_lib: removed %p, count=%d\n", target->font_file, font_lib_count - 1);
    gui_free(target);
    font_lib_count--;
}

void gui_font_lib_release(uint8_t *font_file)
{
    FONT_LIB_NODE *node = gui_font_lib_find(font_file);
    gui_font_lib_release_node(node);
}

void gui_font_lib_release_node(FONT_LIB_NODE *node)
{
    if (node == NULL)
    {
        return;
    }

    if (node->ref_count > 0)
    {
        node->ref_count--;
    }

    // gui_log("font_lib: release %p, ref_count=%d\n", node->font_file, node->ref_count);

    if (node->ref_count == 0)
    {
        font_lib_node_remove(node);
    }
}

uint32_t gui_font_lib_get_count(void)
{
    return font_lib_count;
}

FONT_LIB_NODE *gui_font_lib_get_head(void)
{
    return font_lib_head;
}

void gui_font_lib_clear_all(void)
{
    FONT_LIB_NODE *node = font_lib_head;
    while (node != NULL)
    {
        FONT_LIB_NODE *next = node->next;

        if (node->cached_data != NULL && node->src_mode != FONT_SRC_MEMADDR)
        {
            gui_free(node->cached_data);
        }
        gui_free(node);

        node = next;
    }

    font_lib_head = NULL;
    font_lib_count = 0;
    // gui_log("font_lib: cleared all fonts\n");
}
