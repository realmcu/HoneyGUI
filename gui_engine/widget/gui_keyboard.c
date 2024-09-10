/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file gui_keyboard.c
  * @brief keyboard widget
  * @details keyboard widget
  * @author roy@realsil.com.cn
  * @date 2024/5/20
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
#include <gui_obj.h>
#include "font_mem.h"
#include <gui_img.h>

#include <gui_app.h>
#include "gui_button.h"
#include "gui_keyboard.h"
#include "gui_pinyinIME.h"
/** @defgroup WIDGET WIDGET
  * @{
  */
/*============================================================================*
 *                           Types
 *============================================================================*/
/** @defgroup WIDGET_Exported_Types WIDGET Exported Types
  * @{
  */

/** End of WIDGET_Exported_Types
  * @}
  */

/*============================================================================*
 *                           Constants
 *============================================================================*/
/** @defgroup WIDGET_Exported_Constants WIDGET Exported Constants
  * @{
  */


/** End of WIDGET_Exported_Constants
  * @}
  */

/*============================================================================*
 *                            Macros
 *============================================================================*/
/** @defgroup WIDGET_Exported_Macros WIDGET Exported Macros
  * @{
  */
#define KB_BUFF_INPUT_MAXLEN 128  //!<
#define KB_BUFF_DISPLAY_MAXLEN 128 //!<
#define KB_BUFF_KEY_MAXLEN 128
#define KB_PSWD_KEY_MAXLEN 128
/** End of WIDGET_Exported_Macros
  * @}
  */
/*============================================================================*
 *                            Variables
 *============================================================================*/
/** @defgroup WIDGET_Exported_Variables WIDGET Exported Variables
  * @{
  */

static gui_kb_t *global_kb;
static char buff_display[KB_BUFF_DISPLAY_MAXLEN];
static char buff_txt_display[KB_BUFF_DISPLAY_MAXLEN + 4];
static char buff_key[KB_BUFF_KEY_MAXLEN];
static char buff_pswd[KB_PSWD_KEY_MAXLEN];

static uint16_t len_display = 0;
static uint16_t len_key = 0;
static char *cursor; // point to buff_display


void gui_keyboard_set_mode(gui_kb_t *this, uint8_t mode);

gui_kb_t *gui_get_kb(void)
{
    return global_kb;
}

static char *gui_kb_get_cursor(void)
{
    return cursor;
}

static bool gui_kb_is_ascii(uint8_t byte)
{
    return ((byte >> 7) == 0);
}
static bool gui_kb_cursor_is_head(void)
{
    return (cursor == buff_display);
}
static bool gui_kb_cursor_is_tail(void)
{
    return ((cursor - buff_display) == len_display);
}
static uint8_t buff_kb_cursor_top_byte(void)
{
    return *(cursor - 1);
}
static void gui_kb_cursor_ls(void)
{
    if (cursor <= buff_display)
    {
        cursor = buff_display;
    }
    else
    {
        uint8_t len_shift = 1;
        uint8_t byte = buff_kb_cursor_top_byte();
        gui_kb_t *this = gui_get_kb();
        gui_ime_api_t *ime_api = NULL;
        uint8_t char_len = 1;

        if (this->ime)
        {
            ime_api = (gui_ime_api_t *)(this->ime->api);
            char_len = ime_api->get_char_len_byte(this->ime);
        }

        if (gui_kb_is_ascii(byte))
        {
            len_shift = 1;
        }
        else
        {
            len_shift = char_len;
        }

        cursor -= len_shift;
    }
}
static void gui_kb_cursor_rs(void)
{
    gui_kb_t *this = gui_get_kb();
    gui_ime_api_t *ime_api = NULL;
    uint8_t char_len = 1;

    if (this->ime)
    {
        ime_api = (gui_ime_api_t *)(this->ime->api);
        char_len = ime_api->get_char_len_byte(this->ime);
    }
    if ((cursor - buff_display) >= len_display)
    {
        cursor = buff_display + len_display;
    }
    else
    {
        uint8_t len_shift = 1;
        uint8_t byte = *cursor;

        if (gui_kb_is_ascii(byte))
        {
            len_shift = 1;
        }
        else
        {
            len_shift = char_len;
        }

        cursor += len_shift;
    }
}

static bool buff_display_isempty(void)
{
    if ((len_display == 0))
    {
        return  true;
    }
    else
    {
        return false;
    }
}

static uint8_t buff_display_push(char ch)
{
    uint16_t len = len_display;
    if (len < (KB_BUFF_DISPLAY_MAXLEN - 1))
    {
        buff_display[len] = ch;
        buff_display[len + 1] = '\0';
        len_display++;
        cursor++;
        return 0;
    }
    return 1;
}

static void buff_display_pop(void)
{
    uint16_t len = len_display;

    if (buff_display_isempty())
    {
        return;
    }

    len_display--;
    buff_display[len - 1] = '\0';
    cursor--;
}

static bool buff_key_isempty(void)
{
    if ((len_key == 0))
    {
        return  true;
    }
    else
    {
        return false;
    }
}

static uint8_t buff_key_push(char ch)
{
    uint16_t len = len_key;
    if (len < (KB_BUFF_KEY_MAXLEN - 1))
    {
        buff_key[len] = ch;
        buff_key[len + 1] = '\0';
        len_key++;
        return 0;
    }
    return 1;
}

static void buff_key_pop(void)
{
    uint16_t len = len_key;

    if (buff_key_isempty())
    {
        return;
    }

    len_key--;
    buff_key[len - 1] = '\0';
}
static void buff_key_clean(void)
{
    buff_key[0] = '\0';
    len_key = 0;
}

static uint16_t gui_kb_cursor_len(void)
{
    return (len_display - (cursor - buff_display));
}
static void gui_kb_cursor_set_tail(void)
{
    cursor = buff_display + len_display;
}
static void gui_kb_cursor_insert(char ch)
{
    if (gui_kb_cursor_is_tail())
    {
        buff_display_push(ch);
    }
    else
    {
        if (len_display + 1 < KB_BUFF_DISPLAY_MAXLEN - 1)
        {
            uint16_t len = gui_kb_cursor_len();
            memmove(cursor + 1, cursor, len + 1);
            *cursor = ch;
            len_display++;
            cursor++;
        }
    }
}
static void gui_kb_cursor_del()
{
    uint8_t len_del = 1;
    uint8_t byte = buff_kb_cursor_top_byte();
    gui_kb_t *this = gui_get_kb();
    gui_ime_api_t *ime_api = NULL;
    uint8_t char_len = 1;

    if (this->ime)
    {
        ime_api = (gui_ime_api_t *)(this->ime->api);
        char_len = ime_api->get_char_len_byte(this->ime);
    }

    if (gui_kb_is_ascii(byte))
    {
        len_del = 1;
    }
    else
    {
        len_del = char_len;
    }

    if (gui_kb_cursor_is_tail())
    {
        for (uint8_t i = 0; i < len_del; i++)
        {
            buff_display_pop();
        }
    }
    else
    {
        if (!gui_kb_cursor_is_head())
        {
            uint16_t len = gui_kb_cursor_len();
            memmove(cursor - len_del, cursor, len + 1);
            len_display -= len_del;
            cursor -= len_del;
        }
    }
}
static void gui_kb_input_prepare(gui_obj_t *obj)
{}
static void gui_kb_prepare(gui_obj_t *obj)
{}
static void gui_kb_draw_cb(gui_obj_t *obj)
{}
static void gui_kb_end(gui_obj_t *obj)
{}
static void gui_kb_destory(gui_obj_t *obj)
{}

static void gui_kb_cb(gui_obj_t *obj, T_OBJ_CB_TYPE cb_type)
{
    if (obj != NULL)
    {
        switch (cb_type)
        {
        case OBJ_PREPARE:
            gui_kb_prepare(obj);
            break;

        case OBJ_DRAW:
            gui_kb_draw_cb(obj);
            break;

        case OBJ_END:
            gui_kb_end(obj);
            break;

        case OBJ_DESTORY:
            gui_kb_destory(obj);
            break;

        default:
            break;
        }
    }
}

