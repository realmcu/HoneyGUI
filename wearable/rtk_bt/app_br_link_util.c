/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <string.h>
//#include <app_br_link_util.h>
#include "os_mem.h"
#include "remote.h"
#include "dp_br_info.h"
#if (RTK_BR_TASK == 1)
#include "app_cfg.h"
#endif

/**  @brief  App define global app data structure */


uint32_t app_connected_profiles(void)
{
    uint32_t i, connected_profiles = 0;

    for (i = 0; i < MAX_BR_LINK_NUM; i++)
    {
        connected_profiles |= br_db.br_link[i].connected_profile;
    }
    return connected_profiles;
}


uint8_t app_connected_profile_link_num(uint32_t profile_mask)
{
    uint8_t i, link_number = 0;

    for (i = 0; i < MAX_BR_LINK_NUM; i++)
    {
        if (br_db.br_link[i].connected_profile & profile_mask)
        {
            link_number++;
        }

    }
    return link_number;
}

uint8_t app_find_sco_conn_num(void)
{
    uint8_t i;
    uint8_t num = 0;
    for (i = 0; i < MAX_BR_LINK_NUM; i++)
    {
        if (br_db.br_link[i].used == true &&
            br_db.br_link[i].sco_handle != 0)
        {
            num++;
        }
    }
    return num;
}

T_APP_BR_LINK *app_find_br_link(uint8_t *bd_addr)
{
    T_APP_BR_LINK *p_link = NULL;
    uint8_t        i;

    if (bd_addr != NULL)
    {
        for (i = 0; i < MAX_BR_LINK_NUM; i++)
        {
            if (br_db.br_link[i].used == true &&
                !memcmp(br_db.br_link[i].bd_addr, bd_addr, 6))
            {
                p_link = &br_db.br_link[i];
                break;
            }
        }
    }

    return p_link;
}

T_APP_BR_LINK *app_find_br_link_by_tts_handle(T_TTS_HANDLE handle)
{
    T_APP_BR_LINK *p_link = NULL;
    uint8_t        i;

    if (handle != NULL)
    {
        for (i = 0; i < MAX_BR_LINK_NUM; i++)
        {
            if ((br_db.br_link[i].used == true) &&
                (br_db.br_link[i].tts_handle == handle))
            {
                p_link = &br_db.br_link[i];
                break;
            }
        }
    }

    return p_link;
}

T_APP_BR_LINK *app_alloc_br_link(uint8_t *bd_addr)
{
    T_APP_BR_LINK *p_link = NULL;
    uint8_t        i;

    if (bd_addr != NULL)
    {
        for (i = 0; i < MAX_BR_LINK_NUM; i++)
        {
            if (br_db.br_link[i].used == false)
            {
                p_link = &br_db.br_link[i];

                p_link->used = true;
                p_link->id   = i;
                memcpy(p_link->bd_addr, bd_addr, 6);
                break;
            }
        }
    }

    return p_link;
}

bool app_free_br_link(T_APP_BR_LINK *p_link)
{
    if (p_link != NULL)
    {
        if (p_link->used == true)
        {
            if (p_link->tts_frame_buf != NULL)
            {
                os_mem_free(p_link->tts_frame_buf);
            }

            if (p_link->p_gfps_cmd != NULL)
            {
                os_mem_free(p_link->p_gfps_cmd);
            }

            memset(p_link, 0, sizeof(T_APP_BR_LINK));
            return true;
        }
    }

    return false;
}

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

    if (br_db.br_link[id].used)
    {
        ret = app_check_b2b_link(br_db.br_link[id].bd_addr);
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

    if (br_db.br_link[id].used)
    {
        ret = app_check_b2s_link(br_db.br_link[id].bd_addr);
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
        if (br_db.br_link[i].used)
        {
            num++;
        }
    }
    return num;
}
