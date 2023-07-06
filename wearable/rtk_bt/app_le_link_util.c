/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <string.h>
#include "os_mem.h"
#include "dp_ble_info.h"

/**  @brief  App define global app data structure */


T_APP_LE_LINK *app_find_le_link_by_conn_id(uint8_t conn_id)
{
    T_APP_LE_LINK *p_link = NULL;
    uint8_t        i;

    for (i = 0; i < MAX_BLE_LINK_NUM; i++)
    {
        if (le_db.le_link[i].used == true &&
            le_db.le_link[i].conn_id == conn_id)
        {
            p_link = &le_db.le_link[i];
            break;
        }
    }

    return p_link;
}

T_APP_LE_LINK *app_find_le_link_by_addr(uint8_t *bd_addr)
{
    T_APP_LE_LINK *p_link = NULL;
    uint8_t        i;

    if (bd_addr != NULL)
    {
        for (i = 0; i < MAX_BLE_LINK_NUM; i++)
        {
            if (le_db.le_link[i].used == true &&
                !memcmp(le_db.le_link[i].bd_addr, bd_addr, 6))
            {
                p_link = &le_db.le_link[i];
                break;
            }
        }
    }

    return p_link;
}
#ifdef RTL8772F
T_APP_LE_LINK *app_find_le_link_by_tts_handle(T_TTS_HANDLE handle)
{
    T_APP_LE_LINK *p_link = NULL;
    uint8_t        i;

    if (handle != NULL)
    {
        for (i = 0; i < MAX_BLE_LINK_NUM; i++)
        {
            if ((le_db.le_link[i].used == true) &&
                (le_db.le_link[i].tts_handle == handle))
            {
                p_link = &le_db.le_link[i];
                break;
            }
        }
    }

    return p_link;
}
#endif

T_APP_LE_LINK *app_alloc_le_link_by_conn_id(uint8_t conn_id)
{
    T_APP_LE_LINK *p_link = NULL;
    uint8_t        i;

    for (i = 0; i < MAX_BLE_LINK_NUM; i++)
    {
        if (le_db.le_link[i].used == false)
        {
            p_link = &le_db.le_link[i];

            p_link->used    = true;
            p_link->id      = i;
            p_link->conn_id = conn_id;
            break;
        }
    }

    return p_link;
}

bool app_free_le_link(T_APP_LE_LINK *p_link)
{
    if (p_link != NULL)
    {
        if (p_link->used == true)
        {
#ifdef RTL8772F
            if (p_link->tts_frame_buf != NULL)
            {
                os_mem_free(p_link->tts_frame_buf);
            }
            if (p_link->p_embedded_cmd != NULL)
            {
                os_mem_free(p_link->p_embedded_cmd);
            }

            if (p_link->eq_data_buf != NULL)
            {
                os_mem_free(p_link->eq_data_buf);
            }
#endif
            while (p_link->disc_cb_list.count > 0)
            {
                T_LE_DISC_CB_ENTRY *p_entry;
                p_entry = os_queue_out(&p_link->disc_cb_list);
                if (p_entry)
                {
                    os_mem_free(p_entry);
                }
            }
            memset(p_link, 0, sizeof(T_APP_LE_LINK));
            p_link->conn_id = 0xFF;
            return true;
        }
    }

    return false;
}

bool app_reg_le_link_disc_cb(uint8_t conn_id, P_FUN_LE_LINK_DISC_CB p_fun_cb)
{
    T_APP_LE_LINK *p_link = app_find_le_link_by_conn_id(conn_id);
    if (p_link != NULL)
    {
        T_LE_DISC_CB_ENTRY *p_entry;
#if defined RTL8772F || defined RTL8762G
        for (uint8_t i = 0; i < p_link->disc_cb_list.count; i++)
        {
            p_entry = os_queue_peek(&p_link->disc_cb_list, i);
            if (p_entry != NULL && p_entry->disc_callback == p_fun_cb)
            {
                return true;
            }
        }
#elif defined RTL8762D
        T_OS_QUEUE_ELEM *p_curr_elem;
        for (uint8_t i = 0; i < p_link->disc_cb_list.count; i++)
        {
            p_curr_elem = (void *)&p_link->disc_cb_list.p_first;
            uint8_t index = i;
            while (index > 0 && p_curr_elem != NULL)
            {
                p_curr_elem = p_curr_elem->p_next;
                index--;
            }
            p_entry = (void *)p_curr_elem;
            if (p_entry != NULL && p_entry->disc_callback == p_fun_cb)
            {
                return true;
            }
        }
#endif
        p_entry = os_mem_zalloc(RAM_TYPE_DATA_ON, sizeof(T_LE_DISC_CB_ENTRY));
        if (p_entry != NULL)
        {
            p_entry->disc_callback = p_fun_cb;
            os_queue_in(&p_link->disc_cb_list, p_entry);
            return true;
        }
    }
    return false;
}

uint8_t app_get_ble_link_num(void)
{
    uint8_t num = 0;
    uint8_t        i;

    for (i = 0; i < MAX_BLE_LINK_NUM; i++)
    {
        if (le_db.le_link[i].used == true)
        {
            num++;
        }
    }

    return num;
}

#if 0
bool app_check_b2b_link(uint8_t *bd_addr)
{
    bool ret = false;

    if (!memcmp(bd_addr, app_cfg_nv.bud_local_addr, 6) ||
        !memcmp(bd_addr, app_cfg_nv.bud_peer_addr, 6))
    {
        ret = true;
    }

    return ret;
}

bool app_check_b2b_link_by_id(uint8_t id)
{
    bool ret = false;

    if (app_db.br_link[id].used)
    {
        ret = app_check_b2b_link(app_db.br_link[id].bd_addr);
    }

    return ret;
}

bool app_check_b2s_link(uint8_t *bd_addr)
{
    return !app_check_b2b_link(bd_addr);
}

bool app_check_b2s_link_by_id(uint8_t id)
{
    bool ret = false;

    if (app_db.br_link[id].used)
    {
        ret = app_check_b2s_link(app_db.br_link[id].bd_addr);
    }

    return ret;
}

T_APP_BR_LINK *app_find_b2s_link(uint8_t *bd_addr)
{
    T_APP_BR_LINK *p_link = NULL;

    if (app_check_b2s_link(bd_addr))
    {
        p_link = app_find_br_link(bd_addr);
    }

    return p_link;
}

uint8_t app_find_b2s_link_num(void)
{
    uint8_t num = 0;
    uint8_t i = 0;

    for (i = 0; i < MAX_BR_LINK_NUM; i++)
    {
        if (app_db.br_link[i].used)
        {
            num++;
        }
    }
    return num;
}
#endif