static bool key_is_sw(uint32_t key_value)
{
    uint32_t idx = KB_VALUE_IDX(key_value);
    uint32_t area = KB_VALUE_AREA(key_value);

    if (area == KB_AREA_FUNC)
    {
        if ((idx == KB_SW_IDX_CAP) || (idx == KB_SW_IDX_SYMBOL) || (idx == KB_SW_IDX_NUMBER) ||
            (idx == KB_SW_IDX_CNEN) || (idx == KB_SW_IDX_LOCK))
        {
            return true;
        }
    }
    return false;
}

static uint8_t kb2letter_index(uint8_t key_idx)
{
    char table[26] = {'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'z', 'x', 'c', 'v', 'b', 'n', 'm'};
    return (table[key_idx] - 'a');
}
static uint8_t kb2number_index(uint8_t key_idx)
{
    char table[10] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};
    return (table[key_idx] - '0');
}

static void *get_key_img(gui_kb_t *this, uint32_t key_value, uint8_t type)
{
    void *data = NULL;
    uint32_t idx = KB_VALUE_IDX(key_value);
    uint32_t area = KB_VALUE_AREA(key_value);
    uint16_t offset = 0;

    if (area == KB_AREA_LETTER)
    {
        uint8_t letter_idx = kb2letter_index(idx);

        //   KB_SRC_LOWWER     = 0,
        //   KB_SRC_LOWWER_CLK = 1,
        //   KB_SRC_UPPER      = 2,
        //   KB_SRC_UPPER_CLK  = 3,
        offset = (type > KB_SRC_LOWWER_CLK) ? 2 * 26 : 0;
        idx = 2 * letter_idx + (type & 0x01);
    }
    else if (area == KB_AREA_NUM)
    {
        uint8_t num_idx = kb2number_index(idx);

        offset = 4 * 26;
        idx = 2 * num_idx + (type & 0x01);
    }
    else if (area == KB_AREA_SYMBOL)
    {
        offset = 4 * 26 + 2 * 10;
        idx = 2 * idx + (type & 0x01);
    }
    else if (area == KB_AREA_FUNC)
    {
        offset = this->file_mark[0];
        idx = 2 * idx + (type & 0x01);
    }

    data = this->img_array[idx + offset];
    return data;
}

static void *get_other_img(gui_kb_t *this, uint8_t img_idx)
{
    void *data = NULL;
    uint32_t idx = img_idx;
    uint16_t offset = this->file_mark[1];

    data = this->img_array[idx + offset];
    return data;
}

static void get_letter_location(uint8_t key_idx, uint8_t *cor_x, uint8_t *cor_y)
{
    //  a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z
    char table_x[26] = {0, 4, 2, 2, 2, 3, 4, 5, 7, 6, 7, 8, 6, 5, 8, 9, 0, 3, 1, 4, 6, 3, 1, 1, 5, 0};
    char table_y[26] = {1, 2, 2, 1, 0, 1, 1, 1, 0, 1, 1, 1, 2, 2, 0, 0, 0, 0, 1, 0, 0, 2, 0, 2, 0, 2};

    *cor_x = table_x[key_idx];
    *cor_y = table_y[key_idx];
}
static void get_func_location(uint8_t key_idx, uint8_t *cor_x, uint8_t *cor_y)
{
    //CAP,DL,SY,123, L,SP, R,CE,DN, ,,ET,LK
    char table_x[KB_BASIC_FUNC_NUM] = {0, 6, 0, 1, 2, 3, 4, 5, 6, 0, 5, 0};
    char table_y[KB_BASIC_FUNC_NUM] = {2, 2, 3, 3, 3, 3, 3, 3, 3, 2, 3, 2};

    *cor_x = table_x[key_idx];
    *cor_y = table_y[key_idx];
}
static void get_number_location(uint8_t key_idx, uint8_t *cor_x, uint8_t *cor_y)
{
    //  0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    char table_x[10] = {3, 0, 1, 2, 0, 1, 2, 0, 1, 2};
    char table_y[10] = {2, 0, 0, 0, 1, 1, 1, 2, 2, 2};

    *cor_x = table_x[key_idx];
    *cor_y = table_y[key_idx];
}
static void get_symbol_location(uint8_t key_idx, uint8_t *cor_x, uint8_t *cor_y)
{
    //  ,, :, ?, ~,
    char table_x[15] = {0, 1, 2, 0, 1, 2, 0, 1, 2, 3, 4, 5, 3, 4, 5};
    char table_y[15] = {0, 0, 0, 1, 1, 1, 2, 2, 2, 0, 0, 0, 1, 1, 1};

    *cor_x = table_x[key_idx];
    *cor_y = table_y[key_idx];
}



static bool gui_kb_is_func(uint32_t key_val)
{
    uint8_t area = KB_VALUE_AREA(key_val);
    uint32_t idx = KB_VALUE_IDX(key_val);
    gui_kb_t *this = gui_get_kb();

    gui_ime_api_t *ime_api = NULL;

    if (area == KB_AREA_FUNC)
    {
        if (this->ime)
        {
            ime_api = (gui_ime_api_t *)(this->ime->api);
            if ((idx == KB_BTN_IDX_SP) && (this->mode == KB_MODE_BASIC_PY) &&
                (ime_api->get_cand_num(this->ime) != 0))
            {
                return true;
            }
        }
        if ((idx != KB_BTN_IDX_SP) && (idx != KB_BTN_IDX_COMIC) && (idx != KB_BTN_IDX_ENTER))
        {
            return true;
        }
    }
    return false;
}
static bool gui_kb_is_other(uint32_t key_val)
{
    uint8_t area = KB_VALUE_AREA(key_val);

    if (area == KB_AREA_OTHER)
    {
        return true;
    }
    return false;
}
static gui_obj_t *gui_kb_get_subobj(gui_obj_t *win, uint16_t obj_idx)
{
    gui_obj_t *obj = NULL;
    uint16_t idx = 0;
    gui_list_t *node = NULL;

    gui_list_for_each(node, &win->child_list)
    {
        gui_obj_t *obj_item = gui_list_entry(node, gui_obj_t, brother_list);
        // gui_log("%s type: 0x%x\n", obj_item->name, obj_item->type);
        if (idx == obj_idx)
        {
            obj = obj_item;
            break;
        }
        idx++;
    }
    return obj;
}

// ********************************** IME ****************************** //
static void gui_kb_refresh_cand(void)
{
    gui_kb_t *this = gui_get_kb();
    gui_obj_t *win = (gui_obj_t *)this->win_ime;
    gui_list_t *node = NULL;
    uint8_t idx_cnt = 0;
    gui_ime_api_t *ime_api = (gui_ime_api_t *)(this->ime->api);
    uint8_t char_len = ime_api->get_char_len_byte(this->ime);

    gui_list_for_each(node, &win->child_list)
    {
        gui_obj_t *obj_item = gui_list_entry(node, gui_obj_t, brother_list);
        // gui_log("%s type: 0x%x\n", obj_item->name, obj_item->type);
        if (TEXTBOX == obj_item->type && idx_cnt < ime_api->get_page_size(this->ime))
        {
            gui_text_t *text = (gui_text_t *)obj_item;
            gui_text_content_set(text, text->content, char_len);
        }
        idx_cnt ++;
    }
}
static void gui_kb_clear_cand(void)
{
    gui_kb_t *this = gui_get_kb();
    gui_obj_t *win = (gui_obj_t *)this->win_ime;
    gui_list_t *node = NULL;
    gui_ime_api_t *ime_api = NULL;

    if (!this->ime)
    {
        return ;
    }
    ime_api = (gui_ime_api_t *)(this->ime->api);
    buff_key_clean();
    ime_api->clear_cand(this->ime);
    gui_list_for_each(node, &win->child_list)
    {
        gui_obj_t *obj_item = gui_list_entry(node, gui_obj_t, brother_list);
        // gui_log("%s type: 0x%x\n", obj_item->name, obj_item->type);
        if (TEXTBOX == obj_item->type)
        {
            gui_text_t *text = (gui_text_t *)obj_item;
            gui_text_content_set(text, text->content, 0);
        }
    }
}
static void gui_kb_select_cand(size_t idx)
{
    gui_kb_t *this = gui_get_kb();
    gui_ime_api_t *ime_api = NULL;
    uint8_t *cand_buff = NULL;

    if (!this->ime)
    {
        return;
    }
    ime_api = (gui_ime_api_t *)(this->ime->api);
    cand_buff = ime_api->get_cand_buff(this->ime);

    if (idx >= ime_api->get_cand_num(this->ime))
    {
        return ;
    }

    uint8_t char_len = ime_api->get_char_len_byte(this->ime);
    char *ch = (char *)cand_buff + idx * char_len;
    for (uint8_t i = 0; i < char_len; i++)
    {
        gui_kb_cursor_insert(ch[i]);
    }

    gui_kb_clear_cand();
    ime_api->reset_page(this->ime);
}

