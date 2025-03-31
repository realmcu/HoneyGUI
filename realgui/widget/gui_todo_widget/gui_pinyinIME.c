/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_pinyinIME.c
  * @brief pinyin IME
  * @details pinyin IME
  * @author roy@realsil.com.cn
  * @date 2024/6/03
  * @version 1.0
  ***************************************************************************************
    * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <guidef.h>
#include <string.h>
#include <gui_app.h>

#include "gui_pinyinIME.h"


/*============================================================================*
 *                           Types
 *============================================================================*/


/*============================================================================*
 *                            Macros
 *============================================================================*/


/*============================================================================*
 *                           Constants
 *============================================================================*/


/*============================================================================*
 *                            Variables
 *============================================================================*/

static gui_ime_t *IME_pinyin;

static void gui_pinyin_get_cand_from_page(gui_ime_t *this)
{
    uint32_t cand_offset = this->cand_dict_info->len + 1 + this->cand_dict_info->offset;
    uint32_t before = this->cand_page * this->cand_page_size;
    uint32_t read_num = 0;

    if (before > this->cand_dict_info->num_wd)
    {
        // out of range
        return ;
    }
    else
    {
        cand_offset += this->char_len_byte * before;
    }

    read_num = _UI_MIN(this->cand_page_size, this->cand_dict_info->num_wd - before);
    memset(this->buff_cand, 0, (uint64_t)this->cand_page_size * (uint64_t)this->char_len_byte);

    memcpy(this->buff_cand, this->dict_addr + cand_offset, this->char_len_byte * read_num);
    this->cand_num = read_num;
}

/*============================================================================*
 *                           Public Functions
 *============================================================================*/
void gui_pinyin_reset_page(gui_ime_t *this)
{
    this->cand_page = 0;
}

void gui_pinyin_page_flip(gui_ime_t *this, uint8_t dir)
{
    if (dir == IME_PAGE_FORWARD)
    {
        if ((this->cand_page + 1) * this->cand_page_size < this->cand_dict_info->num_wd)
        {
            this->cand_page ++;
        }
    }
    else if (dir == IME_PAGE_BACKWARD)
    {
        if (this->cand_page > 0)
        {
            this->cand_page --;
        }
    }
}

void gui_pinyin_clear_cand(gui_ime_t *this)
{
    this->cand_num = 0;
    this->last_key_len = 0;
    memset(this->buff_cand, 0, (uint64_t)this->cand_page_size * (uint64_t)this->char_len_byte);
}

uint16_t gui_pinyin_get_page_size(gui_ime_t *this)
{
    return this->cand_page_size;
}

uint16_t gui_pinyin_get_cand_num(gui_ime_t *this)
{
    return this->cand_num;
}

uint8_t *gui_pinyin_get_cand_buff(gui_ime_t *this)
{
    return this->buff_cand;
}
uint8_t gui_pinyin_get_char_len_byte(gui_ime_t *this)
{
    return this->char_len_byte;
}

uint16_t gui_pinyin_update_cand(gui_ime_t *this, void *key, uint8_t len)
{
    uint16_t dict_len = this->dict_len;
    Dict_info_t *item = this->dict_info;
    Dict_info_t *dict = this->dict_info;
    uint8_t last_len = this->last_key_len;
    char *pinyin = (char *)key;

    if (len == 0 || pinyin[0] == 'i' || pinyin[0] == 'u' || pinyin[0] == 'v')
    {
        this->last_key_len = len;
        return 0;
    }

    if (last_len == len)
    {
        gui_pinyin_get_cand_from_page(this);
        // gui_log("%d:%s\n", this->cand_num, this->buff_cand);
        return this->cand_num;
    }
    else
    {
        gui_pinyin_reset_page(this);
    }

    // TODO: 1/2 search
    for (uint16_t i = 0; i < dict_len; i++)
    {
        if (pinyin[0] == item->key[0])
        {
            break;
        }
        item = dict + i;
    }

    while (pinyin[0] == item->key[0])
    {
        if (len != item->len)
        {

        }
        else if (strcmp(pinyin, item->key) == 0)
        {
            {
                char line[1024];
                this->cand_dict_info = item;

                memset(line, 0, sizeof(line));
                // gui_log("%d %s %d, %d\n", item->len, item->key, item->offset, item->num_wd);

                memcpy(line, this->dict_addr + item->offset,
                       this->char_len_byte * item->num_wd + item->len + 1 + 2);
                // gui_log("%s\n", line);

                gui_pinyin_get_cand_from_page(this);
                // gui_log("%d:%s\n", this->cand_num, this->buff_cand);
            }
            break;
        }
        item++;
    }

    this->last_key_len = len;
    return this->cand_num;
}