// ********************************** KB input proc ****************************** //
static void gui_basic_func_proc(uint32_t key_val)
{
    gui_kb_t *this = gui_get_kb();
    uint32_t kb_idx = KB_VALUE_IDX(key_val);
    uint32_t idx = kb_idx;
    uint8_t mode = this->mode;

    // KB_SW_IDX_CAP = 0,

    //   KB_BTN_IDX_DEL,
    //   KB_SW_IDX_SYMBOL,
    //   KB_SW_IDX_NUMBER,
    //   KB_BTN_IDX_LS,    // Left shift
    //   KB_BTN_IDX_SP,    x
    //   KB_BTN_IDX_RS,    // Right shift
    //   KB_SW_IDX_CNEN,
    //   KB_BTN_IDX_DONE,

    //   KB_BTN_IDX_COMIC,    x
    //   KB_BTN_IDX_ENTER,    x

    //   KB_SW_IDX_LOCK,
    switch (idx)
    {
    case KB_SW_IDX_CAP:
        {
            this->caps_Lk = !this->caps_Lk;
            if (this->caps_Lk)
            {
                gui_keyboard_set_mode(this, KB_MODE_BASIC_ENG_UPPER);
            }
            else
            {
                gui_keyboard_set_mode(this, KB_MODE_BASIC_ENG_LOWWER);
            }
        }
        break;
    case KB_BTN_IDX_DEL:
        {
            if (this->mode == KB_MODE_BASIC_PY && len_key)
            {
                buff_key_pop();
            }
            else
            {
                gui_kb_cursor_del();
            }
        }
        break;
    case KB_SW_IDX_SYMBOL:
        {
            if ((mode != KB_MODE_BASIC_SYMBOL_EN) && (mode != KB_MODE_BASIC_SYMBOL_CN))
            {
                if (mode == KB_MODE_BASIC_PY)
                {
                    gui_kb_select_cand(0);
                    gui_keyboard_set_mode(this, KB_MODE_BASIC_SYMBOL_CN);
                }
                else
                {
                    // jump to symbol
                    this->symbol_Lk = false;
                    gui_keyboard_set_mode(this, KB_MODE_BASIC_SYMBOL_EN);
                }
            }
            else
            {
                // jump to abc
                gui_keyboard_set_mode(this, KB_MODE_BASIC_ENG_LOWWER);
            }
        }
        break;
    case KB_SW_IDX_NUMBER:
        {
            if ((mode != KB_MODE_BASIC_NUM))
            {
                if (mode == KB_MODE_BASIC_PY)
                {
                    gui_kb_select_cand(0);
                }

                gui_keyboard_set_mode(this, KB_MODE_BASIC_NUM);
            }
            else
            {
                if (this->last_mode == KB_MODE_BASIC_NUM)
                {
                    this->last_mode = KB_MODE_BASIC_ENG_LOWWER;
                }
                gui_keyboard_set_mode(this, this->last_mode);
            }
        }
        break;
    case KB_BTN_IDX_LS:
        {
            gui_kb_cursor_ls();
        }
        break;
    case KB_BTN_IDX_SP:
        {
            if (mode == KB_MODE_BASIC_PY)
            {
                gui_kb_select_cand(0);
            }
        }
        break;
    case KB_BTN_IDX_RS:
        {
            gui_kb_cursor_rs();
        }
        break;
    case KB_SW_IDX_CNEN:
        {
            if ((mode == KB_MODE_BASIC_SYMBOL_EN))
            {
                // symbol
                gui_keyboard_set_mode(this, KB_MODE_BASIC_SYMBOL_CN);
            }
            else if ((mode == KB_MODE_BASIC_SYMBOL_CN))
            {
                // symbol
                gui_keyboard_set_mode(this, KB_MODE_BASIC_SYMBOL_EN);
            }
            else if ((mode == KB_MODE_BASIC_PY))
            {
                // pinyin to abc
                gui_kb_clear_cand();
                gui_keyboard_set_mode(this, KB_MODE_BASIC_ENG_LOWWER);
            }
            else if ((mode == KB_MODE_BASIC_ENG_LOWWER) || (mode == KB_MODE_BASIC_ENG_UPPER))
            {
                // abc to pinyin
                gui_keyboard_set_mode(this, KB_MODE_BASIC_PY);
            }
        }
        break;
    case KB_BTN_IDX_DONE:
        {
            gui_text_t *txt = this->txt_input;
            uint32_t size = len_display;
            uint8_t *str = gui_malloc(size + 1);
            if (str)
            {
                if (this->ispasswd)
                {
                    uint16_t len = (size < (KB_PSWD_KEY_MAXLEN - 1)) ? size : (KB_PSWD_KEY_MAXLEN - 1);
                    this->ispasswd = false;
                    memcpy(buff_pswd, buff_display, len);
                    memset(str, '*', len);
                    buff_pswd[len] = '\0';
                    str[len] = '\0';
                    gui_text_content_set(txt, (void *)str, len);

                    gui_obj_event_set((gui_obj_t *)txt, (gui_event_t)TXT_EVENT_PSWD_DONE);
                }
                else
                {
                    memcpy(str, buff_display, size);
                    str[size] = '\0';
                    gui_text_content_set(txt, (void *)str, size);
                }
            }
            gui_obj_show(this, false);

        }
        break;
    case KB_SW_IDX_LOCK:
        {
            this->symbol_Lk = !this->symbol_Lk;
        }
        break;
    }


}


static void gui_basic_other_proc(uint32_t key_val)
{
    gui_kb_t *this = gui_get_kb();
    uint32_t kb_idx = KB_VALUE_IDX(key_val);
    uint32_t idx = kb_idx;
    uint8_t mode = this->mode;
    gui_ime_api_t *ime_api = (gui_ime_api_t *)(this->ime->api);

    if (mode == KB_MODE_BASIC_PY)
    {
        switch (idx)
        {
        case KB_IMG_IDX_CAND_LEFT:
            {
                ime_api->page_flip(this->ime, IME_PAGE_BACKWARD);
            }
            break;
        case KB_IMG_IDX_CAND_RIGHT:
            {
                ime_api->page_flip(this->ime, IME_PAGE_FORWARD);
            }
            break;
        }
    }

}

static uint8_t gui_kb_basic_symbol_cn[] =
{
    0xef, 0xbc, 0x8c, 0xe3, 0x80, 0x82, 0xef, 0xbc, 0x9f, 0xef, 0xbc, 0x81, 0xef, 0xbd, 0x9e, 0xef, 0xbc, 0x9a, 0xe2, 0x80, 0xa6, 0xe2, 0x80, 0xa6,
    0xef, 0xbf, 0xa5, 0xe2, 0x80, 0xa6, 0xef, 0xbc, 0x9b, 0xe3, 0x80, 0x81, 0xe2, 0x80, 0xa6, 0xe2, 0x80, 0x9c, 0xe2, 0x80, 0x9d, 0xe3, 0x80, 0x8a,
    0xe3, 0x80, 0x8b, 0xe3, 0x80, 0x90, 0xe3, 0x80, 0x91, 0xef, 0xbd, 0x9b, 0xef, 0xbd, 0x9d, 0xef, 0xbc, 0x9c, 0xef, 0xbc, 0x9e, 0xef, 0xbc, 0x88,
    0xef, 0xbc, 0x89, 0xe2, 0x80, 0xa6
};
static char *gui_kb_basic_symbol[] = {",.-!~:%#+&/@*\"|\\_[]{}<>()?",
                                      (char *)gui_kb_basic_symbol_cn,
                                      ",:?~@.(!)-=+%#*"
                                     };

static uint32_t gui_get_basic_char(uint32_t key_val)
{
    gui_kb_t *this = gui_get_kb();
    uint8_t  kb_area = KB_VALUE_AREA(key_val);
    uint32_t kb_idx = KB_VALUE_IDX(key_val);
    uint32_t idx = kb2letter_index(kb_idx);
    uint32_t ch = 0;
    uint8_t mode = this->mode;
    gui_ime_api_t *ime_api = NULL;
    uint8_t char_len = 1;

    if (this->ime)
    {
        ime_api = (gui_ime_api_t *)(this->ime->api);
        char_len = ime_api->get_char_len_byte(this->ime);
    }

    if (kb_area == KB_AREA_FUNC)
    {
        if (kb_idx == KB_BTN_IDX_SP)
        {
            ch = ' ';
        }
        else if (kb_idx == KB_BTN_IDX_COMIC)
        {
            if (this->ime)
            {
                char *charset = gui_kb_basic_symbol[1];

                gui_kb_select_cand(0);
                memcpy(&ch, &charset[0 * char_len], char_len);
            }
            else
            {
                ch = ',';
            }
        }
        else if (kb_idx == KB_BTN_IDX_ENTER)
        {
            // ch = '\n';
            ch = ' ';
        }
    }
    else
    {
        switch (mode)
        {
        case KB_MODE_BASIC_ENG_LOWWER:
        case KB_MODE_BASIC_PY:
            {
                ch = 'a' + idx;
            }
            break;
        case KB_MODE_BASIC_ENG_UPPER:
            {
                ch = 'A' + idx;
            }
            break;
        case KB_MODE_BASIC_SYMBOL_EN:
            {
                ch = gui_kb_basic_symbol[0][kb_idx];
            }
            break;
        case KB_MODE_BASIC_SYMBOL_CN:
            {
                char *charset = gui_kb_basic_symbol[0];
                if (this->ime)
                {
                    charset = gui_kb_basic_symbol[1];
                }
                memcpy(&ch, &charset[kb_idx * char_len], char_len);
            }
            break;
        case KB_MODE_BASIC_NUM:
            {
                if (kb_area == KB_AREA_NUM)
                {
                    idx = kb2number_index(kb_idx);
                    ch = '0' + idx;
                }
                else
                {
                    ch = gui_kb_basic_symbol[2][kb_idx];
                }
            }
            break;
        }
    }

    return ch;
}

static void gui_kb_input_func_proc(uint32_t key_val)
{
    gui_kb_t *this = gui_get_kb();

    if (this->layout == KB_LAYOUT_BASIC)
    {
        gui_basic_func_proc(key_val);
    }

}
static void gui_kb_input_other_proc(uint32_t key_val)
{
    gui_kb_t *this = gui_get_kb();

    if (this->layout == KB_LAYOUT_BASIC)
    {
        gui_basic_other_proc(key_val);
    }

}
static void gui_kb_input_char_proc(uint32_t key_val)
{
    gui_kb_t *this = gui_get_kb();
    uint8_t  kb_area = KB_VALUE_AREA(key_val);
    uint32_t kb_idx = KB_VALUE_IDX(key_val);
    uint32_t ch = 0;

    if (this->layout == KB_LAYOUT_BASIC)
    {
        ch = gui_get_basic_char(key_val);
    }

    if (kb_area == KB_AREA_LETTER && (this->mode == KB_MODE_BASIC_PY))
    {
        if (this->input_Method != KB_METHOD_NULL)
        {
            buff_key_push(ch);
        }
        else
        {
            gui_kb_cursor_insert(ch);
        }
    }
    else
    {
        if (this->mode == KB_MODE_BASIC_SYMBOL_CN || ((kb_area == KB_AREA_FUNC) &&
                                                      (kb_idx == KB_BTN_IDX_COMIC)))
        {
            uint8_t *byte = (uint8_t *)&ch;
            gui_ime_api_t *ime_api = NULL;
            uint8_t char_len = 1;

            if (this->ime)
            {
                ime_api = (gui_ime_api_t *)(this->ime->api);
                char_len = ime_api->get_char_len_byte(this->ime);
            }

            for (uint8_t i = 0; i < char_len; i++)
            {
                gui_kb_cursor_insert(byte[i]);
            }
        }
        else
        {
            gui_kb_cursor_insert(ch);
        }
    }

    if (this->mode == KB_MODE_BASIC_SYMBOL_EN || this->mode == KB_MODE_BASIC_SYMBOL_CN)
    {
        if (!this->symbol_Lk)
        {
            gui_keyboard_set_mode(this, this->last_mode);
        }
    }
}


static void gui_kb_cursor_animate(void *p)
{
    gui_kb_t *this = gui_get_kb();
    char *cur = gui_kb_get_cursor();
    uint16_t len_bf = cur - buff_display;
    char *pcontent = (char *)(this->txt_display->content);
    static uint8_t i = 0;

    i++;
    // printf(">%c\n", *(char*)cur);
    // printf(">%d %s\n", len_display,(char*)buff_display);

    memset(pcontent, 0, KB_BUFF_DISPLAY_MAXLEN);
    if (len_bf)
    {
        memcpy(pcontent, buff_display, len_bf);
    }
    pcontent[len_bf] = (i & 0x01) ? '_' : ' ';
    memcpy(pcontent + len_bf + 1, cur, len_display - len_bf);

    gui_text_content_set(this->txt_display, pcontent, len_display + 1);
}

static void gui_kb_update_key(void)
{
    gui_kb_t *this = gui_get_kb();
    gui_ime_api_t *ime_api = (gui_ime_api_t *)(this->ime->api);
    uint8_t page_sz = ime_api->get_page_size(this->ime);
    gui_text_t *txt = (gui_text_t *)gui_kb_get_subobj((gui_obj_t *)(this->win_ime), page_sz);

    gui_text_content_set(txt, txt->content, len_key);
}
static void gui_kb_update_cand(void)
{
    gui_kb_t *this = gui_get_kb();
    gui_ime_api_t *ime_api = (gui_ime_api_t *)(this->ime->api);
    if (buff_key_isempty())
    {
        gui_kb_clear_cand();
    }
    ime_api->update_cand(this->ime, buff_key, len_key);
    gui_kb_refresh_cand();
}
static void gui_kb_update_display(void)
{
    gui_kb_cursor_animate(NULL);
}
static void gui_kb_input_proc(void *obj, gui_event_t event_code, void *param)
{
    // gui_obj_t *object = (void *)obj;
    // T_OBJ_TYPE type = object->type;
    size_t key_val = (size_t)param;
    gui_kb_t *kb = gui_get_kb();

    // gui_log("0x%x type: 0x%x, key: 0x%x %d %d\n", obj, type, key_val, KB_VALUE_AREA(key_val),
    // KB_VALUE_IDX(key_val));
    // gui_log(">%s\n", buff_input);

    if (gui_kb_is_func(key_val))
    {
        gui_kb_input_func_proc(key_val);
    }
    else if (gui_kb_is_other(key_val))
    {
        gui_kb_input_other_proc(key_val);
    }
    else
    {
        gui_kb_input_char_proc(key_val);
    }


    if (kb->input_Method != KB_METHOD_NULL && kb->mode == KB_MODE_BASIC_PY)
    {
        gui_kb_update_key();
        gui_kb_update_cand();
    }

    // update display
    // printf(">%s< %d, cur: %d-%c-\n", buff_display, len_display, cursor-buff_display, *cursor);
    gui_kb_update_display();
}