gui_ime_t *gui_pinyin_IME_create(uint16_t kb_cand_page_size)
{
    IME_pinyin = (gui_ime_t *)gui_malloc(sizeof(gui_ime_t));
    GUI_ASSERT(IME_pinyin != NULL);
    IME_pinyin->char_len_byte = CHAR_LEN_BYTE;
    IME_pinyin->buff_cand = (uint8_t *)gui_malloc((uint64_t)kb_cand_page_size *
                                                  (uint64_t)IME_pinyin->char_len_byte);
    GUI_ASSERT(IME_pinyin->buff_cand != NULL);
    IME_pinyin->cand_page_size = kb_cand_page_size;

    // api
    IME_pinyin->api = (void *)gui_malloc(sizeof(gui_ime_api_t));
    GUI_ASSERT(IME_pinyin->api != NULL);
    {
        gui_ime_api_t *api = (gui_ime_api_t *)(IME_pinyin->api);

        api->reset_page = gui_pinyin_reset_page;
        api->page_flip = gui_pinyin_page_flip;
        api->get_page_size = gui_pinyin_get_page_size;
        api->get_cand_buff = gui_pinyin_get_cand_buff;
        api->get_cand_num = gui_pinyin_get_cand_num;
        api->get_char_len_byte = gui_pinyin_get_char_len_byte;
        api->clear_cand = gui_pinyin_clear_cand;
        api->update_cand = gui_pinyin_update_cand;
    }

    return IME_pinyin;
}

#include "cJSON.h"
static void gui_IME_parse_dict_json(gui_ime_t *this, char *buff_json)
{
    cJSON *cjson_root = cJSON_Parse(buff_json);
    if (cjson_root == NULL)
    {
        gui_log("parse dict_json fail.\n");
        return ;
    }

    cJSON *cjson_num_pair = cJSON_GetObjectItem(cjson_root, "num_pair");
    cJSON *cjson_dict_info_array = cJSON_GetObjectItem(cjson_root, "dict_info");
    uint32_t dict_len = cJSON_GetArraySize(cjson_dict_info_array);

    // gui_log(" %s:%d \n", cjson_num_pair->string, cjson_num_pair->valueint);
    // gui_log(" %s:%d \n", cjson_dict_info_array->string, dict_len);
    this->dict_len = dict_len;
    if (cjson_dict_info_array && this->dict_len != 0)
    {
        cJSON *cjson_dict_item = NULL;
        GUI_UNUSED(cjson_dict_item);
        cJSON *cjson_len = NULL;
        cJSON *cjson_key = NULL;
        cJSON *cjson_os = NULL;
        cJSON *cjson_num = NULL;

        this->dict_info = gui_malloc(this->dict_len * sizeof(Dict_info_t));
        for (uint32_t i = 0; i < dict_len; i++)
        {
            cJSON *cjson_dict_item = cJSON_GetArrayItem(cjson_dict_info_array, i);

            cjson_len = cJSON_GetObjectItem(cjson_dict_item, "len");
            cjson_key = cJSON_GetObjectItem(cjson_dict_item, "key");
            cjson_os = cJSON_GetObjectItem(cjson_dict_item, "os");
            cjson_num = cJSON_GetObjectItem(cjson_dict_item, "num");

            // gui_log(" %d, %s, %d, %d \n", cjson_len->valueint, cjson_key->valuestring, cjson_os->valueint, cjson_num->valueint);
            //
            this->dict_info[i].len = cjson_len->valueint;

            this->dict_info[i].key = gui_malloc(this->dict_info[i].len + 1);
            GUI_ASSERT(this->dict_info[i].key != NULL);
            memset(this->dict_info[i].key, 0, this->dict_info[i].len + 1);
            memcpy(this->dict_info[i].key, cjson_key->valuestring, this->dict_info[i].len);

            this->dict_info[i].offset = cjson_os->valueint;
            this->dict_info[i].num_wd = cjson_num->valueint;
        }

    }
    cJSON_Delete(cjson_root);
}

void gui_IME_set_dict(gui_ime_t *this, void *file_dict_info, void *file_dict)
{
    void *dict_info_addr = gui_get_file_address(file_dict_info);

    gui_IME_parse_dict_json(this, dict_info_addr);

    this->dict_addr = gui_get_file_address(file_dict);
    if (!this->dict_addr)
    {
        gui_log("error\n");
        return ;
    }
}