// ********************************** KB layout ****************************** //

static void gui_kb_set_func_mode(gui_kb_t *this, uint8_t mode)
{
    gui_obj_t *win = (gui_obj_t *)this->win_func;
    gui_obj_t *obj_func[KB_BASIC_FUNC_NUM];
    // KB_MODE_BASIC_ENG_LOWWER,
    // KB_MODE_BASIC_ENG_UPPER,
    // KB_MODE_BASIC_PY,
    // KB_MODE_BASIC_NUM,
    // KB_MODE_BASIC_SYMBOL_EN,
    // KB_MODE_BASIC_SYMBOL_CN,
    gui_list_t *node = NULL;
    uint16_t idx = KB_SW_IDX_CAP;
    gui_list_for_each(node, &win->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        // gui_log("%s type: 0x%x\n", obj->name, obj->type);
        obj_func[idx] = obj;
        idx++;
    }

    switch (mode)
    {
    case KB_MODE_BASIC_ENG_LOWWER:
        {
            gui_obj_show(obj_func[KB_BTN_IDX_COMIC], false);
            gui_obj_show(obj_func[KB_SW_IDX_LOCK], false);
            gui_obj_show(obj_func[KB_BTN_IDX_ENTER], false);

            gui_switch_change_state((gui_switch_t *)obj_func[KB_SW_IDX_SYMBOL], false);  // symbol
            gui_switch_change_state((gui_switch_t *)obj_func[KB_SW_IDX_NUMBER], false);  // number
            gui_obj_show(obj_func[KB_SW_IDX_CAP], true);
            gui_obj_show(obj_func[KB_SW_IDX_CNEN], true);
        }
        break;
    case KB_MODE_BASIC_ENG_UPPER:
        {
            gui_obj_show(obj_func[KB_BTN_IDX_COMIC], false);
            gui_obj_show(obj_func[KB_SW_IDX_LOCK], false);
            gui_obj_show(obj_func[KB_BTN_IDX_ENTER], false);

            gui_switch_change_state((gui_switch_t *)obj_func[KB_SW_IDX_SYMBOL], false);  // symbol
            gui_switch_change_state((gui_switch_t *)obj_func[KB_SW_IDX_NUMBER], false);  // number
            gui_obj_show(obj_func[KB_SW_IDX_CAP], true);
            gui_obj_show(obj_func[KB_SW_IDX_CNEN], true);
        }
        break;
    case KB_MODE_BASIC_PY:
        {
            gui_obj_show(obj_func[KB_SW_IDX_CAP], false);
            gui_obj_show(obj_func[KB_SW_IDX_LOCK], false);
            gui_obj_show(obj_func[KB_BTN_IDX_ENTER], false);

            gui_switch_change_state((gui_switch_t *)obj_func[KB_SW_IDX_SYMBOL], false);  // symbol
            gui_switch_change_state((gui_switch_t *)obj_func[KB_SW_IDX_NUMBER], false);  // number
            gui_obj_show(obj_func[KB_BTN_IDX_COMIC], true);
            gui_obj_show(obj_func[KB_SW_IDX_CNEN], true);
        }
        break;
    case KB_MODE_BASIC_NUM:
        {
            gui_obj_show(obj_func[KB_BTN_IDX_COMIC], false);
            gui_obj_show(obj_func[KB_SW_IDX_CNEN], false);
            gui_obj_show(obj_func[KB_SW_IDX_CAP], false);
            gui_obj_show(obj_func[KB_SW_IDX_LOCK], false);

            gui_switch_change_state((gui_switch_t *)obj_func[KB_SW_IDX_SYMBOL], false);  // symbol
            gui_switch_change_state((gui_switch_t *)obj_func[KB_SW_IDX_NUMBER], true);  // back
            gui_obj_show(obj_func[KB_BTN_IDX_ENTER], true);
        }
        break;
    case KB_MODE_BASIC_SYMBOL_EN:
        {
            gui_obj_show(obj_func[KB_BTN_IDX_COMIC], false);
            gui_obj_show(obj_func[KB_SW_IDX_CAP], false);
            gui_obj_show(obj_func[KB_BTN_IDX_ENTER], false);

            gui_switch_change_state((gui_switch_t *)obj_func[KB_SW_IDX_SYMBOL], true);  // abc
            gui_switch_change_state((gui_switch_t *)obj_func[KB_SW_IDX_NUMBER], false);  // number
            gui_obj_show(obj_func[KB_SW_IDX_LOCK], true);
            gui_obj_show(obj_func[KB_SW_IDX_CNEN], true);
        }
        break;
    case KB_MODE_BASIC_SYMBOL_CN:
        {
            gui_obj_show(obj_func[KB_BTN_IDX_COMIC], false);
            gui_obj_show(obj_func[KB_SW_IDX_CAP], false);
            gui_obj_show(obj_func[KB_BTN_IDX_ENTER], false);

            gui_switch_change_state((gui_switch_t *)obj_func[KB_SW_IDX_SYMBOL], true);  // abc
            gui_switch_change_state((gui_switch_t *)obj_func[KB_SW_IDX_NUMBER], false);  // number
            gui_obj_show(obj_func[KB_SW_IDX_LOCK], true);
            gui_obj_show(obj_func[KB_SW_IDX_CNEN], true);
        }
        break;
    }
    // 5 sw
    gui_switch_change_state((gui_switch_t *)obj_func[KB_SW_IDX_CAP], this->caps_Lk);
    gui_switch_change_state((gui_switch_t *)obj_func[KB_SW_IDX_LOCK], this->symbol_Lk);
    gui_switch_change_state((gui_switch_t *)obj_func[KB_SW_IDX_CNEN], this->is_cn);

    gui_obj_show(obj_func[KB_BTN_IDX_DEL], true);
    gui_obj_show(obj_func[KB_SW_IDX_SYMBOL], true);
    gui_obj_show(obj_func[KB_SW_IDX_NUMBER], true);
    gui_obj_show(obj_func[KB_BTN_IDX_LS], true);
    gui_obj_show(obj_func[KB_BTN_IDX_SP], true);
    gui_obj_show(obj_func[KB_BTN_IDX_RS], true);
    gui_obj_show(obj_func[KB_BTN_IDX_DONE], true);
}


static void gui_kb_set_letter_mode(gui_kb_t *this, uint8_t mode)
{
    gui_obj_t *win = (gui_obj_t *)this->win_letter;
    void *img_default = NULL, *img_clk = NULL;
    gui_obj_t *obj_list[26];
    uint32_t key_val = 0;
    uint16_t i = 0;

    gui_list_t *node = NULL;
    uint16_t idx = 0;
    gui_list_for_each(node, &win->child_list)
    {
        gui_obj_t *obj = gui_list_entry(node, gui_obj_t, brother_list);
        // gui_log("%s type: 0x%x\n", obj->name, obj->type);
        obj_list[idx] = obj;
        idx++;
    }

    switch (mode)
    {
    case KB_MODE_BASIC_ENG_LOWWER:
    case KB_MODE_BASIC_PY:
        {
            for (i = 0; i < 26; i++)
            {
                key_val = KB_VALUE_PACK(KB_AREA_LETTER, i);
                img_default = get_key_img(this, key_val, KB_SRC_LOWWER);
                img_clk = get_key_img(this, key_val, KB_SRC_LOWWER_CLK);
                gui_button_set_img((gui_button_t *)obj_list[i], img_default, img_clk);
            }
        }
        break;
    case KB_MODE_BASIC_ENG_UPPER:
        {
            for (i = 0; i < 26; i++)
            {
                key_val = KB_VALUE_PACK(KB_AREA_LETTER, i);
                img_default = get_key_img(this, key_val, KB_SRC_UPPER);
                img_clk = get_key_img(this, key_val, KB_SRC_UPPER_CLK);
                gui_button_set_img((gui_button_t *)obj_list[i], img_default, img_clk);
            }
        }
        break;
    case KB_MODE_BASIC_SYMBOL_EN:
        {
            for (i = 0; i < 26; i++)
            {
                key_val = KB_VALUE_PACK(KB_AREA_SYMBOL, 15 + i);
                img_default = get_key_img(this, key_val, KB_SRC_DEFAULT);
                img_clk = get_key_img(this, key_val, KB_SRC_CLK);
                gui_button_set_img((gui_button_t *)obj_list[i], img_default, img_clk);
            }
        }
        break;
    case KB_MODE_BASIC_SYMBOL_CN:
        {
            for (i = 0; i < 26; i++)
            {
                key_val = KB_VALUE_PACK(KB_AREA_SYMBOL, (15 + 26) + i);
                img_default = get_key_img(this, key_val, KB_SRC_DEFAULT);
                img_clk = get_key_img(this, key_val, KB_SRC_CLK);
                gui_button_set_img((gui_button_t *)obj_list[i], img_default, img_clk);
            }
        }
        break;
    }
}


static void gui_kb_mode_switch(gui_kb_t *this, uint8_t mode)
{
    if (((mode == KB_MODE_BASIC_SYMBOL_CN) && (this->mode == KB_MODE_BASIC_SYMBOL_EN)) \
        || ((mode == KB_MODE_BASIC_SYMBOL_EN) && (this->mode == KB_MODE_BASIC_SYMBOL_CN))
        || ((mode == KB_MODE_BASIC_NUM) && (this->mode == KB_MODE_BASIC_SYMBOL_EN))
        || ((mode == KB_MODE_BASIC_NUM) && (this->mode == KB_MODE_BASIC_SYMBOL_CN)))
    {

    }
    else
    {
        this->last_mode = this->mode;
    }
    this->mode = mode;
    this->caps_Lk = false;
    this->is_cn = false;
    // gui_log("last mode: %d\n", this->last_mode);

    if ((mode == KB_MODE_BASIC_SYMBOL_CN) || (mode == KB_MODE_BASIC_PY))
    {
        this->is_cn = true;
    }
    else if ((mode == KB_MODE_BASIC_ENG_UPPER))
    {
        this->caps_Lk = true;
    }


    gui_obj_show(this->win_ime, false);

    switch (mode)
    {
    case KB_MODE_BASIC_PY:
        {
            gui_obj_show(this->win_ime, true);
        }
    case KB_MODE_BASIC_ENG_LOWWER:
    case KB_MODE_BASIC_ENG_UPPER:
    case KB_MODE_BASIC_SYMBOL_EN:
    case KB_MODE_BASIC_SYMBOL_CN:
        {
            gui_kb_set_letter_mode(this, mode);
            gui_obj_show(this->win_num, false);
            gui_obj_show(this->win_letter, true);
        }
        break;
    case KB_MODE_BASIC_NUM:
        {
            gui_obj_show(this->win_letter, false);
            gui_obj_show(this->win_num, true);
        }
        break;

    }

    gui_kb_set_func_mode(this, mode);
    gui_obj_show(this->win_func, true);

}
static void gui_kb_set_obj_scale(int16_t *obj_x, int16_t *obj_y, int16_t *obj_w, int16_t *obj_h)
{
    gui_kb_t *this = gui_get_kb();
    uint16_t h = gui_get_screen_height();
    float scale = this->scale;

    *obj_x = *obj_x * scale;
    *obj_y = h - (360 - *obj_y) * scale;
    *obj_w = *obj_w * scale;
    *obj_h = *obj_h * scale;
}
static void gui_kb_layout_basic_letter(gui_kb_t  *this)
{
    int16_t x = this->base.x, y = this->base.y, w = this->base.w, h = this->base.h;
    uint16_t i = 0;
    gui_button_t *btn = NULL;


    // location calculate
    // 3 row
    int16_t y_start = 120;
    float y_step = 59;
    float x_letter_step = 61;
    float x_letter_start[3] = {19, 50, 111};
    float scale = this->scale;

    this->win_letter = gui_win_create(this, "WIN_letter",
                                      x, y, w, h);

    // create 26 letter like layout button
    for (i = 0; i < 26; i++)
    {
        int16_t obj_x = 0, obj_y = 0;
        int16_t obj_w = 0, obj_h = 0;
        void *img_default = NULL, *img_clk = NULL;
        // 1: 10, 2: 9, 3: 7
        uint8_t cor_x = 0, cor_y = 0;
        uint8_t letter_idx = kb2letter_index(i);
        size_t key_val = KB_VALUE_PACK(KB_AREA_LETTER, i);

        get_letter_location(letter_idx, &cor_x, &cor_y);
        obj_x = x_letter_start[cor_y] + x_letter_step * cor_x;
        obj_y = y_start + cor_y * y_step;
        img_default = get_key_img(this, key_val, KB_SRC_LOWWER);
        img_clk = get_key_img(this, key_val, KB_SRC_LOWWER_CLK);
        obj_w = ((gui_rgb_data_head_t *)img_default)->w;
        obj_h = ((gui_rgb_data_head_t *)img_default)->h;
        gui_kb_set_obj_scale(&obj_x, &obj_y, &obj_w, &obj_h);


        btn = gui_button_create(this->win_letter, obj_x, obj_y, obj_w, obj_h, img_default, img_clk, "kb_l",
                                BUTTON_BG_ICON, 0);
        gui_img_scale(btn->img, scale, scale);
        gui_button_click(btn, (gui_event_cb_t)gui_kb_input_proc, (void *)key_val);

    }
}

static void gui_kb_layout_basic_func(gui_kb_t  *this)
{
    int16_t x = this->base.x, y = this->base.y, w = this->base.w, h = this->base.h;
    uint16_t i = 0;
    gui_button_t *btn = NULL;
    gui_switch_t *sw = NULL;

    // location calculate
    // 2 row
    int16_t y_start = 120;
    float y_step = 59;
    float x_pos[] = {19, 111, 203, 264, 384, 445, 537};
    float scale = this->scale;

    this->win_func = gui_win_create(this, "WIN_func",
                                    x, y, w, h);

    // create 11 func like layout obj: 7 button, 4 switch KB_BASIC_FUNC_NUM
    for (i = 0; i < KB_BASIC_FUNC_NUM; i++)
    {
        int16_t obj_x = 0, obj_y = 0;
        int16_t obj_w = 0, obj_h = 0;
        size_t key_val = KB_VALUE_PACK(KB_AREA_FUNC, i);
        void *img_default = NULL, *img_clk = NULL;
        // 1: 10, 2: 9, 3: 7
        uint8_t cor_x = 0, cor_y = 0;

        get_func_location(i, &cor_x, &cor_y);
        obj_x = x_pos[cor_x] ;
        obj_y = y_start + cor_y * y_step;

        img_default = get_key_img(this, key_val, KB_SRC_DEFAULT);
        img_clk = get_key_img(this, key_val, KB_SRC_CLK);
        obj_w = ((gui_rgb_data_head_t *)img_default)->w;
        obj_h = ((gui_rgb_data_head_t *)img_default)->h;
        gui_kb_set_obj_scale(&obj_x, &obj_y, &obj_w, &obj_h);

        if (key_is_sw(key_val) == false)
        {
            btn = gui_button_create(this->win_func, obj_x, obj_y, obj_w, obj_h, img_default, img_clk, "kb_f",
                                    BUTTON_BG_ICON, 0);
            gui_img_scale(btn->img, scale, scale);
            gui_button_click(btn, (gui_event_cb_t)gui_kb_input_proc, (void *)key_val);
        }
        else
        {
            sw = gui_switch_create(this->win_func, obj_x, obj_y, obj_w, obj_h, img_default, img_clk);
            gui_img_scale(sw->switch_picture, scale, scale);
            GUI_API(gui_switch_t).on_turn_on(sw, (gui_event_cb_t)gui_kb_input_proc, (void *)key_val);
            GUI_API(gui_switch_t).on_turn_off(sw, (gui_event_cb_t)gui_kb_input_proc, (void *)key_val);
        }
    }
}


static void gui_kb_layout_basic_number(gui_kb_t  *this)
{
    int16_t x = this->base.x, y = this->base.y, w = this->base.w, h = this->base.h;
    uint16_t i = 0;
    gui_button_t *btn = NULL;

    // location calculate
    // 3 row
    int16_t y_start = 120;
    float scale = this->scale;

    this->win_num = gui_win_create(this, "WIN_num",
                                   x, y, w, h);
    // TODO: grid img
    {
        float y_step = 59;
        float x_pos[] = {203, 284, 365, 445};
        // create 10 number like layout button
        for (i = 0; i < 10; i++)
        {
            int16_t obj_x = 0, obj_y = 0;
            int16_t obj_w = 0, obj_h = 0;
            void *img_default = NULL, *img_clk = NULL;
            // 1: 3, 2: 3, 3: 4
            uint8_t cor_x = 0, cor_y = 0;
            uint8_t number_idx = kb2number_index(i);
            size_t key_val = KB_VALUE_PACK(KB_AREA_NUM, i);

            get_number_location(number_idx, &cor_x, &cor_y);
            obj_x = x_pos[cor_x];
            obj_y = y_start + cor_y * y_step;
            img_default = get_key_img(this, key_val, KB_SRC_DEFAULT);
            img_clk = get_key_img(this, key_val, KB_SRC_CLK);
            obj_w = ((gui_rgb_data_head_t *)img_default)->w;
            obj_h = ((gui_rgb_data_head_t *)img_default)->h;
            gui_kb_set_obj_scale(&obj_x, &obj_y, &obj_w, &obj_h);

            btn = gui_button_create(this->win_num, obj_x, obj_y, obj_w, obj_h, img_default, img_clk, "kb_n",
                                    BUTTON_BG_ICON, 0);
            gui_img_scale(btn->img, scale, scale);
            gui_button_click(btn, (gui_event_cb_t)gui_kb_input_proc, (void *)key_val);

        }
    }

    {
        float y_step = 56;
        float x_pos[] = {19, 77, 135, 445, 503, 561};
        // create 15 number symbol layout button
        for (i = 0; i < 15; i++)
        {
            int16_t obj_x = 0, obj_y = 0;
            int16_t obj_w = 0, obj_h = 0;
            void *img_default = NULL, *img_clk = NULL;
            // 1: 3, 2: 3, 3: 4
            uint8_t cor_x = 0, cor_y = 0;
            // uint8_t symbol_idx = kb2number_index(i);
            size_t key_val = KB_VALUE_PACK(KB_AREA_SYMBOL, i);

            get_symbol_location(i, &cor_x, &cor_y);
            obj_x = x_pos[cor_x];
            obj_y = y_start + cor_y * y_step;
            img_default = get_key_img(this, key_val, KB_SRC_DEFAULT);
            img_clk = get_key_img(this, key_val, KB_SRC_CLK);
            obj_w = ((gui_rgb_data_head_t *)img_default)->w;
            obj_h = ((gui_rgb_data_head_t *)img_default)->h;
            gui_kb_set_obj_scale(&obj_x, &obj_y, &obj_w, &obj_h);

            btn = gui_button_create(this->win_num, obj_x, obj_y, obj_w, obj_h, img_default, img_clk, "kb_s",
                                    BUTTON_BG_ICON, 0);
            gui_img_scale(btn->img, scale, scale);
            gui_button_click(btn, (gui_event_cb_t)gui_kb_input_proc, (void *)key_val);

        }
    }

}

static void gui_kb_ime_cand_click(void *obj, gui_event_t e, void *param)
{
    // gui_log("click cand %d\n", param);
    gui_kb_select_cand((size_t)param);
    gui_kb_update_display();
}

static void gui_kb_layout_basic_ime(gui_kb_t  *this)
{
    int16_t x = this->base.x, y = this->base.y, w = this->base.w, h = this->base.h;
    gui_text_t *txt = NULL;
    gui_ime_api_t *ime_api = (gui_ime_api_t *)(this->ime->api);
    uint8_t page_sz = ime_api->get_page_size(this->ime);

    // location calculate
    uint8_t x_step = 57;   // fixed
    float x_pos_cand_start = 34;
    float scale = this->scale;
    int16_t obj_x = 0, obj_y = 0;
    int16_t obj_w = 0, obj_h = 0;
    uint16_t screen_h = gui_get_screen_height();

    int font_size_cn = 26;
    void *addr_font_cn =
        gui_get_file_address("app/system/resource/font/simsun_size26_bits1_font.bin");

    int font_size_key = 26;
    void *addr_font_key = gui_get_file_address("app/system/resource/font/simsun_size26_bits1_font.bin");
    char *text = (char *)ime_api->get_cand_buff(this->ime);
    uint8_t char_len = ime_api->get_char_len_byte(this->ime);

    this->win_ime = gui_win_create(this, "WIN_ime",
                                   x, y, w, h);

    // text: candi cn
    obj_w = x_step; obj_h = font_size_cn;
    int16_t x_pos_cand_start_scale = x_pos_cand_start * scale;
    obj_y = 92;
    obj_y = h - (360 - (obj_y + obj_h)) * scale - obj_h;
    for (size_t i = 0; i < page_sz; i++)
    {
        obj_x = x_pos_cand_start_scale + i * x_step;

        txt = gui_text_create(this->win_ime, "icon_text", obj_x, obj_y,
                              obj_w, obj_h);
        gui_text_mode_set(txt, CENTER);
        // A Chinese character for each
        gui_text_set(txt, text + i * char_len, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX, UINT8_MAX, UINT8_MAX),
                     char_len, font_size_cn);
        gui_font_mem_init(addr_font_cn);
        gui_text_type_set(txt, addr_font_cn, FONT_SRC_MEMADDR);

        gui_text_click(txt, gui_kb_ime_cand_click, (void *)i);
    }

    // text: pinyin
    obj_x = 34; obj_y = 64;
    obj_w = 300; obj_h = font_size_key;
    obj_x = obj_x * scale;
    obj_y = h - (360 - (obj_y + obj_h)) * scale - obj_h;

    memset(buff_key, 0, sizeof(buff_key));
    // len_key = strlen("yong");
    // memcpy(buff_key, "yong", len_key);

    txt = gui_text_create(this->win_ime, "txt_key", obj_x, obj_y,
                          obj_w, obj_h);
    gui_text_mode_set(txt, LEFT);
    gui_text_set(txt, buff_key, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX, UINT8_MAX, UINT8_MAX),
                 len_key, font_size_key);
    gui_font_mem_init(addr_font_key);
    gui_text_type_set(txt, addr_font_key, FONT_SRC_MEMADDR);



    // button: left and right
    {
        gui_button_t *btn = NULL;
        void *img_default = NULL, *img_clk = NULL;
        size_t key_val = KB_VALUE_PACK(KB_AREA_OTHER, KB_IMG_IDX_CAND_LEFT);

        obj_x = 19;
        obj_y = 98;
        img_default = get_other_img(this, KB_IMG_IDX_CAND_LEFT);
        img_clk = img_default;
        obj_w = ((gui_rgb_data_head_t *)img_default)->w;
        obj_h = ((gui_rgb_data_head_t *)img_default)->h;
        gui_kb_set_obj_scale(&obj_x, &obj_y, &obj_w, &obj_h);

        btn = gui_button_create(this->win_ime, obj_x, obj_y, obj_w, obj_h, img_default, img_clk, "",
                                BUTTON_BG_ICON, 0);
        gui_img_scale(btn->img, scale, scale);

        gui_button_click(btn, (gui_event_cb_t)gui_kb_input_proc, (void *)key_val);

        //
        obj_x = 610;
        obj_y = 98;
        key_val = KB_VALUE_PACK(KB_AREA_OTHER, KB_IMG_IDX_CAND_RIGHT);
        img_default = get_other_img(this, KB_IMG_IDX_CAND_RIGHT);
        img_clk = img_default;
        obj_w = ((gui_rgb_data_head_t *)img_default)->w;
        obj_h = ((gui_rgb_data_head_t *)img_default)->h;
        gui_kb_set_obj_scale(&obj_x, &obj_y, &obj_w, &obj_h);

        btn = gui_button_create(this->win_ime, obj_x, obj_y, obj_w, obj_h, img_default, img_clk, "",
                                BUTTON_BG_ICON, 0);
        gui_img_scale(btn->img, scale, scale);

        gui_button_click(btn, (gui_event_cb_t)gui_kb_input_proc, (void *)key_val);
    }

}
static void gui_kb_layout_basic_ctor(gui_kb_t  *this, gui_kb_config_t  *config)
{
    // key
    gui_kb_layout_basic_letter(this);
    gui_kb_layout_basic_func(this);
    gui_kb_layout_basic_number(this);
    if (this->input_Method != KB_METHOD_NULL)
    {
        gui_kb_layout_basic_ime(this);
    }

    // default
    gui_kb_mode_switch(this, KB_MODE_BASIC_ENG_LOWWER);
}

static void gui_kb_ctor(gui_kb_t                        *this,
                        gui_obj_t                 *parent,
                        const char                *name,
                        gui_kb_config_t            *config,
                        int16_t                    x,
                        int16_t                    y,
                        int16_t                    w,
                        int16_t                    h)
{
    gui_obj_t *root = (gui_obj_t *)this;
    void *img_data = NULL;

    gui_obj_ctor(root, parent, name, x, y, w, h);

    root->type = KEYBOARD;
    root->obj_cb = gui_kb_cb;
    root->has_input_prepare_cb = true;
    root->has_prepare_cb = true;
    root->has_draw_cb = true;
    root->has_end_cb = true;
    root->has_destroy_cb = true;

    gui_list_init(&(GET_BASE(this)->child_list));
    if ((GET_BASE(this)->parent) != NULL)
    {
        gui_list_insert_before(&((GET_BASE(this)->parent)->child_list),
                               &(GET_BASE(this)->brother_list));
    }

    // keyboard
    global_kb = this;
    this->layout = config->layout;
    this->mode = config->mode;
    this->input_Method = config->ime;
    // this->input_Method = KB_METHOD_PINYIN;
    this->img_array = config->img_array;
    this->num_pic = config->num_pic;
    this->file_mark[0] = config->file_mark[0];
    this->file_mark[1] = config->file_mark[1];
    this->scale = gui_get_screen_width() / 640.f;

    img_data = get_other_img(this, KB_IMG_IDX_BG);
    this->img_bg = gui_img_create_from_mem(this, "KB_BG",
                                           img_data,
                                           0, 0, w, h);
    gui_img_set_mode(this->img_bg, IMG_BYPASS_MODE);
    gui_img_scale(this->img_bg, this->scale, gui_get_screen_height() / 320.f);
    gui_img_set_tp_block(this->img_bg, true);

    //
    if (this->layout == KB_LAYOUT_BASIC)
    {
        if (this->input_Method == KB_METHOD_PINYIN)
        {
            this->ime = gui_pinyin_IME_create((uint16_t)(10 * this->scale));
#define PATH_PRE "app/system/resource/ime/"
            gui_IME_set_dict(this->ime,  PATH_PRE"dictionary_info.json", PATH_PRE"dict8105.txt");
        }

        gui_kb_layout_basic_ctor(this, config);
    }



    img_data = get_other_img(this, KB_IMG_IDX_INPUTBOX);
    this->img_box = gui_img_create_from_mem(this, "KB_BOX",
                                            img_data,
                                            19, 17, w, h);
    gui_img_set_mode(this->img_box, IMG_BYPASS_MODE);
    gui_img_scale(this->img_box, this->scale, 1.f);

    char *text = buff_txt_display;

    this->txt_display = gui_text_create(this, "icon_text", 24, 26,
                                        w, h - 60);
    gui_text_mode_set(this->txt_display, LEFT);
    {
        int font_size = 26;
        gui_text_set(this->txt_display, text, GUI_FONT_SRC_BMP, gui_rgb(UINT8_MAX, UINT8_MAX, UINT8_MAX),
                     strlen(text), font_size);
        void *addr1 = gui_get_file_address("app/system/resource/font/simsun_size26_bits1_font.bin");
        gui_font_mem_init(addr1);
        gui_text_type_set(this->txt_display, addr1, FONT_SRC_MEMADDR);
    }
    gui_obj_show(this, false);
}
/*============================================================================*
 *                           Public Functions
 *============================================================================*/

void gui_keyboard_set_mode(gui_kb_t *this, uint8_t mode)
{
    // this->mode = mode;
    if (mode != this->mode)
    {
        gui_kb_mode_switch(this, mode);
    }
}

char *gui_kb_get_pswd(void)
{
    return buff_pswd;
}
void gui_kb_set_pswd(char *pswd, uint8_t pswd_len)
{
    if (pswd)
    {
        uint8_t len = (pswd_len < (KB_PSWD_KEY_MAXLEN - 1)) ? pswd_len : (KB_PSWD_KEY_MAXLEN - 1);
        memset(buff_pswd, 0, KB_PSWD_KEY_MAXLEN);
        memcpy(buff_pswd, pswd, len);
    }
}

void gui_keyboard_launch(gui_obj_t *obj)
{
    gui_kb_t *this = global_kb;
    if (this && ((gui_text_t *)obj)->inputable)
    {
        this->txt_input = (gui_text_t *)obj;
        this->ispasswd = this->txt_input->ispasswd;

        memset(buff_display, 0, KB_BUFF_DISPLAY_MAXLEN);
        len_display = 0;
        if (this->ispasswd)
        {
            len_display = strlen(buff_pswd);
            memcpy(buff_display, buff_pswd, len_display);
        }
        else
        {
            len_display = this->txt_input->len;
            memcpy(buff_display, this->txt_input->content, len_display);
        }

        gui_kb_cursor_set_tail();
        gui_kb_update_display();

        gui_text_set_animate(this->txt_display,
                             500,
                             -1,
                             gui_kb_cursor_animate,
                             gui_kb_get_cursor());

        gui_keyboard_set_mode(this, KB_MODE_BASIC_ENG_LOWWER);
        gui_obj_show(this, true);
    }
    else
    {
        gui_log("Keyboard not found !\n");
    }
}

void gui_keyboard_launch_by_widget(void *obj, gui_event_t e, void *param)
{
    gui_keyboard_launch((gui_obj_t *)param);
}


gui_kb_t *gui_keyboard_create(void                    *parent,
                              const char                *name,
                              gui_kb_config_t           *cofig,
                              int16_t                    x,
                              int16_t                    y,
                              int16_t                    w,
                              int16_t                    h)
{
    gui_kb_t *this;

    GUI_ASSERT(parent != NULL);
    if (name == NULL)
    {
        name = "DEFAULT_GLRY";
    }

    this = gui_malloc(sizeof(gui_kb_t));
    GUI_ASSERT(this != NULL);
    memset(this, 0x00, sizeof(gui_kb_t));

    gui_kb_ctor(this, (gui_obj_t *)parent, name, cofig, x, y, w, h);

    GET_BASE(this)->create_done = true;
    return this;
}

/** End of WIDGET_Exported_Functions
  * @}
  */

/** End of WIDGET
  * @}
  */
